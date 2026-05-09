#include "ui/DashboardWindow.h"
#include "ui/DepositWindow.h"
#include "ui/WithdrawWindow.h"

#include <QScreen>
#include <QScrollArea>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QDate>
#include <QStyle>
#include <utility>

//  Constructor
DashboardWindow::DashboardWindow(const User& user, QWidget* parent)
    : BaseWindow(parent), m_user(user)
{
    setWindowTitle("FinStack — Dashboard");
    setupWindowGeometry(1100, 700);

    loadData();
    initialize();
}

void DashboardWindow::setupUI()
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    QFrame* navbar = new QFrame(central);
    navbar->setObjectName("navbar");
    navbar->setFixedHeight(64);
    buildNavbar(navbar);
    root->addWidget(navbar);

    QScrollArea* scroll = new QScrollArea(central);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setObjectName("pageScroll");

    QWidget* page = new QWidget();
    page->setObjectName("pageBody");
    QVBoxLayout* pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(32, 28, 32, 32);
    pageLayout->setSpacing(14);

    buildHeroCard(pageLayout);
    buildQuickActions(pageLayout);
    buildBottomRow(pageLayout);

    pageLayout->addStretch();
    scroll->setWidget(page);
    root->addWidget(scroll, 1);
}

void DashboardWindow::applyStyles()
{
    loadGlobalStylesheet();
}

void DashboardWindow::refreshDashboard()
{
    loadData();

    if (m_balanceLabel)
        m_balanceLabel->setText(QString("$%1").arg(totalBalance(), 0, 'f', 2));
    if (m_incomeLabel)
        m_incomeLabel->setText(QString("$%1").arg(monthlyIncome(), 0, 'f', 2));
    if (m_expenseLabel)
        m_expenseLabel->setText(QString("$%1").arg(monthlyExpenses(), 0, 'f', 2));

    if (m_txLayout) {
        while (m_txLayout->count() > 1) {
            QLayoutItem* item = m_txLayout->takeAt(0);
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
        int shown = qMin(m_transactions.size(), 10);
        for (int i = 0; i < shown; i++)
            m_txLayout->insertWidget(i, makeTransactionRow(m_transactions[i]));
        if (shown == 0) {
            QLabel* empty = new QLabel("No transactions yet");
            empty->setObjectName("emptyLabel");
            empty->setAlignment(Qt::AlignCenter);
            m_txLayout->insertWidget(0, empty);
        }
    }

    if (m_budgetLayout) {
        while (m_budgetLayout->count() > 1) {
            QLayoutItem* item = m_budgetLayout->takeAt(0);
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
        QMap<QString, double> spent;
        for (const auto& tx : std::as_const(m_transactions)) {
            if (tx.get_type() == Transaction::Expense)
                spent[tx.get_category()] += tx.get_amount();
        }
        int row = 0;
        for (const auto& b : std::as_const(m_budgets)) {
            // CHANGE: convert enum to QString before using as map key and passing to makeBudgetBar
            QString catName = Budget::categoryToString(b.get_category());
            double s = spent.value(catName, 0.0);
            m_budgetLayout->insertWidget(row++, makeBudgetBar(catName, s, b.get_monthly_limit()));
        }
    }
}

void DashboardWindow::onWithdrawClicked()
{
    WithdrawWindow* w = new WithdrawWindow(m_user, this);
    w->show();
}
void DashboardWindow::onDepositClicked()
{
    DepositWindow* w = new DepositWindow (m_user , this);
    w->show();
}

void DashboardWindow::onBudgetClicked()    { emit navigateToBudget(); }
void DashboardWindow::onAnalyticsClicked() { emit navigateToAnalytics(); }
void DashboardWindow::onHistoryClicked()   { emit navigateToHistory(); }
void DashboardWindow::onProfileClicked()   { emit navigateToProfile(); }

void DashboardWindow::buildNavbar(QWidget* parent)
{
    QHBoxLayout* layout = new QHBoxLayout(parent);
    layout->setContentsMargins(20, 0, 20, 0);
    layout->setSpacing(12);

    QPushButton* backBtn = new QPushButton("←", parent);
    backBtn->setObjectName("navIconBtn");
    backBtn->setFixedSize(36, 36);
    backBtn->setCursor(Qt::PointingHandCursor);
    layout->addWidget(backBtn);

    QHBoxLayout* logoRow = new QHBoxLayout();
    logoRow->setSpacing(12);
    logoRow->setAlignment(Qt::AlignCenter);

    QWidget* iconWidget = new QWidget();
    iconWidget->setFixedSize(44, 44);
    iconWidget->setObjectName("logoIcon");

    QVBoxLayout* barsLayout = new QVBoxLayout(iconWidget);
    barsLayout->setContentsMargins(10, 10, 10, 10);
    barsLayout->setSpacing(4);

    QLabel* bar1 = new QLabel(); bar1->setObjectName("barLight"); bar1->setMaximumWidth(14); bar1->setFixedHeight(5);
    QLabel* bar2 = new QLabel(); bar2->setObjectName("barMid");   bar2->setMaximumWidth(20); bar2->setFixedHeight(5);
    QLabel* bar3 = new QLabel(); bar3->setObjectName("barDark");  bar3->setMaximumWidth(26); bar3->setFixedHeight(5);

    barsLayout->addStretch();
    barsLayout->addWidget(bar1);
    barsLayout->addWidget(bar2);
    barsLayout->addWidget(bar3);

    m_appName = new QLabel("FinStack");
    m_appName->setObjectName("appName");

    logoRow->addWidget(iconWidget);
    logoRow->addWidget(m_appName);

    layout->addLayout(logoRow);

    QString initials = m_user.get_username().isEmpty()
                           ? "?"
                           : m_user.get_username().left(2).toUpper();
    m_avatarLabel = new QLabel(initials, parent);
    m_avatarLabel->setObjectName("avatarLabel");
    m_avatarLabel->setFixedSize(36, 36);
    m_avatarLabel->setAlignment(Qt::AlignCenter);
    m_avatarLabel->setCursor(Qt::PointingHandCursor);
    layout->addWidget(m_avatarLabel);

    connect(backBtn, &QPushButton::clicked, this, &DashboardWindow::logoutRequested);
}

void DashboardWindow::buildHeroCard(QVBoxLayout* layout)
{
    QFrame* hero = new QFrame();
    hero->setObjectName("heroCard");
    hero->setMinimumHeight(200);

    QVBoxLayout* vl = new QVBoxLayout(hero);
    vl->setContentsMargins(0, 32, 0, 28);
    vl->setSpacing(8);
    vl->setAlignment(Qt::AlignHCenter);

    QLabel* titleLbl = new QLabel("TOTAL BALANCE", hero);
    titleLbl->setObjectName("heroTitle");
    titleLbl->setAlignment(Qt::AlignCenter);

    m_balanceLabel = new QLabel(QString("$%1").arg(totalBalance(), 0, 'f', 2), hero);
    m_balanceLabel->setObjectName("heroAmount");
    m_balanceLabel->setAlignment(Qt::AlignCenter);

    QLabel* trend = new QLabel("↑ +1.3%  vs last month", hero);
    trend->setObjectName("heroTrend");
    trend->setAlignment(Qt::AlignCenter);
    trend->setFixedHeight(24);

    QHBoxLayout* cardsRow = new QHBoxLayout();
    cardsRow->setContentsMargins(24, 8, 24, 0);
    cardsRow->setSpacing(16);

    QFrame* incCard = new QFrame(hero);
    incCard->setObjectName("incomeCard");
    QVBoxLayout* incLayout = new QVBoxLayout(incCard);
    incLayout->setContentsMargins(16, 12, 16, 12);
    QHBoxLayout* incHeader = new QHBoxLayout();
    QLabel* incDot = new QLabel(incCard); incDot->setObjectName("cardDot_income"); incDot->setFixedSize(8, 8);
    QLabel* incTitle = new QLabel("Total Income", incCard); incTitle->setObjectName("cardTitle");
    incHeader->addWidget(incDot); incHeader->addWidget(incTitle); incHeader->addStretch();
    m_incomeLabel = new QLabel(QString("$%1").arg(monthlyIncome(), 0, 'f', 2), incCard);
    m_incomeLabel->setObjectName("cardValue");
    incLayout->addLayout(incHeader);
    incLayout->addWidget(m_incomeLabel);

    QFrame* expCard = new QFrame(hero);
    expCard->setObjectName("expenseCard");
    QVBoxLayout* expLayout = new QVBoxLayout(expCard);
    expLayout->setContentsMargins(16, 12, 16, 12);
    QHBoxLayout* expHeader = new QHBoxLayout();
    QLabel* expDot = new QLabel(expCard); expDot->setObjectName("cardDot_expense"); expDot->setFixedSize(8, 8);
    QLabel* expTitle = new QLabel("Total Expenses", expCard); expTitle->setObjectName("cardTitle");
    expHeader->addWidget(expDot); expHeader->addWidget(expTitle); expHeader->addStretch();
    m_expenseLabel = new QLabel(QString("$%1").arg(monthlyExpenses(), 0, 'f', 2), expCard);
    m_expenseLabel->setObjectName("cardValue");
    expLayout->addLayout(expHeader);
    expLayout->addWidget(m_expenseLabel);

    QFrame* netCard = new QFrame(hero);
    netCard->setObjectName("incomeCard");
    QVBoxLayout* netLayout = new QVBoxLayout(netCard);
    netLayout->setContentsMargins(16, 12, 16, 12);
    QLabel* netTitle = new QLabel("Net Balance", netCard); netTitle->setObjectName("cardTitle");
    QLabel* netValue = new QLabel(QString("$%1").arg(monthlyIncome() - monthlyExpenses(), 0, 'f', 2), netCard);
    netValue->setObjectName("cardValue");
    netLayout->addWidget(netTitle);
    netLayout->addWidget(netValue);

    cardsRow->addWidget(incCard, 1);
    cardsRow->addWidget(expCard, 1);
    cardsRow->addWidget(netCard, 1);

    vl->addWidget(titleLbl);
    vl->addWidget(m_balanceLabel);
    vl->addWidget(trend, 0, Qt::AlignHCenter);
    vl->addLayout(cardsRow);

    layout->addWidget(hero);
}

void DashboardWindow::buildQuickActions(QVBoxLayout* layout)
{
    QHBoxLayout* row = new QHBoxLayout();
    row->setSpacing(12);

    struct BtnDef { QString icon; QString label; QString color; };
    QList<BtnDef> defs = {
                          { "↓", "Withdraw",  "#f85149" },
                          { "↑", "Deposit",   "#3fb950" },
                          { "◎", "Budget",    "#f0883e" },
                          { "∿", "Analytics", "#7c8cf8" },
                          { "☰", "History",   "#58a6ff" },
                          };

    for (const auto& d : defs) {
        // Use a real QPushButton so clicks work reliably — style it to look
        // like a card using QSS (objectName "quickBtn")
        QPushButton* btn = new QPushButton();
        btn->setObjectName("quickBtn");
        btn->setCursor(Qt::PointingHandCursor);
        btn->setMinimumHeight(80);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        // Stack icon + label vertically inside the button using a layout
        QVBoxLayout* vl = new QVBoxLayout(btn);
        vl->setContentsMargins(8, 12, 8, 12);
        vl->setSpacing(6);
        vl->setAlignment(Qt::AlignHCenter);

        QLabel* iconLbl = new QLabel(d.icon);
        iconLbl->setObjectName("quickIcon");
        iconLbl->setAlignment(Qt::AlignCenter);
        iconLbl->setStyleSheet(QString("color: %1; background: transparent;").arg(d.color));
        iconLbl->setAttribute(Qt::WA_TransparentForMouseEvents);

        QLabel* label = new QLabel(d.label);
        label->setObjectName("quickLabel");
        label->setAlignment(Qt::AlignCenter);
        label->setAttribute(Qt::WA_TransparentForMouseEvents);

        vl->addWidget(iconLbl);
        vl->addWidget(label);

        QPushButton* clickArea = new QPushButton(btn);
        clickArea->setFlat(true);
        clickArea->setStyleSheet("background:transparent;border:none;");
        clickArea->setGeometry(0, 0, 300, 100);
        clickArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        if      (d.label == "Withdraw")  connect(clickArea, &QPushButton::clicked, this, &DashboardWindow::onWithdrawClicked);
        else if (d.label == "Deposit")   connect(clickArea, &QPushButton::clicked, this, &DashboardWindow::onDepositClicked);
        else if (d.label == "Budget")    connect(clickArea, &QPushButton::clicked, this, &DashboardWindow::onBudgetClicked);
        else if (d.label == "Analytics") connect(clickArea, &QPushButton::clicked, this, &DashboardWindow::onAnalyticsClicked);
        else if (d.label == "History")   connect(clickArea, &QPushButton::clicked, this, &DashboardWindow::onHistoryClicked);

        row->addWidget(btn, 1);
    }

    layout->addLayout(row);
}

void DashboardWindow::buildBottomRow(QVBoxLayout* layout)
{
    QHBoxLayout* row = new QHBoxLayout();
    row->setSpacing(20);

    QFrame* txPanel = new QFrame();
    txPanel->setObjectName("txPanel");

    QVBoxLayout* txOuter = new QVBoxLayout(txPanel);
    txOuter->setContentsMargins(16, 16, 16, 16);
    txOuter->setSpacing(12);

    QHBoxLayout* txHeader = new QHBoxLayout();
    QLabel* txTitle = new QLabel("Transactions", txPanel);
    txTitle->setObjectName("sectionLabel");
    QPushButton* viewAll = new QPushButton("View all", txPanel);
    viewAll->setObjectName("viewAllBtn");
    viewAll->setCursor(Qt::PointingHandCursor);
    connect(viewAll, &QPushButton::clicked, this, &DashboardWindow::onHistoryClicked);
    txHeader->addWidget(txTitle);
    txHeader->addStretch();
    txHeader->addWidget(viewAll);
    txOuter->addLayout(txHeader);

    QScrollArea* txScroll = new QScrollArea(txPanel);
    txScroll->setWidgetResizable(true);
    txScroll->setFrameShape(QFrame::NoFrame);
    txScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* txListWidget = new QWidget();
    m_txLayout = new QVBoxLayout(txListWidget);
    m_txLayout->setContentsMargins(0, 0, 0, 0);
    m_txLayout->setSpacing(8);

    int shown = qMin(m_transactions.size(), 10);
    for (int i = 0; i < shown; i++)
        m_txLayout->addWidget(makeTransactionRow(m_transactions[i]));

    if (shown == 0) {
        QLabel* empty = new QLabel("No transactions yet.\nMake your first deposit!");
        empty->setObjectName("emptyLabel");
        empty->setAlignment(Qt::AlignCenter);
        m_txLayout->addWidget(empty);
    }
    m_txLayout->addStretch();

    txScroll->setWidget(txListWidget);
    txOuter->addWidget(txScroll, 1);

    QFrame* budPanel = new QFrame();
    budPanel->setObjectName("budgetPanel");
    budPanel->setFixedWidth(280);

    QVBoxLayout* budOuter = new QVBoxLayout(budPanel);
    budOuter->setContentsMargins(16, 16, 16, 16);
    budOuter->setSpacing(14);

    QLabel* budTitle = new QLabel("Monthly Budget", budPanel);
    budTitle->setObjectName("sectionLabel");
    budOuter->addWidget(budTitle);

    QWidget* barsWidget = new QWidget(budPanel);
    m_budgetLayout = new QVBoxLayout(barsWidget);
    m_budgetLayout->setContentsMargins(0, 0, 0, 0);
    m_budgetLayout->setSpacing(12);

    // CHANGE: spent map key is now QString (category name) to match transaction's get_category()
    QMap<QString, double> spent;
    for (const auto& tx : std::as_const(m_transactions))
        if (tx.get_type() == Transaction::Expense)
            spent[tx.get_category()] += tx.get_amount();

    if (m_budgets.isEmpty()) {
        QStringList placeholders = { "Food", "Transport", "Rent", "Entertainment" };
        for (const auto& cat : placeholders)
            m_budgetLayout->addWidget(makeBudgetBar(cat, spent.value(cat, 0.0), 0.0));
    } else {
        for (const auto& b : std::as_const(m_budgets)) {
            // CHANGE: convert Budget::Category enum to QString using the new helper
            QString catName = Budget::categoryToString(b.get_category());
            m_budgetLayout->addWidget(makeBudgetBar(catName,
                                                    spent.value(catName, 0.0),
                                                    b.get_monthly_limit()));
        }
    }


    m_budgetLayout->addStretch();
    budOuter->addWidget(barsWidget, 1);

    row->addWidget(txPanel, 1);
    row->addWidget(budPanel);

    layout->addLayout(row, 1);
}

QWidget* DashboardWindow::makeTransactionRow(const Transaction& tx)
{
    QFrame* row = new QFrame();
    row->setObjectName("txRow");
    row->setFixedHeight(60);

    QHBoxLayout* layout = new QHBoxLayout(row);
    layout->setContentsMargins(12, 0, 12, 0);
    layout->setSpacing(10);

    bool isIncome = (tx.get_type() == Transaction::Income);

    QLabel* icon = new QLabel(isIncome ? "↑" : "↓", row);
    icon->setObjectName(isIncome ? "txIconDep" : "txIconExp");
    icon->setFixedSize(30, 30);
    icon->setAlignment(Qt::AlignCenter);

    QVBoxLayout* textCol = new QVBoxLayout();
    textCol->setSpacing(2);

    QString desc = tx.get_description().isEmpty() ? tx.get_category() : tx.get_description();
    QLabel* descLbl = new QLabel(desc, row);
    descLbl->setObjectName("txDesc");

    QLabel* metaLbl = new QLabel(
        tx.get_category() + "  ·  " + tx.get_transac_date().toString("MMM d, yyyy"), row);
    metaLbl->setObjectName("txMeta");

    textCol->addWidget(descLbl);
    textCol->addWidget(metaLbl);

    QString sign = isIncome ? "+" : "-";
    QLabel* amtLbl = new QLabel(
        QString("%1$%2").arg(sign).arg(tx.get_amount(), 0, 'f', 2), row);
    amtLbl->setObjectName(isIncome ? "txAmtPos" : "txAmtNeg");
    amtLbl->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    layout->addWidget(icon);
    layout->addLayout(textCol, 1);
    layout->addWidget(amtLbl);

    return row;
}

QWidget* DashboardWindow::makeBudgetBar(const QString& category, double spent, double limit)
{
    QWidget* w = new QWidget();
    QVBoxLayout* vl = new QVBoxLayout(w);
    vl->setContentsMargins(0, 0, 0, 0);
    vl->setSpacing(4);

    QHBoxLayout* header = new QHBoxLayout();
    QLabel* catLbl = new QLabel(category, w); catLbl->setObjectName("budgetCat");
    int pct = (limit > 0) ? qMin((int)((spent / limit) * 100), 100) : 0;
    QLabel* pctLbl = new QLabel(QString("%1%").arg(pct), w); pctLbl->setObjectName("budgetPct");
    header->addWidget(catLbl); header->addStretch(); header->addWidget(pctLbl);

    QProgressBar* bar = new QProgressBar(w);
    bar->setObjectName("budgetBar");
    bar->setRange(0, 100);
    bar->setValue(pct);
    bar->setTextVisible(false);
    bar->setFixedHeight(6);

    if      (pct >= 100) bar->setProperty("pct", "danger");
    else if (pct >= 75)  bar->setProperty("pct", "warning");
    bar->style()->unpolish(bar);
    bar->style()->polish(bar);

    vl->addLayout(header);
    vl->addWidget(bar);
    return w;
}

// =============================================================================
//  Data
// =============================================================================
void DashboardWindow::loadData()
{
    if (m_transactions.isEmpty()) {
        Transaction t1;
        t1.set_type(Transaction::Income);  t1.set_amount(5000.00);
        t1.set_category("Other");          t1.set_description("Monthly Stipend");
        t1.set_transac_date(QDateTime::currentDateTime().addDays(-1));
        m_transactions.append(t1);

        Transaction t2;
        t2.set_type(Transaction::Expense); t2.set_amount(450.00);
        t2.set_category("Food");           t2.set_description("Grocery Shopping");
        t2.set_transac_date(QDateTime::currentDateTime().addDays(-2));
        m_transactions.append(t2);

        Transaction t3;
        t3.set_type(Transaction::Expense); t3.set_amount(120.00);
        t3.set_category("Transport");      t3.set_description("Uber Ride");
        t3.set_transac_date(QDateTime::currentDateTime().addDays(-3));
        m_transactions.append(t3);
    }

    // Uncomment when we implement the service layer:
    // TransactionService txSvc; txSvc.initialize();
    // m_transactions = txSvc.getTransactions(m_user.get_id());
    // BudgetService budSvc; budSvc.initialize();
    // m_budgets = budSvc.getBudgets(m_user.get_id());
}

double DashboardWindow::totalBalance() const
{
    double bal = 0.0;
    for (const auto& tx : m_transactions)
        bal += (tx.get_type() == Transaction::Income) ? tx.get_amount() : -tx.get_amount();
    return bal;
}

double DashboardWindow::monthlyIncome() const
{
    int m = QDate::currentDate().month(), y = QDate::currentDate().year();
    double total = 0.0;
    for (const auto& tx : m_transactions)
        if (tx.get_type() == Transaction::Income &&
            tx.get_transac_date().date().month() == m &&
            tx.get_transac_date().date().year()  == y)
            total += tx.get_amount();
    return total;
}

double DashboardWindow::monthlyExpenses() const
{
    int m = QDate::currentDate().month(), y = QDate::currentDate().year();
    double total = 0.0;
    for (const auto& tx : m_transactions)
        if (tx.get_type() == Transaction::Expense &&
            tx.get_transac_date().date().month() == m &&
            tx.get_transac_date().date().year()  == y)
            total += tx.get_amount();
    return total;
}