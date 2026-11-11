#ifndef SIGNUP_H
#define SIGNUP_H

#include <QWidget>
#include "firebaseauthmanager.h"

class QCloseEvent;

namespace Ui {
class SignUp;
}

class SignUp : public QWidget
{
    Q_OBJECT

public:
    void setFirebaseConfig(const QString &apiKey, const QString &projectId);
    void setFirebaseAuthManager(FirebaseAuthManager *reference);
    explicit SignUp(QWidget *parent = nullptr);
    ~SignUp();

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void onSignUpClicked();
    void onSignUpSuccess();
    void onSignUpFailed(const QString &error);

private:
    Ui::SignUp *ui;

    FirebaseAuthManager *m_authManager = nullptr;
};

#endif // SIGNUP_H
