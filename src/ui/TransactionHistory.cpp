#include "ui/TransactionHistory.h"
#include "services/TransactionService.h"
#include <QScrollArea>
#include <QMessageBox>
#include <QStyle>

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

    // Assembly using your builders
    buildHeader(pageLayout);
    buildSummaryCards(pageLayout);
    buildFilterBar(pageLayout);
    buildTableHeader(pageLayout);
    buildTableArea(pageLayout);

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


//  Data & Rendering Logic

void TransactionHistoryWindow::loadData()
{
    TransactionService service;
    m_allTransactions = service.getAllByUser(m_user.get_id());
}

void TransactionHistoryWindow::applyFilters()
{
    m_filtered.clear();
    QString search = m_searchInput->text().trimmed().toLower();

    for (const auto& tx : m_allTransactions) {
        // Filter by Type
        if (m_activeFilter == "Income" && tx.get_type() != Transaction::Income) continue;
        if (m_activeFilter == "Expense" && tx.get_type() != Transaction::Expense) continue;

        // Filter by Search (Description or Category)
        if (!search.isEmpty()) {
            if (!tx.get_description().toLower().contains(search) &&
                !tx.get_category().toLower().contains(search)) continue;
        }
        m_filtered.push_back(tx);
    }
}

void TransactionHistoryWindow::renderRows()
{
    // Clear layout
    while (m_tableRows->count() > 0) {
        QLayoutItem* item = m_tableRows->takeAt(0);
        if (item->widget()) delete item->widget();
        delete item;
    }

    if (m_filtered.empty()) {
        m_emptyLabel->show();
    } else {
        m_emptyLabel->hide();
        for (const auto& tx : m_filtered) {
            m_tableRows->addWidget(makeRow(tx));
        }
    }
    m_tableRows->addStretch();
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

    QPushButton* delBtn = new QPushButton("🗑");
    delBtn->setObjectName("navIconBtn");
    delBtn->setFixedSize(30, 30);
    connect(delBtn, &QPushButton::clicked, this, [this, tx](){ onDeleteClicked(tx.get_transaction_id()); });

    layout->addWidget(date, 2);
    layout->addWidget(desc, 4);
    layout->addWidget(cat, 2);
    layout->addWidget(amt, 2, Qt::AlignRight);
    layout->addWidget(delBtn, 1);

    return row;
}


//  Slots & Helpers


void TransactionHistoryWindow::refreshHistory()
{
    loadData();
    applyFilters();
    renderRows();
    updateSummaryCards();
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

void TransactionHistoryWindow::setActiveFilter(const QString& filter)
{
    m_activeFilter = filter;

    //Repolish buttons for CSS active state updates
    auto repolish = [](QWidget* w) {
        w->style()->unpolish(w);
        w->style()->polish(w);
        w->update();
    };

    m_allBtn->setObjectName(filter == "All" ? "filterBtnActive" : "filterBtn");
    m_incomeBtn->setObjectName(filter == "Income" ? "filterBtnActive" : "filterBtn");
    m_expenseBtn->setObjectName(filter == "Expense" ? "filterBtnActive" : "filterBtn");

    repolish(m_allBtn); repolish(m_incomeBtn); repolish(m_expenseBtn);
    applyFilters();
    renderRows();
}

void TransactionHistoryWindow::onBackClicked() { emit navigateBack(); close(); }
void TransactionHistoryWindow::onSearchChanged(const QString&) { applyFilters(); renderRows(); }
void TransactionHistoryWindow::onAllClicked()     { setActiveFilter("All"); }
void TransactionHistoryWindow::onIncomeClicked()  { setActiveFilter("Income"); }
void TransactionHistoryWindow::onExpenseClicked() { setActiveFilter("Expense"); }