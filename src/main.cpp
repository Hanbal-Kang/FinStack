#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include "ui/LoginWindow.h"
#include <database/DatabaseManager.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("FinStack");

    // Open SQLite DB at <AppData>/FinStack/finstack.db. AppDataLocation gives
    // us a writable per-user folder (e.g. ~/Library/Application Support on macOS).
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);   // ensure folder exists
    QString dbPath = dataDir + "/finstack.db";

    //Just to test wether my database is opened or not
    qDebug() << "DB path:" << dbPath;

    if (!DatabaseManager::instance().open(dbPath)) {
        qDebug() << "Could not open database — exiting";
        return 1;
    }

    if (!DatabaseManager::instance().initSchema()) {
        qDebug() << "Could not init schema — exiting";
        return 1;
    }

    QFile styleFile(":/styles/app.qss");
    if (styleFile.open(QFile::ReadOnly))
    {
        app.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }

    LoginWindow window;
    window.show();

    return app.exec();
}