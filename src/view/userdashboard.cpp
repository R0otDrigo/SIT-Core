#include "userdashboard.h"
#include <QVBoxLayout>

#include "common/Exceptions.h"
#include "controller/SimulationController.h"
#include "controller/UnitRegistry.h"
#include "view/UserBroadcast.h"
#include "view/UserWidget.h"

UserDashboard::UserDashboard(QWidget *parent) : Dashboard(parent) {
    retranslate();
}

void UserDashboard::setBroadcast(UserBroadcast *broadcast)
{
    broadcast_ = broadcast;
    if (userWidget_) userWidget_->setBroadcast(broadcast_);
}

void UserDashboard::setUnitRegistry(UnitRegistry *registry)
{
    unitRegistry_ = registry ? registry : &UnitRegistry::instance();
}

void UserDashboard::resetSession()
{
    delete userWidget_;
    userWidget_ = nullptr;
}

void UserDashboard::initializeSession(const QString &userId, const QString &idToken)
{
    if (unitRegistry_ && !idToken.isEmpty())
        unitRegistry_->configure(unitRegistry_->projectId(), idToken);
    if (userWidget_) return;

    try {
        userWidget_ = new UserWidget(
            SimulationController::loadNetwork(SimulationController::defaultNetworkPath()), userId,
            this);
        userWidget_->setBroadcast(broadcast_);
        userWidget_->setUnitRegistry(unitRegistry_);
        contentLayout()->addWidget(userWidget_, 1);
    } catch (const sit::SitException &e) {
        showError(tr("No se pudo cargar la red:\n%1").arg(e.what()));
    }
}

QString UserDashboard::dashboardTitle() const { return tr("User Dashboard"); }

QString UserDashboard::welcomeMessage() const { return tr("Bienvenido Usuario"); }
