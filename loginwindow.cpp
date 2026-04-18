#include "loginwindow.h"
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFrame>

LoginWindow::LoginWindow(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    QFrame *card = new QFrame();
    card->setObjectName("Card");

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setSpacing(15);

    titleLabel = new QLabel ("Welcome Back");
    titleLabel->setObjectName("title");

    //Email Input Box
    emailInput = new QLineEdit();
    emailInput->setPlaceholderText("Email");

    //Password Input Box
    passwordInput = new QLineEdit();
    passwordInput->setPlaceholderText("Password");

    //Login Button
    loginButton = new QPushButton("Login");

    //Adding this all to the layout

    cardLayout->addWidget(titleLabel);
    cardLayout->addWidget(emailInput);
    cardLayout->addWidget(passwordInput);
    cardLayout->addWidget(loginButton);

    mainLayout->addWidget(card);

    connect(loginButton, &QPushButton::clicked, this, &LoginWindow::handleLogin);


    //Styling the Login Page Here

    this->setStyleSheet(R"(

    QWidget {

        background-color: #F7F9FB;

        font-family: Arial;

    }

    #card {

        background: white;

        border-radius: 15px;

        padding: 30px;

        min-width: 320px;

    }

    #title {

        font-size: 20px;

        font-weight: bold;

        color: #2E2E2E;

    }

    QLineEdit {

        border: 1px solid #E0E0E0;

        border-radius: 10px;

        padding: 10px;

        font-size: 14px;

    }

    QLineEdit:focus {

        border: 1px solid #4A90E2;

    }

    QPushButton {

        background-color: #4A90E2;

        color: white;

        border-radius: 10px;

        padding: 10px;

        font-weight: bold;

    }
    QPushButton::hover
    {
    background-color #357ABD
}

)");
}

void LoginWindow::handleLogin()
{
    QString email = emailInput->text();
    QString passsword = passwordInput->text();
}

LoginWindow::~LoginWindow()
{

}