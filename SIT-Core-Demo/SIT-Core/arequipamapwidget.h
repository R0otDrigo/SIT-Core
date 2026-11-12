#pragma once

#include <QWidget>
#include <QTimer>
#include <QMouseEvent>
#include <QMap>
#include <QVector>
#include <QPointF>

struct BusInfo {
    QString placa;
    QString rutaCodigo;     // e.g., "C-1A", "A-25", "T-14", "A-5", "T-18"
    QString conductor;
    int posIndex;           // current node index
    int direction;        // 1 = forward, -1 = backward
    QPointF pos;          // current position
    QPointF targetPos;    // target position for smooth movement
    QPointF _posActual;   // posición actual interpolada
    QPointF _posObjetivo; // posición objetivo
};

struct StopInfo {
    QString nombre;
    QPointF pos;
    bool isCurrent;
};

struct RouteData {
    QString id;           // e.g., "C-1A"
    QString nombre;       // e.g., "Tronral C-1A"
    QString unidadNegocio;// e.g., "Consorcio Íntegra Arequipa"
    QColor color;         // color hex
    QVector<QPointF> nodos; // coordenadas (x,y)
};

class ArequipaMapWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ArequipaMapWidget(QWidget *parent = nullptr);
    ~ArequipaMapWidget();

    void setRouteVisible(const QString &routeId, bool visible);
    bool isRouteVisible(const QString &routeId) const;

signals:
    void unitClicked(const QString &placa, const QString &conductor, const QString &rutaCodigo);
    void stopClicked(const QString &stopNombre);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

private:
    // Mapa base 1000x700, escalado al widget
    void drawBackground(QPainter &painter);
    void drawVolcanoes(QPainter &painter);
    void drawRiver(QPainter &painter);
    void drawDistricts(QPainter &painter);
    void drawRoutes(QPainter &painter);
    void drawBuses(QPainter &painter);
    void drawStops(QPainter &painter);
    QPointF normalizedPos(int x, int y) const;

    // Rutas de buses (5 rutas demo) - almacenadas como array
    RouteData m_routes[5]; // C-1A, A-25, T-14, A-5, T-18

    // Visibilidad de rutas
    bool m_routeVisible[5]; // visible state for C-1A, A-25, T-14, A-5, T-18

    // Posiciones de paraderos (fijos en el mapa)
    QVector<StopInfo> m_stops;

    // Buses en movimiento
    QVector<BusInfo> m_buses;

    // Estado
    QTimer *m_timer;
    int m_timerId;
    int m_currentRouteIdx; // para cycle through routes

    // Interacción
    QPointF m_lastMousePos;
    QString m_hoverUnitPlaca;
    QString m_hoverStopNombre;
};