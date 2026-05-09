#pragma once
#include "ui/BaseWindow.h"
#include "models/User.h"
#include "models/SavingsGoal.h"
#include <QDate>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QDateEdit>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
//  AddGoalDialog
// ─────────────────────────────────────────────────────────────────────────────
class AddGoalDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AddGoalDialog(QWidget* parent = nullptr);

    QString getGoalName()  const;
    double  getTargetAmt() const;
    QDate   getDeadline()  const;

private:
    QLineEdit* m_nameInput   = nullptr;
    QLineEdit* m_amountInput = nullptr;
    QDateEdit* m_dateEdit    = nullptr;
    QLabel*    m_errorLabel  = nullptr;

    void setupUI();

private slots:
    void onConfirmClicked();
};

// ─────────────────────────────────────────────────────────────────────────────
//  SavingsGoalWindow  (was incorrectly named GoalsWindow)
// ─────────────────────────────────────────────────────────────────────────────
class SavingsGoalWindow : public BaseWindow
{
    Q_OBJECT
public:
    explicit SavingsGoalWindow(const User& user, QWidget* parent = nullptr);
    ~SavingsGoalWindow() override = default;

signals:
    void backToDashboard();

protected:
    void setupUI()     override;
    void applyStyles() override;

private:
    User                     m_user;
    std::vector<SavingsGoal> m_goals;
    double                   m_balance;

    // header widgets
    QLabel*      m_balanceLabel = nullptr;
    QPushButton* m_addBtn       = nullptr;
    QPushButton* m_backBtn      = nullptr;

    // goals grid
    QFrame* m_goalsGrid  = nullptr;
    QLabel* m_emptyLabel = nullptr;

    // layout builders — each adds its widget(s) to root
    void buildHeader(QVBoxLayout* root);
    void buildBalanceBanner(QVBoxLayout* root);
    void buildGoalsGrid(QVBoxLayout* root);

    // data
    void   loadGoals();
    void   refreshGoalCards();
    double computeBalance();

    QString getGoalStatus(const SavingsGoal& goal) const;
    QFrame* buildGoalCard(const SavingsGoal& goal);
    QString goalEmoji(const QString& description)  const;

private slots:
    void onAddGoalClicked();
    void onDeleteGoalClicked(int goalId);
    void onBackClicked();
};