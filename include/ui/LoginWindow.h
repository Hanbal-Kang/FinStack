#pragma once

#include "ui/BaseWindow.h"
#include "models/User.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>

class LoginWindow : public BaseWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget* parent = nullptr);
    ~LoginWindow() = default;

signals:
    // Emitted after successful login — AppController connects this
    void loginSucceeded(const User& user);

private:
    QWidget* m_leftPanel;
    QWidget* m_rightPanel;

    QLabel*      m_logoIcon;
    QLabel*      m_appName;
    QLabel*      m_tagline;

    QLabel*      m_heading;
    QLabel*      m_subheading;
    QLabel*      m_usernameLabel;
    QLineEdit*   m_usernameInput;
    QLabel*      m_passwordLabel;
    QLineEdit*   m_passwordInput;
    QPushButton* m_forgotPassword;
    QPushButton* m_loginButton;
    QPushButton* m_registerButton;
    QLabel*      m_errorLabel;

    void setupUI()     override;
    void applyStyles() override;
    void setupLeftPanel();
    void setupRightPanel();

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onForgotPasswordClicked();
};