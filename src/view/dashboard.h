#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QString>
#include <QWidget>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QWidget;

// Pagina base de cada rol (herencia + polimorfismo). Cada subclase define su titulo y
// agrega su contenido en contentLayout(). Para un rol nuevo: heredar de Dashboard y
// registrarlo en MainWindow.
class Dashboard : public QWidget {
    Q_OBJECT
public:
    explicit Dashboard(QWidget *parent = nullptr);

    void retranslate();
    void setSessionInfo(const QString &name, const QString &role);

signals:
    void logoutRequested();

protected:
    void changeEvent(QEvent *event) override;
    QVBoxLayout *contentLayout() const { return content_; }

    virtual QString dashboardTitle() const;
    virtual QString welcomeMessage() const;

private:
    void updateSessionLabels();

    QLabel *brand_;
    QLabel *subtitle_;
    QLabel *title_;
    QLabel *welcome_;
    QLabel *roleBadge_;
    QPushButton *logoutButton_;
    QWidget *contentContainer_;
    QVBoxLayout *content_;
    QString userName_;
    QString role_;
};

#endif // DASHBOARD_H
