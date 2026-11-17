#include <QCoreApplication>
#include <QEventLoop>
#include <QTimer>
#include "check.h"
#include "controller/CallSignaling.h"
#include "controller/UnitRegistry.h"
#include "controller/firebaseauthmanager.h"

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);

    CallSignaling admin(CallSignaling::Role::Admin);
    CallSignaling user(CallSignaling::Role::User);
    CHECK(admin.start());
    CHECK(user.start());

    bool received = false;
    CallInfo got;
    QEventLoop loop;
    QObject::connect(&admin, &CallSignaling::messageReceived, &loop,
                     [&](const CallInfo& info) {
                         got = info;
                         received = true;
                         loop.quit();
                     });
    QTimer::singleShot(2000, &loop, &QEventLoop::quit);

    user.sendCallTo(QHostAddress(QHostAddress::LocalHost), QStringLiteral("uid-1"),
                    QStringLiteral("7"), QStringLiteral("Usuario"),
                    QStringLiteral("Llamada entrante"));
    loop.exec();

    CHECK(received);
    CHECK(got.type == QStringLiteral("call"));
    CHECK(got.from == QStringLiteral("user"));
    CHECK(got.uid == QStringLiteral("uid-1"));
    CHECK(got.vehicleId == QStringLiteral("7"));
    CHECK(got.status == QStringLiteral("Llamada entrante"));

    UnitRegistry registry;
    sit::VehicleView vehicle;
    vehicle.id = 7;
    vehicle.x = 12.34;
    vehicle.y = -56.78;
    registry.syncVehicle(vehicle, QStringLiteral("uid-1"));
    CHECK(registry.vehicle().id == 7);
    CHECK_NEAR(registry.vehicle().x, 12.34, 1e-9);
    CHECK_NEAR(registry.vehicle().y, -56.78, 1e-9);

    // Test Singleton usage with &instance
    UnitRegistry &inst = UnitRegistry::instance();
    CHECK(&inst == &UnitRegistry::instance());
    inst.syncVehicle(vehicle, QStringLiteral("uid-singleton"));
    CHECK(inst.vehicle().id == 7);
    CHECK_NEAR(inst.vehicle().x, 12.34, 1e-9);
    CHECK_NEAR(inst.vehicle().y, -56.78, 1e-9);
    CHECK(inst.findUnit(QStringLiteral("7")) != nullptr);
    CHECK(inst.findUnit(QStringLiteral("7"))->driverUid == QStringLiteral("uid-singleton"));

    FirebaseAuthManager &auth = FirebaseAuthManager::instance();
    CHECK(&auth == &FirebaseAuthManager::instance());
    auth.setApiKey(QStringLiteral("test-key-123"));
    auth.setProjectId(QStringLiteral("test-proj-456"));
    CHECK(auth.apiKey() == QStringLiteral("test-key-123"));
    CHECK(auth.projectId() == QStringLiteral("test-proj-456"));

    return 0;
}
