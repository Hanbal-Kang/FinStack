#include "ui/TransactionWindow.h"
#include "utils/Validator.h"
#include "models/Transaction.h"
#include <QApplication>
#include <QScreen>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDate>

TransactionWindow::TransactionWindow(const User& user, QWidget* parent) : BaseWindow(parent) , m_user(user)
{
    //In construcotr setting up the window
    setupWindowGeometry(500 , 650);

    //intialize calls - setupUI() and applyStyles()
    initialize();
}

//Building entire window layout here
void TransactionWindow::setupUI()
{
    QWidget* central = new QWidget(this);
    //puts the widget in the centre of screen
    setCentralWidget(central);

    //vertical layout box
    QVBoxLayout* root = new QVBoxLayout(central);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(20);

    //Header Row
    //Contains: colored icon box | title + subtile | close button
    QHBoxLayout* headerRow = new QHBoxLayout();
    headerRow->setSpacing(14);

    //Icon box --  The color and symbol woll come from child classes via iconColor() / iconSymbol()
    QLabel* iconLabel = new QLabel(iconSymbol());
    iconLabel->setObjectName("txtIconBox");
    iconLabel->setFixedSize(44, 44);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet(QString("background-color: %1; border-radius: 10px; color: white; font-size: 18px; font-weight: bold;").arg(iconColor()));

    //title and subtile - text will come from child class via windowTitle() / subtitle()
    QVBoxLayout* titleCol = new QVBoxLayout();
    titleCol->setSpacing(3);

    QLabel* titleLabel = new QLabel(windowTitle());
    titleLabel->setObjectName("txTitle");

    QLabel* subtitleLabel = new QLabel(subtitle());
    subtitleLabel->setObjectName("txSubtitle");

    //Adding to the box
    titleCol->addWidget(titleLabel);
    titleCol->addWidget(subtitleLabel);


    //X button - in the top right corner
    QPushButton* closeBtn = new QPushButton("X");
    closeBtn->setObjectName("txCloseBtn");
    closeBtn->setFixedSize(28, 28);
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, this , TransactionWindow::onCancelClicked());

    //Adding widgets to header row
    headerRow->addWidget(iconLabel);
    headerRow->addWidget(titleCol);
    headerRow->addStretch();
    headerRow->addWidget(closeBtn);

    //Divider line
    QFrame* divider = new QFrame();
    divider->setFrameShape(QFrame::HLine);
    divider->setObjectName("txDivider");
    root->addWidget(divider);

    //Amount
    QLabel* amountLabel = new QLabel("Amount");
    amountLabel->setObjectName("txFieldLabel");

    // Rs + input in the same row
    QHBoxLayout* amountRow = new QHBoxLayout();
    amountRow->setSpacing(0);

    QLabel* RupeesSign = new QLabel("Rs");
    RupeesSign->setObjectName("txRsSign");
    RupeesSign->setFixedWidth(32);
    RupeesSign->setAlignment(Qt::AlignCenter);

    //Amount input field
    m_amountInput = new QLineEdit();
    m_amountInput->setObjectName("txAmountInput");
    m_amountInput->setPlaceholderText("0.00");
    m_amountInput->setFixedHeight(48);

    //Adding widgets to our Row
    amountRow->addWidget(RupeesSign);
    amountRow->addWidget(m_amountInput);

    root->addWidget(amountLabel);
    root->addWidget(amountRow);


    //Category
    QLabel* categoryLabel = new QLabel("Category");
    categoryLabel->setObjectName("txFieldLabel");

    m_categoryCombo = new QComboBox();
    m_categoryCombo->setObjectName("txComboBox");
    m_categoryCombo->setFixedHeight(44);
    m_categoryCombo->addItems({"Food" , "Transport", "Rent", "Tuition", "Entertainment" , "Other"});

    //Adding to root
    root->addWidget(categoryLabel);
    root->addWidget(m_categoryCombo);


    //Date
    QLabel* dateLabel = new QLabel("Date");
    dateLabel->setObjectName("txFieldLabel");

    m_dateEdit = new QDateEdit();
    m_dateEdit->setObjectName("txDateEdit");
    m_dateEdit->setFixedHeight(44);
    m_dateEdit->setDate(QDate::currentDate());  //sets to default date (today)
    m_dateEdit->setCalendarPopup(true);

    //Adding to root
    root->addWidget(dateLabel);
    root->addWidget(m_dateEdit);


    //Description
    QLabel* descripLabel = new QLabel("Description (Optional)");
    descripLabel->setObjectName("txFieldLabel");

    m_descripInput = new QTextEdit();
    m_descripInput->setObjectName("txDescInput");
    m_descripInput->setPlaceholderText("Add a note...");
    m_descripInput->setFixedHeight(90);

    //Adding to root
    root->addWidget(descripLabel);
    root->addWidget(m_descripInput);


    //Error Label
    //Shown only when validation failts
    errorLabel = new QLabel("");
    errorLabel->setObjectName("txErrorLabel");
    errorLabel->hide();
    root->addWidget(errorLabel);

    root->addStretch();


    //Bottom Buttons; cancel(left) | confirm (right) , our confirm comes from the child class e.g (confirm deposit)

    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->setSpacing(12);

    QPushButton* cancelBtn = new QPushButton("Cancel");
    cancelBtn->setObjectName("txCancelBtn");
    cancelBtn->setFixedHeight(46);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    connect(cancelBtn, &QPushButton::clicked, this, &TransactionWindow::onCancelClicked);

    confirm_Button = new QPushButton(confirmBtnText());
    confirm_Button->setObjectName("txConfirmBtn");
    confirm_Button->setFixedHeight(46);
    confirm_Button->setCursor(Qt::PointingHandCursor);
    connect(confirm_Button, &QPushButton::clicked , this , &TransactionWindow::onConfirmClicked);

    //Adding these two buttons to Horizontal layout
    btnRow->addWidget(cancelBtn, 1);
    btnRow->addWidget(confirm_Button, 1);

    //Adding to root
    root->addLayout(btnRow);
}

//Loads global QSS stylesheet
void TransactionWindow::applyStyles()
{
    loadGlobalStylesheet();
}


//Confirm clicked: validates input, makes a transaction obj, emits transaction confirmed

void TransactionWindow::onConfirmClicked()
{
    //validating amount
    if(!Validator::isValidAmount(m_amountInput->text()))
    {
        errorLabel->setText("Please Enter a Valid Amount Greater than 0");
        errorLabel->show();
        return;
    }

    errorLabel->hide();

    Transaction t;
    t.set_user_id(m_user.get_id());
    t.set_type(transactionType());
    t.set_amount(m_amountInput->text().toDouble());
    t.set_category(m_categoryCombo->currentText());
    t.set_description(m_descripInput->toPlainText().trimmed());
    t.set_transac_date(QDateTime(m_dateEdit->date(), QTime::currentTime()));

    emit transaction_confirmed(t);
    close();
}

//Cancel Clicked; Discards the form and closes the window
void TransactionWindow::onCancelClicked()
{
    emit navigateBack();
    close();
}

































