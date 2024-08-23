#include "orelement.h"

OrElement::OrElement(QRectF rect, QPointF point, QGraphicsItem *qgi):AndElement(rect, point, qgi)
{
    typeEl = OR;//Установим тип элемента//
    logoType = "1";
}

OrElement::~OrElement()
{

}

