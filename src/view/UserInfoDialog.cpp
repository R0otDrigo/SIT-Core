#include "view/UserInfoDialog.h"
#include <QQuickItem>
#include <QQuickWidget>
#include <QQmlContext>
#include <QVBoxLayout>
#include <QUrl>
#include "view/UserBridge.h"

UserInfoDialog::UserInfoDialog(Mode mode, UserBridge* bridge, QWidget* parent)
    : QDialog(parent)
{
    QString modeName;
    int height = 330;
    switch (mode) {
    case Mode::MoreInfo:
        modeName = QStringLiteral("info");
        height = 430;
        break;
    case Mode::Call:
        modeName = QStringLiteral("call");
        height = 280;
        break;
    case Mode::VehicleStatus:
        modeName = QStringLiteral("vehicle");
        height = 370;
        break;
    }

    setModal(true);
    setWindowTitle(modeName == QStringLiteral("info")
                       ? tr("Más información")
                       : (modeName == QStringLiteral("call") ? tr("Llamar")
                                                             : tr("Estado del vehículo")));

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    view_ = new QQuickWidget(this);
    view_->setResizeMode(QQuickWidget::SizeRootObjectToView);
    view_->rootContext()->setContextProperty("userBridge", bridge);
    view_->rootContext()->setContextProperty("dialogMode", modeName);
    view_->setFixedSize(460, height);
    layout->addWidget(view_);

    view_->setSource(QUrl(QStringLiteral("qrc:/qml/UserInfoDialog.qml")));
    if (view_->rootObject())
        connect(view_->rootObject(), SIGNAL(closeRequested()), this, SLOT(accept()));
}
