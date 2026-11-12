#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "admindashboard.h"
#include "guestdashboard.h"
#include "loadingdialog.h"
#include "logindialog.h"
#include "movementdashboard.h"
#include "trafficdashboard.h"
#include "userdashboard.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setMinimumSize(1100, 700);
    resize(1280, 800);

    m_auth = new FirebaseAuthManager(this);
    m_login = new LoginDialog(m_auth, this);
    ui->stack->addWidget(m_login);
    ui->stack->setCurrentWidget(m_login);

    connect(m_auth, &FirebaseAuthManager::authenticated, this, &MainWindow::onAuthenticated);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAuthenticated(const AuthResult &result)
{
    // Transición "Cargando..." entre el login y el dashboard (~1.5 s).
    LoadingDialog::run(this, tr("Cargando..."),
                       tr("Preparando tu panel de %1").arg(Roles::label(result.role)), 1500);

    QWidget *dash = createDashboard(result);
    if (m_dashboard) {
        ui->stack->removeWidget(m_dashboard);
        m_dashboard->deleteLater();
    }
    m_dashboard = dash;
    ui->stack->addWidget(m_dashboard);
    ui->stack->setCurrentWidget(m_dashboard);
    setWindowTitle(tr("SIT-Core — %1").arg(Roles::label(result.role)));
}

QWidget *MainWindow::createDashboard(const AuthResult &r)
{
    QWidget *dash = nullptr;
    switch (r.role) {
    case Role::FleetManager: {
        auto *d = new AdminDashboard(r.displayName, false);
        connect(d, &DashboardBase::logoutRequested, this, &MainWindow::showLogin);
        dash = d;
        break;
    }
    case Role::User: {
        auto *d = new UserDashboard(r.displayName, false);
        connect(d, &DashboardBase::logoutRequested, this, &MainWindow::showLogin);
        dash = d;
        break;
    }
    case Role::JefeDeTrafico: {
        auto *d = new TrafficDashboard(r.displayName, false);
        connect(d, &DashboardBase::logoutRequested, this, &MainWindow::showLogin);
        dash = d;
        break;
    }
    case Role::ResponsableDesplazamientos: {
        auto *d = new MovementDashboard(r.displayName, false);
        connect(d, &DashboardBase::logoutRequested, this, &MainWindow::showLogin);
        dash = d;
        break;
    }
    case Role::Guess: {
        auto *d = new GuestDashboard(r.displayName);
        connect(d, &GuestDashboard::logoutRequested, this, &MainWindow::showLogin);
        dash = d;
        break;
    }
    }
    return dash;
}

void MainWindow::showLogin()
{
    m_login->reset();
    ui->stack->setCurrentWidget(m_login);
    if (m_dashboard) {
        ui->stack->removeWidget(m_dashboard);
        m_dashboard->deleteLater();
        m_dashboard = nullptr;
    }
    setWindowTitle(tr("SIT-Core - Sistema de Gestión y Monitoreo de Flotas"));
}
