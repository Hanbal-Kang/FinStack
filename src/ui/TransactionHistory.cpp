#include "ui/TransactionHistory.h"
#include "services/TransactionService.h"
#include <QScrollArea>
#include <QMessageBox>

// ─────────────────────────────────────────────────────────────────────────────
//  Constructor
// ─────────────────────────────────────────────────────────────────────────────

TransactionHistory::TransactionHistory(const User& user, QWidget* parent)
    : BaseWindow(parent), m_user(user)
{
    // Sets standard layout size for the history view
    setupWindowGeometry(1100, 720);
    initialize();
}

// ─────────────────────────────────────────────────────────────────────────────
//  setupUI — Builds the scrollable table area
// ─────────────────────────────────────────────────────────────────────────────

void TransactionHistory::setupUI()
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

    // Building the UI sections
    buildHeader(pageLayout);
    buildSummaryCards(pageLayout);
    buildFilterBar(pageLayout);
    buildTableHeader(pageLayout);

    // Container for the dynamic rows
    QWidget* tableArea = new QWidget();
    m_tableRows = new QVBoxLayout(tableArea);
    m_tableRows->setContentsMargins(0, 0, 0, 0);
    m_tableRows->setSpacing(8);
    pageLayout->addWidget(tableArea);

    pageLayout->addStretch();
    outerScroll->setWidget(page);

    QVBoxLayout* root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->addWidget(outerScroll);

    refreshHistory();
}

void TransactionHistory::applyStyles()
{
    loadGlobalStylesheet();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Data Handling
// ─────────────────────────────────────────────────────────────────────────────

void TransactionHistory::loadData()
{
    // Fetches all transactions for the current user
    TransactionService service;
    m_allTransactions = service.getAllByUser(m_user.get_id());
}

void TransactionHistory::refreshHistory()
{
    loadData();
    renderRows();
}

void TransactionHistory::renderRows()
{
    // Clear existing rows first
    while (m_tableRows->count() > 0) {
        QLayoutItem* item = m_tableRows->takeAt(0);
        if (item->widget()) delete item->widget();
        delete item;
    }

    if (m_allTransactions.empty()) {
        m_tableRows->addWidget(new QLabel("No transactions found."));
        return;
    }

    // Add each transaction as a styled row
    for (const auto& tx : m_allTransactions) {
        m_tableRows->addWidget(makeRow(tx));
    }
    m_tableRows->addStretch();
}

QWidget* TransactionHistory::makeRow(const Transaction& tx)
{
    QFrame* row = new QFrame();
    row->setObjectName("txRow");
    row->setFixedHeight(60);

    QHBoxLayout* layout = new QHBoxLayout(row);
    layout->setContentsMargins(15, 0, 15, 0);

    bool isIncome = (tx.get_type() == Transaction::Income);

    QLabel* desc = new QLabel(tx.get_description());
    desc->setStyleSheet("font-weight: bold; color: #E6EDF3;");

    QLabel* amt = new QLabel((isIncome ? "+ Rs " : "- Rs ") + QString::number(tx.get_amount(), 'f', 2));
    amt->setStyleSheet(isIncome ? "color: #3FB950;" : "color: #F85149;");
    amt->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QPushButton* delBtn = new QPushButton("🗑");
    delBtn->setFixedSize(30, 30);
    delBtn->setCursor(Qt::PointingHandCursor);
    connect(delBtn, &QPushButton::clicked, this, [this, tx](){ onDeleteClicked(tx.get_transaction_id()); });

    layout->addWidget(desc, 1);
    layout->addWidget(amt);
    layout->addWidget(delBtn);

    return row;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Slots
// ─────────────────────────────────────────────────────────────────────────────

void TransactionHistory::onDeleteClicked(int transactionId)
{
    auto reply = QMessageBox::question(this, "Delete", "Delete this record?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        TransactionService service;
        service.deleteTransaction(transactionId);
        refreshHistory();
    }
}

void TransactionHistory::onBackClicked()
{
    emit navigateBack();
    this->close();
}