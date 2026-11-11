
#include "signup.h"
#include "ui_signup.h"
#include "login.h"
#include "ui_login.h"
#include "QSettings"

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
    , signUp(new Ui::SignUp)
    , m_authManager(new FirebaseAuthManager(this))
{
    ui->setupUi(this);

    connect(ui->loginButton, &QPushButton::clicked, this, &Login::onLoginClicked);
    connect(ui->signUpButton, &QPushButton::clicked, this, &Login::onSignUpClicked);
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


void Login::onSignUpClicked(){
    // Crear el objeto en el heap
    SignUp *signDialog = new SignUp(this);

    // Importante: decirle a Qt que elimine la memoria automáticamente al cerrar la ventana
    signDialog->setAttribute(Qt::WA_DeleteOnClose);

    const QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(configPath, QSettings::IniFormat);

    QString API_KEY = settings.value("FIREBASE_API_KEY").toString();

    QString PROJECT_ID = settings.value("FIREBASE_PROJECT_ID").toString();
    signDialog->setFirebaseConfig(API_KEY, PROJECT_ID);

    signDialog->show();
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
