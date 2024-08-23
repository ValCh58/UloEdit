#ifndef BVRELEMENT_H
#define BVRELEMENT_H

#include <QObject>
#include "andelement.h"

class BvrElement : public AndElement
{
public:
    BvrElement(QRectF rect, QPointF point = QPointF(0.0, 0.0), QGraphicsItem * qgi = 0);
    ~BvrElement();

    void buildElement(DataGraph &dtg);
    void makeBvrElement(QPainter &qp);
};

#endif // BVRELEMENT_H
