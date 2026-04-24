#include "models/Transaction.h"

Transaction::Transaction()
{
    //Setting default type to income
    type = Income;

    transaction_id = 0;

    //No user will have user_id = 0
    user_id = 0;

    amount = 0.0;

    category = "";

    description = "";

    //We ll have an invalid empty date

    //QDateTime will automatically intialize date to an invalid date
}

//Getters
Transaction::Type Transaction::get_type() const
{
    return this->type;
}

int Transaction::get_transaction_id() const
{
    return this->transaction_id;
}

int Transaction::get_user_id() const
{
    return this->user_id;
}

double Transaction::get_amount() const
{
    return this->amount;
}

QString Transaction::get_category() const
{
    return this->category;
}

QString Transaction::get_description() const
{
    return this->description;
}

QDateTime Transaction::get_transac_date() const
{
    return this->transac_date;
}

//Setters
void Transaction::set_type(Transaction::Type t)
{
    this->type = t;
}

void Transaction::set_transaction_id(int id)
{
    this->transaction_id = id;
}

void Transaction::set_user_id(int id)
{
    this->user_id = id;
}

void Transaction::set_amount(double amount)
{
    this->amount = amount;
}

void Transaction::set_category(const QString& category)
{
    this->category = category;
}

void Transaction::set_description(const QString& descrip)
{
    this->description = descrip;
}

void Transaction::set_transac_date(const QDateTime& transac_date)
{
    this->transac_date = transac_date;
}

bool Transaction::isValid() const
{
    //So if any of these is false no transaction will happen
    return (this->transaction_id > 0 && this->amount > 0.0);
}