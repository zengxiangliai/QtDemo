#include "CallOut.h"
#include <QtCharts/QChart>
#include <QPainter>
#include <QFontMetrics>
//悬浮弹窗提示基类，继承自QGraphicsItem

CallOut::CallOut(QtCharts::QChart* chart)
    : QGraphicsItem(chart), m_chart(chart)
{
    setZValue(1000);
    m_font = QFont("Microsoft YaHei", 9);
}

CallOut::~CallOut() {}

QRectF CallOut::boundingRect() const
{
    QPointF anchor = mapFromParent(m_anchor);
    QRectF rect;
    rect.setLeft(qMin(m_rect.left(), anchor.x()));
    rect.setRight(qMax(m_rect.right(), anchor.x()));
    rect.setTop(qMin(m_rect.top(), anchor.y()));
    rect.setBottom(qMax(m_rect.bottom(), anchor.y()));
    return rect;
}

void CallOut::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*)
{
    QPainterPath path;
    path.addRoundedRect(m_rect, 5, 5);

    QPointF anchor = mapFromParent(m_anchor);
    if (!m_rect.contains(anchor)) {
        QPointF point1, point2;
        // 画一个小三角指向 anchor
        if (anchor.x() > m_rect.right()) {
            point1 = QPointF(m_rect.right(), m_rect.center().y() - 10);
            point2 = QPointF(m_rect.right(), m_rect.center().y() + 10);
        }
        else if (anchor.x() < m_rect.left()) {
            point1 = QPointF(m_rect.left(), m_rect.center().y() - 10);
            point2 = QPointF(m_rect.left(), m_rect.center().y() + 10);
        }
        else if (anchor.y() > m_rect.bottom()) {
            point1 = QPointF(m_rect.center().x() - 10, m_rect.bottom());
            point2 = QPointF(m_rect.center().x() + 10, m_rect.bottom());
        }
        else {
            point1 = QPointF(m_rect.center().x() - 10, m_rect.top());
            point2 = QPointF(m_rect.center().x() + 10, m_rect.top());
        }
        path.moveTo(anchor);
        path.lineTo(point1);
        path.lineTo(point2);
        path.closeSubpath();
    }

    p->setPen(QPen(Qt::black));
    p->setBrush(QColor(255, 255, 224));
    p->drawPath(path);
    p->setFont(m_font);
    p->drawText(m_textRect, m_text);
}

void CallOut::setText(const QString& text)
{
    m_text = text;
    QFontMetrics fm(m_font);
    QRectF container(m_anchor, QSize(300, 200));
    m_textRect = fm.boundingRect(container.toRect(), Qt::AlignLeft | Qt::AlignVCenter, m_text);
    m_textRect.adjust(5, 5, 5, 5);
}

void CallOut::setAnchor(const QPointF& point)
{
    m_anchor = point; // 像素坐标
}

void CallOut::updateGeometry()
{
    prepareGeometryChange();
    m_rect = m_textRect.adjusted(-10, -10, 10, 10);
}
