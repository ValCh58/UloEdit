#ifndef TGRELEMENT_H
#define TGRELEMENT_H

#include <QObject>
#include <QPainter>

#include "customelement.h"
#include "terminal.h"

class TgrElement : public CustomElement
{
public:
    TgrElement(QRectF rect, int logo, QPointF point = QPointF(0.0, 0.0), QGraphicsItem * qgi = 0);
    ~TgrElement();

    void makeTgrElement(QPainter &qp);
    void buildElement(DataGraph &dtg);
    QString getNameLogo1El();
    QString getNameLogo2El();

private:

};

#endif // TGRELEMENT_H
