#include "controller/UnitRegistry.h"
#include "controller/firebaseauthmanager.h"
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
#include <cmath>

// UnitRegistry.cpp
UnitRegistry &UnitRegistry::instance() {
  static UnitRegistry instance;
  return instance;
}

// Ejemplo de uso para este nuevo QObject :
// UnitRegistry::instance().loadUnits(); e implementar de manera distinta (Una
// vez para compilación global de sessiones sincronizada.

UnitRegistry::UnitRegistry(QObject *parent)
    : QObject(parent), network_(new QNetworkAccessManager(this)) {
  readConfig();
}

void UnitRegistry::clearSession() {
  idToken_.clear();
  units_.clear();
  vehicle_ = {};
  syncedUnitId_.clear();
  syncedDriver_.clear();
  syncedPosX_ = 0;
  syncedPosY_ = 0;
}

const UnitInfo *UnitRegistry::findUnit(const QString &unitId) const {
  for (const UnitInfo &u : units_) {
    if (u.id == unitId)
      return &u;
  }
  return nullptr;
}

UnitInfo *UnitRegistry::findUnit(const QString &unitId) {
  for (UnitInfo &u : units_) {
    if (u.id == unitId)
      return &u;
  }
  return nullptr;
}

void UnitRegistry::readConfig() {
  const QString path =
      QDir(QCoreApplication::applicationDirPath()).filePath("config.ini");
  QSettings settings(path, QSettings::IniFormat);
  projectId_ = settings.value("FIREBASE_PROJECT_ID").toString();
}

void UnitRegistry::configure(const QString &projectId, const QString &idToken) {
  if (!projectId.isEmpty())
    projectId_ = projectId;
  if (!idToken.isEmpty())
    idToken_ = idToken;
}

bool UnitRegistry::ready(QString *error) const {
  const QString pid = projectId_.isEmpty() ? FirebaseAuthManager::instance().projectId() : projectId_;
  if (pid.isEmpty()) {
    if (error)
      *error = tr("FIREBASE_PROJECT_ID no configurado");
    return false;
  }
  const QString token = idToken_.isEmpty() ? FirebaseAuthManager::instance().idToken() : idToken_;
  if (token.isEmpty()) {
    if (error)
      *error = tr("Sesion de Firebase no disponible");
    return false;
  }
  return true;
}

QNetworkRequest UnitRegistry::requestFor(const QString &path) const {
  const QString pid = projectId_.isEmpty() ? FirebaseAuthManager::instance().projectId() : projectId_;
  const QString token = idToken_.isEmpty() ? FirebaseAuthManager::instance().idToken() : idToken_;
  QUrl url(QStringLiteral("https://firestore.googleapis.com/v1/projects/%1/"
                          "databases/(default)/documents/units%2")
               .arg(pid, path));
  QNetworkRequest request(url);
  request.setRawHeader("Authorization",
                       QByteArray("Bearer ") + token.toUtf8());
  request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
  return request;
}

void UnitRegistry::loadUnits() {
  QString error;
  if (!ready(&error)) {
    emit errorOccurred(error);
    return;
  }

  QNetworkReply *reply = network_->get(requestFor(QString()));
  connect(reply, &QNetworkReply::finished, this, &UnitRegistry::onLoadFinished);
}

void UnitRegistry::onLoadFinished() {
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (!reply)
    return;
  reply->deleteLater();

  if (reply->error() != QNetworkReply::NoError) {
    emit errorOccurred(
        tr("No se pudieron leer las unidades: %1").arg(reply->errorString()));
    return;
  }

  const QJsonObject root = QJsonDocument::fromJson(reply->readAll()).object();
  const QJsonArray documents = root.value("documents").toArray();
  QVector<UnitInfo> parsed;
  parsed.reserve(documents.size());

  for (const QJsonValue &value : documents) {
    const QJsonObject doc = value.toObject();
    const QJsonObject fields = doc.value("fields").toObject();
    UnitInfo unit;
    unit.id = doc.value("name").toString().section('/', -1);
    unit.posX = fields.value("pos_x")
                    .toObject()
                    .value("integerValue")
                    .toString()
                    .toLongLong();
    unit.posY = fields.value("pos_y")
                    .toObject()
                    .value("integerValue")
                    .toString()
                    .toLongLong();
    if (unit.posX == 0 && unit.posY == 0) {
      unit.posX = fields.value("X")
                      .toObject()
                      .value("integerValue")
                      .toString()
                      .toLongLong();
      unit.posY = fields.value("Y")
                      .toObject()
                      .value("integerValue")
                      .toString()
                      .toLongLong();
    }
    unit.driverUid = fields.value("driver")
                         .toObject()
                         .value("referenceValue")
                         .toString()
                         .section('/', -1);
    if (unit.driverUid.isEmpty())
      unit.driverUid =
          fields.value("driver").toObject().value("stringValue").toString();
    unit.status =
        fields.value("status").toObject().value("stringValue").toString();
    unit.vehicle.id = unit.id.toInt();
    unit.vehicle.x = static_cast<double>(unit.posX);
    unit.vehicle.y = static_cast<double>(unit.posY);
    parsed.append(unit);
  }

  units_ = parsed;
  emit unitsLoaded(units_);
}

void UnitRegistry::syncVehicle(const sit::VehicleView &vehicle,
                               const QString &driverUid,
                               const QString &status) {
  vehicle_ = vehicle;

  const QString unitId = QString::number(vehicle.id);
  const qint64 posX = std::llround(vehicle.x);
  const qint64 posY = std::llround(vehicle.y);

  // Update in-memory unit list to keep local sessions synchronized
  bool found = false;
  for (UnitInfo &u : units_) {
    if (u.id == unitId) {
      u.posX = posX;
      u.posY = posY;
      u.driverUid = driverUid;
      if (!status.isEmpty())
        u.status = status;
      u.vehicle = vehicle;
      found = true;
      break;
    }
  }
  if (!found) {
    UnitInfo u;
    u.id = unitId;
    u.posX = posX;
    u.posY = posY;
    u.driverUid = driverUid;
    u.status = status;
    u.vehicle = vehicle;
    units_.append(u);
  }

  if (syncedUnitId_ == unitId && syncedDriver_ == driverUid &&
      syncedPosX_ == posX && syncedPosY_ == posY)
    return;

  registerUnit(unitId, driverUid, posX, posY, status);
  syncedUnitId_ = unitId;
  syncedDriver_ = driverUid;
  syncedPosX_ = posX;
  syncedPosY_ = posY;
}

void UnitRegistry::registerUnit(const QString &unitId, const QString &driverUid,
                                qint64 posX, qint64 posY,
                                const QString &status) {
  QString error;
  if (!ready(&error)) {
    emit errorOccurred(error);
    return;
  }
  if (unitId.isEmpty() || driverUid.isEmpty())
    return;

  const QString pid = projectId_.isEmpty() ? FirebaseAuthManager::instance().projectId() : projectId_;

  QUrlQuery query;
  for (const QString &field :
       {QStringLiteral("pos_x"), QStringLiteral("pos_y"),
        QStringLiteral("driver"), QStringLiteral("status")})
    query.addQueryItem(QStringLiteral("updateMask.fieldPaths"), field);

  QNetworkRequest request =
      requestFor(QStringLiteral("/%1?%2").arg(unitId, query.query()));

  QJsonObject fields;
  fields["pos_x"] =
      QJsonObject{{QStringLiteral("integerValue"), QString::number(posX)}};
  fields["pos_y"] =
      QJsonObject{{QStringLiteral("integerValue"), QString::number(posY)}};
  fields["driver"] = QJsonObject{
      {QStringLiteral("referenceValue"),
       QStringLiteral("projects/%1/databases/(default)/documents/users/%2")
           .arg(pid, driverUid)}};
  fields["status"] = QJsonObject{{QStringLiteral("stringValue"), status}};

  QNetworkReply *reply = network_->sendCustomRequest(
      request, "PATCH",
      QJsonDocument(QJsonObject{{QStringLiteral("fields"), fields}}).toJson());
  connect(reply, &QNetworkReply::finished, this,
          &UnitRegistry::onWriteFinished);
}

void UnitRegistry::setStatus(const QString &unitId, const QString &status) {
  for (UnitInfo &u : units_) {
    if (u.id == unitId) {
      u.status = status;
      break;
    }
  }

  QString error;
  if (!ready(&error)) {
    emit errorOccurred(error);
    return;
  }
  if (unitId.isEmpty())
    return;

  QUrlQuery query;
  query.addQueryItem(QStringLiteral("updateMask.fieldPaths"),
                     QStringLiteral("status"));
  QNetworkRequest request =
      requestFor(QStringLiteral("/%1?%2").arg(unitId, query.query()));

  QJsonObject fields;
  fields["status"] = QJsonObject{{QStringLiteral("stringValue"), status}};

  QNetworkReply *reply = network_->sendCustomRequest(
      request, "PATCH",
      QJsonDocument(QJsonObject{{QStringLiteral("fields"), fields}}).toJson());
  connect(reply, &QNetworkReply::finished, this,
          &UnitRegistry::onWriteFinished);
}

void UnitRegistry::onWriteFinished() {
  QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
  if (!reply)
    return;
  reply->deleteLater();
  if (reply->error() != QNetworkReply::NoError)
    emit errorOccurred(
        tr("No se pudo actualizar la unidad: %1").arg(reply->errorString()));
}
