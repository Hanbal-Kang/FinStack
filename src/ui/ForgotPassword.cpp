#include "ui/ForgotPassword.h"
#include "ui/LoginWindow.h"
#include "ui/RecoveryCodeDialog.h"
#include "services/AuthService.h"
#include "utils/Validator.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

ForgotPasswordWindow::ForgotPasswordWindow(QWidget* parent) : BaseWindow(parent)
{
    setWindowTitle("FinStack — Reset Password");
    setupWindowGeometry(900, 600);
    initialize();
}

void ForgotPasswordWindow::setupUI()
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* root = new QVBoxLayout(central);
    root->setContentsMargins(60, 40, 60, 40);
    root->setSpacing(0);

    //Top row: Our back button
    QHBoxLayout* topRow = new QHBoxLayout();
    m_backBtn = new QPushButton("←", this);
    m_backBtn->setObjectName("navIconBtn");
    m_backBtn->setFixedSize(36, 36);
    m_backBtn->setCursor(Qt::PointingHandCursor);
    connect(m_backBtn, &QPushButton::clicked, this, &ForgotPasswordWindow::onBackClicked);
    topRow->addWidget(m_backBtn);
    topRow->addStretch();
    root->addLayout(topRow);

    root->addStretch(1);

    //Centered form
    QVBoxLayout* formLayout = new QVBoxLayout();
    formLayout->setSpacing(14);
    formLayout->setAlignment(Qt::AlignCenter);

    //Heading
    QLabel* heading = new QLabel("Reset Your Password");
    heading->setObjectName("authHeading");
    heading->setAlignment(Qt::AlignCenter);
    heading->setStyleSheet("font-size: 28px; font-weight: 700; color: #ffffff;");
    formLayout->addWidget(heading);

    QLabel* sub = new QLabel("Enter your username and recovery code to set a new password");
    sub->setObjectName("pageSubtitle");
    sub->setAlignment(Qt::AlignCenter);
    sub->setWordWrap(true);
    formLayout->addWidget(sub);

    formLayout->addSpacing(10);

    //Username
    QLabel* userLbl = new QLabel("Username");
    userLbl->setObjectName("inputLabel");
    m_usernameInput = new QLineEdit();
    m_usernameInput->setPlaceholderText("Enter your username");
    m_usernameInput->setFixedHeight(46);
    m_usernameInput->setObjectName("inputField");
    formLayout->addWidget(userLbl);
    formLayout->addWidget(m_usernameInput);

    //Recovery code
    QLabel* codeLbl = new QLabel("Recovery Code");
    codeLbl->setObjectName("inputLabel");
    m_codeInput = new QLineEdit();
    m_codeInput->setPlaceholderText("XXXX-XXXX-XXXX-XXXX");
    m_codeInput->setFixedHeight(46);
    m_codeInput->setObjectName("inputField");
    //auto-uppercase as user types (Recovery Code is Taken Given and Taken in Upper case)
    m_codeInput->setStyleSheet("font-family: monospace; letter-spacing: 2px;");
    formLayout->addWidget(codeLbl);
    formLayout->addWidget(m_codeInput);

    //New password
    QLabel* pwLbl = new QLabel("New Password");
    pwLbl->setObjectName("inputLabel");
    m_passwordInput = new QLineEdit();
    m_passwordInput->setPlaceholderText("8+ chars, uppercase, digit, symbol");
    m_passwordInput->setEchoMode(QLineEdit::Password);
    m_passwordInput->setFixedHeight(46);
    m_passwordInput->setObjectName("inputField");
    formLayout->addWidget(pwLbl);
    formLayout->addWidget(m_passwordInput);

    //Confirm
    QLabel* confLbl = new QLabel("Confirm New Password");
    confLbl->setObjectName("inputLabel");
    m_confirmInput = new QLineEdit();
    m_confirmInput->setPlaceholderText("Re-enter new password");
    m_confirmInput->setEchoMode(QLineEdit::Password);
    m_confirmInput->setFixedHeight(46);
    m_confirmInput->setObjectName("inputField");
    formLayout->addWidget(confLbl);
    formLayout->addWidget(m_confirmInput);

    //Error label
    m_errorLabel = new QLabel();
    m_errorLabel->setStyleSheet("color: #f85149;");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->hide();
    formLayout->addWidget(m_errorLabel);

    formLayout->addSpacing(6);

    //Submission button
    m_submitBtn = new QPushButton("Reset Password");
    m_submitBtn->setObjectName("primaryBtn");
    m_submitBtn->setFixedHeight(48);
    m_submitBtn->setCursor(Qt::PointingHandCursor);
    m_submitBtn->setStyleSheet("font-size: 15px; font-weight: 600;");
    connect(m_submitBtn, &QPushButton::clicked, this, &ForgotPasswordWindow::onSubmitClicked);
    formLayout->addWidget(m_submitBtn);

    //Constrain form width
    QHBoxLayout* centerWrap = new QHBoxLayout();
    centerWrap->addStretch();
    QWidget* formContainer = new QWidget();
    formContainer->setLayout(formLayout);
    formContainer->setMaximumWidth(420);
    centerWrap->addWidget(formContainer);
    centerWrap->addStretch();

    root->addLayout(centerWrap);
    root->addStretch(2);
}

void ForgotPasswordWindow::applyStyles()
{
    loadGlobalStylesheet();
}

void ForgotPasswordWindow::onSubmitClicked()
{
    QString username = m_usernameInput->text().trimmed();
    QString code     = m_codeInput->text().trimmed().toUpper();   // codes are uppercase
    QString password = m_passwordInput->text();
    QString confirm  = m_confirmInput->text();

    // Validation chain — same pattern as RegisterWindow
    if (username.isEmpty() || code.isEmpty() || password.isEmpty() || confirm.isEmpty()) {
        m_errorLabel->setText("Please fill in all fields");
        m_errorLabel->show();
        return;
    }
    if (password != confirm) {
        m_errorLabel->setText("Passwords do not match");
        m_errorLabel->show();
        return;
    }
    if (!Validator::isPasswordStrong(password)) {
        m_errorLabel->setText("Password must be 8+ chars with uppercase, digit, and symbol");
        m_errorLabel->show();
        return;
    }

    //Try the reset
    AuthService auth;
    QString newCode;
    if (!auth.resetPasswordWithRecoveryCode(username, code, password, newCode)) {
        m_errorLabel->setText("Username or recovery code is incorrect");
        m_errorLabel->show();
        return;
    }

    m_errorLabel->hide();

    //Show the new (rotated) recovery code — IMPORTANT: old one is now invalid
    RecoveryCodeDialog dialog(newCode, this);
    dialog.exec();

    //Back to login
    LoginWindow* login = new LoginWindow(nullptr);
    login->show();
    this->close();
}

void ForgotPasswordWindow::onBackClicked()
{
    LoginWindow* login = new LoginWindow(nullptr);
    login->show();
    this->close();
}