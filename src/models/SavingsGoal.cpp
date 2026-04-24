#include "models/SavingsGoal.h"

SavingsGoal::SavingsGoal()
{
    savings_goal_id = 0;
    user_id = 0;

    target_amount = 0.0;

    //#for    QDateTime deadline
    //Deadline does not need to be intialized here. QDateTime automatically sets it to invalid

    description = "";
}

//Getters
int SavingsGoal::get_savings_goal_id() const
{
    return this->savings_goal_id;
}

int SavingsGoal::get_user_id() const
{
    return this->user_id;
}

double SavingsGoal::get_target_amount() const
{
    return this->target_amount;
}

QDateTime SavingsGoal::get_deadline() const
{
    return this->deadline;
}

QString SavingsGoal::get_description() const
{
    return this->description;
}

//Setters
void SavingsGoal::set_savings_goal_id(int id)
{
    this->savings_goal_id = id;
}

void SavingsGoal::set_user_id(int id)
{
    this->user_id = id;
}

void SavingsGoal::set_target_amount(double amount)
{
    this->target_amount = amount;
}

void SavingsGoal::set_deadline(const QDateTime& deadline)
{
    this->deadline = deadline;
}

void SavingsGoal::set_description(const QString& description)
{
    this->description = description;
}

//Checking the Validity
bool SavingsGoal::isValid() const
{
    return (this->savings_goal_id > 0 && this->target_amount > 0.0);
}