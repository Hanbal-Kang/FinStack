#pragma once
#include "ui/BaseWindow.h"
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>

class ForgotPasswordWindow : public BaseWindow
{
    Q_OBJECT
public:
    explicit ForgotPasswordWindow(QWidget* parent = nullptr);
    ~ForgotPasswordWindow() override = default;

protected:
    void setupUI()override;
    void applyStyles() override;

private:
    QLineEdit*   m_usernameInput = nullptr;
    QLineEdit*   m_codeInput = nullptr;
    QLineEdit*   m_passwordInput = nullptr;
    QLineEdit*   m_confirmInput  = nullptr;
    QLabel*      m_errorLabel = nullptr;
    QPushButton* m_submitBtn = nullptr;
    QPushButton* m_backBtn = nullptr;

private slots:
    void onSubmitClicked();
    void onBackClicked();
};