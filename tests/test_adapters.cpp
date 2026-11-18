#include <filesystem>
#include <fstream>
#include <sstream>
#include "check.h"
#include "common/Exceptions.h"
#include "model/CsvRouteAdapter.h"
#include "model/GtfsRouteAdapter.h"
#include "model/NetworkBuilder.h"

using namespace sit;

int main() {
    // Archivo real del proyecto.
    CsvRouteAdapter csv(std::string(SIT_DATA_DIR) + "/network.csv");
    NetworkData data = csv.load();
    CHECK(data.stops.size() == 288);
    CHECK(data.routes.size() == 9);
    CHECK(data.fleet.size() == 12);
    CHECK(data.fleetLimit == 20);
    CHECK(data.stops.front().geo.isValid());
    CHECK(data.stops.front().geo.latitude < -16.0 && data.stops.front().geo.latitude > -17.0);
    for (const auto& r : data.routes) {
        CHECK(r.gtfsShapeId == r.id);
        CHECK(r.path.size() >= 20);
        CHECK(!r.path.front().hasLocal);
        CHECK(r.path.front().geo.isValid());
    }

    auto net = NetworkBuilder().withSeed(7).withData(data).build();
    CHECK(net->activeVehicles() == 12);
    CHECK(net->routes().size() == 9);
    CHECK(net->routes().byId("17643070")->size() == 36);
    for (const auto& r : net->routes().all()) CHECK(r->hasPath());
    CHECK(net->snapshot().routes.front().path.size() >= 20);
    CHECK(net->stops().byId("10270858987")->typeName() == "transfer");
    CHECK(net->routes().byId("17688207")->stops().back() ==
          net->routes().byId("17688018")->stops().front());

    // Entradas invalidas -> excepciones propias.
    CHECK_THROWS(CsvRouteAdapter("no_existe.csv").load(), DataLoadException);
    std::istringstream badNumber("STOP,A,A,regular,xx,0,10\n");
    CHECK_THROWS(CsvRouteAdapter::parse(badNumber), DataLoadException);
    std::istringstream badRecord("HOLA,1,2\n");
    CHECK_THROWS(CsvRouteAdapter::parse(badRecord), DataLoadException);
    std::istringstream badGeo("STOP,A,A,regular,0,0,10,999,0\n");
    CHECK_THROWS(CsvRouteAdapter::parse(badGeo), DataLoadException);

    std::istringstream withPath("STOP,A,A,regular,0,0,10\n"
                                "STOP,B,B,regular,10,0,10\n"
                                "ROUTE,R,R,trunk,#f00,A;B,shape123\n"
                                "PATH,R,0,0\n"
                                "PATH,R,5,2\n"
                                "PATH,R,10,0,-16.4,-71.5\n"
                                "FLEET,R,convencional,0\n");
    const NetworkData parsed = CsvRouteAdapter::parse(withPath);
    CHECK(parsed.routes.size() == 1);
    CHECK(parsed.routes[0].gtfsShapeId == "shape123");
    CHECK(parsed.routes[0].path.size() == 3);
    CHECK_NEAR(parsed.routes[0].path[1].x, 5.0, 1e-9);
    CHECK_NEAR(parsed.routes[0].path[1].y, 2.0, 1e-9);
    CHECK(parsed.routes[0].path[2].geo.isValid());

    std::istringstream badPathRoute("PATH,ZZ,0,0\n");
    CHECK_THROWS(CsvRouteAdapter::parse(badPathRoute), DataLoadException);
    std::istringstream badPathFields("PATH,R,0\n");
    CHECK_THROWS(CsvRouteAdapter::parse(badPathFields), DataLoadException);

    {
        const GtfsDataset gtfs = GtfsRouteAdapter(std::string(SIT_DATA_DIR)).load();
        std::string shapeId;
        for (const auto& g : gtfs.routes)
            if (!g.shape.empty()) { shapeId = g.routeId; break; }
        CHECK(!shapeId.empty());

        const auto tempCsv = std::filesystem::temp_directory_path() / "sit_csv_gtfs_fallback.csv";
        {
            std::ofstream out(tempCsv);
            out << "LIMIT,2\n"
                << "STOP,A,A,regular,0,0,10\n"
                << "STOP,B,B,regular,10,0,10\n"
                << "ROUTE,R,R,trunk,#f00,A;B," << shapeId << "\n"
                << "FLEET,R,convencional,0\n";
        }
        const NetworkData fromGtfs =
            CsvRouteAdapter(tempCsv.string(), std::string(SIT_DATA_DIR)).load();
        CHECK(fromGtfs.routes.size() == 1);
        CHECK(fromGtfs.routes[0].path.size() > 2);
        CHECK(!fromGtfs.routes[0].path.front().hasLocal);

        {
            std::ofstream out(tempCsv);
            out << "ROUTE,R,R,trunk,#f00,A;B," << shapeId << "\n"
                << "PATH,R,0,0\nPATH,R,5,2\nPATH,R,10,0\n";
        }
        const NetworkData fromCsv =
            CsvRouteAdapter(tempCsv.string(), std::string(SIT_DATA_DIR)).load();
        CHECK(fromCsv.routes[0].path.size() == 3);
        CHECK(fromCsv.routes[0].path.front().hasLocal);
        std::filesystem::remove(tempCsv);
    }

    NetworkData bad = data;
    bad.routes[0].stops.push_back("XXX");
    CHECK_THROWS(NetworkBuilder().withData(bad).build(), InvalidRouteException);
    bad = data;
    bad.stops.push_back(bad.stops[0]);
    CHECK_THROWS(NetworkBuilder().withData(bad).build(), DataLoadException);
    bad = data;
    bad.routes[0].stops.resize(1);
    CHECK_THROWS(NetworkBuilder().withData(bad).build(), InvalidRouteException);
    bad = data;
    bad.routes[0].path = {PathPointData{300, 50, {}, true}, PathPointData{130, 50, {}, true}};
    CHECK_THROWS(NetworkBuilder().withData(bad).build(), DataLoadException);
    return 0;
}
