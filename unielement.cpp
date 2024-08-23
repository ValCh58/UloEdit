#include "unielement.h"

UniElement::UniElement(QRectF rect, int logo, QPointF point, QGraphicsItem *qgi):AndElement(rect, point, qgi)
{
    typeEl = UNI;//Установим тип элемента//

    if(logo == 0xFE || logo == 0xEE){
       logoType = "1";
    }else if(logo == 0x80 || logo == 0x88){
       logoType = "&";
    }
    else{
       logoType = "*";
    }
}

UniElement::~UniElement()
{

}

void UniElement::buildElement(DataGraph &dtg)
{
    QPainter qp;
    qp.begin(&drawing);
    makeUniElement(qp);
    adjustTermElement(qp,dtg);
    qp.end();
}

void UniElement::makeUniElement(QPainter &qp)
{
    //Строим прямоугольник элемента/////////////////////////////////////////////////
    //makeRect(localPos.x(),  localPos.y(),  sizeElem.width(), sizeElem.height(), qp);
    makeRectRounded(localPos.x(),  localPos.y(),  sizeElem.width(), sizeElem.height(), qp, 5.0, 5.0);
    makeRect(localPos.x(),  localPos.y(),  sizeElem.width()/3, sizeElem.height(), qp);
    //Укажем тип элемента и имя//
    makeText(localPos.x()+sizeElem.width()/2+4,  localPos.y()+Terminal::deltaY*2-9, qp, logoType, 13);
    makeText(localPos.x()+sizeElem.width()/2-14,  localPos.y()-3, qp, getNameEl(), 9);

    //Строим входы элемента///////////////////////////////////////////////////////////////////////////
    qreal partY1 = sizeElem.height()-(cntTermWest*Terminal::deltaY);
    qreal deltaH1 = (partY1/2)-(Terminal::deltaY/2);
    int logoCurrent = 0;//Текущий логотип входного элем//
    int numInCurr = 0;//Текущий номер входа//
    int numInNext = 0;//Следующий номер входа//

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

           logoCurrent = t->getTypePrimElUni();
           if(numInCurr == 0 && numInNext == 0){//Начало отсчета входных элементов//
              QString type=getTypeAndOr(logoCurrent);
              makeText(localPos.x()+(sizeElem.width()/10),  t->getP1().y()+1, qp, type, 10);
              numInCurr = t->getNumInputUni();
              numInNext = t->getNumInputUni();
           }
           numInNext = t->getNumInputUni();
           if(numInCurr != numInNext){//Определение перехода на другой элемент//
              QString type=getTypeAndOr(logoCurrent);
              makeText(localPos.x()+(sizeElem.width()/10),  t->getP1().y()+8, qp, type, 10);//Рисуем логотип элемента//
              QPointF tmp2(t->getP2().x()+(sizeElem.width()/3), t->getP2().y()-(Terminal::deltaY/2));
              QPointF tmp1(t->getP2().x(), t->getP2().y()-(Terminal::deltaY/2));
              makeLine(tmp1, tmp2, qp);//Разделительная линия между входными элементами//
              numInCurr = t->getNumInputUni();
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
//Определение типа элемента AND/OR ///////
QString UniElement::getTypeAndOr(int logo)
{
    QString type="";

    if(logo == 0 || logo == 1){
       type = "1";
    }else if(logo == 2 || logo == 3){
        type = "&";
    }

    return type;
}

