#include "userdashboard.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QLabel>
#include <QVBoxLayout>

#include "common/Exceptions.h"
#include "controller/SimulationController.h"
#include "controller/UnitRegistry.h"
#include "view/UserBroadcast.h"
#include "view/UserWidget.h"

#ifndef SIT_DATA_DIR
#define SIT_DATA_DIR "data"
#endif

UserDashboard::UserDashboard(QWidget *parent) : Dashboard(parent) {
    retranslate();
}

UserDashboard::~UserDashboard() = default;

void UserDashboard::initializeSession(const QString &userId, const QString &idToken)
{
    if (unitRegistry_ && !idToken.isEmpty())
        unitRegistry_->configure(unitRegistry_->projectId(), idToken);
    if (userWidget_) return;
    auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
    if (!layout) return;

    QString path = QDir(QCoreApplication::applicationDirPath()).filePath("data/network.csv");
    if (!QFile::exists(path)) path = QString(SIT_DATA_DIR) + "/network.csv";

    try {
        userWidget_ = new UserWidget(SimulationController::loadNetwork(path), userId, this);
        userWidget_->setBroadcast(broadcast_);
        userWidget_->setUnitRegistry(unitRegistry_);
        layout->addWidget(userWidget_, 1);
    } catch (const sit::SitException &e) {
        auto *error = new QLabel(tr("No se pudo cargar la red:\n%1").arg(e.what()), this);
        error->setWordWrap(true);
        contentLayout()->addWidget(error);
    }
}

void UserDashboard::setBroadcast(UserBroadcast *broadcast)
{
    broadcast_ = broadcast;
    if (userWidget_) userWidget_->setBroadcast(broadcast_);
}

void UserDashboard::setUnitRegistry(UnitRegistry *registry)
{
    unitRegistry_ = registry;
}

void UserDashboard::resetSession()
{
    delete userWidget_;
    userWidget_ = nullptr;
}

QString UserDashboard::welcomeMessage() const { return tr("Bienvenido Usuario"); }
