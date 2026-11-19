#include "admindashboard.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "common/Exceptions.h"
#include "controller/SimulationController.h"
#include "controller/UnitRegistry.h"
#include "view/SimulationWidget.h"
#include "view/UserBroadcast.h"

#ifndef SIT_DATA_DIR
#define SIT_DATA_DIR "data"
#endif

AdminDashboard::AdminDashboard(QWidget *parent)
    : Dashboard(parent)
{
    retranslate();

    auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
    messagesBox_ = new QGroupBox(tr("Mensajes para el usuario"), this);
    auto *grid = new QGridLayout(messagesBox_);
    antecedentesEdit_ = new QLineEdit(messagesBox_);
    eventoEdit_ = new QLineEdit(messagesBox_);
    auto *clearButton = new QPushButton(tr("Limpiar"), messagesBox_);
    auto *sendButton = new QPushButton(tr("Enviar"), messagesBox_);
    grid->addWidget(new QLabel(tr("Antecedentes:"), messagesBox_), 0, 0);
    grid->addWidget(antecedentesEdit_, 0, 1);
    grid->addWidget(new QLabel(tr("Evento:"), messagesBox_), 1, 0);
    grid->addWidget(eventoEdit_, 1, 1);
    grid->addWidget(clearButton, 2, 0);
    grid->addWidget(sendButton, 2, 1);
    if (layout) layout->addWidget(messagesBox_);
    messagesBox_->setEnabled(false);

    connect(sendButton, &QPushButton::clicked, this, [this] {
        sendMessages(antecedentesEdit_->text(), eventoEdit_->text());
    });
    connect(clearButton, &QPushButton::clicked, this, [this] {
        antecedentesEdit_->clear();
        eventoEdit_->clear();
        if (broadcast_) broadcast_->clear();
    });
}

AdminDashboard::~AdminDashboard() = default;

void AdminDashboard::setBroadcast(UserBroadcast *broadcast)
{
    broadcast_ = broadcast;
    if (messagesBox_) messagesBox_->setEnabled(broadcast_ != nullptr);
}

void AdminDashboard::setUnitRegistry(UnitRegistry *registry)
{
    unitRegistry_ = registry;
}

void AdminDashboard::sendMessages(const QString &antecedentes, const QString &evento)
{
    if (!broadcast_) return;
    if (antecedentesEdit_) antecedentesEdit_->setText(antecedentes);
    if (eventoEdit_) eventoEdit_->setText(evento);
    broadcast_->setAntecedentes(antecedentes);
    broadcast_->setEvento(evento);

    if (unitRegistry_) {
        const QVector<UnitInfo> units = unitRegistry_->units();
        for (const UnitInfo &unit : units)
            unitRegistry_->setStatus(unit.id, evento);
    }
}

void AdminDashboard::showIncomingCall(const QString &vehicleId, const QString &name,
                                      const QString &status)
{
    if (!simulation_) return;
    simulation_->showIncomingCall(vehicleId, name, status);
    if (unitRegistry_) unitRegistry_->setStatus(vehicleId, QStringLiteral("Llamada entrante: %1").arg(name));
}

void AdminDashboard::initialize(const QString &idToken)
{
    idToken_ = idToken;
    if (unitRegistry_) {
        unitRegistry_->configure(unitRegistry_->projectId(), idToken_);
        unitRegistry_->loadUnits();
    }
    if (simulation_) return;
    auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
    if (!layout) return;

    QString path = QDir(QCoreApplication::applicationDirPath()).filePath("data/network.csv");
    if (!QFile::exists(path)) path = QString(SIT_DATA_DIR) + "/network.csv";

    try {
        simulation_ = new SimulationWidget(SimulationController::loadNetwork(path), this);
        layout->addWidget(simulation_, 1);
    } catch (const sit::SitException &e) {
        auto *error = new QLabel(tr("No se pudo cargar la red:\n%1").arg(e.what()), this);
        error->setWordWrap(true);
        layout->addWidget(error, 1);
    }
}

void AdminDashboard::resetSession()
{
    delete simulation_;
    simulation_ = nullptr;
}

QString AdminDashboard::dashboardTitle() const
{
    return tr("Admin Dashboard");
}

QString AdminDashboard::welcomeMessage() const
{
    return tr("Welcome Administrator");
}
