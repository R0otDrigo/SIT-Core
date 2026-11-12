#pragma once
#include <QList>
#include <QString>
#include <QStringList>

class QFrame;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QTableWidget;
class QWidget;

// Utilidades para construir las interfaces de los dashboards por código.
namespace UI {

struct FormField {
    QString label;
    QStringList options;      // si no está vacío -> QComboBox; si no -> QLineEdit
    QString placeholder;
};

QFrame *card(QWidget *parent = nullptr);
QLabel *label(const QString &text, const char *objectName = nullptr);
QPushButton *button(const QString &text, const char *objectName = nullptr);

// Tarjeta de métrica (título, valor grande de color, subtítulo).
QWidget *statCard(const QString &title, const QString &value,
                  const QString &sub, const QString &color);
// Tarjeta con título (h2) y contenido.
QWidget *panel(const QString &title, QWidget *content, QWidget *headerExtra = nullptr);
// Barra de progreso con leyenda.
QWidget *meter(const QString &caption, int value, const QString &color,
               const QString &valueText = QString());
// Ítem de alerta: punto de color + texto + hora.
QWidget *alertItem(const QString &text, const QString &when, const QString &color);

// Color asociado a un estado ("En ruta" -> verde, "Retraso" -> ámbar, ...).
QString statusColor(const QString &status);

QTableWidget *makeTable(const QStringList &headers, const QList<QStringList> &rows,
                        int statusCol = -1);
int appendRow(QTableWidget *table, const QStringList &cells);
void fitTable(QTableWidget *table);

QHBoxLayout *hbox(const QList<QWidget *> &widgets, int spacing = 16);
// Envuelve un contenido en un QScrollArea con márgenes y fondo del tema.
QWidget *pageWithScroll(QWidget *content);

// Diálogo de formulario sencillo. Devuelve true si se aceptó.
bool formDialog(QWidget *parent, const QString &title,
                const QList<FormField> &fields, QStringList &values);

} // namespace UI
