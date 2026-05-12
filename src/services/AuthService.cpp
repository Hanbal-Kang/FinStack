#include "services/AuthService.h"
#include "database/DatabaseManager.h"
#include <QRandomGenerator>

#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>
#include <QCryptographicHash>

QString AuthService::hashPassword(const QString& password) {
    QByteArray hash = QCryptographicHash::hash(
        password.toUtf8(),
        QCryptographicHash::Sha256
        );
    return QString(hash.toHex());
}

bool AuthService::registerUser(const QString& username,
                               const QString& password,
                               const QString& email) {
    // 1. Insert the user row
    QSqlQuery q = DatabaseManager::instance().prepare(
        "INSERT INTO users(username, password_hash, email, acc_creation_date) "
        "VALUES (?, ?, ?, ?)"
        );

    q.addBindValue(username);
    q.addBindValue(hashPassword(password));
    q.addBindValue(email);
    q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    if (!DatabaseManager::instance().executePrepared(q))
        return false;

    // 2. Grab the new user's id (auto-incremented by SQLite).
    int newUserId = q.lastInsertId().toInt();
    if (newUserId <= 0)
        return false;   // shouldn't happen

    // 3. Create a default "Main" account for the user.
    //  Without this, transactions can never be saved (they reference account_id).
    QSqlQuery acc = DatabaseManager::instance().prepare(
        "INSERT INTO accounts(user_id, name) VALUES (?, ?)"
        );
    acc.addBindValue(newUserId);
    acc.addBindValue("Main");

    return DatabaseManager::instance().executePrepared(acc);
}

// Bool-only overload — kept for backwards compatibility / quick checks
bool AuthService::login(const QString& username, const QString& password) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT password_hash FROM users WHERE username = ?"
        );

    q.addBindValue(username);

    if (!DatabaseManager::instance().executePrepared(q) || !q.next())
        return false;

    QString storedHash = q.value(0).toString();
    return storedHash == hashPassword(password);
}

// Full overload — verifies password AND fills `outUser` on success.
// LoginWindow uses this so it can pass a real User to Dashboard.
bool AuthService::login(const QString& username, const QString& password, User& outUser) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT id, username, password_hash, acc_creation_date "
        "FROM users WHERE username = ?"
        );

    q.addBindValue(username);

    if (!DatabaseManager::instance().executePrepared(q) || !q.next())
        return false;

    QString storedHash = q.value(2).toString();
    if (storedHash != hashPassword(password))
        return false;

    outUser.set_id(q.value(0).toInt());
    outUser.set_username(q.value(1).toString());
    outUser.set_passwordHash(storedHash);
    outUser.set_acc_creation_date(QDateTime::fromString(q.value(3).toString(), Qt::ISODate));

    return true;
}

User AuthService::getUserByUsername(const QString& username) {
    User user;   // default-constructed: id == 0, isValid() == false

    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT id, username, password_hash, acc_creation_date "
        "FROM users WHERE username = ?"
        );

    q.addBindValue(username);

    if (!DatabaseManager::instance().executePrepared(q) || !q.next())
        return user;   // not found → invalid User

    user.set_id(q.value(0).toInt());
    user.set_username(q.value(1).toString());
    user.set_passwordHash(q.value(2).toString());
    user.set_acc_creation_date(QDateTime::fromString(q.value(3).toString(), Qt::ISODate));

    return user;
}

//Helpers to Generate the hash For recovery code
//Generates a recovery code in format XXXX-XXXX-XXXX-XXXX (16 chars + 3 dashes).
QString AuthService::makeRandomCode()
{
    static const QString alphabet = "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";   // 32 chars

    QString code;
    for (int group = 0; group < 4; group++) {
        if (group > 0) code += '-';
        for (int i = 0; i < 4; i++) {
            int idx = QRandomGenerator::global()->bounded(alphabet.length());
            code += alphabet.at(idx);
        }
    }
    return code;
}

QString AuthService::generateRecoveryCode(const QString& username)
{
    QString plaintext = makeRandomCode();
    QString hashed    = hashPassword(plaintext);   // reuse SHA-256 helper

    QSqlQuery q = DatabaseManager::instance().prepare(
        "UPDATE users SET recovery_code_hash = ? WHERE username = ?"
        );
    q.addBindValue(hashed);
    q.addBindValue(username);

    if (!DatabaseManager::instance().executePrepared(q))
        return QString();   // empty on failure

    return plaintext;
}

bool AuthService::changePassword(const QString& username,
                                 const QString& oldPassword,
                                 const QString& newPassword)
{
    // Verify old password first
    QSqlQuery readQ = DatabaseManager::instance().prepare(
        "SELECT password_hash FROM users WHERE username = ?"
        );
    readQ.addBindValue(username);
    if (!DatabaseManager::instance().executePrepared(readQ) || !readQ.next())
        return false;

    if (readQ.value(0).toString() != hashPassword(oldPassword))
        return false;

    // Update
    QSqlQuery updQ = DatabaseManager::instance().prepare(
        "UPDATE users SET password_hash = ? WHERE username = ?"
        );
    updQ.addBindValue(hashPassword(newPassword));
    updQ.addBindValue(username);

    return DatabaseManager::instance().executePrepared(updQ);
}

bool AuthService::resetPasswordWithRecoveryCode(const QString& username,
                                                const QString& recoveryCode,
                                                const QString& newPassword,
                                                QString& outNewCode)
{
    // Find the user and check the recovery code
    QSqlQuery readQ = DatabaseManager::instance().prepare(
        "SELECT recovery_code_hash FROM users WHERE username = ?"
        );
    readQ.addBindValue(username);
    if (!DatabaseManager::instance().executePrepared(readQ) || !readQ.next())
        return false;

    QString storedHash = readQ.value(0).toString();
    if (storedHash.isEmpty() || storedHash != hashPassword(recoveryCode))
        return false;

    // Code matches — update password AND rotate the recovery code so the
    // old one can't be reused.
    QString newCodePlain  = makeRandomCode();
    QString newCodeHashed = hashPassword(newCodePlain);

    QSqlQuery updQ = DatabaseManager::instance().prepare(
        "UPDATE users SET password_hash = ?, recovery_code_hash = ? WHERE username = ?"
        );
    updQ.addBindValue(hashPassword(newPassword));
    updQ.addBindValue(newCodeHashed);
    updQ.addBindValue(username);

    if (!DatabaseManager::instance().executePrepared(updQ))
        return false;

    outNewCode = newCodePlain;   // give the new code back to the caller
    return true;
}