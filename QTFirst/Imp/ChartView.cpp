#include "ChartView.h"
#include "DrawLayer.h"
#include <QtCharts/QChart>
#include <QtCharts/QAbstractSeries>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QPainter>

using namespace QtCharts;

ChartView::ChartView(QChart* chart, QWidget* parent)
    : QChartView(chart, parent)
{
    setMouseTracking(true);
    viewport()->setMouseTracking(true);
    setRubberBand(QChartView::RectangleRubberBand);
    m_drawLayer.reset(new DrawLayer(this));

}

void ChartView::setDrawMode(DrawMode m) {
    m_mode = m;
    if (m_drawLayer)
        m_drawLayer->setMode(static_cast<DrawLayer::Mode>(m));
}

void ChartView::wheelEvent(QWheelEvent* e)
{
    const double factor = (e->angleDelta().y() < 0) ? 0.9 : 1.1;
    chart()->zoom(factor);
    QChartView::wheelEvent(e);
}

void ChartView::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::MiddleButton || e->button() == Qt::RightButton) 
    {
        m_panning = true;
        m_lastPos = e->pos();
    }
    if (m_drawLayer)
        m_drawLayer->mousePress(e, chart(), m_refSeries);

    QChartView::mousePressEvent(e);
}

void ChartView::mouseMoveEvent(QMouseEvent* e)
{
    if (m_panning) {
        QPointF d = e->pos() - m_lastPos;
        chart()->scroll(-d.x(), d.y());
        m_lastPos = e->pos();
    }

    m_cursorPos = e->pos();

    if (m_drawLayer)
        m_drawLayer->mouseMove(e, chart(), m_refSeries);

    if (m_refSeries) {
        // 将像素坐标映射到数据坐标
        QPointF val = chart()->mapToValue(e->pos(), m_refSeries);
        emit crosshairMoved(val);
    }

    viewport()->update();
    QChartView::mouseMoveEvent(e);
}

void ChartView::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::MiddleButton || e->button() == Qt::RightButton)
        m_panning = false;

    if (m_drawLayer)
        m_drawLayer->mouseRelease(e, chart(), m_refSeries);

    QChartView::mouseReleaseEvent(e);
}

void ChartView::drawForeground(QPainter* p, const QRectF& rect)
{
    Q_UNUSED(rect);
    // 十字光标
    if (!m_cursorPos.isNull()) {
        p->setRenderHint(QPainter::Antialiasing, false);
        p->setPen(QPen(QColor(160, 160, 160, 180), 1, Qt::DashLine));
        p->drawLine(QPointF(m_cursorPos.x(), 0), QPointF(m_cursorPos.x(), height()));
        p->drawLine(QPointF(0, m_cursorPos.y()), QPointF(width(), m_cursorPos.y()));
    }

    // 画线层
    if (m_drawLayer)
    {
        m_drawLayer->paint(p, chart(), m_refSeries);
    }
}