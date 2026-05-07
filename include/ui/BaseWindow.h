#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QFile>
#include <QApplication>

class BaseWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit BaseWindow(QWidget* parent = nullptr);
    virtual ~BaseWindow() = default;

    void initialize();
    void CenterOnScreen();

protected:
    virtual void setupUI() = 0;
    virtual void applyStyles() = 0;
    void loadGlobalStylesheet();
    void setupWindowGeometry(int width = 1100, int height = 680);

    void setupLogo();
};
