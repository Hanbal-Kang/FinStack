#include "database/DatabaseManager.h"
#include <QDebug>

// Constructor
DatabaseManager::DatabaseManager() {
}

// Singleton instance
DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

// Open database (SAFE VERSION)
bool DatabaseManager::open(const QString& dbPath) {

    if (QSqlDatabase::contains("MainConnection")) {
        db = QSqlDatabase::database("MainConnection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", "MainConnection");
    }

    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qDebug() << "Failed to open DB:" << db.lastError().text();
        return false;
    }

    qDebug() << "Database opened successfully";
    return true;
}

// Execute query
bool DatabaseManager::execute(const QString& queryStr) {
    QSqlQuery query(db);

    if (!query.exec(queryStr)) {
        qDebug() << "SQL Error:" << query.lastError().text();
        return false;
    }

    //little change for goal window
    QString createTableQuery =
        "CREATE TABLE IF NOT EXISTS savings_goals ("
        "savings_goal_id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER, "
        "description TEXT, "
        "target_amount REAL, "
        "deadline DATETIME, "
        "saved_amount REAL DEFAULT 0.0)";

    QSqlQuery q(db);
    if (!q.exec(createTableQuery)) {
        qDebug() << "Error creating table:" << q.lastError().text();
    }
    return true;
}

// Prepare query
QSqlQuery DatabaseManager::prepare(const QString& queryStr) {
    QSqlQuery query(db);
    query.prepare(queryStr);
    return query;
}

//execute prepared
bool DatabaseManager::executePrepared(QSqlQuery& query) {   // ADDED
    if (!query.exec()) {
        qDebug() << "SQL Error:" << query.lastError().text();
        return false;
    }
    return true;
}

// Creates all tables if they don't exist. Safe to call every app start.
bool DatabaseManager::initSchema()
{
    //users table — matches User model + email for AuthService
    bool ok = execute(
        "CREATE TABLE IF NOT EXISTS users ("
        "  id                INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  username          TEXT NOT NULL UNIQUE,"
        "  password_hash     TEXT NOT NULL,"
        "  email             TEXT,"
        "  acc_creation_date TEXT NOT NULL"
        ")"
        );

    //accounts table - transactions reference account_id
    ok &= execute(
        "CREATE TABLE IF NOT EXISTS accounts ("
        "  id      INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  user_id INTEGER NOT NULL,"
        "  name    TEXT NOT NULL DEFAULT 'Main',"
        "  FOREIGN KEY(user_id) REFERENCES users(id)"
        ")"
        );

    //transactions table
    ok &= execute(
        "CREATE TABLE IF NOT EXISTS transactions ("
        "  id               INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  account_id       INTEGER NOT NULL,"
        "  type             TEXT NOT NULL CHECK(type IN ('income','expense')),"
        "  amount           REAL NOT NULL,"
        "  category         TEXT,"
        "  description      TEXT,"
        "  transaction_date TEXT NOT NULL,"
        "  FOREIGN KEY(account_id) REFERENCES accounts(id)"
        ")"
        );

    //budgets table — matches BudgetService
    ok &= execute(
        "CREATE TABLE IF NOT EXISTS budgets ("
        "  id            INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  user_id       INTEGER NOT NULL,"
        "  category      TEXT NOT NULL,"
        "  monthly_limit REAL NOT NULL,"
        "  FOREIGN KEY(user_id) REFERENCES users(id)"
        ")"
        );

    // goals table — for SavingsGoalWindow
    ok &= execute(
        "CREATE TABLE IF NOT EXISTS savings_goals ("
        "  savings_goal_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  user_id         INTEGER NOT NULL,"
        "  description     TEXT NOT NULL,"
        "  target_amount   REAL NOT NULL,"
        "  saved_amount    REAL NOT NULL DEFAULT 0,"
        "  deadline        TEXT NOT NULL,"
        "  FOREIGN KEY(user_id) REFERENCES users(id)"
        ")"
        );

    return ok;
}

// Close database
void DatabaseManager::close() {
    if (db.isOpen()) {
        db.close();
        qDebug() << "Database closed";
    }
}