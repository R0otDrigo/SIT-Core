#pragma once
#include <QDialog>
#include <QVariantAnimation>

class QProgressBar;
class QLabel;

// Diálogo de transición "Cargando..." (spinner + barra de progreso fluida).
// Por defecto es 100% C++; con SIT_QML_LOADING usa cargando.qml (QQuickWidget).
class LoadingDialog : public QDialog
{
    Q_OBJECT
public:
    explicit LoadingDialog(const QString &message = QString(),
                           const QString &detail = QString(),
                           int durationMs = 1500,
                           QWidget *parent = nullptr);

    // Muestra el diálogo de forma modal y vuelve cuando termina la animación.
    static void run(QWidget *parent, const QString &message = QString(),
                    const QString &detail = QString(), int durationMs = 1500);

protected:
    void showEvent(QShowEvent *event) override;

private:
    QVariantAnimation m_anim;
    QProgressBar *m_bar = nullptr;
    QObject *m_qmlRoot = nullptr;
    bool m_started = false;
};
