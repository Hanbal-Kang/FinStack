#pragma once
#include <QString>

class Validator
{
    //Stateless, No private members

public:

    //Making the member fucntion static so It can be called directly by any window without any object of it.
    static bool isValidAmount(const QString& amount);
    static bool isValidUsername(const QString& username);
    static bool isPasswordStrong(const QString& password);
};