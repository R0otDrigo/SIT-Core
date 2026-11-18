#include <QApplication>
#include <QGraphicsRectItem>
#include <QMouseEvent>
#include "check.h"
#include "view/MapView.h"

static void click(MapView& view, const QPointF& scenePoint, Qt::MouseButton button = Qt::LeftButton) {
    const QPoint pos = view.mapFromScene(scenePoint);
    QMouseEvent press(QEvent::MouseButtonPress, QPointF(pos),
                      QPointF(view.viewport()->mapToGlobal(pos)), button, button, Qt::NoModifier);
    QApplication::sendEvent(view.viewport(), &press);
    QMouseEvent release(QEvent::MouseButtonRelease, QPointF(pos),
                        QPointF(view.viewport()->mapToGlobal(pos)), button, Qt::NoButton, Qt::NoModifier);
    QApplication::sendEvent(view.viewport(), &release);
}

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    MapView view;
    view.resize(700, 500);
    view.show();

    sit::Snapshot snapshot;
    sit::StopView stop;
    stop.id = "P1";
    stop.name = "Paradero";
    stop.x = 100;
    stop.y = 100;
    snapshot.stops.push_back(stop);
    sit::RouteView route;
    route.id = "T1";
    route.name = "Troncal";
    route.color = "#336699";
    route.stopIds.push_back("P1");
    snapshot.routes.push_back(route);
    sit::VehicleView bus;
    bus.id = 7;
    bus.type = "Articulado";
    bus.routeId = "T1";
    bus.x = 140;
    bus.y = 100;
    bus.passengers = 23;
    bus.capacity = 160;
    bus.dwelling = true;
    bus.nextStopName = "Terminal";
    snapshot.vehicles.push_back(bus);
    view.refresh(snapshot);
    app.processEvents();

    int clicks = 0;
    QObject::connect(&view, &MapView::busClicked,
                     [&](int id, const QString& type, const QString& routeName,
                         int passengers, int capacity, bool dwelling, const QString& nextStop) {
        ++clicks;
        if (clicks <= 2) {
            CHECK(id == 7);
            CHECK(type == "Articulado");
            CHECK(routeName == "T1 - Troncal");
            CHECK(passengers == 23);
            CHECK(capacity == 160);
            CHECK(dwelling);
            CHECK(nextStop == "Terminal");
        } else {
            CHECK(id == 8);
        }
    });

    QGraphicsRectItem* drawing = nullptr;
    for (QGraphicsItem* item : view.scene()->items()) {
        auto* rect = dynamic_cast<QGraphicsRectItem*>(item);
        if (rect && rect->data(0).isValid() && rect->data(0).toInt() == 7) drawing = rect;
    }
    CHECK(drawing != nullptr);
    click(view, drawing->mapToScene(QPointF(drawing->rect().right() - 1, 0)));
    CHECK(clicks == 1);
    CHECK(drawing->pen().color() == QColor("#f59e0b"));
    CHECK(drawing->zValue() == 5);
    CHECK(!drawing->childItems().isEmpty());
    QGraphicsItem* number = drawing->childItems().first();
    click(view, number->mapToScene(number->boundingRect().center()));
    CHECK(clicks == 2);

    sit::VehicleView secondBus = bus;
    secondBus.id = 8;
    secondBus.x = 200;
    snapshot.vehicles.push_back(secondBus);
    view.refresh(snapshot);
    QGraphicsRectItem* secondDrawing = nullptr;
    for (QGraphicsItem* item : view.scene()->items()) {
        auto* rect = dynamic_cast<QGraphicsRectItem*>(item);
        if (rect && rect->data(0).isValid() && rect->data(0).toInt() == 8) secondDrawing = rect;
    }
    CHECK(secondDrawing != nullptr);
    click(view, secondDrawing->scenePos());
    CHECK(clicks == 3);
    CHECK(secondDrawing->pen().color() == QColor("#f59e0b"));
    CHECK(drawing->pen().color() == QColor(Qt::black));

    snapshot.vehicles.back().x = 220;
    view.refresh(snapshot);
    CHECK(secondDrawing->scenePos().x() == 220);
    CHECK(secondDrawing->pen().color() == QColor("#f59e0b"));

    click(view, QPointF(300, 200));
    click(view, drawing->scenePos(), Qt::RightButton);
    CHECK(clicks == 3);

    snapshot.vehicles.clear();
    view.refresh(snapshot);
    click(view, QPointF(bus.x, bus.y));
    click(view, QPointF(secondBus.x, secondBus.y));
    CHECK(clicks == 3);
    return 0;
}
