#pragma once
#include <iosfwd>
#include <string>
#include "model/NetworkData.h"

namespace sit {

// Adapter: convierte un archivo de texto CSV (adaptado) a la interfaz IRouteSource.
//   LIMIT,10
//   STOP,id,nombre,tipo,x,y,capacidad[,lat,lon]   (lat/lon opcionales)
//   ROUTE,id,nombre,tipo,#color,id1;id2;id3[,shapeIdGTFS]
//   PATH,idRuta,x,y[,lat,lon]                     (puntos ordenados del trazado)
//   FLEET,idRuta,tipoBus,indiceInicial
class CsvRouteAdapter : public IRouteSource {
public:
    explicit CsvRouteAdapter(std::string path, std::string gtfsDir = {})
        : path_(std::move(path)), gtfsDir_(std::move(gtfsDir)) {}
    NetworkData load() override;
    std::string describe() const override { return "CSV: " + path_; }

    static NetworkData parse(std::istream& in);   // separado para poder probarlo sin archivos

private:
    std::string path_;
    std::string gtfsDir_;

    void applyGtfsFallback(NetworkData& data) const;
};

} // namespace sit
