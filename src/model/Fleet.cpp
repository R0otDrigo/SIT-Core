#include "model/Fleet.h"

namespace teoto {

void Fleet::add(std::unique_ptr<Vehicle> vehicle)
{
    if (vehicle)
        m_vehicles.push_back(std::move(vehicle));
}

Vehicle *Fleet::at(std::size_t index) const
{
    if (index >= m_vehicles.size())
        return nullptr;
    return m_vehicles[index].get();
}

void Fleet::updateAll(double deltaTime)
{
    for (const auto &vehicle : m_vehicles)
        vehicle->update(deltaTime);
}

} // namespace teoto
