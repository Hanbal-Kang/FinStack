#include "ui/TransactionHistory.h"
#include "ui/DashboardWindow.h"
#include "services/TransactionService.h"
#include <QScrollArea>
#include <QMessageBox>
#include <QStyle>
#include <QSet>
#include <algorithm>

TransactionHistoryWindow::TransactionHistoryWindow(const User& user, QWidget* parent)
    : BaseWindow(parent), m_user(user)
{
    setupWindowGeometry(1100, 720);
    initialize();
}

void TransactionHistoryWindow::setupUI()
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QScrollArea* scroll = new QScrollArea(central);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setObjectName("pageScroll");

    QWidget* page = new QWidget();
    page->setObjectName("pageBody");
    QVBoxLayout* pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(40, 36, 40, 40);
    pageLayout->setSpacing(24);

    // Assembly using your builders (renamed to match header: buildSummaryBar, buildList)
    buildHeader(pageLayout);
    buildSummaryBar(pageLayout);
    buildFilterBar(pageLayout);
    buildList(pageLayout);

    pageLayout->addStretch();
    scroll->setWidget(page);

    QVBoxLayout* root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->addWidget(scroll);

    refreshHistory();
}

void TransactionHistoryWindow::applyStyles()
{
    loadGlobalStylesheet();
}


// =============================================================================
//  Section builders
// =============================================================================
void TransactionHistoryWindow::buildHeader(QVBoxLayout* root)
{
    QHBoxLayout* topRow = new QHBoxLayout();
    m_backBtn = new QPushButton("←", this);
    m_backBtn->setObjectName("navIconBtn");
    m_backBtn->setFixedSize(36, 36);
    m_backBtn->setCursor(Qt::PointingHandCursor);
    connect(m_backBtn, &QPushButton::clicked, this, &TransactionHistoryWindow::onBackClicked);
    topRow->addWidget(m_backBtn);
    topRow->addStretch();
    root->addLayout(topRow);

    QHBoxLayout* titleRow = new QHBoxLayout();
    titleRow->setSpacing(14);

    QLabel* iconBox = new QLabel("📜");
    iconBox->setObjectName("budgetTitleIcon");
    iconBox->setFixedSize(52, 52);
    iconBox->setAlignment(Qt::AlignCenter);

    QVBoxLayout* titleCol = new QVBoxLayout();
    titleCol->setSpacing(4);
    QLabel* title = new QLabel("Transaction History");
    title->setObjectName("pageTitle");
    QLabel* subtitle = new QLabel("Search and filter all your past transactions");
    subtitle->setObjectName("pageSubtitle");
    titleCol->addWidget(title);
    titleCol->addWidget(subtitle);

    titleRow->addWidget(iconBox);
    titleRow->addLayout(titleCol);
    titleRow->addStretch();
    root->addLayout(titleRow);
}

void TransactionHistoryWindow::buildSummaryBar(QVBoxLayout* root)
{
    QFrame* card = new QFrame();
    card->setObjectName("budgetSummaryCard");

    QHBoxLayout* row = new QHBoxLayout(card);
    row->setContentsMargins(28, 20, 28, 20);
    row->setSpacing(40);

    auto makeTile = [](const QString& label, QLabel*& valueOut) {
        QVBoxLayout* col = new QVBoxLayout();
        col->setSpacing(4);
        QLabel* lbl = new QLabel(label);
        lbl->setObjectName("infoCardLabel");
        valueOut = new QLabel("—");
        valueOut->setObjectName("summaryValue");
        col->addWidget(lbl);
        col->addWidget(valueOut);
        return col;
    };

    row->addLayout(makeTile("INCOME",       m_incomeLabel));
    row->addLayout(makeTile("EXPENSES",     m_expenseLabel));
    row->addLayout(makeTile("NET",          m_netLabel));
    row->addLayout(makeTile("TRANSACTIONS", m_countLabel));
    row->addStretch();

    root->addWidget(card);
}

void TransactionHistoryWindow::buildFilterBar(QVBoxLayout* root)
{
    QFrame* card = new QFrame();
    card->setObjectName("budgetSummaryCard");

    QHBoxLayout* layout = new QHBoxLayout(card);
    layout->setContentsMargins(20, 16, 20, 16);
    layout->setSpacing(12);

    m_searchInput = new QLineEdit();
    m_searchInput->setPlaceholderText("🔍  Search description or category...");
    m_searchInput->setFixedHeight(38);
    m_searchInput->setObjectName("inputField");
    connect(m_searchInput, &QLineEdit::textChanged,
            this, &TransactionHistoryWindow::onSearchChanged);

    m_typeFilter = new QComboBox();
    m_typeFilter->addItems({"All Types", "Income", "Expense"});
    m_typeFilter->setFixedHeight(38);
    m_typeFilter->setObjectName("filterCombo");
    connect(m_typeFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TransactionHistoryWindow::onTypeFilterChanged);

    m_catFilter = new QComboBox();
    m_catFilter->addItem("All Categories");
    m_catFilter->setFixedHeight(38);
    m_catFilter->setObjectName("filterCombo");
    connect(m_catFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TransactionHistoryWindow::onCatFilterChanged);

    //Changed Name to clear Ambiguity in its functionality
    m_clearBtn = new QPushButton("Reset Filters");
    m_clearBtn->setObjectName("primaryBtn");
    m_clearBtn->setCursor(Qt::PointingHandCursor);
    m_clearBtn->setFixedHeight(38);
    connect(m_clearBtn, &QPushButton::clicked,
            this, &TransactionHistoryWindow::onClearFilters);

    layout->addWidget(m_searchInput, 2);
    layout->addWidget(m_typeFilter,  1);
    layout->addWidget(m_catFilter,   1);
    layout->addWidget(m_clearBtn);

    root->addWidget(card);
}

void TransactionHistoryWindow::buildList(QVBoxLayout* root)
{
    QFrame* card = new QFrame();
    card->setObjectName("budgetSummaryCard");

    QVBoxLayout* outer = new QVBoxLayout(card);
    outer->setContentsMargins(20, 16, 20, 16);
    outer->setSpacing(8);

    QLabel* title = new QLabel("All Transactions");
    title->setObjectName("cardTitle");
    outer->addWidget(title);

    m_listLayout = new QVBoxLayout();
    m_listLayout->setSpacing(8);
    outer->addLayout(m_listLayout);

    m_emptyLabel = new QLabel("No transactions match your filters.");
    m_emptyLabel->setObjectName("budgetCatSub");
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    outer->addWidget(m_emptyLabel);

    root->addWidget(card);
}


// =============================================================================
//  Data & Rendering Logic
// =============================================================================

void TransactionHistoryWindow::loadData()
{
    TransactionService service;
    m_allTransactions = service.getAllByUser(m_user.get_id());

    // Newest-first
    std::sort(m_allTransactions.begin(), m_allTransactions.end(),
              [](const Transaction& a, const Transaction& b) {
                  return a.get_transac_date() > b.get_transac_date();
              });

    // Refresh category filter from data
    QSet<QString> cats;
    for (const auto& tx : std::as_const(m_allTransactions))
        cats.insert(tx.get_category());

    QStringList sorted(cats.begin(), cats.end());
    sorted.sort();

    m_catFilter->blockSignals(true);
    QString prev = m_catFilter->currentText();
    m_catFilter->clear();
    m_catFilter->addItem("All Categories");
    for (const QString& cat : std::as_const(sorted))
        m_catFilter->addItem(cat);
    int idx = m_catFilter->findText(prev);
    if (idx >= 0) m_catFilter->setCurrentIndex(idx);
    m_catFilter->blockSignals(false);
}

void TransactionHistoryWindow::applyFilters()
{
    m_filtered.clear();
    QString search  = m_searchInput->text().trimmed().toLower();
    int     typeIdx = m_typeFilter->currentIndex();   // 0=all, 1=income, 2=expense
    QString catSel  = m_catFilter->currentText();

    for (const auto& tx : std::as_const(m_allTransactions)) {
        // Filter by Type
        if (typeIdx == 1 && tx.get_type() != Transaction::Income)  continue;
        if (typeIdx == 2 && tx.get_type() != Transaction::Expense) continue;

        // Filter by Category
        if (catSel != "All Categories" && tx.get_category() != catSel) continue;

        // Filter by Search (Description or Category)
        if (!search.isEmpty()) {
            if (!tx.get_description().toLower().contains(search) &&
                !tx.get_category().toLower().contains(search)) continue;
        }
        m_filtered.push_back(tx);
    }
}

void TransactionHistoryWindow::renderList()
{
    // Clear layout — use deleteLater so widgets get cleaned up safely
    QLayoutItem* item;
    while ((item = m_listLayout->takeAt(0)) != nullptr) {
        if (QWidget* w = item->widget()) w->deleteLater();
        delete item;
    }

    if (m_filtered.empty()) {
        m_emptyLabel->show();
    } else {
        m_emptyLabel->hide();
        for (const auto& tx : std::as_const(m_filtered)) {
            m_listLayout->addWidget(makeRow(tx));
        }
    }
}

QWidget* TransactionHistoryWindow::makeRow(const Transaction& tx)
{
    QFrame* row = new QFrame();
    row->setObjectName("tableRow");
    QHBoxLayout* layout = new QHBoxLayout(row);

    bool isIncome = (tx.get_type() == Transaction::Income);

    QLabel* date = new QLabel(tx.get_transac_date().date().toString("MMM d, yyyy"));
    QLabel* desc = new QLabel(tx.get_description().isEmpty() ? tx.get_category() : tx.get_description());
    desc->setStyleSheet("font-weight: bold; color: #E6EDF3;");

    QLabel* cat = new QLabel(tx.get_category());
    cat->setObjectName("txCategoryPill");

    QString sign = isIncome ? "+" : "-";
    QLabel* amt = new QLabel(sign + " Rs " + QString::number(tx.get_amount(), 'f', 2));
    amt->setObjectName(isIncome ? "txAmountIncome" : "txAmountExpense");

    QPushButton* delBtn = new QPushButton("✕");
    delBtn->setFixedSize(32, 32);
    delBtn->setCursor(Qt::PointingHandCursor);
    delBtn->setStyleSheet
        (
        "QPushButton "
        "{"
        "  background-color:rgba(248,81,73,0.15);"
        "  color: #f85149;"
        "  border: 1px solid rgba(248,81,73,0.4);"
        "  border-radius: 8px;"
        "  font-size: 14px;"
        "  font-weight: bold;"
        "}"
        "QPushButton:hover "
        "{"
        "  background-color: rgba(248,81,73,0.3);"
        "  border-color: #f85149;"
        "}"
        );
    // Capture id by value — `tx` goes out of scope after this returns
    int txId = tx.get_transaction_id();
    connect(delBtn, &QPushButton::clicked, this, [this, txId](){ onDeleteClicked(txId); });

    layout->addWidget(date, 2);
    layout->addWidget(desc, 4);
    layout->addWidget(cat, 2);
    layout->addWidget(amt, 2, Qt::AlignRight);
    layout->addWidget(delBtn, 1);

    return row;
}


// =============================================================================
//  Summary calculations
// =============================================================================
double TransactionHistoryWindow::sumByType(const std::vector<Transaction>& list,
                                           Transaction::Type type) const
{
    double total = 0.0;
    for (const auto& tx : std::as_const(list))
        if (tx.get_type() == type) total += tx.get_amount();
    return total;
}

void TransactionHistoryWindow::updateSummaryBar()
{
    double income  = sumByType(m_filtered, Transaction::Income);
    double expense = sumByType(m_filtered, Transaction::Expense);
    double net     = income - expense;

    m_incomeLabel ->setText("Rs " + QString::number(income,  'f', 2));
    m_expenseLabel->setText("Rs " + QString::number(expense, 'f', 2));
    bool isZero = qFuzzyIsNull(net);

    //CHANGE (Bug Fix): When Net amount 0.00 Color will be grey and no Plus Sign
    QString sign = isZero ? "" : (net > 0 ? "+" : "-");
    m_netLabel->setText(sign + QString(" Rs ")+ QString::number(std::abs(net), 'f', 2));
    m_countLabel->setText(QString::number(m_filtered.size()));

    //on 0.00 color Grey otherwise green or red
    QString netColor = isZero ? "#8b949e" : (net > 0 ? "#10b981" : "#f85149");
    m_netLabel->setStyleSheet(QString("color: %1; font-size: 18px; font-weight: 700;").arg(netColor));
}


// =============================================================================
//  Category Emoji Helper
// =============================================================================
QString TransactionHistoryWindow::categoryEmoji(const QString& category) const
{
    static const QMap<QString, QString> map = {
        {"Food",          "🍔"},
        {"Transport",     "🚗"},
        {"Rent",          "🏠"},
        {"Tuition",       "📚"},
        {"Entertainment", "🎮"},
        {"Other",         "📦"}
    };
    return map.value(category, "💰");
}


// =============================================================================
//  Slots & Helpers
// =============================================================================

void TransactionHistoryWindow::refreshHistory()
{
    loadData();
    applyFilters();
    renderList();
    updateSummaryBar();
}

void TransactionHistoryWindow::onDeleteClicked(int transactionId)
{
    auto reply = QMessageBox::question(this, "Confirm Delete", "Delete this transaction?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        TransactionService service;
        service.deleteTransaction(transactionId);
        refreshHistory();
    }
}

void TransactionHistoryWindow::onBackClicked()
{
    DashboardWindow* dashboard = new DashboardWindow(m_user, nullptr);
    dashboard->show();
    close();
}

void TransactionHistoryWindow::onSearchChanged(const QString&)
{
    applyFilters();
    renderList();
    updateSummaryBar();
}

void TransactionHistoryWindow::onTypeFilterChanged(int)
{
    applyFilters();
    renderList();
    updateSummaryBar();
}

void TransactionHistoryWindow::onCatFilterChanged(int)
{
    applyFilters();
    renderList();
    updateSummaryBar();
}

void TransactionHistoryWindow::onClearFilters()
{
    m_searchInput->blockSignals(true);
    m_searchInput->clear();
    m_searchInput->blockSignals(false);

    m_typeFilter->blockSignals(true);
    m_typeFilter->setCurrentIndex(0);
    m_typeFilter->blockSignals(false);

    m_catFilter->blockSignals(true);
    m_catFilter->setCurrentIndex(0);
    m_catFilter->blockSignals(false);

    applyFilters();
    renderList();
    updateSummaryBar();
}