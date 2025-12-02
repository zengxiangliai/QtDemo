#pragma once
#include <QVector>
#include <QPointF>
#include <QScopedPointer>
#include <QWidget>
class QPainter;
class QMouseEvent;

namespace QtCharts { class QChart; class QAbstractSeries; }

class DrawLayer {
public:
    enum class Mode { None, TrendLine, HLine };

    explicit DrawLayer(QWidget* viewWidget);

    void setMode(Mode m) { m_mode = m; m_tempActive = false; }

    void mousePress(QMouseEvent* e, QtCharts::QChart* chart, QtCharts::QAbstractSeries* ref);
    void mouseMove(QMouseEvent* e, QtCharts::QChart* chart, QtCharts::QAbstractSeries* ref);
    void mouseRelease(QMouseEvent* e, QtCharts::QChart* chart, QtCharts::QAbstractSeries* ref);

    void paint(QPainter* p, QtCharts::QChart* chart, QtCharts::QAbstractSeries* ref);

private:
    QWidget* m_view = nullptr; // 用于 update()
    Mode m_mode = Mode::None;

    struct Trend { QPointF aVal; QPointF bVal; }; // 数据坐标
    struct HLine { double yVal = 0.0; };

    QVector<Trend> m_trends;
    QVector<HLine> m_hlines;

    bool m_tempActive = false; // 是否在绘制中的临时线
    QPointF m_tmpAVal; // 数据坐标
    QPointF m_tmpBVal; // 数据坐标
};