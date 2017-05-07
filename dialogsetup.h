#ifndef DIALOGSETUP_H
#define DIALOGSETUP_H

#include <QDialog>
#include <QFileDialog>

namespace Ui {
class DialogSetup;
}

class DialogSetup : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSetup(QWidget *parent = 0);
    ~DialogSetup();

    double getK5() const;
    void setK5(double value);

    double getK20() const;
    void setK20(double value);

    double getK50() const;
    void setK50(double value);

    double getKShift() const;
    void setKShift(double value);

    QString getLeAddressIP() const;
    void setLeAddressIP(const QString &value);

    QString getLeSaveDir() const;
    void setLeSaveDir(const QString &value);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_pushButton_clicked();

private:
    Ui::DialogSetup *ui;
    double  k5, k20, k50, kShift;
    QString leAddressIP;
    QString leSaveDir;

signals:
    void finished(double, double, double, double, QString, QString);

};

#endif // DIALOGSETUP_H
