#include "signup.h"
#include "ui_signup.h"


#include <iostream>
#include "string"

SignUp::SignUp(QWidget *parent)
    : QWidget(parent)
    , m_authManager(new FirebaseAuthManager(this))
    , ui(new Ui::SignUp)
{
    ui->setupUi(this);

    connect(ui->confirmButton, &QPushButton::clicked, this, &SignUp::onSignUpClicked);
    connect(m_authManager, &FirebaseAuthManager::signUpSucceeded, this, &SignUp::onSignUpSuccess);
    connect(m_authManager, &FirebaseAuthManager::signUpFailed, this, &SignUp::onSignUpFailed);
}


void SignUp::setFirebaseConfig(const QString &apiKey, const QString &projectId)
{
    m_authManager->setApiKey(apiKey);
    m_authManager->setProjectId(projectId);
}

void SignUp::onSignUpClicked()
{
    QString email = ui->correoElectronicoLineEdit->text().trimmed();
    QString password = ui->contraseALineEdit->text();
    QString role = ui->rolComboBox->currentText();

    if (email.isEmpty() || password.isEmpty()) {
        ui->info->setText("Please enter email and password");
        return;
    }

    ui->info->setText("Register incoming...");
    ui->confirmButton->setEnabled(false);
    m_authManager->signUp(email, password, role);
}

void SignUp::onSignUpSuccess()
{
    ui->confirmButton->setEnabled(true);
    ui->info->setText("Registro Completado");


    std::cout << "En teoria si se registró correctamente" << std::endl;
}

void SignUp::onSignUpFailed(const QString &error)
{
    ui->confirmButton->setEnabled(true);
    ui->info->setText("Error: " + error);
    std::cout << qPrintable(error) << std::endl;
}


SignUp::~SignUp()
{
    delete ui;
}
