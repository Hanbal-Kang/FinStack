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

    return true;
}

// Prepare query
QSqlQuery DatabaseManager::prepare(const QString& queryStr) {
    QSqlQuery query(db);
    query.prepare(queryStr);
    return query;
}

//execute prepared
bool DatabaseManager::executePrepared(QSqlQuery& query) {   // ✅ ADDED
    if (!query.exec()) {
        qDebug() << "SQL Error:" << query.lastError().text();
        return false;
    }
    return true;
}


// Close database
void DatabaseManager::close() {
    if (db.isOpen()) {
        db.close();
        qDebug() << "Database closed";
    }
}