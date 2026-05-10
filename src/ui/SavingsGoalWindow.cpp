#include "ui/SavingsGoalWindow.h"
#include "ui/DashboardWindow.h"
#include "services/GoalService.h"
#include "services/TransactionService.h"
#include "utils/Validator.h"

#include <QProgressBar>
#include <QGridLayout>
#include <QScrollArea>
#include <QMessageBox>
#include <QDialogButtonBox>
#include <QFormLayout>

// =============================================================================
//  AddGoalDialog — the modal popup for creating a new goal
// =============================================================================
AddGoalDialog::AddGoalDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("New Savings Goal");
    setModal(true);
    setMinimumWidth(420);
    setupUI();
}

void AddGoalDialog::setupUI()
{
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(16);

    // Title
    QLabel* title = new QLabel("Create a New Goal");
    title->setObjectName("cardTitle");
    root->addWidget(title);

    QLabel* sub = new QLabel("Set a target you want to save toward.");
    sub->setObjectName("pageSubtitle");
    root->addWidget(sub);

    // Goal name
    QLabel* nameLbl = new QLabel("Goal Name");
    nameLbl->setObjectName("inputLabel");
    m_nameInput = new QLineEdit();
    m_nameInput->setPlaceholderText("e.g. New laptop, Trip to Hunza");
    m_nameInput->setFixedHeight(40);
    m_nameInput->setObjectName("inputField");
    root->addWidget(nameLbl);
    root->addWidget(m_nameInput);

    // Target amount
    QLabel* amtLbl = new QLabel("Target Amount (Rs)");
    amtLbl->setObjectName("inputLabel");
    m_amountInput = new QLineEdit();
    m_amountInput->setPlaceholderText("0.00");
    m_amountInput->setFixedHeight(40);
    m_amountInput->setObjectName("inputField");
    root->addWidget(amtLbl);
    root->addWidget(m_amountInput);

    // Deadline
    QLabel* dateLbl = new QLabel("Deadline");
    dateLbl->setObjectName("inputLabel");
    m_dateEdit = new QDateEdit();
    m_dateEdit->setFixedHeight(40);
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setDate(QDate::currentDate().addMonths(3));   // sensible default
    m_dateEdit->setMinimumDate(QDate::currentDate());          // can't pick past dates
    root->addWidget(dateLbl);
    root->addWidget(m_dateEdit);

    // Error label (hidden until validation fails)
    m_errorLabel = new QLabel();
    m_errorLabel->setObjectName("txErrorLabel");
    m_errorLabel->setStyleSheet("color: #f85149;");
    m_errorLabel->hide();
    root->addWidget(m_errorLabel);

    // Buttons
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->setSpacing(12);

    QPushButton* cancelBtn = new QPushButton("Cancel");
    cancelBtn->setObjectName("primaryBtn");
    cancelBtn->setFixedHeight(40);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    QPushButton* createBtn = new QPushButton("Create Goal");
    createBtn->setObjectName("primaryBtn");
    createBtn->setFixedHeight(40);
    createBtn->setCursor(Qt::PointingHandCursor);
    createBtn->setDefault(true);
    connect(createBtn, &QPushButton::clicked, this, &AddGoalDialog::onConfirmClicked);

    btnRow->addStretch();
    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(createBtn);
    root->addLayout(btnRow);
}

void AddGoalDialog::onConfirmClicked()
{
    QString name = m_nameInput->text().trimmed();
    QString amt  = m_amountInput->text().trimmed();

    if (name.isEmpty()) {
        m_errorLabel->setText("Please enter a goal name");
        m_errorLabel->show();
        return;
    }
    if (!Validator::isValidAmount(amt)) {
        m_errorLabel->setText("Please enter a valid amount greater than 0");
        m_errorLabel->show();
        return;
    }

    accept();   // closes the dialog with Accepted result
}

QString AddGoalDialog::getGoalName()  const { return m_nameInput->text().trimmed(); }
double  AddGoalDialog::getTargetAmt() const { return m_amountInput->text().toDouble(); }
QDate   AddGoalDialog::getDeadline()  const { return m_dateEdit->date(); }

// =============================================================================
//  ContributeDialog — popup for adding money to an existing goal
// =============================================================================
ContributeDialog::ContributeDialog(double maxAmount, QWidget* parent)
    : QDialog(parent), m_maxAmount(maxAmount)
{
    setWindowTitle("Contribute to Goal");
    setModal(true);
    setMinimumWidth(380);
    setupUI();
}

void ContributeDialog::setupUI()
{
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(16);

    QLabel* title = new QLabel("Add Money to Your Goal");
    title->setObjectName("cardTitle");
    root->addWidget(title);

    QLabel* sub = new QLabel(QString("Spendable balance: Rs %1")
                                 .arg(m_maxAmount, 0, 'f', 0));
    sub->setObjectName("pageSubtitle");
    root->addWidget(sub);

    QLabel* amtLbl = new QLabel("Amount (Rs)");
    amtLbl->setObjectName("inputLabel");
    m_amountInput = new QLineEdit();
    m_amountInput->setPlaceholderText("0.00");
    m_amountInput->setFixedHeight(40);
    m_amountInput->setObjectName("inputField");
    root->addWidget(amtLbl);
    root->addWidget(m_amountInput);

    m_errorLabel = new QLabel();
    m_errorLabel->setStyleSheet("color: #f85149;");
    m_errorLabel->hide();
    root->addWidget(m_errorLabel);

    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->setSpacing(12);

    QPushButton* cancelBtn = new QPushButton("Cancel");
    cancelBtn->setObjectName("primaryBtn");
    cancelBtn->setFixedHeight(40);
    cancelBtn->setCursor(Qt::PointingHandCursor);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);

    QPushButton* okBtn = new QPushButton("Contribute");
    okBtn->setObjectName("primaryBtn");
    okBtn->setFixedHeight(40);
    okBtn->setCursor(Qt::PointingHandCursor);
    okBtn->setDefault(true);
    connect(okBtn, &QPushButton::clicked, this, &ContributeDialog::onConfirmClicked);

    btnRow->addStretch();
    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(okBtn);
    root->addLayout(btnRow);
}

void ContributeDialog::onConfirmClicked()
{
    bool ok = false;
    double amount = m_amountInput->text().toDouble(&ok);

    if (!ok || amount <= 0) {
        m_errorLabel->setText("Please enter a valid amount greater than 0");
        m_errorLabel->show();
        return;
    }

    if (amount > m_maxAmount) {
        m_errorLabel->setText("You don't have enough spendable balance");
        m_errorLabel->show();
        return;
    }

    accept();
}

double ContributeDialog::getAmount() const
{
    return m_amountInput->text().toDouble();
}

// =============================================================================
//  SavingsGoalWindow — the main page
// =============================================================================
SavingsGoalWindow::SavingsGoalWindow(const User& user, QWidget* parent)
    : BaseWindow(parent), m_user(user), m_balance(0.0)
{
    setWindowTitle("FinStack — Savings Goals");
    setupWindowGeometry(1100, 720);
    initialize();
}

void SavingsGoalWindow::setupUI()
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
    buildBalanceBanner(pageLayout);
    buildGoalsGrid(pageLayout);

    pageLayout->addStretch();
    scroll->setWidget(page);
    root->addWidget(scroll, 1);

    // Initial data pull + render
    loadGoals();
    refreshGoalCards();
}

void SavingsGoalWindow::applyStyles()
{
    loadGlobalStylesheet();
}


// =============================================================================
//  Section builders
// =============================================================================
void SavingsGoalWindow::buildHeader(QVBoxLayout* root)
{
    // Top row: back button (left) | + Add Goal button (right)
    QHBoxLayout* topRow = new QHBoxLayout();
    m_backBtn = new QPushButton("←", this);
    m_backBtn->setObjectName("navIconBtn");
    m_backBtn->setFixedSize(36, 36);
    m_backBtn->setCursor(Qt::PointingHandCursor);
    connect(m_backBtn, &QPushButton::clicked, this, &SavingsGoalWindow::onBackClicked);

    m_addBtn = new QPushButton("+ Add Goal", this);
    m_addBtn->setObjectName("primaryBtn");
    m_addBtn->setFixedHeight(38);
    m_addBtn->setCursor(Qt::PointingHandCursor);
    connect(m_addBtn, &QPushButton::clicked, this, &SavingsGoalWindow::onAddGoalClicked);

    topRow->addWidget(m_backBtn);
    topRow->addStretch();
    topRow->addWidget(m_addBtn);
    root->addLayout(topRow);

    // Title row: icon + title + subtitle
    QHBoxLayout* titleRow = new QHBoxLayout();
    titleRow->setSpacing(14);

    QLabel* iconBox = new QLabel("🎯");
    iconBox->setObjectName("budgetTitleIcon");
    iconBox->setFixedSize(52, 52);
    iconBox->setAlignment(Qt::AlignCenter);

    QVBoxLayout* titleCol = new QVBoxLayout();
    titleCol->setSpacing(4);

    QLabel* title = new QLabel("Savings Goals");
    title->setObjectName("pageTitle");
    QLabel* subtitle = new QLabel("Track what you're saving toward and how close you are");
    subtitle->setObjectName("pageSubtitle");

    titleCol->addWidget(title);
    titleCol->addWidget(subtitle);

    titleRow->addWidget(iconBox);
    titleRow->addLayout(titleCol);
    titleRow->addStretch();
    root->addLayout(titleRow);
}

void SavingsGoalWindow::buildBalanceBanner(QVBoxLayout* root)
{
    QFrame* card = new QFrame();
    card->setObjectName("budgetSummaryCard");

    QHBoxLayout* h = new QHBoxLayout(card);
    h->setContentsMargins(28, 22, 28, 22);
    h->setSpacing(20);

    QVBoxLayout* col = new QVBoxLayout();
    col->setSpacing(4);

    QLabel* lbl = new QLabel("AVAILABLE BALANCE");
    lbl->setObjectName("infoCardLabel");

    m_balanceLabel = new QLabel("Rs 0");
    m_balanceLabel->setObjectName("summaryValue");
    m_balanceLabel->setStyleSheet("font-size: 22px; font-weight: 700; color: #ffffff;");

    col->addWidget(lbl);
    col->addWidget(m_balanceLabel);

    QLabel* hint = new QLabel("This is your current net balance from all transactions.");
    hint->setObjectName("budgetCatSub");

    h->addLayout(col);
    h->addStretch();
    h->addWidget(hint);

    root->addWidget(card);
}

void SavingsGoalWindow::buildGoalsGrid(QVBoxLayout* root)
{
    // m_goalsGrid is the wrapper QFrame; its layout is the actual QGridLayout.
    m_goalsGrid = new QFrame();
    m_goalsGrid->setObjectName("budgetSummaryCard");

    QGridLayout* grid = new QGridLayout(m_goalsGrid);
    grid->setContentsMargins(20, 18, 20, 18);
    grid->setHorizontalSpacing(16);
    grid->setVerticalSpacing(16);

    // Title at row 0 spanning 2 columns
    QLabel* title = new QLabel("Your Goals");
    title->setObjectName("cardTitle");
    grid->addWidget(title, 0, 0, 1, 2);

    // Empty-state label, also at row 0 (will be hidden when there are goals).
    // We add it at column 0 spanning 2 columns, but visually it shows below
    // the title because we hide the title when empty? No — keep both visible:
    // empty label sits at row 1 if no goals.
    m_emptyLabel = new QLabel("No goals yet. Click \"+ Add Goal\" to set your first target.");
    m_emptyLabel->setObjectName("budgetCatSub");
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    grid->addWidget(m_emptyLabel, 1, 0, 1, 2);

    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);

    root->addWidget(m_goalsGrid);
}


// =============================================================================
//  Goal card — one card per goal
// =============================================================================
QFrame* SavingsGoalWindow::buildGoalCard(const SavingsGoal& goal)
{
    QFrame* card = new QFrame();
    card->setObjectName("budgetCatCard");

    QVBoxLayout* cl = new QVBoxLayout(card);
    cl->setContentsMargins(20, 18, 20, 18);
    cl->setSpacing(12);

    double saved     = goal.get_saved_amount();
    double target    = goal.get_target_amount();
    double remaining = target - saved;
    double pct       = (target > 0) ? qMin(saved / target * 100.0, 100.0) : 0.0;

    // Header row: emoji + name (left), status badge (right)
    QHBoxLayout* headerRow = new QHBoxLayout();
    headerRow->setSpacing(10);

    QLabel* emoji = new QLabel(goalEmoji(goal.get_description()));
    emoji->setObjectName("categoryEmoji");
    emoji->setFixedSize(36, 36);
    emoji->setAlignment(Qt::AlignCenter);

    QVBoxLayout* nameCol = new QVBoxLayout();
    nameCol->setSpacing(2);

    QLabel* name = new QLabel(goal.get_description());
    name->setObjectName("budgetCatName");

    QString deadlineText = goal.get_deadline().isValid()
                               ? "By " + goal.get_deadline().date().toString("MMM d, yyyy")
                               : "No deadline";
    QLabel* deadline = new QLabel(deadlineText);
    deadline->setObjectName("budgetCatSub");

    nameCol->addWidget(name);
    nameCol->addWidget(deadline);

    QString status = getGoalStatus(goal);
    QLabel* badge = new QLabel(status);
    if (status == "Achieved")
        badge->setStyleSheet("color:#14532D; background:#DCFCE7; border-radius:6px; padding:3px 10px; font-weight:600;");
    else if (status == "At Risk")
        badge->setStyleSheet("color:#991B1B; background:#FEE2E2; border-radius:6px; padding:3px 10px; font-weight:600;");
    else
        badge->setStyleSheet("color:#14532D; background:#DCFCE7; border-radius:6px; padding:3px 10px; font-weight:600;");

    headerRow->addWidget(emoji);
    headerRow->addLayout(nameCol);
    headerRow->addStretch();
    headerRow->addWidget(badge);

    cl->addLayout(headerRow);

    // Amount row: saved (green) | "Still Needed" / "Surplus" | remaining
    QHBoxLayout* amtRow = new QHBoxLayout();
    QLabel* curLbl = new QLabel("Rs " + QString::number(saved, 'f', 0));
    curLbl->setStyleSheet("color: #22C55E; font-weight: bold; font-size: 18px;");
    QLabel* remLbl = new QLabel("Rs " + QString::number(qAbs(remaining), 'f', 0));
    remLbl->setStyleSheet("color: #E6EDF3; font-weight: bold; font-size: 18px;");

    QLabel* remTitle = new QLabel(remaining <= 0 ? "Surplus" : "Still Needed");
    remTitle->setObjectName("budgetCatSub");

    amtRow->addWidget(curLbl);
    amtRow->addStretch();
    amtRow->addWidget(remTitle);
    amtRow->addWidget(remLbl);
    cl->addLayout(amtRow);

    // Progress bar
    QProgressBar* bar = new QProgressBar();
    bar->setRange(0, 100);
    bar->setValue(static_cast<int>(pct));
    bar->setFixedHeight(8);
    bar->setTextVisible(false);
    bar->setObjectName("budgetBar");
    cl->addWidget(bar);

    QLabel* pctLabel = new QLabel(QString::number(pct, 'f', 0) + "% complete");
    pctLabel->setObjectName("budgetCatSub");
    cl->addWidget(pctLabel);

    //Action row: contribute button (left) + delete button (right)
    QHBoxLayout* actionRow = new QHBoxLayout();
    actionRow->setSpacing(8);

    int gid = goal.get_savings_goal_id();   // capture by value — goal is local

    QPushButton* contribBtn = new QPushButton("+ Add Money");
    contribBtn->setObjectName("primaryBtn");
    contribBtn->setFixedHeight(32);
    contribBtn->setCursor(Qt::PointingHandCursor);
    connect(contribBtn, &QPushButton::clicked, this, [this, gid]() { onContributeClicked(gid); });

    QPushButton* delBtn = new QPushButton("Delete");
    delBtn->setObjectName("dangerBtn");
    delBtn->setFixedHeight(32);
    delBtn->setCursor(Qt::PointingHandCursor);
    connect(delBtn, &QPushButton::clicked, this, [this, gid]() { onDeleteGoalClicked(gid); });

    actionRow->addWidget(contribBtn);
    actionRow->addStretch();
    actionRow->addWidget(delBtn);
    cl->addLayout(actionRow);

    return card;
}


// =============================================================================
//  Helpers — status, emoji
// =============================================================================
QString SavingsGoalWindow::getGoalStatus(const SavingsGoal& goal) const
{
    double saved  = goal.get_saved_amount();
    double target = goal.get_target_amount();
    if (target <= 0) return "On Track";
    if (saved >= target) return "Achieved";

    // "At Risk" = deadline within 14 days AND less than 50% saved
    if (goal.get_deadline().isValid()) {
        qint64 daysLeft = QDate::currentDate().daysTo(goal.get_deadline().date());
        double pct = saved / target;
        if (daysLeft <= 14 && pct < 0.5) return "At Risk";
    }
    return "On Track";
}

QString SavingsGoalWindow::goalEmoji(const QString& description) const
{
    QString d = description.toLower();
    if (d.contains("car"))                                   return "🚗";
    if (d.contains("laptop") || d.contains("pc") || d.contains("computer")) return "💻";
    if (d.contains("phone") || d.contains("iphone"))         return "📱";
    if (d.contains("trip") || d.contains("travel") || d.contains("vacation")) return "✈️";
    if (d.contains("home") || d.contains("house"))           return "🏠";
    if (d.contains("wedding"))                               return "💍";
    if (d.contains("emergency"))                             return "🚨";
    if (d.contains("education") || d.contains("school") || d.contains("tuition")) return "🎓";
    return "🎯";
}


// =============================================================================
//  Data helpers (kept from partner's version, with small fixes)
// =============================================================================
void SavingsGoalWindow::loadGoals()
{
    m_balance = computeBalance();
    if (m_balanceLabel)
        m_balanceLabel->setText("Rs " + QString::number(m_balance, 'f', 0));

    GoalService service;
    m_goals = service.getGoalsForUser(m_user.get_id());
}

double SavingsGoalWindow::computeBalance()
{
    TransactionService service;
    std::vector<Transaction> all = service.getAllByUser(m_user.get_id());

    double currentBalance = 0.0;
    for (const auto& tx : all) {
        if (tx.get_type() == Transaction::Income)
            currentBalance += tx.get_amount();
        else
            currentBalance -= tx.get_amount();
    }
    return currentBalance;
}

void SavingsGoalWindow::refreshGoalCards()
{
    QGridLayout* grid = qobject_cast<QGridLayout*>(m_goalsGrid->layout());
    if (!grid) return;

    // Clear all items except the title (row 0). takeAt(1) keeps stripping
    // from index 1 onward until only the title remains.
    while (grid->count() > 1) {
        QLayoutItem* item = grid->takeAt(1);
        if (item) {
            if (QWidget* w = item->widget()) w->deleteLater();
            delete item;
        }
    }

    if (m_goals.empty()) {
        // Re-add the empty label at row 1
        m_emptyLabel = new QLabel("No goals yet. Click \"+ Add Goal\" to set your first target.");
        m_emptyLabel->setObjectName("budgetCatSub");
        m_emptyLabel->setAlignment(Qt::AlignCenter);
        grid->addWidget(m_emptyLabel, 1, 0, 1, 2);
        return;
    }

    // 2-column grid of cards
    int col = 0, row = 1;
    for (const auto& goal : m_goals) {
        grid->addWidget(buildGoalCard(goal), row, col);
        col++;
        if (col == 2) { col = 0; row++; }
    }
}


// =============================================================================
//  Slots
// =============================================================================
void SavingsGoalWindow::onAddGoalClicked()
{
    AddGoalDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    SavingsGoal newGoal;
    newGoal.set_user_id(m_user.get_id());
    newGoal.set_description(dialog.getGoalName());
    newGoal.set_target_amount(dialog.getTargetAmt());
    newGoal.set_saved_amount(0.0);
    newGoal.set_deadline(QDateTime(dialog.getDeadline(), QTime(0, 0)));

    GoalService service;
    if (!service.createGoal(newGoal)) {
        QMessageBox::warning(this, "Error", "Failed to save your new goal.");
        return;
    }

    loadGoals();
    refreshGoalCards();
}

void SavingsGoalWindow::onDeleteGoalClicked(int goalId)
{
    auto reply = QMessageBox::question(this, "Delete Goal",
                                       "Are you sure you want to remove this goal?",
                                       QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) return;

    GoalService service;
    if (!service.deleteGoal(goalId)) {
        QMessageBox::warning(this, "Error", "Failed to delete the goal.");
        return;
    }

    loadGoals();
    refreshGoalCards();
}
void SavingsGoalWindow::onContributeClicked(int goalId)
{
    // m_balance was already computed in loadGoals(). It's the user's spendable
    // balance: total income − total expenses − total locked in other goals.
    ContributeDialog dialog(m_balance, this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    GoalService service;
    if (!service.contributeToGoal(goalId, dialog.getAmount())) {
        QMessageBox::warning(this, "Error", "Failed to add money to the goal.");
        return;
    }

    loadGoals();
    refreshGoalCards();
}
void SavingsGoalWindow::onBackClicked()
{
    DashboardWindow* dashboard = new DashboardWindow(m_user, nullptr);
    dashboard->show();
    this->close();
}