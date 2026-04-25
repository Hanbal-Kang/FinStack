#include "services\AuthService.h"
#include "database\DatabaseManager.h"

#include <QSqlQuery>
#include <QVariant>
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
        "INSERT INTO users(username, password_hash, email) VALUES (?, ?, ?)"
        );

    q.addBindValue(username);
    q.addBindValue(hashPassword(password));
    q.addBindValue(email);

     return DatabaseManager::instance().executePrepared(q);
}

bool AuthService::login(const QString& username,
                        const QString& password) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT password_hash FROM users WHERE username = ?"
        );

    q.addBindValue(username);

    if (!DatabaseManager::instance().executePrepared(q) || !q.next())
        return false;

    QString storedHash = q.value(0).toString();
    return storedHash == hashPassword(password);
}