#include "ui/BudgetWindow.h"
#include "ui/DashboardWindow.h"
#include <QScrollArea>
#include <QSizePolicy>
#include <QDate>
#include <QStyle>
#include <QtMath>

BudgetWindow::BudgetWindow(const User& user, QWidget* parent) : BaseWindow(parent), m_user(user)
{
    setWindowTitle("FinStack — Budget Management");
    setupWindowGeometry(1100, 700);

    loadData();
    initialize();
}

void BudgetWindow::setupUI()
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QVBoxLayout* root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    QScrollArea* scroll = new QScrollArea(central);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setObjectName("pageScroll");

    QWidget* page = new QWidget();
    page->setObjectName("pageBody");

    QVBoxLayout* pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(40, 36, 40, 40);
    pageLayout->setSpacing(24);

    buildHeader(pageLayout);
    buildSummaryCard(pageLayout);
    buildCategoryGrid(pageLayout);

    pageLayout->addStretch();
    scroll->setWidget(page);
    root->addWidget(scroll, 1);
}

void BudgetWindow::applyStyles()
{
    loadGlobalStylesheet();
}

void BudgetWindow::refreshBudgets()
{
    loadData();

    double budget    = totalBudget();
    double spent     = totalSpent();
    double remaining = budget - spent;
    int    pct       = (budget > 0) ? qMin((int)((spent / budget) * 100), 100) : 0;

    if (m_totalBudgetLabel) m_totalBudgetLabel->setText(QString("$%1").arg(budget,    0, 'f', 2));
    if (m_totalSpentLabel)  m_totalSpentLabel ->setText(QString("$%1").arg(spent,     0, 'f', 2));
    if (m_remainingLabel)   m_remainingLabel  ->setText(QString("$%1").arg(remaining, 0, 'f', 2));
    if (m_overallBar)       m_overallBar->setValue(pct);
    if (m_overallPctLabel)  m_overallPctLabel->setText(QString("%1% of total budget used").arg(pct));

    if (!m_gridWidget || !m_gridLayout) return;

    QLayoutItem* item;
    while ((item = m_gridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    m_editInputs.clear();
    m_saveButtons.clear();
    m_editButtons.clear();

    for (int i = 0; i < m_categories.size(); i++) {
        const auto& meta = m_categories[i];
        double limit = 0.0;
        double categorySpent = m_spent.value(meta.name, 0.0);

        for (const auto& b : m_budgets)
            if (Budget::categoryToString(b.get_category()) == meta.name) { limit = b.get_monthly_limit(); break; }

        QFrame* card = makeCategoryCard(i, meta.emoji, meta.name, limit, categorySpent);
        m_gridLayout->addWidget(card, i / 2, i % 2);
    }
}
//CHANGE : HANBAL
void BudgetWindow::onBackClicked()
{
    // Reopen Dashboard with the same User, then close this window.
    DashboardWindow* dashboard = new DashboardWindow(m_user, nullptr);
    dashboard->show();
    this->close();
}

void BudgetWindow::onEditClicked(int index)
{
    if (index < 0 || index >= m_editInputs.size()) return;

    m_editButtons[index]->setVisible(false);
    m_editInputs [index]->setVisible(true);
    m_saveButtons[index]->setVisible(true);

    // Pre-fill with current limit
    double currentLimit = 0.0;
    const QString& cat = m_categories[index].name;
    for (const auto& b : m_budgets)
        if (Budget::categoryToString(b.get_category()) == cat) { currentLimit = b.get_monthly_limit(); break; }

    m_editInputs[index]->setText(QString::number(currentLimit, 'f', 2));
    m_editInputs[index]->setFocus();
    m_editInputs[index]->selectAll();
}

void BudgetWindow::onSaveClicked(int index)
{
    if (index < 0 || index >= m_editInputs.size()) return;

    bool ok = false;
    double newLimit = m_editInputs[index]->text().toDouble(&ok);
    if (!ok || newLimit < 0) {
        m_editInputs[index]->setStyleSheet("border-color: #f85149;");
        return;
    }

    // Update the in-memory budget (service call goes here when ready)
    const QString& cat = m_categories[index].name;
    bool found = false;
    for (auto& b : m_budgets) {
        if (Budget::categoryToString(b.get_category()) == cat) {
            b.set_monthly_limit(newLimit);
            found = true;
            break;
        }
    }
    if (!found) {
        Budget newBudget;
        newBudget.set_user_id(m_user.get_id());
        newBudget.set_category(Budget::categoryFromString(cat));
        newBudget.set_monthly_limit(newLimit);
        m_budgets.append(newBudget);
    }

    // Uncomment when BudgetService is ready:
    // BudgetService svc; svc.initialize();
    // Budget b; b.set_user_id(m_user.get_id());
    // b.set_category(cat); b.set_monthly_limit(newLimit);
    // svc.setBudget(b);

    // Restore UI state and refresh
    m_editInputs [index]->setVisible(false);
    m_saveButtons[index]->setVisible(false);
    m_editButtons[index]->setVisible(true);

    refreshBudgets();
}

void BudgetWindow::buildHeader(QVBoxLayout* root)
{
    QHBoxLayout* topRow = new QHBoxLayout();
    topRow->setSpacing(16);

    QPushButton* backBtn = new QPushButton("←", this);
    backBtn->setObjectName("navIconBtn");
    backBtn->setFixedSize(36, 36);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, &BudgetWindow::onBackClicked);
    topRow->addWidget(backBtn);
    topRow->addStretch();

    root->addLayout(topRow);

    QHBoxLayout* titleRow = new QHBoxLayout();
    titleRow->setSpacing(14);

    QLabel* iconBox = new QLabel("💰");
    iconBox->setObjectName("budgetTitleIcon");
    iconBox->setFixedSize(52, 52);
    iconBox->setAlignment(Qt::AlignCenter);

    QVBoxLayout* titleCol = new QVBoxLayout();
    titleCol->setSpacing(4);

    QLabel* title = new QLabel("Budget Management");
    title->setObjectName("pageTitle");

    QLabel* subtitle = new QLabel("Set spending limits and track progress by category");
    subtitle->setObjectName("pageSubtitle");

    titleCol->addWidget(title);
    titleCol->addWidget(subtitle);

    titleRow->addWidget(iconBox);
    titleRow->addLayout(titleCol);
    titleRow->addStretch();

    root->addLayout(titleRow);
}

void BudgetWindow::buildSummaryCard(QVBoxLayout* root)
{
    QFrame* card = new QFrame();
    card->setObjectName("budgetSummaryCard");

    QVBoxLayout* vl = new QVBoxLayout(card);
    vl->setContentsMargins(28, 24, 28, 24);
    vl->setSpacing(16);

    QHBoxLayout* cols = new QHBoxLayout();
    cols->setSpacing(0);

    auto makeCol = [&](const QString& title, QLabel*& valueLabel,
                       const QString& objName) {
        QVBoxLayout* col = new QVBoxLayout();
        col->setSpacing(6);
        QLabel* titleLbl = new QLabel(title);
        titleLbl->setObjectName("summaryColTitle");
        valueLabel = new QLabel("$0.00");
        valueLabel->setObjectName(objName);
        col->addWidget(titleLbl);
        col->addWidget(valueLabel);
        return col;
    };

    cols->addLayout(makeCol("Total Budget", m_totalBudgetLabel, "summaryValueNeutral"));
    cols->addStretch();
    cols->addLayout(makeCol("Total Spent",  m_totalSpentLabel,  "summaryValueDanger"));
    cols->addStretch();
    cols->addLayout(makeCol("Remaining",    m_remainingLabel,   "summaryValueSuccess"));

    m_overallBar = new QProgressBar(card);
    m_overallBar->setObjectName("overallBar");
    m_overallBar->setRange(0, 100);
    m_overallBar->setTextVisible(false);
    m_overallBar->setFixedHeight(10);

    m_overallPctLabel = new QLabel("0% of total budget used", card);
    m_overallPctLabel->setObjectName("overallPctLabel");
    m_overallPctLabel->setAlignment(Qt::AlignCenter);

    vl->addLayout(cols);
    vl->addWidget(m_overallBar);
    vl->addWidget(m_overallPctLabel);

    double budget    = totalBudget();
    double spent     = totalSpent();
    double remaining = budget - spent;
    int    pct       = (budget > 0) ? qMin((int)((spent / budget) * 100), 100) : 0;

    m_totalBudgetLabel->setText(QString("$%1").arg(budget,    0, 'f', 2));
    m_totalSpentLabel ->setText(QString("$%1").arg(spent,     0, 'f', 2));
    m_remainingLabel  ->setText(QString("$%1").arg(remaining, 0, 'f', 2));
    m_overallBar->setValue(pct);
    m_overallPctLabel->setText(QString("%1% of total budget used").arg(pct));


    QString barColor = (pct >= 100) ? "#f85149" : (pct >= 75) ? "#f0883e" : "#3fb950";
    m_overallBar->setStyleSheet(QString(
                                    "QProgressBar { background-color: #21262d; border: none; border-radius: 5px; }"
                                    "QProgressBar::chunk { background-color: %1; border-radius: 5px; }"
                                    ).arg(barColor));

    root->addWidget(card);
}

void BudgetWindow::buildCategoryGrid(QVBoxLayout* root)
{
    m_gridWidget = new QWidget();
    m_gridLayout = new QGridLayout(m_gridWidget);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    m_gridLayout->setSpacing(16);
    m_gridLayout->setColumnStretch(0, 1);
    m_gridLayout->setColumnStretch(1, 1);

    for (int i = 0; i < m_categories.size(); i++) {
        const auto& meta = m_categories[i];
        double limit = 0.0;
        double spent = m_spent.value(meta.name, 0.0);

        for (const auto& b : m_budgets)
            if (Budget::categoryToString(b.get_category()) == meta.name) { limit = b.get_monthly_limit(); break; }

        QFrame* card = makeCategoryCard(i, meta.emoji, meta.name, limit, spent);
        m_gridLayout->addWidget(card, i / 2, i % 2);
    }

    root->addWidget(m_gridWidget);
}

QFrame* BudgetWindow::makeCategoryCard(int index, const QString& emoji, const QString& category, double limit, double spent)
{
    QFrame* card = new QFrame();
    card->setObjectName("budgetCategoryCard");

    QVBoxLayout* vl = new QVBoxLayout(card);
    vl->setContentsMargins(20, 18, 20, 18);
    vl->setSpacing(12);

    QHBoxLayout* topRow = new QHBoxLayout();
    topRow->setSpacing(12);

    QLabel* emojiLbl = new QLabel(emoji);
    emojiLbl->setObjectName("categoryEmoji");
    emojiLbl->setFixedSize(36, 36);
    emojiLbl->setAlignment(Qt::AlignCenter);

    QVBoxLayout* nameCol = new QVBoxLayout();
    nameCol->setSpacing(2);
    QLabel* nameLbl = new QLabel(category);
    nameLbl->setObjectName("budgetCatName");
    QLabel* subLbl = new QLabel("Monthly limit");
    subLbl->setObjectName("budgetCatSub");
    nameCol->addWidget(nameLbl);
    nameCol->addWidget(subLbl);

    QString status    = statusLabel(spent, limit);
    QString badgeName = (status == "On Track")    ? "badgeOnTrack"
                        : (status == "Warning")     ? "badgeWarning"
                                                :                             "badgeOverBudget";

    QLabel* badge = new QLabel(status);
    badge->setObjectName(badgeName);
    badge->setAlignment(Qt::AlignCenter);
    badge->setFixedHeight(26);
    badge->setContentsMargins(12, 0, 12, 0);

    topRow->addWidget(emojiLbl);
    topRow->addLayout(nameCol);
    topRow->addStretch();
    topRow->addWidget(badge);

    QHBoxLayout* limitRow = new QHBoxLayout();

    QVBoxLayout* limitCol = new QVBoxLayout();
    limitCol->setSpacing(4);

    QLabel* limitLbl = new QLabel(
        limit > 0 ? QString("$%1").arg(limit, 0, 'f', 2) : "Not set");
    limitLbl->setObjectName("budgetLimit");

    double remaining = limit - spent;
    QLabel* spentLbl = new QLabel(
        QString("$%1 spent  •  $%2 left")
            .arg(spent,     0, 'f', 0)
            .arg(qMax(remaining, 0.0), 0, 'f', 0));
    spentLbl->setObjectName("budgetSpentLeft");

    limitCol->addWidget(limitLbl);
    limitCol->addWidget(spentLbl);

    QPushButton* editBtn = new QPushButton("✎");
    editBtn->setObjectName("editBudgetBtn");
    editBtn->setFixedSize(36, 36);
    editBtn->setCursor(Qt::PointingHandCursor);
    editBtn->setToolTip("Edit limit");

    QLineEdit* editInput = new QLineEdit();
    editInput->setObjectName("budgetInput");
    editInput->setPlaceholderText("New limit...");
    editInput->setFixedWidth(100);
    editInput->setVisible(false);

    QPushButton* saveBtn = new QPushButton("✓ Save");
    saveBtn->setObjectName("saveBudgetBtn");
    saveBtn->setFixedHeight(32);
    saveBtn->setVisible(false);
    saveBtn->setCursor(Qt::PointingHandCursor);

    m_editButtons.append(editBtn);
    m_editInputs .append(editInput);
    m_saveButtons.append(saveBtn);

    connect(editBtn, &QPushButton::clicked, this, [this, index]() { onEditClicked(index); });
    connect(saveBtn, &QPushButton::clicked, this, [this, index]() { onSaveClicked(index); });

    QHBoxLayout* editRow = new QHBoxLayout();
    editRow->setSpacing(8);
    editRow->addWidget(editInput);
    editRow->addWidget(saveBtn);
    editRow->addStretch();

    limitRow->addLayout(limitCol);
    limitRow->addStretch();
    limitRow->addWidget(editBtn);

    int pct = (limit > 0) ? qMin((int)((spent / limit) * 100), 100) : 0;

    QProgressBar* bar = new QProgressBar(card);
    bar->setObjectName("categoryBar");
    bar->setRange(0, 100);
    bar->setValue(pct);
    bar->setTextVisible(false);
    bar->setFixedHeight(8);

    QString barColor = (pct >= 100) ? "#f85149"
                       : (pct >= 75)  ? "#f0883e"
                                     :                "#3fb950";
    bar->setStyleSheet(QString(
                           "QProgressBar { background-color: #21262d; border: none; border-radius: 4px; }"
                           "QProgressBar::chunk { background-color: %1; border-radius: 4px; }"
                           ).arg(barColor));

    QLabel* pctLbl = new QLabel(QString("%1%").arg(static_cast<double>(pct), 0, 'f', 1));
    pctLbl->setObjectName("budgetPctRight");
    pctLbl->setAlignment(Qt::AlignRight);

    vl->addLayout(topRow);
    vl->addLayout(limitRow);
    vl->addLayout(editRow);
    vl->addWidget(bar);
    vl->addWidget(pctLbl);

    return card;
}

// =============================================================================
//  Data
// =============================================================================
void BudgetWindow::loadData()
{
    if (m_budgets.isEmpty()) {
        struct Seed { QString cat; double limit; };
        QList<Seed> seeds = {
                             { "Food",          1000.0 },
                             { "Transport",      300.0 },
                             { "Rent",          2500.0 },
                             { "Tuition",       3500.0 },
                             { "Entertainment",  200.0 },
                             { "Other",          500.0 },
                             };
        for (const auto& s : seeds) {
            Budget b;
            b.set_user_id(m_user.get_id());
            b.set_category(Budget::categoryFromString(s.cat));
            b.set_monthly_limit(s.limit);
            m_budgets.append(b);
        }
    }

    if (m_transactions.isEmpty()) {
        struct TxSeed { QString cat; double amount; };
        QList<TxSeed> txSeeds = {
                                 { "Food",           850.0 },
                                 { "Transport",      180.0 },
                                 { "Rent",          2500.0 },
                                 { "Tuition",       3500.0 },
                                 { "Entertainment",  195.0 },
                                 { "Other",          120.0 },
                                 };
        for (const auto& s : txSeeds) {
            Transaction t;
            t.set_type(Transaction::Expense);
            t.set_account_id(m_user.get_id());
            t.set_category(s.cat);
            t.set_amount(s.amount);
            t.set_transac_date(QDateTime::currentDateTime());
            m_transactions.append(t);
        }
    }

    m_spent.clear();
    int currentMonth = QDate::currentDate().month();
    int currentYear  = QDate::currentDate().year();
    for (const auto& tx : m_transactions) {
        if (tx.get_type() == Transaction::Expense &&
            tx.get_transac_date().date().month() == currentMonth &&
            tx.get_transac_date().date().year()  == currentYear)
        {
            m_spent[tx.get_category()] += tx.get_amount();
        }
    }

    // ── Uncomment when services are implemented ───────────────────────────────
    // BudgetService budSvc; budSvc.initialize();
    // m_budgets = budSvc.getBudgets(m_user.get_id());
    // TransactionService txSvc; txSvc.initialize();
    // m_transactions = txSvc.getTransactions(m_user.get_id());
}

double BudgetWindow::totalBudget() const
{
    double total = 0.0;
    for (const auto& b : m_budgets) total += b.get_monthly_limit();
    return total;
}

double BudgetWindow::totalSpent() const
{
    double total = 0.0;
    for (auto it = m_spent.begin(); it != m_spent.end(); ++it) total += it.value();
    return total;
}

QString BudgetWindow::statusLabel(double spent, double limit) const
{
    if (limit <= 0)            return "On Track";
    double pct = spent / limit;
    if (pct >= 1.0)            return "Over Budget";
    if (pct >= 0.75)           return "Warning";
    return "On Track";
}

QString BudgetWindow::barStatus(double spent, double limit) const
{
    if (limit <= 0)            return "ok";
    double pct = spent / limit;
    if (pct >= 1.0)            return "danger";
    if (pct >= 0.75)           return "warning";
    return "ok";
}
