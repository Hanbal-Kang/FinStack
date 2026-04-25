#include "services/AnalyticsService.h"
#include "database/DatabaseManager.h"

#include <QSqlQuery>
#include <QVariant>

double AnalyticsService::getTotalIncome(int userId) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT SUM(amount) FROM transactions "
        "WHERE type = 'income' AND account_id IN "
        "(SELECT id FROM accounts WHERE user_id = ?)"
        );

    q.addBindValue(userId);
    if (!DatabaseManager::instance().executePrepared(q) || !q.next())
        return 0.0;

     // null safety
    QVariant val = q.value(0);
    return val.isNull() ? 0.0 : val.toDouble();
}

double AnalyticsService::getTotalExpense(int userId) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT SUM(amount) FROM transactions "
        "WHERE type = 'expense' AND account_id IN "
        "(SELECT id FROM accounts WHERE user_id = ?)"
        );

    q.addBindValue(userId);

    if (!DatabaseManager::instance().executePrepared(q) || !q.next())
        return 0.0;

     // null safety
    QVariant val = q.value(0);
    return val.isNull() ? 0.0 : val.toDouble();
}

double AnalyticsService::getBalance(int userId) {
    return getTotalIncome(userId) - getTotalExpense(userId);
}

double AnalyticsService::getCategoryExpense(int userId, int categoryId) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT SUM(amount) FROM transactions "
        "WHERE type = 'expense' AND category_id = ? "
        "AND account_id IN (SELECT id FROM accounts WHERE user_id = ?)"
        );

    q.addBindValue(categoryId);
    q.addBindValue(userId);
    if (!DatabaseManager::instance().executePrepared(q) || !q.next())
        return 0.0;

    // null safety
    QVariant val = q.value(0);
    return val.isNull() ? 0.0 : val.toDouble();
}