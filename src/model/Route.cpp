#include "model/Route.h"

namespace teoto {

Route::Route(QString id, QString name)
    : m_id(std::move(id))
    , m_name(std::move(name))
{
}

void Route::addStop(const Stop &stop)
{
    m_stops.push_back(stop);
}

const Stop *Route::stopAt(std::size_t index) const
{
    if (index >= m_stops.size())
        return nullptr;
    return &m_stops[index];
}

Stop *Route::mutableStopAt(std::size_t index)
{
    if (index >= m_stops.size())
        return nullptr;
    return &m_stops[index];
}

std::optional<std::size_t> Route::indexOfStop(const QString &stopId) const
{
    for (std::size_t i = 0; i < m_stops.size(); ++i) {
        if (m_stops[i].id() == stopId)
            return i;
    }
    return std::nullopt;
}

double Route::segmentLength(std::size_t fromIndex) const
{
    if (m_stops.size() < 2 || fromIndex >= m_stops.size())
        return 0.0;

    const std::size_t nextIndex = (fromIndex + 1) % m_stops.size();
    return m_stops[fromIndex].position().distanceTo(m_stops[nextIndex].position());
}

double Route::totalLength() const
{
    double total = 0.0;
    for (std::size_t i = 0; i < m_stops.size(); ++i)
        total += segmentLength(i);
    return total;
}

} // namespace teoto
