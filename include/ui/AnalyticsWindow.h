#pragma once
#include "ui/BaseWindow.h"
#include "models/User.h"
#include "models/Transaction.h"
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVector>
#include <QMap>
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
    QVector<Transaction> m_transactions;
    QString m_currentPeriod; // time

    // summary cards at the top
    QLabel* m_totalLabel     = nullptr;
    QLabel* m_highestCatLabel = nullptr;
    QLabel* m_dailyAvgLabel  = nullptr;

    // period toggle buttons
    QPushButton* m_weekBtn  = nullptr;
    QPushButton* m_monthBtn = nullptr;
    QPushButton* m_yearBtn  = nullptr;

    // pie chart stuff
    QChart*     m_chart     = nullptr;
    QPieSeries* m_series    = nullptr;
    QChartView* m_chartView = nullptr;

    // right side breakdown panel
    QFrame* m_breakdownPanel = nullptr;

    QPushButton* m_backBtn = nullptr;

    // helper functions
    void setupHeaderSection();
    void setupSummaryCards();
    void setupPieChart();
    void setupBreakdownPanel();

    void loadTransactions();
    void refreshAll();

    // "Food" -> 850.0, "Rent" -> 2500.0
    QMap<QString, double> getCategoryTotals();
    double getTotalSpending();

    void updatePieChart();
    void updateSummaryCards();
    void updateBreakdownPanel();

    QColor getColorForCategory(const QString& cat);

private slots:
    void onWeekClicked();
    void onMonthClicked();
    void onYearClicked();
    void onBackClicked();
};