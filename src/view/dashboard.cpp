#include "dashboard.h"

#include <QEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>

namespace {

QString roleText(const QString &role) {
    const QString normalized = role.trimmed().toUpper();
    if (normalized == QStringLiteral("ADMIN") || normalized == QStringLiteral("ADMINISTRATOR"))
        return QObject::tr("Administrador");
    if (normalized == QStringLiteral("GUESS") || normalized == QStringLiteral("GUEST"))
        return QObject::tr("Invitado");
    if (normalized.isEmpty())
        return QString();
    return QObject::tr("Usuario");
}

QString roleStyle(const QString &role) {
    const QString normalized = role.trimmed().toUpper();
    if (normalized == QStringLiteral("ADMIN") || normalized == QStringLiteral("ADMINISTRATOR"))
        return QStringLiteral("admin");
    if (normalized == QStringLiteral("GUESS") || normalized == QStringLiteral("GUEST"))
        return QStringLiteral("guest");
    return QStringLiteral("user");
}

} // namespace

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent), brand_(new QLabel(this)), subtitle_(new QLabel(this)),
      title_(new QLabel(this)), welcome_(new QLabel(this)),
      roleBadge_(new QLabel(this)), logoutButton_(new QPushButton(this)),
      contentContainer_(new QWidget(this)), content_(new QVBoxLayout(contentContainer_)) {
    brand_->setObjectName("appBrand");
    subtitle_->setObjectName("appSubtitle");
    title_->setObjectName("dashboardTitle");
    welcome_->setObjectName("welcomeLabel");
    roleBadge_->setObjectName("roleBadge");
    logoutButton_->setObjectName("logoutButton");
    logoutButton_->setCursor(Qt::PointingHandCursor);
    connect(logoutButton_, &QPushButton::clicked, this, &Dashboard::logoutRequested);

    auto *brandBlock = new QVBoxLayout;
    brandBlock->setSpacing(0);
    brandBlock->addWidget(brand_);
    brandBlock->addWidget(subtitle_);

    auto *header = new QFrame(this);
    header->setObjectName("appHeader");
    header->setFixedHeight(68);
    auto *headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 10, 20, 10);
    headerLayout->setSpacing(16);
    headerLayout->addLayout(brandBlock);
    headerLayout->addWidget(title_);
    headerLayout->addStretch();
    headerLayout->addWidget(roleBadge_);
    headerLayout->addWidget(welcome_);
    headerLayout->addWidget(logoutButton_);

    contentContainer_->setObjectName("dashboardContent");
    content_->setContentsMargins(20, 16, 20, 20);
    content_->setSpacing(12);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);
    root->addWidget(header);
    root->addWidget(contentContainer_, 1);

    retranslate();
}

QString Dashboard::dashboardTitle() const { return tr("Dashboard"); }

QString Dashboard::welcomeMessage() const { return tr("Bienvenido"); }

void Dashboard::setSessionInfo(const QString &name, const QString &role) {
    userName_ = name.trimmed();
    role_ = role.trimmed().toUpper();
    updateSessionLabels();
}

void Dashboard::updateSessionLabels() {
    const QString text = roleText(role_);
    const bool duplicateName = userName_.isEmpty() ||
                               userName_.compare(text, Qt::CaseInsensitive) == 0;
    welcome_->setText(userName_);
    welcome_->setVisible(!duplicateName);
    roleBadge_->setText(text);
    roleBadge_->setVisible(!text.isEmpty());
    roleBadge_->setProperty("role", roleStyle(role_));
    roleBadge_->style()->unpolish(roleBadge_);
    roleBadge_->style()->polish(roleBadge_);
}

// Llamarlo tambien al final del constructor de cada subclase: dentro del constructor base
// las funciones virtuales todavia no despachan a la subclase.
void Dashboard::retranslate() {
    brand_->setText(QStringLiteral("SIT-Core"));
    subtitle_->setText(tr("Transporte urbano"));
    title_->setText(dashboardTitle());
    logoutButton_->setText(tr("Cerrar sesión"));
    updateSessionLabels();
}

void Dashboard::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange)
        retranslate();
    QWidget::changeEvent(event);
}
