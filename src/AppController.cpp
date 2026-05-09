#include "AppController.h"
#include "ui/LoginWindow.h"
#include "ui/DashboardWindow.h"
#include "ui/BudgetWindow.h"

// Add these as teammates finish their windows:
// #include "ui/TransactionWindow.h"
// #include "ui/AnalyticsWindow.h"

AppController::AppController(QObject* parent) : QObject(parent) {}

AppController::~AppController()
{
    delete m_budgetWindow;
    delete m_dashboardWindow;
    delete m_loginWindow;
}

// ─── Entry point ─────────────────────────────────────────────────────────────
void AppController::start()
{
    m_loginWindow = new LoginWindow();
    connect(m_loginWindow, &LoginWindow::loginSucceeded,
            this,          &AppController::onLoginSucceeded);
    m_loginWindow->show();
}

// ─── Login ────────────────────────────────────────────────────────────────────
void AppController::onLoginSucceeded(const User& user)
{
    m_currentUser = user;
    m_loginWindow->hide();
    showDashboard();
}

// ─── Dashboard ────────────────────────────────────────────────────────────────
void AppController::showDashboard()
{
    if (!m_dashboardWindow) {
        m_dashboardWindow = new DashboardWindow(m_currentUser);

        connect(m_dashboardWindow, &DashboardWindow::navigateToBudget,
                this, &AppController::onNavigateToBudget);
        connect(m_dashboardWindow, &DashboardWindow::navigateToWithdraw,
                this, &AppController::onNavigateToWithdraw);
        connect(m_dashboardWindow, &DashboardWindow::navigateToDeposit,
                this, &AppController::onNavigateToDeposit);
        connect(m_dashboardWindow, &DashboardWindow::navigateToHistory,
                this, &AppController::onNavigateToHistory);
        connect(m_dashboardWindow, &DashboardWindow::navigateToAnalytics,
                this, &AppController::onNavigateToAnalytics);
        connect(m_dashboardWindow, &DashboardWindow::logoutRequested,
                this, &AppController::onLogout);
    }

    m_dashboardWindow->refreshDashboard();
    m_dashboardWindow->show();
    m_dashboardWindow->raise();
    m_dashboardWindow->activateWindow();
}

// ─── Budget ───────────────────────────────────────────────────────────────────
void AppController::onNavigateToBudget()
{
    if (!m_budgetWindow) {
        m_budgetWindow = new BudgetWindow(m_currentUser);
        connect(m_budgetWindow, &BudgetWindow::navigateBack,
                this, &AppController::onReturnToDashboard);
    }

    m_dashboardWindow->hide();
    m_budgetWindow->refreshBudgets();
    m_budgetWindow->show();
    m_budgetWindow->raise();
    m_budgetWindow->activateWindow();
}

// ─── Stubs — wire up when teammates finish their windows ─────────────────────
void AppController::onNavigateToWithdraw()
{
    // m_dashboardWindow->hide();
    // if (!m_withdrawWindow) {
    //     m_withdrawWindow = new TransactionWindow(m_currentUser, TransactionWindow::Withdraw);
    //     connect(m_withdrawWindow, &TransactionWindow::navigateBack,
    //             this, &AppController::onReturnToDashboard);
    // }
    // m_withdrawWindow->show();
}

void AppController::onNavigateToDeposit()
{
    // m_dashboardWindow->hide();
    // if (!m_depositWindow) {
    //     m_depositWindow = new TransactionWindow(m_currentUser, TransactionWindow::Deposit);
    //     connect(m_depositWindow, &TransactionWindow::navigateBack,
    //             this, &AppController::onReturnToDashboard);
    // }
    // m_depositWindow->show();
}

void AppController::onNavigateToHistory()
{
    // m_dashboardWindow->hide();
    // if (!m_historyWindow) {
    //     m_historyWindow = new TransactionWindow(m_currentUser, TransactionWindow::History);
    //     connect(m_historyWindow, &TransactionWindow::navigateBack,
    //             this, &AppController::onReturnToDashboard);
    // }
    // m_historyWindow->show();
}

void AppController::onNavigateToAnalytics()
{
    // m_dashboardWindow->hide();
    // if (!m_analyticsWindow) {
    //     m_analyticsWindow = new AnalyticsWindow(m_currentUser);
    //     connect(m_analyticsWindow, &AnalyticsWindow::navigateBack,
    //             this, &AppController::onReturnToDashboard);
    // }
    // m_analyticsWindow->show();
}

// ─── Return to dashboard ──────────────────────────────────────────────────────
void AppController::onReturnToDashboard()
{
    if (m_budgetWindow && m_budgetWindow->isVisible())
        m_budgetWindow->hide();
    // Hide other windows here as they are built

    showDashboard();
}

// ─── Logout ───────────────────────────────────────────────────────────────────
void AppController::onLogout()
{
    // Destroy windows so they re-read fresh data on next login
    delete m_dashboardWindow; m_dashboardWindow = nullptr;
    delete m_budgetWindow;    m_budgetWindow    = nullptr;

    m_currentUser = User();

    m_loginWindow->show();
    m_loginWindow->raise();
    m_loginWindow->activateWindow();
}