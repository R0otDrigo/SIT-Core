#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QString>
#include "firebaseauthmanager.h"

namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    void setFirebaseConfig(const QString &apiKey, const QString &projectId);
    explicit Login(QWidget *parent = nullptr);
    ~Login();

signals:
    void loginSuccessful(const QString &role);

private slots:
    void onLoginClicked();
    void onSignInSuccess(const QString &localId, const QString &idToken, const QString &role);
    void onSignInFailed(const QString &error);


private:
    Ui::Login *ui;
    FirebaseAuthManager *m_authManager;
};

#endif // LOGIN_H
