#include "maindialog.h"
#include "ui_maindialog.h"

#include <QMessageBox>
#include <QSerialPortInfo>
#include <QDebug>

MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainDialog)
{
    ui->setupUi(this);

    ui->sliderLight->setRange(0, 1000);
    ui->sliderServo->setRange(0, 180);

    memset(&devCmd, 0, sizeof(devCmd));
    devCmd.head = 0x5AA5;
    devCmd.tail = 0x3CC3;

    // 枚举系统中所有可用串口
    QList<QSerialPortInfo> allPorts = QSerialPortInfo::availablePorts();

    for(const QSerialPortInfo& sp : allPorts)
    {
        ui->comboSerialPort->addItem(sp.portName());
    }

    ui->comboBaud->addItem(QString::number(115200));
    ui->comboBaud->addItem(QString::number(9600));
    ui->comboBaud->addItem(QString::number(4800));


    m_sp = new QSerialPort  (this);

    connect(m_sp, &QIODevice::readyRead, this, &MainDialog::recvData);
}

MainDialog::~MainDialog()
{
    delete ui;
}

void MainDialog::recvData()
{
    int i;
    static QByteArray prevData;

    QByteArray curData = prevData + m_sp->readAll();

    //qDebug() << "--------------" << curData << "-----------------";

    QList<QByteArray> msgs = curData.split('\n');
    QList<QByteArray> p;

    for(i = 0; i < msgs.size() - 1; i++)
    {
        qDebug() << msgs[i];

        // 每提取一条完整的消息就进行一次处理
        p = msgs[i].split(':');

        if(p.size() != 2) continue;

        if(p[0] == "BEEP1")
        {
            if(p[1] == "0") ui->btnAlarm->setText("报 警");
            else if(p[1] == "1") ui->btnAlarm->setText("静 音");
        }

        if(p[0] == "LED2")
        {
            if(p[1] == "0") ui->btnLED2On->setText("开 灯");
            else if(p[1] == "1") ui->btnLED2On->setText("关 灯");
        }

        if(p[0] == "DHT11_1")
        {
            QList<QByteArray> th = p[1].split('_');
            ui->labelTemp->setText(QString(th[0]) + " ℃");
            ui->labelHumi->setText(QString(th[1]) + " %RH");
        }
    }

    prevData = msgs[i];
}


void MainDialog::on_btnOpen_clicked()
{
    if(ui->btnOpen->text() == "打 开")
    {
        QString portName = ui->comboSerialPort->currentText();
        int baud = ui->comboBaud->currentText().toUInt();

        qDebug() << baud;

        m_sp->setPortName(portName);             // 设置串口号
        m_sp->setBaudRate(baud);                 // 设置波特率
        m_sp->setDataBits(QSerialPort::Data8);   // 设置数据位为 8 位
        m_sp->setParity(QSerialPort::NoParity);  // 无校验
        m_sp->setStopBits(QSerialPort::OneStop); // 停止位为 1 位

        // 万事俱备，打开串口
        if(!m_sp->open(QIODevice::ReadWrite))
        {
            QMessageBox::critical(this, "君哥提示", "打开串口失败！");
        }
        else
        {
            ui->btnOpen->setText("关 闭");
        }
    }
    else
    {
       m_sp->close();  // 关闭串口
       ui->btnOpen->setText("打 开");
    }
}

void MainDialog::on_btnAlarm_clicked()
{


    if(m_sp->isOpen())
    {
         devCmd.devType = 2;  // 蜂鸣器
         devCmd.devID = 1;

        if(ui->btnAlarm->text() == "报 警")
            devCmd.devCmd = 1;
        else if(ui->btnAlarm->text() == "静 音")
            devCmd.devCmd = 0;

        m_sp->write((const char*)&devCmd, sizeof(devCmd));
    }

}

void MainDialog::on_btnLED2On_clicked()
{
    if(m_sp->isOpen())
    {
         devCmd.devType = 1;  // LED
         devCmd.devID = 2;

        if(ui->btnLED2On->text() == "开 灯")
            devCmd.devCmd = 1;
        else if(ui->btnLED2On->text() == "关 灯")
            devCmd.devCmd = 0;

        m_sp->write((const char*)&devCmd, sizeof(devCmd));
    }
}

void MainDialog::on_sliderLight_valueChanged(int value)
{
    if(m_sp->isOpen())
    {
        devCmd.devType = 1;  // LED
        devCmd.devID = 1;
        devCmd.devCmd = 1;
        devCmd.devCmdArg = value;

        m_sp->write((const char*)&devCmd, sizeof(devCmd));
    }
}

void MainDialog::on_sliderServo_valueChanged(int value)
{
    if(m_sp->isOpen())
    {
        devCmd.devType = 4;  // 舵机
        devCmd.devID = 1;
        devCmd.devCmd = 1;
        devCmd.devCmdArg = value;

        m_sp->write((const char*)&devCmd, sizeof(devCmd));
    }
}
