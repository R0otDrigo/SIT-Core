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
    explicit SignUp(QWidget *parent = nullptr);
    ~SignUp();

private:
    Ui::SignUp *ui;

    FirebaseAuthManager *m_authManager;
};

#endif // SIGNUP_H
