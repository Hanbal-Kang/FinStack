#include "models/RecurringTransaction.h"

RecurringTransaction::RecurringTransaction()
{
    reccur_transac_id = 0;
    user_id = 0;

    amount = 0.0;

    category = "";
    description = "";
    interval = "";

    //next_due is handled by QDateTime by default, it sets it to invalid
}

//Getters
int RecurringTransaction::get_reccur_transac_id() const
{
    return this->reccur_transac_id;
}

int RecurringTransaction::get_user_id() const
{
    return this->user_id;
}

double RecurringTransaction::get_amount() const
{
    return this->amount;
}

QString RecurringTransaction::get_category() const
{
    return this->category;
}

QString RecurringTransaction::get_interval() const
{
    return this->interval;
}

QDateTime RecurringTransaction::get_next_due() const
{
    return this->next_due;
}

//Setters
void RecurringTransaction::set_reccur_transac_id(int id)
{
    this->reccur_transac_id = id;
}

void RecurringTransaction::set_user_id(int id)
{
    this->user_id = id;
}

void RecurringTransaction::set_amount(double amount)
{
    this->amount = amount;
}

void RecurringTransaction::set_category(const QString& category)
{
    this->category = category;
}

void RecurringTransaction::set_interval(const QString& interval)
{
    this->interval = interval;
}

void RecurringTransaction::set_next_due(const QDateTime& next_due)
{
    this->next_due = next_due;
}

//To Check the Validity
bool RecurringTransaction::isValid() const
{
    return (this->reccur_transac_id > 0 && this->amount > 0.0);
}
