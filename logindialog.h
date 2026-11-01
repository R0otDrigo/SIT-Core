#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include "firebaseauthmanager.h"

class LoginDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoginDialog(QWidget *parent = nullptr);
    void setFirebaseConfig(const QString &apiKey, const QString &projectId);

signals:
    void loginSuccessful(const QString &role);

private slots:
    void onLoginClicked();
    void onSignInSuccess(const QString &localId, const QString &idToken, const QString &role);
    void onSignInFailed(const QString &error);

private:
    QLineEdit *m_emailEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginButton;
    QLabel *m_statusLabel;
    FirebaseAuthManager *m_authManager;
};

#endif // LOGINDIALOG_H
