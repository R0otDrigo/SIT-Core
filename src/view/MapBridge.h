#pragma once
#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QVariantMap>
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
    // Ruta resaltada en el mapa ("" = todas). focusedStops = {stopId: true} de esa ruta.
    Q_PROPERTY(QString focusedRouteId READ focusedRouteId WRITE setFocusedRouteId NOTIFY focusChanged)
    Q_PROPERTY(QString focusedRouteName READ focusedRouteName NOTIFY focusChanged)
    Q_PROPERTY(QVariantMap focusedStops READ focusedStops NOTIFY focusChanged)
    Q_PROPERTY(QString selectedVehicleId READ selectedVehicleId NOTIFY selectionChanged)
    Q_PROPERTY(QString selectedVehicleStats READ selectedVehicleStats NOTIFY selectionChanged)
    Q_PROPERTY(QString incomingCallVehicleId READ incomingCallVehicleId NOTIFY callChanged)
    Q_PROPERTY(QString incomingCallName READ incomingCallName NOTIFY callChanged)
    Q_PROPERTY(QString incomingCallStatus READ incomingCallStatus NOTIFY callChanged)

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
    QString focusedRouteId() const { return focusedRouteId_; }
    QString focusedRouteName() const { return focusedRouteName_; }
    QVariantMap focusedStops() const { return focusedStops_; }
    QString selectedVehicleId() const { return selectedVehicleId_; }
    QString selectedVehicleStats() const { return selectedVehicleStats_; }
    QString incomingCallVehicleId() const { return incomingCallVehicleId_; }
    QString incomingCallName() const { return incomingCallName_; }
    QString incomingCallStatus() const { return incomingCallStatus_; }

public slots:
    void setShowGtfs(bool show);
    void setFocusedRouteId(const QString& routeId);
    void selectVehicle(const QString& vehicleId);
    void clearSelection();
    void setIncomingCall(const QString& vehicleId, const QString& name, const QString& status);
    void clearIncomingCall();

signals:
    void viewChanged();
    void eventTextChanged();
    void showGtfsChanged();
    void gtfsStatusChanged();
    void focusChanged();
    void selectionChanged();
    void callChanged();
    void callAnswered(QString vehicleId);
    void callRequested(QString vehicleId);
    void busSelected(int id, const QString& type, const QString& route,
                     int passengers, int capacity, bool dwelling,
                     const QString& nextStop);

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
    bool showGtfs_{false};      // los 227 trazados GTFS saturan el mapa: se activan a pedido
    QString gtfsStatus_;
    QString focusedRouteId_;
    QString focusedRouteName_;
    QVariantMap focusedStops_;
    sit::Snapshot lastSnapshot_;
    QString selectedVehicleId_;
    QString selectedVehicleStats_;
    QString incomingCallVehicleId_;
    QString incomingCallName_;
    QString incomingCallStatus_;
};
