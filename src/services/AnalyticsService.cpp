#include "ui/AnalyticsWindow.h"
#include "services/TransactionService.h"
#include <QDate>

AnalyticsWindow::AnalyticsWindow(const User& user, QWidget* parent)
    : BaseWindow(parent), m_user(user), m_currentPeriod("This Month")
{
    initialize();
}

void AnalyticsWindow::setupUI()
{
    setupWindowGeometry(1200, 720);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* mainLayout = new QVBoxLayout(central);
    mainLayout->setContentsMargins(24, 16, 24, 16);
    mainLayout->setSpacing(16);

    setupHeaderSection();
    setupSummaryCards();

    QHBoxLayout* bottomRow = new QHBoxLayout();

    // left side: pie chart
    QFrame* leftCard = new QFrame();
    leftCard->setObjectName("panelCard");
    QVBoxLayout* leftLayout = new QVBoxLayout(leftCard);
    leftLayout->setContentsMargins(20, 20, 20, 20);
    QLabel* chartTitle = new QLabel("Spending Distribution");
    chartTitle->setObjectName("sectionTitle");
    leftLayout->addWidget(chartTitle);
    setupPieChart();
    leftLayout->addWidget(m_chartView);

    // right side: category breakdown
    QFrame* rightCard = new QFrame();
    rightCard->setObjectName("panelCard");
    QVBoxLayout* rightLayout = new QVBoxLayout(rightCard);
    rightLayout->setContentsMargins(20, 20, 20, 20);
    QLabel* breakdownTitle = new QLabel("Category Breakdown");
    breakdownTitle->setObjectName("sectionTitle");
    rightLayout->addWidget(breakdownTitle);
    setupBreakdownPanel();
    rightLayout->addWidget(m_breakdownPanel);

    bottomRow->addWidget(leftCard, 5);
    bottomRow->addWidget(rightCard, 4);
    mainLayout->addLayout(bottomRow);

    loadTransactions();
    refreshAll();
}

void AnalyticsWindow::applyStyles()
{
    loadGlobalStylesheet();
}

void AnalyticsWindow::setupHeaderSection()
{
    m_weekBtn  = new QPushButton("This Week",  this);
    m_monthBtn = new QPushButton("This Month", this);
    m_yearBtn  = new QPushButton("This Year",  this);
    m_backBtn  = new QPushButton("Back",       this);

    // month is selected by default
    m_weekBtn->setObjectName("periodBtn");
    m_monthBtn->setObjectName("periodBtnActive");
    m_yearBtn->setObjectName("periodBtn");

    connect(m_weekBtn,  &QPushButton::clicked, this, &AnalyticsWindow::onWeekClicked);
    connect(m_monthBtn, &QPushButton::clicked, this, &AnalyticsWindow::onMonthClicked);
    connect(m_yearBtn,  &QPushButton::clicked, this, &AnalyticsWindow::onYearClicked);
    connect(m_backBtn,  &QPushButton::clicked, this, &AnalyticsWindow::onBackClicked);
}

void AnalyticsWindow::setupSummaryCards()
{
    // values are filled in later by updateSummaryCards()
    m_totalLabel      = new QLabel("$0", this);
    m_highestCatLabel = new QLabel("None", this);
    m_dailyAvgLabel   = new QLabel("$0", this);

    m_totalLabel->setObjectName("summaryValue");
    m_highestCatLabel->setObjectName("summaryValueLarge");
    m_dailyAvgLabel->setObjectName("summaryValue");
}

void AnalyticsWindow::setupPieChart()
{
    m_series = new QPieSeries();
    m_series->setHoleSize(0.5); // donut shape

    m_chart = new QChart();
    m_chart->addSeries(m_series);
    m_chart->setBackgroundVisible(false);
    m_chart->legend()->hide();
    m_chart->setMargins(QMargins(0, 0, 0, 0));

    m_chartView = new QChartView(m_chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    m_chartView->setMinimumHeight(260);
}

void AnalyticsWindow::setupBreakdownPanel()
{
    m_breakdownPanel = new QFrame(this);
    m_breakdownPanel->setObjectName("breakdownPanel");
    new QVBoxLayout(m_breakdownPanel);
double AnalyticsService::getTotalIncome(int userId) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT SUM(amount) FROM transactions "
        "WHERE type = 'income' AND account_id IN "
        "(SELECT id FROM accounts WHERE user_id = ?)"
        );
    q.addBindValue(userId);
    if (!DatabaseManager::instance().executePrepared(q) || !q.next()) return 0.0;
    QVariant val = q.value(0);
    return val.isNull() ? 0.0 : val.toDouble();
}

void AnalyticsWindow::loadTransactions()
{
    TransactionService service;
    std::vector<Transaction> all = service.getAllByUser(m_user.get_user_id());

    QDate today = QDate::currentDate();
    QDate startDate;

    if (m_currentPeriod == "This Week")
        startDate = today.addDays(-7);
    else if (m_currentPeriod == "This Year")
        startDate = today.addDays(-365);
    else
        startDate = today.addDays(-30);

    // filter transactions to only include ones in the selected period
    m_transactions.clear();
    for (int i = 0; i < all.size(); i++)
    {
        QDate txDate = all[i].get_transac_date().date();
        if (txDate >= startDate && txDate <= today)
            m_transactions.push_back(all[i]);
    }
}

void AnalyticsWindow::refreshAll()
{
    updateSummaryCards();
    updatePieChart();
    updateBreakdownPanel();
}

QMap<QString, double> AnalyticsWindow::getCategoryTotals()
{
    QMap<QString, double> totals;

    for (int i = 0; i < m_transactions.size(); i++)
    {
        if (m_transactions[i].get_type() == Transaction::Expense)
            totals[m_transactions[i].get_category()] += m_transactions[i].get_amount();
    }

    return totals;
}

double AnalyticsWindow::getTotalSpending()
{
    double total = 0.0;

    for (int i = 0; i < m_transactions.size(); i++)
    {
        if (m_transactions[i].get_type() == Transaction::Expense)
            total += m_transactions[i].get_amount();
    }

    return total;
}

void AnalyticsWindow::updateSummaryCards()
{
    double total = getTotalSpending();
    QMap<QString, double> cats = getCategoryTotals();

    m_totalLabel->setText("$" + QString::number(total, 'f', 0));

    // find the category with the highest spending
    QString highestCat = "None";
    double highestAmt  = 0.0;

    QMap<QString, double>::iterator it;
    for (it = cats.begin(); it != cats.end(); it++)
    {
        if (it.value() > highestAmt)
        {
            highestAmt = it.value();
            highestCat = it.key();
        }
    }
    m_highestCatLabel->setText(highestCat);

    // calculate daily average based on period length
    int days = 30;
    if (m_currentPeriod == "This Week") days = 7;
    if (m_currentPeriod == "This Year") days = 365;

    double avg = total / days;
    m_dailyAvgLabel->setText("$" + QString::number(avg, 'f', 0));
}

void AnalyticsWindow::updatePieChart()
{
    m_series->clear();

    QMap<QString, double> cats = getCategoryTotals();

    QMap<QString, double>::iterator it;
    for (it = cats.begin(); it != cats.end(); it++)
    {
        QPieSlice* slice = m_series->append(it.key(), it.value());
        slice->setColor(getColorForCategory(it.key()));
        slice->setBorderColor(Qt::transparent);
    }
}

void AnalyticsWindow::updateBreakdownPanel()
{
    // remove all old rows before rebuilding
    QLayout* layout = m_breakdownPanel->layout();
    while (layout->count() > 0)
    {
        QLayoutItem* item = layout->takeAt(0);
        delete item->widget();
        delete item;
    }

    QMap<QString, double> cats = getCategoryTotals();
    double total = getTotalSpending();

    // copy map into a list so we can sort it
    QVector<QPair<QString, double>> list;
    QMap<QString, double>::iterator it;
    for (it = cats.begin(); it != cats.end(); it++)
        list.append(qMakePair(it.key(), it.value()));

    // bubble sort(highest spending first)
    for (int i = 0; i < list.size() - 1; i++)
    {
        for (int j = 0; j < list.size() - i - 1; j++)
        {
            if (list[j].second < list[j + 1].second)
            {
                QPair<QString, double> temp = list[j];
                list[j] = list[j + 1];
                list[j + 1] = temp;
            }
        }
    }

    QVBoxLayout* panelLayout = qobject_cast<QVBoxLayout*>(m_breakdownPanel->layout());

    for (int i = 0; i < list.size(); i++)
    {
        QString cat = list[i].first;
        double amt  = list[i].second;
        double pct  = 0.0;
        if (total > 0)
            pct = (amt / total) * 100.0;

        QFrame* row = new QFrame();
        row->setObjectName("breakdownRow");

        QVBoxLayout* rowLayout = new QVBoxLayout(row);
        rowLayout->setContentsMargins(12, 8, 12, 8);

        QHBoxLayout* topLine = new QHBoxLayout();
        QLabel* nameLabel = new QLabel(cat);
        nameLabel->setObjectName("catName");
        QLabel* amtLabel = new QLabel("$" + QString::number(amt, 'f', 0));
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

        panelLayout->addWidget(row);
    }
}

QColor AnalyticsWindow::getColorForCategory(const QString& cat)
{
    if (cat == "Tuition")       return QColor("#5B8EF0");
    if (cat == "Rent")          return QColor("#9B5CF0");
    if (cat == "Food")          return QColor("#F05B5B");
    if (cat == "Transport")     return QColor("#F0B429");
    if (cat == "Entertainment") return QColor("#F05BA0");
    return QColor("#7B8FA6");
}

void AnalyticsWindow::onWeekClicked()
{
    m_currentPeriod = "This Week";
    m_weekBtn->setObjectName("periodBtnActive");
    m_monthBtn->setObjectName("periodBtn");
    m_yearBtn->setObjectName("periodBtn");
    loadTransactions();
    refreshAll();
}

void AnalyticsWindow::onMonthClicked()
{
    m_currentPeriod = "This Month";
    m_weekBtn->setObjectName("periodBtn");
    m_monthBtn->setObjectName("periodBtnActive");
    m_yearBtn->setObjectName("periodBtn");
    loadTransactions();
    refreshAll();
}

void AnalyticsWindow::onYearClicked()
{
    m_currentPeriod = "This Year";
    m_weekBtn->setObjectName("periodBtn");
    m_monthBtn->setObjectName("periodBtn");
    m_yearBtn->setObjectName("periodBtnActive");
    loadTransactions();
    refreshAll();
}

void AnalyticsWindow::onBackClicked()
{
    emit backToDashboard();
    close();
double AnalyticsService::getCategoryExpense(int userId, const QString& category) {
    QSqlQuery q = DatabaseManager::instance().prepare(
        "SELECT SUM(amount) FROM transactions "
        "WHERE type = 'expense' AND category = ? "
        "AND account_id IN (SELECT id FROM accounts WHERE user_id = ?)"
        );
    q.addBindValue(category);
    q.addBindValue(userId);
    if (!DatabaseManager::instance().executePrepared(q) || !q.next()) return 0.0;
    QVariant val = q.value(0);
    return val.isNull() ? 0.0 : val.toDouble();
}