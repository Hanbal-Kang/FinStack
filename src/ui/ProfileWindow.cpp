#include "ui/ProfileWindow.h"
#include "ui/LoginWindow.h"
#include "ui/DashboardWindow.h"
#include "utils/Formatter.h"
#include <QScrollArea>
#include <QGridLayout>
#include <QMessageBox>

ProfileWindow::ProfileWindow(const User& user, QWidget* parent) : BaseWindow(parent), m_user(user)
{
    setWindowTitle("FinStack — Profile & Settings");
    setupWindowGeometry(1100, 700);
    initialize();
}

//Setting up the Main Layout Here
void ProfileWindow::setupUI()
{
    QWidget* central = new QWidget(this);
    setCentralWidget(central);


    QVBoxLayout* root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    //Scrollable page wrapper
    QScrollArea* scroll = new QScrollArea(central);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setObjectName("pageScroll");

    QWidget* page = new QWidget();
    page->setObjectName("pageBody");

    QVBoxLayout* pageLayout = new QVBoxLayout(page);
    pageLayout->setContentsMargins(40, 36, 40, 40);
    pageLayout->setSpacing(24);

    //Header: back button  + title row
    QHBoxLayout* topRow = new QHBoxLayout();
    QPushButton* backBtn = new QPushButton("←", this);
    backBtn->setObjectName("navIconBtn");
    backBtn->setFixedSize(36, 36);

    backBtn->setCursor(Qt::PointingHandCursor);
    connect(backBtn, &QPushButton::clicked, this, [this]() {
        DashboardWindow* dashboard = new DashboardWindow(m_user, nullptr);
        dashboard->show();
        this->close();
    });
    topRow->addWidget(backBtn);
    topRow->addStretch();

    pageLayout->addLayout(topRow);

    QHBoxLayout* titleRow = new QHBoxLayout();
    titleRow->setSpacing(14);

    QLabel* iconBox = new QLabel("👤");
    iconBox->setObjectName("budgetTitleIcon");
    iconBox->setFixedSize(52, 52);
    iconBox->setAlignment(Qt::AlignCenter);

    QVBoxLayout* titleCol = new QVBoxLayout();
    titleCol->setSpacing(4);

    QLabel* title = new QLabel("Profile & Settings");
    title->setObjectName("pageTitle");

    QLabel* subtitle = new QLabel("Manage your account settings and preferences");
    subtitle->setObjectName("pageSubtitle");

    titleCol->addWidget(title);
    titleCol->addWidget(subtitle);

    titleRow->addWidget(iconBox);
    titleRow->addLayout(titleCol);
    titleRow->addStretch();
    pageLayout->addLayout(titleRow);

    //Account Information Card Here
    QFrame* accountCard = new QFrame();
    accountCard->setObjectName("budgetSummaryCard");

    QVBoxLayout* accVl = new QVBoxLayout(accountCard);
    accVl->setContentsMargins(28, 24, 28, 24);
    accVl->setSpacing(20);

    QLabel* accTitle = new QLabel("Account Information");
    accTitle->setObjectName("cardTitle");
    accVl->addWidget(accTitle);

    // Avatar + big username row
    QHBoxLayout* avatarRow = new QHBoxLayout();
    avatarRow->setSpacing(20);

    // Build initials (e.g. "John" → "JO"). Used inside the avatar tile.
    QString initials;
    if (!m_user.get_username().isEmpty()) {
        initials = m_user.get_username().left(1).toUpper();
        if (m_user.get_username().length() > 1)
            initials += m_user.get_username().mid(1, 1).toUpper();
    } else {
        initials = "?";
    }

    QLabel* avatar = new QLabel(initials);
    avatar->setObjectName("profileAvatar");
    avatar->setFixedSize(72, 72);
    avatar->setAlignment(Qt::AlignCenter);

    QLabel* usernameLbl = new QLabel(m_user.get_username().isEmpty() ? "Guest" : m_user.get_username());
    usernameLbl->setObjectName("profileUsername");

    avatarRow->addWidget(avatar);
    avatarRow->addWidget(usernameLbl);
    avatarRow->addStretch();
    accVl->addLayout(avatarRow);

    //2-column grid of info cards (Member Since + Username)
    QGridLayout* infoGrid = new QGridLayout();
    infoGrid->setHorizontalSpacing(16);
    infoGrid->setVerticalSpacing(16);
    infoGrid->setColumnStretch(0, 1);
    infoGrid->setColumnStretch(1, 1);

    QFrame* infoSince = buildInfoCard("📅", "MEMBER SINCE",
                                      m_user.get_acc_creation_date().isValid()
                                          ? Formatter::formatDate(m_user.get_acc_creation_date())
                                          : "—");
    QFrame* infoUsername = buildInfoCard("👤", "USERNAME",
                                         m_user.get_username().isEmpty() ? "—" : m_user.get_username());

    infoGrid->addWidget(infoSince,    0, 0);
    infoGrid->addWidget(infoUsername, 0, 1);

    accVl->addLayout(infoGrid);
    pageLayout->addWidget(accountCard);

    //Security Card (flat layout: title/sub on left, button on right)
    QFrame* securityCard = new QFrame();
    securityCard->setObjectName("budgetSummaryCard");

    QHBoxLayout* secRow = new QHBoxLayout(securityCard);
    secRow->setContentsMargins(28, 22, 28, 22);
    secRow->setSpacing(16);

    QVBoxLayout* secTextCol = new QVBoxLayout();
    secTextCol->setSpacing(4);

    QLabel* secTitle = new QLabel("Security");
    secTitle->setObjectName("cardTitle");
    QLabel* secSub = new QLabel("Update your password to keep your account secure");
    secSub->setObjectName("pageSubtitle");

    secTextCol->addWidget(secTitle);
    secTextCol->addWidget(secSub);

    QPushButton* changePwBtn = new QPushButton("🔒  Change Password");
    changePwBtn->setObjectName("primaryBtn");
    changePwBtn->setCursor(Qt::PointingHandCursor);
    changePwBtn->setFixedHeight(40);
    connect(changePwBtn, &QPushButton::clicked, this, &ProfileWindow::onChangePasswordClicked);

    secRow->addLayout(secTextCol);
    secRow->addStretch();
    secRow->addWidget(changePwBtn);

    pageLayout->addWidget(securityCard);

    //Sign Out Card (same flat layout as Security)
    QFrame* logoutCard = new QFrame();
    logoutCard->setObjectName("budgetSummaryCard");

    QHBoxLayout* logRow = new QHBoxLayout(logoutCard);
    logRow->setContentsMargins(28, 22, 28, 22);
    logRow->setSpacing(16);

    QVBoxLayout* logTextCol = new QVBoxLayout();
    logTextCol->setSpacing(4);

    QLabel* logTitle = new QLabel("Sign Out");
    logTitle->setObjectName("cardTitle");
    QLabel* logSub = new QLabel("Log out of your FinStack account");
    logSub->setObjectName("pageSubtitle");

    logTextCol->addWidget(logTitle);
    logTextCol->addWidget(logSub);

    QPushButton* logoutBtn = new QPushButton("🚪  Logout");
    logoutBtn->setObjectName("dangerBtn");
    logoutBtn->setCursor(Qt::PointingHandCursor);
    logoutBtn->setFixedHeight(40);
    connect(logoutBtn, &QPushButton::clicked, this, &ProfileWindow::onLogoutClicked);

    logRow->addLayout(logTextCol);
    logRow->addStretch();
    logRow->addWidget(logoutBtn);

    pageLayout->addWidget(logoutCard);

    pageLayout->addStretch();
    scroll->setWidget(page);
    root->addWidget(scroll, 1);
}

void ProfileWindow::applyStyles()
{
    loadGlobalStylesheet();
}

// Helper for one info card in the 2-column grid: icon + uppercase label, then bold value
QFrame* ProfileWindow::buildInfoCard(const QString& iconSymbol, const QString& label, const QString& value)
{
    QFrame* card = new QFrame();
    card->setObjectName("profileInfoRow");

    QVBoxLayout* col = new QVBoxLayout(card);
    col->setContentsMargins(18, 14, 18, 14);
    col->setSpacing(8);

    QHBoxLayout* labelRow = new QHBoxLayout();
    labelRow->setSpacing(8);

    QLabel* icon = new QLabel(iconSymbol);
    icon->setObjectName("infoCardIcon");

    QLabel* labelLbl = new QLabel(label);
    labelLbl->setObjectName("infoCardLabel");

    labelRow->addWidget(icon);
    labelRow->addWidget(labelLbl);
    labelRow->addStretch();

    QLabel* valueLbl = new QLabel(value);
    valueLbl->setObjectName("infoCardValue");

    col->addLayout(labelRow);
    col->addWidget(valueLbl);

    return card;
}

void ProfileWindow::onChangePasswordClicked()
{
    QMessageBox::information(this, "Change Password",
                             "Password change flow is coming soon.");
}

void ProfileWindow::onLogoutClicked()
{
    LoginWindow* login = new LoginWindow(nullptr);
    login->show();
    this->close();
}