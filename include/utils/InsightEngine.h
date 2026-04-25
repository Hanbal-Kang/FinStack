#pragma once

#include <QString>
#include <QList>
#include "models/Transaction.h"

class InsightEngine
{
    //Stateless, no private

public:
    //Only static member functions so they can be called directly to Transaction
    static QString topSpendingCategory(const QList<Transaction>& transactions);
    static double  spendingPercentage(const QString& category, const QList<Transaction>& transactions);

    //Creates our insights
    static QString generateInsight(const QList<Transaction>& transactions);
};
