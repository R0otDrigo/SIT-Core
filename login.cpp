
#include "signup.h"
#include "ui_signup.h"
#include "login.h"
#include "ui_login.h"
#include "QSettings"
#include <QEvent>

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
    , signUp(nullptr)
    , m_authManager(new FirebaseAuthManager(this))
{
    ui->setupUi(this);

    connect(ui->loginButton, &QPushButton::clicked, this, &Login::onLoginClicked);
    connect(ui->signUpButton, &QPushButton::clicked, this, &Login::onSignUpClicked);
    connect(m_authManager, &FirebaseAuthManager::signInSuccess, this, &Login::onSignInSuccess);
    connect(m_authManager, &FirebaseAuthManager::signInFailed, this, &Login::onSignInFailed);
    connect(m_authManager, &FirebaseAuthManager::signUpSucceeded, this, &Login::onSignUpSuccess);
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
        ui->info->setText(tr("Por favor, insertar correo y contraseña"));
        return;
    }

    ui->info->setText(tr("Iniciando Sessión..."));
    ui->loginButton->setEnabled(false);
    m_authManager->signIn(email, password);
}


void Login::onSignUpClicked(){

    if (!signUp) {
        signUp = new SignUp();
        connect(signUp, &SignUp::closed, this, &Login::show);
    }

    signUp->setFirebaseAuthManager(m_authManager);

    const QString configPath = QCoreApplication::applicationDirPath() + "/config.ini";
    QSettings settings(configPath, QSettings::IniFormat);

    QString API_KEY = settings.value("FIREBASE_API_KEY").toString();

    QString PROJECT_ID = settings.value("FIREBASE_PROJECT_ID").toString();
    signUp->setFirebaseConfig(API_KEY, PROJECT_ID);

    signUp->show();

    this->hide();

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

void Login::onSignUpSuccess(){
    this->show();
}

void Login::reset(){

}

void Login::retranslate(){
    ui->retranslateUi(this);
}

void Login::changeEvent(QEvent *event){
    if (event->type() == QEvent::LanguageChange)
        retranslate();
    QWidget::changeEvent(event);
}

Login::~Login()
{
    delete signUp;
    delete ui;
}
