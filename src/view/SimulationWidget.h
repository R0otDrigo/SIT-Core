#pragma once
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QWidget>
#include <map>
#include <memory>
#include "controller/SimulationController.h"

class QQuickWidget;
class MapBridge;

// Vista de la simulacion: mapa OSM (QML) + panel de control del "controlador de flota". Es un
// QWidget hijo para poder incrustarse dentro de UserDashboard. Solo muestra datos (Snapshot) y
// reenvia las acciones del usuario al SimulationController.
class SimulationWidget : public QWidget {
    Q_OBJECT
public:
    explicit SimulationWidget(std::unique_ptr<sit::Network> network, QWidget* parent = nullptr);

    SimulationController* controller() const { return controller_; }
    QTableWidget* stopsTable() const { return stopsTable_; }
    QQuickWidget* mapWidget() const { return map_; }

private slots:
    void onSnapshot(const sit::Snapshot& snapshot);
    void onError(const QString& message);
    void onRouteChanged();

private:
    void buildUi();
    void populateRoutes(const sit::Snapshot& snapshot);
    void addLog(const QString& line);

    SimulationController* controller_{};
    MapBridge* mapBridge_{};
    QQuickWidget* map_{};

    QLabel* clockLabel_{};
    QLabel* busesLabel_{};
    QLabel* transportedLabel_{};
    QLabel* abandonedLabel_{};
    QLabel* saturationLabel_{};
    QLabel* eventsLabel_{};
    QPushButton* pauseButton_{};
    QComboBox* speedCombo_{};
    QComboBox* routeCombo_{};
    QComboBox* vehicleTypeCombo_{};
    QComboBox* eventCombo_{};
    QTableWidget* stopsTable_{};
    QListWidget* logList_{};

    bool routesLoaded_{false};
    std::map<QString, bool> routeIsTrunk_;
};
