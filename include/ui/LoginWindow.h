#pragma once
#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>

class LoginWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit LoginWindow(QWidget*parent = nullptr);
    ~LoginWindow() = default;
private:
    QWidget* m_leftPanel;
    QWidget* m_rightPanel;

    //Elements of Our Left Panel
    QLabel* m_logoIcon;
    QLabel* m_appName;
    QLabel* m_tagline;

    //Right Panel Elements
    QLabel* m_heading;
    QLabel* m_subheading;

    QLabel*     m_usernameLabel;
    QLineEdit*  m_usernameInput;

    QLabel*     m_passwordLabel;
    QLineEdit*  m_passwordInput;

    //Error Message
    QLabel* m_errorLabel;

    void setupUI();
    void setupLeftPanel();
    void setupRightPanel();
    void applyStyles();

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onForgotPasswordClicked();

};