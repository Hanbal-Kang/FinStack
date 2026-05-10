#pragma once
#include<QString>
#include<QDateTime>

class Transaction
{
public:
    enum Type {Income, Expense};
private:
    Type type;

    int transaction_id;

    //the id of the user which is unique
    int account_id;

    //Amount of the transaction
    double amount;

    //Category of transaction   (e.g food,transport,entertainment)
    QString category;

    //Description of Transaction
    QString description;

    QDateTime transac_date;

public:
    Transaction();

    //Getters
    Type get_type()        const;
    int get_transaction_id() const;
    int get_account_id()     const;
    double get_amount()      const;
    QString get_category()    const;
    QString  get_description() const;
    QDateTime get_transac_date() const;

    //Setters
    void set_type(Type type);
    void set_transaction_id(int id);
    void set_account_id(int user_id);
    void set_amount(double amount);
    void set_category(const QString& category);
    void set_description(const QString& description);
    void set_transac_date(const QDateTime& date);

    //Validity
    bool isValid() const;
};