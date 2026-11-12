#include "logindialog.h"
#include "firebaseauthmanager.h"
#include "uihelpers.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

LoginDialog::LoginDialog(FirebaseAuthManager *auth, QWidget *parent)
    : QWidget(parent), m_auth(auth)
{
    auto *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ---------- Panel izquierdo (marca) ----------
    auto *hero = new QFrame;
    hero->setObjectName("loginHero");
    hero->setMinimumWidth(400);
    auto *hv = new QVBoxLayout(hero);
    hv->setContentsMargins(48, 48, 48, 48);
    hv->addStretch();
    hv->addWidget(UI::label(QStringLiteral("🚚"), "heroTitle"));
    hv->addWidget(UI::label(QStringLiteral("SIT-Core"), "heroTitle"));
    auto *sub = UI::label(tr("Sistema de Gestión y Monitoreo de Flotas"), "heroSub");
    sub->setWordWrap(true);
    hv->addWidget(sub);
    hv->addSpacing(24);
    hv->addWidget(UI::label(tr("•  Monitoreo de unidades en tiempo real"), "heroBullet"));
    hv->addWidget(UI::label(tr("•  Control de horarios y asignaciones"), "heroBullet"));
    hv->addWidget(UI::label(tr("•  Delegación inteligente de tareas (IA / RPA)"), "heroBullet"));
    hv->addStretch();
    root->addWidget(hero, 4);

    // ---------- Panel derecho (formulario) ----------
    auto *right = new QWidget;
    right->setObjectName("loginRight");
    right->setAttribute(Qt::WA_StyledBackground, true);
    auto *rv = new QVBoxLayout(right);
    rv->setContentsMargins(40, 30, 40, 30);

    QFrame *c = UI::card();
    c->setMaximumWidth(460);
    auto *cv = new QVBoxLayout(c);
    cv->setContentsMargins(32, 28, 32, 28);
    cv->setSpacing(8);

    cv->addWidget(UI::label(tr("Iniciar sesión"), "h1"));
    cv->addWidget(UI::label(tr("Ingresa con tu cuenta corporativa"), "muted"));
    cv->addSpacing(10);

    cv->addWidget(UI::label(tr("Correo electrónico")));
    m_email = new QLineEdit;
    m_email->setPlaceholderText(QStringLiteral("usuario@empresa.com"));
    cv->addWidget(m_email);

    cv->addSpacing(4);
    cv->addWidget(UI::label(tr("Contraseña")));
    m_password = new QLineEdit;
    m_password->setEchoMode(QLineEdit::Password);
    m_password->setPlaceholderText(QStringLiteral("••••••••"));
    cv->addWidget(m_password);

    m_error = UI::label(QString(), "errorLabel");
    m_error->setWordWrap(true);
    m_error->hide();
    cv->addWidget(m_error);

    cv->addSpacing(6);
    m_loginBtn = UI::button(tr("Ingresar"));
    m_loginBtn->setMinimumHeight(40);
    cv->addWidget(m_loginBtn);

    cv->addSpacing(14);
    cv->addWidget(UI::label(tr("ACCESO RÁPIDO DEMO"), "statTitle"));
    cv->addWidget(UI::label(tr("Modo Demo offline: no requiere Firebase ni contraseña."), "muted"));

    const auto accounts = FirebaseAuthManager::demoAccounts();
    for (const DemoAccount &a : accounts) {
        auto *b = UI::button(QStringLiteral("%1   ·   %2").arg(a.description, a.email), "demoButton");
        b->setProperty("demoEmail", a.email);
        m_demoButtons << b;
        cv->addWidget(b);
        connect(b, &QPushButton::clicked, this, [this, a]() {
            m_error->hide();
            setBusy(true);
            m_auth->signIn(a.email, QStringLiteral("demo"));
        });
    }

    rv->addStretch();
    rv->addWidget(c, 0, Qt::AlignHCenter);
    rv->addStretch();
    root->addWidget(right, 5);

    connect(m_loginBtn, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(m_password, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    connect(m_email, &QLineEdit::returnPressed, m_password, qOverload<>(&QWidget::setFocus));
    connect(m_auth, &FirebaseAuthManager::authenticated, this, &LoginDialog::onAuthenticated);
    connect(m_auth, &FirebaseAuthManager::authFailed, this, &LoginDialog::onAuthFailed);
}

void LoginDialog::onLoginClicked()
{
    const QString email = m_email->text().trimmed();
    if (email.isEmpty()) {
        onAuthFailed(tr("Ingresa tu correo electrónico."));
        return;
    }
    if (!FirebaseAuthManager::findDemoAccount(email) && m_password->text().isEmpty()) {
        onAuthFailed(tr("Ingresa tu contraseña."));
        return;
    }
    m_error->hide();
    setBusy(true);
    m_auth->signIn(email, m_password->text());
}

void LoginDialog::onAuthenticated(const AuthResult &)
{
    setBusy(false);
}

void LoginDialog::onAuthFailed(const QString &message)
{
    setBusy(false);
    m_error->setText(message);
    m_error->show();
}

void LoginDialog::setBusy(bool busy)
{
    m_loginBtn->setEnabled(!busy);
    m_loginBtn->setText(busy ? tr("Verificando...") : tr("Ingresar"));
    for (QPushButton *b : std::as_const(m_demoButtons))
        b->setEnabled(!busy);
}

void LoginDialog::reset()
{
    m_password->clear();
    m_error->hide();
    setBusy(false);
}
