#pragma once

#include <QObject>
#include "models/User.h"

class LoginWindow;
class DashboardWindow;
class BudgetWindow;

//  AppController
//
//  Owns every window and wires their navigation signals together.
//  Each window only emits signals — it never creates or knows about
//  other windows. The controller is the only place that decides what
//  to show next.

class AppController : public QObject
{
    Q_OBJECT

public:
    explicit AppController(QObject* parent = nullptr);
    ~AppController() override;

    void start();   // called once from main.cpp

private slots:
    void onLoginSucceeded(const User& user);
    void onNavigateToBudget();
    void onNavigateToWithdraw();
    void onNavigateToDeposit();
    void onNavigateToHistory();
    void onNavigateToAnalytics();
    void onReturnToDashboard();
    void onLogout();

private:
    void showDashboard();

    LoginWindow*     m_loginWindow     = nullptr;
    DashboardWindow* m_dashboardWindow = nullptr;
    BudgetWindow*    m_budgetWindow    = nullptr;

    User m_currentUser;
};