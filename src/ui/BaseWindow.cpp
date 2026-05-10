#include "ui/BaseWindow.h"
#include <QScreen>

BaseWindow::BaseWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setMinimumSize(900, 600);
    setWindowTitle("FinStack");
}

void BaseWindow::initialize()
{
    setupUI();
    applyStyles();
}

void BaseWindow::CenterOnScreen()
{
    QScreen* screen = QApplication::primaryScreen();
    if(!screen) return;

    QRect screenRect = screen->availableGeometry();
    int x = (screenRect.width()  - width())  / 2;
    int y = (screenRect.height() - height()) / 2;
    move(x, y);
}

void BaseWindow::loadGlobalStylesheet()
{
    QFile file(":/styles/app.qss");
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString stylesheet = QString::fromUtf8(file.readAll());
        qApp->setStyleSheet(stylesheet);
        file.close();
    }
}

void BaseWindow::setupWindowGeometry(int width, int height)
{
    QScreen* screen = QApplication::primaryScreen();
    if (!screen) {
        resize(width, height);
        return;
    }
    QRect screenRect = screen->availableGeometry();
    int x = (screenRect.width()  - width)  / 2;
    int y = (screenRect.height() - height) / 2;
    setGeometry(x, y, width, height);
}