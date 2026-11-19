#include <QApplication>
#include <QQuickWidget>
#include <QTimer>
#include "check.h"
#include "controller/SimulationController.h"
#include "view/UserBridge.h"
#include "view/UserBroadcast.h"
#include "view/UserInfoDialog.h"
#include "view/UserWidget.h"
#include "view/admindashboard.h"
#include "view/userdashboard.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    auto net = SimulationController::loadNetwork(QString(SIT_DATA_DIR) + "/network.csv");
    const sit::Snapshot initial = net->snapshot();

    UserBridge bridge;
    bridge.setUser("uid-demo-123");
    CHECK(!bridge.hasVehicle());
    CHECK(bridge.vehicleModel()->rowCount() == 0);
    bridge.update(initial);
    CHECK(bridge.hasVehicle());
    CHECK(bridge.vehicleModel()->rowCount() == 1);
    const QString bus = bridge.vehicleId();
    CHECK(!bus.isEmpty());
    CHECK(bridge.placa().startsWith("SIT-"));
    CHECK(!bridge.marca().isEmpty());
    CHECK(!bridge.antecedentes().isEmpty());
    CHECK(!bridge.evento().isEmpty());

    UserBridge sameUser;
    sameUser.setUser("uid-demo-123");
    sameUser.update(initial);
    CHECK(sameUser.vehicleId() == bus);

    UserBridge otherUser;
    otherUser.setUser("otro-uid");
    otherUser.update(initial);
    CHECK(!otherUser.vehicleId().isEmpty());

    UserBroadcast broadcast;
    AdminDashboard admin;
    CHECK(admin.simulation() == nullptr);
    admin.setBroadcast(&broadcast);
    admin.initialize();
    CHECK(admin.simulation() != nullptr);
    admin.initialize();
    CHECK(admin.simulation() != nullptr);

    UserDashboard user;
    CHECK(user.userWidget() == nullptr);
    user.setBroadcast(&broadcast);
    user.initializeSession("uid-demo-123");
    CHECK(user.userWidget() != nullptr);
    UserBridge* userBridge = user.userWidget()->bridge();
    CHECK(userBridge->userId() == "uid-demo-123");

    admin.sendMessages("Sin antecedentes penales", "Reunion con el administrador");
    CHECK(broadcast.antecedentes() == "Sin antecedentes penales");
    CHECK(userBridge->antecedentes() == "Sin antecedentes penales");
    CHECK(userBridge->evento() == "Reunion con el administrador");
    broadcast.clear();
    CHECK(!userBridge->antecedentes().isEmpty());
    CHECK(!userBridge->evento().isEmpty());

    UserInfoDialog infoDialog(UserInfoDialog::Mode::MoreInfo, userBridge);
    UserInfoDialog callDialog(UserInfoDialog::Mode::Call, userBridge);
    UserInfoDialog vehicleDialog(UserInfoDialog::Mode::VehicleStatus, userBridge);
    infoDialog.show();
    callDialog.show();
    vehicleDialog.show();

    QTimer::singleShot(600, &app, &QApplication::quit);
    app.exec();

    CHECK(user.userWidget()->mapWidget() != nullptr);
    CHECK(user.userWidget()->mapWidget()->status() == QQuickWidget::Ready);
    CHECK(user.userWidget()->mapWidget()->errors().isEmpty());
    CHECK(userBridge->hasVehicle());

    for (QQuickWidget* view : {infoDialog.view(), callDialog.view(), vehicleDialog.view()}) {
        CHECK(view != nullptr);
        CHECK(view->status() == QQuickWidget::Ready);
        CHECK(view->errors().isEmpty());
    }

    user.resetSession();
    CHECK(user.userWidget() == nullptr);
    admin.resetSession();
    CHECK(admin.simulation() == nullptr);
    return 0;
}
