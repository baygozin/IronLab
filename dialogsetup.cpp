#include "dialogsetup.h"
#include "ui_dialogsetup.h"

DialogSetup::DialogSetup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSetup)
{
    ui->setupUi(this);
    ui->dblK20->setValue(k20);
    ui->dblK50->setValue(k50);
    ui->dblKShift->setValue(kShift);
    ui->leAddressIP->setText(leAddressIP);
    ui->leSaveDir->setText(leSaveDir);
}

DialogSetup::~DialogSetup()
{
    delete ui;
}

void DialogSetup::on_buttonBox_accepted()
{
    setK5(ui->dblK5->value());
    setK20(ui->dblK20->value());
    setK50(ui->dblK50->value());
    setKShift(ui->dblKShift->value());
    setLeAddressIP(ui->leAddressIP->text());
    setLeSaveDir(ui->leSaveDir->text());
    emit finished(getK5(), getK20(), getK50(), getKShift(), getLeAddressIP(), getLeSaveDir());
}

void DialogSetup::on_buttonBox_rejected()
{

}

QString DialogSetup::getLeAddressIP() const
{
    return leAddressIP;
}

void DialogSetup::setLeAddressIP(const QString &value)
{
    leAddressIP = value;
    ui->leAddressIP->setText(leAddressIP);
}

QString DialogSetup::getLeSaveDir() const
{
    return leSaveDir;
}

void DialogSetup::setLeSaveDir(const QString &value)
{
    leSaveDir = value;
    ui->leSaveDir->setText(leSaveDir);
}

double DialogSetup::getKShift() const
{
    return kShift;
}

void DialogSetup::setKShift(double value)
{
    kShift = value;
    ui->dblKShift->setValue(kShift);
}

double DialogSetup::getK50() const
{
    return k50;
}

void DialogSetup::setK50(double value)
{
    k50 = value;
    ui->dblK50->setValue(k50);
}

double DialogSetup::getK20() const
{
    return k20;
}

void DialogSetup::setK20(double value)
{
    k20 = value;
    ui->dblK20->setValue(k20);
}

double DialogSetup::getK5() const
{
    return k5;
}

void DialogSetup::setK5(double value)
{
    k5 = value;
    ui->dblK5->setValue(k5);
}


void DialogSetup::on_pushButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                               QString::fromUtf8("Выбрать папку"),
                               leSaveDir,
                               QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->leSaveDir->setText(dir);
}
