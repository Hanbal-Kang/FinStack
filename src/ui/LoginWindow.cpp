#include "ui/LoginWindow.h"
#include <QApplication>
#include <QScreen>
#include <QSizePolicy>
#include <QSpacerItem>

LoginWindow::LoginWindow(QWidget* parent):QMainWindow(parent)
{
    setWindowTitle("FinStack");
    setMinimumSize(900,600);

    QScreen* screen = QApplication::primaryScreen();
    QRect screenRect = screen->availableGeometry();
    int x = (screenRect.width()  - 1100) / 2;
    int y = (screenRect.height() - 680)  / 2;
    setGeometry(x, y, 1100, 680);

    setupUI();
    applyStyles();
}

void LoginWindow::setupUI()
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_leftPanel = new QWidget();
    m_leftPanel->setFixedWidth(380);
    m_leftPanel->setObjectName("leftPanel");

    m_rightPanel = new QWidget();
    m_rightPanel->setObjectName("rightPanel");

    mainLayout->addWidget(m_leftPanel);
    mainLayout->addWidget(m_rightPanel);

    setupLeftPanel();
    setupRightPanel();
}

void LoginWindow::setupLeftPanel()
{
    QVBoxLayout* layout = new QVBoxLayout(m_leftPanel);
    layout->setContentsMargins(40, 48, 40, 48);
    layout->setSpacing(0);

    QHBoxLayout* logoRow = new QHBoxLayout();
    logoRow->setSpacing(12);

    // Logo icon — stacked bars widget
    QWidget* iconWidget = new QWidget();
    iconWidget->setFixedSize(44, 44);
    iconWidget->setObjectName("logoIcon");

    QVBoxLayout* barsLayout = new QVBoxLayout(iconWidget);
    barsLayout->setContentsMargins(10, 10, 10, 10);
    barsLayout->setSpacing(4);

    QLabel* bar1 = new QLabel();
    QLabel* bar2 = new QLabel();
    QLabel* bar3 = new QLabel();

    bar1->setMaximumWidth(14);
    bar2->setMaximumWidth(20);
    bar3->setMaximumWidth(26);

    bar1->setFixedHeight(5);
    bar2->setFixedHeight(5);
    bar3->setFixedHeight(5);

    bar1->setObjectName("barLight");
    bar2->setObjectName("barMid");
    bar3->setObjectName("barDark");

    barsLayout->addStretch();
    barsLayout->addWidget(bar1);
    barsLayout->addWidget(bar2);
    barsLayout->addWidget(bar3);

m_appName = new QLabel("FinStack");
m_appName->setObjectName("appName");

logoRow->addWidget(iconWidget);
logoRow->addWidget(m_appName);
    logoRow->addStretch();

    layout->addLayout(logoRow);
    layout->addStretch(2);

    QLabel* line1 = new QLabel("Your money.");
    QLabel* line2 = new QLabel("Your control.");
    line1->setObjectName("tagline");
    line2->setObjectName("tagline");

    layout->addWidget(line1);
    layout->addWidget(line2);

    layout->addSpacing(24);

    QHBoxLayout* dotsRow = new QHBoxLayout();
    QLabel* dot1 = new QLabel();
    QLabel* dot2 = new QLabel();
    QLabel* dot3 = new QLabel();

    dot1->setFixedSize(8, 8);
    dot2->setFixedSize(8, 8);
    dot3->setFixedSize(8, 8);

    dot1->setObjectName("dotActive");
    dot2->setObjectName("dotInactive");
    dot3->setObjectName("dotInactive");

    dotsRow->addWidget(dot1);
    dotsRow->addWidget(dot2);
    dotsRow->addWidget(dot3);
    dotsRow->addStretch();
    dotsRow->setSpacing(8);

    layout->addLayout(dotsRow);
    layout->addStretch(1);
}

void LoginWindow::setupRightPanel()
{
    QVBoxLayout* layout = new QVBoxLayout(m_rightPanel);
    layout->setContentsMargins(72, 0, 72, 0);
    layout->setSpacing(0);

    layout->addStretch(2);

    m_heading = new QLabel("Welcome back");
    m_heading->setObjectName("heading");

    m_subheading = new QLabel("Sign in to your FinStack account");
    m_subheading->setObjectName("subheading");

    layout->addWidget(m_heading);
    layout->addSpacing(6);
    layout->addWidget(m_subheading);
    layout->addSpacing(40);

    m_usernameLabel = new QLabel("Username");
    m_usernameLabel->setObjectName("inputLabel");

    m_usernameInput = new QLineEdit();
    m_usernameInput->setPlaceholderText("Enter your username");
    m_usernameInput->setFixedHeight(46);
    m_usernameInput->setObjectName("inputField");

    layout->addWidget(m_usernameLabel);
    layout->addSpacing(6);
    layout->addWidget(m_usernameInput);
    layout->addSpacing(20);

    QHBoxLayout* passLabelRow = new QHBoxLayout();
    m_passwordLabel = new QLabel("Password");
    m_passwordLabel->setObjectName("inputLabel");

    m_forgotPassword = new QPushButton("Forgot password?");
    m_forgotPassword->setObjectName("linkButton");
    m_forgotPassword->setCursor(Qt::PointingHandCursor);

    passLabelRow->addWidget(m_passwordLabel);
    passLabelRow->addStretch();
    passLabelRow->addWidget(m_forgotPassword);

    m_passwordInput = new QLineEdit();
    m_passwordInput->setPlaceholderText("Enter your password");
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_passwordInput->setFixedHeight(46);
    m_passwordInput->setObjectName("inputField");

    layout->addLayout(passLabelRow);
    layout->addSpacing(6);
    layout->addWidget(m_passwordInput);
    layout->addSpacing(28);

    m_errorLabel = new QLabel("");
    m_errorLabel->setObjectName("errorLabel");
    m_errorLabel->hide();
    layout->addWidget(m_errorLabel);

    m_loginButton = new QPushButton("Sign in");
    m_loginButton->setFixedHeight(48);
    m_loginButton->setObjectName("primaryButton");
    m_loginButton->setCursor(Qt::PointingHandCursor);

    layout->addWidget(m_loginButton);
    layout->addSpacing(24);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setObjectName("divider");
    layout->addWidget(line);
    layout->addSpacing(24);

    QHBoxLayout* registerRow = new QHBoxLayout();
    QLabel* registerText = new QLabel("Don't have an account?");
    registerText->setObjectName("mutedText");

    m_registerButton = new QPushButton("Create one");
    m_registerButton->setObjectName("linkButton");
    m_registerButton->setCursor(Qt::PointingHandCursor);

    registerRow->addStretch();
    registerRow->addWidget(registerText);
    registerRow->addSpacing(4);
    registerRow->addWidget(m_registerButton);
    registerRow->addStretch();

    layout->addLayout(registerRow);
    layout->addStretch(2);

    connect(m_loginButton,    &QPushButton::clicked,
            this, &LoginWindow::onLoginClicked);
    connect(m_registerButton, &QPushButton::clicked,
            this, &LoginWindow::onRegisterClicked);
    connect(m_forgotPassword, &QPushButton::clicked,
            this, &LoginWindow::onForgotPasswordClicked);
}

void LoginWindow::applyStyles()
{
    setStyleSheet(R"(
    QMainWindow {
        background-color: #0f172a;
    }
    QWidget#leftPanel {
        background-color: #111827;
    }
    QLabel#appName {
        color: #f1f5f9;
        font-size: 22px;
        font-weight: 600;
    }
    QWidget#logoIcon {
        background-color: #1e3a5f;
        border-radius: 10px;
    }
    QLabel#tagline {
        color: #f1f5f9;
        font-size: 20px;
        font-weight: 500;
    }
    QLabel#dotActive {
        background-color: #1d4ed8;
        border-radius: 4px;
    }
    QLabel#dotInactive {
        background-color: #1e293b;
        border-radius: 4px;
    }
    QWidget#rightPanel {
        background-color: #0f172a;
    }
    QLabel#heading {
        color: #f1f5f9;
        font-size: 26px;
        font-weight: 600;
    }
    QLabel#subheading {
        color: #64748b;
        font-size: 13px;
    }
    QLabel#inputLabel {
        color: #94a3b8;
        font-size: 12px;
        font-weight: 500;
    }
    QLabel#mutedText {
        color: #64748b;
        font-size: 13px;
    }
    QLineEdit#inputField {
        background-color: #1e293b;
        color: #f1f5f9;
        border: 1px solid #334155;
        border-radius: 8px;
        padding: 0px 14px;
        font-size: 13px;
    }
    QLineEdit#inputField:focus {
        border: 1px solid #3b82f6;
        background-color: #1e3a5f;
    }
    QPushButton#primaryButton {
        background-color: #1d4ed8;
        color: #ffffff;
        border: none;
        border-radius: 8px;
        font-size: 14px;
        font-weight: 500;
    }
    QPushButton#primaryButton:hover {
        background-color: #2563eb;
    }
    QPushButton#primaryButton:pressed {
        background-color: #1e40af;
    }
    QPushButton#linkButton {
        background: transparent;
        color: #3b82f6;
        border: none;
        font-size: 12px;
        padding: 0;
    }
    QPushButton#linkButton:hover {
        color: #60a5fa;
    }
    QFrame#divider {
        border: none;
        border-top: 1px solid #1e293b;
    }
    QLabel#errorLabel {
        color: #ef4444;
        font-size: 12px;
    }
    QLabel#barLight {
    background-color: #93c5fd;
    border-radius: 2px;
    }
    QLabel#barMid {
    background-color: #3b82f6;
    border-radius: 2px;
    }
    QLabel#barDark {
    background-color: #1d4ed8;
    border-radius: 2px;
    }
    )");
}

void LoginWindow::onLoginClicked()
{
    QString username = m_usernameInput->text().trimmed();
    QString password = m_passwordInput->text();

    if (username.isEmpty() || password.isEmpty())
    {
        m_errorLabel->setText("Please fill in all fields");
        m_errorLabel->show();
        return;
    }

    m_errorLabel->hide();
}

void LoginWindow::onRegisterClicked()
{
    // TODO: open RegisterWindow
}

void LoginWindow::onForgotPasswordClicked()
{
    // TODO: open ForgotPasswordWindow
}


