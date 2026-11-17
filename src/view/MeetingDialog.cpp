#include "view/MeetingDialog.h"
#include <QCloseEvent>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QVBoxLayout>

MeetingDialog::MeetingDialog(const QString& peerName, QWidget* parent)
    : QDialog(parent), preview_(new QProcess(this)) {
    setWindowTitle(tr("Reunión con %1").arg(peerName.isEmpty() ? tr("Administrador") : peerName));
    resize(520, 360);

    auto* layout = new QVBoxLayout(this);
    status_ = new QLabel(tr("Iniciando cámara y micrófono..."), this);
    status_->setProperty("role", "subtitle");
    status_->setWordWrap(true);
    layout->addWidget(status_);

    auto* endButton = new QPushButton(tr("Finalizar llamada"), this);
    endButton->setProperty("variant", "danger");
    layout->addStretch(1);
    layout->addWidget(endButton, 0, Qt::AlignRight);
    connect(endButton, &QPushButton::clicked, this, &QDialog::reject);

    status_->setText(tr("Cámara: vista previa local | Micrófono: %1").arg(microphoneStatus()));
    startPreview();
}

MeetingDialog::~MeetingDialog() {
    stopPreview();
}

void MeetingDialog::startPreview() {
    const QStringList gst = {
        QStringLiteral("v4l2src"), QStringLiteral("device=/dev/video0"), QStringLiteral("!"),
        QStringLiteral("videoconvert"), QStringLiteral("!"), QStringLiteral("autovideosink")};
    preview_->start(QStringLiteral("gst-launch-1.0"), gst);
    if (preview_->waitForStarted(1500)) return;

    const QStringList ffmpeg = {
        QStringLiteral("-f"), QStringLiteral("v4l2"), QStringLiteral("-i"),
        QStringLiteral("/dev/video0"), QStringLiteral("-autoexit")};
    preview_->start(QStringLiteral("ffplay"), ffmpeg);
    if (preview_->waitForStarted(1500)) return;

    status_->setText(tr("Cámara: no disponible | Micrófono: %1").arg(microphoneStatus()));
}

void MeetingDialog::stopPreview() {
    if (preview_->state() == QProcess::NotRunning) return;
    preview_->terminate();
    if (!preview_->waitForFinished(1500)) preview_->kill();
}

QString MeetingDialog::microphoneStatus() const {
    QProcess probe;
    probe.start(QStringLiteral("sh"),
                {QStringLiteral("-c"), QStringLiteral("[ -e /dev/snd/controlC0 ] && echo yes || echo no")});
    if (probe.waitForFinished(1000) && QString::fromUtf8(probe.readAllStandardOutput()).trimmed() == QStringLiteral("yes"))
        return tr("dispositivo por defecto activo");
    return tr("sin dispositivo detectado");
}

void MeetingDialog::closeEvent(QCloseEvent* event) {
    stopPreview();
    QDialog::closeEvent(event);
}
