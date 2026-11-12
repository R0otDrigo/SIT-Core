#pragma once
#include <QObject>
#include <QString>
#include <memory>
#include "model/Network.h"
#include "model/SimulationEngine.h"
#include "model/Snapshot.h"

Q_DECLARE_METATYPE(sit::Snapshot)

// Controlador (MVC): traduce las acciones de la vista a llamadas al modelo y los cambios del
// modelo (que ocurren en otro hilo) a senales Qt. No contiene logica de simulacion.
class SimulationController : public QObject {
    Q_OBJECT
public:
    explicit SimulationController(std::unique_ptr<sit::Network> network, QObject* parent = nullptr);
    ~SimulationController() override;

    // Carga un archivo .csv usando el Adapter y arma la red con el Builder.
    // Lanza sit::SitException si el archivo es invalido.
    static std::unique_ptr<sit::Network> loadNetwork(const QString& path);

public slots:
    void start();
    void setPaused(bool paused);
    void setSpeed(double speed);
    void dispatchBus(const QString& routeId, int vehicleType);   // vehicleType = sit::VehicleType
    void retireBus(const QString& routeId);
    void triggerEvent(int eventType, const QString& routeId);    // eventType = sit::EventType

signals:
    void snapshotReady(const sit::Snapshot& snapshot);
    void errorOccurred(const QString& message);

private:
    sit::SimulationEngine engine_;
};
