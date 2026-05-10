#pragma once
#include <QString>
#include <QDateTime>

class RecurringTransaction
{
public:
    enum Type { Income, Expense };

private:
    int     reccur_transac_id;
    int     user_id;
    Type    type;                  // Income or Expense

    double  amount;
    QString category;              // e.g. Rent, Food, Other
    QString description;           // title shown on UI, e.g. Monthly Stipend
    QString interval;              // daily, weekly, monthly

    QDateTime start_date;          // when this rule was created
    QDateTime next_due;            // when the next occurrence should fire
    bool      is_active;           // true = will auto-fire; false = paused

public:
    RecurringTransaction();

    //Getters
    int       get_reccur_transac_id() const;
    int       get_user_id()           const;
    Type      get_type()              const;
    double    get_amount()            const;
    QString   get_category()          const;
    QString   get_description()       const;
    QString   get_interval()          const;
    QDateTime get_start_date()        const;
    QDateTime get_next_due()          const;
    bool      get_is_active()         const;

    //Setters
    void set_reccur_transac_id(int id);
    void set_user_id(int id);
    void set_type(Type type);
    void set_amount(double amount);
    void set_category(const QString& category);
    void set_description(const QString& description);
    void set_interval(const QString& interval);
    void set_start_date(const QDateTime& date);
    void set_next_due(const QDateTime& next_due);
    void set_is_active(bool active);

    //Validity
    bool isValid() const;
};