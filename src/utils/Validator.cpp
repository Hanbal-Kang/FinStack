#include "utils/Validator.h"

bool Validator::isValidAmount(const QString& amount)
{
    bool ok;

    //if string passes "abc" so it fails to convert to double and changes bool ok to false
    double value = amount.toDouble(&ok);

    return (ok && value > 0.0);
}

bool Validator::isValidUsername(const QString& username)
{
    // Minimum length for username should be 3 characters
    return (!username.isEmpty() && username.length() >= 3);
}

bool Validator::isPasswordStrong(const QString& password)
{
    if(password.length() < 8)
        return false;

    //To keep check that the password has atleast one symbol, on digit and one uppercase letter
    bool hasUpper   = false;
    bool hasDigit   = false;
    bool hasSymbol  = false;
    for (QChar c : password)
    {
        if(c.isUpper())
        hasUpper  = true;

        if(c.isDigit())
        hasDigit  = true;

        if(c.isPunct() || c.isSymbol())
        hasSymbol = true;
    }

    return hasUpper && hasDigit && hasSymbol;
}