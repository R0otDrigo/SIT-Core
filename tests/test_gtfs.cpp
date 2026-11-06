#include <sstream>
#include "check.h"
#include "common/Exceptions.h"
#include "model/GtfsRouteAdapter.h"

using namespace sit;

int main() {
    std::istringstream routes(
        "route_id,route_short_name,route_long_name,route_color,route_type\n"
        "R1,1,\"Linea, con coma\",FF0000,3\n"
        "R2,2,Segunda linea,,3\n");
    std::istringstream trips(
        "route_id,shape_id,trip_headsign\n"
        "R1,S1,Centro\n"
        "R2,S2,Norte\n");
    std::istringstream shapes(
        "shape_id,shape_pt_lat,shape_pt_lon,shape_pt_sequence\n"
        "S1,-16.40,-71.50,2\n"
        "S1,-16.39,-71.50,1\n"
        "S2,-16.41,-71.51,1\n");

    const GtfsDataset data = GtfsRouteAdapter::parse(routes, trips, shapes);
    CHECK(data.routes.size() == 2);
    CHECK(data.shapePointCount == 3);
    CHECK(data.routes[0].routeId == "R1");
    CHECK(data.routes[0].shortName == "1");
    CHECK(data.routes[0].longName == "Linea, con coma");
    CHECK(data.routes[0].color == "#FF0000");
    CHECK(data.routes[0].headsign == "Centro");
    CHECK(data.routes[0].shape.size() == 2);
    CHECK_NEAR(data.routes[0].shape[0].latitude, -16.39, 1e-9);   // ordenado por sequence
    CHECK_NEAR(data.routes[0].shape[1].latitude, -16.40, 1e-9);
    CHECK(data.routes[1].color == "#808080");
    CHECK(data.routes[1].shape.size() == 1);

    std::istringstream missingColumn(
        "route_short_name,route_long_name\n"
        "1,Sin id\n");
    std::istringstream emptyTrips("route_id,shape_id\n");
    std::istringstream emptyShapes("shape_id,shape_pt_lat,shape_pt_lon,shape_pt_sequence\n");
    CHECK_THROWS(GtfsRouteAdapter::parse(missingColumn, emptyTrips, emptyShapes), DataLoadException);
    return 0;
}
