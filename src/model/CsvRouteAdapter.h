#pragma once
#include <iosfwd>
#include <string>
#include "model/NetworkData.h"

namespace sit {

// Adapter: convierte un archivo de texto CSV (adaptado) a la interfaz IRouteSource.
//   LIMIT,10
//   STOP,id,nombre,tipo,x,y,capacidad
//   ROUTE,id,nombre,tipo,#color,id1;id2;id3
//   FLEET,idRuta,tipoBus,indiceInicial
class CsvRouteAdapter : public IRouteSource {
public:
    explicit CsvRouteAdapter(std::string path) : path_(std::move(path)) {}
    NetworkData load() override;
    std::string describe() const override { return "CSV: " + path_; }

    static NetworkData parse(std::istream& in);   // separado para poder probarlo sin archivos

private:
    std::string path_;
};

} // namespace sit
