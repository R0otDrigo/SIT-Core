#include "view/UserWidget.h"
#include <QLabel>
#include <QQuickWidget>
#include <QQmlContext>
#include <QQmlError>
#include <QStringList>
#include <QUrl>
#include <QVBoxLayout>
#include "view/UserBridge.h"
#include "view/UserBroadcast.h"

UserWidget::UserWidget(std::unique_ptr<sit::Network> network, const QString& userId,
                       QWidget* parent)
    : QWidget(parent)
{
    controller_ = new SimulationController(std::move(network), this);
    bridge_ = new UserBridge(this);
    bridge_->setUser(userId);
    buildUi();
    connect(controller_, &SimulationController::snapshotReady, this, &UserWidget::onSnapshot);
    connect(controller_, &SimulationController::errorOccurred, this, &UserWidget::onError);
    controller_->start();
}

void UserWidget::setBroadcast(UserBroadcast* broadcast)
{
    bridge_->setBroadcast(broadcast);
}

void UserWidget::setUnitRegistry(UnitRegistry* registry)
{
    bridge_->setUnitRegistry(registry);
}

void UserWidget::buildUi()
{
    setWindowTitle(tr("Mi espacio de usuario"));
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);

    errorLabel_ = new QLabel(this);
    errorLabel_->setWordWrap(true);
    errorLabel_->setVisible(false);
    root->addWidget(errorLabel_);

    map_ = new QQuickWidget;
    map_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    map_->setClearColor(QColor("#e8eef5"));
    map_->setMinimumSize(480, 360);
    map_->rootContext()->setContextProperty("userBridge", bridge_);
    root->addWidget(map_, 1);

    connect(map_, &QQuickWidget::statusChanged, this, [this](QQuickWidget::Status status) {
        if (status != QQuickWidget::Error) return;
        QStringList messages;
        for (const QQmlError& error : map_->errors()) messages << error.toString();
        errorLabel_->setText(tr("Error en la vista de usuario: %1").arg(messages.join(" | ")));
        errorLabel_->setVisible(true);
    });
    map_->setSource(QUrl(QStringLiteral("qrc:/qml/user.qml")));
}

void UserWidget::onSnapshot(const sit::Snapshot& snapshot)
{
    bridge_->update(snapshot);
}

void UserWidget::onError(const QString& message)
{
    errorLabel_->setText(tr("Error: %1").arg(message));
    errorLabel_->setVisible(true);
}
