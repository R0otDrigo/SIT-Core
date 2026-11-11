#pragma once

#include <cstddef>
#include <memory>

#include <QGeoCoordinate>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QVariantList>

#include "model/Bus.h"
#include "model/Fleet.h"
#include "model/Route.h"

namespace teoto {

/// Puente entre el modelo y la vista QML (rol ViewModel, requerimientos §8).
/// Expone el estado de la simulacion como propiedades reactivas y modelos
/// simples para el mapa y los controles.
class SimulationView : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QGeoCoordinate busPosition READ busPosition NOTIFY stateChanged)
    Q_PROPERTY(QVariantList routePath READ routePath CONSTANT)
    Q_PROPERTY(QVariantList stops READ stops CONSTANT)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(double speedMultiplier READ speedMultiplier WRITE setSpeedMultiplier NOTIFY speedChanged)
    Q_PROPERTY(QString mapMode READ mapMode WRITE setMapMode NOTIFY mapModeChanged)
    Q_PROPERTY(QString statusText READ statusText NOTIFY stateChanged)

public:
    explicit SimulationView(QObject *parent = nullptr);

    QGeoCoordinate busPosition() const;
    QVariantList routePath() const;
    QVariantList stops() const;
    bool running() const { return m_running; }
    double speedMultiplier() const { return m_speedMultiplier; }
    QString mapMode() const { return m_mapMode; }
    QString statusText() const;

    void setSpeedMultiplier(double factor);
    void setMapMode(const QString &mode);

public slots:
    void start();
    void pause();
    void toggleRunning();
    void reset();

signals:
    void stateChanged();
    void runningChanged();
    void speedChanged();
    void mapModeChanged();

private slots:
    void tick();

private:
    void buildScenario();
    void dispatchBoarding(Bus &bus, std::size_t stopIndex);

    QTimer m_timer;
    Fleet m_fleet;
    std::shared_ptr<Route> m_route;
    Bus *m_bus{nullptr};
    std::size_t m_lastStopIndex{0};
    double m_simTimeSeconds{0.0};
    bool m_running{false};
    double m_speedMultiplier{1.0};
    QString m_mapMode{QStringLiteral("online")};
    QString m_lastEvent;
};

} // namespace teoto
