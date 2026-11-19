#include "signup.h"
#include "ui_signup.h"

#include <QEvent>

SignUp::SignUp(FirebaseAuthManager *authManager, QWidget *parent)
    : QWidget(parent), ui(new Ui::SignUp), m_authManager(authManager) {
  ui->setupUi(this);
  setWindowFlag(Qt::Window);
  setWindowModality(Qt::ApplicationModal);

  connect(ui->confirmButton, &QPushButton::clicked, this, &SignUp::onSignUpClicked);
  connect(m_authManager, &FirebaseAuthManager::signUpSucceeded, this, &SignUp::onSignUpSuccess);
  connect(m_authManager, &FirebaseAuthManager::signUpFailed, this, &SignUp::onSignUpFailed);
}

SignUp::~SignUp() { delete ui; }

void SignUp::onSignUpClicked() {
  QString email = ui->correoElectronicoLineEdit->text().trimmed();
  QString password = ui->contraseALineEdit->text();
  QString username = ui->nombreDeUsuarioLineEdit->text();
  QString role = ui->rolComboBox->currentText();

  if (email.isEmpty() || password.isEmpty()) {
    ui->info->setText(tr("Por favor, insertar correo y contraseña"));
    return;
  }

  ui->info->setText(tr("Registrando..."));
  ui->confirmButton->setEnabled(false);
  qDebug() << email << password << role << username;
  m_authManager->signUp(email, password, role, username);
}

void SignUp::onSignUpSuccess() {
  ui->confirmButton->setEnabled(true);
  ui->info->setText(tr("Registro Completado"));
  close();
}

void SignUp::onSignUpFailed(const QString &error) {
  ui->confirmButton->setEnabled(true);
  ui->info->setText(error.isEmpty() ? tr("Error: Sin conexión a internet") : "Error: " + error);
}

void SignUp::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange)
    retranslate();
  QWidget::changeEvent(event);
}

void SignUp::retranslate() { ui->retranslateUi(this); }
