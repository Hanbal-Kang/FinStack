#include "ui/RecurringTransactionWindow.h"
#include "ui/DashboardWindow.h"
#include "services/RecurringTransactionService.h"
#include "utils/Validator.h"

#include <QScrollArea>
#include <QMessageBox>
#include <QGridLayout>

// =================================
//      AddRecurringDialog
// ==================================
AddRecurringDialog::AddRecurringDialog(QWidget* parent) : QDialog(parent)
{
    setWindowTitle("New Recurring Transaction");
    setModal(true);
    setMinimumWidth(440);
    setupUI();
}

void AddRecurringDialog::setupUI()
{
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(28, 24, 28, 24);
    root->setSpacing(14);

    QLabel* title = new QLabel("New Recurring Transaction");
    title->setObjectName("cardTitle");
    root->addWidget(title);

    QLabel* sub = new QLabel("Automate a regular income or expense.");
    sub->setObjectName("pageSubtitle");
    root->addWidget(sub);

    //Description / Title
    QLabel* dLbl = new QLabel("Title");
    dLbl->setObjectName("inputLabel");
    m_descInput = new QLineEdit();
    m_descInput->setPlaceholderText("e.g. Monthly Stipend, Apartment Rent");
    m_descInput->setFixedHeight(38);
    m_descInput->setObjectName("inputField");
    root->addWidget(dLbl);
    root->addWidget(m_descInput);

    //Type + Amount on same row
    QHBoxLayout* typeAmtRow = new QHBoxLayout();
    typeAmtRow->setSpacing(12);

    QVBoxLayout* typeCol = new QVBoxLayout();
    QLabel* tLbl = new QLabel("Type");
    tLbl->setObjectName("inputLabel");
    m_typeCombo = new QComboBox();
    m_typeCombo->addItems({"Expense", "Income"});
    m_typeCombo->setFixedHeight(38);
    typeCol->addWidget(tLbl);
    typeCol->addWidget(m_typeCombo);

    QVBoxLayout* amtCol = new QVBoxLayout();
    QLabel* aLbl = new QLabel("Amount (Rs)");
    aLbl->setObjectName("inputLabel");
    m_amountInput = new QLineEdit();
    m_amountInput->setPlaceholderText("0.00");
    m_amountInput->setFixedHeight(38);
    m_amountInput->setObjectName("inputField");
    amtCol->addWidget(aLbl);
    amtCol->addWidget(m_amountInput);

    typeAmtRow->addLayout(typeCol, 1);
    typeAmtRow->addLayout(amtCol, 1);
    root->addLayout(typeAmtRow);

    //Category
    QLabel* cLbl = new QLabel("Category");
    cLbl->setObjectName("inputLabel");
    m_categoryCombo = new QComboBox();
    m_categoryCombo->addItems({"Food", "Transport", "Rent", "Tuition", "Entertainment", "Other"});
    m_categoryCombo->setFixedHeight(38);
    root->addWidget(cLbl);
    root->addWidget(m_categoryCombo);

    //Interval + Start date on the same row
    QHBoxLayout* iDateRow = new QHBoxLayout();
    iDateRow->setSpacing(12);

    QVBoxLayout* iCol = new QVBoxLayout();
    QLabel* iLbl = new QLabel("Frequency");
    iLbl->setObjectName("inputLabel");
    m_intervalCombo = new QComboBox();
    m_intervalCombo->addItems({"Daily", "Weekly", "Monthly"});
    m_intervalCombo->setCurrentIndex(2);   // default: Monthly
    m_intervalCombo->setFixedHeight(38);
    iCol->addWidget(iLbl);
    iCol->addWidget(m_intervalCombo);

    QVBoxLayout* dateCol = new QVBoxLayout();
    QLabel* dateLbl = new QLabel("Start Date");
    dateLbl->setObjectName("inputLabel");
    m_startDateEdit = new QDateEdit();
    m_startDateEdit->setDate(QDate::currentDate());
    m_startDateEdit->setCalendarPopup(true);
    m_startDateEdit->setFixedHeight(38);
    dateCol->addWidget(dateLbl);
    dateCol->addWidget(m_startDateEdit);

    iDateRow->addLayout(iCol, 1);
    iDateRow->addLayout(dateCol, 1);
    root->addLayout(iDateRow);

    // Error
    m_errorLabel = new QLabel();
    m_errorLabel->setStyleSheet("color: #f85149;");
    m_errorLabel->hide();
    root->addWidget(m_errorLabel);

    // Buttons
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->setSpacing(12);

    QPushButton* cancel = new QPushButton("Cancel");
    cancel->setObjectName("primaryBtn");
    cancel->setFixedHeight(40);
    cancel->setCursor(Qt::PointingHandCursor);
    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);

    QPushButton* ok = new QPushButton("Create");
    ok->setObjectName("primaryBtn");
    ok->setFixedHeight(40);
    ok->setCursor(Qt::PointingHandCursor);
    ok->setDefault(true);
    connect(ok, &QPushButton::clicked, this, &AddRecurringDialog::onConfirmClicked);

    btnRow->addStretch();
    btnRow->addWidget(cancel);
    btnRow->addWidget(ok);
    root->addLayout(btnRow);
}

void AddRecurringDialog::onConfirmClicked()
{
    if (m_descInput->text().trimmed().isEmpty()) {
        m_errorLabel->setText("Please enter a title");
        m_errorLabel->show();
        return;
    }
    if (!Validator::isValidAmount(m_amountInput->text())) {
        m_errorLabel->setText("Please enter a valid amount greater than 0");
        m_errorLabel->show();
        return;
    }
    accept();
}

QString AddRecurringDialog::getDescription() const { return m_descInput->text().trimmed(); }
QString AddRecurringDialog::getType()        const { return m_typeCombo->currentText() == "Income" ? "income" : "expense"; }
double  AddRecurringDialog::getAmount()      const { return m_amountInput->text().toDouble(); }
QString AddRecurringDialog::getCategory()    const { return m_categoryCombo->currentText(); }
QString AddRecurringDialog::getInterval()    const { return m_intervalCombo->currentText().toLower(); }
QDate   AddRecurringDialog::getStartDate()   const { return m_startDateEdit->date(); }


// =============================================================================
//  RecurringTransactionWindow
// =============================================================================
RecurringTransactionWindow::RecurringTransactionWindow(const User& user, QWidget* parent)
    : BaseWindow(parent), m_user(user)
{
    setWindowTitle("FinStack — Recurring Transactions");
    setupWindowGeometry(1100, 720);
    initialize();
}

void RecurringTransactionWindow::setupUI()
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
    buildSummaryRow(pageLayout);
    buildList(pageLayout);

    pageLayout->addStretch();
    scroll->setWidget(page);
    root->addWidget(scroll, 1);

    loadRules();
    refreshAll();
}

void RecurringTransactionWindow::applyStyles()
{
    loadGlobalStylesheet();
}

// =============================================================================
//  Headerr
// =============================================================================
void RecurringTransactionWindow::buildHeader(QVBoxLayout* root)
{
    //Top row: back button (left) | + Add Recurring button (right)
    QHBoxLayout* topRow = new QHBoxLayout();
    m_backBtn = new QPushButton("←", this);
    m_backBtn->setObjectName("navIconBtn");
    m_backBtn->setFixedSize(36, 36);
    m_backBtn->setCursor(Qt::PointingHandCursor);
    connect(m_backBtn, &QPushButton::clicked, this, &RecurringTransactionWindow::onBackClicked);

    m_addBtn = new QPushButton("+ Add Recurring", this);
    m_addBtn->setObjectName("primaryBtn");
    m_addBtn->setFixedHeight(38);
    m_addBtn->setCursor(Qt::PointingHandCursor);
    connect(m_addBtn, &QPushButton::clicked, this, &RecurringTransactionWindow::onAddClicked);

    topRow->addWidget(m_backBtn);
    topRow->addStretch();
    topRow->addWidget(m_addBtn);
    root->addLayout(topRow);

    // Title row: icon + title + subtitle
    QHBoxLayout* titleRow = new QHBoxLayout();
    titleRow->setSpacing(14);

    QLabel* iconBox = new QLabel("🔄");
    iconBox->setObjectName("budgetTitleIcon");
    iconBox->setFixedSize(52, 52);
    iconBox->setAlignment(Qt::AlignCenter);

    QVBoxLayout* titleCol = new QVBoxLayout();
    titleCol->setSpacing(4);

    QLabel* title = new QLabel("Recurring Transactions");
    title->setObjectName("pageTitle");

    m_subtitleLabel = new QLabel("Automate regular income and expenses");
    m_subtitleLabel->setObjectName("pageSubtitle");

    titleCol->addWidget(title);
    titleCol->addWidget(m_subtitleLabel);

    titleRow->addWidget(iconBox);
    titleRow->addLayout(titleCol);
    titleRow->addStretch();
    root->addLayout(titleRow);
}

// =============================================================================
//  Summary row — 3 cards: Income / Expenses / Active count
// =============================================================================
void RecurringTransactionWindow::buildSummaryRow(QVBoxLayout* root)
{
    QHBoxLayout* row = new QHBoxLayout();
    row->setSpacing(16);

    auto makeCard = [](const QString& dotColor, const QString& title,
                       QLabel*& valueOut, const QString& sub) {
        QFrame* card = new QFrame();
        card->setObjectName("budgetSummaryCard");

        QVBoxLayout* vl = new QVBoxLayout(card);
        vl->setContentsMargins(24, 20, 24, 20);
        vl->setSpacing(8);

        QHBoxLayout* hdr = new QHBoxLayout();
        hdr->setSpacing(8);
        QLabel* dot = new QLabel("●");
        dot->setStyleSheet("color: " + dotColor + "; font-size: 14px;");
        QLabel* lbl = new QLabel(title);
        lbl->setObjectName("infoCardLabel");
        hdr->addWidget(dot);
        hdr->addWidget(lbl);
        hdr->addStretch();

        valueOut = new QLabel("Rs 0");
        valueOut->setStyleSheet("color: #ffffff; font-size: 28px; font-weight: 700;");

        QLabel* subLbl = new QLabel(sub);
        subLbl->setObjectName("budgetCatSub");

        vl->addLayout(hdr);
        vl->addWidget(valueOut);
        vl->addWidget(subLbl);
        return card;
    };

    row->addWidget(makeCard("#22c55e", "Monthly Income",   m_incomeLabel,  "Automated recurring"));
    row->addWidget(makeCard("#f85149", "Monthly Expenses", m_expenseLabel, "Automated recurring"));
    row->addWidget(makeCard("#a78bfa", "Active Rules",     m_activeLabel,  "Currently running"));

    root->addLayout(row);
}

// =============================================================================
//  List card
// =============================================================================
void RecurringTransactionWindow::buildList(QVBoxLayout* root)
{
    QFrame* card = new QFrame();
    card->setObjectName("budgetSummaryCard");

    QVBoxLayout* outer = new QVBoxLayout(card);
    outer->setContentsMargins(20, 18, 20, 18);
    outer->setSpacing(10);

    QLabel* title = new QLabel("All Recurring Transactions");
    title->setObjectName("cardTitle");
    outer->addWidget(title);

    m_listLayout = new QVBoxLayout();
    m_listLayout->setSpacing(8);
    outer->addLayout(m_listLayout);

    m_emptyLabel = new QLabel("No recurring transactions yet. Click \"+ Add Recurring\" to set up your first one.");
    m_emptyLabel->setObjectName("budgetCatSub");
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    outer->addWidget(m_emptyLabel);

    root->addWidget(card);
}

//=============================================================================
//  buildRuleRow — one row per rule
// =============================================================================
QFrame* RecurringTransactionWindow::buildRuleRow(const RecurringTransaction& r)
{
    QFrame* row = new QFrame();
    row->setObjectName("profileInfoRow");

    QHBoxLayout* h = new QHBoxLayout(row);
    h->setContentsMargins(16, 12, 16, 12);
    h->setSpacing(14);

    //Emoji
    QLabel* emoji = new QLabel(iconForCategory(r.get_category()));
    emoji->setObjectName("categoryEmoji");
    emoji->setFixedSize(36, 36);
    emoji->setAlignment(Qt::AlignCenter);

    //Middle: title + (badges line) + (category · next due)
    QVBoxLayout* mid = new QVBoxLayout();
    mid->setSpacing(4);

    // Title row: name + type badge + interval badge
    QHBoxLayout* titleRow = new QHBoxLayout();
    titleRow->setSpacing(8);

    QLabel* name = new QLabel(r.get_description());
    name->setObjectName("budgetCatName");
    titleRow->addWidget(name);

    bool isIncome = (r.get_type() == RecurringTransaction::Income);
    QLabel* typeBadge = new QLabel(isIncome ? "INCOME" : "EXPENSE");
    QString typeBgColor = isIncome ? "rgba(34,197,94,0.15)" : "rgba(248,81,73,0.15)";
    QString typeFgColor = isIncome ? "#22c55e" : "#f85149";
    typeBadge->setStyleSheet(QString(
                                 "background:%1; color:%2; border-radius:4px; padding:2px 8px; "
                                 "font-weight:700; font-size:10px;").arg(typeBgColor, typeFgColor));
    titleRow->addWidget(typeBadge);

    QLabel* intervalBadge = new QLabel(r.get_interval().toUpper());
    intervalBadge->setStyleSheet(
        "background: rgba(167,139,250,0.15); color: #a78bfa; "
        "border-radius:4px; padding:2px 8px; font-weight:700; font-size:10px;");
    titleRow->addWidget(intervalBadge);

    titleRow->addStretch();
    mid->addLayout(titleRow);

    // Sub line: "Category: X  •  Next: <date> (Nd)"
    QString nextText = "—";
    if (r.get_next_due().isValid()) {
        QDate nextDate = r.get_next_due().date();
        qint64 daysAway = QDate::currentDate().daysTo(nextDate);
        nextText = nextDate.toString("MMM d") + QString(" (%1d)").arg(daysAway);
    }
    QLabel* subLine = new QLabel(QString("Category: %1   •   Next: %2")
                                     .arg(r.get_category(), nextText));
    subLine->setObjectName("budgetCatSub");
    mid->addWidget(subLine);

    // Right: amount + per-interval label + pause/resume + delete
    QVBoxLayout* amtCol = new QVBoxLayout();
    amtCol->setSpacing(2);
    amtCol->setAlignment(Qt::AlignRight);

    QString sign = isIncome ? "+" : "−";
    QLabel* amt = new QLabel(sign + "Rs " + QString::number(r.get_amount(), 'f', 0));
    QString amtColor = isIncome ? "#22c55e" : "#f85149";
    amt->setStyleSheet(QString("color: %1; font-weight: 700; font-size: 16px;").arg(amtColor));
    amt->setAlignment(Qt::AlignRight);

    QLabel* perLabel = new QLabel("per " + r.get_interval().mid(0, r.get_interval().length() - 2));
    // "monthly" → "per month", "weekly" → "per week", "daily" → "per dai" (oops)
    QString iv = r.get_interval();
    if      (iv == "monthly") perLabel->setText("per month");
    else if (iv == "weekly")  perLabel->setText("per week");
    else                       perLabel->setText("per day");
    perLabel->setObjectName("budgetCatSub");
    perLabel->setAlignment(Qt::AlignRight);

    amtCol->addWidget(amt);
    amtCol->addWidget(perLabel);

    int rid = r.get_reccur_transac_id();
    bool active = r.get_is_active();

    QPushButton* toggleBtn = new QPushButton(active ? "⏸" : "▶");
    toggleBtn->setObjectName(active ? "primaryBtn" : "primaryBtn");
    toggleBtn->setFixedSize(36, 36);
    toggleBtn->setCursor(Qt::PointingHandCursor);
    toggleBtn->setToolTip(active ? "Pause" : "Resume");
    connect(toggleBtn, &QPushButton::clicked, this,
            [this, rid, active]() { onToggleActiveClicked(rid, active); });

    QPushButton* delBtn = new QPushButton("🗑");
    delBtn->setObjectName("dangerBtn");
    delBtn->setFixedSize(36, 36);
    delBtn->setCursor(Qt::PointingHandCursor);
    connect(delBtn, &QPushButton::clicked, this,
            [this, rid]() { onDeleteClicked(rid); });

    h->addWidget(emoji);
    h->addLayout(mid, 1);
    h->addLayout(amtCol);
    h->addWidget(toggleBtn);
    h->addWidget(delBtn);

    // Dim the row if paused
    if (!active) row->setStyleSheet("opacity: 0.6;");

    return row;
}

// =============================================================================
//  Data
//=============================================================================
void RecurringTransactionWindow::loadRules()
{
    RecurringTransactionService svc;
    m_rules = svc.getAllByUser(m_user.get_id());
}

void RecurringTransactionWindow::refreshAll()
{
    // Compute summary numbers (normalized to monthly equivalents)
    double income = 0.0, expense = 0.0;
    int activeCount = 0;
    int totalCount  = m_rules.size();

    for (const auto& r : m_rules) {
        if (!r.get_is_active()) continue;
        activeCount++;

        // Normalize to monthly: weekly × 4, daily × 30
        double monthlyAmount = r.get_amount();
        if      (r.get_interval() == "weekly") monthlyAmount *= 4.0;
        else if (r.get_interval() == "daily")  monthlyAmount *= 30.0;

        if (r.get_type() == RecurringTransaction::Income) income  += monthlyAmount;
        else                                                expense += monthlyAmount;
    }

    m_incomeLabel ->setText("Rs " + QString::number(income, 'f', 0));
    m_expenseLabel->setText("Rs " + QString::number(expense, 'f', 0));
    m_activeLabel ->setText(QString::number(activeCount));

    m_subtitleLabel->setText(QString("Automate regular income and expenses  •  %1 active")
                                 .arg(activeCount));

    // Clear list and rebuild
    QLayoutItem* item;
    while ((item = m_listLayout->takeAt(0)) != nullptr) {
        if (QWidget* w = item->widget()) w->deleteLater();
        delete item;
    }

    if (m_rules.empty()) {
        m_emptyLabel->show();
    } else {
        m_emptyLabel->hide();
        for (const auto& r : m_rules)
            m_listLayout->addWidget(buildRuleRow(r));
    }
}

QString RecurringTransactionWindow::iconForCategory(const QString& cat) const
{
    static const QMap<QString, QString> map = {
        {"Food",          "🛒"},
        {"Transport",     "🚌"},
        {"Rent",          "🏠"},
        {"Tuition",       "🎓"},
        {"Entertainment", "📺"},
        {"Other",         "💰"}
    };
    return map.value(cat, "💰");
}

//=============================================================================
//  Slots
//=============================================================================
void RecurringTransactionWindow::onAddClicked()
{
    AddRecurringDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) return;

    RecurringTransaction r;
    r.set_user_id(m_user.get_id());
    r.set_type(dialog.getType() == "income"
                   ? RecurringTransaction::Income
                   : RecurringTransaction::Expense);
    r.set_amount(dialog.getAmount());
    r.set_category(dialog.getCategory());
    r.set_description(dialog.getDescription());
    r.set_interval(dialog.getInterval());
    r.set_start_date(QDateTime(dialog.getStartDate(), QTime(0, 0)));
    r.set_next_due  (QDateTime(dialog.getStartDate(), QTime(0, 0)));
    r.set_is_active(true);

    RecurringTransactionService svc;
    if (!svc.createRecurring(r)) {
        QMessageBox::warning(this, "Error", "Failed to create the recurring transaction.");
        return;
    }

    loadRules();
    refreshAll();
}

void RecurringTransactionWindow::onToggleActiveClicked(int recurringId, bool currentlyActive)
{
    RecurringTransactionService svc;
    if (!svc.setActive(recurringId, !currentlyActive)) {
        QMessageBox::warning(this, "Error", "Failed to update the rule.");
        return;
    }
    loadRules();
    refreshAll();
}

void RecurringTransactionWindow::onDeleteClicked(int recurringId)
{
    auto reply = QMessageBox::question(this, "Delete Rule",
                                       "Stop this recurring transaction permanently?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    RecurringTransactionService svc;
    if (!svc.deleteRecurring(recurringId)) {
        QMessageBox::warning(this, "Error", "Failed to delete the rule.");
        return;
    }
    loadRules();
    refreshAll();
}

void RecurringTransactionWindow::onBackClicked()
{
    DashboardWindow* dashboard = new DashboardWindow(m_user, nullptr);
    dashboard->show();
    this->close();
}