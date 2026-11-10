#include "signup.h"
#include "ui_signup.h"

SignUp::SignUp(QWidget *parent)
    : QWidget(parent)
    , m_authManager(new FirebaseAuthManager(this))
    , ui(new Ui::SignUp)
{
    ui->setupUi(this);
}

SignUp::~SignUp()
{
    delete ui;
}
