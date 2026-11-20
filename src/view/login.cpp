#include "login.h"
#include "ui_login.h"

#include <QApplication>
#include <QCheckBox>
#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QStyle>

Login::Login(QWidget *parent)
    : QWidget(parent), ui(new Ui::Login), m_authManager(new FirebaseAuthManager(this)) {
  ui->setupUi(this);

  auto *shadow = new QGraphicsDropShadowEffect(ui->card);
  shadow->setBlurRadius(40);
  shadow->setOffset(0, 12);
  shadow->setColor(QColor(15, 23, 42, 40));
  ui->card->setGraphicsEffect(shadow);

  ui->correoLineEdit->setClearButtonEnabled(true);
  setInfo(QString(), false);

  connect(ui->loginButton, &QPushButton::clicked, this, &Login::onLoginClicked);
  connect(ui->contraseALineEdit, &QLineEdit::returnPressed, this, &Login::onLoginClicked);
  connect(ui->signUpButton, &QPushButton::clicked, this, &Login::onSignUpClicked);
  connect(ui->exitButton, &QPushButton::clicked, qApp, &QApplication::quit);
  connect(ui->showPasswordCheckBox, &QCheckBox::toggled, this, [this](bool visible) {
    ui->contraseALineEdit->setEchoMode(visible ? QLineEdit::Normal : QLineEdit::Password);
  });

  connect(m_authManager, &FirebaseAuthManager::signInSuccess, this, &Login::onSignInSuccess);
  connect(m_authManager, &FirebaseAuthManager::signInFailed, this, &Login::onSignInFailed);
}

Login::~Login() { delete ui; }

void Login::setFirebaseConfig(const QString &apiKey, const QString &projectId) {
  m_authManager->setApiKey(apiKey);
  m_authManager->setProjectId(projectId);
}

void Login::setInfo(const QString &text, bool error) {
  ui->info->setText(text);
  ui->info->setProperty("state", error ? "error" : "idle");
  ui->info->style()->unpolish(ui->info);
  ui->info->style()->polish(ui->info);
}

void Login::onLoginClicked() {
  const QString email = ui->correoLineEdit->text().trimmed();
  const QString password = ui->contraseALineEdit->text();

  if (email.isEmpty() || password.isEmpty()) {
    setInfo(tr("Ingresa tu correo y contraseña."), true);
    return;
  }

  setInfo(tr("Iniciando sesión..."), false);
  ui->loginButton->setEnabled(false);
  m_authManager->signIn(email, password);
}

// SignUp es una ventana modal hija que comparte el mismo FirebaseAuthManager (ya configurado).
void Login::onSignUpClicked() {
  if (!signUp)
    signUp = new SignUp(m_authManager, this);
  signUp->reset();
  signUp->show();
}

void Login::onSignInSuccess(const QString &localId, const QString &idToken,
                            const QString &role) {
  ui->loginButton->setEnabled(true);
  setInfo(QString(), false);
  emit loginSuccessful(localId, idToken, role);
}

void Login::onSignInFailed(const QString &error) {
  QString message = error;
  if (error.contains("INVALID_LOGIN_CREDENTIALS") || error.contains("EMAIL_NOT_FOUND") ||
      error.contains("INVALID_PASSWORD")) {
    message = tr("Correo o contraseña incorrectos.");
  } else if (error.contains("INVALID_EMAIL")) {
    message = tr("El correo ingresado no es válido.");
  } else if (error.contains("TOO_MANY_ATTEMPTS_TRY_LATER")) {
    message = tr("Demasiados intentos. Espera un momento e inténtalo de nuevo.");
  } else if (error.isEmpty()) {
    message = tr("No se pudo conectar. Revisa tu conexión a internet.");
  }

  ui->loginButton->setEnabled(true);
  setInfo(message, true);
}

void Login::reset() {
  ui->contraseALineEdit->clear();
  ui->showPasswordCheckBox->setChecked(false);
  ui->contraseALineEdit->setEchoMode(QLineEdit::Password);
  ui->loginButton->setEnabled(true);
  setInfo(QString(), false);
}

void Login::retranslate() { ui->retranslateUi(this); }

void Login::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange)
    retranslate();
  QWidget::changeEvent(event);
}
