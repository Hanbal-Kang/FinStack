#pragma once
#include "ui/BaseWindow.h"
#include "models/User.h"
#include "models/Transaction.h"
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <vector>


class TransactionHistoryWindow : public BaseWindow
{
    Q_OBJECT

public:
    explicit TransactionHistoryWindow(const User& user, QWidget* parent = nullptr);
    ~TransactionHistoryWindow() override = default;

    //call after a new deposit/withdrawal so the list updates
    void refreshHistory();

signals:
    void navigateBack();

protected:
    void setupUI()     override;
    void applyStyles() override;

private:
    User                     m_user;
    std::vector<Transaction> m_allTransactions;
    std::vector<Transaction> m_filtered;

    //summary card labels
    QLabel* m_incomeTotalLabel  = nullptr;
    QLabel* m_expenseTotalLabel = nullptr;
    QLabel* m_netLabel          = nullptr;

    //filter bar
    QLineEdit*   m_searchInput = nullptr;
    QPushButton* m_allBtn      = nullptr;
    QPushButton* m_incomeBtn   = nullptr;
    QPushButton* m_expenseBtn  = nullptr;
    QString      m_activeFilter = "All";   // "All" | "Income" | "Expense"

    //table
    QVBoxLayout* m_tableRows  = nullptr;   // holds one row-widget per transaction
    QLabel*      m_emptyLabel = nullptr;

    //layout builders
    void buildHeader(QVBoxLayout* root);
    void buildSummaryCards(QVBoxLayout* root);
    void buildFilterBar(QVBoxLayout* root);
    void buildTableHeader(QVBoxLayout* root);
    void buildTableArea(QVBoxLayout* root);

    //data / render
    void    loadData();
    void    applyFilters();
    void    renderRows();
    QWidget* makeRow(const Transaction& tx);

    //summary helpers
    double sumByType(Transaction::Type type) const;
    void   updateSummaryCards();

    //filter-button toggle helper
    void setActiveFilter(const QString& filter);

private slots:
    void onSearchChanged(const QString& text);
    void onAllClicked();
    void onIncomeClicked();
    void onExpenseClicked();
    void onDeleteClicked(int transactionId);
    void onBackClicked();
};