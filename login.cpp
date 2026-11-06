#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
    , m_authManager(new FirebaseAuthManager(this))
{
    ui->setupUi(this);

    connect(ui->loginButton, &QPushButton::clicked, this, &Login::onLoginClicked);
    connect(m_authManager, &FirebaseAuthManager::signInSuccess, this, &Login::onSignInSuccess);
    connect(m_authManager, &FirebaseAuthManager::signInFailed, this, &Login::onSignInFailed);
}


void Login::setFirebaseConfig(const QString &apiKey, const QString &projectId)
{
    m_authManager->setApiKey(apiKey);
    m_authManager->setProjectId(projectId);
}

void Login::onLoginClicked()
{
    QString email = ui->correoLineEdit->text().trimmed();
    QString password = ui->contraseALineEdit->text();

    if (email.isEmpty() || password.isEmpty()) {
        ui->info->setText("Please enter email and password");
        return;
    }

    ui->info->setText("Logging in...");
    ui->loginButton->setEnabled(false);
    m_authManager->signIn(email, password);
}

void Login::onSignInSuccess(const QString &localId, const QString &idToken, const QString &role)
{
    Q_UNUSED(localId);
    Q_UNUSED(idToken);
    ui->loginButton->setEnabled(true);
    ui->info->setText("");
    // accept(); // What does that ... o.o
    emit loginSuccessful(role);
}

void Login::onSignInFailed(const QString &error)
{
    ui->loginButton->setEnabled(true);
    ui->info->setText("Error: " + error);
}


Login::~Login()
{
    delete ui;
}
