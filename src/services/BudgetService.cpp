#include "services/BudgetService.h"
#include "database/DatabaseManager.h"

#include <QSqlQuery>
#include <QVariant>
#include <vector>

bool BudgetService::createBudget(const Budget& budget) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "INSERT INTO budgets(user_id, category_id, monthly_limit) VALUES (?, ?, ?)"
        );

    q.addBindValue(budget.get_user_id());
    q.addBindValue(budget.get_category()); // should ideally be category_id
    q.addBindValue(budget.get_monthly_limit());

    return q.exec();
}

std::vector<Budget> BudgetService::getUserBudgets(int userId) {
    std::vector<Budget> list;

    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT id, user_id, category_id, monthly_limit FROM budgets WHERE user_id = ?"
        );

    q.addBindValue(userId);
    q.exec();

    while (q.next()) {
        Budget b;
        b.set_id(q.value(0).toInt());
        b.set_user_id(q.value(1).toInt());
        b.set_category(QString::number(q.value(2).toInt()));
        b.set_monthly_limit(q.value(3).toDouble());

        list.push_back(b);
    }

    return list;
}

bool BudgetService::updateBudget(int id, double newLimit) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "UPDATE budgets SET monthly_limit = ? WHERE id = ?"
        );

    q.addBindValue(newLimit);
    q.addBindValue(id);

    return q.exec();
}

bool BudgetService::deleteBudget(int id) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "DELETE FROM budgets WHERE id = ?"
        );

    q.addBindValue(id);

    return q.exec();
}