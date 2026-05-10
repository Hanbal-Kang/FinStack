// #pragma once
// #include "ui/BaseWindow.h"
// #include "models/User.h"
// #include "models/Transaction.h"
// #include <QLabel>
// #include <QPushButton>
// #include <QLineEdit>
// #include <QComboBox>
// #include <QFrame>
// #include <QVBoxLayout>
// #include <QHBoxLayout>
// #include <QScrollArea>
// #include <vector>


// class TransactionHistoryWindow : public BaseWindow
// {
//     Q_OBJECT

// public:
//     explicit TransactionHistoryWindow(const User& user, QWidget* parent = nullptr);
//     ~TransactionHistoryWindow() override = default;

//     // calling this after a deposit/withdrawal list refreshes
//     void refreshHistory();

// signals:
//     void navigateBack();

// protected:
//     void setupUI()     override;
//     void applyStyles() override;

// private:
//     User                     m_user;
//     std::vector<Transaction> m_allTransactions;    // raw data from DB
//     std::vector<Transaction> m_filtered;           // currently displayed

//     //header / filter widgets
//     QPushButton* m_backBtn      = nullptr;
//     QLineEdit*   m_searchInput  = nullptr;
//     QComboBox*   m_typeFilter   = nullptr;
//     QComboBox*   m_catFilter    = nullptr;
//     QPushButton* m_clearBtn     = nullptr;

//     //summary bar
//     QLabel* m_incomeLabel  = nullptr;
//     QLabel* m_expenseLabel = nullptr;
//     QLabel* m_netLabel     = nullptr;
//     QLabel* m_countLabel   = nullptr;

//     //transaction list
//     QVBoxLayout* m_listLayout  = nullptr;
//     QLabel*      m_emptyLabel  = nullptr;

//     //setup helpers
//     void buildHeader(QVBoxLayout* root);
//     void buildFilterBar(QVBoxLayout* root);
//     void buildSummaryBar(QVBoxLayout* root);
//     void buildList(QVBoxLayout* root);

//     //data helpers
//     void loadData();
//     void applyFilters();
//     void renderList();
//     QWidget* makeRow(const Transaction& tx);

//     //summary calculations
//     double sumByType(const std::vector<Transaction>& list,
//                      Transaction::Type type) const;
//     void   updateSummaryBar();

//     QString categoryEmoji(const QString& category) const;

// private slots:
//     void onSearchChanged(const QString& text);
//     void onTypeFilterChanged(int index);
//     void onCatFilterChanged(int index);
//     void onClearFilters();
//     void onDeleteClicked(int transactionId);
//     void onBackClicked();
// };
