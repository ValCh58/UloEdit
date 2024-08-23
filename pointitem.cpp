#include "pointitem.h"

PointItem::PointItem(QGraphicsItem *parent):QAbstractGraphicsShapeItem(parent)
{}

PointItem::~PointItem()
{}

QRectF PointItem::boundingRect() const
{
    qreal penWidth = this->pen().widthF()/2;
    return QRectF(-penWidth, -penWidth, penWidth, penWidth);
}

void PointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(QPen(Qt::red,2));
    painter->setBrush(this->brush());
    painter->drawPoint(0, 0);
}

