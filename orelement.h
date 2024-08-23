#ifndef ORELEMENT_H
#define ORELEMENT_H

#include <QObject>

#include "andelement.h"

class OrElement : public AndElement
{
public:
    OrElement(QRectF rect, QPointF point = QPointF(0.0, 0.0), QGraphicsItem * qgi = 0);
    ~OrElement();
};

#endif // ORELEMENT_H
