#pragma once
#include <QDialog>

class QQuickWidget;
class UserBridge;

class UserInfoDialog : public QDialog {
    Q_OBJECT
public:
    enum class Mode { MoreInfo, Call, VehicleStatus };

    explicit UserInfoDialog(Mode mode, UserBridge* bridge, QWidget* parent = nullptr);

    QQuickWidget* view() const { return view_; }

private:
    QQuickWidget* view_{};
};
