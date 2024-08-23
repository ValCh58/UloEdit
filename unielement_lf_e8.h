#ifndef UNIELEMENT_LF_E8_H
#define UNIELEMENT_LF_E8_H

#include <QObject>
#include "andelement.h"


class UniElement_LF_E8 : public AndElement
{
public:
    UniElement_LF_E8(QRectF rect, int logo, QPointF point = QPointF(0.0, 0.0), QGraphicsItem * qgi = nullptr);
    ~UniElement_LF_E8();

    void buildElement(DataGraph &dtg);
    void makeUniElement(QPainter &qp);
};

#endif // UNIELEMENT_LF_E8_H
