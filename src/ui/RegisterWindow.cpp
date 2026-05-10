#include "ui/RegisterWindow.h"
#include "ui/LoginWindow.h"
#include "services/AuthService.h"
#include "utils/Validator.h"
#include <QApplication>
#include <QScreen>

RegisterWindow::RegisterWindow(QWidget* parent) : BaseWindow(parent)
{
    setWindowTitle("FinStack");
    setMinimumSize(900, 600);

    QScreen* screen = QApplication::primaryScreen();
    QRect screenRect = screen->availableGeometry();
    int x = (screenRect.width()  - 1100) / 2;
    int y = (screenRect.height() - 680)  / 2;
    setGeometry(x, y, 1100, 680);

    initialize();
}

void RegisterWindow::setupUI()
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

void RegisterWindow::applyStyles()
{
    loadGlobalStylesheet();
}

void RegisterWindow::setupLeftPanel()
{
    QVBoxLayout* layout = new QVBoxLayout(m_leftPanel);
    layout->setContentsMargins(40, 48, 40, 48);
    layout->setSpacing(0);

    QHBoxLayout* logoRow = new QHBoxLayout();
    logoRow->setSpacing(12);

    QWidget* iconWidget = new QWidget();
    iconWidget->setFixedSize(44, 44);
    iconWidget->setObjectName("logoIcon");

    QVBoxLayout* barsLayout = new QVBoxLayout(iconWidget);
    barsLayout->setContentsMargins(10, 10, 10, 10);
    barsLayout->setSpacing(4);

    QLabel* bar1 = new QLabel(); bar1->setMaximumWidth(14); bar1->setFixedHeight(5); bar1->setObjectName("barLight");
    QLabel* bar2 = new QLabel(); bar2->setMaximumWidth(20); bar2->setFixedHeight(5); bar2->setObjectName("barMid");
    QLabel* bar3 = new QLabel(); bar3->setMaximumWidth(26); bar3->setFixedHeight(5); bar3->setObjectName("barDark");

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
    dotsRow->setSpacing(8);
    QLabel* dot1 = new QLabel(); dot1->setFixedSize(8, 8); dot1->setObjectName("dotActive");
    QLabel* dot2 = new QLabel(); dot2->setFixedSize(8, 8); dot2->setObjectName("dotInactive");
    QLabel* dot3 = new QLabel(); dot3->setFixedSize(8, 8); dot3->setObjectName("dotInactive");
    dotsRow->addWidget(dot1);
    dotsRow->addWidget(dot2);
    dotsRow->addWidget(dot3);
    dotsRow->addStretch();

    layout->addLayout(dotsRow);
    layout->addStretch(1);
}

void RegisterWindow::setupRightPanel()
{
    QVBoxLayout* layout = new QVBoxLayout(m_rightPanel);
    layout->setContentsMargins(72, 0, 72, 0);
    layout->setSpacing(0);

    layout->addStretch(2);

    m_heading = new QLabel("Create account");
    m_heading->setObjectName("heading");

    m_subheading = new QLabel("Sign up for FinStack");
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

    //Added Email Field
    m_emailLabel = new QLabel("Email");
    m_emailLabel->setObjectName("inputLabel");
    m_emailInput = new QLineEdit();
    m_emailInput->setPlaceholderText("Enter your email");
    m_emailInput->setFixedHeight(46);
    m_emailInput->setObjectName("inputField");

    layout->addWidget(m_emailLabel);
    layout->addSpacing(6);
    layout->addWidget(m_emailInput);
    layout->addSpacing(20);

    m_passwordLabel = new QLabel("Password");
    m_passwordLabel->setObjectName("inputLabel");
    m_passwordInput = new QLineEdit();
    m_passwordInput->setPlaceholderText("Enter your password");
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_passwordInput->setFixedHeight(46);
    m_passwordInput->setObjectName("inputField");

    layout->addWidget(m_passwordLabel);
    layout->addSpacing(6);
    layout->addWidget(m_passwordInput);
    layout->addSpacing(20);

    m_confirmPasswordLabel = new QLabel("Confirm Password");
    m_confirmPasswordLabel->setObjectName("inputLabel");
    m_confirmPasswordInput = new QLineEdit();
    m_confirmPasswordInput->setPlaceholderText("Re-enter your password");
    m_confirmPasswordInput->setEchoMode(QLineEdit::Password);
    m_confirmPasswordInput->setFixedHeight(46);
    m_confirmPasswordInput->setObjectName("inputField");

    layout->addWidget(m_confirmPasswordLabel);
    layout->addSpacing(6);
    layout->addWidget(m_confirmPasswordInput);
    layout->addSpacing(28);

    m_errorLabel = new QLabel("");
    m_errorLabel->setObjectName("errorLabel");
    m_errorLabel->hide();
    layout->addWidget(m_errorLabel);

    m_registerButton = new QPushButton("Create Account");
    m_registerButton->setFixedHeight(48);
    m_registerButton->setObjectName("primaryButton");
    m_registerButton->setCursor(Qt::PointingHandCursor);
    layout->addWidget(m_registerButton);
    layout->addSpacing(24);

    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setObjectName("divider");
    layout->addWidget(line);
    layout->addSpacing(24);

    QHBoxLayout* loginRow = new QHBoxLayout();
    QLabel* loginText = new QLabel("Already have an account?");
    loginText->setObjectName("mutedText");

    m_loginButton = new QPushButton("Sign in");
    m_loginButton->setObjectName("linkButton");
    m_loginButton->setCursor(Qt::PointingHandCursor);

    loginRow->addStretch();
    loginRow->addWidget(loginText);
    loginRow->addSpacing(4);
    loginRow->addWidget(m_loginButton);
    loginRow->addStretch();

    layout->addLayout(loginRow);
    layout->addStretch(2);

    connect(m_registerButton, &QPushButton::clicked, this, &RegisterWindow::onRegisterClicked);
    connect(m_loginButton, &QPushButton::clicked, this, [this]() {
        LoginWindow* login = new LoginWindow(nullptr);
        login->show();
        this->close();
    });
}

void RegisterWindow::onRegisterClicked()
{
    QString username = m_usernameInput->text().trimmed();
    QString email    = m_emailInput->text().trimmed();
    QString password = m_passwordInput->text();
    QString confirm  = m_confirmPasswordInput->text();

    //1. Empty-field check
    if (username.isEmpty() || email.isEmpty() || password.isEmpty() || confirm.isEmpty()) {
        m_errorLabel->setText("Please fill in all fields");
        m_errorLabel->show();
        return;
    }

    //2. Username rule (≥ 3 chars, non-empty) — uses Validator utility (from our utils)
    if (!Validator::isValidUsername(username)) {
        m_errorLabel->setText("Username must be at least 3 characters");
        m_errorLabel->show();
        return;
    }

    //3. Email basic sanity check (contains '@' and a '.')
    if (!email.contains('@') || !email.contains('.')) {
        m_errorLabel->setText("Please enter a valid email");
        m_errorLabel->show();
        return;
    }

    //4. Password match (Both password fields should have the same pass)
    if (password != confirm) {
        m_errorLabel->setText("Passwords do not match");
        m_errorLabel->show();
        return;
    }

    //5. Password strength (8+ chars, uppercase + digit + symbol)
    if (!Validator::isPasswordStrong(password)) {
        m_errorLabel->setText("Password must be 8+ chars with uppercase, digit, and symbol");
        m_errorLabel->show();
        return;
    }

    //6. Hand off to AuthService — inserts hashed password into the DB
    AuthService auth;
    if (!auth.registerUser(username, password, email)) {
        //Most common cause: username already exists (UNIQUE constraint)
        m_errorLabel->setText("Could not create account. Username may already be taken.");
        m_errorLabel->show();
        return;
    }

    m_errorLabel->hide();

    //Success — go back to LoginWindow so the user can sign in
    LoginWindow* login = new LoginWindow(nullptr);
    login->show();
    this->close();
}