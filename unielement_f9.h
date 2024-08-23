#ifndef UNIELEMENT_F9_H
#define UNIELEMENT_F9_H

#include <QObject>
#include "andelement.h"

class UniElement_F9 : public AndElement
{
    //Q_OBJECT
public:
    UniElement_F9(QRectF rect, int logo, QPointF point = QPointF(0.0, 0.0), QGraphicsItem * qgi = nullptr);
    ~UniElement_F9();

    void buildElement(DataGraph &dtg);
    void makeUniElement(QPainter &qp);
};

#endif // UNIELEMENT_F9_H
