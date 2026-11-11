#include "login.h"
#include "ui_login.h"

#include <QApplication>
#include <QEvent>

Login::Login(QWidget *parent)
    : QWidget(parent), ui(new Ui::Login), m_authManager(&FirebaseAuthManager::instance()) {
  ui->setupUi(this);
  ui->info->hide();

  connect(ui->loginButton, &QPushButton::clicked, this, &Login::onLoginClicked);
  connect(ui->correoLineEdit, &QLineEdit::returnPressed, this, &Login::onLoginClicked);
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

void Login::setBusy(bool busy) {
  ui->loginButton->setEnabled(!busy);
  ui->loginButton->setText(busy ? tr("Ingresando…") : tr("Ingresar"));
}

void Login::showError(const QString &message) {
  ui->info->setText(message);
  ui->info->setVisible(!message.isEmpty());
}

void Login::onLoginClicked() {
  if (!ui->loginButton->isEnabled())
    return; // ya hay una solicitud en curso

  const QString email = ui->correoLineEdit->text().trimmed();
  const QString password = ui->contraseALineEdit->text();

  if (email.isEmpty() || password.isEmpty()) {
    showError(tr("Por favor, ingresa tu correo y contraseña."));
    return;
  }

  showError(QString());
  setBusy(true);
  m_authManager->signIn(email, password);
}

// SignUp es una ventana modal hija que comparte el mismo FirebaseAuthManager (ya configurado).
void Login::onSignUpClicked() {
  if (!signUp)
    signUp = new SignUp(&FirebaseAuthManager::instance(), this);
  signUp->show();
}

void Login::onSignInSuccess(const QString &localId, const QString &idToken,
                            const QString &role) {
  setBusy(false);
  showError(QString());
  emit loginSuccessful(localId, idToken, role);
}

void Login::onSignInFailed(const QString &error) {
  setBusy(false);
  showError(error.isEmpty() ? tr("Error: Sin conexión disponible") : tr("Error: ") + error);
}

void Login::reset() {
  ui->contraseALineEdit->clear();
  showError(QString());
  setBusy(false);
}

void Login::retranslate() {
  ui->retranslateUi(this);
  setBusy(!ui->loginButton->isEnabled());
}

void Login::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange)
    retranslate();
  QWidget::changeEvent(event);
}
