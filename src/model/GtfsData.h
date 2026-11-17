#pragma once
#include <cstddef>
#include <string>
#include <vector>
#include "common/GeoCoordinate.h"

namespace sit {

// Metadatos GTFS usados para dibujar el trazado real de cada ruta en el mapa OSM.
struct GtfsRoute {
    std::string routeId;
    std::string shortName;
    std::string longName;
    std::string color;
    std::string headsign;
    std::string shapeId;
    int routeType{3};
    std::vector<GeoCoordinate> shape;      // shape.txt ordenado por shape_pt_sequence
};

struct GtfsDataset {
    std::vector<GtfsRoute> routes;
    std::size_t shapePointCount{0};
};

} // namespace sit
