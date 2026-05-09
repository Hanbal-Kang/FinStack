#pragma once
#include <QString>
#include <QDateTime>

class SavingsGoal {
private:
    int savings_goal_id;
    int user_id;
    double target_amount;
    double saved_amount; // Added to track progress
    QDateTime deadline;
    QString description;

public:
    SavingsGoal();

    // Getters
    int get_savings_goal_id() const;
    int get_user_id() const;
    double get_target_amount() const;
    double get_saved_amount() const;
    QDateTime get_deadline() const;
    QString get_description() const;

    // Setters
    void set_savings_goal_id(int id);
    void set_user_id(int id);
    void set_target_amount(double amount);
    void set_saved_amount(double amount);
    void set_deadline(const QDateTime& deadline);
    void set_description(const QString& description);

    bool isValid() const;
};