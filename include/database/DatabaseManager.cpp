#include "DatabaseManager.h"
#include <iostream>
using namespace std;

// Constructor
DatabaseManager::DatabaseManager() {
    db = nullptr;
}

// Singleton instance
DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

// Open database
bool DatabaseManager::open(const string& dbPath) {
    if (sqlite3_open(dbPath.c_str(), &db) == SQLITE_OK) {
        return true;
    }
    else {
        cerr << "Failed to open DB\n";
        return false;
    }
}

// Execute INSERT / UPDATE / DELETE
bool DatabaseManager::execute(const string& query) {
    char* errMsg = nullptr;

    int result = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);

    if (result != SQLITE_OK) {
        cerr << "SQL Error: " << errMsg << "\n";
        sqlite3_free(errMsg);
        return false;
    }

    return true;
}

// Prepare SELECT queries
sqlite3_stmt* DatabaseManager::prepare(const string& query) {
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Failed to prepare statement\n";
        return nullptr;
    }

    return stmt;
}

// Close database
void DatabaseManager::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}