#include "dashboard.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

Dashboard::Dashboard(QWidget *parent)
    : QWidget(parent), title_(new QLabel(this)), welcome_(new QLabel(this)),
      content_(new QVBoxLayout) {
    QFont font = title_->font();
    font.setPointSize(16);
    font.setBold(true);
    title_->setFont(font);

    auto *header = new QHBoxLayout;
    header->addWidget(title_);
    header->addStretch();
    header->addWidget(welcome_);

    auto *root = new QVBoxLayout(this);
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

void Dashboard::changeEvent(QEvent *event) {
    if (event->type() == QEvent::LanguageChange)
        retranslate();
    QWidget::changeEvent(event);
}
