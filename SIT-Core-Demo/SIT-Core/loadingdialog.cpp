#include "loadingdialog.h"
#include "theme.h"

#include <QEasingCurve>
#include <QLabel>
#include <QPainter>
#include <QProgressBar>
#include <QShowEvent>
#include <QTimer>
#include <QVBoxLayout>

#ifdef SIT_QML_LOADING
#include <QQuickWidget>
#include <QQuickItem>
#include <QUrl>
#endif

namespace {

// Spinner pintado con QPainter (arco giratorio sobre un aro gris).
class SpinnerWidget : public QWidget
{
public:
    explicit SpinnerWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        setFixedSize(72, 72);
        m_timer.setInterval(16);
        QObject::connect(&m_timer, &QTimer::timeout, this, [this]() {
            m_angle = (m_angle + 7) % 360;
            update();
        });
        m_timer.start();
    }

protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        const QRectF r = QRectF(rect()).adjusted(7, 7, -7, -7);
        QPen pen(QColor(Theme::Border), 7);
        pen.setCapStyle(Qt::RoundCap);
        p.setPen(pen);
        p.drawEllipse(r);
        pen.setColor(QColor(Theme::Accent));
        p.setPen(pen);
        p.drawArc(r, -m_angle * 16, -110 * 16);
    }

private:
    QTimer m_timer;
    int m_angle = 0;
};

} // namespace

LoadingDialog::LoadingDialog(const QString &message, const QString &detail,
                             int durationMs, QWidget *parent)
    : QDialog(parent)
{
    setModal(true);
    setWindowFlag(Qt::FramelessWindowHint, true);
    setWindowTitle(tr("Cargando"));
    setFixedSize(360, 220);

    const QString msg = message.isEmpty() ? tr("Cargando...") : message;

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);

    bool usingQml = false;
#ifdef SIT_QML_LOADING
    {
        auto *qw = new QQuickWidget(this);
        qw->setResizeMode(QQuickWidget::SizeRootObjectToView);
        qw->setSource(QUrl(QStringLiteral("qrc:/cargando.qml")));
        if (qw->status() == QQuickWidget::Ready && qw->rootObject()) {
            m_qmlRoot = qw->rootObject();
            m_qmlRoot->setProperty("message", msg);
            m_qmlRoot->setProperty("detail", detail);
            outer->addWidget(qw);
            usingQml = true;
        } else {
            delete qw; // sin QtQuick disponible: se usa el spinner en C++
        }
    }
#endif

    if (!usingQml) {
        auto *card = new QFrame(this);
        card->setObjectName("loadingCard");
        auto *v = new QVBoxLayout(card);
        v->setContentsMargins(36, 24, 36, 28);
        v->setSpacing(10);

        auto *spinner = new SpinnerWidget(card);
        v->addWidget(spinner, 0, Qt::AlignHCenter);

        auto *title = new QLabel(msg, card);
        title->setObjectName("loadingTitle");
        title->setAlignment(Qt::AlignCenter);
        v->addWidget(title);

        auto *sub = new QLabel(detail, card);
        sub->setObjectName("muted");
        sub->setAlignment(Qt::AlignCenter);
        sub->setVisible(!detail.isEmpty());
        v->addWidget(sub);

        m_bar = new QProgressBar(card);
        m_bar->setRange(0, 100);
        m_bar->setTextVisible(false);
        v->addWidget(m_bar);

        outer->addWidget(card);
    }

    m_anim.setStartValue(0.0);
    m_anim.setEndValue(1.0);
    m_anim.setDuration(durationMs);
    m_anim.setEasingCurve(QEasingCurve::InOutQuad);
    connect(&m_anim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        const double p = v.toDouble();
        if (m_bar) m_bar->setValue(int(p * 100));
        if (m_qmlRoot) m_qmlRoot->setProperty("progress", p);
    });
    connect(&m_anim, &QVariantAnimation::finished, this, &QDialog::accept);
}

void LoadingDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    if (QWidget *p = parentWidget()) {
        const QPoint c = p->mapToGlobal(p->rect().center());
        move(c - QPoint(width() / 2, height() / 2));
    }
    if (!m_started) {
        m_started = true;
        m_anim.start();
    }
}

void LoadingDialog::run(QWidget *parent, const QString &message,
                        const QString &detail, int durationMs)
{
    LoadingDialog dlg(message, detail, durationMs, parent);
    dlg.exec();
}
