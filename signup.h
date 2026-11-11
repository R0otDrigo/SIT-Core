#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>

#include <QWidget>
#include "firebaseauthmanager.h"
namespace Ui {
class SignUp;
}

class SignUp : public QWidget
{
    Q_OBJECT

public:
    void setFirebaseConfig(const QString &apiKey, const QString &projectId);
    explicit SignUp(QWidget *parent = nullptr);
    ~SignUp();


private slots:
    void onSignUpClicked();
    void onSignUpSuccess();
    void onSignUpFailed(const QString &error);

private:
    Ui::SignUp *ui;

    FirebaseAuthManager *m_authManager;
};

#endif // SIGNUP_H
