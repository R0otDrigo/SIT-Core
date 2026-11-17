#pragma once
#include <QComboBox>
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
#include "view/MapView.h"

// Vista de la simulacion: mapa + panel de control del "controlador de flota". Es un QWidget
// hijo para poder incrustarse dentro de UserDashboard. Solo muestra datos (Snapshot) y reenvia
// las acciones del usuario al SimulationController.
class SimulationWidget : public QWidget {
    Q_OBJECT
public:
    explicit SimulationWidget(std::unique_ptr<sit::Network> network, QWidget* parent = nullptr);

    SimulationController* controller() const { return controller_; }
    QTableWidget* stopsTable() const { return stopsTable_; }

private slots:
    void onSnapshot(const sit::Snapshot& snapshot);
    void onError(const QString& message);
    void onRouteChanged();

private:
    void buildUi();
    void populateRoutes(const sit::Snapshot& snapshot);
    void showBusInfo(int id, const QString& type, const QString& route,
                     int passengers, int capacity, bool dwelling, const QString& nextStop);
    void clearBusInfo(const QString& message);
    void addLog(const QString& line);

    SimulationController* controller_{};
    MapView* map_{};

    QLabel* clockLabel_{};
    QGroupBox* busInfoPanel_{};
    QWidget* busInfoDetails_{};
    QLabel* busInfoHint_{};
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
    QListWidget* logList_{};

    bool routesLoaded_{false};
    int selectedBusId_{-1};
    std::map<QString, bool> routeIsTrunk_;
};
