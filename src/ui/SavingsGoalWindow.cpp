#include "ui/SavingsGoalWindow.h"
#include "services/GoalService.h"
#include "services/TransactionService.h"
#include <QProgressBar>
#include <QGridLayout>
#include <QScrollArea>

QFrame* SavingsGoalWindow::buildGoalCard(const SavingsGoal& goal) {
    QFrame* card = new QFrame();
    card->setObjectName("goalCard");
    QVBoxLayout* cl = new QVBoxLayout(card);

    double saved = goal.get_saved_amount();
    double target = goal.get_target_amount();
    double remaining = target - saved;
    double pct = (target > 0) ? qMin(saved / target * 100.0, 100.0) : 0.0;

    QString status = getGoalStatus(goal);
    QLabel* badge = new QLabel(status);
    if (status == "Achieved") badge->setStyleSheet("color:#14532D; background:#DCFCE7; border-radius:6px; padding:3px 10px;");
    else if (status == "At Risk") badge->setStyleSheet("color:#991B1B; background:#FEE2E2; border-radius:6px; padding:3px 10px;");
    else badge->setStyleSheet("color:#14532D; border:1px solid #22C55E; border-radius:6px; padding:3px 10px;");

    QHBoxLayout* amtRow = new QHBoxLayout();
    QLabel* curLbl = new QLabel("Rs " + QString::number(saved, 'f', 0));
    curLbl->setStyleSheet("color: #22C55E; font-weight: bold; font-size: 18px;");
    QLabel* remLbl = new QLabel("Rs " + QString::number(qAbs(remaining), 'f', 0));
    remLbl->setStyleSheet("color: #E6EDF3; font-weight: bold; font-size: 18px;");

    amtRow->addWidget(curLbl);
    amtRow->addStretch();
    amtRow->addWidget(new QLabel(remaining <= 0 ? "Surplus" : "Still Needed"));
    amtRow->addWidget(remLbl);

    cl->addLayout(amtRow);
    QProgressBar* bar = new QProgressBar();
    bar->setValue(static_cast<int>(pct));
    bar->setFixedHeight(8);
    bar->setTextVisible(false);
    bar->setObjectName("budgetBar");
    cl->addWidget(bar);
    return card;
}

//data helpers

void SavingsGoalWindow::loadGoals()
{
    // Compute the actual balance from the TransactionService
    m_balance = computeBalance();
    m_balanceLabel->setText("Rs " + QString::number(m_balance, 'f', 0));

    // Connect to your GoalService to fetch the user's saved goals
    GoalService service;
    m_goals = service.getGoalsForUser(m_user.get_id());
}

double SavingsGoalWindow::computeBalance()
{
    // This iterates through all transactions to find the current "Available" amount
    TransactionService service;
    std::vector<Transaction> all = service.getAllByUser(m_user.get_id());

    double currentBalance = 0.0;
    for (const auto& tx : all) {
        if (tx.get_type() == Transaction::Income)
            currentBalance += tx.get_amount();
        else
            currentBalance -= tx.get_amount();
    }
    return currentBalance;
}

void SavingsGoalWindow::refreshGoalCards()
{
    QGridLayout* grid = qobject_cast<QGridLayout*>(m_goalsGrid->layout());

    // Clear existing cards before re-rendering
    while (grid->count() > 1) {
        QLayoutItem* item = grid->takeAt(1);
        if (item->widget()) delete item->widget();
        delete item;
    }

    if (m_goals.empty()) {
        m_emptyLabel->show();
        return;
    }
    m_emptyLabel->hide();

    int col = 0, row = 1;
    for (const auto& goal : m_goals) {
        grid->addWidget(buildGoalCard(goal), row, col);
        col++;
        if (col == 2) { col = 0; row++; }
    }
}

// button actions

void SavingsGoalWindow::onAddGoalClicked()
{
    AddGoalDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    // Map the dialog inputs to a new SavingsGoal model
    SavingsGoal newGoal;
    newGoal.set_description(dialog.getGoalName());
    newGoal.set_target_amount(dialog.getTargetAmt());
    newGoal.set_user_id(m_user.get_id());
    newGoal.set_deadline(QDateTime(dialog.getDeadline(), QTime(0, 0)));
    newGoal.set_saved_amount(0.0); // Start new goals at zero progress

    GoalService service;
    if (service.createGoal(newGoal)) {
        loadGoals(); // Refresh data from DB
        refreshGoalCards(); // Update UI grid
    } else {
        QMessageBox::warning(this, "Error", "Failed to save your new goal.");
    }
}

void SavingsGoalWindow::onDeleteGoalClicked(int goalId)
{
    auto reply = QMessageBox::question(this, "Delete Goal",
                                       "Are you sure you want to remove this target?",
                                       QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        GoalService service;
        service.deleteGoal(goalId);
        loadGoals();
        refreshGoalCards();
    }
}

void SavingsGoalWindow::onBackClicked()
{
    emit backToDashboard();
    this->close();
}