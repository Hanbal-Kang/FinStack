#pragma once

#include <QString>

class AuthService {
public:
    bool registerUser(const QString& username, const QString& password, const QString& email);
    bool login(const QString& username, const QString& password);

private:
    QString hashPassword(const QString& password);
};