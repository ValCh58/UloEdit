#include "tmrelement.h"
#include "mainwindow.h"

TmrElement::TmrElement(QRectF rect, QPointF point, QGraphicsItem *qgi):AndElement(rect, point, qgi)
{
    typeEl = TMR;//Установим тип элемента//
    logoType = "|--------|";
}

TmrElement::~TmrElement()
{

}

void TmrElement::buildElement(DataGraph &dtg)
{
    QPainter qp;
    qp.begin(&drawing);
    makeTmrElement(qp);
    adjustTermElement(qp,dtg);
    qp.end();
}

void TmrElement::makeTmrElement(QPainter &qp)
{
    bool ok;
    //Строим прямоугольник элемента////////////////////////////////////////////////////////////////
    makeRectRounded(localPos.x(), localPos.y(), sizeElem.width(), sizeElem.height(), qp, 5.0, 5.0);
    //Укажем тип элемента и имя//
    makeText(localPos.x()+sizeElem.width()/2-15,  localPos.y()+Terminal::deltaY*2-5, qp, logoType, 7);
    QString nmTimer = operandToText(getNameEl());//T+operand//
    QString pathDir = MainWindow::cfgDir+"tmrset.bin";
    //Преобразовать адрес таймера ->nmTimer///////////////////////////////
    int addr = (nmTimer.right(nmTimer.length()-1).toInt(&ok,16) & 0x00FF)<<2;
    QString strTime = getTimerVal(pathDir, addr);
    makeText(localPos.x()+sizeElem.width()/2-6, localPos.y()+Terminal::deltaY*2-15, qp, strTime, 9);//Запись задержки таймера//
    makeText(localPos.x()+sizeElem.width()/2-14, localPos.y()-3, qp, nmTimer, 9);

    //Строим входы элемента/////////////////////////////////////////
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


QString TmrElement::getTimerVal(QString fileName, int idx)
{
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
        return nullptr;

    QByteArray byte = file.read(0x1FF+1);
    int val1 = byte[idx] & 0x00FF;
    int val2 = byte[idx+1] & 0x00FF;
    val2 = val2 << 8;
    int val = val1 | val2;
    file.close();

    return getTypeTime(val);
}

QString TmrElement::getTypeTime(int val)
{
    QString ret;

    if(val > 60){
        val = val/60;
        ret = ret.setNum(val)+'\'';
    }
    else{
        ret = ret.setNum(val)+'\"';
    }

    return ret;
}

