#pragma once
#include <QString>

class Budget
{

public:

    //CHANGE : Introduced enum for controlled categories(must come before private)
    enum Category {
        Food = 1,
        Transport = 2,
        Rent = 3,
        Utilities = 4,
        Entertainment = 5,
        Other = 6
    };


    // CHANGE: added static helper to convert enum to display string
    static QString categoryToString(Category c) {
        switch (c) {
        case Food:          return "Food";
        case Transport:     return "Transport";
        case Rent:          return "Rent";
        case Utilities:     return "Utilities";
        case Entertainment: return "Entertainment";
        default:            return "Other";
        }
    }

    // CHANGE: added reverse helper to convert string to enum (for transaction matching)
    static Category categoryFromString(const QString& s) {
        if (s == "Food")          return Food;
        if (s == "Transport")     return Transport;
        if (s == "Rent")          return Rent;
        if (s == "Utilities")     return Utilities;
        if (s == "Entertainment") return Entertainment;
        return Other;
    }

private:
    int Budget_id;
    int user_id;

    //Category for which we need to set a budget (e.g Food ,transport etc)
    Category category;

    //The budget amount set for that specific category
    double monthly_limit;
public:
    Budget();

    //Getters
    int get_id() const;
    int get_user_id() const;
    Category get_category() const;
    double get_monthly_limit() const;

    //Setters
    void set_id(int id);
    void set_user_id(int id);
    void set_category(Category category);
    void set_monthly_limit(double amount);

    //Check the validty
    bool isValid() const;
};
