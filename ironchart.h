#ifndef IRONCHART_H
#define IRONCHART_H

#include <QtCharts/QChart>

QT_BEGIN_NAMESPACE
class QGestureEvent;
QT_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE


class IronChart : public QChart
{
public:
    explicit IronChart(QGraphicsItem *parent = 0, Qt::WindowFlags wFlags = 0);
    ~IronChart();

protected:
    bool sceneEvent(QEvent *event);

private:
    bool gestureEvent(QGestureEvent *event);
};

#endif // IRONCHART_H
