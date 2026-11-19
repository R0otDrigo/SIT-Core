#include "model/GtfsRouteAdapter.h"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <vector>
#include "common/Exceptions.h"
#include "common/ParseNumber.h"

namespace sit {
namespace {

std::string trim(const std::string& s) {
    const auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    return s.substr(b, s.find_last_not_of(" \t\r\n") - b + 1);
}

std::string stripBom(std::string s) {
    if (s.size() >= 3 && static_cast<unsigned char>(s[0]) == 0xEF &&
        static_cast<unsigned char>(s[1]) == 0xBB && static_cast<unsigned char>(s[2]) == 0xBF)
        s.erase(0, 3);
    return s;
}

// Parser CSV minimo con soporte de campos entre comillas (los nombres GTFS traen comas).
std::vector<std::string> splitCsv(const std::string& line) {
    std::vector<std::string> out;
    std::string field;
    bool quoted = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        const char c = line[i];
        if (c == '"') {
            if (quoted && i + 1 < line.size() && line[i + 1] == '"') {
                field.push_back('"');
                ++i;
            } else {
                quoted = !quoted;
            }
        } else if (c == ',' && !quoted) {
            out.push_back(trim(field));
            field.clear();
        } else {
            field.push_back(c);
        }
    }
    out.push_back(trim(field));
    return out;
}

struct CsvTable {
    std::vector<std::string> header;
    std::unordered_map<std::string, std::size_t> index;
};

CsvTable readHeader(std::istream& in, const std::string& what) {
    CsvTable table;
    std::string line;
    while (std::getline(in, line)) {
        if (trim(line).empty()) continue;
        table.header = splitCsv(line);
        if (!table.header.empty()) table.header[0] = stripBom(table.header[0]);
        for (std::size_t i = 0; i < table.header.size(); ++i)
            table.index[table.header[i]] = i;
        return table;
    }
    throw DataLoadException("GTFS " + what + ": archivo vacio");
}

bool readRow(std::istream& in, std::vector<std::string>& row) {
    std::string line;
    while (std::getline(in, line)) {
        if (trim(line).empty()) continue;
        row = splitCsv(line);
        return true;
    }
    return false;
}

std::size_t column(const CsvTable& table, const std::string& name, const std::string& what) {
    const auto it = table.index.find(name);
    if (it == table.index.end())
        throw DataLoadException("GTFS " + what + ": falta la columna '" + name + "'");
    return it->second;
}

std::string field(const std::vector<std::string>& row, std::size_t index) {
    return index < row.size() ? row[index] : "";
}

std::string normalizeColor(std::string color) {
    color = trim(color);
    if (color.empty()) return "#808080";
    if (color.front() != '#') color.insert(color.begin(), '#');
    return color;
}

} // namespace

GtfsDataset GtfsRouteAdapter::load() const {
    const std::string dir = directory_;
    std::ifstream routes(dir + "/routes.txt");
    std::ifstream trips(dir + "/trips.txt");
    std::ifstream shapes(dir + "/shapes.txt");
    if (!routes || !trips || !shapes)
        throw DataLoadException("GTFS incompleto en " + dir + " (faltan routes/trips/shapes)");
    return parse(routes, trips, shapes);
}

GtfsDataset GtfsRouteAdapter::parse(std::istream& routesIn, std::istream& tripsIn,
                                    std::istream& shapesIn) {
    GtfsDataset data;

    // --- routes.txt ---
    const CsvTable routesHeader = readHeader(routesIn, "routes");
    const std::size_t routeIdCol = column(routesHeader, "route_id", "routes");
    const std::size_t shortNameCol = column(routesHeader, "route_short_name", "routes");
    const std::size_t longNameCol = column(routesHeader, "route_long_name", "routes");
    const std::size_t colorCol = column(routesHeader, "route_color", "routes");
    const auto typeIt = routesHeader.index.find("route_type");
    const std::size_t routeTypeCol =
        typeIt == routesHeader.index.end() ? routesHeader.header.size() : typeIt->second;

    std::unordered_map<std::string, std::size_t> routeIndex;
    std::vector<std::string> row;
    while (readRow(routesIn, row)) {
        GtfsRoute route;
        route.routeId = field(row, routeIdCol);
        if (route.routeId.empty()) continue;
        route.shortName = field(row, shortNameCol);
        route.longName = field(row, longNameCol);
        route.color = normalizeColor(field(row, colorCol));
        if (routeTypeCol < row.size()) {
            try {
                route.routeType = parseInt(row[routeTypeCol]);
            } catch (const std::logic_error&) {
                route.routeType = 3;
            }
        }
        routeIndex[route.routeId] = data.routes.size();
        data.routes.push_back(std::move(route));
    }

    // --- trips.txt: route_id -> shape_id / headsign ---
    const CsvTable tripsHeader = readHeader(tripsIn, "trips");
    const std::size_t tripRouteCol = column(tripsHeader, "route_id", "trips");
    const std::size_t shapeCol = column(tripsHeader, "shape_id", "trips");
    const auto headsignIt = tripsHeader.index.find("trip_headsign");
    const std::size_t headsignCol =
        headsignIt == tripsHeader.index.end() ? tripsHeader.header.size() : headsignIt->second;

    while (readRow(tripsIn, row)) {
        const auto it = routeIndex.find(field(row, tripRouteCol));
        if (it == routeIndex.end()) continue;
        GtfsRoute& route = data.routes[it->second];
        if (route.shapeId.empty()) route.shapeId = field(row, shapeCol);
        if (route.headsign.empty()) route.headsign = field(row, headsignCol);
    }

    // --- shapes.txt: shape_id -> puntos ordenados por sequence ---
    const CsvTable shapesHeader = readHeader(shapesIn, "shapes");
    const std::size_t shapeIdCol = column(shapesHeader, "shape_id", "shapes");
    const std::size_t latCol = column(shapesHeader, "shape_pt_lat", "shapes");
    const std::size_t lonCol = column(shapesHeader, "shape_pt_lon", "shapes");
    const std::size_t seqCol = column(shapesHeader, "shape_pt_sequence", "shapes");

    using Point = std::pair<long long, GeoCoordinate>;
    std::unordered_map<std::string, std::vector<Point>> shapePoints;
    while (readRow(shapesIn, row)) {
        try {
            const std::string shapeId = field(row, shapeIdCol);
            if (shapeId.empty()) continue;
            const long long seq = parseLongLong(field(row, seqCol));
            const GeoCoordinate geo{parseDouble(field(row, latCol)), parseDouble(field(row, lonCol))};
            if (!geo.isValid()) throw DataLoadException("GTFS shapes: coordenada invalida");
            shapePoints[shapeId].push_back({seq, geo});
        } catch (const std::logic_error&) {
            throw DataLoadException("GTFS shapes: numero invalido");
        }
    }
    data.shapePointCount = 0;
    for (auto& [shapeId, points] : shapePoints) data.shapePointCount += points.size();

    for (GtfsRoute& route : data.routes) {
        if (route.shapeId.empty()) continue;
        const auto it = shapePoints.find(route.shapeId);
        if (it == shapePoints.end()) continue;
        auto& points = it->second;
        std::sort(points.begin(), points.end(),
                  [](const Point& a, const Point& b) { return a.first < b.first; });
        route.shape.reserve(points.size());
        for (const auto& p : points) route.shape.push_back(p.second);
    }

    return data;
}

} // namespace sit
