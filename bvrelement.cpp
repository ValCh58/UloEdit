#include "bvrelement.h"

BvrElement::BvrElement(QRectF rect, QPointF point, QGraphicsItem *qgi):AndElement(rect, point, qgi)
{
    typeEl = BVR;//Установим тип элемента//
    static const QChar data[1] = {9655};
    QString str(data, 1);
    logoType = str;

}

BvrElement::~BvrElement()
{

}

void BvrElement::buildElement(DataGraph &dtg)
{
    Q_UNUSED(dtg)

    QPainter qp;
    qp.begin(&drawing);
    makeBvrElement(qp);
    qp.end();
}


void BvrElement::makeBvrElement(QPainter &qp)
{
    //Строим прямоугольник элемента/////////////////////////////////////////////////
    makeRect(localPos.x(),  localPos.y(),  sizeElem.width(), sizeElem.height(), qp);
    //Укажем тип элемента и имя//
    makeText(localPos.x()+sizeElem.width()/2-7,  localPos.y()+Terminal::deltaY*2-5, qp, logoType, 25);
    //makeText(localPos.x()+sizeElem.width()/2-14,  localPos.y()-3, qp, getNameEl(), 9);

    //Строим входы элемента///////////////////////////////////////////////////////////////////////////
    qreal partY1 = sizeElem.height()-(cntTermWest*Terminal::deltaY);
    qreal deltaH1 = (partY1/2)-(Terminal::deltaY/2);

    for(Terminal *t:listTerminals){
        if(t->ori == Ulo::Orientation::West){
           QPointF point1(localPos.x(), localPos.y()+deltaH1);
           t->setP1(point1); t->setP2(point1);//установка координат терминала
           QString str = operandToText(t->getName());
           makeText(t->getP1().x()+Terminal::deltaXText-4,  t->getP1().y()-Terminal::deltaYText, qp, str, 8);
           makeLine(t->getP1(), t->getP2(), qp);
           if(!t->typeInv){
              QPointF tmp(t->getP2().x()-4.0, t->getP2().y());//Сместим кружок инверсии влево -4!//
              makeCircle(tmp, qp);
           }
        }
    }

    //Строим выходы элемента//////////////////////////////////////////////////////////////////////////
    qreal partY = sizeElem.height()-(cntTermEast*Terminal::deltaY);
    qreal deltaH = (partY/2)-(Terminal::deltaY/2);

    for(Terminal *t:listTerminals){
        if(t->ori == Ulo::Orientation::East){
           QPointF point(localPos.x()+sizeElem.width()+Terminal::termLen, localPos.y()+deltaH);
           t->setP1(point); t->setP2(point);
           QString str = t->getName();//operandToText(t->getName());
           makeText(t->getP1().x()+Terminal::deltaXText-4,  t->getP1().y()-Terminal::deltaYText, qp, str, 8);
           makeLine(t->getP1(), t->getP2(), qp);
        }
    }
}


