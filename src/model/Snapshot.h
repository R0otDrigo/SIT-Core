#pragma once
#include <string>
#include <vector>

namespace sit {

// Fotografia inmutable del estado de la simulacion. Es el "contrato" entre el modelo (que vive
// en otro hilo) y la vista: son datos simples copiables, sin punteros al modelo, por lo que
// pasarlos entre hilos es seguro.
struct VehicleView {
    int id{};
    std::string type;
    std::string routeId;
    double x{}, y{};
    int passengers{};
    int capacity{};
    bool dwelling{};
};

struct StopView {
    std::string id;
    std::string name;
    double x{}, y{};
    int waiting{};
    double saturation{};
    bool closed{};
};

struct RouteView {
    std::string id;
    std::string name;
    std::string color;
    bool trunk{};
    std::vector<std::string> stopIds;
};

struct EventView {
    std::string name;
    std::string routeId;
    std::string stopId;
    double remaining{};
};

struct Stats {
    long long transported{};     // pasajeros que llegaron a destino
    long long abandoned{};       // pasajeros que desistieron por paradero desbordado
    int activeVehicles{};
    int fleetLimit{};
    double avgSaturation{};
};

struct Snapshot {
    double simTime{};
    std::string clock;           // "HH:MM" hora simulada
    bool paused{};
    double speed{1.0};
    std::string strategy;
    std::vector<VehicleView> vehicles;
    std::vector<StopView> stops;
    std::vector<RouteView> routes;
    std::vector<EventView> events;
    Stats stats;
    std::vector<std::string> log;   // mensajes nuevos desde el snapshot anterior
};

} // namespace sit
