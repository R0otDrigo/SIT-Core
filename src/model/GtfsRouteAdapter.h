#pragma once
#include <iosfwd>
#include <string>
#include "model/GtfsData.h"

namespace sit {

// Adapter para el estandar GTFS: lee routes.txt, trips.txt y shapes.txt y produce los
// trazados geograficos que consume la vista. Solo depende de la biblioteca estandar.
class GtfsRouteAdapter {
public:
    explicit GtfsRouteAdapter(std::string directory) : directory_(std::move(directory)) {}

    GtfsDataset load() const;      // lanza DataLoadException si faltan archivos o son invalidos
    std::string describe() const { return "GTFS: " + directory_; }

    static GtfsDataset parse(std::istream& routes, std::istream& trips, std::istream& shapes);

private:
    std::string directory_;
};

} // namespace sit
