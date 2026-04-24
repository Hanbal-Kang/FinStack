#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <sqlite3.h>
#include <string>
using namespace std;

class DatabaseManager {
private:
    sqlite3* db;

    DatabaseManager(); // private constructor (singleton)

public:
    static DatabaseManager& instance();

    bool open(const string& dbPath);
    bool execute(const string& query);
    sqlite3_stmt* prepare(const string& query);
    void close();

    // prevent copying
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
};

#endif