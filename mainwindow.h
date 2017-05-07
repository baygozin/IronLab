#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCharts>
#include <QLineSeries>
#include <QSplineSeries>
#include <QScatterSeries>
#include <QPieSeries>
#include <QChartView>
#include <QtCore/QTime>
#include <QTcpSocket>
#include <QTimer>
#include "QtTelnet"
#include <QDateTime>
#include "dialogsetup.h"
#include "xlsxdocument.h"

//#define TEST

using namespace QtCharts;

QTXLSX_USE_NAMESPACE

namespace Ui {
    class MainWindow;
}

struct DataEncoder {
    uint8_t code;
    uint32_t dataEncoder;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void getDialogSetting(double, double, double, double, QString, QString);
    void on_pushButtonStart_clicked();
    void on_pushButtonClearAll_clicked();
    void on_pushButtonStop_clicked();
    void on_pushButtonSaveExcel_clicked();
    void saveExcel(QString);
    void on_pushButtonSavePdf_clicked();
    void savePdf(QString);

    void on_pbTelnetConnect_clicked();

    void telnetMessage(const QString &msg);
    void telnetConnectionError(QAbstractSocket::SocketError error);

    QString stripCR(const QString &msg);

    void on_pbReset_clicked();

    void on_btnDisconnect_clicked();

    void on_actionSetting_triggered();

    void on_cbPower_currentIndexChanged(int index);
    void curentPoverSet(int);

    void on_pushButton_clicked();

    void updateTimer();
    void sendPing();

private:

#ifdef TEST
    QTimer *timer = NULL;
#endif
    Ui::MainWindow *ui;
    QTimer *tmr = NULL;
    DialogSetup *dialogSetup;
    QChart *chartDP;
    QChartView *chartViewDP;
    QLineSeries *seriesDP;
    QValueAxis *axisDPX;
    QValueAxis *axisDPY;

    QXlsx::Document xlsx;
    QString colX = "B";
    QString colY = "C";
    int shiftY = 6;

    LONGLONG ay = 0;
    LONGLONG ax = 0;

    QString directory;

    bool connected = false;

    int rangeD = 100;
    int rangeP = 100;
    int sizeArray = 0;
    double  k100, k200, k500, kPower, kShift; // коэффициенты для пересчета
    QString ipAddress;
    QString saveDir;
    quint16  ipPort = 23;

    QtTelnet *telnet;

    void settingDP();

    void readSettings();
    void writeSettings();

    void calcAxisX(double);
    void calcAxisY(double);

    void sendTelnetData(const QByteArray &writeData);

    QByteArray bytes;

};

#endif // MAINWINDOW_H
