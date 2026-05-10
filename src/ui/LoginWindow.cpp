#include "ui/LoginWindow.h"
#include "ui/RegisterWindow.h"
#include "services/AuthService.h"
#include "utils/Validator.h"
#include "services/RecurringTransactionService.h"
#include <QApplication>
#include <QScreen>
#include <QSizePolicy>
#include <QSpacerItem>
#include <ui/DashboardWindow.h>

LoginWindow::LoginWindow(QWidget* parent):BaseWindow(parent)
{
    setWindowTitle("FinStack");
    setMinimumSize(900,600);

    QScreen* screen = QApplication::primaryScreen();
    QRect screenRect = screen->availableGeometry();
    int x = (screenRect.width()  - 1100) / 2;
    int y = (screenRect.height() - 680)  / 2;
    setGeometry(x, y, 1100, 680);

    initialize();
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

void LoginWindow::applyStyles()
{
    loadGlobalStylesheet();
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

void LoginWindow::onLoginClicked()
{
    QString username = m_usernameInput->text().trimmed();
    QString password = m_passwordInput->text();

    //1. Empty-field check
    if (username.isEmpty() || password.isEmpty()) {
        m_errorLabel->setText("Please fill in all fields");
        m_errorLabel->show();
        return;
    }

    //2. Verify credentials AND fetch the full User in one shot.
    // We use the 3-arg overload of login() — it fills `user` if password matches.
    AuthService auth;
    User user;
    if (!auth.login(username, password, user)) {
        // Either username doesn't exist OR password is wrong. We don't tell the
        // user which one (Because its a better security practice to not tell)
        m_errorLabel->setText("Invalid username or password");
        m_errorLabel->show();
        return;
    }

    m_errorLabel->hide();

   //So when the user logs in, it will instantly fire, and user will see fresh data in dashboard with transactions deducted
    RecurringTransactionService recSvc;
    recSvc.processDueRecurrences(user.get_id());

    DashboardWindow* dashboard = new DashboardWindow(user, nullptr);
    dashboard->show();
    this->close();
}

void LoginWindow::onRegisterClicked()
{
    // Open RegisterWindow, close this one. No nested connect — that was a bug
    // that made every click stack another connection on the same button.
    RegisterWindow* reg = new RegisterWindow(nullptr);
    reg->show();
    this->close();
}

void LoginWindow::onForgotPasswordClicked()
{
    // TODO: open ForgotPasswordWindow
}


