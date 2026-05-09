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
#include <QPainter>
#include <vector>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
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
    void setupUI()     override;
    void applyStyles() override;

private:
    User                     m_user;
    std::vector<Transaction> m_transactions;
    QString                  m_currentPeriod;

    //summary card labels
    QLabel* m_totalLabel      = nullptr;
    QLabel* m_highestCatLabel = nullptr;
    QLabel* m_dailyAvgLabel   = nullptr;

    //period toggle buttons
    QPushButton* m_weekBtn  = nullptr;
    QPushButton* m_monthBtn = nullptr;
    QPushButton* m_yearBtn  = nullptr;
    QPushButton* m_backBtn  = nullptr;

    //pie chart
    QChart*     m_chart     = nullptr;
    QPieSeries* m_series    = nullptr;
    QChartView* m_chartView = nullptr;

    //right panel
    QFrame*      m_breakdownPanel = nullptr;
    QVBoxLayout* m_breakdownLayout = nullptr;

    //layouts
    QHBoxLayout* m_headerRow  = nullptr;
    QHBoxLayout* m_summaryRow = nullptr;

    //setup helpers (called once from setupUI)
    void buildHeaderRow(QVBoxLayout* root);
    void buildSummaryRow(QVBoxLayout* root);
    void buildPieChart(QVBoxLayout* leftLayout);
    void buildBreakdownPanel(QVBoxLayout* rightLayout);

    //data helpers
    void loadTransactions();
    void refreshAll();

    QMap<QString, double> getCategoryTotals() const;
    double                getTotalSpending()  const;

    //UI update helpers
    void updateSummaryCards();
    void updatePieChart();
    void updateBreakdownPanel();

    //period-toggle helper
    void setPeriod(const QString& period);   // sets names + repolishes + reloads

    QColor getColorForCategory(const QString& cat) const;

private slots:
    void onWeekClicked();
    void onMonthClicked();
    void onYearClicked();
    void onBackClicked();
};