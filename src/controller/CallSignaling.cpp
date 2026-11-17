#include "controller/CallSignaling.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QHostAddress>
#include <QNetworkDatagram>

CallSignaling::CallSignaling(Role role, QObject* parent,
                             quint16 listenPort, quint16 targetPort)
    : QObject(parent), role_(role) {
    if (role_ == Role::Admin) {
        listenPort_ = listenPort ? listenPort : 45454;
        targetPort_ = targetPort ? targetPort : 45455;
    } else {
        listenPort_ = listenPort ? listenPort : 45455;
        targetPort_ = targetPort ? targetPort : 45454;
    }
    connect(&receiver_, &QUdpSocket::readyRead, this, &CallSignaling::onReadyRead);
}

bool CallSignaling::start(quint16 port) {
    if (port) listenPort_ = port;
    if (receiver_.state() == QUdpSocket::BoundState) return true;
    const auto flags = QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint;
    if (!receiver_.bind(QHostAddress::AnyIPv4, listenPort_, flags)) {
        emit errorOccurred(receiver_.errorString());
        return false;
    }
    return true;
}

void CallSignaling::stop() {
    receiver_.close();
    sender_.close();
}

void CallSignaling::sendCall(const QString& uid, const QString& vehicleId,
                             const QString& displayName, const QString& status) {
    sendCallTo(QHostAddress(QHostAddress::Broadcast), uid, vehicleId, displayName, status);
}

void CallSignaling::sendCallTo(const QHostAddress& target, const QString& uid,
                               const QString& vehicleId, const QString& displayName,
                               const QString& status) {
    QJsonObject payload;
    payload["type"] = QStringLiteral("call");
    payload["from"] = roleString();
    payload["uid"] = uid;
    payload["vehicleId"] = vehicleId;
    payload["displayName"] = displayName;
    payload["status"] = status;

    const QByteArray datagram = QJsonDocument(payload).toJson(QJsonDocument::Compact);
    const qint64 written = sender_.writeDatagram(datagram, target, targetPort_);
    if (written < 0) emit errorOccurred(sender_.errorString());
}

void CallSignaling::onReadyRead() {
    while (receiver_.hasPendingDatagrams()) {
        const QNetworkDatagram datagram = receiver_.receiveDatagram();
        const QJsonDocument doc = QJsonDocument::fromJson(datagram.data());
        if (!doc.isObject()) continue;

        const QJsonObject obj = doc.object();
        if (obj.value("type").toString() != QStringLiteral("call")) continue;
        if (obj.value("from").toString() == roleString()) continue;

        CallInfo info;
        info.type = obj.value("type").toString();
        info.from = obj.value("from").toString();
        info.uid = obj.value("uid").toString();
        info.vehicleId = obj.value("vehicleId").toString();
        info.displayName = obj.value("displayName").toString();
        info.status = obj.value("status").toString();
        emit messageReceived(info);
    }
}

QString CallSignaling::roleString() const {
    return role_ == Role::Admin ? QStringLiteral("admin") : QStringLiteral("user");
}
