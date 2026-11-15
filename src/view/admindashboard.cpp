#include "admindashboard.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "common/Exceptions.h"
#include "controller/SimulationController.h"
#include "controller/UnitRegistry.h"
#include "view/SimulationWidget.h"
#include "view/UserBroadcast.h"

AdminDashboard::AdminDashboard(QWidget *parent)
    : Dashboard(parent)
{
    retranslate();

    // Una sola fila bajo el encabezado: Antecedentes | Evento | Limpiar | Enviar.
    messagesBox_ = new QGroupBox(tr("Mensajes para el usuario"), this);
    auto *row = new QHBoxLayout(messagesBox_);
    antecedentesEdit_ = new QLineEdit(messagesBox_);
    antecedentesEdit_->setPlaceholderText(tr("Ej.: Sin antecedentes"));
    eventoEdit_ = new QLineEdit(messagesBox_);
    eventoEdit_->setPlaceholderText(tr("Ej.: Reunion con el administrador"));
    auto *clearButton = new QPushButton(tr("Limpiar"), messagesBox_);
    clearButton->setProperty("variant", "ghost");
    auto *sendButton = new QPushButton(tr("Enviar"), messagesBox_);
    sendButton->setProperty("variant", "primary");
    sendButton->setDefault(true);
    row->addWidget(new QLabel(tr("Antecedentes"), messagesBox_));
    row->addWidget(antecedentesEdit_, 1);
    row->addWidget(new QLabel(tr("Evento"), messagesBox_));
    row->addWidget(eventoEdit_, 1);
    row->addWidget(clearButton);
    row->addWidget(sendButton);
    contentLayout()->addWidget(messagesBox_);
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
    messagesBox_->setEnabled(broadcast_ != nullptr);
}

void AdminDashboard::setUnitRegistry(UnitRegistry *registry)
{
    unitRegistry_ = registry ? registry : &UnitRegistry::instance();
}

void AdminDashboard::sendMessages(const QString &antecedentes, const QString &evento)
{
    if (!broadcast_) return;
    antecedentesEdit_->setText(antecedentes);
    eventoEdit_->setText(evento);
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

    try {
        simulation_ = new SimulationWidget(
            SimulationController::loadNetwork(SimulationController::defaultNetworkPath()), this);
        contentLayout()->addWidget(simulation_, 1);
    } catch (const sit::SitException &e) {
        showError(tr("No se pudo cargar la red:\n%1").arg(e.what()));
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
