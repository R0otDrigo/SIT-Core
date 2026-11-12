#include "view/MapView.h"
#include <QBrush>
#include <QFont>
#include <QPainter>
#include <QPen>
#include <QSet>
#include <QLineF>
#include <algorithm>
#include <vector>

namespace {

QColor saturationColor(double s, bool closed) {
    if (closed) return QColor("#495057");
    if (s < 0.5) return QColor("#43aa8b");
    if (s < 0.8) return QColor("#f9c74f");
    return QColor("#d62828");
}

} // namespace

MapView::MapView(QWidget* parent) : QGraphicsView(parent) {
    setScene(&scene_);
    setRenderHint(QPainter::Antialiasing);
    setBackgroundBrush(QColor("#f1f5f9"));
    setFrameShape(QFrame::NoFrame);
}

void MapView::build(const sit::Snapshot& snap) {
    std::map<std::string, QPointF> pos;
    double minX = 1e9, minY = 1e9, maxX = -1e9, maxY = -1e9;
    for (const auto& s : snap.stops) {
        pos[s.id] = QPointF(s.x, s.y);
        minX = std::min(minX, s.x); maxX = std::max(maxX, s.x);
        minY = std::min(minY, s.y); maxY = std::max(maxY, s.y);
    }
    scene_.setSceneRect(minX - 70, minY - 60, (maxX - minX) + 270, (maxY - minY) + 150);

    // Rutas (debajo de todo).
    for (const auto& r : snap.routes) {
        QPainterPath path;
        bool first = true;
        for (const auto& id : r.stopIds) {
            if (first) { path.moveTo(pos[id]); first = false; } else path.lineTo(pos[id]);
        }
        RouteItem item;
        item.color = QColor(QString::fromStdString(r.color));
        item.trunk = r.trunk;
        item.path = scene_.addPath(path);
        item.path->setZValue(0);
        routes_[r.id] = item;
    }

    // Leyenda.
    double ly = scene_.sceneRect().top() + 8;
    for (const auto& r : snap.routes) {
        auto* t = scene_.addSimpleText(QString::fromStdString(r.id + "  " + r.name));
        t->setBrush(QColor(QString::fromStdString(r.color)).darker(115));
        QFont f = t->font(); f.setBold(true); f.setPointSize(9); t->setFont(f);
        t->setPos(scene_.sceneRect().right() - 260, ly);
        ly += 18;
    }

    // Paraderos.
    for (const auto& s : snap.stops) {
        StopItem item;
        item.dot = scene_.addEllipse(-12, -12, 24, 24, QPen(Qt::white, 2), QBrush(Qt::green));
        item.dot->setPos(s.x, s.y);
        item.dot->setZValue(2);
        item.count = new QGraphicsSimpleTextItem(item.dot);         // pasajeros esperando
        QFont cf = item.count->font(); cf.setPointSize(7); cf.setBold(true); item.count->setFont(cf);
        item.label = scene_.addSimpleText(QString::fromStdString(s.name));
        QFont f = item.label->font(); f.setPointSize(8); item.label->setFont(f);
        item.label->setBrush(QColor("#1e293b"));
        item.label->setZValue(4);
        stops_[s.id] = item;
    }

    placeLabels(snap, pos);

    eventBanner_ = scene_.addSimpleText("");
    QFont bf = eventBanner_->font(); bf.setBold(true); bf.setPointSize(10); eventBanner_->setFont(bf);
    eventBanner_->setBrush(QColor("#b91c1c"));
    eventBanner_->setPos(scene_.sceneRect().left() + 10, scene_.sceneRect().bottom() - 28);
    eventBanner_->setZValue(5);

    built_ = true;
    fitInView(scene_.sceneRect(), Qt::KeepAspectRatio);
}

// Coloca cada etiqueta en la esquina del paradero (6 candidatas) que menos se cruce con otros
// puntos, otras etiquetas y las lineas de las rutas.
void MapView::placeLabels(const sit::Snapshot& snap, const std::map<std::string, QPointF>& pos) {
    std::vector<QPointF> routePoints;                       // muestras a lo largo de las rutas
    for (const auto& r : snap.routes)
        for (std::size_t i = 1; i < r.stopIds.size(); ++i) {
            const QPointF a = pos.at(r.stopIds[i - 1]), b = pos.at(r.stopIds[i]);
            const int n = std::max(2, static_cast<int>(QLineF(a, b).length() / 6));
            for (int k = 0; k <= n; ++k) routePoints.push_back(a + (b - a) * (static_cast<double>(k) / n));
        }

    std::vector<QRectF> dots;
    for (const auto& s : snap.stops) dots.push_back(QRectF(s.x - 14, s.y - 14, 28, 28));
    std::vector<QRectF> placed;
    const QRectF bounds = scene_.sceneRect();

    for (std::size_t i = 0; i < snap.stops.size(); ++i) {
        const auto& s = snap.stops[i];
        auto* label = stops_[s.id].label;
        const QSizeF sz = label->boundingRect().size();
        const double w = sz.width(), h = sz.height();

        const QPointF cands[] = {{s.x + 14, s.y - h - 2}, {s.x + 14, s.y + 2}, {s.x - 14 - w, s.y - h - 2},
                                 {s.x - 14 - w, s.y + 2}, {s.x - w / 2, s.y - h - 14}, {s.x - w / 2, s.y + 14}};
        QPointF best = cands[0];
        int bestScore = 1 << 30;
        for (const QPointF& c : cands) {
            const QRectF rect(c, QSizeF(w, h));
            int score = bounds.contains(rect) ? 0 : 1000;
            for (std::size_t j = 0; j < dots.size(); ++j) if (j != i && rect.intersects(dots[j])) score += 50;
            for (const QRectF& p : placed) if (rect.intersects(p)) score += 80;
            const QRectF padded = rect.adjusted(-4, -4, 4, 4);
            for (const QPointF& pt : routePoints) if (padded.contains(pt)) score += 1;
            if (score < bestScore) { bestScore = score; best = c; }
        }
        label->setPos(best);
        placed.push_back(QRectF(best, QSizeF(w, h)));
    }
}

void MapView::refresh(const sit::Snapshot& snap) {
    if (!built_) build(snap);

    // Rutas afectadas por eventos: linea roja discontinua.
    QSet<QString> affected;
    QStringList banner;
    for (const auto& e : snap.events) {
        affected.insert(QString::fromStdString(e.routeId));
        banner << QString("%1 en %2 (%3 s)").arg(QString::fromStdString(e.name), QString::fromStdString(e.routeId))
                      .arg(static_cast<int>(e.remaining));
    }
    for (auto& [id, r] : routes_) {
        const bool hit = affected.contains(QString::fromStdString(id));
        QColor c = hit ? QColor("#b91c1c") : r.color;
        c.setAlpha(hit ? 220 : 150);
        QPen pen(c, r.trunk ? 8 : 5, hit ? Qt::DashLine : Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        r.path->setPen(pen);
    }
    eventBanner_->setText(banner.isEmpty() ? QString() : "ALERTA: " + banner.join("  |  "));

    // Paraderos: color por saturacion, contador de pasajeros dentro del circulo.
    for (const auto& s : snap.stops) {
        auto it = stops_.find(s.id);
        if (it == stops_.end()) continue;
        const QColor fill = saturationColor(s.saturation, s.closed);
        it->second.dot->setBrush(fill);
        it->second.dot->setPen(QPen(s.closed ? QColor("#b91c1c") : Qt::white, s.closed ? 3 : 2));
        it->second.dot->setToolTip(QString("%1\nEsperan: %2 (%3 %)%4").arg(QString::fromStdString(s.name)).arg(s.waiting)
                                       .arg(s.saturation * 100.0, 0, 'f', 0).arg(s.closed ? "\nCERRADO por evento" : ""));
        auto* count = it->second.count;
        count->setText(QString::number(s.waiting));
        count->setBrush(fill == QColor("#f9c74f") ? QColor("#1e293b") : QColor(Qt::white));
        count->setPos(-count->boundingRect().width() / 2, -count->boundingRect().height() / 2);
    }

    // Buses: crear, mover y eliminar los que ya no existen.
    QSet<int> alive;
    for (const auto& v : snap.vehicles) {
        alive.insert(v.id);
        auto it = vehicles_.find(v.id);
        if (it == vehicles_.end()) {
            const double w = 12.0 + v.capacity / 16.0;
            auto* rect = scene_.addRect(-w / 2, -6, w, 12, QPen(Qt::black, 1.5));
            rect->setZValue(3);
            auto rc = routes_.find(v.routeId);
            rect->setBrush(rc != routes_.end() ? rc->second.color : QColor(Qt::gray));
            auto* txt = new QGraphicsSimpleTextItem(rect);
            txt->setBrush(Qt::white);
            QFont f = txt->font(); f.setPointSize(7); f.setBold(true); txt->setFont(f);
            it = vehicles_.emplace(v.id, rect).first;
        }
        auto* rect = it->second;
        rect->setPos(v.x, v.y);
        rect->setToolTip(QString("Bus #%1 - %2\nRuta %3\nPasajeros %4/%5")
                             .arg(v.id).arg(QString::fromStdString(v.type), QString::fromStdString(v.routeId))
                             .arg(v.passengers).arg(v.capacity));
        if (auto* txt = static_cast<QGraphicsSimpleTextItem*>(rect->childItems().first())) {
            txt->setText(QString::number(v.passengers));
            txt->setPos(-txt->boundingRect().width() / 2, -txt->boundingRect().height() / 2);
        }
    }
    for (auto it = vehicles_.begin(); it != vehicles_.end();) {
        if (!alive.contains(it->first)) { scene_.removeItem(it->second); delete it->second; it = vehicles_.erase(it); }
        else ++it;
    }
}

void MapView::resizeEvent(QResizeEvent* event) {
    QGraphicsView::resizeEvent(event);
    if (built_) fitInView(scene_.sceneRect(), Qt::KeepAspectRatio);
}
