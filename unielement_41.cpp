#include "unielement_41.h"

UniElement_41::UniElement_41(QRectF rect, int logo, QPointF point, QGraphicsItem *qgi):AndElement(rect, point, qgi)
{
    if(logo == 0x41){
       logoType = "=if";
    }
    else{
       logoType = "*";
    }

    typeEl = UNI_99;//Установим тип элемента//
}

UniElement_41::~UniElement_41()
{

}

void UniElement_41::buildElement(DataGraph &dtg)
{
    QPainter qp;
    qp.begin(&drawing);
    makeUniElement(qp);
    adjustTermElement(qp,dtg);
    qp.end();
}

void UniElement_41::makeUniElement(QPainter &qp)
{
    //Строим прямоугольник элемента/////////////////////////////////////////////////
    //makeRect(localPos.x(),  localPos.y(),  sizeElem.width(), sizeElem.height(), qp);
    makeRectRounded(localPos.x(),  localPos.y(),  sizeElem.width(), sizeElem.height(), qp, 5.0, 5.0);
    //Укажем тип элемента и имя//
    makeText(localPos.x()+sizeElem.width()/2-7,  localPos.y()+Terminal::deltaY*2-9, qp, logoType, 13);
    makeText(localPos.x()+sizeElem.width()/2-14,  localPos.y()-3, qp, getNameEl(), 9);

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
           QString str = operandToText(t->getName());
           makeText(t->getP1().x()+Terminal::deltaXText-4,  t->getP1().y()-Terminal::deltaYText, qp, str, 8);
           makeLine(t->getP1(), t->getP2(), qp);
        }
    }
}
