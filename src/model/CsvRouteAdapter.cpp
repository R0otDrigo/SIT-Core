#include "model/CsvRouteAdapter.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "common/Exceptions.h"
#include "common/ParseNumber.h"
#include "model/GtfsRouteAdapter.h"

namespace sit {
namespace {

std::string trim(const std::string& s) {
    const auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    return s.substr(b, s.find_last_not_of(" \t\r\n") - b + 1);
}

std::vector<std::string> split(const std::string& s, char sep) {
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, sep)) out.push_back(trim(item));
    return out;
}

[[noreturn]] void fail(int line, const std::string& msg) {
    throw DataLoadException("CSV linea " + std::to_string(line) + ": " + msg);
}

} // namespace

NetworkData CsvRouteAdapter::load() {
    std::ifstream file(path_);
    if (!file) throw DataLoadException("no se pudo abrir " + path_);
    NetworkData data = parse(file);
    applyGtfsFallback(data);
    return data;
}

void CsvRouteAdapter::applyGtfsFallback(NetworkData& data) const {
    bool needed = false;
    for (const auto& route : data.routes)
        if (route.path.empty() && !route.gtfsShapeId.empty()) { needed = true; break; }
    if (!needed) return;

    std::string dir = gtfsDir_;
    if (dir.empty()) {
        const auto slash = path_.find_last_of("/\\");
        dir = slash == std::string::npos ? "." : path_.substr(0, slash);
    }

    GtfsDataset dataset;
    try {
        dataset = GtfsRouteAdapter(dir).load();
    } catch (const SitException&) {
        return;
    }

    std::unordered_map<std::string, const GtfsRoute*> byId;
    std::unordered_map<std::string, const GtfsRoute*> byShape;
    for (const auto& route : dataset.routes) {
        byId.emplace(route.routeId, &route);
        if (!route.shapeId.empty()) byShape.emplace(route.shapeId, &route);
    }
    for (auto& route : data.routes) {
        if (!route.path.empty() || route.gtfsShapeId.empty()) continue;
        const GtfsRoute* match = nullptr;
        const auto it = byId.find(route.gtfsShapeId);
        if (it != byId.end()) {
            match = it->second;
        } else {
            const auto shapeIt = byShape.find(route.gtfsShapeId);
            if (shapeIt != byShape.end()) match = shapeIt->second;
        }
        if (!match) continue;
        for (const auto& geo : match->shape) route.path.push_back({0.0, 0.0, geo, false});
    }
}

NetworkData CsvRouteAdapter::parse(std::istream& in) {
    NetworkData data;
    std::vector<std::pair<std::string, PathPointData>> pendingPaths;
    std::string raw;
    int lineNo = 0;
    while (std::getline(in, raw)) {
        ++lineNo;
        const std::string line = trim(raw);
        if (line.empty() || line[0] == '#') continue;
        const auto f = split(line, ',');
        try {
            if (f[0] == "LIMIT" && f.size() == 2) {
                data.fleetLimit = parseInt(f[1]);
            } else if (f[0] == "STOP" && (f.size() == 7 || f.size() == 9)) {
                GeoCoordinate geo;
                if (f.size() == 9) {
                    geo = GeoCoordinate{parseDouble(f[7]), parseDouble(f[8])};
                    if (!geo.isValid())
                        fail(lineNo, "coordenada geografica invalida");
                }
                data.stops.push_back(
                    {f[1], f[2], f[3], parseDouble(f[4]), parseDouble(f[5]), parseInt(f[6]), geo});
            } else if (f[0] == "ROUTE" && (f.size() == 6 || f.size() == 7)) {
                data.routes.push_back({f[1], f[2], f[3], f[4], split(f[5], ';'), {},
                                       f.size() == 7 ? f[6] : ""});
            } else if (f[0] == "PATH" && (f.size() == 4 || f.size() == 6)) {
                PathPointData point;
                point.x = parseDouble(f[2]);
                point.y = parseDouble(f[3]);
                if (!std::isfinite(point.x) || !std::isfinite(point.y))
                    fail(lineNo, "punto de trazado invalido");
                point.hasLocal = true;
                if (f.size() == 6) {
                    point.geo = GeoCoordinate{parseDouble(f[4]), parseDouble(f[5])};
                    if (!point.geo.isValid())
                        fail(lineNo, "coordenada geografica invalida en PATH");
                }
                pendingPaths.push_back({f[1], point});
            } else if (f[0] == "FLEET" && f.size() == 4) {
                data.fleet.push_back({f[1], f[2], static_cast<std::size_t>(parseUnsigned(f[3]))});
            } else {
                fail(lineNo, "registro desconocido o con campos incorrectos: " + f[0]);
            }
        } catch (const std::logic_error&) {      // parseDouble / parseInt / parseUnsigned
            fail(lineNo, "numero invalido");
        }
    }

    for (const auto& entry : pendingPaths) {
        auto it = std::find_if(data.routes.begin(), data.routes.end(),
                               [&](const RouteData& r) { return r.id == entry.first; });
        if (it == data.routes.end())
            throw DataLoadException("PATH usa una ruta inexistente: " + entry.first);
        it->path.push_back(entry.second);
    }
    return data;
}

} // namespace sit
