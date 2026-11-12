#pragma once
#include <QWidget>

class QButtonGroup;
class QLabel;
class QStackedWidget;
class QVBoxLayout;

// Estructura común de todos los dashboards: sidebar azul + header + páginas.
class DashboardBase : public QWidget
{
    Q_OBJECT
public:
    // embedded = true: sin botón de cerrar sesión (usado dentro del Sandbox).
    DashboardBase(const QString &roleLabel, const QString &userName,
                  bool embedded, QWidget *parent = nullptr);

    void addPage(const QString &navText, QWidget *page);

signals:
    void logoutRequested();

private:
    void selectPage(int index, const QString &title);

    QVBoxLayout *m_navLayout = nullptr;
    QButtonGroup *m_group = nullptr;
    QStackedWidget *m_stack = nullptr;
    QLabel *m_pageTitle = nullptr;
};
