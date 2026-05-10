#include "services/ReportService.h"
#include "database/DatabaseManager.h"
#include "services/TransactionService.h"
#include "services/BudgetService.h"
#include "services/GoalService.h"
#include "models/Transaction.h"
#include <QFile>
#include <QTextStream>

#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>


double ReportService::getTotalIncome(int userId) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT SUM(amount) FROM transactions "
        "WHERE type = 'income' AND account_id IN "
        "(SELECT id FROM accounts WHERE user_id = ?)"
        );

    q.addBindValue(userId);
    q.exec();

    if (q.next())
        return q.value(0).toDouble();

    return 0.0;
}


double ReportService::getTotalExpense(int userId) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT SUM(amount) FROM transactions "
        "WHERE type = 'expense' AND account_id IN "
        "(SELECT id FROM accounts WHERE user_id = ?)"
        );

    q.addBindValue(userId);
    q.exec();

    if (q.next())
        return q.value(0).toDouble();

    return 0.0;
}

double ReportService::getBalance(int userId) {
    return getTotalIncome(userId) - getTotalExpense(userId);
}

// Build a single CSV file containing the user's full financial picture: (sort of Financial report | if user wants to use it in excel)
// - Summary block (income, expenses, balance)
// - All transactions
// - All budgets
// - All savings goals
// Excel / Google Sheets / Numbers can all open this directly.
bool ReportService::exportTransactionsCSV(int userId, const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);

    //Summary section
    double income  = getTotalIncome(userId);
    double expense = getTotalExpense(userId);
    double balance = income - expense;

    out << "FinStack Financial Report\n";
    out << "Generated:," << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n\n";
    out << "Summary\n";
    out << "Total Income," << income << "\n";
    out << "Total Expense," << expense << "\n";
    out << "Net Balance," << balance << "\n\n";

    //Transactions section
    out << "Transactions\n";
    out << "Date,Type,Category,Amount,Description\n";

    TransactionService txSvc;
    std::vector<Transaction> txs = txSvc.getAllByUser(userId);
    for (const auto& tx : txs) {
        QString type = (tx.get_type() == Transaction::Income) ? "Income" : "Expense";
        // Wrap description in quotes and escape any embedded quotes — CSV safety
        QString desc = tx.get_description();
        desc.replace("\"", "\"\"");

        out << tx.get_transac_date().toString("yyyy-MM-dd") << ","
            << type << ","
            << tx.get_category() << ","
            << tx.get_amount() << ","
            << "\"" << desc << "\"\n";
    }
    out << "\n";

    //Budgets section
    out << "Budgets\n";
    out << "Category,Monthly Limit\n";

    BudgetService budSvc;
    std::vector<Budget> bgs = budSvc.getUserBudgets(userId);
    for (const auto& b : bgs) {
        out << Budget::categoryToString(b.get_category()) << ","
            << b.get_monthly_limit() << "\n";
    }
    out << "\n";

    //Savings goals section
    out << "Savings Goals\n";
    out << "Description,Target,Saved,Deadline\n";

    GoalService goalSvc;
    std::vector<SavingsGoal> goals = goalSvc.getGoalsForUser(userId);
    for (const auto& g : goals) {
        out << "\"" << g.get_description() << "\","
            << g.get_target_amount() << ","
            << g.get_saved_amount() << ","
            << g.get_deadline().toString("yyyy-MM-dd") << "\n";
    }

    file.close();
    return true;
}