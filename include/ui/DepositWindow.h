#pragma once
#include "ui/TransactionWindow.h"

class DepositWindow : public TransactionWindow
{
    Q_OBJECT
public:
    explicit DepositWindow(const User& user , QWidget* parent = nullptr);
    ~DepositWindow() override = default;

protected:

    //Overriding All virtual functions from TransactionWindow
    QString windowTitle() const override {return "Add Deposit";}
    QString subtitle() const override {return "Record incoming Funds";}
    QString iconSymbol() const override {return "↑";}
    QString iconColor() const override {return "#3fb950";}  //green color
    QString confirmBtnText() const override {return "Confirm Deposit";}
    Transaction::Type transactionType() const override {return Transaction::Income;}
};