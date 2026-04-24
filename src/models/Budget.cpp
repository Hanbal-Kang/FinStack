#include "models/Budget.h"

Budget::Budget()
{
    Budget_id = 0;
    user_id =0;

    //Keeping empty by default
    category = "";

    monthly_limit = 0.0;
}

//Getters

int Budget::get_id() const
{
    return this->Budget_id;
}

int Budget::get_user_id() const
{
    return this->user_id;
}

QString Budget::get_category() const
{
    return this->category;
}

double Budget::get_monthly_limit() const
{
    return this->monthly_limit;
}

//Setters

void Budget::set_id(int id)
{
    this->Budget_id = id;
}

void Budget::set_user_id(int id)
{
    this->user_id = id;
}

void Budget::set_category(const QString& category)
{
    this->category = category;
}

void Budget::set_monthly_limit(double amount)
{
    this->monthly_limit = amount;
}

//Validty
bool Budget::isValid() const
{
    return (this->Budget_id > 0 && monthly_limit > 0.0);
}