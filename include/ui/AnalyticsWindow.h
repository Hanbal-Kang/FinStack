#pragma once
#include "ui/BaseWindow.h"
#include "models/User.h"
#include "models/Transaction.h"
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMap>
#include <vector>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>

class AnalyticsWindow : public BaseWindow
{
    Q_OBJECT

public:
    explicit AnalyticsWindow(const User& user, QWidget* parent = nullptr);
    ~AnalyticsWindow() override = default;

signals:
    void backToDashboard();

protected:
    void setupUI() override;
    void applyStyles() override;

private:
    User m_user;
    std::vector<Transaction> m_transactions;
    QString m_currentPeriod;

    // summary card labels
    QLabel* m_totalLabel      = nullptr;
    QLabel* m_highestCatLabel = nullptr;
    QLabel* m_dailyAvgLabel   = nullptr;

    //time period buttons
    QPushButton* m_weekBtn  = nullptr;
    QPushButton* m_monthBtn = nullptr;
    QPushButton* m_yearBtn  = nullptr;
    QPushButton* m_backBtn  = nullptr;

    // pie chart
    QChart*     m_chart     = nullptr;
    QPieSeries* m_series    = nullptr;
    QChartView* m_chartView = nullptr;

    // right panel
    QFrame* m_breakdownPanel = nullptr;

    // setup
    void setupHeaderSection();
    void setupSummaryCards();
    void setupPieChart();
    void setupBreakdownPanel();

    // data
    void loadTransactions();
    void refreshAll();

    // calculations
    QMap<QString, double> getCategoryTotals();
    double getTotalSpending();

    // UI update
    void updateSummaryCards();
    void updatePieChart();
    void updateBreakdownPanel();

    QColor getColorForCategory(const QString& cat);

private slots:
    void onWeekClicked();
    void onMonthClicked();
    void onYearClicked();
    void onBackClicked();
};