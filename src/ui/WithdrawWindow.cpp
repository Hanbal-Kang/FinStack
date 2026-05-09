#include "ui/WithdrawWindow.h"

WithdrawWindow::WithdrawWindow(const User& user, QWidget* parent)
    : TransactionWindow(user, parent)
    {
    initialize();
    }