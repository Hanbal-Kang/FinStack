#pragma once

#include <QString>
#include "models/User.h"

class AuthService {
public:
    bool registerUser(const QString& username, const QString& password, const QString& email);

    // login overloads:
    //   bool-only version: returns true if credentials match
    //   out-param version: also fills `outUser` with the full User from DB on success
    bool login(const QString& username, const QString& password);
    bool login(const QString& username, const QString& password, User& outUser);

    // Fetches a user by username. Returns a User with id==0 if not found
    // (User::isValid() will return false in that case).
    User getUserByUsername(const QString& username);

private:
    QString hashPassword(const QString& password);
};