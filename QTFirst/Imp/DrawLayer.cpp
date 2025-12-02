#include "DrawLayer.h"
#include <QtCharts/QChart>
#include <QtCharts/QAbstractSeries>
#include <QMouseEvent>
#include <QPainter>

using namespace QtCharts;

static QPointF toValue(QChart* c, const QPoint& px, QAbstractSeries* s) {
    return s ? c->mapToValue(px, s) : c->mapToValue(px);
}
static QPoint toPixel(QChart* c, const QPointF& val, QAbstractSeries* s) {
    return c->mapToPosition(val, s).toPoint();
}

DrawLayer::DrawLayer(QWidget* viewWidget) : m_view(viewWidget) {}

void DrawLayer::mousePress(QMouseEvent* e, QChart* chart, QAbstractSeries* ref)
{
    if (m_mode == Mode::None || !chart) return;

    if (m_mode == Mode::TrendLine) {
        if (!m_tempActive) {
            m_tmpAVal = toValue(chart, e->pos(), ref);
            m_tmpBVal = m_tmpAVal;
            m_tempActive = true;
        }
    }
    else if (m_mode == Mode::HLine) {
        // 点击一次就落一条水平线
        QPointF v = toValue(chart, e->pos(), ref);
        m_hlines.push_back(HLine{ v.y() });
        if (m_view)
        {
            m_view->update();
        }

    }
}

void DrawLayer::mouseMove(QMouseEvent* e, QChart* chart, QAbstractSeries* ref)
{
    if (m_mode != Mode::TrendLine || !m_tempActive || !chart) return;
    m_tmpBVal = toValue(chart, e->pos(), ref);
    if (m_view) m_view->update();
}

void DrawLayer::mouseRelease(QMouseEvent* e, QChart* chart, QAbstractSeries* ref)
{
    if (m_mode != Mode::TrendLine || !m_tempActive || !chart) return;
    m_tmpBVal = toValue(chart, e->pos(), ref);
    if (m_tmpAVal != m_tmpBVal) {
        m_trends.push_back(Trend{ m_tmpAVal, m_tmpBVal });
    }
    m_tempActive = false;
    if (m_view) m_view->update();
}

void DrawLayer::paint(QPainter* p, QChart* chart, QAbstractSeries* ref)
{
    if (!chart) return;

    p->setRenderHint(QPainter::Antialiasing, true);
    // 已完成的趋势线
    p->setPen(QPen(Qt::yellow, 1.5));
    for (const auto& t : m_trends) {
        QPoint a = toPixel(chart, t.aVal, ref);
        QPoint b = toPixel(chart, t.bVal, ref);
        p->drawLine(a, b);
    }

    // 临时趋势线
    if (m_tempActive && m_mode == Mode::TrendLine) {
        p->setPen(QPen(Qt::yellow, 1.0, Qt::DashLine));
        QPoint a = toPixel(chart, m_tmpAVal, ref);
        QPoint b = toPixel(chart, m_tmpBVal, ref);
        p->drawLine(a, b);
    }

    // 水平线
    p->setPen(QPen(Qt::cyan, 1.0));
    for (const auto& h : m_hlines) {
        QPoint a = toPixel(chart, QPointF(chart->plotArea().left(), h.yVal), ref);
        QPoint b = toPixel(chart, QPointF(chart->plotArea().right(), h.yVal), ref);
        // 上面用 plotArea 边界只是为了取一个范围，真正位置以 mapToPosition 为准
        p->drawLine(QPoint(0, a.y()), QPoint(m_view->width(), a.y()));
    }
}