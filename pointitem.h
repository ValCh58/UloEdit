#ifndef POINTITEM_H
#define POINTITEM_H

#include "QAbstractGraphicsShapeItem"
#include <qpen.h>
#include <qpainter.h>

class PointItem : public QAbstractGraphicsShapeItem
{
public:
    PointItem(QGraphicsItem *parent=0);
    ~PointItem();

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

#endif // POINTITEM_H
