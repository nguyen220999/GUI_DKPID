#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include<QTimer>
#include <qcustomplot.h>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setupRealtimeDataDemo(QCustomPlot *customPlot);
private slots:
    void on_nutketnoi_clicked();

    void on_nutngatketnoi_clicked();

    void on_dung_clicked();

    void on_khoidong_clicked();

    //void setupRealtimeDataDemo(QCustomPlot *customPlot);
    void SerialReceived();
    void on_xoadothi_clicked();

private:
    Ui::MainWindow *ui;
    QTimer dataTimer;
};
#endif // MAINWINDOW_H
