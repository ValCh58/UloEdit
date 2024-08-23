#include "customelement.h"
#include "terminal.h"


CustomElement::CustomElement(QGraphicsItem *qgi):QGraphicsObject(qgi)
{
    //setSize(720, 640);
    //setHotspot(QPoint(5, 5));
    sizeElem.setWidth(56.0);//Ширина эелемента по умолчанию//
    cntTermWest = 0;//Количество терминалов на входе//
    cntTermEast = 0;//Количество терминалов на выходе//

}

CustomElement::~CustomElement()
{}
QPicture CustomElement::getDrawing() const
{
    return drawing;
}

QRectF CustomElement::boundingRect() const
{
    return(QRectF(QPointF(hotspot_coord.x(), hotspot_coord.y()), dimensions));
}

void CustomElement::paint(QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget)
{
    Q_UNUSED(options)
    Q_UNUSED(widget)

    drawing.play(painter);
}
QList<Terminal *> CustomElement::getListTerminals() const
{
    return listTerminals;
}

void CustomElement::setListTerminals(const QList<Terminal *> &value)
{
    listTerminals = value;
}


QSize CustomElement::setSize(int wid, int hei)
{
    prepareGeometryChange();
    while (wid % 10) ++ wid;
    while (hei % 10) ++ hei;

    return(dimensions = QSize(wid, hei));
}

QPoint CustomElement::setHotspot(QPoint hs)
{
    prepareGeometryChange();
    if (dimensions.isNull()) hotspot_coord = QPoint(0, 0);
    else {
        int hsx = qMin(hs.x(), dimensions.width());
        int hsy = qMin(hs.y(), dimensions.height());
        hotspot_coord = QPoint(hsx, hsy);
    }
    return(hotspot_coord);
}

void CustomElement::makeLine(QPointF p1, QPointF p2, QPainter &qp, Qt::GlobalColor color)
{
    qp.save();
    QPen pen;
    pen.setColor(color/*Qt::black*/);
    pen.setWidth(1);
    pen.setCosmetic(true);
    qp.setPen(pen);
    qp.drawLine(p1, p2);
    qp.restore();
}

bool CustomElement::makeRect(qreal x, qreal y, qreal w, qreal h, QPainter &qp)
{
    qreal rect_x = x, rect_y = y, rect_w = w, rect_h = h;

    qp.save();
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(1);
    pen.setCosmetic(true);
    qp.setPen(pen);
    qp.setBrush(QBrush(QColor(Qt::lightGray)));
    qp.drawRect(QRectF(rect_x, rect_y, rect_w, rect_h));
    qp.restore();
    return true;
}

bool CustomElement::makeRectRounded(qreal x, qreal y, qreal w, qreal h, QPainter &qp, qreal xRadius, qreal yRadius, Qt::SizeMode mode)
{
    qreal rect_x = x, rect_y = y, rect_w = w, rect_h = h;

    qp.save();
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(1);
    pen.setCosmetic(true);
    qp.setPen(pen);
    qp.setBrush(QBrush(QColor(Qt::lightGray)));
    qp.drawRoundedRect(QRectF(rect_x, rect_y, rect_w, rect_h), xRadius, yRadius, mode);
    qp.restore();
    return true;
}

void CustomElement::makeCircle(QPointF p, QPainter &qp, Qt::GlobalColor color)
{
    qp.save();
    QPen pen;
    pen.setColor(color);
    pen.setWidth(1);
    pen.setCosmetic(true);
    qp.setPen(pen);
    qp.setBrush(QBrush(QColor(Qt::white)));
    qp.drawEllipse(p, 4, 4);
    qp.restore();
}

void CustomElement::makeText(qreal x, qreal y, QPainter &qp, QString text, int size, Qt::GlobalColor color)
{
    qp.save();
    qp.setFont(QFont("Arial", size, QFont::Black));
    qp.setPen(color);
    qp.setBackground(QBrush(QColor(Qt::white)));
    qp.drawText(x, y, text);
}

QString CustomElement::operandToText(QString oprd)
{
    bool ok;
    QString text = oprd.right(3);
    int result = text.toInt(&ok, 16);
    QString strRet = oprd;


    if(ok && result >= 0x600 && result <= 0x7ff){
       return ("X" + text);
    }else if(ok && result >= 0x400 && result <= 0x5ff){
       return ("Y" + text);
    }else if(ok && result >= 0x000 && result <= 0x1ff){
       return ("Z" + text);
    }else if(ok && result >= 0x200 && result <= 0x2ff){
       return ("T" + text);
    }

    return "";
}
int CustomElement::getCodLF() const
{
    return codLF;
}

void CustomElement::setCodLF(int value)
{
    codLF = value;
}


void CustomElement::swapCoordTerm(Terminal *term1, Terminal *term2, QPainter &qp){}

/*/Сортируем терминалы Вх. = Вых. //13-07-2017/////////////////////////////////////////////////
void CustomElement::sortListTerminals()//Не используется
{
    int idxOut, idxIn;
    QList<Terminal*> listWest, listEast;

    //Разделение терминалов по разным спискам////////////////////////////////////////////////
    for(idxOut=0; idxOut<listTerminals.size(); idxOut++){
        if(listTerminals.at(idxOut)->getOri() == Ulo::Orientation::East)
           continue;
        for(idxIn=0; idxIn<listTerminals.size(); idxIn++){
            if(listTerminals.at(idxOut)->getName() == listTerminals.at(idxIn)->getName()&&
                               listTerminals.at(idxIn)->getOri() == Ulo::Orientation::East){
                  listWest.append(listTerminals.at(idxOut));//Входы//
                  listEast.append(listTerminals.at(idxIn)); //Выходы//
            }
        }
    }//-------------------------------------------------------------------------------------//

    //Упорядочивание вых терминалов от мин до мах/////////////////////////////////////////////
    for(int indEast=0; indEast<listEast.size(); indEast++){
        for(int indMin=indEast; indMin<listEast.size(); indMin++){
            if(listEast.at(indEast)->getP1().y() > listEast.at(indMin)->getP1().y()){
               swapCoordTerm(listEast.at(indEast), listEast.at(indMin));
            }
        }
    }//-------------------------------------------------------------------------------------//

    for(int i=0; i<listTerminals.size(); i++){
        for(int j=0; j<listEast.size(); j++){
            int ind = getIndex(listEast.at(j));
            if(ind != -1){
               listTerminals.at(ind)->set_P1(listEast.at(j)->getP1());
               listTerminals.at(ind)->set_P2(listEast.at(j)->getP2());
            }
        }
    }
    int deb=0;
}//----------------------------------------------------------------------------------------/*/

/*/Коррекция терминалов элементов/////////////////////////////////////////////////////////////
void CustomElement::adjustTermElement(CustomElement *el)
Перенесено в AndElement/////////////////
{
    int adust = elemTypeAdust(el);

    switch(adust){
    case 1:
        adjustElem3Term(el);
        break;
    }

}//----------------------------------------------------------------------------------------/*/

/*/Корреция элементов с тремя терминалами. Два входа один выход./////////////////////////////
void CustomElement::adjustElem3Term(CustomElement *el)
Перенесено в AndElement/////////////////
{
    bool flagName=false;
    Terminal *t1, *t2;
    const QList<Terminal*>& lTerm = el->getListTerminals();

    for(int idxOut=0; idxOut<lTerm.size(); idxOut++){//Входные терминалы//
        if(lTerm.at(idxOut)->getOri() == Ulo::Orientation::East)
           continue;
        for(int idxIn=0; idxIn<lTerm.size(); idxIn++){//Выходные терминалы//
            if(lTerm.at(idxIn)->getOri() == Ulo::Orientation::West)
               continue;
            if(lTerm.at(idxOut)->getName() == lTerm.at(idxIn)->getName()){
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
       swapCoordTerm(t1, t2);
       int r=0;
    }

}//---------------------------------------------------------------------------------------/*/

/*/Получение типа элемента для коррекции терминалов///////////////////////////////////////////
Перенесено в AndElement/////////////////
int CustomElement::elemTypeAdust(CustomElement *el)
{
    int retVal=0;

    if(el->getCntTermWest()==2 && el->getCntTermEast()==1){
       retVal=1;
    }

    return retVal;

}//----------------------------------------------------------------------------------------/*/


/*/Получить индекс терминала в списке терминалов по параметру t///////////////////////////////
Перенесено в AndElement/////////////////

int CustomElement::getIndex(Terminal* t)
{
    int ret=-1;

    for(int i=0; i<listTerminals.size(); i++){
        if(listTerminals.at(i)==t && listTerminals.at(i)->getOri()==Ulo::Orientation::East){
           ret = i;
           break;
        }
    }

    return ret;
}//----------------------------------------------------------------------------------------/*/

/*/Объмен координатами терминалов//13-07-2017/////////////////////////////////////////////////
Перенесено в AndElement/////////////////

void CustomElement::swapCoordTerm(Terminal *term1, Terminal *term2)
{
    if(term1->getOri() != term2->getOri()){return;}//Оба теминала должны быть или вх или вых//

     QPointF tempP1(term2->getP1());
     QPointF tempP2(term2->getP2());
     term2->set_P1(term1->getP1());
     term2->set_P2(term1->getP2());
     term1->set_P1(tempP1);
     term1->set_P2(tempP2);

}//----------------------------------------------------------------------------------------/*/




QString CustomElement::getNameEl() const
{
    return nameEl;
}

void CustomElement::setNameEl(const QString &value)
{
    nameEl = value;
}

void CustomElement::makeAnyElement(QPainter &qp){}

bool CustomElement::isInputElem()
{
    bool ret = false;

    if(listTerminals.size() < 2)
       return ret;
    for(Terminal *t : listTerminals){
        if(t->ori == Ulo::West && t->uloDat.getOperCommand()=="INPUT"){
           ret = true;
           break;
        }
    }

    return ret;
}



QSizeF &CustomElement::getRefElem() const
{
    return refElem;
}

void CustomElement::buildElement(DataGraph &dtg){}


QPointF CustomElement::getLocalPos() const
{
    return localPos;
}

void CustomElement::setLocalPos(const QPointF &value)
{
    localPos = value;
}

QSizeF CustomElement::getSizeElem() const
{
    return sizeElem;
}

void CustomElement::setSizeElem(const QSizeF &value)
{
    sizeElem = value;
}

int CustomElement::getTypeEl() const
{
    return typeEl;
}

int CustomElement::getCntTermEast() const
{
    return cntTermEast;
}

void CustomElement::setCntTermEast(int value)
{
    cntTermEast = value;
}

int CustomElement::getCntTermWest() const
{
    return cntTermWest;
}

void CustomElement::setCntTermWest(int value)
{
    cntTermWest = value;
}










