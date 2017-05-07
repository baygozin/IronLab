#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QtWidgets>
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCore/QTime>
#include <QtDebug>
#include <QFileDialog>
#include <QDir>
#include <QtTelnet>
#include <QThread>
#include <QPdfWriter>
#include <QTimer>
#include "ironchart.h"
#include "chartview.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    settingDP();
    readSettings();
    dialogSetup = new DialogSetup(this);
    telnet = new QtTelnet(this);
    connect(dialogSetup, SIGNAL(finished(double, double, double, double, QString, QString)),
            this, SLOT(getDialogSetting(double, double, double, double, QString, QString)));
    connect(telnet, SIGNAL(message(const QString &)),
            this, SLOT(telnetMessage(const QString &)));
    connect(telnet, SIGNAL(connectionError(QAbstractSocket::SocketError)),
            this, SLOT(telnetConnectionError(QAbstractSocket::SocketError)));
#ifdef TEST
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));
#endif
    tmr = new QTimer(this);
    tmr->setInterval(5000);
    connect(tmr, SIGNAL(timeout()), SLOT(sendPing()));
    ui->gridLayout->addWidget(chartViewDP, 0, 1, 0, 5);
    chartViewDP->setFrameStyle(6);
    ui->pbTelnetConnect->setFocus();
    ui->pbReset->setText(tr("Сброс МК"));
    axisDPX->setMax(10.0);
    axisDPY->setMax(20000);
    on_pbTelnetConnect_clicked();
    curentPoverSet(ui->cbPower->currentIndex());
}

MainWindow::~MainWindow()
{
    on_btnDisconnect_clicked();
    writeSettings();
    delete ui;
}

void MainWindow::getDialogSetting(double nK100, double nK200, double nK500, double nKShift, QString nIpAddress, QString nSaveDir)
{
    ipAddress = nIpAddress;
    directory = nSaveDir;
    k100 = nK100;
    k200 = nK200;
    k500 = nK500;
    kShift = nKShift;
    curentPoverSet(ui->cbPower->currentIndex());
}

//== настройка графика Перемещение-Нагрузка
void MainWindow::settingDP()
{
    chartDP = new IronChart();
    chartDP->setAnimationOptions(QChart::AllAnimations);
    chartDP->layout()->setContentsMargins(0,0,0,0);
    chartDP->setBackgroundRoundness(0);
    chartViewDP = new ChartView(chartDP);
    chartViewDP->setRenderHint(QPainter::Antialiasing);
    seriesDP = new QLineSeries();
    seriesDP->clear();
    chartDP->legend()->setVisible(false);
    axisDPX = new QValueAxis;
    axisDPY = new QValueAxis;
    axisDPY->setRange(0, rangeP);
    axisDPY->setLabelFormat("%8.0f");
    axisDPY->setTitleText(tr("Усилие, Н"));
    axisDPY->setTickCount(11);
    axisDPX->setRange(0, rangeD);
    axisDPX->setLabelFormat("%6.2f");
    axisDPX->setTitleText(tr("Перемещение, мм"));
    axisDPX->setTickCount(11);
    chartDP->addSeries(seriesDP);
    chartDP->setAxisX(axisDPX, seriesDP);
    chartDP->setAxisY(axisDPY, seriesDP);
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2, availableGeometry.height() - height() / 2);
    } else {
        restoreGeometry(geometry);
    }
    directory = settings.value("DirToSave", "").toString();
    ui->lineEditNameTesting->setText(settings.value("NameTesting", tr("испытание № ")).toString());
    k100 = settings.value("k100", 0.01).toDouble();
    k200 = settings.value("k200", 0.01).toDouble();
    k500 = settings.value("k500", 0.01).toDouble();
    kShift = settings.value("kShift", 0.01).toDouble();
    kPower = settings.value("kPower", 0.01).toDouble();
    ui->cbPower->setCurrentIndex(settings.value("kPowerStr", 0).toInt());
    ipAddress = settings.value("ipAddress", "0.0.0.0").toString();
    ui->leParam1->setText(settings.value("param1", tr("параметр 1")).toString());
    ui->leParam2->setText(settings.value("param2", tr("параметр 2")).toString());
    ui->leParam3->setText(settings.value("param3", tr("параметр 3")).toString());
    ui->leParamComment->setPlainText(settings.value("paramComment", tr("Комментарий к испытанию №1")).toString());
}

void MainWindow::writeSettings()
{
    QSettings setting(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    setting.setValue("geometry", saveGeometry());
    setting.setValue("DirToSave", directory);
    setting.setValue("NameTesting", ui->lineEditNameTesting->text());
    setting.setValue("k100", k100);
    setting.setValue("k200", k200);
    setting.setValue("k500", k500);
    setting.setValue("kShift", kShift);
    setting.setValue("kPower", kPower);
    setting.setValue("kPowerStr", ui->cbPower->currentIndex());
    setting.setValue("ipAddress", ipAddress);
    setting.setValue("param1", ui->leParam1->text());
    setting.setValue("param2", ui->leParam2->text());
    setting.setValue("param3", ui->leParam3->text());
    setting.setValue("paramComment", ui->leParamComment->toPlainText());
}

void MainWindow::calcAxisX(double x)
{
    for (int ii = 0; ii < 5000.0; ii = ii + 10.0) {
        if (x <= ii) {
            axisDPX->setRange(0, ii);
            break;
        }
    }
}

void MainWindow::calcAxisY(double y)
{
    qreal max = axisDPY->max();
    if (kPower == k100) {
        if (y > max) {
            for (qreal ii = 0.; ii < 200000.0; ii = ii + 20000) { // 100kN
                if (y <= ii) {
                    axisDPY->setRange(0, ii);
                    break;
                }
            }
        }
    } else if (kPower == k200) {
        if (y > max) {
            for (qreal ii = 0.; ii < 300000.0; ii = ii + 40000) { // 200kN
                if (y <= ii) {
                    axisDPY->setRange(0, ii);
                    break;
                }
            }
        }
    } else if (kPower == k500) {
        if (y > max) {
            for (qreal ii = 0.; ii < 600000.0; ii = ii + 100000) { // 500kN
                if (y <= ii) {
                    axisDPY->setRange(0, ii);
                    break;
                }
            }
        }
    }
}

// Пишем данные в порт
void MainWindow::sendTelnetData(const QByteArray &writeData)
{
    telnet->sendData(writeData);
}

// Сообщение от устройства
void MainWindow::telnetMessage(const QString &msg)
{
    QList<QString> lists = msg.trimmed().split(',');
    if (lists.size() == 4) { // должно прийти 4 числа
        uint32_t ch = lists[0].toUInt(); // код пакета
        int numberA = lists[1].toInt(); // 1-й энкодер
        int numberB = lists[2].toInt(); // 2-й энкодер
        //qreal time = lists[3].toUInt() * 0.1; // условное время
        switch (ch) {
        case 1:{
            qreal kd = kShift * numberA;
            qreal kp = kPower * numberB;
            ui->lineEditDmm->setText(QString::number(kd, 'f', 2));
            ui->lineEditPmm->setText(QString::number(kp, 'f', 0));
            calcAxisX(kd);
            calcAxisY(kp);
            seriesDP->append(kd, kp);
            sizeArray++;
            break;
        }
        case 0: {
            on_pushButtonClearAll_clicked();
            break;
        }
        default: {break;}
        }
    }
}

// запуск чтения данных с датчиков
void MainWindow::on_pushButtonStart_clicked()
{
    QByteArray a;
    a.append('1');
    sendTelnetData(a);
#ifdef TEST
    timer->start(250);
#endif
}

// останов чтения данных с датчиков
void MainWindow::on_pushButtonStop_clicked()
{
    QByteArray a;
    a.append('2');
    sendTelnetData(a);
#ifdef TEST
    timer->stop();
#endif
}

// очистить все данные в нуль
void MainWindow::on_pushButtonClearAll_clicked()
{
    ax = 0;
    ay = 0;
    QByteArray a;
    a.append('3');
    sendTelnetData(a);
    ui->lineEditDmm->setText("0");
    ui->lineEditPmm->setText("0");
    seriesDP->clear();
    sizeArray = 0;
    curentPoverSet(ui->cbPower->currentIndex());
}

// Сохраним данные в файле excel...
void MainWindow::on_pushButtonSaveExcel_clicked()
{
    QFileDialog fDlg;
    QString selFilter;
    fDlg.setDirectory(directory);
    fDlg.selectFile("Test");
    QString testName = ui->lineEditNameTesting->text() + QDateTime::currentDateTime().toString("_dd-MM-yy_hh-mm-ss");
    QString fileName = fDlg.getSaveFileName(this, "Save data", directory + "/" + testName, "*.xlsx", &selFilter);
    saveExcel(fileName);
}

void MainWindow::saveExcel(QString fileName)
{
    int size = seriesDP->count();
    xlsx.write("A1", ui->lineEditNameTesting->text());
    xlsx.write("A2", ui->leParam1->text());
    xlsx.write("A3", ui->leParam2->text());
    xlsx.write("A4", ui->leParam3->text());
    xlsx.write("A5", ui->leParamComment->toPlainText());
    xlsx.write("E1", "sizeArray - " + QString::number(sizeArray) + ";  seriesDP.count - " + QString::number(size));

    for (int i = 0; i < sizeArray; i++) {
        xlsx.write(colX + QString::number(i + shiftY), seriesDP->points().value(i).rx());
        xlsx.write(colY + QString::number(i + shiftY), seriesDP->points().value(i).ry());
    }
    xlsx.saveAs(fileName);
}

// Сохраним данные в файле pdf...
void MainWindow::on_pushButtonSavePdf_clicked()
{
    QFileDialog fDlg;
    QString selFilter;
    fDlg.setDirectory(directory);
    QString testName = ui->lineEditNameTesting->text() + QDateTime::currentDateTime().toString("_dd-MM-yy_hh-mm-ss");
    QString fileName = fDlg.getSaveFileName(this, "Save data", directory + "/" + testName, "*.pdf", &selFilter);
    savePdf(fileName);
}

void MainWindow::savePdf(QString fileName)
{
    QPdfWriter writer(fileName);
    writer.setCreator("IronLab");
    writer.setPageSize(QPagedPaintDevice::A4);
    writer.setPageOrientation(QPageLayout::Orientation::Landscape);
    writer.setTitle(ui->lineEditNameTesting->text());
    QPainter painter(&writer);

    chartViewDP->render(&painter);

    painter.setPen(Qt::black);
    painter.drawText(500,7100, ui->lineEditNameTesting->text());
    painter.drawText(500,7400, ui->leParam1->text());
    painter.drawText(500,7700, ui->leParam2->text());
    painter.drawText(500,8000, ui->leParam3->text());
    painter.drawText(500,8300, ui->leParamComment->toPlainText());

    painter.end();
}

void MainWindow::updateTimer()
{
    ax = ax + 1;
    if (ax < 100) {
        ay = ay + 2;
    } else {
        ay = ay - 2;
    }
    telnetMessage("1," + QString::number(ax) +"," +QString::number(ay) + ", 1");
}

void MainWindow::sendPing()
{
    QByteArray a;
    a.append('9');
    sendTelnetData(a);
}

void MainWindow::on_pbTelnetConnect_clicked()
{
    QByteArray a;
    telnet->connectToHost(ipAddress, ipPort);
    if (telnet->socket()->waitForConnected(1000)) {
        QMessageBox::information(this, "IronLab", "Устройство найдено и подключено!");
        ui->statusBar->showMessage("Устройство найдено и подключено!", 1000);
        tmr->start();
    } else {
        QMessageBox::critical(this, "IronLab", "Устройство не найдено!");
        ui->statusBar->showMessage("Устройство не найдено!", 1000);
        return;
    }
}

void MainWindow::on_btnDisconnect_clicked()
{
    if (telnet->socket()->isOpen()) {
        tmr->stop();
        telnet->logout();
        telnet->close();
        telnet->socket()->close();
        ui->statusBar->showMessage("Oтключились!");
    }
}

// Обработка ошибки telnet
void MainWindow::telnetConnectionError(QAbstractSocket::SocketError error)
{
    if (tmr != NULL) {
        if (tmr->isActive()) {
            tmr->stop();
        }
    }
    QMessageBox::critical(this, "IronLab", "Ошибка подключения!");
    ui->statusBar->showMessage(QString("Ошибка подключения: %1").arg(error));
    connected = false;
}

// Удаление возврата каретки из конца строки
QString MainWindow::stripCR(const QString &msg)
{
    QString nmsg(msg);
    nmsg.remove('\r');
    nmsg.remove(QRegExp("\033\\[[0-9;]*[A-Za-z]")); // Also remove terminal control codes
    return nmsg;
}

void MainWindow::on_pbReset_clicked()
{
    QByteArray a;
    a.append('6');
    sendTelnetData(a);
    ui->lineEditDmm->setText("0");
    ui->lineEditPmm->setText("0");
    seriesDP->clear();
    ax=0;
    ay=0;
}

// Окно настроек
void MainWindow::on_actionSetting_triggered()
{
    dialogSetup->setK5(k100);
    dialogSetup->setK20(k200);
    dialogSetup->setK50(k500);
    dialogSetup->setKShift(kShift);
    dialogSetup->setLeAddressIP(ipAddress);
    dialogSetup->setLeSaveDir(directory);
    dialogSetup->show();
}

void MainWindow::on_cbPower_currentIndexChanged(int index)
{
    curentPoverSet(index);
}

void MainWindow::curentPoverSet(int index)
{
    switch (index) {
    case 0: {
        kPower = k100;
        axisDPY->setMax(20000);
        break;
    }
    case 1: {
        kPower = k200;
        axisDPY->setMax(40000);
        break;
    }
    case 2: {
        kPower = k500;
        axisDPY->setMax(100000);
        break;
    }
    default: kPower = 0.01;
    }
}


// Масштаб 1:1
void MainWindow::on_pushButton_clicked()
{
    if (seriesDP->points().length() == 0 || seriesDP->points().length() == 01) return;
    qreal x = seriesDP->points().last().rx();
    qreal y = seriesDP->points().last().ry();
    calcAxisX(x);
    calcAxisY(y);
}
