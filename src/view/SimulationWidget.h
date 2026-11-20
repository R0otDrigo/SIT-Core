#pragma once
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QProgressBar>
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
    QListWidget* routeList() const { return routeList_; }
    MapBridge* mapBridge() const { return mapBridge_; }

    // Resalta una ruta en el mapa y la deja elegida para flota y eventos ("" = todas).
    void focusRoute(const QString& routeId);

    void showIncomingCall(const QString& vehicleId, const QString& name,
                          const QString& status = QString());

signals:
    void callAnswered(const QString& vehicleId);
    void callRequested(const QString& vehicleId);

private slots:
    void onSnapshot(const sit::Snapshot& snapshot);
    void onError(const QString& message);
    void onRouteChanged();

private:
    void buildUi();
    void populateRoutes(const sit::Snapshot& snapshot);
    void showBusInfo(int id, const QString& type, const QString& route,
                     int passengers, int capacity, bool dwelling, const QString& nextStop);
    void clearBusInfo();
    void addLog(const QString& line);
    void updateRouteList(const sit::Snapshot& snapshot);
    void filterRoutes(const QString& text);

    SimulationController* controller_{};
    MapBridge* mapBridge_{};
    QQuickWidget* map_{};

    QLabel* clockCaption_{};
    QLabel* clockLabel_{};
    QGroupBox* busInfoPanel_{};
    QWidget* busInfoDetails_{};
    QPushButton* backToOverviewButton_{};
    QLabel* busTypeLabel_{};
    QLabel* busRouteLabel_{};
    QLabel* busPassengersLabel_{};
    QLabel* busFreeSeatsLabel_{};
    QLabel* busStatusLabel_{};
    QLabel* busNextStopLabel_{};
    QProgressBar* busOccupancyBar_{};
    QLabel* busesLabel_{};
    QLabel* transportedLabel_{};
    QLabel* abandonedLabel_{};
    QLabel* saturationLabel_{};
    QPushButton* pauseButton_{};
    QComboBox* speedCombo_{};
    QComboBox* routeCombo_{};
    QComboBox* vehicleTypeCombo_{};
    QComboBox* eventCombo_{};
    QTableWidget* stopsTable_{};
    QGroupBox* stopsBox_{};
    QLineEdit* routeSearch_{};
    QListWidget* routeList_{};
    QCheckBox* gtfsCheck_{};
    int snapshotCount_{0};
    QListWidget* logList_{};

    bool routesLoaded_{false};
    int selectedBusId_{-1};
    std::map<QString, bool> routeIsTrunk_;
};
