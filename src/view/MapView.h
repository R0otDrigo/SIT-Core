#pragma once
#include <QGraphicsEllipseItem>
#include <QGraphicsPathItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsView>
#include <map>
#include <string>
#include "model/Snapshot.h"

// Vista del mapa: dibuja rutas, paraderos (color segun saturacion) y buses en movimiento.
class MapView : public QGraphicsView {
    Q_OBJECT
public:
    explicit MapView(QWidget* parent = nullptr);
    void refresh(const sit::Snapshot& snapshot);

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void build(const sit::Snapshot& snapshot);        // items estaticos (una sola vez)
    void placeLabels(const sit::Snapshot& snapshot, const std::map<std::string, QPointF>& pos);

    struct StopItem {
        QGraphicsEllipseItem* dot{};
        QGraphicsSimpleTextItem* label{};
        QGraphicsSimpleTextItem* count{};
    };
    struct RouteItem {
        QGraphicsPathItem* path{};
        QColor color;
        bool trunk{};
    };

    QGraphicsScene scene_;
    bool built_{false};
    std::map<std::string, StopItem> stops_;
    std::map<std::string, RouteItem> routes_;
    std::map<int, QGraphicsRectItem*> vehicles_;
    QGraphicsSimpleTextItem* eventBanner_{};
};
