#include "model/CsvRouteAdapter.h"
#include <fstream>
#include <sstream>
#include "common/Exceptions.h"
#include "common/ParseNumber.h"

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
    return parse(file);
}

NetworkData CsvRouteAdapter::parse(std::istream& in) {
    NetworkData data;
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
            } else if (f[0] == "ROUTE" && f.size() == 6) {
                data.routes.push_back({f[1], f[2], f[3], f[4], split(f[5], ';')});
            } else if (f[0] == "FLEET" && f.size() == 4) {
                data.fleet.push_back({f[1], f[2], static_cast<std::size_t>(parseUnsigned(f[3]))});
            } else {
                fail(lineNo, "registro desconocido o con campos incorrectos: " + f[0]);
            }
        } catch (const std::logic_error&) {      // parseDouble / parseInt / parseUnsigned
            fail(lineNo, "numero invalido");
        }
    }
    return data;
}

} // namespace sit
