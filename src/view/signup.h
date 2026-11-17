#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>
#include "controller/firebaseauthmanager.h"

namespace Ui {
class SignUp;
}

// Ventana modal de registro. Usa el FirebaseAuthManager de Login (no es su duenio).
class SignUp : public QWidget {
    Q_OBJECT
public:
    explicit SignUp(FirebaseAuthManager *authManager, QWidget *parent = nullptr);
    ~SignUp() override;

    void retranslate();

protected:
    void changeEvent(QEvent *event) override;

private slots:
    void onSignUpClicked();
    void onSignUpSuccess();
    void onSignUpFailed(const QString &error);

private:
    Ui::SignUp *ui;
    FirebaseAuthManager *m_authManager;
};

#endif // SIGNUP_H
