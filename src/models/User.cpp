#include "models/User.h"

User::User()
{
    this->id = 0;
}

//Getters
int User::get_id() const
{
    return id;
}
QString User::get_username() const
{
    return username;
}
QString User::get_password() const
{
    return password;
}
QDateTime User::get_acc_creation_date() const
{
    return acc_creation_date;
}

//Setters
void User::set_id(int id)
{
    this->id = id;
}
void User::set_username(const QString& username)
{
    this->username = username;
}
void User::set_password(const QString& password)
{
    this->password = password;
}

void User::set_acc_creation_date(const QDateTime& date)
{
    this->acc_creation_date = date;
}


bool User::isValid() const
{
    return ((id > 0) && (!username.isEmpty()));
}
