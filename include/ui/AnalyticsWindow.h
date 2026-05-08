#pragma once
#include "ui/BaseWindow.h"
#include "models/User.h"
#include "models/Transaction.h"
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QChart>
#include <QVector>

class AnalyticsWindow : public BaseWindow
{
    Q_OBJECT

public:
    explicit AnalyticsWindow(const User& currentUser, QWidget* parent = nullptr);
    ~AnalyticsWindow() override = default;

    // Reload all analytics data (call after new transactions)
    void refreshData();

signals:
    void backToDashboard();

protected:
    void setupUI() override;
    void applyStyles() override;

private:
    // data
    User user;
    QVector<Transaction> transactions; // loaded from DB for selected period


    // period filter
    QString selectedPeriod;

    //summary cards(top panel)
    QLabel* totalSpendingLabel = nullptr; // total spendings
    QLabel* totalSubtitleLabel = nullptr; // "Across N categories"
    QLabel* highestCatLabel = nullptr; // category
    QLabel* highestAmtLabel = nullptr; // contribution
    QLabel* dailyAvgLabel = nullptr; // daily average
    QLabel* dailySubtitleLabel = nullptr; // daily avg subtitle

    //Period buttons
    QPushButton* weekBtn = nullptr;
    QPushButton* monthBtn = nullptr;
    QPushButton* yearBtn = nullptr;

    //Pie chart
    QChart* chart = nullptr;
    QPieSeries* pieSeries = nullptr;
    QChartView* chartView = nullptr;

    // Legend labels (below pie)
    // Each entry: coloured dot + category name + percentage
    QFrame* legendFrame = nullptr;

    //Category breakdown list (right panel)
    // Each row: icon, name, rank label, dollar amount,
    // percentage label, progress bar
    QFrame* breakdownFrame = nullptr;
    QScrollArea* breakdownScroll = nullptr;

    //Back button
    QPushButton* backBtn = nullptr;

    //Private helper functions
    void setupHeaderBar();
    void setupSummaryCards();
    void setupPeriodToggle();
    void setupPieChart();
    void setupLegend();
    void setupCategoryBreakdown();

    // Aggregates transactions into per-category totals.
    // Returns map of { categoryName -> total amount }
    QMap<QString, double> computeCategoryTotals() const;

    // Computes total spending across all categories
    double computeTotalSpending() const;

    // Re-renders pie slices, legend, breakdown rows, and
    // summary card values using current transactions data
    void updateChartData();
    void updateSummaryCards(double total, const QMap<QString, double>& totals);
    void updateBreakdownRows(double total, const QMap<QString, double>& totals);
    void updateLegend(double total, const QMap<QString, double>& totals);

    // Builds a single breakdown row widget for one category
    QFrame* buildBreakdownRow(const QString& category, double amount, double percentage,int rank,const QColor& color);

    // Returns a deterministic color for each category name
    QColor colorForCategory(const QString& category) const;

private slots:
    void onPeriodChanged(const QString& period);
    void onBackClicked();
};