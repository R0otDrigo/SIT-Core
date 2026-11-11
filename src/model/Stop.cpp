#include "model/Stop.h"

#include <algorithm>

namespace teoto {

Stop::Stop(QString id, QString name, QGeoCoordinate position, int waitingPassengers)
    : m_id(std::move(id))
    , m_name(std::move(name))
    , m_position(std::move(position))
    , m_waitingPassengers(std::max(0, waitingPassengers))
{
}

void Stop::addWaiting(int passengers)
{
    if (passengers > 0)
        m_waitingPassengers += passengers;
}

int Stop::takeWaiting(int maxPassengers)
{
    if (maxPassengers <= 0)
        return 0;

    const int taken = std::min(maxPassengers, m_waitingPassengers);
    m_waitingPassengers -= taken;
    return taken;
}

} // namespace teoto
