#pragma once
#include <QMainWindow>
#include "firebaseauthmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class LoginDialog;

// Ventana principal: alterna entre el login y el dashboard del rol autenticado.
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onAuthenticated(const AuthResult &result);
    void showLogin();

private:
    QWidget *createDashboard(const AuthResult &result);

    Ui::MainWindow *ui;
    FirebaseAuthManager *m_auth;
    LoginDialog *m_login;
    QWidget *m_dashboard = nullptr;
};
