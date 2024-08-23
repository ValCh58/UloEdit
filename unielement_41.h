#ifndef UNIELEMENT_41_H
#define UNIELEMENT_41_H

#include "andelement.h"
class UniElement_41 : public AndElement
{
public:
    UniElement_41(QRectF rect, int logo, QPointF point = QPointF(0.0, 0.0), QGraphicsItem * qgi = nullptr);
    ~UniElement_41();

    void buildElement(DataGraph &dtg);
    void makeUniElement(QPainter &qp);
};

#endif // UNIELEMENT_41_H
