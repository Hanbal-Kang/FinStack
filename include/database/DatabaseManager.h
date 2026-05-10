#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QSqlError>

class DatabaseManager {
private:
    QSqlDatabase db;

    DatabaseManager();

public:
    static DatabaseManager& instance();

    bool open(const QString& dbPath);
    bool execute(const QString& query);
    QSqlQuery prepare(const QString& query);
    bool executePrepared(QSqlQuery& query);

    //creates tables if they dont exist
    bool initSchema();

    void close();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
};

#endif