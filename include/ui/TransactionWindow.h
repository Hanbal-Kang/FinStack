// This is a Base Parent class for Deposit and Withdrawl window
#pragma once
#include "ui/BaseWindow.h"
#include "models/User.h"
#include "models/Transaction.h"
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

class TransactionWindow : public BaseWindow
{
    Q_OBJECT
public:

    //constructor and Destructor
    explicit TransactionWindow(const User& user, QWidget* parent = nullptr);
    ~TransactionWindow() override = default;

signals:
    void transaction_confirmed(const Transaction& t);
    void cancelled();

    //navigate back when "X" on top right is clicked
    void navigateBack();

protected:
    void setupUI() override;
    void applyStyles() override;


    //Child Classes will override these to customize appearance
    virtual QString windowTitle() const  = 0;
    virtual QString subtitle() const = 0;
    virtual QString iconSymbol() const  = 0;
    virtual QString confirmBtnText() const  = 0;

    virtual QString iconColor()      const = 0;  //green for deposit | red for withdraw
    virtual Transaction::Type transactionType() const = 0;  //Income or Expense


    //Shared widgets; Accessible by child classes
    QLineEdit* m_amountInput = nullptr;
    QComboBox* m_categoryCombo = nullptr;   //list of categories to choose
    QDateEdit* m_dateEdit = nullptr;
    QTextEdit* m_descripInput = nullptr;
    QLabel* errorLabel = nullptr;
    QPushButton* confirm_Button = nullptr;

    //User
    User m_user;

private slots:

    void onConfirmClicked();
    void onCancelClicked();

};