#pragma once
#include <QString>
#include <QDateTime>

class SavingsGoal
{
private:

    int savings_goal_id;
    int user_id;

    //The amount which the user will set as goal to save
    double target_amount;

    //The date which the user will as set as till when he has to complete this saving goal
    QDateTime deadline;

    //Note like for what he is saving this(e.g Buy new Laptop) (Optional)
    QString description;
public:

    SavingsGoal();

    //Getters
    int get_savings_goal_id() const;
    int get_user_id() const;
    double get_target_amount() const;
    QDateTime get_deadline() const;
    QString get_description() const;

    //Setters
    void set_savings_goal_id(int id);
    void set_user_id(int id);
    void set_target_amount(double amount);
    void set_deadline(const QDateTime& deadline);
    void set_description(const QString& description);

    //Validty
    bool isValid() const;
};
