#include "services/TransactionService.h"
#include "database/DatabaseManager.h"

#include <QSqlQuery>
#include <QVariant>
#include <vector>

bool TransactionService::addTransaction(const Transaction& t) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "INSERT INTO transactions(account_id, type, amount, category, description, transaction_date) "
        "VALUES (?, ?, ?, ?, ?, ?)"
        );
    q.addBindValue(t.get_user_id());
    q.addBindValue(t.get_type() == Transaction::Income ? "income" : "expense");
    q.addBindValue(t.get_amount());
    q.addBindValue(t.get_category()); // already QString, no change
    q.addBindValue(t.get_description());
    q.addBindValue(t.get_transac_date());
    return DatabaseManager::instance().executePrepared(q);
}

std::vector<Transaction> TransactionService::getAllByUser(int userId) {
    std::vector<Transaction> list;
    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT id, account_id, type, amount, category, description, transaction_date "
        "FROM transactions "
        "WHERE account_id IN (SELECT id FROM accounts WHERE user_id = ?)"
        );
    q.addBindValue(userId);
    if (!DatabaseManager::instance().executePrepared(q)) return list;

    while (q.next()) {
        Transaction t;
        t.set_transaction_id(q.value(0).toInt());
        t.set_user_id(q.value(1).toInt());
        t.set_type(q.value(2).toString() == "income" ? Transaction::Income : Transaction::Expense);
        t.set_amount(q.value(3).toDouble());
        t.set_category(q.value(4).toString());
        t.set_description(q.value(5).toString());
        t.set_transac_date(q.value(6).toDateTime());
        list.push_back(t);
    }
    return list;
}

std::vector<Transaction> TransactionService::getByType(int userId, Transaction::Type type) {
    std::vector<Transaction> list;

    QString typeStr = (type == Transaction::Income) ? "income" : "expense";

    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT id, account_id, category_id, type, amount, description, transaction_date "
        "FROM transactions "
        "WHERE account_id IN (SELECT id FROM accounts WHERE user_id = ?) AND type = ?"
        );

    q.addBindValue(userId);
    q.addBindValue(typeStr);
    q.exec();

    while (q.next()) {
        Transaction t;
        t.set_transaction_id(q.value(0).toInt());
        t.set_user_id(q.value(1).toInt());
        t.set_category(q.value(2).toString());
        t.set_type(type);
        t.set_amount(q.value(4).toDouble());
        t.set_description(q.value(5).toString());
        t.set_transac_date(q.value(6).toDateTime());
        list.push_back(t);
    }

    return list;
}

bool TransactionService::deleteTransaction(int id) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "DELETE FROM transactions WHERE id = ?"
        );

    q.addBindValue(id);

    return q.exec();
}