#pragma once
#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include "model/Snapshot.h"

class RouteListModel;
class StopListModel;
class VehicleListModel;
class GtfsRouteListModel;

// Puente C++ -> QML. Mantiene modelos estables y expone el centro/zoom inicial del mapa.
// Toda la logica de simulacion sigue en el modelo (Network/Route/Stop); aqui solo se adapta
// el Snapshot y se cargan los metadatos GTFS para el trazado de rutas.
class MapBridge : public QObject {
    Q_OBJECT
    Q_PROPERTY(QAbstractListModel* routeModel READ routeModel CONSTANT)
    Q_PROPERTY(QAbstractListModel* stopModel READ stopModel CONSTANT)
    Q_PROPERTY(QAbstractListModel* vehicleModel READ vehicleModel CONSTANT)
    Q_PROPERTY(QAbstractListModel* gtfsRouteModel READ gtfsRouteModel CONSTANT)
    Q_PROPERTY(double centerLatitude READ centerLatitude NOTIFY viewChanged)
    Q_PROPERTY(double centerLongitude READ centerLongitude NOTIFY viewChanged)
    Q_PROPERTY(double minLatitude READ minLatitude NOTIFY viewChanged)
    Q_PROPERTY(double maxLatitude READ maxLatitude NOTIFY viewChanged)
    Q_PROPERTY(double minLongitude READ minLongitude NOTIFY viewChanged)
    Q_PROPERTY(double maxLongitude READ maxLongitude NOTIFY viewChanged)
    Q_PROPERTY(double initialZoom READ initialZoom NOTIFY viewChanged)
    Q_PROPERTY(QString eventText READ eventText NOTIFY eventTextChanged)
    Q_PROPERTY(bool showGtfs READ showGtfs WRITE setShowGtfs NOTIFY showGtfsChanged)
    Q_PROPERTY(QString gtfsStatus READ gtfsStatus NOTIFY gtfsStatusChanged)

public:
    explicit MapBridge(QObject* parent = nullptr);

    // Carga routes.txt/trips.txt/shapes.txt. Si dataDir esta vacio usa data/ junto al ejecutable
    // y, como respaldo, SIT_DATA_DIR.
    void loadGtfs(const QString& dataDir = QString());
    void update(const sit::Snapshot& snapshot);

    QAbstractListModel* routeModel() const;
    QAbstractListModel* stopModel() const;
    QAbstractListModel* vehicleModel() const;
    QAbstractListModel* gtfsRouteModel() const;
    double centerLatitude() const { return centerLatitude_; }
    double centerLongitude() const { return centerLongitude_; }
    double minLatitude() const { return minLatitude_; }
    double maxLatitude() const { return maxLatitude_; }
    double minLongitude() const { return minLongitude_; }
    double maxLongitude() const { return maxLongitude_; }
    double initialZoom() const { return initialZoom_; }
    QString eventText() const { return eventText_; }
    bool showGtfs() const { return showGtfs_; }
    QString gtfsStatus() const { return gtfsStatus_; }

public slots:
    void setShowGtfs(bool show);

signals:
    void viewChanged();
    void eventTextChanged();
    void showGtfsChanged();
    void gtfsStatusChanged();

private:
    void updateViewFrom(const sit::Snapshot& snapshot);

    RouteListModel* routeModel_{};
    StopListModel* stopModel_{};
    VehicleListModel* vehicleModel_{};
    GtfsRouteListModel* gtfsModel_{};

    bool routesInitialized_{false};
    double centerLatitude_{-16.3989};
    double centerLongitude_{-71.5369};
    double minLatitude_{-16.3989};
    double maxLatitude_{-16.3989};
    double minLongitude_{-71.5369};
    double maxLongitude_{-71.5369};
    double initialZoom_{12.0};
    QString eventText_;
    bool showGtfs_{true};
    QString gtfsStatus_;
};
