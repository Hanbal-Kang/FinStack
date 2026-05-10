#include "services/RecurringTransactionService.h"
#include "services/TransactionService.h"
#include "database/DatabaseManager.h"
#include "models/Transaction.h"

#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>

bool RecurringTransactionService::createRecurring(const RecurringTransaction& r)
{
    QSqlQuery q = DatabaseManager::instance().prepare(
        "INSERT INTO recurring_transactions"
        "(user_id, type, amount, category, description, interval, "
        " start_date, next_due, is_active) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)"
        );

    q.addBindValue(r.get_user_id());
    q.addBindValue(r.get_type() == RecurringTransaction::Income ? "income" : "expense");
    q.addBindValue(r.get_amount());
    q.addBindValue(r.get_category());
    q.addBindValue(r.get_description());
    q.addBindValue(r.get_interval());
    q.addBindValue(r.get_start_date().toString(Qt::ISODate));
    q.addBindValue(r.get_next_due().toString(Qt::ISODate));
    q.addBindValue(r.get_is_active() ? 1 : 0);

    return DatabaseManager::instance().executePrepared(q);
}

std::vector<RecurringTransaction> RecurringTransactionService::getAllByUser(int userId)
{
    std::vector<RecurringTransaction> list;

    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT reccur_transac_id, user_id, type, amount, category, description, "
        "       interval, start_date, next_due, is_active "
        "FROM recurring_transactions WHERE user_id = ? "
        "ORDER BY next_due ASC"
        );
    q.addBindValue(userId);

    if (!DatabaseManager::instance().executePrepared(q)) return list;

    while (q.next()) {
        RecurringTransaction r;
        r.set_reccur_transac_id(q.value(0).toInt());
        r.set_user_id(q.value(1).toInt());
        r.set_type(q.value(2).toString() == "income"
                       ? RecurringTransaction::Income
                       : RecurringTransaction::Expense);
        r.set_amount(q.value(3).toDouble());
        r.set_category(q.value(4).toString());
        r.set_description(q.value(5).toString());
        r.set_interval(q.value(6).toString());
        r.set_start_date(QDateTime::fromString(q.value(7).toString(), Qt::ISODate));
        r.set_next_due(QDateTime::fromString(q.value(8).toString(), Qt::ISODate));
        r.set_is_active(q.value(9).toBool());
        list.push_back(r);
    }
    return list;
}

bool RecurringTransactionService::setActive(int recurringId, bool active)
{
    QSqlQuery q = DatabaseManager::instance().prepare(
        "UPDATE recurring_transactions SET is_active = ? WHERE reccur_transac_id = ?"
        );
    q.addBindValue(active ? 1 : 0);
    q.addBindValue(recurringId);
    return DatabaseManager::instance().executePrepared(q);
}

bool RecurringTransactionService::deleteRecurring(int recurringId)
{
    QSqlQuery q = DatabaseManager::instance().prepare(
        "DELETE FROM recurring_transactions WHERE reccur_transac_id = ?"
        );
    q.addBindValue(recurringId);
    return DatabaseManager::instance().executePrepared(q);
}

// Helper — advance a date by one "tick" of the given interval
static QDateTime advance(const QDateTime& from, const QString& interval)
{
    if (interval == "daily")   return from.addDays(1);
    if (interval == "weekly")  return from.addDays(7);
    return from.addMonths(1);   // "monthly" or anything unrecognized
}

int RecurringTransactionService::processDueRecurrences(int userId)
{
    int created = 0;

    //1.find this user's account_id (we need it to insert real transactions)
    QSqlQuery accQ = DatabaseManager::instance().prepare(
        "SELECT id FROM accounts WHERE user_id = ? LIMIT 1"
        );
    accQ.addBindValue(userId);
    int accountId = 0;
    if (DatabaseManager::instance().executePrepared(accQ) && accQ.next())
        accountId = accQ.value(0).toInt();
    if (accountId == 0) return 0;   // user has no account — nothing we can do

    //2. get all active recurring rules for this user
    std::vector<RecurringTransaction> rules = getAllByUser(userId);

    QDateTime now = QDateTime::currentDateTime();
    TransactionService txSvc;

    for (auto& r : rules) {
        if (!r.get_is_active()) continue;
        if (!r.get_next_due().isValid()) continue;

        //3.while the rule is overdue, fire transactions and advance next_due.
        // This catches up missed runs (e.g. user didn't open the app for 3 days
        // on a daily rule — fires 3 transactions in a row).
        QDateTime due = r.get_next_due();
        while (due <= now) {
            Transaction t;
            t.set_account_id(accountId);
            t.set_type(r.get_type() == RecurringTransaction::Income
                           ? Transaction::Income
                           : Transaction::Expense);
            t.set_amount(r.get_amount());
            t.set_category(r.get_category());
            t.set_description(r.get_description() + " (auto)");
            t.set_transac_date(due);

            if (txSvc.addTransaction(t)) {
                created++;
            } else {
                // If insert failed, bail out of this rule rather than having an infinite-loop
                break;
            }

            due = advance(due, r.get_interval());
        }

        //4.save the new next_due back to the DB if anything changed
        if (due != r.get_next_due()) {
            QSqlQuery upd = DatabaseManager::instance().prepare(
                "UPDATE recurring_transactions SET next_due = ? WHERE reccur_transac_id = ?"
                );
            upd.addBindValue(due.toString(Qt::ISODate));
            upd.addBindValue(r.get_reccur_transac_id());
            DatabaseManager::instance().executePrepared(upd);
        }
    }

    return created;
}