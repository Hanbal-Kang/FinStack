#pragma once
#include "ui/BaseWindow.h"
#include "models/User.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>

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