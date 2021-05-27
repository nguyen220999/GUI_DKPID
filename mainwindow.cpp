#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qcustomplot.h"
#include <QtSerialPort>
#include "QPixmap"
#include "string.h"

#include "QDebug"
#include "QString"
#include "QFile"
#include "QTextStream"
#include "QMessageBox"
#include "qcustomplot.h"
#include "QThread"
#include "QTimer"
#include "QWidget"
#include "QCloseEvent"

#include "stdio.h"
#include "conio.h"
#include "QFile"


//hoan thanh ngay 26/6/2020 -BK.HCM
// Tran Hoang Nguyen- 1712395-PFIEV-CDT
// dieu khien dong co bang giai thuat PID
//che tao Driver dong co, viet code cho vi dieu khien STM32F103, viet GUI truyen nhan du lieu tu vi dieu khien



QSerialPort *serial;


QTimer *timer;
int j = 0, i=0, k=0;
QByteArray senddata, datasendfile, ReceiveData;
QString serialData;
QString serialBuffer, textBuffer;
union dou64bit
{
    double send;
    char sendbyte[8];  //tach so 64 bit thanh cac char 8 bit de truyen di
}gui, nhan;


double SetPoint=0, SetSetPoint, Speed_PV=0, Kp, Ki, Kd, Ts, pre_key=0, key=0, error=0;
double max=20, min=-20;





MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{


    ui->setupUi(this);


    timer = new QTimer(this);
    timer->start();
    QFont font;
    font.setPointSize(21);
    font.setItalic(true);
    font.setBold(true);
    setupRealtimeDataDemo(ui->customPlot);




}

MainWindow::~MainWindow()
{
    delete ui;
    serial->close();
}


void MainWindow::on_nutketnoi_clicked()
{

    serial = new QSerialPort(this);
    serial->setPortName(ui->comboBox->currentText());
        serial->setBaudRate(QSerialPort::Baud38400);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        serial->open(QIODevice::ReadWrite);
        connect(serial,SIGNAL(readyRead()),this,SLOT(SerialReceived()));
    ui->trangthai->setText("Đã kết nối");
   // QMessageBox::information(this,"","Đã kết nối");
}

void MainWindow::on_nutngatketnoi_clicked()
{
    serial->close();
    ui->trangthai->setText("Đã ngắt kết nối");
    //QMessageBox::information(this,"","Đã ngắt kết nối");
}

void MainWindow::on_dung_clicked()
{
    if(ui->trangthai->text()=="Đã kết nối")
    {
    //ui->kp->setText("0");
    //ui->ki->setText("0");
    //ui->kd->setText("0");
    ui->chonvantoc->setText("0");


    Kp = ui->kp->text().toDouble();
    Ki = ui->ki->text().toDouble(); //chuyen cac noi dung trong o Kp, Ki, Kd sang kieu double
    Kd = ui->kd->text().toDouble();

    SetSetPoint = ui->chonvantoc->text().toDouble();


    gui.send=Kp;
    for(int z=0; z<=7;z++)
        {
           senddata.append(gui.sendbyte[z]);
        }
    gui.send=Ki;
    for(int z=0; z<=7;z++)
        {
            senddata.append(gui.sendbyte[z]);
        }
    gui.send=Kd;
    for(int z=0; z<=7;z++)
        {
            senddata.append(gui.sendbyte[z]);
        }

    gui.send=SetSetPoint;
    for(int z=0; z<=7;z++)
        {
            senddata.append(gui.sendbyte[z]);
        }
    senddata.append("E");
        serial->write(senddata);
    }

    else
    {
        QMessageBox::information(this,"","Chưa kết nối với cổng COM");
    }

}



void MainWindow::on_khoidong_clicked()
{
    if(ui->trangthai->text()=="Đã kết nối")
    {
        Kp = ui->kp->text().toDouble();
        Ki = ui->ki->text().toDouble(); //chuyen cac noi dung trong o Kp, Ki, Kd sang kieu double
        Kd = ui->kd->text().toDouble();


        //Ts = ui->T_lbl->text().toDouble();
        //Ts = 0.01;


        SetSetPoint = ui->chonvantoc->text().toDouble();


        gui.send=Kp;
        for(int z=0; z<=7;z++)
            {
               senddata.append(gui.sendbyte[z]);
            }
        gui.send=Ki;
        for(int z=0; z<=7;z++)
            {
                senddata.append(gui.sendbyte[z]);
            }
        gui.send=Kd;
        for(int z=0; z<=7;z++)
            {
                senddata.append(gui.sendbyte[z]);
            }
       // gui.send=Ts;
        //for(int z=0;z<=7;z++)
        //    {
        //    senddata.append(gui.sendbyte[z]);
         //   }
        gui.send=SetSetPoint;
        for(int z=0; z<=7;z++)
            {
                senddata.append(gui.sendbyte[z]);
            }
        senddata.append("E");
            serial->write(senddata);
            }
            else
            {
                QMessageBox::warning(this,"","Chưa kết nối với cổng COM");
            }
}



void MainWindow::setupRealtimeDataDemo(QCustomPlot *customPlot)
{
    customPlot->addGraph()->setName("Speed_PV");    // khởi tạo graph vẽ Speed_PV
    customPlot->graph(0)->setPen(QPen(QColor(40, 110, 255)));
    customPlot->addGraph()->setName("Speed_SP"); // khởi tạo graph vẽ Speed_PVs
    customPlot->graph(1)->setPen(QPen(QColor(255, 110, 40)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%s");
    customPlot->xAxis->setTicker(timeTicker);
    customPlot->axisRect()->setupFullAxesBox();
    customPlot->yAxis->setRange(0, 200);

    ui->customPlot->xAxis->setLabel("thoi gian");
    ui->customPlot->yAxis->setLabel("van toc (RPM)");
    ui->customPlot->xAxis2->setVisible(true);
    ui->customPlot->yAxis->setVisible(true);
    ui->customPlot->xAxis2->setTicks(false);
    ui->customPlot->yAxis2->setTicks(false);

    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
}


void MainWindow::SerialReceived()   //cu phap truyen Speed+a+error+b
{
    serialData = serial->readAll();
    for(int i=0; i < serialData.size();i++)
    {
    if (serialData[i]=='a')
            {
                j=0;
                Speed_PV=serialBuffer.toDouble();
                ui->vantocthuc->display(Speed_PV);    //hien thi toc do ra LCD
                serialBuffer.clear();
            }
    else if(serialData[i]=='b')
        {
            j=0;
            SetPoint=serialBuffer.toDouble();
            serialBuffer.clear();
        }
    else
        {
        serialBuffer[j]=serialData[i];  //xu ly chuoi tin hieu nhan ve, thi du 123-4a12-34a-1234 ra 1234-1234-1234
        j++;
        qDebug()<<serialData;
        }
    }
    error=SetPoint-Speed_PV;
   //  ui->saiso->display(error);    //hien thi sai so ra LCD
     static QTime time(QTime::currentTime());
      key = time.elapsed()/1000.0;
          ui->customPlot->graph(0)->addData(key,SetPoint);    //graph 0 la SetPoint
          ui->customPlot->graph(1)->addData(key,Speed_PV);    //graph 1 la Real Speed
          ui->customPlot->graph(0)->rescaleValueAxis();
          ui->customPlot->graph(1)->rescaleValueAxis();
          if(SetPoint>max) max=SetPoint;
          if(Speed_PV>max) max=Speed_PV;
          if(SetPoint<min) min=SetPoint;
          if(Speed_PV<min) min=Speed_PV;
          ui->vantocthuc->display(Speed_PV);    //hien thi toc do ra LCD
          ui->saiso->display(error);       //hien thi sai so


          ui->customPlot->yAxis->setRange(min-50,max+50);    //scale truc y
          ui->customPlot->xAxis->setRange(pre_key,key);     //scale truc x
          ui->customPlot->replot();
}



void MainWindow::on_xoadothi_clicked()
{
    pre_key=key-1
            ;
    max=200;
    min=0;
}
