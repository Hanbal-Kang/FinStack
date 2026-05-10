#pragma once
#include "ui/BaseWindow.h"
#include "models/User.h"
#include "models/RecurringTransaction.h"
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <vector>

//Dialog for creating a new recurring rule
class AddRecurringDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddRecurringDialog(QWidget* parent = nullptr);

    QString getDescription() const;
    QString getType()        const;   //income or expense
    double  getAmount()      const;
    QString getCategory()    const;
    QString getInterval()    const;   //daily/ weekly / monthly (3 intervals for now)
    QDate   getStartDate()   const;

private:
    QLineEdit* m_descInput     = nullptr;
    QComboBox* m_typeCombo     = nullptr;
    QLineEdit* m_amountInput   = nullptr;
    QComboBox* m_categoryCombo = nullptr;
    QComboBox* m_intervalCombo = nullptr;
    QDateEdit* m_startDateEdit = nullptr;
    QLabel*    m_errorLabel    = nullptr;

    void setupUI();

private slots:
    void onConfirmClicked();
};

//The main window
class RecurringTransactionWindow : public BaseWindow
{
    Q_OBJECT
public:
    explicit RecurringTransactionWindow(const User& user, QWidget* parent = nullptr);
    ~RecurringTransactionWindow() override = default;

protected:
    void setupUI()     override;
    void applyStyles() override;

private:
    User                              m_user;
    std::vector<RecurringTransaction> m_rules;

    //header
    QPushButton* m_addBtn   = nullptr;
    QPushButton* m_backBtn  = nullptr;
    QLabel*      m_subtitleLabel = nullptr;

    //summary cards
    QLabel* m_incomeLabel  = nullptr;
    QLabel* m_expenseLabel = nullptr;
    QLabel* m_activeLabel  = nullptr;

    //list
    QVBoxLayout* m_listLayout = nullptr;
    QLabel*      m_emptyLabel = nullptr;

    //builders
    void buildHeader(QVBoxLayout* root);
    void buildSummaryRow(QVBoxLayout* root);
    void buildList(QVBoxLayout* root);

    //data
    void loadRules();
    void refreshAll();
    QFrame* buildRuleRow(const RecurringTransaction& r);
    QString iconForCategory(const QString& cat) const;

private slots:
    void onAddClicked();
    void onToggleActiveClicked(int recurringId, bool currentlyActive);
    void onDeleteClicked(int recurringId);
    void onBackClicked();
};