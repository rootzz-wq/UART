#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class MainDialog; }
QT_END_NAMESPACE

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();

private slots:
    void recvData();

    void on_btnOpen_clicked();

    void on_btnAlarm_clicked();

    void on_btnLED2On_clicked();

    void on_sliderLight_valueChanged(int value);

    void on_sliderServo_valueChanged(int value);

private:

    #pragma pack(push, 1)

    struct DeviceCtrlCmd
    {
        quint16 head;
        quint16 devType;
        quint16 devID;
        quint16 devCmd;
        quint16 devCmdArg;
        quint16 tail;
    } devCmd;

    #pragma pack(pop)

    Ui::MainDialog *ui;
    QSerialPort* m_sp;
};
#endif // MAINDIALOG_H
