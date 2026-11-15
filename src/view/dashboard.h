#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QWidget>

class QLabel;
class QVBoxLayout;

// Pagina base de cada rol (herencia + polimorfismo). Cada subclase define su titulo y
// agrega su contenido en contentLayout(). Para un rol nuevo: heredar de Dashboard y
// registrarlo en MainWindow.
class Dashboard : public QWidget {
    Q_OBJECT
public:
    explicit Dashboard(QWidget *parent = nullptr);

    void retranslate();

protected:
    void changeEvent(QEvent *event) override;
    QVBoxLayout *contentLayout() const { return content_; }
    // Muestra un error (p. ej. red no cargada) dentro del contenido.
    void showError(const QString &message);

    virtual QString dashboardTitle() const;
    virtual QString welcomeMessage() const;

private:
    QLabel *title_;
    QLabel *welcome_;
    QVBoxLayout *content_;
    QLabel *error_ = nullptr;
};

#endif // DASHBOARD_H
