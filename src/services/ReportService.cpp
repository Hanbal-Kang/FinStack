#include "services/ReportService.h"
#include "database/DatabaseManager.h"

#include <QSqlQuery>
#include <QVariant>


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