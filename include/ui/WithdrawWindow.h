#pragma once
#include "ui/TransactionWindow.h"

class WithdrawWindow : public TransactionWindow
{
    Q_OBJECT
public:

    explicit WithdrawWindow(const User& user, QWidget* parent = nullptr);
    ~WithdrawWindow () override = default;

protected:
    //Overriding all pure virtual functions of TransactionWindow

    QString windowTitle() const override { return "Add Withdrawal"; }
    QString subtitle() const override { return "Record outgoing funds"; }
    QString iconSymbol() const override { return "↓"; }
    QString iconColor() const override { return "#f85149"; }    //red
    QString confirmBtnText() const override { return "Confirm Withdrawal"; }
    Transaction::Type transactionType() const override { return Transaction::Expense; }
};