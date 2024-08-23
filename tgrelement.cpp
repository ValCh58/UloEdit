#include "tgrelement.h"

TgrElement::TgrElement(QRectF rect, int logo, QPointF point, QGraphicsItem *qgi):CustomElement(qgi)
{
    setSize(rect.width(), rect.height());
    setHotspot(QPoint(rect.x(), rect.y()));
    localPos = point;
    typeEl = UNI_TG;//Установим тип элемента//

    switch(logo){
    case 0x32:
        logoType = "TR";
        break;
    case 0xB2:
        logoType = "TQ";
        break;
    case 0xBA:
        logoType = "TS";
        break;
    default:
        logoType = "T";
    }


}

TgrElement::~TgrElement()
{

}

void TgrElement::makeTgrElement(QPainter &qp)
{

    //localPos.setX(150.0);  localPos.setY(50.0);
    //makeRect(localPos.x(),  localPos.y(),  sizeElem.width(), sizeElem.height(), qp);
    makeRectRounded(localPos.x(),  localPos.y(),  sizeElem.width(), sizeElem.height(), qp, 5.0, 5.0);
    makeRect(localPos.x(),  localPos.y(),  17, sizeElem.height(), qp);
    QPointF p1(localPos.x(),  localPos.y()+sizeElem.height()/2);
    QPointF p2(localPos.x()+17,  localPos.y()+sizeElem.height()/2);
    makeLine(p1, p2, qp);
    makeText(localPos.x()+4, localPos.y()+sizeElem.height()/3.3, qp, "S", 9);
    makeText(localPos.x()+4,  localPos.y()+sizeElem.height()/1.25, qp, "R", 9);
    //Укажем тип элемента и имя//
    makeText(localPos.x()+sizeElem.width()/2-1,  localPos.y()+Terminal::deltaY*2-7, qp, getNameLogo1El(), 13);
    makeText(localPos.x()+sizeElem.width()/2+8,  localPos.y()+Terminal::deltaY*2-7, qp, getNameLogo2El(), 7);
    makeText(localPos.x()+sizeElem.width()/2-14,  localPos.y()-3, qp, getNameEl(), 9);

    //Строим входы элемента////////////////////
    for(Terminal *t:listTerminals){
        if(t->ori == Ulo::Orientation::West){
           QString str = operandToText(t->getName());
           qreal dY = localPos.y()+(sizeElem.height()*(t->getInpTg()==IN_S?0.25:0.75));
           QPointF p1(localPos.x()-Terminal::termLen, dY);
           QPointF p2(localPos.x(), dY);
           makeText(p1.x()+Terminal::deltaXText,  p1.y()-Terminal::deltaYText, qp, str, 8);
           makeLine(p1, p2, qp);
           t->setP1Tg(p1);
           t->setP2Tg(p2);
        }
    }

    //Строим выходы элемента////////////////////
    qreal partY = sizeElem.height()-(cntTermEast*Terminal::deltaY);
    qreal deltaH = (partY/2)-(Terminal::deltaY/2);

    for(Terminal *t:listTerminals){
        if(t->ori == Ulo::Orientation::East){
           QPointF point(localPos.x()+sizeElem.width()+Terminal::termLen, localPos.y()+deltaH);
           t->setP1(point); t->setP2(point);
           QString str = operandToText(t->getName());
           makeText(t->getP1().x()+Terminal::deltaXText,  t->getP1().y()-Terminal::deltaYText, qp, str, 8);
           makeLine(t->getP1(), t->getP2(), qp);
        }
    }
}


void TgrElement::buildElement(DataGraph &dtg)
{
    QPainter qp;
    qp.begin(&drawing);
    makeTgrElement(qp);
    qp.end();
}

QString TgrElement::getNameLogo1El()
{
    return logoType.left(1);
}

QString TgrElement::getNameLogo2El()
{
    return logoType.right(1);
}

