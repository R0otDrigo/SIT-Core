#ifndef LOGIN_H
#define LOGIN_H

#include "controller/firebaseauthmanager.h"
#include "signup.h"
#include <QWidget>

namespace Ui {
class Login;
}

class Login : public QWidget {
  Q_OBJECT
public:
  explicit Login(QWidget *parent = nullptr);
  ~Login() override;

  void setFirebaseConfig(const QString &apiKey, const QString &projectId);
  void reset();
  void retranslate();

signals:
  void loginSuccessful(const QString &userId, const QString &idToken,
                       const QString &role);

protected:
  void changeEvent(QEvent *event) override;

private slots:
  void onLoginClicked();
  void onSignUpClicked();
  void onSignInSuccess(const QString &localId, const QString &idToken,
                       const QString &role);
  void onSignInFailed(const QString &error);

private:
  Ui::Login *ui;
  SignUp *signUp = nullptr;
  FirebaseAuthManager *m_authManager;
};

#endif // LOGIN_H
