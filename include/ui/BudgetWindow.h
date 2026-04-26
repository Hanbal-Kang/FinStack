#pragma once

#include "ui/BaseWindow.h"
#include "models/User.h"
#include "models/Budget.h"
#include "models/Transaction.h"

#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QFrame>
#include <QList>
#include <QMap>

// ─────────────────────────────────────────────────────────────────────────────
//  BudgetWindow
//
//  Inherits BaseWindow (Inheritance).
//  Overrides setupUI() and applyStyles() (Polymorphism / Abstraction).
//
//  Responsibilities:
//    - Show a top summary card: Total Budget | Total Spent | Remaining
//    - Overall progress bar across all categories
//    - 2-column grid of per-category cards, each showing:
//        emoji icon · category name · status badge (On Track / Warning / Over Budget)
//        monthly limit · spent · left · progress bar · edit button
//    - Inline editing: clicking the edit button reveals a QLineEdit to update
//      the limit for that category without opening a new window
// ─────────────────────────────────────────────────────────────────────────────
class BudgetWindow : public BaseWindow
{
    Q_OBJECT

public:
    explicit BudgetWindow(const User& user, QWidget* parent = nullptr);
    ~BudgetWindow() override = default;

    void refreshBudgets();

signals:
    void navigateBack();

protected:
    void setupUI()     override;
    void applyStyles() override;

private slots:
    void onBackClicked();
    void onEditClicked(int categoryIndex);
    void onSaveClicked(int categoryIndex);

private:
    void buildHeader(QVBoxLayout* root);
    void buildSummaryCard(QVBoxLayout* root);
    void buildCategoryGrid(QVBoxLayout* root);

    QFrame* makeCategoryCard(int index, const QString& emoji, const QString& category, double limit, double spent);

    void    loadData();
    double  totalBudget()   const;
    double  totalSpent()    const;
    QString statusLabel(double spent, double limit) const;
    QString barStatus(double spent, double limit)   const;

    User               m_user;
    QList<Budget>      m_budgets;
    QList<Transaction> m_transactions;

    QMap<QString, double> m_spent;

    QLabel*      m_totalBudgetLabel    = nullptr;
    QLabel*      m_totalSpentLabel     = nullptr;
    QLabel*      m_remainingLabel      = nullptr;
    QProgressBar* m_overallBar         = nullptr;
    QLabel*      m_overallPctLabel     = nullptr;

    QList<QLineEdit*>    m_editInputs;
    QList<QPushButton*>  m_saveButtons;
    QList<QPushButton*>  m_editButtons;

    QGridLayout* m_gridLayout = nullptr;
    QWidget*     m_gridWidget = nullptr;

    struct CategoryMeta { QString emoji; QString name; };
    const QList<CategoryMeta> m_categories = {
        { "🍔", "Food"          }, { "🚗", "Transport"     }, { "🏠", "Rent"          }, { "📚", "Tuition"       }, { "🎮", "Entertainment" }, { "📦", "Other"         }, };
};
