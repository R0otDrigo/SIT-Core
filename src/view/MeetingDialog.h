#pragma once
#include <QDialog>

class QLabel;
class QProcess;
class QVBoxLayout;

class MeetingDialog : public QDialog {
    Q_OBJECT
public:
    explicit MeetingDialog(const QString& peerName, QWidget* parent = nullptr);
    ~MeetingDialog() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void startPreview();
    void stopPreview();
    QString microphoneStatus() const;

    QLabel* status_{};
    QProcess* preview_{};
};
