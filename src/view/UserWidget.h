#pragma once
#include <QWidget>
#include <memory>

#include "controller/SimulationController.h"

class QLabel;
class QQuickWidget;
class UserBridge;
class UserBroadcast;

// Vista de solo lectura para el rol "user": carga user.qml con un contexto propio y nunca
// expone el MapBridge ni los controles del simulador (eso es exclusivo del administrador).
class UserWidget : public QWidget {
    Q_OBJECT
public:
    explicit UserWidget(std::unique_ptr<sit::Network> network, const QString& userId,
                        QWidget* parent = nullptr);

    void setBroadcast(UserBroadcast* broadcast);

    SimulationController* controller() const { return controller_; }
    UserBridge* bridge() const { return bridge_; }
    QQuickWidget* mapWidget() const { return map_; }

private slots:
    void onSnapshot(const sit::Snapshot& snapshot);
    void onError(const QString& message);

private:
    void buildUi();

    SimulationController* controller_{};
    UserBridge* bridge_{};
    QQuickWidget* map_{};
    QLabel* errorLabel_{};
};
