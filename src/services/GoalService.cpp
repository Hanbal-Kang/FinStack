#include "services/GoalService.h"
#include "database/DatabaseManager.h"
#include <QSqlQuery>
#include <QVariant>

bool GoalService::createGoal(const SavingsGoal& goal) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "INSERT INTO savings_goals (user_id, description, target_amount, deadline, saved_amount) "
        "VALUES (?, ?, ?, ?, ?)"
        );
    q.addBindValue(goal.get_user_id());
    q.addBindValue(goal.get_description());
    q.addBindValue(goal.get_target_amount());
    q.addBindValue(goal.get_deadline().toString(Qt::ISODate));
    q.addBindValue(goal.get_saved_amount());

    return DatabaseManager::instance().executePrepared(q);
}

std::vector<SavingsGoal> GoalService::getGoalsForUser(int userId) {
    std::vector<SavingsGoal> goals;
    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT savings_goal_id, user_id, description, target_amount, deadline, saved_amount "
        "FROM savings_goals WHERE user_id = ?"
        );
    q.addBindValue(userId);

    if (DatabaseManager::instance().executePrepared(q)) {
        while (q.next()) {
            SavingsGoal g;
            g.set_savings_goal_id(q.value(0).toInt());
            g.set_user_id(q.value(1).toInt());
            g.set_description(q.value(2).toString());
            g.set_target_amount(q.value(3).toDouble());
            g.set_deadline(QDateTime::fromString(q.value(4).toString(), Qt::ISODate));
            g.set_saved_amount(q.value(5).toDouble());
            goals.push_back(g);
        }
    }
    return goals;
}

bool GoalService::deleteGoal(int goalId) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "DELETE FROM savings_goals WHERE savings_goal_id = ?"
        );
    q.addBindValue(goalId);
    return DatabaseManager::instance().executePrepared(q);
}

bool GoalService::updateGoalProgress(int goalId, double amount) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "UPDATE savings_goals SET saved_amount = ? WHERE savings_goal_id = ?"
        );
    q.addBindValue(amount);
    q.addBindValue(goalId);
    return DatabaseManager::instance().executePrepared(q);
}

//Adds amount to the goal's saved_amount
bool GoalService::contributeToGoal(int goalId, double amount)
{
    if (amount <= 0) return false;

    //Read current saved_amount
    QSqlQuery readQ = DatabaseManager::instance().prepare(
        "SELECT saved_amount FROM savings_goals WHERE savings_goal_id = ?"
        );
    readQ.addBindValue(goalId);

    if (!DatabaseManager::instance().executePrepared(readQ) || !readQ.next())
        return false;

    double current = readQ.value(0).toDouble();
    double newSaved = current + amount;

    //Write back the new total
    QSqlQuery writeQ = DatabaseManager::instance().prepare(
        "UPDATE savings_goals SET saved_amount = ? WHERE savings_goal_id = ?"
        );
    writeQ.addBindValue(newSaved);
    writeQ.addBindValue(goalId);

    return DatabaseManager::instance().executePrepared(writeQ);
}