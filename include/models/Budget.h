#pragma once
#include <QString>

class Budget
{
private:
    int Budget_id;
    int user_id;

    //Category for which we need to set a budget (e.g Food ,transport etc)
    QString category;

    //The budget amount set for that specific category
    double monthly_limit;
public:
    Budget();

    //Getters
    int get_id() const;
    int get_user_id() const;
    QString get_category() const;
    double get_monthly_limit() const;

    //Setters
    void set_id(int id);
    void set_user_id(int id);
    void set_category(const QString& category);
    void set_monthly_limit(double amount);

    //Check the validty
    bool isValid() const;
};
