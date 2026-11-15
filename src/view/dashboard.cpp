#include "dashboard.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent), title_(new QLabel(this)), welcome_(new QLabel(this)),
      content_(new QVBoxLayout) {
    // El estilo lo pone la hoja QSS global segun la propiedad "role".
    title_->setProperty("role", "title");
    welcome_->setProperty("role", "subtitle");

    auto *header = new QHBoxLayout;
    header->addWidget(title_);
    header->addStretch();
    header->addWidget(welcome_);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);
    content_->setSpacing(12);
    root->addLayout(header);
    root->addLayout(content_, 1);

    retranslate();
}

QString Dashboard::dashboardTitle() const { return tr("Dashboard"); }

QString Dashboard::welcomeMessage() const { return tr("Welcome"); }

// Llamarlo tambien al final del constructor de cada subclase: dentro del constructor base
// las funciones virtuales todavia no despachan a la subclase.
void Dashboard::retranslate() {
    title_->setText(dashboardTitle());
    welcome_->setText(welcomeMessage());
}

void Dashboard::showError(const QString &message) {
    // Un solo label: reintentar el login no apila mensajes repetidos.
    if (!error_) {
        error_ = new QLabel(this);
        error_->setProperty("role", "error");
        error_->setWordWrap(true);
        content_->addWidget(error_, 1);
    }
    error_->setText(message);
}

void Dashboard::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange)
        retranslate();
    QWidget::changeEvent(event);
}
