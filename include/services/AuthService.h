#pragma once

#include <QString>
#include "models/User.h"

class AuthService {
public:
    bool registerUser(const QString& username, const QString& password, const QString& email);

    // After a successful registerUser, call this to generate + store a recovery
    QString generateRecoveryCode(const QString& username);

    bool login(const QString& username, const QString& password);
    bool login(const QString& username, const QString& password, User& outUser);

    // Verifies oldPassword for `username`, then sets the password to newPassword.
    // Returns false if the user doesn't exist OR if oldPassword is wrong.
    bool changePassword(const QString& username, const QString& oldPassword, const QString& newPassword);

    // Resets a forgotten password using the recovery code. Returns false if
    // username is unknown OR the code doesn't match. On success, also rotates
    // the recovery code (caller gets the new one back via outNewCode) so a
    // leaked code can't be reused.
    bool resetPasswordWithRecoveryCode(const QString& username,
                                       const QString& recoveryCode,
                                       const QString& newPassword,
                                       QString& outNewCode);

    User getUserByUsername(const QString& username);

private:
    QString hashPassword(const QString& password);
    QString makeRandomCode();   // generates the XXXX-XXXX-XXXX-XXXX recovery code
};