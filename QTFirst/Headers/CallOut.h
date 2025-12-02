#pragma once

#include <QGraphicsItem>
#include <QFont>
namespace QtCharts { class QChart; }

class CallOut  : public QGraphicsItem
{
public:
    explicit CallOut(QtCharts::QChart* chart);
    ~CallOut() override;

    void setText(const QString& text);
    void setAnchor(const QPointF& point);
    void updateGeometry();

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    QtCharts::QChart* m_chart;
    QString m_text;
    QRectF m_textRect;
    QRectF m_rect;
    QPointF m_anchor; // ÏñËØ×ø±ê
    QFont m_font;
};

