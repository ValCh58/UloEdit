#include "andelement.h"

AndElement::AndElement(QRectF rect, QPointF point, QGraphicsItem *qgi):CustomElement(qgi)
{
    setSize(rect.width(), rect.height());
    setHotspot(QPoint(rect.x(), rect.y()));
    localPos = point;
    typeEl = AND;//Установим тип элемента//
    logoType = "&";
    //crElem = &makeAndElement;
}

AndElement::~AndElement()
{

}

void AndElement::makeAndElement(QPainter &qp)
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

//Создание терминала/////////////////////////////////////////////////////////////////////////
void AndElement::makeTerminal(Terminal *t, Qt::GlobalColor color, QPainter &qp)
{
    QString str = operandToText(t->getName());
    makeText(t->getP1().x()+Terminal::deltaXText-4,  t->getP1().y()-Terminal::deltaYText, qp, str, 8, color);
    makeLine(t->getP1(), t->getP2(), qp, color);
    if(!t->typeInv){
       QPointF tmp(t->getP2().x()-4.0, t->getP2().y());//Сместим кружок инверсии влево на -4!//
       makeCircle(tmp, qp, color);
    }
}//----------------------------------------------------------------------------------------//


void AndElement::buildElement(DataGraph &dtg)
{
    QPainter qp;
    qp.begin(&drawing);
    makeAndElement(qp);
    adjustTermElement(qp,dtg);
    qp.end();
}

//Получить индекс терминала в списке терминалов по параметру t////////////////////////////////
int AndElement::getIndex(Terminal *t)
{
    int ret=-1;

    for(int i=0; i<listTerminals.size(); i++){
        if(listTerminals.at(i)==t && listTerminals.at(i)->getOri()==Ulo::Orientation::East){
           ret = i;
           break;
        }
    }

    return ret;
}//-----------------------------------------------------------------------------------------//

//Обмен терминалов///////////////////////////////////////////////////////////////////////////
void AndElement::swapCoordTerm(Terminal *term1, Terminal *term2, QPainter &qp)
{
    if(term1->getOri() != term2->getOri()){return;}//Оба теминала должны быть или вх или вых//

     QPointF tempP1(term2->getP1());
     QPointF tempP2(term2->getP2());
     makeTerminal(term1, Qt::white, qp);//Очистка терминала
     makeTerminal(term2, Qt::white, qp);//
     term2->set_P1(term1->getP1());
     term2->set_P2(term1->getP2());
     term1->set_P1(tempP1);
     term1->set_P2(tempP2);
     makeTerminal(term1, Qt::black, qp);//Отрисовка терминала
     makeTerminal(term2, Qt::black, qp);//

}//----------------------------------------------------------------------------------------//

void AndElement::makeAnyElement(QPainter &qp)
{
    makeAndElement(qp);
}//----------------------------------------------------------------------------------------//

//Корреция элементов с тремя терминалами. Два входа один выход.//////////////////////////////
void AndElement::adjustElem3Term(QPainter &qp, DataGraph &dtg)
{
    bool flagName=false;
    Terminal *t1, *t2;
    const QList<Terminal*>& lTerm = this->getListTerminals();

    for(int idxOut=0; idxOut<lTerm.size(); idxOut++){//Входные терминалы//
        if(lTerm.at(idxOut)->getOri() == Ulo::Orientation::East)
           continue;
        for(int idxIn=0; idxIn<lTerm.size(); idxIn++){//Выходные терминалы//
            if(lTerm.at(idxIn)->getOri() == Ulo::Orientation::West)
               continue;
            if(!lTerm.at(idxOut)->getName().isEmpty() && lTerm.at(idxOut)->getName() == lTerm.at(idxIn)->getName()){
               flagName=true;
               t1 = lTerm.at(idxOut);//Найдем одноименный терминал//
            }
            else if(lTerm.at(idxOut)->getName() != lTerm.at(idxIn)->getName()){
               t2 = lTerm.at(idxOut);//Найдем одиночный терминал//
            }
        }
    }
    if(!flagName){return;}

    if(t1->getP1().y() > t2->getP1().y()){
       swapCoordTerm(t1, t2, qp);//Обмен терминалов местами в элементе//
    }
    //Направим линию обратной связи через верх элемента//////////////////////////////////////
    dtg.setTempWalkX(t1->getP1().x(), t1->getP1().x(), t1->getP1().y()+SchemeAlgo::yGrid, 1);
}

//Получение типа элемента для коррекции терминалов////////////////////////////////////////////
int AndElement::elemTypeAdust()
{
    int retVal=0;

    if(this->getCntTermWest()==2 && this->getCntTermEast()==1){
       retVal=1;
    }

    return retVal;
}

//Коррекция терминалов элементов/////////////////////////////////////////////////////////////
void AndElement::adjustTermElement(QPainter &qp, DataGraph &dtg)
{
    int adust = elemTypeAdust();

    switch(adust){
    case 1:
        adjustElem3Term(qp, dtg);
        break;
    }
}//----------------------------------------------------------------------------------------//



