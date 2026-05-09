#include "services/AuthService.h"
#include "database/DatabaseManager.h"

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
    QSqlQuery q = DatabaseManager::instance().prepare(
        "INSERT INTO users(username, password_hash, email, acc_creation_date) "
        "VALUES (?, ?, ?, ?)"
        );

    q.addBindValue(username);
    q.addBindValue(hashPassword(password));
    q.addBindValue(email);
    // ISO format string keeps round-tripping clean across SQLite text storage
    q.addBindValue(QDateTime::currentDateTime().toString(Qt::ISODate));

    return DatabaseManager::instance().executePrepared(q);
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