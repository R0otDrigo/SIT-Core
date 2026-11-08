#pragma once
#include <string>
#include <vector>
#include "model/Network.h"

namespace sit {

enum class DispatchAction { Dispatch, Retire };

struct DispatchOrder {
    DispatchAction action;
    std::string routeId;
    VehicleType type;       // solo se usa con Dispatch
};

// Patron Strategy: la politica de despacho se puede cambiar en caliento.
// Demo 1 trae solo la politica manual; la automatica llega en la Demo 2.
class IDispatchStrategy {
public:
    virtual ~IDispatchStrategy() = default;
    virtual std::string name() const = 0;
    virtual std::vector<DispatchOrder> decide(const Network& network) = 0;
};

// El controlador de flota decide todo a mano desde la interfaz.
class ManualDispatch : public IDispatchStrategy {
public:
    std::string name() const override { return "Manual"; }
    std::vector<DispatchOrder> decide(const Network&) override { return {}; }
};

} // namespace sit
