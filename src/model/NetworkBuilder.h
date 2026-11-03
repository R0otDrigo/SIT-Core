#pragma once
#include <memory>
#include "model/Network.h"
#include "model/NetworkData.h"

namespace sit {

// Patron creacional Builder: arma una Network valida paso a paso a partir de NetworkData.
//   auto net = NetworkBuilder().withSeed(7).withData(data).build();
class NetworkBuilder {
public:
    NetworkBuilder& withSeed(unsigned seed) { seed_ = seed; return *this; }
    NetworkBuilder& withData(NetworkData data) { data_ = std::move(data); return *this; }

    // Valida y construye. Lanza DataLoadException / InvalidRouteException si hay inconsistencias.
    std::unique_ptr<Network> build() const;

private:
    unsigned seed_{42};
    NetworkData data_;
};

} // namespace sit
