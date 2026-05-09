#pragma once
#include "ui/BaseWindow.h"
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>

class RegisterWindow : public BaseWindow
{
    Q_OBJECT
public:
    explicit RegisterWindow(QWidget* parent = nullptr);
    ~RegisterWindow() = default;

private:
    QWidget* m_leftPanel;
    QWidget* m_rightPanel;

    QLabel*  m_appName;

    QLabel*      m_heading;
    QLabel*      m_subheading;
    QLabel*      m_usernameLabel;
    QLineEdit*   m_usernameInput;
    QLabel*      m_emailLabel;
    QLineEdit*   m_emailInput;
    QLabel*      m_passwordLabel;
    QLineEdit*   m_passwordInput;
    QLabel*      m_confirmPasswordLabel;
    QLineEdit*   m_confirmPasswordInput;
    QPushButton* m_registerButton;
    QPushButton* m_loginButton;
    QLabel*      m_errorLabel;

    void setupUI() override;
    void setupLeftPanel();
    void setupRightPanel();
    void applyStyles() override;

private slots:
    void onRegisterClicked();
};