#include "ui/RecoveryCodeDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QApplication>
#include <QClipboard>
#include <QTimer>

RecoveryCodeDialog::RecoveryCodeDialog(const QString& code, QWidget* parent)
    : QDialog(parent), m_code(code)
{
    setWindowTitle("Save Your Recovery Code");
    setModal(true);
    setMinimumWidth(480);
    setupUI();
}

void RecoveryCodeDialog::setupUI()
{
    QVBoxLayout* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 24);
    root->setSpacing(16);

    // Title
    QLabel* title = new QLabel("🔐  Save Your Recovery Code");
    title->setObjectName("cardTitle");
    title->setStyleSheet("font-size: 20px; font-weight: 700; color: #ffffff;");
    root->addWidget(title);

    // Warning explanation
    QLabel* warning = new QLabel(
        "This is the ONLY way to reset your password if you forget it.\n"
        "Save it somewhere safe (password manager, notes, paper).\n"
        "We will not show this code again."
        );
    warning->setObjectName("pageSubtitle");
    warning->setWordWrap(true);
    warning->setStyleSheet("color: #f0883e; font-weight: 500;");
    root->addWidget(warning);

    // The code itself, in a styled box
    m_codeLabel = new QLabel(m_code);
    m_codeLabel->setAlignment(Qt::AlignCenter);
    m_codeLabel->setStyleSheet(
        "background-color: #0d1117;"
        "color: #58a6ff;"
        "border: 2px solid #21262d;"
        "border-radius: 10px;"
        "padding: 18px;"
        "font-family: monospace;"
        "font-size: 22px;"
        "font-weight: 700;"
        "letter-spacing: 3px;"
        );
    m_codeLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    root->addWidget(m_codeLabel);

    //Buttons row
    QHBoxLayout* btnRow = new QHBoxLayout();
    btnRow->setSpacing(12);

    //Makes it easy to Directly copy the code
    QPushButton* copyBtn = new QPushButton("📋  Copy to Clipboard");
    copyBtn->setObjectName("primaryBtn");
    copyBtn->setFixedHeight(42);
    copyBtn->setCursor(Qt::PointingHandCursor);
    connect(copyBtn, &QPushButton::clicked, this, &RecoveryCodeDialog::onCopyClicked);

    QPushButton* okBtn = new QPushButton("I've Saved It");
    okBtn->setObjectName("primaryBtn");
    okBtn->setFixedHeight(42);
    okBtn->setCursor(Qt::PointingHandCursor);
    okBtn->setDefault(true);
    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);

    btnRow->addWidget(copyBtn);
    btnRow->addStretch();
    btnRow->addWidget(okBtn);
    root->addLayout(btnRow);
}

void RecoveryCodeDialog::onCopyClicked()
{
    QApplication::clipboard()->setText(m_code);

    //Quick visual feedback — change the code label briefly to confirm (Effect confirms that its copied)
    QString original = m_codeLabel->text();
    m_codeLabel->setText("✓  Copied!");
    m_codeLabel->setStyleSheet(
        "background-color: #0d1117;"
        "color: #3fb950;"
        "border: 2px solid #3fb950;"
        "border-radius: 10px;"
        "padding: 18px;"
        "font-size: 22px;"
        "font-weight: 700;"
        );

    // Restore after 1.2 seconds
    QTimer::singleShot(1200, this, [this, original]() {
        m_codeLabel->setText(original);
        m_codeLabel->setStyleSheet(
            "background-color: #0d1117;"
            "color: #58a6ff;"
            "border: 2px solid #21262d;"
            "border-radius: 10px;"
            "padding: 18px;"
            "font-family: monospace;"
            "font-size: 22px;"
            "font-weight: 700;"
            "letter-spacing: 3px;"
            );
    });
}