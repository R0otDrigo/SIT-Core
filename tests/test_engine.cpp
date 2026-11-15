#include <atomic>
#include <chrono>
#include <thread>
#include "check.h"
#include "common/Exceptions.h"
#include "model/CsvRouteAdapter.h"
#include "model/NetworkBuilder.h"
#include "model/SimulationEngine.h"

using namespace sit;

static std::unique_ptr<Network> loadNet() {
    CsvRouteAdapter csv(std::string(SIT_DATA_DIR) + "/network.csv");
    return NetworkBuilder().withSeed(3).withData(csv.load()).build();
}

int main() {
    using namespace std::chrono_literals;

    // --- Determinista, sin hilos ---
    {
        SimulationEngine engine(loadNet());
        engine.stepOnce(60.0);
        auto snap = engine.snapshot();
        CHECK_NEAR(snap.simTime, 60.0, 1e-6);
        CHECK(snap.strategy == "Manual");
        engine.dispatch("T1", VehicleType::Conventional);
        CHECK(engine.snapshot().stats.activeVehicles == 7);
        CHECK_THROWS(engine.dispatch("NOPE", VehicleType::Conventional), InvalidRouteException);
        CHECK_THROWS(engine.dispatch("A1", VehicleType::Articulated), InvalidRouteException);
    }

    // --- Con hilos: el tiempo avanza, el observador recibe snapshots, stop() es limpio ---
    {
        SimulationEngine engine(loadNet());
        std::atomic<int> received{0};
        engine.setObserver([&](const Snapshot&) { ++received; });
        engine.setSpeed(10.0);
        engine.start();
        engine.start();                                    // idempotente
        CHECK(engine.running());

        std::this_thread::sleep_for(500ms);
        auto s1 = engine.snapshot();
        CHECK(s1.simTime > 1.0);                           // ~ 0.5 s * 10x
        CHECK(received > 3);

        // Acciones concurrentes desde otro hilo mientras la simulacion corre.
        std::thread other([&] {
            for (int i = 0; i < 20; ++i) { engine.snapshot(); engine.setSpeed(5.0 + i); std::this_thread::sleep_for(5ms); }
        });
        engine.triggerEvent(EventType::Accident, "T1");
        other.join();

        // Pausa: el reloj se congela.
        engine.setPaused(true);
        std::this_thread::sleep_for(150ms);
        const double frozen = engine.snapshot().simTime;
        std::this_thread::sleep_for(250ms);
        CHECK_NEAR(engine.snapshot().simTime, frozen, 1e-9);
        engine.setPaused(false);

        engine.stop();
        engine.stop();                                     // idempotente
        CHECK(!engine.running());
        const int after = received;
        std::this_thread::sleep_for(150ms);
        CHECK(received == after);                          // ya no hay notificaciones
    }

    // --- Strategy: el despacho manual nunca decide nada por su cuenta ---
    {
        auto net = loadNet();
        ManualDispatch manual;
        CHECK(manual.name() == "Manual");
        CHECK(manual.decide(*net).empty());
    }
    return 0;
}
