#include "signup.h"
#include "ui_signup.h"

#include <QEvent>

SignUp::SignUp(FirebaseAuthManager *authManager, QWidget *parent)
    : QWidget(parent), ui(new Ui::SignUp),
      m_authManager(authManager ? authManager : &FirebaseAuthManager::instance()) {
  ui->setupUi(this);
  ui->info->hide();
  setWindowFlag(Qt::Window);
  setWindowModality(Qt::ApplicationModal);

  connect(ui->confirmButton, &QPushButton::clicked, this, &SignUp::onSignUpClicked);
  for (QLineEdit *edit : {ui->nombreDeUsuarioLineEdit, ui->correoElectronicoLineEdit,
                          ui->contraseALineEdit})
    connect(edit, &QLineEdit::returnPressed, this, &SignUp::onSignUpClicked);
  connect(ui->backButton, &QPushButton::clicked, this, &QWidget::close);
  connect(m_authManager, &FirebaseAuthManager::signUpSucceeded, this, &SignUp::onSignUpSuccess);
  connect(m_authManager, &FirebaseAuthManager::signUpFailed, this, &SignUp::onSignUpFailed);
}

SignUp::~SignUp() { delete ui; }

void SignUp::setBusy(bool busy) {
  ui->confirmButton->setEnabled(!busy);
  ui->confirmButton->setText(busy ? tr("Registrando…") : tr("Crear cuenta"));
}

void SignUp::showError(const QString &message) {
  ui->info->setText(message);
  ui->info->setVisible(!message.isEmpty());
}

void SignUp::onSignUpClicked() {
  if (!ui->confirmButton->isEnabled())
    return; // ya hay una solicitud en curso

  QString email = ui->correoElectronicoLineEdit->text().trimmed();
  QString password = ui->contraseALineEdit->text();
  QString username = ui->nombreDeUsuarioLineEdit->text();
  QString role = ui->rolComboBox->currentText();

  if (email.isEmpty() || password.isEmpty()) {
    showError(tr("Por favor, ingresa tu correo y contraseña."));
    return;
  }

  showError(QString());
  setBusy(true);
  m_authManager->signUp(email, password, role, username);
}

void SignUp::onSignUpSuccess() {
  setBusy(false);
  showError(QString());
  ui->contraseALineEdit->clear();
  close();
}

void SignUp::onSignUpFailed(const QString &error) {
  setBusy(false);
  showError(error.isEmpty() ? tr("Error: Sin conexión a internet") : tr("Error: ") + error);
}

void SignUp::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange)
    retranslate();
  QWidget::changeEvent(event);
}

void SignUp::retranslate() {
  ui->retranslateUi(this);
  setBusy(!ui->confirmButton->isEnabled());
}
