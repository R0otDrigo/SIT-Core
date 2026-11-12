#pragma once
#include <QWidget>

class FirebaseAuthManager;
class QLabel;
class QLineEdit;
class QPushButton;
struct AuthResult;

// Pantalla de inicio de sesión (Firebase + Acceso Rápido Demo).
class LoginDialog : public QWidget
{
    Q_OBJECT
public:
    explicit LoginDialog(FirebaseAuthManager *auth, QWidget *parent = nullptr);

public slots:
    void reset();

private slots:
    void onLoginClicked();
    void onAuthenticated(const AuthResult &result);
    void onAuthFailed(const QString &message);

private:
    void setBusy(bool busy);

    FirebaseAuthManager *m_auth;
    QLineEdit *m_email = nullptr;
    QLineEdit *m_password = nullptr;
    QPushButton *m_loginBtn = nullptr;
    QLabel *m_error = nullptr;
    QList<QPushButton *> m_demoButtons;
};
