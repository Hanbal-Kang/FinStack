#pragma once
#include <QDialog>
#include <QLabel>
#include <QPushButton>

class RecoveryCodeDialog : public QDialog
{
    Q_OBJECT
public:
    explicit RecoveryCodeDialog(const QString& code, QWidget* parent = nullptr);

private:
    QString m_code;
    QLabel* m_codeLabel = nullptr;
    void setupUI();

private slots:
    void onCopyClicked();
};