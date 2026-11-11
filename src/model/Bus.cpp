#include "model/Bus.h"

#include <algorithm>

namespace teoto {

Bus::Bus(QString id, int capacity, double maxSpeed, std::shared_ptr<Route> route)
    : Vehicle(std::move(id), capacity, maxSpeed)
    , m_route(std::move(route))
    , m_effectiveSpeed(maxSpeed)
{
    if (m_route && m_route->stopAt(0))
        m_position = m_route->stopAt(0)->position();
}

VehicleType Bus::type() const
{
    return VehicleType::TrunkBus;
}

double Bus::speed() const
{
    return m_effectiveSpeed;
}

void Bus::update(double deltaTime)
{
    if (deltaTime <= 0.0)
        return;
    advanceAlongRoute(deltaTime);
}

bool Bus::boardPassenger()
{
    if (freeSeats() <= 0)
        return false;
    ++m_occupancy;
    return true;
}

int Bus::disembark(int count)
{
    if (count <= 0)
        return 0;
    const int leaving = std::min(count, m_occupancy);
    m_occupancy -= leaving;
    return leaving;
}

Bus &Bus::operator++()
{
    boardPassenger();
    return *this;
}

const Stop *Bus::currentStop() const
{
    if (!m_route)
        return nullptr;
    return m_route->stopAt(m_currentStopIndex);
}

void Bus::advanceAlongRoute(double deltaTime)
{
    if (!m_route || m_route->stopCount() < 2)
        return;

    // Velocidad efectiva en km/h -> metros por segundo.
    const double metersPerSecond = m_effectiveSpeed * 1000.0 / 3600.0;
    m_distanceOnSegment += metersPerSecond * deltaTime;

    double segment = m_route->segmentLength(m_currentStopIndex);
    while (segment > 0.0 && m_distanceOnSegment >= segment) {
        m_distanceOnSegment -= segment;
        m_currentStopIndex = (m_currentStopIndex + 1) % m_route->stopCount();
        segment = m_route->segmentLength(m_currentStopIndex);
    }

    const Stop *from = m_route->stopAt(m_currentStopIndex);
    const Stop *to = m_route->stopAt((m_currentStopIndex + 1) % m_route->stopCount());
    if (!from || !to)
        return;

    const double bearing = from->position().azimuthTo(to->position());
    m_position = from->position().atDistanceAndAzimuth(m_distanceOnSegment, bearing);
}

} // namespace teoto
