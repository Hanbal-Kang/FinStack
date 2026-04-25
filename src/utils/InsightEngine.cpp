#include "utils/InsightEngine.h"
#include <QMap>

QString InsightEngine::topSpendingCategory(const QList<Transaction>& transactions)
{
    //QMap is a class container like a sorted dictionary (evvery key has a value)
    //Here using QString is key of it as category AND double amount (spent on that category)
    QMap<QString, double> categoryTotals;

    for (const Transaction& t : transactions) {

        //if money is spent (called as Expense)

        if(t.get_type() == Transaction::Expense)
        {
            //Inserting the QString category and double amount
            categoryTotals[t.get_category()] += t.get_amount();
        }
    }

    if(categoryTotals.isEmpty())
        return "None";

    //Our top category
    QString topCategory;
    double topAmount = 0.0;

    for (auto i = categoryTotals.begin(); i != categoryTotals.end(); ++i) {

        if(i.value() > topAmount)
        {
            topAmount = i.value();
            topCategory = i.key();
        }
    }

    return topCategory;
}

double InsightEngine::spendingPercentage(const QString& category, const QList<Transaction>& transactions)
{

    double categoryTotal = 0.0;
    double expenseTotal = 0.0;

    for (const Transaction& t: transactions) {

        if(t.get_type() == Transaction::Expense)
        {
            //So here our total expense gets calculated including all categories
            expenseTotal += t.get_amount();

            //total expense for specific category
            if(t.get_category() == category)
            {
                categoryTotal += t.get_amount();
            }
        }
    }

    if(expenseTotal == 0.0)
        return 0.0;

    return ( (categoryTotal / expenseTotal) * 100);
}

//Creates human readable Insights based on specific results
QString InsightEngine::generateInsight(const QList<Transaction>& transactions)
{
    if(transactions.isEmpty())
        return "No Transactions yet. Start Tracking Your Expenses.";

    //We ll get our top spending category
    QString topCatog = topSpendingCategory(transactions);
    double percentage = spendingPercentage(topCatog, transactions);

    if(topCatog == "None")
        return "No Expenses Record Yet.";

    //Message at 50% spending or more
    if(percentage >= 50)
    {
        return ( "You are Spending " + QString::number(percentage , 'f' , 1) + "% of Your Budget on " + topCatog + ". Consider Setting up a Catogery Limit");
    }

    else if(percentage >= 30)
    {
        return ( topCatog + " is Your Top Spending Category at " + QString::number(percentage, 'f' , 1) + "%. Keep an Eye on it.");
    }

    else
    {
        return ( "Your Spending Looks Balanced. Your Top Spending Category " + topCatog + " is at " + QString::number(percentage, 'f' , 1) + "%.");
    }

}