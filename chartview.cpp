#include "chartview.h"
#include <QtGui/QMouseEvent>

ChartView::ChartView(QChart *chart, QWidget *parent) :
    QChartView(chart, parent),
    m_isTouching(false)
{
    setRubberBand(QChartView::RectangleRubberBand);
}

bool ChartView::viewportEvent(QEvent *event)
{
    if (event->type() == QEvent::TouchBegin) {
        m_isTouching = true;
        chart()->setAnimationOptions(QChart::NoAnimation);
    }
    return QChartView::viewportEvent(event);
}

void ChartView::mousePressEvent(QMouseEvent *event)
{
    if (m_isTouching)
        return;
    QChartView::mousePressEvent(event);
}

void ChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isTouching)
        return;
    QChartView::mouseMoveEvent(event);
}

void ChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isTouching)
        m_isTouching = false;

    chart()->setAnimationOptions(QChart::SeriesAnimations);

    QChartView::mouseReleaseEvent(event);
}

void ChartView::keyPressEvent(QKeyEvent *event)
{
    // zoom
    switch (event->key()) {
    case Qt::Key_I:
        chart()->zoomIn();
        break;
    case Qt::Key_O:
        chart()->zoomOut();
        break;
        // key events
    case Qt::Key_Q:
        chart()->scroll(-20, 0);
        break;
    case Qt::Key_E:
        chart()->scroll(20, 0);
        break;
    case Qt::Key_W:
        chart()->scroll(0, 20);
        break;
    case Qt::Key_S:
        chart()->scroll(0, -20);
        break;
    case Qt::Key_Plus:
        chart()->zoomIn();
        break;
    case Qt::Key_Minus:
        chart()->zoomOut();
        break;
        // key events
    case Qt::Key_Left:
        chart()->scroll(-20, 0);
        break;
    case Qt::Key_Right:
        chart()->scroll(20, 0);
        break;
    case Qt::Key_Up:
        chart()->scroll(0, 20);
        break;
    case Qt::Key_Down:
        chart()->scroll(0, -20);
        break;
    default:
        QGraphicsView::keyPressEvent(event);
        break;
    }
}


