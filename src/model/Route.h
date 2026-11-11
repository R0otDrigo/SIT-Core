#pragma once

#include <cstddef>
#include <optional>
#include <vector>

#include <QString>

#include "model/Stop.h"

namespace teoto {

/// Ruta compuesta por una secuencia ordenada de paraderos (RF-05, RF-06).
class Route
{
public:
    Route(QString id, QString name);

    const QString &id() const { return m_id; }
    const QString &name() const { return m_name; }

    void addStop(const Stop &stop);

    std::size_t stopCount() const { return m_stops.size(); }
    const std::vector<Stop> &stops() const { return m_stops; }

    /// Devuelve el paradero en \a index, o nullptr si el indice es invalido.
    const Stop *stopAt(std::size_t index) const;

    /// Igual que stopAt() pero permite modificar el paradero (p. ej. su demanda).
    Stop *mutableStopAt(std::size_t index);

    std::optional<std::size_t> indexOfStop(const QString &stopId) const;

    /// Longitud del segmento que empieza en \a fromIndex (0 si es invalido).
    double segmentLength(std::size_t fromIndex) const;

    /// Longitud total de la ruta en metros, incluido el cierre al primer paradero.
    double totalLength() const;

private:
    QString m_id;
    QString m_name;
    std::vector<Stop> m_stops;
};

} // namespace teoto
