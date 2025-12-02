#pragma once
#include <QtCharts/QChartView>
#include <QPoint>
#include <QVector>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE
class QWheelEvent;
class QMouseEvent;
class QPainter;
QT_END_NAMESPACE

namespace QtCharts { class QAbstractSeries; }

class DrawLayer; // 前置声明（画线层）

class ChartView : public QtCharts::QChartView {
    Q_OBJECT
public:
    explicit ChartView(QtCharts::QChart* chart, QWidget* parent = nullptr);

    // 供外部设置：参考哪条数据序列进行坐标映射（通常是蜡烛序列）
    void setReferenceSeries(QtCharts::QAbstractSeries* s) { m_refSeries = s; }

    // 画线模式开关
    enum class DrawMode { None, TrendLine, HLine };
    void setDrawMode(DrawMode m);
    DrawMode drawMode() const { return m_mode; }

    // 访问画线层（保存/加载时可能用到）
    DrawLayer* drawLayer() const { return m_drawLayer.data(); }

signals:
    void crosshairMoved(const QPointF& valuePos); // 数据坐标

protected:
    void wheelEvent(QWheelEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void drawForeground(QPainter* p, const QRectF& rect) override;

private:
    bool m_panning = false;
    QPoint m_lastPos;
    QPoint m_cursorPos;
    QtCharts::QAbstractSeries* m_refSeries = nullptr;
    DrawMode m_mode = DrawMode::None;
    QScopedPointer<DrawLayer> m_drawLayer; // 管理注记对象
};