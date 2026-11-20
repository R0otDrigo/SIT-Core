#include "signup.h"
#include "ui_signup.h"

#include <QCheckBox>
#include <QEvent>
#include <QGraphicsDropShadowEffect>
#include <QStyle>
#include <QTimer>

SignUp::SignUp(FirebaseAuthManager *authManager, QWidget *parent)
    : QWidget(parent), ui(new Ui::SignUp), m_authManager(authManager) {
  ui->setupUi(this);
  setWindowFlag(Qt::Window);
  setWindowModality(Qt::ApplicationModal);
  setWindowTitle(tr("SIT-Core | Crear cuenta"));

  auto *shadow = new QGraphicsDropShadowEffect(ui->card);
  shadow->setBlurRadius(40);
  shadow->setOffset(0, 12);
  shadow->setColor(QColor(15, 23, 42, 40));
  ui->card->setGraphicsEffect(shadow);

  ui->correoElectronicoLineEdit->setClearButtonEnabled(true);
  setInfo(QString(), false);

  connect(ui->confirmButton, &QPushButton::clicked, this, &SignUp::onSignUpClicked);
  connect(ui->backButton, &QPushButton::clicked, this, &SignUp::close);
  connect(ui->showPasswordCheckBox, &QCheckBox::toggled, this, [this](bool visible) {
    ui->contraseALineEdit->setEchoMode(visible ? QLineEdit::Normal : QLineEdit::Password);
  });
  connect(m_authManager, &FirebaseAuthManager::signUpSucceeded, this, &SignUp::onSignUpSuccess);
  connect(m_authManager, &FirebaseAuthManager::signUpFailed, this, &SignUp::onSignUpFailed);
}

SignUp::~SignUp() { delete ui; }

void SignUp::setInfo(const QString &text, bool error) {
  ui->info->setText(text);
  ui->info->setProperty("state", error ? "error" : "idle");
  ui->info->style()->unpolish(ui->info);
  ui->info->style()->polish(ui->info);
}

void SignUp::reset() {
  ui->info->clear();
  ui->showPasswordCheckBox->setChecked(false);
  ui->contraseALineEdit->setEchoMode(QLineEdit::Password);
  ui->confirmButton->setEnabled(true);
  setInfo(QString(), false);
  ui->correoElectronicoLineEdit->setFocus();
}

void SignUp::onSignUpClicked() {
  QString email = ui->correoElectronicoLineEdit->text().trimmed();
  QString password = ui->contraseALineEdit->text();
  QString username = ui->nombreDeUsuarioLineEdit->text().trimmed();
  QString role = ui->rolComboBox->currentText();

  if (email.isEmpty() || password.isEmpty() || username.isEmpty()) {
    setInfo(tr("Completa el correo, la contraseña y el nombre de usuario."), true);
    return;
  }
  if (password.size() < 6) {
    setInfo(tr("La contraseña debe tener al menos 6 caracteres."), true);
    return;
  }

  setInfo(tr("Creando cuenta..."), false);
  ui->confirmButton->setEnabled(false);
  m_authManager->signUp(email, password, role, username);
}

void SignUp::onSignUpSuccess() {
  ui->confirmButton->setEnabled(true);
  setInfo(tr("Cuenta creada. Ya puedes iniciar sesión."), false);
  QTimer::singleShot(900, this, &QWidget::close);
}

void SignUp::onSignUpFailed(const QString &error) {
  QString message = error;
  if (error.contains("EMAIL_EXISTS")) {
    message = tr("Ese correo ya está registrado.");
  } else if (error.contains("WEAK_PASSWORD")) {
    message = tr("La contraseña debe tener al menos 6 caracteres.");
  } else if (error.contains("INVALID_EMAIL")) {
    message = tr("El correo ingresado no es válido.");
  } else if (error.isEmpty()) {
    message = tr("No se pudo conectar. Revisa tu conexión a internet.");
  }

  ui->confirmButton->setEnabled(true);
  setInfo(message, true);
}

void SignUp::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange)
    retranslate();
  QWidget::changeEvent(event);
}

void SignUp::retranslate() { ui->retranslateUi(this); }
