#include "controller/UnitRegistry.h"
#include <QCoreApplication>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QUrl>
#include <QUrlQuery>

UnitRegistry::UnitRegistry(QObject* parent)
    : QObject(parent), network_(new QNetworkAccessManager(this)) {
    readConfig();
}

void UnitRegistry::readConfig() {
    const QString path = QDir(QCoreApplication::applicationDirPath()).filePath("config.ini");
    QSettings settings(path, QSettings::IniFormat);
    projectId_ = settings.value("FIREBASE_PROJECT_ID").toString();
}

void UnitRegistry::configure(const QString& projectId, const QString& idToken) {
    projectId_ = projectId;
    idToken_ = idToken;
}

bool UnitRegistry::ready(QString* error) const {
    if (projectId_.isEmpty()) {
        if (error) *error = tr("FIREBASE_PROJECT_ID no configurado");
        return false;
    }
    if (idToken_.isEmpty()) {
        if (error) *error = tr("Sesion de Firebase no disponible");
        return false;
    }
    return true;
}

QNetworkRequest UnitRegistry::requestFor(const QString& path) const {
    QUrl url(QStringLiteral("https://firestore.googleapis.com/v1/projects/%1/databases/(default)/documents/units%2")
                 .arg(projectId_, path));
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", QByteArray("Bearer ") + idToken_.toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    return request;
}

void UnitRegistry::loadUnits() {
    QString error;
    if (!ready(&error)) {
        emit errorOccurred(error);
        return;
    }

    QNetworkReply* reply = network_->get(requestFor(QString()));
    connect(reply, &QNetworkReply::finished, this, &UnitRegistry::onLoadFinished);
}

void UnitRegistry::onLoadFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(tr("No se pudieron leer las unidades: %1").arg(reply->errorString()));
        return;
    }

    const QJsonObject root = QJsonDocument::fromJson(reply->readAll()).object();
    const QJsonArray documents = root.value("documents").toArray();
    QVector<UnitInfo> parsed;
    parsed.reserve(documents.size());

    for (const QJsonValue& value : documents) {
        const QJsonObject doc = value.toObject();
        const QJsonObject fields = doc.value("fields").toObject();
        UnitInfo unit;
        unit.id = doc.value("name").toString().section('/', -1);
        unit.x = fields.value("X").toObject().value("integerValue").toString().toLongLong();
        unit.y = fields.value("Y").toObject().value("integerValue").toString().toLongLong();
        unit.driverUid = fields.value("driver").toObject().value("referenceValue").toString().section('/', -1);
        if (unit.driverUid.isEmpty())
            unit.driverUid = fields.value("driver").toObject().value("stringValue").toString();
        unit.status = fields.value("status").toObject().value("stringValue").toString();
        parsed.append(unit);
    }

    units_ = parsed;
    emit unitsLoaded(units_);
}

void UnitRegistry::registerUnit(const QString& unitId, const QString& driverUid,
                                qint64 x, qint64 y, const QString& status) {
    QString error;
    if (!ready(&error)) {
        emit errorOccurred(error);
        return;
    }
    if (unitId.isEmpty() || driverUid.isEmpty()) return;

    QUrlQuery query;
    for (const QString& field : {QStringLiteral("X"), QStringLiteral("Y"),
                                 QStringLiteral("driver"), QStringLiteral("status")})
        query.addQueryItem(QStringLiteral("updateMask.fieldPaths"), field);

    QNetworkRequest request = requestFor(QStringLiteral("/%1?%2").arg(unitId, query.query()));

    QJsonObject fields;
    fields["X"] = QJsonObject{{QStringLiteral("integerValue"), QString::number(x)}};
    fields["Y"] = QJsonObject{{QStringLiteral("integerValue"), QString::number(y)}};
    fields["driver"] = QJsonObject{{QStringLiteral("referenceValue"),
        QStringLiteral("projects/%1/databases/(default)/documents/users/%2").arg(projectId_, driverUid)}};
    fields["status"] = QJsonObject{{QStringLiteral("stringValue"), status}};

    QNetworkReply* reply = network_->sendCustomRequest(request, "PATCH",
        QJsonDocument(QJsonObject{{QStringLiteral("fields"), fields}}).toJson());
    connect(reply, &QNetworkReply::finished, this, &UnitRegistry::onWriteFinished);
}

void UnitRegistry::setStatus(const QString& unitId, const QString& status) {
    QString error;
    if (!ready(&error)) {
        emit errorOccurred(error);
        return;
    }
    if (unitId.isEmpty()) return;

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("updateMask.fieldPaths"), QStringLiteral("status"));
    QNetworkRequest request = requestFor(QStringLiteral("/%1?%2").arg(unitId, query.query()));

    QJsonObject fields;
    fields["status"] = QJsonObject{{QStringLiteral("stringValue"), status}};

    QNetworkReply* reply = network_->sendCustomRequest(request, "PATCH",
        QJsonDocument(QJsonObject{{QStringLiteral("fields"), fields}}).toJson());
    connect(reply, &QNetworkReply::finished, this, &UnitRegistry::onWriteFinished);
}

void UnitRegistry::onWriteFinished() {
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError)
        emit errorOccurred(tr("No se pudo actualizar la unidad: %1").arg(reply->errorString()));
}
