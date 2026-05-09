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

class AnalyticsWindow : public BaseWindow {
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

    QLabel *m_totalLabel, *m_highestCatLabel, *m_dailyAvgLabel;
    QPushButton *m_weekBtn, *m_monthBtn, *m_yearBtn;

    QChart* m_chart = nullptr;
    QtCharts::QPieSeries* m_series = nullptr;
    QtCharts::QChartView* m_chartView = nullptr;

    QFrame* m_breakdownPanel = nullptr;
    QVBoxLayout* m_breakdownLayout = nullptr;

    void buildHeader(QVBoxLayout* root);
    void buildSummaryRow(QVBoxLayout* root);
    void buildBottomRow(QVBoxLayout* root);
    void buildPieChart(QVBoxLayout* leftLayout);
    void buildBreakdownPanel(QVBoxLayout* rightLayout);

    void loadTransactions();
    void refreshAll();
    QMap<QString, double> getCategoryTotals() const;
    double getTotalSpending() const;
    void updateSummaryCards();
    void updatePieChart();
    void updateBreakdownPanel();
    void setPeriod(const QString& period);
    QColor getColorForCategory(const QString& cat) const;

private slots:
    void onWeekClicked();
    void onMonthClicked();
    void onYearClicked();
    void onBackClicked();
};