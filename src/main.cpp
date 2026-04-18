#include <QApplication>
#include "ui/LoginWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("FinStack");
    app.setOrganizationName("FinStack");

    LoginWindow window;
    window.show();

    return app.exec();
}