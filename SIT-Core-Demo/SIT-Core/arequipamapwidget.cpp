#include "arequipamapwidget.h"
#include "uihelpers.h"
#include "theme.h"

#include <QPainter>
#include <QPainterPath>
#include <QTimerEvent>
#include <QMouseEvent>
#include <QDebug>
#include <cmath>

ArequipaMapWidget::ArequipaMapWidget(QWidget *parent)
    : QWidget(parent)
    , m_timerId(0)
    , m_currentRouteIdx(0)
{
    // Inicializar rutas (5 rutas demo del SIT Arequipa)
    // Ruta 0: C-1A Troncal (Cono Norte - Río Seco ↔ Lara / Socabaya) - Rojo
    m_routes[0].id = "C-1A";
    m_routes[0].nombre = "Troncal C-1A";
    m_routes[0].unidadNegocio = "Consorcio Íntegra Arequipa";
    m_routes[0].color = QColor("#EF4444");
    m_routes[0].nodos = {
        QPointF(220, 120), // Estación Norte Río Seco
        QPointF(290, 190), // Zamácola / Aeropuerto
        QPointF(360, 250), // Puente Añashuayco
        QPointF(440, 290), // Av. Ejército (Real Plaza)
        QPointF(510, 330), // Puente Grau / Cercado
        QPointF(530, 410), // Av. Daniel Alcides Carrión (Hospital Honorio Delgado)
        QPointF(550, 480), // Plat. Andrés Avelino Cáceres
        QPointF(540, 610)  // Lara / Socabaya (Terminal Sur)
    };

    // Ruta 1: A-25 Alimentadora (Cayma - Enace ↔ Centro / UNSA) - Azul
    m_routes[1].id = "A-25";
    m_routes[1].nombre = "Alimentadora A-25";
    m_routes[1].unidadNegocio = "C-2 (TransCayma)";
    m_routes[1].color = QColor("#3B82F6");
    m_routes[1].nodos = {
        QPointF(460, 100), // Enace / Dean Valdivia (Cayma)
        QPointF(450, 180), // Plaza de Cayma
        QPointF(440, 290), // Mall Plaza Cayma (Av. Ejército)
        QPointF(470, 330), // Óvalo Quiñones / Yanahuara
        QPointF(550, 350), // Av. Goyeneche / Cercado
        QPointF(600, 370)  // Campus UNSA (Av. Venezuela)
    };

    // Ruta 2: T-14 Estructurante (Paucarpata - Miguel Grau ↔ Terminal Terrestre) - Verde
    m_routes[2].id = "T-14";
    m_routes[2].nombre = "Estructurante T-14";
    m_routes[2].unidadNegocio = "C-7 (Cotum / Cono Este)";
    m_routes[2].color = QColor("#10B981");
    m_routes[2].nodos = {
        QPointF(840, 370), // Miguel Grau (Paucarpata)
        QPointF(760, 410), // Mall Aventura Porongoche
        QPointF(660, 400), // Óvalo Mariscal Castilla
        QPointF(600, 370), // Puente Universitaria / UNSA
        QPointF(480, 430), // Terminal Terrestre / Av. Arturo Ibáñez
        QPointF(390, 490)  // Jacobo Hunter
    };

    // Ruta 3: A-5 Alimentadora (Cerro Colorado - Ciudad Municipal ↔ Río Seco / Avelino) - Ámbar
    m_routes[3].id = "A-5";
    m_routes[3].nombre = "Alimentadora A-5";
    m_routes[3].unidadNegocio = "C-3 (Cono Norte)";
    m_routes[3].color = QColor("#F59E0B");
    m_routes[3].nodos = {
        QPointF(140, 80),  // Ciudad Municipal (Cono Norte)
        QPointF(220, 120), // Estación Río Seco
        QPointF(310, 260), // Vía 54 / Cerro Colorado
        QPointF(350, 340), // Terminal Pesquero (Río Seco)
        QPointF(450, 360), // Umacollo / Estadio Melgar
        QPointF(550, 480)  // Plat. Andrés Avelino Cáceres
    };

    // Ruta 4: T-18 Estructurante (Miraflores - Alto Misti ↔ Parque Industrial) - Morado
    m_routes[4].id = "T-18";
    m_routes[4].nombre = "Estructurante T-18";
    m_routes[4].unidadNegocio = "C-4 (Miraflores)";
    m_routes[4].color = QColor("#8B5CF6");
    m_routes[4].nodos = {
        QPointF(710, 200), // Alto Misti (Miraflores)
        QPointF(650, 270), // Plaza Mayta Cápac
        QPointF(560, 320), // Calle Paz Soldán / Filtro
        QPointF(500, 390), // Av. Parra
        QPointF(440, 450)  // Parque Industrial
    };

    // Inicializar paraderos (paradas clave con posiciones fijas)
    m_stops.resize(10);
    m_stops[0] = {"Estación Norte Río Seco", QPointF(220, 120), false};
    m_stops[1] = {"Zamácola / Aeropuerto", QPointF(290, 190), false};
    m_stops[2] = {"Puente Añashuayco", QPointF(360, 250), false};
    m_stops[3] = {"Av. Ejército / Real Plaza", QPointF(440, 290), false};
    m_stops[4] = {"Puente Grau / Cercado", QPointF(510, 330), false};
    m_stops[5] = {"Av. Daniel Alcides Carrión / Hospital Honorio Delgado", QPointF(530, 410), false};
    m_stops[6] = {"Plat. Andrés Avelino Cáceres", QPointF(550, 480), false};
    m_stops[7] = {"Lara / Socabaya", QPointF(540, 610), false};
    m_stops[8] = {"Enace / Dean Valdivia (Cayma)", QPointF(460, 100), false};
    m_stops[9] = {"Campus UNSA (Av. Venezuela)", QPointF(600, 370), false};

    // Inicializar buses (2-3 unidades por ruta) con placas formatos Arequipa (empiezan con V)
    // Ruta C-1A: 3 buses
    m_buses.append(BusInfo{"V8A-912", "C-1A", "Carlos Mendoza", 0, 1, QPointF(220, 120), QPointF(220, 120)});
    m_buses.append(BusInfo{"V4T-751", "C-1A", "Luis Quispe", 0, 1, QPointF(290, 190), QPointF(290, 190)});
    m_buses.append(BusInfo{"V9M-703", "C-1A", "Rosa Flores", 0, 1, QPointF(360, 250), QPointF(360, 250)});

    // Ruta A-25: 2 buses
    m_buses.append(BusInfo{"V2B-819", "A-25", "Ana María", 0, 1, QPointF(460, 100), QPointF(460, 100)});
    m_buses.append(BusInfo{"V6K-405", "A-25", "José Carlos", 0, 1, QPointF(450, 180), QPointF(450, 180)});

    // Ruta T-14: 2 buses
    m_buses.append(BusInfo{"V87X-234", "T-14", "Miguel Ríos", 0, 1, QPointF(840, 370), QPointF(840, 370)});
    m_buses.append(BusInfo{"V3K-112", "T-14", "Fernanda", 0, 1, QPointF(760, 410), QPointF(760, 410)});

    // Ruta A-5: 2 buses
    m_buses.append(BusInfo{"V7M-551", "A-5", "Diego Álvarez", 0, 1, QPointF(140, 80), QPointF(140, 80)});
    m_buses.append(BusInfo{"V5N-333", "A-5", "Patricia Soto", 0, 1, QPointF(220, 120), QPointF(220, 120)});

    // Ruta T-18: 2 buses
    m_buses.append(BusInfo{"V6Q-890", "T-18", "Sara Mamani", 0, 1, QPointF(710, 200), QPointF(710, 200)});
    m_buses.append(BusInfo{"V4P-112", "T-18", "Luis Ortega", 0, 1, QPointF(650, 270), QPointF(650, 270)});

    // Estado visible de rutas (todos encendidos por defecto)
    for (int i = 0; i < 5; ++i) m_routeVisible[i] = true;

    // Configurar timer para actualización cada 50ms
    m_timerId = startTimer(50); // 50ms ≈ 20 FPS

    setMinimumSize(800, 560);
    setMaximumSize(1920, 1080);
}

ArequipaMapWidget::~ArequipaMapWidget()
{
    if (m_timerId != 0) {
        killTimer(m_timerId);
    }
}

void ArequipaMapWidget::setRouteVisible(const QString &routeId, bool visible)
{
    for (int i = 0; i < 5; ++i) {
        if (m_routes[i].id == routeId) {
            m_routeVisible[i] = visible;
            update();
            return;
        }
    }
}

bool ArequipaMapWidget::isRouteVisible(const QString &routeId) const
{
    for (int i = 0; i < 5; ++i) {
        if (m_routes[i].id == routeId) {
            return m_routeVisible[i];
        }
    }
    return false;
}

void ArequipaMapWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    // Actualizar posiciones de todos los buses
    for (BusInfo &bus : m_buses) {
        // Buscar la ruta correspondiente por código
        int routeIdx = -1;
        for (int r = 0; r < 5; ++r) {
            if (m_routes[r].id == bus.rutaCodigo) {
                routeIdx = r;
                break;
            }
        }

        if (routeIdx < 0 || routeIdx >= 5) continue;

        const RouteData &route = m_routes[routeIdx];
        int &pos = bus.posIndex; // índice del nodo actual
        const int numNodes = route.nodos.size();

        if (numNodes < 2) continue;

        // Mover al siguiente nodo en la dirección actual
        if (bus.direction == 1) {
            pos++;
            if (pos >= numNodes) {
                pos = numNodes - 2; // cambiar de dirección al final
                bus.direction = -1;
            }
        } else {
            pos--;
            if (pos <= 0) {
                pos = 1; // cambiar de dirección al inicio
                bus.direction = 1;
            }
        }

        // Posición actual: interpolación entre nodo[pos] y nodo[pos+1]
        QPointF startPos = route.nodos[pos];
        QPointF endPos = route.nodos[pos + 1];

        // Factor de progreso 0.5 (mitad del segmento para visualización)
        bus._posActual = startPos + (endPos - startPos) * 0.5;

        // Posición objetivo
        bus._posObjetivo = endPos;
    }

    update(); // repaint para redibujar
}

void ArequipaMapWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Escalado: el mapa base es 1000x700, escalar al tamaño del widget
    qreal scaleX = width() / 1000.0;
    qreal scaleY = height() / 700.0;
    painter.scale(scaleX, scaleY);

    // Fondo: azul pizarra oscuro
    painter.fillRect(0, 0, 1000, 700, QColor("#0F172A"));

    // Dibujar todos los elementos geográficos
    drawBackground(painter);
    drawVolcanoes(painter);
    drawRiver(painter);
    drawDistricts(painter);
    drawRoutes(painter);
    drawBuses(painter);
    drawStops(painter);
}

void ArequipaMapWidget::drawBackground(QPainter &painter)
{
    // Fondo base ya puesto con fillRect arriba; aquí podríamos añadir grid sutil si se desea
}

void ArequipaMapWidget::drawVolcanoes(QPainter &painter)
{
    // Chachani (noreste) - polígono decorativo sutil
    QPainterPath chachaniPath;
    chachaniPath.moveTo(420, 40);
    chachaniPath.cubicTo(490, 20, 530, 20, 560, 40);
    chachaniPath.cubicTo(530, 60, 490, 80, 420, 60);
    chachaniPath.closeSubpath();
    painter.setBrush(QColor(15, 23, 42, 100));
    painter.setPen(Qt::NoPen);
    painter.drawPath(chachaniPath);

    // Misti - polígono cónico
    QPainterPath mistiPath;
    mistiPath.moveTo(650, 35);
    mistiPath.lineTo(720, 35);
    mistiPath.lineTo(780, 35); // apex del cono
    mistiPath.lineTo(780, 80);
    mistiPath.lineTo(650, 80);
    mistiPath.closeSubpath();
    painter.setBrush(QColor(15, 23, 42, 150));
    painter.setPen(Qt::NoPen);
    painter.drawPath(mistiPath);

    // Etiqueta "Volcán Misti"
    painter.setFont(QFont("Arial", 8));
    painter.setPen(QColor(255, 255, 255, 180));
    painter.drawText(700, 50, "Volcán Misti");

    // Picchu Picchu
    QPainterPath picchuPath;
    picchuPath.moveTo(820, 60);
    picchuPath.cubicTo(880, 40, 920, 40, 950, 60);
    picchuPath.cubicTo(920, 80, 880, 80, 820, 80);
    picchuPath.closeSubpath();
    painter.setBrush(QColor(15, 23, 42, 100));
    painter.setPen(Qt::NoPen);
    painter.drawPath(picchuPath);
}

void ArequipaMapWidget::drawRiver(QPainter &painter)
{
    painter.setPen(QPen(QColor("#38BDF8"), 14, Qt::SolidLine, Qt::RoundCap));
    // El río Chili cruza la ciudad de Noreste a Suroeste
    QPainterPath riverPath;
    riverPath.moveTo(630, 80);
    riverPath.cubicTo(540, 220, 490, 340, 430, 470);
    riverPath.cubicTo(310, 650, 250, 650, 200, 650);
    painter.drawPath(riverPath);
}

void ArequipaMapWidget::drawDistricts(QPainter &painter)
{
    QFont font("Arial", 10);
    painter.setFont(font);

    QColor bgColor(15, 23, 42, 200); // azul pizarra con opacidad

    // CERRO COLORADO en (230, 140)
    painter.fillRect(QRectF(220, 130, 200, 25), bgColor);
    painter.setPen(Qt::white);
    painter.drawText(QRectF(220, 130, 200, 25), Qt::AlignLeft, "CERRO COLORADO");

    // CAYMA en (450, 130)
    painter.fillRect(QRectF(440, 120, 180, 25), bgColor);
    painter.drawText(QRectF(440, 120, 180, 25), Qt::AlignLeft, "CAYMA");

    // YANAHUARA en (410, 260)
    painter.fillRect(QRectF(400, 250, 200, 25), bgColor);
    painter.drawText(QRectF(400, 250, 200, 25), Qt::AlignLeft, "YANAHUARA");

    // CERCADO (CENTRO HISTÓRICO) en (530, 340)
    painter.fillRect(QRectF(520, 330, 250, 25), bgColor);
    painter.drawText(QRectF(520, 330, 250, 25), Qt::AlignLeft, "CERCADO (CENTRO HISTÓRICO)");

    // MIRAFLORES en (680, 270)
    painter.fillRect(QRectF(670, 260, 180, 25), bgColor);
    painter.drawText(QRectF(670, 260, 180, 25), Qt::AlignLeft, "MIRAFLORES");

    // PAUCARPATA en (780, 430)
    painter.fillRect(QRectF(770, 420, 200, 25), bgColor);
    painter.drawText(QRectF(770, 420, 200, 25), Qt::AlignLeft, "PAUCARPATA");

    // J.L. BUSTAMANTE Y RIVERO en (570, 490)
    painter.fillRect(QRectF(560, 480, 250, 25), bgColor);
    painter.drawText(QRectF(560, 480, 250, 25), Qt::AlignLeft, "J.L. BUSTAMANTE Y RIVERO");

    // HUNTER en (380, 530)
    painter.fillRect(QRectF(370, 520, 180, 25), bgColor);
    painter.drawText(QRectF(370, 520, 180, 25), Qt::AlignLeft, "HUNTER");

    // SOCABAYA en (540, 630)
    painter.fillRect(QRectF(530, 620, 200, 25), bgColor);
    painter.drawText(QRectF(530, 620, 200, 25), Qt::AlignLeft, "SOCABAYA");
}

void ArequipaMapWidget::drawRoutes(QPainter &painter)
{
    QPen routePen;
    routePen.setWidth(4);
    routePen.setCapStyle(Qt::RoundCap);
    routePen.setJoinStyle(Qt::RoundJoin);

    for (int i = 0; i < 5; ++i) {
        if (!m_routeVisible[i]) continue;

        const RouteData &route = m_routes[i];
        routePen.setColor(route.color);
        painter.setPen(routePen);
        painter.setBrush(Qt::NoBrush);

        // Dibujar línea a través de nodos
        if (route.nodos.size() >= 2) {
            for (int j = 0; j < route.nodos.size() - 1; ++j) {
                QPointF p1 = route.nodos[j];
                QPointF p2 = route.nodos[j + 1];
                painter.drawLine(p1, p2);

                // Círculo en cada nodo (paradero) con radio 6px
                painter.setBrush(route.color);
                painter.setPen(Qt::white);
                painter.drawEllipse(p1, 6, 6);

                // Etiqueta del paradero (fijo) debajo del círculo
                QString nombreParadero;
                if (j == 0) nombreParadero = "Estación Norte Río Seco";
                else if (j == 1) nombreParadero = "Zamácola / Aeropuerto";
                else if (j == 2) nombreParadero = "Puente Añashuayco";
                else if (j == 3) nombreParadero = "Av. Ejército / Real Plaza";
                else if (j == 4) nombreParadero = "Puente Grau / Cercado";
                else if (j == 5) nombreParadero = "Av. Daniel Alcides Carrión";
                else if (j == 6) nombreParadero = "Plat. Andrés Avelino Cáceres";
                else if (j == 7) nombreParadero = "Lara / Socabaya";

                if (!nombreParadero.isEmpty()) {
                    QFontMetrics fm(QFont("Arial", 7));
                    QRectF rect = fm.boundingRect(nombreParadero);
                    QSizeF ts = rect.size();
                    QRectF labelRect(p1.x() - ts.width() / 2 - 1,
                                     p1.y() + 8, // abajo del círculo
                                     ts.width() + 2, ts.height());
                    painter.setBrush(QColor(0, 0, 0, 180));
                    painter.setPen(Qt::NoPen);
                    painter.drawRoundedRect(labelRect, 2, 2);
                    painter.setPen(Qt::white);
                    painter.setFont(QFont("Arial", 7));
                    painter.drawText(labelRect, Qt::AlignCenter, nombreParadero);
                }
            }
        }
    }
}

void ArequipaMapWidget::drawBuses(QPainter &painter)
{
    for (const BusInfo &bus : m_buses) {
        // Buscar ruta por código
        int routeIdx = -1;
        for (int r = 0; r < 5; ++r) {
            if (m_routes[r].id == bus.rutaCodigo) {
                routeIdx = r;
                break;
            }
        }
        if (routeIdx < 0 || routeIdx >= 5) continue;

        const RouteData &route = m_routes[routeIdx];
        int pos = bus.posIndex;
        int numNodes = route.nodos.size();

        if (numNodes < 2) continue;

        QPointF startPos = route.nodos[pos];
        QPointF endPos = route.nodos[(pos + 1) % numNodes];

        // Posición interpolada (50% del segmento)
        QPointF posActual = startPos + (endPos - startPos) * 0.5;

        // Determinar color basado en la ruta
        QColor busColor = route.color;
        if (busColor.isValid()) {
            // Brillo para el bus
            painter.setBrush(QColor(255, 255, 255)); // blanco brillante
            painter.setPen(QPen(busColor, 2)); // borde con color de ruta

            // Etiqueta flotante: "[C-1A] V8A-912"
            QString label = QString("[%1] %2").arg(bus.rutaCodigo).arg(bus.placa);

            QFontMetrics fm(QFont("Arial", 7));
            QRectF rect = fm.boundingRect(label);
            QSizeF textSize = rect.size();
            QRectF labelRect(posActual.x() - textSize.width() / 2 - 2,
                              posActual.y() - textSize.height() - 8,
                              textSize.width() + 4, textSize.height() + 4);

            // Fondo de la etiqueta
            painter.setBrush(QColor(0, 0, 0, 180));
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(labelRect, 3, 3);

            // Texto de la etiqueta
            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 7));
            painter.drawText(labelRect, Qt::AlignCenter, label);
        } else {
            // Sin color, usar blanco
            painter.setBrush(QColor(255, 255, 255));
            painter.setPen(QPen(Qt::white, 2));

            QString label = QString("[%1] %2").arg(bus.rutaCodigo).arg(bus.placa);

            QFontMetrics fm(QFont("Arial", 7));
            QRectF rect = fm.boundingRect(label);
            QSizeF textSize = rect.size();
            QRectF labelRect(posActual.x() - textSize.width() / 2 - 2,
                              posActual.y() - textSize.height() - 8,
                              textSize.width() + 4, textSize.height() + 4);

            painter.setBrush(QColor(0, 0, 0, 180));
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(labelRect, 3, 3);

            painter.setPen(Qt::white);
            painter.setFont(QFont("Arial", 7));
            painter.drawText(labelRect, Qt::AlignCenter, label);
        }

        // Dibujar el bus - punto circular brillante r=8px
        painter.drawEllipse(posActual, 8, 8);

        // Borde extra blanco
        painter.setPen(QPen(Qt::white, 2));
        painter.drawEllipse(posActual, 8, 8);
    }
}

void ArequipaMapWidget::drawStops(QPainter &painter)
{
    QPen stopPen;
    stopPen.setColor(QColor("#EF4444"));
    stopPen.setWidth(2);
    stopPen.setCapStyle(Qt::RoundCap);

    for (const StopInfo &stop : m_stops) {
        // Círculo del paradero
        painter.setPen(stopPen);
        painter.setBrush(QColor("#F8FAFC"));
        painter.drawEllipse(stop.pos, 6, 6);

        // Nombre del paradero con fondito detrás
        painter.setFont(QFont("Arial", 7));
        painter.setPen(Qt::black);

        QFontMetrics fm(QFont("Arial", 7));
            QRectF rect = fm.boundingRect(stop.nombre);
            QSizeF ts = rect.size();
        QRectF textRect(stop.pos.x() - ts.width() / 2 - 1,
                        stop.pos.y() - ts.height() - 2,
                        ts.width() + 2, ts.height() + 2);

        painter.setBrush(QColor(255, 255, 255, 220)); // blanco semi-transparente
        painter.drawRoundedRect(textRect, 2, 2);

        painter.setPen(Qt::black);
        painter.drawText(textRect, Qt::AlignCenter, stop.nombre);
    }
}

void ArequipaMapWidget::mousePressEvent(QMouseEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    qreal scaleX = width() / 1000.0;
    qreal scaleY = height() / 700.0;

    // Convertir evento a coordenadas normalizadas (1000x700)
    QPointF normalizedEvent(event->position().x() / scaleX, event->position().y() / scaleY);

    // Verificar clic en un bus
    for (const BusInfo &bus : m_buses) {
        int routeIdx = -1;
        for (int r = 0; r < 5; ++r) {
            if (m_routes[r].id == bus.rutaCodigo) {
                routeIdx = r;
                break;
            }
        }
        if (routeIdx < 0) continue;

        const RouteData &route = m_routes[routeIdx];
        int pos = bus.posIndex;
        int numNodes = route.nodos.size();

        if (numNodes < 2) continue;

        QPointF startPos = route.nodos[pos];
        QPointF endPos = route.nodos[(pos + 1) % numNodes];
        QPointF posActual = startPos + (endPos - startPos) * 0.5;

        // Distancia al centro del bus
        qreal dist = (normalizedEvent - posActual).manhattanLength();
        if (dist < 16) { // radio 8px + tolerancia
            emit unitClicked(bus.placa, bus.conductor, bus.rutaCodigo);
            return;
        }
    }

    // Verificar clic en un paradero
    for (const StopInfo &stop : m_stops) {
        qreal dist = (QPointF(event->position().x() / scaleX, event->position().y() / scaleY) - stop.pos).manhattanLength();
        if (dist < 16) {
            emit stopClicked(stop.nombre);
            return;
        }
    }

    QWidget::mousePressEvent(event);
}