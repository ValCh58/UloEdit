#ifndef TMRELEMENT_H
#define TMRELEMENT_H

#include <QObject>
#include "andelement.h"

class TmrElement : public AndElement
{
public:
    TmrElement(QRectF rect, QPointF point = QPointF(0.0, 0.0), QGraphicsItem * qgi = 0);
    ~TmrElement();

    void buildElement(DataGraph &dtg);
    void makeTmrElement(QPainter &qp);
private:
    QString getTimerVal(QString fileName, int idx);
    QString getTypeTime(int val);
};

#endif // TMRELEMENT_H
