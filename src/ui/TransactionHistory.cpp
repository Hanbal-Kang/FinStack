#include "ui/TransactionHistoryWindow.h"
#include "services/TransactionService.h"
#include <QScrollArea>
#include <QMessageBox>

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────────────────────────────────────

TransactionHistoryWindow::TransactionHistoryWindow(const User& user, QWidget* parent)
    : BaseWindow(parent), m_user(user)
{
    setupWindowGeometry(1100, 720);
    initialize();
}

// ─────────────────────────────────────────────────────────────────────────────
//  setupUI
// ─────────────────────────────────────────────────────────────────────────────

void TransactionHistoryWindow::setupUI()
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QScrollArea* outerScroll = new QScrollArea(central);
    outerScroll->setWidgetResizable(true);
    outerScroll->setFrameShape(QFrame::NoFrame);
    outerScroll->setObjectName("pageScroll");

    QWidget* page = new QWidget();
    page->setObjectName("pageBody");

    QVBoxLayout* pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(40, 36, 40, 40);
    pageLayout->setSpacing(24);

    buildHeader(pageLayout);
    buildSummaryCards(pageLayout);
    buildFilterBar(pageLayout);
    buildTableHeader(pageLayout);
    buildTableArea(pageLayout);
    pageLayout->addStretch();

    outerScroll->setWidget(page);

    QVBoxLayout* root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->addWidget(outerScroll);

    loadData();
    applyFilters();
    renderRows();
    updateSummaryCards();
}

void TransactionHistoryWindow::applyStyles()
{
    loadGlobalStylesheet();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Layout builders
// ─────────────────────────────────────────────────────────────────────────────

// Screenshot: list icon box + "Transaction History" + "View and manage…" subtitle
// Back button top-left (same pattern as BudgetWindow)
void TransactionHistoryWindow::buildHeader(QVBoxLayout* root)
{
    QHBoxLayout* topRow = new QHBoxLayout();

    QPushButton* backBtn = new QPushButton("←");
    backBtn->setObjectName("navIconBtn");
    backBtn->setFixedSize(36, 36);
    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, &TransactionHistoryWindow::onBackClicked);

    topRow->addWidget(backBtn);
    topRow->addStretch();
    root->addLayout(topRow);

    QHBoxLayout* titleRow = new QHBoxLayout();
    titleRow->setSpacing(14);

    // blue list icon box (matches screenshot)
    QLabel* iconBox = new QLabel("≡");
    iconBox->setObjectName("historyTitleIcon");
    iconBox->setFixedSize(52, 52);
    iconBox->setAlignment(Qt::AlignCenter);

    QVBoxLayout* titleCol = new QVBoxLayout();
    titleCol->setSpacing(4);

    QLabel* title    = new QLabel("Transaction History");
    QLabel* subtitle = new QLabel("View and manage all your transactions");
    title   ->setObjectName("pageTitle");
    subtitle->setObjectName("pageSubtitle");

    titleCol->addWidget(title);
    titleCol->addWidget(subtitle);

    titleRow->addWidget(iconBox);
    titleRow->addLayout(titleCol);
    titleRow->addStretch();
    root->addLayout(titleRow);
}

// Screenshot: three equal-width cards with coloured dot + label + large amount
void TransactionHistoryWindow::buildSummaryCards(QVBoxLayout* root)
{
    QHBoxLayout* row = new QHBoxLayout();
    row->setSpacing(16);

    auto makeCard = [&](const QString& dotColor, const QString& labelText,
                        QLabel*& valueLabel, const QString& valueObj) -> QFrame*
    {
        QFrame* card = new QFrame();
        card->setObjectName("summaryCard");

        QVBoxLayout* cl = new QVBoxLayout(card);
        cl->setContentsMargins(20, 18, 20, 18);
        cl->setSpacing(8);

        // dot + label row
        QHBoxLayout* dotRow = new QHBoxLayout();
        dotRow->setSpacing(8);
        QLabel* dot = new QLabel("●");
        dot->setStyleSheet("color:" + dotColor + "; font-size:10px;");
        QLabel* lbl = new QLabel(labelText);
        lbl->setObjectName("summaryCardLabel");
        dotRow->addWidget(dot);
        dotRow->addWidget(lbl);
        dotRow->addStretch();

        valueLabel = new QLabel("Rs 0");
        valueLabel->setObjectName(valueObj);

        cl->addLayout(dotRow);
        cl->addWidget(valueLabel);
        return card;
    };

    QFrame* incomeCard  = makeCard("#22C55E", "Total Income",
                                  m_incomeTotalLabel,  "summaryCardIncome");
    QFrame* expenseCard = makeCard("#EF4444", "Total Expenses",
                                   m_expenseTotalLabel, "summaryCardExpense");
    QFrame* netCard     = makeCard("#3B82F6", "Net Balance",
                               m_netLabel,          "summaryCardNet");

    row->addWidget(incomeCard,  1);
    row->addWidget(expenseCard, 1);
    row->addWidget(netCard,     1);
    root->addLayout(row);
}

// Screenshot: full-width search bar (left) + All / Income / Expense toggle (right)
void TransactionHistoryWindow::buildFilterBar(QVBoxLayout* root)
{
    QFrame* bar = new QFrame();
    bar->setObjectName("filterBar");

    QHBoxLayout* bl = new QHBoxLayout(bar);
    bl->setContentsMargins(16, 12, 16, 12);
    bl->setSpacing(12);

    // search icon + input
    QLabel* searchIcon = new QLabel("🔍");
    searchIcon->setObjectName("searchIcon");

    m_searchInput = new QLineEdit();
    m_searchInput->setObjectName("searchInput");
    m_searchInput->setPlaceholderText("Search transactions...");
    m_searchInput->setFixedHeight(40);
    connect(m_searchInput, &QLineEdit::textChanged,
            this, &TransactionHistoryWindow::onSearchChanged);

    // type toggle buttons
    m_allBtn     = new QPushButton("All");
    m_incomeBtn  = new QPushButton("Income");
    m_expenseBtn = new QPushButton("Expense");

    for (QPushButton* btn : {m_allBtn, m_incomeBtn, m_expenseBtn}) {
        btn->setFixedHeight(36);
        btn->setCursor(Qt::PointingHandCursor);
    }
    // "All" selected by default
    m_allBtn    ->setObjectName("filterBtnActive");
    m_incomeBtn ->setObjectName("filterBtn");
    m_expenseBtn->setObjectName("filterBtn");

    connect(m_allBtn,     &QPushButton::clicked, this, &TransactionHistoryWindow::onAllClicked);
    connect(m_incomeBtn,  &QPushButton::clicked, this, &TransactionHistoryWindow::onIncomeClicked);
    connect(m_expenseBtn, &QPushButton::clicked, this, &TransactionHistoryWindow::onExpenseClicked);

    bl->addWidget(searchIcon);
    bl->addWidget(m_searchInput, 1);
    bl->addWidget(m_allBtn);
    bl->addWidget(m_incomeBtn);
    bl->addWidget(m_expenseBtn);

    root->addWidget(bar);
}

// Screenshot: column header row — DATE | DESCRIPTION | CATEGORY | TYPE | AMOUNT
void TransactionHistoryWindow::buildTableHeader(QVBoxLayout* root)
{
    QFrame* header = new QFrame();
    header->setObjectName("tableHeader");

    QHBoxLayout* hl = new QHBoxLayout(header);
    hl->setContentsMargins(20, 10, 20, 10);
    hl->setSpacing(0);

    auto makeCol = [&](const QString& text, int stretch, Qt::Alignment align = Qt::AlignLeft) {
        QLabel* lbl = new QLabel(text);
        lbl->setObjectName("tableHeaderLabel");
        lbl->setAlignment(align);
        hl->addWidget(lbl, stretch);
    };

    makeCol("DATE",        2);
    makeCol("DESCRIPTION", 5);
    makeCol("CATEGORY",    3, Qt::AlignCenter);
    makeCol("TYPE",        2, Qt::AlignCenter);
    makeCol("AMOUNT",      2, Qt::AlignRight);
    hl->addSpacing(40);    // space for delete button column

    root->addWidget(header);
}

// The scrollable list area
void TransactionHistoryWindow::buildTableArea(QVBoxLayout* root)
{
    QFrame* tableFrame = new QFrame();
    tableFrame->setObjectName("tableFrame");

    QVBoxLayout* tl = new QVBoxLayout(tableFrame);
    tl->setContentsMargins(0, 0, 0, 0);
    tl->setSpacing(0);

    m_tableRows = tl;

    m_emptyLabel = new QLabel("No transactions found.");
    m_emptyLabel->setObjectName("emptyState");
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setFixedHeight(80);
    m_emptyLabel->hide();
    tl->addWidget(m_emptyLabel);

    root->addWidget(tableFrame);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Data / render
// ─────────────────────────────────────────────────────────────────────────────

void TransactionHistoryWindow::loadData()
{
    TransactionService service;
    m_allTransactions = service.getAllByUser(m_user.get_id());
}

void TransactionHistoryWindow::applyFilters()
{
    QString searchText = m_searchInput ? m_searchInput->text().trimmed().toLower() : "";

    m_filtered.clear();
    for (const auto& tx : m_allTransactions)
    {
        // type filter
        if (m_activeFilter == "Income"  && tx.get_type() != Transaction::Income)  continue;
        if (m_activeFilter == "Expense" && tx.get_type() != Transaction::Expense) continue;

        // search filter — matches description or category
        if (!searchText.isEmpty()) {
            bool matchDesc = tx.get_description().toLower().contains(searchText);
            bool matchCat  = tx.get_category().toLower().contains(searchText);
            if (!matchDesc && !matchCat) continue;
        }

        m_filtered.push_back(tx);
    }
}

void TransactionHistoryWindow::renderRows()
{
    // remove all old rows (keep index 0 = empty label)
    while (m_tableRows->count() > 1) {
        QLayoutItem* item = m_tableRows->takeAt(1);
        if (item->widget()) delete item->widget();
        delete item;
    }

    if (m_filtered.empty()) {
        m_emptyLabel->show();
        return;
    }
    m_emptyLabel->hide();

    for (const auto& tx : m_filtered)
        m_tableRows->addWidget(makeRow(tx));
}

// One row — matches screenshot: date | bold description | category pill
// | ↑Income / ↓Expense badge | ±amount | trash icon
QWidget* TransactionHistoryWindow::makeRow(const Transaction& tx)
{
    QFrame* row = new QFrame();
    row->setObjectName("tableRow");

    QHBoxLayout* rl = new QHBoxLayout(row);
    rl->setContentsMargins(20, 14, 20, 14);
    rl->setSpacing(0);

    // DATE
    QLabel* dateLabel = new QLabel(tx.get_transac_date().date().toString("MMM d, yyyy"));
    dateLabel->setObjectName("txDateCell");

    // DESCRIPTION
    QLabel* descLabel = new QLabel(tx.get_description());
    descLabel->setObjectName("txDescCell");

    // CATEGORY pill
    QLabel* catLabel = new QLabel(tx.get_category());
    catLabel->setObjectName("txCategoryPill");
    catLabel->setAlignment(Qt::AlignCenter);

    // TYPE badge — ↑Income (green) or ↓Expense (red) as in screenshot
    bool isIncome = (tx.get_type() == Transaction::Income);
    QLabel* typeLabel = new QLabel(isIncome ? "↑Income" : "↓Expense");
    typeLabel->setObjectName(isIncome ? "txTypeBadgeIncome" : "txTypeBadgeExpense");
    typeLabel->setAlignment(Qt::AlignCenter);

    // AMOUNT — green "+Rs X" for income, plain "-Rs X" for expense
    QString amtText = isIncome
                          ? "+Rs " + QString::number(tx.get_amount(), 'f', 0)
                          : "-Rs " + QString::number(tx.get_amount(), 'f', 0);
    QLabel* amtLabel = new QLabel(amtText);
    amtLabel->setObjectName(isIncome ? "txAmountIncome" : "txAmountExpense");
    amtLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    // DELETE button — trash icon
    QPushButton* deleteBtn = new QPushButton("🗑");
    deleteBtn->setObjectName("dangerIconBtn");
    deleteBtn->setFixedSize(28, 28);
    deleteBtn->setCursor(Qt::PointingHandCursor);

    int txId = tx.get_transaction_id();
    connect(deleteBtn, &QPushButton::clicked, this, [this, txId]() {
        onDeleteClicked(txId);
    });

    rl->addWidget(dateLabel, 2);
    rl->addWidget(descLabel, 5);
    rl->addWidget(catLabel,  3, Qt::AlignCenter);
    rl->addWidget(typeLabel, 2, Qt::AlignCenter);
    rl->addWidget(amtLabel,  2);
    rl->addWidget(deleteBtn, 0, Qt::AlignRight | Qt::AlignVCenter);

    return row;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Summary helpers
// ─────────────────────────────────────────────────────────────────────────────

double TransactionHistoryWindow::sumByType(Transaction::Type type) const
{
    double total = 0.0;
    for (const auto& tx : m_allTransactions)
        if (tx.get_type() == type)
            total += tx.get_amount();
    return total;
}

void TransactionHistoryWindow::updateSummaryCards()
{
    double income  = sumByType(Transaction::Income);
    double expense = sumByType(Transaction::Expense);
    double net     = income - expense;

    m_incomeTotalLabel ->setText("Rs " + QString::number(income,  'f', 0));
    m_expenseTotalLabel->setText("Rs " + QString::number(expense, 'f', 0));

    // Net label: red if negative (matches screenshot), normal otherwise
    if (net < 0) {
        m_netLabel->setText("-Rs " + QString::number(qAbs(net), 'f', 0));
        m_netLabel->setObjectName("summaryCardNetNegative");
    } else {
        m_netLabel->setText("Rs " + QString::number(net, 'f', 0));
        m_netLabel->setObjectName("summaryCardNet");
    }

    // force QSS repolish so the colour change takes effect
    m_netLabel->style()->unpolish(m_netLabel);
    m_netLabel->style()->polish(m_netLabel);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Filter toggle helper — repolishes buttons (fixes QSS not updating)
// ─────────────────────────────────────────────────────────────────────────────

void TransactionHistoryWindow::setActiveFilter(const QString& filter)
{
    m_activeFilter = filter;

    auto repolish = [](QWidget* w, const QString& name) {
        w->setObjectName(name);
        w->style()->unpolish(w);
        w->style()->polish(w);
        w->update();
    };

    repolish(m_allBtn,     filter == "All"     ? "filterBtnActive" : "filterBtn");
    repolish(m_incomeBtn,  filter == "Income"  ? "filterBtnActive" : "filterBtn");
    repolish(m_expenseBtn, filter == "Expense" ? "filterBtnActive" : "filterBtn");

    applyFilters();
    renderRows();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Public refresh
// ─────────────────────────────────────────────────────────────────────────────

void TransactionHistoryWindow::refreshHistory()
{
    loadData();
    applyFilters();
    renderRows();
    updateSummaryCards();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Slots
// ─────────────────────────────────────────────────────────────────────────────

void TransactionHistoryWindow::onSearchChanged(const QString& /*text*/)
{
    applyFilters();
    renderRows();
}

void TransactionHistoryWindow::onAllClicked()     { setActiveFilter("All");     }
void TransactionHistoryWindow::onIncomeClicked()  { setActiveFilter("Income");  }
void TransactionHistoryWindow::onExpenseClicked() { setActiveFilter("Expense"); }

void TransactionHistoryWindow::onDeleteClicked(int transactionId)
{
    auto reply = QMessageBox::question(
        this, "Delete Transaction",
        "Are you sure you want to delete this transaction?\nThis cannot be undone.",
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    TransactionService service;
    service.deleteTransaction(transactionId);

    loadData();
    applyFilters();
    renderRows();
    updateSummaryCards();
}

void TransactionHistoryWindow::onBackClicked()
{
    emit navigateBack();
    close();
}