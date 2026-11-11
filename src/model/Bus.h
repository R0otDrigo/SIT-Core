#pragma once

#include <cstddef>
#include <memory>

#include <QGeoCoordinate>

#include "model/Route.h"
#include "model/Vehicle.h"

namespace teoto {

/// Bus que recorre una ruta. Implementa el movimiento sobre la ruta y la
/// gestion de ocupacion, incluida la sobrecarga de operador (RF-14, RF-15).
class Bus : public Vehicle
{
public:
    Bus(QString id, int capacity, double maxSpeed, std::shared_ptr<Route> route);

    VehicleType type() const override;
    double speed() const override;
    void update(double deltaTime) override;

    /// Embarca un pasajero. Devuelve false si el bus esta lleno (RF-16).
    bool boardPassenger();

    /// Desembarca \a count pasajeros. Devuelve cuantos bajaron realmente.
    int disembark(int count);

    /// Equivalente a boardPassenger() pero como operador (OBJ-09).
    Bus &operator++();

    int occupancy() const { return m_occupancy; }
    int freeSeats() const { return m_capacity - m_occupancy; }

    const QGeoCoordinate &position() const { return m_position; }
    std::size_t currentStopIndex() const { return m_currentStopIndex; }
    const Stop *currentStop() const;
    const std::shared_ptr<Route> &route() const { return m_route; }

protected:
    /// Permite a los decoradores o al controlador de trafico ajustar la
    /// velocidad efectiva (usado en P4).
    void setEffectiveSpeed(double speed) { m_effectiveSpeed = speed; }

private:
    void advanceAlongRoute(double deltaTime);

    std::shared_ptr<Route> m_route;
    QGeoCoordinate m_position;
    std::size_t m_currentStopIndex{0};
    double m_distanceOnSegment{0.0};
    double m_effectiveSpeed{0.0};
    int m_occupancy{0};
};

} // namespace teoto
