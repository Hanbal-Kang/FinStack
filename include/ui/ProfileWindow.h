#pragma once
#include "ui/BaseWindow.h"
#include "models/User.h"
#include "services/AuthService.h"
#include "utils/Validator.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QDialog>
#include <QLineEdit>

//Dialog for changing the password while logged in
class ChangePasswordDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ChangePasswordDialog(const QString& username, QWidget* parent = nullptr);

private:
    QString    m_username;
    QLineEdit* m_oldPwInput  = nullptr;
    QLineEdit* m_newPwInput  = nullptr;
    QLineEdit* m_confirmInput = nullptr;
    QLabel*    m_errorLabel  = nullptr;
    void setupUI();

private slots:
    void onConfirmClicked();
};


class ProfileWindow : public BaseWindow
{
    Q_OBJECT
public:
    explicit ProfileWindow(const User& user, QWidget* parent = nullptr);
    ~ProfileWindow() = default;

private:
    User m_user;

    void setupUI() override;
    void applyStyles() override;

    QFrame* buildInfoCard(const QString& iconSymbol, const QString& label, const QString& value);

private slots:
    void onChangePasswordClicked();
    void onLogoutClicked();
    void onExportReportClicked();
};