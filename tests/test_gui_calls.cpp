#include <QCoreApplication>
#include <QEventLoop>
#include <QTimer>
#include "check.h"
#include "controller/CallSignaling.h"

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
    return 0;
}
