#include "login.h"
#include "ui_login.h"

#include <QApplication>
#include <QEvent>

Login::Login(QWidget *parent)
    : QWidget(parent), ui(new Ui::Login), m_authManager(new FirebaseAuthManager(this)) {
  ui->setupUi(this);

  connect(ui->loginButton, &QPushButton::clicked, this, &Login::onLoginClicked);
  connect(ui->contraseALineEdit, &QLineEdit::returnPressed, this, &Login::onLoginClicked);
  connect(ui->signUpButton, &QPushButton::clicked, this, &Login::onSignUpClicked);
  connect(ui->exitButton, &QPushButton::clicked, qApp, &QApplication::quit);

  connect(m_authManager, &FirebaseAuthManager::signInSuccess, this, &Login::onSignInSuccess);
  connect(m_authManager, &FirebaseAuthManager::signInFailed, this, &Login::onSignInFailed);
}

Login::~Login() { delete ui; }

void Login::setFirebaseConfig(const QString &apiKey, const QString &projectId) {
  m_authManager->setApiKey(apiKey);
  m_authManager->setProjectId(projectId);
}

void Login::onLoginClicked() {
  const QString email = ui->correoLineEdit->text().trimmed();
  const QString password = ui->contraseALineEdit->text();

  if (email.isEmpty() || password.isEmpty()) {
    ui->info->setText(tr("Por favor, insertar correo y contraseña"));
    return;
  }

  ui->info->setText(tr("Iniciando Sessión..."));
  ui->loginButton->setEnabled(false);
  m_authManager->signIn(email, password);
}

// SignUp es una ventana modal hija que comparte el mismo FirebaseAuthManager (ya configurado).
void Login::onSignUpClicked() {
  if (!signUp)
    signUp = new SignUp(m_authManager, this);
  signUp->show();
}

void Login::onSignInSuccess(const QString &localId, const QString &idToken, const QString &role) {
  Q_UNUSED(localId);
  Q_UNUSED(idToken);
  ui->loginButton->setEnabled(true);
  ui->info->clear();
  emit loginSuccessful(role);
}

void Login::onSignInFailed(const QString &error) {
  ui->loginButton->setEnabled(true);
  ui->info->setText(error.isEmpty() ? tr("Error: Sin conexión disponible") : "Error: " + error);
}

void Login::reset() {
  ui->contraseALineEdit->clear();
  ui->info->clear();
}

void Login::retranslate() { ui->retranslateUi(this); }

void Login::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange)
    retranslate();
  QWidget::changeEvent(event);
}
