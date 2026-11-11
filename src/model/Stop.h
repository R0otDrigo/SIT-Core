#pragma once

#include <QGeoCoordinate>
#include <QString>

namespace teoto {

/// Paradero de una ruta. Mantiene la cantidad de pasajeros en espera de forma
/// agregada (ver requerimientos RF-07 y RF-09).
class Stop
{
public:
    Stop() = default;
    Stop(QString id, QString name, QGeoCoordinate position, int waitingPassengers = 0);

    const QString &id() const { return m_id; }
    const QString &name() const { return m_name; }
    const QGeoCoordinate &position() const { return m_position; }
    int waitingPassengers() const { return m_waitingPassengers; }

    /// Incrementa la demanda en espera (nunca negativa).
    void addWaiting(int passengers);

    /// Extrae hasta \a maxPassengers pasajeros en espera y devuelve cuantos se
    /// pudieron tomar realmente.
    int takeWaiting(int maxPassengers);

private:
    QString m_id;
    QString m_name;
    QGeoCoordinate m_position;
    int m_waitingPassengers{0};
};

} // namespace teoto
