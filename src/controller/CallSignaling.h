#pragma once
#include <QObject>
#include <QHostAddress>
#include <QUdpSocket>
#include <QString>

struct CallInfo {
    QString type;
    QString from;
    QString uid;
    QString vehicleId;
    QString displayName;
    QString status;
};

class CallSignaling : public QObject {
    Q_OBJECT
public:
    enum class Role { User, Admin };

    explicit CallSignaling(Role role, QObject* parent = nullptr,
                           quint16 listenPort = 0, quint16 targetPort = 0);

    bool start(quint16 port = 0);
    void stop();

public slots:
    void sendCall(const QString& uid, const QString& vehicleId,
                  const QString& displayName, const QString& status = QString());
    void sendCallTo(const QHostAddress& target, const QString& uid,
                    const QString& vehicleId, const QString& displayName,
                    const QString& status = QString());

signals:
    void messageReceived(const CallInfo& info);
    void errorOccurred(const QString& message);

private slots:
    void onReadyRead();

private:
    QString roleString() const;

    Role role_{Role::User};
    quint16 listenPort_{45455};
    quint16 targetPort_{45454};
    QUdpSocket receiver_;
    QUdpSocket sender_;
};
