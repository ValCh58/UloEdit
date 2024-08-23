#ifndef ANDELEMENT_H
#define ANDELEMENT_H

#include <QObject>
#include <QPainter>

#include "customelement.h"
#include "terminal.h"

class AndElement : public CustomElement
{
public:

    explicit AndElement(QRectF rect, QPointF point = QPointF(0.0, 0.0), QGraphicsItem * qgi = 0);
    virtual ~AndElement();
    void makeAndElement(QPainter &qp);
    void makeTerminal(Terminal *t, Qt::GlobalColor color, QPainter &qp);
    void buildElement(DataGraph &dtg);
    //Обмен координатами терминалов//
    void swapCoordTerm(Terminal *term1, Terminal *term2, QPainter &qp);
    void makeAnyElement(QPainter&qp);
protected:
    //Получить индекс терминала в списке терминалов по параметру t//
    int  getIndex(Terminal* t);
    //Корреция элементов с тремя терминалами. Два входа один выход.//
    void adjustElem3Term(QPainter &qp, DataGraph &dtg);
    //Получение типа элемента для коррекции терминалов//
    int  elemTypeAdust();
    //Коррекция терминалов элементов//
    void adjustTermElement(QPainter &qp, DataGraph &dtg);

};

#endif // ANDELEMENT_H
