#pragma once
#include "models/SavingsGoal.h"
#include "ui/BaseWindow.h"
#include "models/User.h"
#include "models/Transaction.h"
#include "models/Budget.h"

#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QFrame>
#include <QList>

class DashboardWindow : public BaseWindow
{
    Q_OBJECT

public:
    explicit DashboardWindow(const User& user, QWidget* parent = nullptr);
    ~DashboardWindow() override = default;

    void refreshDashboard();

signals:
    void navigateToWithdraw();
    void navigateToDeposit();
    void navigateToBudget();
    void navigateToAnalytics();
    void navigateToHistory();
    void navigateToSavings();
    void navigateToProfile();
    void logoutRequested();

protected:
    void setupUI()     override;
    void applyStyles() override;

private slots:
    void onWithdrawClicked();
    void onDepositClicked();
    void onBudgetClicked();
    void onAnalyticsClicked();
    void onHistoryClicked();
    void onProfileClicked();
    void onSavingsGoalsClicked();
    void onRecurringClicked();

private:
    void buildNavbar(QWidget* parent);
    void buildHeroCard(QVBoxLayout* layout);
    void buildQuickActions(QVBoxLayout* layout);
    void buildBottomRow(QVBoxLayout* layout);
    void buildTransactionsList(QWidget* parent);
    void buildBudgetPanel(QWidget* parent);

    QFrame*  makeQuickActionBtn(const QString& icon, const QString& label, const QString& accentColor, const char*    signal);
    QWidget* makeTransactionRow(const Transaction& tx);
    QWidget* makeBudgetBar(const QString& category, double spent, double limit);

    void   loadData();
    double totalBalance() const;
    double monthlyIncome()  const;
    double monthlyExpenses() const;

    //Added so We can have a comparison between previous month and this month
    double previousMonthNet() const;
    double currentMonthNet() const;

    User               m_user;
    QList<Transaction> m_transactions;
    QList<Budget>      m_budgets;
    QList<SavingsGoal> m_goals;

    QLabel*      m_balanceLabel  = nullptr;
    QLabel*      m_incomeLabel   = nullptr;
    QLabel*      m_expenseLabel  = nullptr;
    QVBoxLayout* m_txLayout      = nullptr;
    QVBoxLayout* m_budgetLayout  = nullptr;
    QLabel*      m_appName       = nullptr;
};
