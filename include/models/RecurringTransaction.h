#pragma once
#include <QString>
#include <QDateTime>

class RecurringTransaction
{
private:
    int reccur_transac_id;
    int user_id;

    double amount;

    //the category (reason) for which this money will be deducted (e.g Rent)
    QString category;

    //description or notes about it(transaction), but still is OPTIONAL
    QString description;

    //e.g (daily, weekly, monthly etc)
    QString interval;

    //When it should deduct next amount
    QDateTime next_due;
public:
    RecurringTransaction();

    //Getters
    int get_reccur_transac_id() const;
    int get_user_id() const;
    double get_amount() const;
    QString get_category() const;
    QString get_description() const;
    QString get_interval () const;
    QDateTime get_next_due() const;
    QString get_description() const;

    //Setters
    void set_reccur_transac_id(int id);
    void set_user_id(int id);
    void set_amount(double amount);
    void set_category(const QString& category);
    void set_description(const QString& description);
    void set_interval(const QString& interval);
    void set_next_due(const QDateTime& next_due);

    //Validty
    bool isValid() const;
};
