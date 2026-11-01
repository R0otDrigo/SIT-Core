#include "logindialog.h"
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent)
    , m_emailEdit(new QLineEdit(this))
    , m_passwordEdit(new QLineEdit(this))
    , m_loginButton(new QPushButton("Login", this))
    , m_statusLabel(new QLabel(this))
    , m_authManager(new FirebaseAuthManager(this))
{
    setWindowTitle("Login - Select Role");
    setFixedSize(400, 250);

    m_passwordEdit->setEchoMode(QLineEdit::Password);

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("Email:", m_emailEdit);
    formLayout->addRow("Password:", m_passwordEdit);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(m_loginButton);
    mainLayout->addWidget(m_statusLabel);
    setLayout(mainLayout);

    connect(m_loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(m_authManager, &FirebaseAuthManager::signInSuccess, this, &LoginDialog::onSignInSuccess);
    connect(m_authManager, &FirebaseAuthManager::signInFailed, this, &LoginDialog::onSignInFailed);
}

void LoginDialog::setFirebaseConfig(const QString &apiKey, const QString &projectId)
{
    m_authManager->setApiKey(apiKey);
    m_authManager->setProjectId(projectId);
}

void LoginDialog::onLoginClicked()
{
    QString email = m_emailEdit->text().trimmed();
    QString password = m_passwordEdit->text();

    if (email.isEmpty() || password.isEmpty()) {
        m_statusLabel->setText("Please enter email and password");
        return;
    }

    m_statusLabel->setText("Logging in...");
    m_loginButton->setEnabled(false);
    m_authManager->signIn(email, password);
}

void LoginDialog::onSignInSuccess(const QString &localId, const QString &idToken, const QString &role)
{
    Q_UNUSED(localId);
    Q_UNUSED(idToken);
    m_loginButton->setEnabled(true);
    m_statusLabel->setText("");
    accept();
    emit loginSuccessful(role);
}

void LoginDialog::onSignInFailed(const QString &error)
{
    m_loginButton->setEnabled(true);
    m_statusLabel->setText("Error: " + error);
}
