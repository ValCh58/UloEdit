#ifndef UNIELEMENT_99_H
#define UNIELEMENT_99_H

#include <QObject>
#include "andelement.h"
class UniElement_99 : public AndElement
{
public:
    UniElement_99(QRectF rect, int logo, QPointF point = QPointF(0.0, 0.0), QGraphicsItem * qgi = nullptr);
    ~UniElement_99();

    void buildElement(DataGraph &dtg);
    void makeUniElement(QPainter &qp);
};

#endif // UNIELEMENT_99_H
