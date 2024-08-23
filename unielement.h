#ifndef UNIELEMENT_H
#define UNIELEMENT_H

#include <QObject>
#include "andelement.h"


class UniElement : public AndElement
{
public:
    UniElement(QRectF rect, int logo, QPointF point = QPointF(0.0, 0.0), QGraphicsItem * qgi = nullptr);
    ~UniElement();

    void buildElement(DataGraph &dtg);
    void makeUniElement(QPainter &qp);
private:
    QString getTypeAndOr(int logo);
};

#endif // UNIELEMENT_H
