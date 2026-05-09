#include <QApplication>
#include <QFile>
#include "ui/LoginWindow.h"
#include <database/DatabaseManager.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("FinStack");

    QFile styleFile(":/styles/app.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        app.setStyleSheet(styleFile.readAll());
        styleFile.close();
    }

    AppController controller;
    controller.start();

    return app.exec();
}