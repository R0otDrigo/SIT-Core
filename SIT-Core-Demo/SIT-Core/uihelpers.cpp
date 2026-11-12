#include "uihelpers.h"
#include "theme.h"

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QScrollArea>
#include <QTableWidget>
#include <QVBoxLayout>

namespace UI {

QFrame *card(QWidget *parent)
{
    auto *f = new QFrame(parent);
    f->setObjectName("card");
    return f;
}

QLabel *label(const QString &text, const char *objectName)
{
    auto *l = new QLabel(text);
    if (objectName)
        l->setObjectName(QString::fromLatin1(objectName));
    return l;
}

QPushButton *button(const QString &text, const char *objectName)
{
    auto *b = new QPushButton(text);
    b->setCursor(Qt::PointingHandCursor);
    if (objectName)
        b->setObjectName(QString::fromLatin1(objectName));
    return b;
}

QWidget *statCard(const QString &title, const QString &value,
                  const QString &sub, const QString &color)
{
    QFrame *c = card();
    auto *h = new QHBoxLayout(c);
    h->setContentsMargins(14, 14, 16, 14);
    h->setSpacing(12);

    auto *bar = new QFrame(c);
    bar->setFixedWidth(5);
    bar->setStyleSheet(QStringLiteral("background:%1; border:none; border-radius:2px;").arg(color));
    h->addWidget(bar);

    auto *v = new QVBoxLayout;
    v->setSpacing(2);
    v->addWidget(label(title.toUpper(), "statTitle"));
    QLabel *val = label(value, "statValue");
    val->setStyleSheet(QStringLiteral("color:%1;").arg(color));
    v->addWidget(val);
    v->addWidget(label(sub, "muted"));
    h->addLayout(v, 1);

    c->setMinimumWidth(180);
    return c;
}

QWidget *panel(const QString &title, QWidget *content, QWidget *headerExtra)
{
    QFrame *c = card();
    auto *v = new QVBoxLayout(c);
    v->setContentsMargins(18, 16, 18, 18);
    v->setSpacing(12);

    auto *head = new QHBoxLayout;
    head->addWidget(label(title, "h2"));
    head->addStretch();
    if (headerExtra)
        head->addWidget(headerExtra);
    v->addLayout(head);
    v->addWidget(content);
    return c;
}

QWidget *meter(const QString &caption, int value, const QString &color, const QString &valueText)
{
    auto *w = new QWidget;
    auto *v = new QVBoxLayout(w);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(5);

    auto *top = new QHBoxLayout;
    top->addWidget(label(caption));
    top->addStretch();
    QLabel *val = label(valueText.isEmpty() ? QStringLiteral("%1%").arg(value) : valueText);
    val->setStyleSheet(QStringLiteral("font-weight:600;"));
    top->addWidget(val);
    v->addLayout(top);

    auto *bar = new QProgressBar;
    bar->setRange(0, 100);
    bar->setValue(value);
    bar->setTextVisible(false);
    bar->setStyleSheet(QStringLiteral("QProgressBar::chunk{background:%1;}").arg(color));
    v->addWidget(bar);
    return w;
}

QWidget *alertItem(const QString &text, const QString &when, const QString &color)
{
    auto *w = new QWidget;
    auto *h = new QHBoxLayout(w);
    h->setContentsMargins(0, 4, 0, 4);
    h->setSpacing(10);

    auto *dot = new QFrame;
    dot->setFixedSize(10, 10);
    dot->setStyleSheet(QStringLiteral("background:%1; border:none; border-radius:5px;").arg(color));
    h->addWidget(dot, 0, Qt::AlignVCenter);

    auto *t = label(text);
    t->setWordWrap(true);
    h->addWidget(t, 1);
    h->addWidget(label(when, "muted"));
    return w;
}

QString statusColor(const QString &status)
{
    const QString s = status.trimmed().toLower();
    static const QStringList green = { "en ruta", "operativo", "activo", "disponible", "a tiempo",
                                       "completado", "confirmado", "delegada", "llegó", "en curso" };
    static const QStringList amber = { "retraso", "retrasado", "alerta", "mantenimiento",
                                       "pendiente", "programado", "por salir", "en revisión" };
    static const QStringList red   = { "detenido", "fallo", "falla", "cancelado", "incidencia",
                                       "inactivo", "suspendido" };
    for (const QString &g : green) if (s == g) return Theme::Green;
    for (const QString &a : amber) if (s == a) return Theme::Amber;
    for (const QString &r : red)   if (s == r) return Theme::Red;
    return Theme::Muted;
}

void fitTable(QTableWidget *t)
{
    t->setFixedHeight(38 + t->rowCount() * 40 + 4);
}

int appendRow(QTableWidget *t, const QStringList &cells)
{
    const int row = t->rowCount();
    const int statusCol = t->property("statusCol").toInt();
    t->insertRow(row);
    for (int c = 0; c < cells.size() && c < t->columnCount(); ++c) {
        auto *item = new QTableWidgetItem;
        if (c == statusCol) {
            item->setText(QStringLiteral("● ") + cells.at(c));
            item->setForeground(QColor(statusColor(cells.at(c))));
            QFont f = item->font();
            f.setBold(true);
            item->setFont(f);
        } else {
            item->setText(cells.at(c));
        }
        t->setItem(row, c, item);
    }
    fitTable(t);
    return row;
}

QTableWidget *makeTable(const QStringList &headers, const QList<QStringList> &rows, int statusCol)
{
    auto *t = new QTableWidget(0, headers.size());
    t->setProperty("statusCol", statusCol);
    t->setHorizontalHeaderLabels(headers);
    t->verticalHeader()->setVisible(false);
    t->verticalHeader()->setDefaultSectionSize(40);
    t->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    t->horizontalHeader()->setHighlightSections(false);
    t->horizontalHeader()->setFixedHeight(38);
    t->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setSelectionMode(QAbstractItemView::SingleSelection);
    t->setShowGrid(false);
    t->setFocusPolicy(Qt::NoFocus);
    t->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    t->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    for (const QStringList &r : rows)
        appendRow(t, r);
    fitTable(t);
    return t;
}

QHBoxLayout *hbox(const QList<QWidget *> &widgets, int spacing)
{
    auto *h = new QHBoxLayout;
    h->setSpacing(spacing);
    for (QWidget *w : widgets)
        h->addWidget(w, 1);
    return h;
}

QWidget *pageWithScroll(QWidget *content)
{
    auto *scroll = new QScrollArea;
    scroll->setObjectName("pageScroll");
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto *wrap = new QWidget;
    wrap->setObjectName("pageContent");
    auto *l = new QVBoxLayout(wrap);
    l->setContentsMargins(24, 24, 24, 24);
    l->addWidget(content);
    l->addStretch();
    scroll->setWidget(wrap);
    return scroll;
}

bool formDialog(QWidget *parent, const QString &title,
                const QList<FormField> &fields, QStringList &values)
{
    QDialog dlg(parent);
    dlg.setWindowTitle(title);
    dlg.setMinimumWidth(380);
    dlg.setStyleSheet(QStringLiteral("QDialog{background:%1;}").arg(Theme::Card));

    auto *v = new QVBoxLayout(&dlg);
    v->setContentsMargins(22, 20, 22, 18);
    v->setSpacing(14);
    v->addWidget(label(title, "h2"));

    auto *form = new QFormLayout;
    form->setVerticalSpacing(10);
    QList<QWidget *> editors;
    for (const FormField &f : fields) {
        QWidget *w = nullptr;
        if (!f.options.isEmpty()) {
            auto *cb = new QComboBox;
            cb->addItems(f.options);
            w = cb;
        } else {
            auto *le = new QLineEdit;
            le->setPlaceholderText(f.placeholder);
            w = le;
        }
        editors << w;
        form->addRow(f.label, w);
    }
    v->addLayout(form);

    auto *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    box->button(QDialogButtonBox::Ok)->setText(QStringLiteral("Guardar"));
    box->button(QDialogButtonBox::Cancel)->setText(QStringLiteral("Cancelar"));
    box->button(QDialogButtonBox::Cancel)->setObjectName("ghost");
    QObject::connect(box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    QObject::connect(box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    v->addWidget(box);

    if (dlg.exec() != QDialog::Accepted)
        return false;

    values.clear();
    for (QWidget *w : editors) {
        if (auto *cb = qobject_cast<QComboBox *>(w))
            values << cb->currentText();
        else if (auto *le = qobject_cast<QLineEdit *>(w))
            values << le->text().trimmed();
    }
    return true;
}

} // namespace UI
