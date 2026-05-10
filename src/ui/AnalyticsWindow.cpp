#include "ui/AnalyticsWindow.h"
#include "services/TransactionService.h"
#include "ui/DashboardWindow.h"
#include <algorithm>
#include <QDate>
#include <QStyle>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>

//  Constructor
AnalyticsWindow::AnalyticsWindow(const User& user, QWidget* parent)
    : BaseWindow(parent), m_user(user), m_currentPeriod("This Month")
{
    // geometry before initialize() so the window doesn't resize mid-construction
    setupWindowGeometry(1200, 720);
    initialize();
}

//  setupUI  — builds the entire layout

void AnalyticsWindow::setupUI()
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(24, 16, 24, 16);
    mainLayout->setSpacing(16);

    // header (back button + period toggles) added to mainLayout inside helper
    buildHeaderRow(mainLayout);

    // three summary cards added to mainLayout inside helper
    buildSummaryRow(mainLayout);

    // bottom two panels
    QHBoxLayout* bottomRow = new QHBoxLayout();
    bottomRow->setSpacing(16);

    // left card: pie chart
    QFrame* leftCard = new QFrame();
    leftCard->setObjectName("panelCard");
    QVBoxLayout* leftLayout = new QVBoxLayout(leftCard);
    leftLayout->setContentsMargins(20, 20, 20, 20);
    leftLayout->setSpacing(12);

    QLabel* chartTitle = new QLabel("Spending Distribution");
    chartTitle->setObjectName("sectionTitle");
    leftLayout->addWidget(chartTitle);
    buildPieChart(leftLayout);     // appends m_chartView to leftLayout

    // right card: category breakdown
    QFrame* rightCard = new QFrame();
    rightCard->setObjectName("panelCard");
    QVBoxLayout* rightLayout = new QVBoxLayout(rightCard);
    rightLayout->setContentsMargins(20, 20, 20, 20);
    rightLayout->setSpacing(12);

    QLabel* breakdownTitle = new QLabel("Category Breakdown");
    breakdownTitle->setObjectName("sectionTitle");
    rightLayout->addWidget(breakdownTitle);
    buildBreakdownPanel(rightLayout);  // appends m_breakdownPanel to rightLayout

    bottomRow->addWidget(leftCard,   5);
    bottomRow->addWidget(rightCard,  4);
    mainLayout->addLayout(bottomRow);

    // load data and populate everything
    loadTransactions();
    refreshAll();
}

void AnalyticsWindow::applyStyles()
{
    loadGlobalStylesheet();
}

//helpers
void AnalyticsWindow::buildHeaderRow(QVBoxLayout* root)
{
    QHBoxLayout* row = new QHBoxLayout();
    row->setSpacing(8);

    m_backBtn = new QPushButton("← Back");
    m_backBtn->setObjectName("navIconBtn");
    m_backBtn->setCursor(Qt::PointingHandCursor);
    connect(m_backBtn, &QPushButton::clicked, this, &AnalyticsWindow::onBackClicked);

    m_weekBtn  = new QPushButton("This Week");
    m_monthBtn = new QPushButton("This Month");
    m_yearBtn  = new QPushButton("This Year");

    m_weekBtn ->setObjectName("periodBtn");
    m_monthBtn->setObjectName("periodBtnActive");   // default selection
    m_yearBtn ->setObjectName("periodBtn");

    m_weekBtn ->setCursor(Qt::PointingHandCursor);
    m_monthBtn->setCursor(Qt::PointingHandCursor);
    m_yearBtn ->setCursor(Qt::PointingHandCursor);

    connect(m_weekBtn,  &QPushButton::clicked, this, &AnalyticsWindow::onWeekClicked);
    connect(m_monthBtn, &QPushButton::clicked, this, &AnalyticsWindow::onMonthClicked);
    connect(m_yearBtn,  &QPushButton::clicked, this, &AnalyticsWindow::onYearClicked);

    row->addWidget(m_backBtn);
    row->addStretch();
    row->addWidget(m_weekBtn);
    row->addWidget(m_monthBtn);
    row->addWidget(m_yearBtn);

    root->addLayout(row);
}

void AnalyticsWindow::buildSummaryRow(QVBoxLayout* root)
{
    QHBoxLayout* row = new QHBoxLayout();
    row->setSpacing(12);

    auto makeCard = [&](const QString& titleText, QLabel*& valueLabel,
                        const QString& valueName) {
        QFrame* card = new QFrame();
        card->setObjectName("summaryCard");

        QVBoxLayout* vl = new QVBoxLayout(card);
        vl->setContentsMargins(16, 14, 16, 14);
        vl->setSpacing(6);

        QLabel* title = new QLabel(titleText);
        title->setObjectName("summaryTitle");

        valueLabel = new QLabel("Rs 0");
        valueLabel->setObjectName(valueName);

        vl->addWidget(title);
        vl->addWidget(valueLabel);
        return card;
    };

    row->addWidget(makeCard("Total Spending",    m_totalLabel,      "summaryValue"));
    row->addWidget(makeCard("Top Category",      m_highestCatLabel, "summaryValueLarge"));
    row->addWidget(makeCard("Daily Average",     m_dailyAvgLabel,   "summaryValue"));

    root->addLayout(row);
}

void AnalyticsWindow::buildPieChart(QVBoxLayout* leftLayout)
{
    m_series = new QPieSeries();
    m_series->setHoleSize(0.5);   // donut shape

    m_chart = new QChart();
    m_chart->addSeries(m_series);
    m_chart->setBackgroundVisible(false);
    m_chart->legend()->hide();
    m_chart->setMargins(QMargins(0, 0, 0, 0));

    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setMinimumHeight(260);

    leftLayout->addWidget(m_chartView);
}

void AnalyticsWindow::buildBreakdownPanel(QVBoxLayout* rightLayout)
{
    QScrollArea* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    m_breakdownPanel = new QFrame();
    m_breakdownPanel->setObjectName("breakdownPanel");

    m_breakdownLayout = new QVBoxLayout(m_breakdownPanel);
    m_breakdownLayout->setContentsMargins(0, 0, 0, 0);
    m_breakdownLayout->setSpacing(8);
    m_breakdownLayout->addStretch();  // pushes cards to the top initially

    scroll->setWidget(m_breakdownPanel);
    rightLayout->addWidget(scroll, 1);
}

//Data

void AnalyticsWindow::loadTransactions()
{
    TransactionService service;
    std::vector<Transaction> all = service.getAllByUser(m_user.get_id());

    QDate today = QDate::currentDate();
    QDate startDate;
    QDate endDate = today;

    if (m_currentPeriod == "This Week") {
        // Monday of the current week. Qt: dayOfWeek() returns 1=Mon..7=Sun
        startDate = today.addDays(-(today.dayOfWeek() - 1));
    }
    else if (m_currentPeriod == "This Year") {
        // Jan 1 of the current year
        startDate = QDate(today.year(), 1, 1);
    }
    else {
        // "This Month" — first day of the current month
        startDate = QDate(today.year(), today.month(), 1);
    }

    m_transactions.clear();
    for (const auto& tx : all)
    {
        QDate txDate = tx.get_transac_date().date();
        if (txDate >= startDate && txDate <= endDate)
            m_transactions.push_back(tx);
    }
}

void AnalyticsWindow::refreshAll()
{
    updateSummaryCards();
    updatePieChart();
    updateBreakdownPanel();
}

QMap<QString, double> AnalyticsWindow::getCategoryTotals() const
{
    QMap<QString, double> totals;
    for (const auto& tx : m_transactions)
        if (tx.get_type() == Transaction::Expense)
            totals[tx.get_category()] += tx.get_amount();
    return totals;
}

double AnalyticsWindow::getTotalSpending() const
{
    double total = 0.0;
    for (const auto& tx : m_transactions)
        if (tx.get_type() == Transaction::Expense)
            total += tx.get_amount();
    return total;
}


// UI update
void AnalyticsWindow::updateSummaryCards()
{
    double total = getTotalSpending();
    QMap<QString, double> cats = getCategoryTotals();

    m_totalLabel->setText("Rs " + QString::number(total, 'f', 0));

    // highest-spending category
    QString highestCat = "None";
    double  highestAmt = 0.0;
    for (auto it = cats.cbegin(); it != cats.cend(); ++it)
    {
        if (it.value() > highestAmt)
        {
            highestAmt = it.value();
            highestCat = it.key();
        }
    }
    m_highestCatLabel->setText(highestCat);

    // Days elapsed in the current period (up to today, not the full period length)
    QDate today = QDate::currentDate();
    QDate startDate;

    if (m_currentPeriod == "This Week")
        startDate = today.addDays(-(today.dayOfWeek() - 1));
    else if (m_currentPeriod == "This Year")
        startDate = QDate(today.year(), 1, 1);
    else
        startDate = QDate(today.year(), today.month(), 1);

    int days = startDate.daysTo(today) + 1;   // +1 so today counts
    double avg = (days > 0) ? total / days : 0.0;
    m_dailyAvgLabel->setText("Rs " + QString::number(avg, 'f', 0));
}

void AnalyticsWindow::updatePieChart()
{
    m_series->clear();
    QMap<QString, double> cats = getCategoryTotals();
    for (auto it = cats.cbegin(); it != cats.cend(); ++it)
    {
        QPieSlice* slice = m_series->append(it.key(), it.value());
        slice->setColor(getColorForCategory(it.key()));
        slice->setBorderColor(Qt::transparent);
    }
}

void AnalyticsWindow::updateBreakdownPanel()
{
    // remove all old rows (skip the trailing stretch at the end)
    while (m_breakdownLayout->count() > 1)
    {
        QLayoutItem* item = m_breakdownLayout->takeAt(0);
        if (item->widget())
            delete item->widget();
        delete item;
    }

    QMap<QString, double> cats  = getCategoryTotals();
    double                total = getTotalSpending();

    // copy (bubble sort: highest first)
    QVector<QPair<QString, double>> list;
    for (auto it = cats.cbegin(); it != cats.cend(); ++it)
        list.append(qMakePair(it.key(), it.value()));

    std::sort(list.begin(), list.end(),
              [](const QPair<QString,double>& a, const QPair<QString,double>& b) {
                  return a.second > b.second;   // descending
              });

    for (int i = 0; i < list.size(); ++i)
    {
        const QString& cat = list[i].first;
        double         amt = list[i].second;
        double         pct = (total > 0) ? (amt / total * 100.0) : 0.0;

        QFrame* row = new QFrame();
        row->setObjectName("breakdownRow");

        QVBoxLayout* rowLayout = new QVBoxLayout(row);
        rowLayout->setContentsMargins(12, 8, 12, 8);
        rowLayout->setSpacing(4);

        QHBoxLayout* topLine = new QHBoxLayout();
        QLabel* nameLabel = new QLabel(cat);
        nameLabel->setObjectName("catName");
        QLabel* amtLabel  = new QLabel("Rs " + QString::number(amt, 'f', 0));
        amtLabel->setObjectName("catAmount");
        topLine->addWidget(nameLabel);
        topLine->addStretch();
        topLine->addWidget(amtLabel);
        rowLayout->addLayout(topLine);

        QLabel* pctLabel = new QLabel(
            "#" + QString::number(i + 1) + "  " +
            QString::number(pct, 'f', 1) + "%");
        pctLabel->setObjectName("catPercent");
        pctLabel->setStyleSheet("color: " + getColorForCategory(cat).name() + ";");
        rowLayout->addWidget(pctLabel);


        m_breakdownLayout->insertWidget(m_breakdownLayout->count() - 1, row);
    }
}

//Period toggle helper

void AnalyticsWindow::setPeriod(const QString& period)
{
    m_currentPeriod = period;

    auto repolish = [](QWidget* w, const QString& name) {
        w->setObjectName(name);
        w->style()->unpolish(w);
        w->style()->polish(w);
        w->update();
    };

    repolish(m_weekBtn,  period == "This Week"  ? "periodBtnActive" : "periodBtn");
    repolish(m_monthBtn, period == "This Month" ? "periodBtnActive" : "periodBtn");
    repolish(m_yearBtn,  period == "This Year"  ? "periodBtnActive" : "periodBtn");

    loadTransactions();
    refreshAll();
}

//  Slots

void AnalyticsWindow::onWeekClicked()  { setPeriod("This Week");  }
void AnalyticsWindow::onMonthClicked() { setPeriod("This Month"); }
void AnalyticsWindow::onYearClicked()  { setPeriod("This Year");  }

//Change : HANBAL
void AnalyticsWindow::onBackClicked()
{
    DashboardWindow* dashboard = new DashboardWindow(m_user, nullptr);
    dashboard->show();
    close();
}


//  Colour map

QColor AnalyticsWindow::getColorForCategory(const QString& cat) const
{
    if (cat == "Tuition")       return QColor(0x5B8EF0);
    if (cat == "Rent")          return QColor(0x9B5CF0);
    if (cat == "Food")          return QColor(0xF05B5B);
    if (cat == "Transport")     return QColor(0xF0B429);
    if (cat == "Entertainment") return QColor(0xF05BA0);
    return QColor(0x7B8FA6);   // Other
}