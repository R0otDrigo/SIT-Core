#include "viewmodel/SimulationView.h"

#include <algorithm>

#include <QVariantMap>

#include "model/TrunkBus.h"
#include "utils/Constants.h"

namespace teoto {

SimulationView::SimulationView(QObject *parent)
    : QObject(parent)
{
    m_timer.setInterval(constants::TickIntervalMs);
    connect(&m_timer, &QTimer::timeout, this, &SimulationView::tick);
    buildScenario();
}

void SimulationView::buildScenario()
{
    m_route = std::make_shared<Route>(QStringLiteral("R1"), QStringLiteral("Ruta 1 - Centro"));

    // Paraderos de referencia de Arequipa (ver requerimientos §9.1).
    m_route->addStop(Stop(QStringLiteral("S1"), QStringLiteral("Terminal Terrestre"),
                          QGeoCoordinate(-16.4290, -71.5210), 8));
    m_route->addStop(Stop(QStringLiteral("S2"), QStringLiteral("Mercado San Camilo"),
                          QGeoCoordinate(-16.4016, -71.5350), 15));
    m_route->addStop(Stop(QStringLiteral("S3"), QStringLiteral("Plaza de Armas"),
                          QGeoCoordinate(-16.3989, -71.5369), 22));
    m_route->addStop(Stop(QStringLiteral("S4"), QStringLiteral("Puente Grau"),
                          QGeoCoordinate(-16.3959, -71.5390), 6));

    m_fleet = Fleet();
    m_fleet.add(std::make_unique<TrunkBus>(QStringLiteral("BUS-01"), m_route));

    m_bus = dynamic_cast<Bus *>(m_fleet.at(0));
    m_lastStopIndex = 0;
    m_simTimeSeconds = 0.0;
    m_lastEvent = QStringLiteral("listo");
}

QGeoCoordinate SimulationView::busPosition() const
{
    if (m_bus)
        return m_bus->position();
    return QGeoCoordinate();
}

QVariantList SimulationView::routePath() const
{
    QVariantList path;
    if (!m_route)
        return path;
    for (const Stop &stop : m_route->stops())
        path.append(QVariant::fromValue(stop.position()));
    return path;
}

QVariantList SimulationView::stops() const
{
    QVariantList list;
    if (!m_route)
        return list;
    for (const Stop &stop : m_route->stops()) {
        QVariantMap item;
        item[QStringLiteral("id")] = stop.id();
        item[QStringLiteral("name")] = stop.name();
        item[QStringLiteral("lat")] = stop.position().latitude();
        item[QStringLiteral("lon")] = stop.position().longitude();
        item[QStringLiteral("waiting")] = stop.waitingPassengers();
        list.append(item);
    }
    return list;
}

QString SimulationView::statusText() const
{
    const QString state = m_running ? QStringLiteral("en marcha") : QStringLiteral("detenida");
    const QString mode = (m_mapMode == QStringLiteral("online"))
                             ? QStringLiteral("mapa en linea")
                             : QStringLiteral("mapa esquematico");
    return QStringLiteral("t = %1 s  |  simulacion %2  |  velocidad %3x  |  %4  |  %5")
        .arg(m_simTimeSeconds, 0, 'f', 1)
        .arg(state)
        .arg(m_speedMultiplier, 0, 'f', 1)
        .arg(mode)
        .arg(m_lastEvent);
}

void SimulationView::setSpeedMultiplier(double factor)
{
    if (factor <= 0.0 || qFuzzyCompare(factor, m_speedMultiplier))
        return;
    m_speedMultiplier = factor;
    emit speedChanged();
    emit stateChanged();
}

void SimulationView::setMapMode(const QString &mode)
{
    if (mode == m_mapMode)
        return;
    m_mapMode = mode;
    emit mapModeChanged();
    emit stateChanged();
}

void SimulationView::start()
{
    if (m_running)
        return;
    m_running = true;
    m_timer.start();
    emit runningChanged();
    emit stateChanged();
}

void SimulationView::pause()
{
    if (!m_running)
        return;
    m_running = false;
    m_timer.stop();
    emit runningChanged();
    emit stateChanged();
}

void SimulationView::toggleRunning()
{
    if (m_running)
        pause();
    else
        start();
}

void SimulationView::reset()
{
    m_timer.stop();
    m_running = false;
    buildScenario();
    emit runningChanged();
    emit stateChanged();
}

void SimulationView::tick()
{
    const double dt = (constants::TickIntervalMs / 1000.0) * m_speedMultiplier;
    m_simTimeSeconds += dt;

    m_fleet.updateAll(dt);

    if (m_bus && m_bus->currentStopIndex() != m_lastStopIndex) {
        m_lastStopIndex = m_bus->currentStopIndex();
        dispatchBoarding(*m_bus, m_lastStopIndex);
    }

    emit stateChanged();
}

void SimulationView::dispatchBoarding(Bus &bus, std::size_t stopIndex)
{
    if (!m_route)
        return;

    Stop *stop = m_route->mutableStopAt(stopIndex);
    if (!stop)
        return;

    const int leaving = bus.disembark(bus.occupancy() / 2);

    const int canBoard = std::min(stop->waitingPassengers(), bus.freeSeats());
    const int boarded = stop->takeWaiting(canBoard);
    for (int i = 0; i < boarded; ++i)
        ++bus; // sobrecarga de operador (OBJ-09)

    if (stop->waitingPassengers() > 0 && bus.freeSeats() == 0) {
        m_lastEvent = QStringLiteral("bus lleno en %1").arg(stop->name());
    } else {
        m_lastEvent = QStringLiteral("%1: +%2 / -%3").arg(stop->name()).arg(boarded).arg(leaving);
    }

    // Reponer algo de demanda para mantener la demostracion activa.
    stop->addWaiting(2);
}

} // namespace teoto
