#include <sstream>
#include "check.h"
#include "common/Exceptions.h"
#include "model/CsvRouteAdapter.h"
#include "model/NetworkBuilder.h"

using namespace sit;

int main() {
    // Archivo real del proyecto.
    CsvRouteAdapter csv(std::string(SIT_DATA_DIR) + "/network.csv");
    NetworkData data = csv.load();
    CHECK(data.stops.size() == 15);
    CHECK(data.routes.size() == 4);
    CHECK(data.fleet.size() == 6);
    CHECK(data.fleetLimit == 10);

    // Builder arma la red completa.
    auto net = NetworkBuilder().withSeed(7).withData(data).build();
    CHECK(net->activeVehicles() == 6);
    CHECK(net->routes().size() == 4);
    CHECK(net->routes().byId("T1")->size() == 6);
    CHECK(net->stops().byId("PLA")->typeName() == "transfer");
    // Plaza de Armas es compartida por T1 y T2 (mismo objeto).
    CHECK(net->routes().byId("T1")->stops()[2] == net->routes().byId("T2")->stops()[2]);

    // Entradas invalidas -> excepciones propias.
    CHECK_THROWS(CsvRouteAdapter("no_existe.csv").load(), DataLoadException);
    std::istringstream badNumber("STOP,A,A,regular,xx,0,10\n");
    CHECK_THROWS(CsvRouteAdapter::parse(badNumber), DataLoadException);
    std::istringstream badRecord("HOLA,1,2\n");
    CHECK_THROWS(CsvRouteAdapter::parse(badRecord), DataLoadException);

    NetworkData bad = data;
    bad.routes[0].stops.push_back("XXX");
    CHECK_THROWS(NetworkBuilder().withData(bad).build(), InvalidRouteException);
    bad = data;
    bad.stops.push_back(bad.stops[0]);
    CHECK_THROWS(NetworkBuilder().withData(bad).build(), DataLoadException);
    bad = data;
    bad.routes[0].stops.resize(1);
    CHECK_THROWS(NetworkBuilder().withData(bad).build(), InvalidRouteException);
    return 0;
}
