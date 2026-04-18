#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QLabel;

class LoginWindow : public QWidget
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void handleLogin();
signals:
    void loginSuccessful();
private:
    QLabel *titleLabel;

    QLineEdit *emailInput;
    QLineEdit *passwordInput;

    QPushButton *loginButton;
};

#endif // LOGINWINDOW_H
