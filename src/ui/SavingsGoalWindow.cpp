#include "ui/SavingsGoalWindow.h"
#include "services/GoalService.h"
#include "services/TransactionService.h"
#include <QGridLayout>
#include <QMessageBox>
#include <QProgressBar>
#include <QScrollArea>

// =============================================================================
//  AddGoalDialog
// =============================================================================

AddGoalDialog::AddGoalDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("New Savings Goal");
    setFixedWidth(400);
    setupUI();
}

void AddGoalDialog::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(12);

    layout->addWidget(new QLabel("Goal description:"));
    m_nameInput = new QLineEdit();
    m_nameInput->setPlaceholderText("e.g. Buy a Laptop");
    layout->addWidget(m_nameInput);

    layout->addWidget(new QLabel("Target amount (Rs):"));
    m_amountInput = new QLineEdit();
    m_amountInput->setPlaceholderText("e.g. 150000");
    layout->addWidget(m_amountInput);

    layout->addWidget(new QLabel("Deadline:"));
    m_dateEdit = new QDateEdit(QDate::currentDate().addMonths(3));
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setMinimumDate(QDate::currentDate().addDays(1));
    layout->addWidget(m_dateEdit);

    m_errorLabel = new QLabel();
    m_errorLabel->setObjectName("errorLabel");
    m_errorLabel->hide();
    layout->addWidget(m_errorLabel);

    QHBoxLayout* btnRow = new QHBoxLayout();
    QPushButton* cancelBtn  = new QPushButton("Cancel");
    QPushButton* confirmBtn = new QPushButton("Add Goal");
    confirmBtn->setObjectName("primaryButton");
    cancelBtn ->setCursor(Qt::PointingHandCursor);
    confirmBtn->setCursor(Qt::PointingHandCursor);
    btnRow->addWidget(cancelBtn);
    btnRow->addWidget(confirmBtn);
    layout->addLayout(btnRow);

    connect(cancelBtn,  &QPushButton::clicked, this, &QDialog::reject);
    connect(confirmBtn, &QPushButton::clicked, this, &AddGoalDialog::onConfirmClicked);
}

void AddGoalDialog::onConfirmClicked()
{
    if (m_nameInput->text().trimmed().isEmpty()) {
        m_errorLabel->setText("Please enter a description.");
        m_errorLabel->show();
        return;
    }
    bool ok;
    double amt = m_amountInput->text().toDouble(&ok);
    if (!ok || amt <= 0) {
        m_errorLabel->setText("Please enter a valid amount greater than 0.");
        m_errorLabel->show();
        return;
    }
    accept();
}

QString AddGoalDialog::getGoalName()  const { return m_nameInput->text().trimmed(); }
double  AddGoalDialog::getTargetAmt() const { return m_amountInput->text().toDouble(); }
QDate   AddGoalDialog::getDeadline()  const { return m_dateEdit->date(); }

// =============================================================================
//  SavingsGoalWindow
// =============================================================================

SavingsGoalWindow::SavingsGoalWindow(const User& user, QWidget* parent)
    : BaseWindow(parent), m_user(user), m_balance(0.0)
{
    setupWindowGeometry(1100, 720);
    initialize();
}

void SavingsGoalWindow::setupUI()
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
    buildBalanceBanner(pageLayout);
    buildGoalsGrid(pageLayout);
    pageLayout->addStretch();

    outerScroll->setWidget(page);

    QVBoxLayout* root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->addWidget(outerScroll);

    loadGoals();
    refreshGoalCards();
}

void SavingsGoalWindow::applyStyles()
{
    loadGlobalStylesheet();
}

// ─────────────────────────────────────────────────────────────────────────────
//  Layout builders
// ─────────────────────────────────────────────────────────────────────────────

void SavingsGoalWindow::buildHeader(QVBoxLayout* root)
{
    // top row: back ← (left)  |  + Add New Goal (right)
    QHBoxLayout* topRow = new QHBoxLayout();

    m_backBtn = new QPushButton("←");
    m_backBtn->setObjectName("navIconBtn");
    m_backBtn->setFixedSize(36, 36);
    m_backBtn->setCursor(Qt::PointingHandCursor);
    connect(m_backBtn, &QPushButton::clicked, this, &SavingsGoalWindow::onBackClicked);

    m_addBtn = new QPushButton("+ Add New Goal");
    m_addBtn->setObjectName("primaryButton");
    m_addBtn->setCursor(Qt::PointingHandCursor);
    connect(m_addBtn, &QPushButton::clicked, this, &SavingsGoalWindow::onAddGoalClicked);

    topRow->addWidget(m_backBtn);
    topRow->addStretch();
    topRow->addWidget(m_addBtn);
    root->addLayout(topRow);

    // title row: green icon box | "Savings Goals" title + subtitle
    QHBoxLayout* titleRow = new QHBoxLayout();
    titleRow->setSpacing(14);

    QLabel* iconBox = new QLabel("✓");
    iconBox->setObjectName("goalsTitleIcon");   // styled green in QSS
    iconBox->setFixedSize(52, 52);
    iconBox->setAlignment(Qt::AlignCenter);

    QVBoxLayout* titleCol = new QVBoxLayout();
    titleCol->setSpacing(4);

    QLabel* title    = new QLabel("Savings Goals");
    QLabel* subtitle = new QLabel("Track your savings targets and progress");
    title   ->setObjectName("pageTitle");
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
    // Wide card: "Available for Savings" / big green number / hint / Rs icon box
    QFrame* banner = new QFrame();
    banner->setObjectName("balanceBanner");

    QHBoxLayout* bl = new QHBoxLayout(banner);
    bl->setContentsMargins(28, 24, 28, 24);

    QVBoxLayout* textCol = new QVBoxLayout();
    textCol->setSpacing(6);

    QLabel* bannerTitle = new QLabel("Available for Savings");
    bannerTitle->setObjectName("bannerTitle");

    m_balanceLabel = new QLabel("Rs 0");
    m_balanceLabel->setObjectName("balanceBig");    // large green text in QSS

    QLabel* hint = new QLabel("Current balance after expenses");
    hint->setObjectName("bannerHint");

    textCol->addWidget(bannerTitle);
    textCol->addWidget(m_balanceLabel);
    textCol->addWidget(hint);

    // Rs icon box (top-right corner of banner, as seen in screenshot)
    QLabel* rsIcon = new QLabel("Rs");
    rsIcon->setObjectName("balanceIconBox");
    rsIcon->setFixedSize(56, 56);
    rsIcon->setAlignment(Qt::AlignCenter);

    bl->addLayout(textCol, 1);
    bl->addWidget(rsIcon, 0, Qt::AlignTop);
    root->addWidget(banner);
}

void SavingsGoalWindow::buildGoalsGrid(QVBoxLayout* root)
{
    m_goalsGrid = new QFrame();
    m_goalsGrid->setObjectName("goalsGridFrame");

    QGridLayout* grid = new QGridLayout(m_goalsGrid);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(16);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);

    // empty-state at row 0, spans both columns
    m_emptyLabel = new QLabel("No goals yet. Click '+ Add New Goal' to start!");
    m_emptyLabel->setObjectName("emptyState");
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->hide();
    grid->addWidget(m_emptyLabel, 0, 0, 1, 2, Qt::AlignCenter);

    root->addWidget(m_goalsGrid);
}

// ─────────────────────────────────────────────────────────────────────────────
//  Data helpers
// ─────────────────────────────────────────────────────────────────────────────

void SavingsGoalWindow::loadGoals()
{
    m_balance = computeBalance();
    m_balanceLabel->setText("Rs " + QString::number(m_balance, 'f', 0));

    GoalService service;
    m_goals = service.getGoalsForUser(m_user.get_id());
}

double SavingsGoalWindow::computeBalance()
{
    TransactionService service;
    std::vector<Transaction> all = service.getAllByUser(m_user.get_id());

    double balance = 0.0;
    for (const auto& tx : all) {
        if (tx.get_type() == Transaction::Income)
            balance += tx.get_amount();
        else
            balance -= tx.get_amount();
    }
    return balance;
}

// Status is driven by per-goal saved_amount, not the overall account balance
QString SavingsGoalWindow::getGoalStatus(const SavingsGoal& goal) const
{
    double saved    = goal.get_saved_amount();
    double target   = goal.get_target_amount();
    QDate  deadline = goal.get_deadline().date();
    int    daysLeft = QDate::currentDate().daysTo(deadline);
    double pct      = (target > 0) ? (saved / target * 100.0) : 0.0;

    if (saved >= target)              return "Achieved";
    if (daysLeft <= 0)                return "At Risk";
    if (daysLeft <= 30 && pct < 50.0) return "At Risk";
    return "On Track";
}

// ─────────────────────────────────────────────────────────────────────────────
//  refreshGoalCards
// ─────────────────────────────────────────────────────────────────────────────

void SavingsGoalWindow::refreshGoalCards()
{
    QGridLayout* grid = qobject_cast<QGridLayout*>(m_goalsGrid->layout());
    if (!grid) return;

    // keep index 0 (empty-state label), remove everything else
    while (grid->count() > 1) {
        QLayoutItem* item = grid->takeAt(1);
        if (item->widget()) delete item->widget();
        delete item;
    }

    if (m_goals.empty()) {
        m_emptyLabel->show();
        return;
    }
    m_emptyLabel->hide();

    // start at grid row 1; row 0 holds the hidden empty label
    int col = 0, row = 1;
    for (const auto& goal : m_goals) {
        grid->addWidget(buildGoalCard(goal), row, col);
        col++;
        if (col == 2) { col = 0; row++; }
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  buildGoalCard — matches screenshot
// ─────────────────────────────────────────────────────────────────────────────

QFrame* SavingsGoalWindow::buildGoalCard(const SavingsGoal& goal)
{
    QFrame* card = new QFrame();
    card->setObjectName("goalCard");

    QVBoxLayout* cl = new QVBoxLayout(card);
    cl->setContentsMargins(20, 18, 20, 18);
    cl->setSpacing(10);

    QString status = getGoalStatus(goal);

    // ── top row: emoji icon + name/target col + status badge + delete ─────────
    QHBoxLayout* topRow = new QHBoxLayout();
    topRow->setSpacing(10);

    QLabel* iconLabel = new QLabel(goalEmoji(goal.get_description()));
    iconLabel->setObjectName("goalCardIcon");
    iconLabel->setFixedSize(36, 36);
    iconLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout* nameCol = new QVBoxLayout();
    nameCol->setSpacing(2);
    QLabel* nameLabel   = new QLabel(goal.get_description());
    nameLabel->setObjectName("goalName");
    QLabel* targetSmall = new QLabel(
        "Target: Rs " + QString::number(goal.get_target_amount(), 'f', 0));
    targetSmall->setObjectName("goalTargetSmall");
    nameCol->addWidget(nameLabel);
    nameCol->addWidget(targetSmall);

    // status badge colours match screenshot
    QLabel* badge = new QLabel(status);
    badge->setObjectName("statusBadge");
    if (status == "Achieved")
        badge->setStyleSheet(
            "color:#14532D; background:#DCFCE7;"
            " border-radius:6px; padding:3px 10px; font-size:12px;");
    else if (status == "At Risk")
        badge->setStyleSheet(
            "color:#991B1B; background:#FEE2E2;"
            " border-radius:6px; padding:3px 10px; font-size:12px;");
    else   // On Track — green outline style as in screenshot
        badge->setStyleSheet(
            "color:#14532D; background:transparent;"
            " border:1px solid #22C55E;"
            " border-radius:6px; padding:3px 10px; font-size:12px;");

    QPushButton* deleteBtn = new QPushButton("🗑");
    deleteBtn->setFixedSize(28, 28);
    deleteBtn->setObjectName("dangerIconBtn");
    deleteBtn->setCursor(Qt::PointingHandCursor);

    int goalId = goal.get_savings_goal_id();
    connect(deleteBtn, &QPushButton::clicked, this, [this, goalId]() {
        onDeleteGoalClicked(goalId);
    });

    topRow->addWidget(iconLabel);
    topRow->addLayout(nameCol, 1);
    topRow->addWidget(badge);
    topRow->addWidget(deleteBtn);
    cl->addLayout(topRow);

    // ── progress section ──────────────────────────────────────────────────────
    double saved     = goal.get_saved_amount();
    double target    = goal.get_target_amount();
    double pct       = (target > 0) ? qMin(saved / target * 100.0, 100.0) : 0.0;
    double remaining = target - saved;

    // "Current Progress"  (left)   "Still Needed" / "Surplus" (right) labels
    QHBoxLayout* headerRow = new QHBoxLayout();
    QLabel* progTitle  = new QLabel("Current Progress");
    QLabel* rightTitle = new QLabel(remaining <= 0 ? "Surplus" : "Still Needed");
    progTitle ->setObjectName("progressLabel");
    rightTitle->setObjectName("progressLabel");
    rightTitle->setAlignment(Qt::AlignRight);
    headerRow->addWidget(progTitle);
    headerRow->addWidget(rightTitle);
    cl->addLayout(headerRow);

    // amounts row: green saved (left) | right amount
    QHBoxLayout* amtRow = new QHBoxLayout();
    QLabel* savedLbl   = new QLabel("Rs " + QString::number(saved, 'f', 0));
    QLabel* remainLbl  = new QLabel(remaining <= 0
                                       ? "Rs " + QString::number(-remaining, 'f', 0)
                                       : "Rs " + QString::number(remaining,  'f', 0));
    savedLbl ->setObjectName("progressAmount");      // green in QSS
    remainLbl->setObjectName("progressAmountRight");
    remainLbl->setAlignment(Qt::AlignRight);
    amtRow->addWidget(savedLbl);
    amtRow->addWidget(remainLbl);
    cl->addLayout(amtRow);

    // QProgressBar — scales correctly with card width (replaces hardcoded 300px)
    QString barColor = (status == "Achieved") ? "#22C55E"
                       : (status == "At Risk")  ? "#EF4444" : "#3B82F6";
    QProgressBar* bar = new QProgressBar();
    bar->setRange(0, 100);
    bar->setValue(static_cast<int>(pct));
    bar->setTextVisible(false);
    bar->setFixedHeight(8);
    bar->setStyleSheet(QString(
                           "QProgressBar{background:rgba(255,255,255,0.08);border-radius:4px;border:none;}"
                           "QProgressBar::chunk{background:%1;border-radius:4px;}").arg(barColor));
    cl->addWidget(bar);

    // percentage label (right-aligned, muted)
    QLabel* pctLabel = new QLabel(QString::number(pct, 'f', 1) + "% complete");
    pctLabel->setObjectName("pctLabel");
    pctLabel->setAlignment(Qt::AlignRight);
    cl->addWidget(pctLabel);

    // ── deadline row ──────────────────────────────────────────────────────────
    QDate  deadline = goal.get_deadline().date();
    int    daysLeft = QDate::currentDate().daysTo(deadline);
    QString daysText = (daysLeft > 0)
                           ? QString::number(daysLeft) + " days left"
                           : "Deadline passed";

    QHBoxLayout* deadlineRow = new QHBoxLayout();
    QLabel* dateLbl = new QLabel("📅  " + deadline.toString("MMM d, yyyy"));
    QLabel* daysLbl = new QLabel(daysText);
    dateLbl->setObjectName("deadlineLabel");
    daysLbl->setObjectName("deadlineLabel");
    daysLbl->setAlignment(Qt::AlignRight);
    deadlineRow->addWidget(dateLbl);
    deadlineRow->addWidget(daysLbl);
    cl->addLayout(deadlineRow);

    return card;
}

QString SavingsGoalWindow::goalEmoji(const QString& desc) const
{
    QString d = desc.toLower();
    if (d.contains("emergency") || d.contains("fund"))  return "🛡";
    if (d.contains("laptop") || d.contains("computer")) return "💻";
    if (d.contains("vacation") || d.contains("travel") ||
        d.contains("trip"))                              return "✈";
    if (d.contains("car"))                               return "🚗";
    if (d.contains("house") || d.contains("home"))      return "🏠";
    if (d.contains("phone"))                             return "📱";
    return "🎯";
}

// ─────────────────────────────────────────────────────────────────────────────
//  Slots
// ─────────────────────────────────────────────────────────────────────────────

void SavingsGoalWindow::onAddGoalClicked()
{
    AddGoalDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    SavingsGoal newGoal;
    newGoal.set_description(dialog.getGoalName());
    newGoal.set_target_amount(dialog.getTargetAmt());
    newGoal.set_user_id(m_user.get_id());
    newGoal.set_deadline(QDateTime(dialog.getDeadline(), QTime(0, 0)));

    GoalService service;
    if (!service.createGoal(newGoal)) {
        QMessageBox::warning(this, "Error", "Could not save goal. Please try again.");
        return;
    }

    loadGoals();
    refreshGoalCards();
}

void SavingsGoalWindow::onDeleteGoalClicked(int goalId)
{
    auto reply = QMessageBox::question(
        this, "Delete Goal",
        "Are you sure you want to delete this goal?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    GoalService service;
    service.deleteGoal(goalId);

    loadGoals();
    refreshGoalCards();
}

void SavingsGoalWindow::onBackClicked()
{
    emit backToDashboard();
    close();
}