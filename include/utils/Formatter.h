#pragma once
#include <QString>
#include <QDateTime>

class Formatter
{
    //Stateless no members. Will only be callled by any window
public:

    //Static methods will be directly called in windows, no object needed
    static QString formatCurrency(double amount);
    static QString formatDate(const QDateTime& date);
    static QString shortDate(const QDateTime& date);
};