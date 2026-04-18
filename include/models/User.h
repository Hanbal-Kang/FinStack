#pragma once
#include<QString>
#include<QDateTime>
class User
{
private:
    int id;
    QString username;
    QString password;
    QDateTime acc_creation_date;
public:
    User();

    //Getters
    int get_id() const;
    QString get_username() const;
    QString get_password() const;
    QDateTime get_acc_creation_date() const;

    //Setters
    void set_id(int id);
    void set_username(const QString& username);
    void set_password(const QString& password);
    void set_acc_creation_date(const QDateTime& date);

    //Checking Edge Cases for our id and name
    bool isValid() const;

};