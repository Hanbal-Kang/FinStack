#include "models/RecurringTransaction.h"

RecurringTransaction::RecurringTransaction()
{
    reccur_transac_id = 0;
    user_id = 0;
    type = Expense;     //default: most recurrences are expenses (rent, subscriptions etc)
    amount = 0.0;
    category = "";
    description = "";
    interval = "";
    is_active = true;   //new rules are active by default
    //start_date and next_due are handled by QDateTime by default, it sets them to invalid
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
RecurringTransaction::Type RecurringTransaction::get_type() const
{
    return this->type;
}
double RecurringTransaction::get_amount() const
{
    return this->amount;
}
QString RecurringTransaction::get_category() const
{
    return this->category;
}
QString RecurringTransaction::get_description() const
{
    return this->description;
}
QString RecurringTransaction::get_interval() const
{
    return this->interval;
}
QDateTime RecurringTransaction::get_start_date() const
{
    return this->start_date;
}
QDateTime RecurringTransaction::get_next_due() const
{
    return this->next_due;
}
bool RecurringTransaction::get_is_active() const
{
    return this->is_active;
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
void RecurringTransaction::set_type(Type type)
{
    this->type = type;
}
void RecurringTransaction::set_amount(double amount)
{
    this->amount = amount;
}
void RecurringTransaction::set_category(const QString& category)
{
    this->category = category;
}
void RecurringTransaction::set_description(const QString& description)
{
    this->description = description;
}
void RecurringTransaction::set_interval(const QString& interval)
{
    this->interval = interval;
}
void RecurringTransaction::set_start_date(const QDateTime& start_date)
{
    this->start_date = start_date;
}
void RecurringTransaction::set_next_due(const QDateTime& next_due)
{
    this->next_due = next_due;
}
void RecurringTransaction::set_is_active(bool active)
{
    this->is_active = active;
}

//To Check the Validity
bool RecurringTransaction::isValid() const
{
    return (this->reccur_transac_id > 0 && this->amount > 0.0 &&
            !this->category.isEmpty() && !this->description.isEmpty());
}