#pragma once

#include <utility>

#include <QString>

namespace teoto {

/// Tipos de vehiculo soportados por la fabrica inicial (ver requerimientos §10.1).
enum class VehicleType {
    TrunkBus,
    FeederBus,
};

/// Clase base abstracta de todo vehiculo de la flota (OBJ-05).
class Vehicle
{
public:
    Vehicle(QString id, int capacity, double maxSpeed)
        : m_id(std::move(id))
        , m_capacity(capacity)
        , m_maxSpeed(maxSpeed)
    {
    }

    virtual ~Vehicle() = default;

    virtual VehicleType type() const = 0;

    /// Velocidad efectiva actual en km/h.
    virtual double speed() const = 0;

    virtual int capacity() const { return m_capacity; }

    /// Avanza el estado del vehiculo \a deltaTime segundos de simulacion.
    virtual void update(double deltaTime) = 0;

    const QString &id() const { return m_id; }
    double maxSpeed() const { return m_maxSpeed; }

protected:
    QString m_id;
    int m_capacity;
    double m_maxSpeed;
};

} // namespace teoto
