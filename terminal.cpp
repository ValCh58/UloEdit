#include "terminal.h"
#include "conductor.h"
#include "mainwindow.h"

#include "customelement.h"

const qreal Terminal::termLen = 40.0;
const qreal Terminal::deltaY = 16.0;
const qreal Terminal::deltaXText = 7;
const qreal Terminal::deltaYText = 2;

Terminal::Terminal(UloData *dat, int num, QObject *parent) : QObject(parent)
{
    cElem = static_cast<CustomElement*>(parent);
    number = num;
    setUloDat(dat);
    typePrimElUni = dat->getCodOper().toInt();//Логический тип первой части универсального элемента//
    numInputUni = dat->getLogCellCommand().toInt();//Номер входа первичного элемента в универсальный элемент//
    koMult = 0;
    init();
}

Terminal::Terminal(int logotg, UloData *dat, int num, QObject *parent) : QObject(parent)
{
    cElem = static_cast<CustomElement*>(parent);
    number = num;
    setUloDat(dat);
    koMult = 0;
    initTg(logotg);
}

Terminal::~Terminal()
{

}
Ulo::Orientation Terminal::getOri() const
{
    return ori;
}

int Terminal::getNumber() const
{
    return number;
}

void Terminal::setNumber(int value)
{
    number = value;
}

QString Terminal::getName() const
{
    return name;
}

void Terminal::setName(const QString &value)
{

    name = operandToText(value);
}

QPointF Terminal::getP2() const
{
    return p2;
}

//Установки для второй точки линии////////
void Terminal::setP2(const QPointF &value)
{
    QPointF p(value.x(), value.y() + Terminal::deltaY * koMult);
    p2 = p;
}

void Terminal::set_P2(QPointF value)
{
    p2 = value;
}

void Terminal::setP2Tg(const QPointF &value)
{
    p2=value;
}

QPointF Terminal::getP1() const
{
    return p1;
}

//Установки для первой точки линии////////
void Terminal::setP1(const QPointF &value)
{
    QPointF p(value.x() - Terminal::termLen, value.y() + Terminal::deltaY * koMult);
    p1 = p;
}

void Terminal::set_P1(QPointF value)
{
    p1 = value;
}

void Terminal::setP1Tg(const QPointF &value)
{
    p1=value;
}

//Получим точку входа или точку выхода в зависимости от терминала////////////////////////////
QPointF Terminal::getDesiredPoint()
{
    if(ori==Ulo::East)
       return getP2();
    if(ori==Ulo::West)
       return getP1();
    return QPointF();
}


Terminal *Terminal::alignedWithTerminal() const
{

    return nullptr;
}

QString Terminal::operandToText(QString oprd)
{
    bool ok;
    QString text = oprd.right(3);
    int result = text.toInt(&ok, 16);
    QString strRet;
    QString pathDir;

    if(uloDat.getOperCommand().indexOf("BVR") == 0 && uloDat.getCodOper().toInt() == O_UT){
        //Читаем файл cfg
        pathDir = MainWindow::cfgDir+"discret.cfg";
        strRet = getNameBVR(pathDir, oprd);

        return (strRet.isNull()?"cfg???":strRet);
    }

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

//Чтение данных из файла discret.cfg////////////////////////
QString Terminal::getNameBVR(QString fileName, QString addr)
{
    QString ret;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return nullptr;

       QTextStream in(&file);
       while (!in.atEnd()) {
           QString line = in.readLine();
           //qDebug()<<line;
           ret = returnNameBVR(line, addr);
           if(!ret.isNull())
              break;
       }

       file.close();

       return ret;
}

//Возврат наименования выхода БВР из строки////////////////
QString Terminal::returnNameBVR(QString line, QString addr)
{
    QStringList list;
    QString ret=nullptr;

    if(line.indexOf(addr) > 0){
       list = line.split('\t', QString::SkipEmptyParts);
       ret = list.at(3);
    }

    return ret;
}


void Terminal::init()
{
    typeInv = getTypeInv();
    setNumber(number);
    setName(uloDat.getOperandCommand());
    if(uloDat.getCodOper().toInt() == O_UT){
       ori = Ulo::Orientation::East;
       koMult = cElem->getCntTermEast()+1;
       cElem->setCntTermEast(koMult);//Счетчик выходов элемента//
    }else{
       ori = Ulo::Orientation::West;
       koMult = cElem->getCntTermWest()+1;
       cElem->setCntTermWest(koMult);//Счетчик входов элемента//
    }
}

void Terminal::initTg(int logo)
{
    Q_UNUSED(logo)

    typeInv = true;
    if(uloDat.getCodOper().toInt() == 6 && uloDat.getLogCellCommand().toInt() == 3){
       return;
    }
    setNumber(number);
    setName(uloDat.getOperandCommand());
    if(uloDat.getCodOper().toInt() == 5){
       ori = Ulo::Orientation::East;
       koMult = cElem->getCntTermEast()+1;
       cElem->setCntTermEast(koMult);//Счетчик выходов элемента//
    }else if(uloDat.getCodOper().toInt() == 6){
       ori = Ulo::Orientation::West;
       if(uloDat.getLogCellCommand().toInt()==1)
          inpTg = IN_S;
       else if(uloDat.getLogCellCommand().toInt()==2)
          inpTg = IN_R;
       else if(uloDat.getLogCellCommand().toInt()==3)
          inpTg = IN_X;
       koMult = cElem->getCntTermWest()+1;
       cElem->setCntTermWest(koMult);//Счетчик входов элемента//
    }
}

int Terminal::getInpTg() const
{
    return inpTg;
}

void Terminal::setInpTg(int value)
{
    inpTg = value;
}


//Получим тип входа true-прямой, false-инверсный//
bool Terminal::getTypeInv()
{
    int codOper = uloDat.getCodOper().toInt();

    switch(codOper){
    case AND_N:
    case  OR_N:
        return false;
    }

    return true;
}

int Terminal::getKoMult() const
{
    return koMult;
}

void Terminal::setKoMult(int value)
{
    koMult = value;
}

void Terminal::setTypePrimElUni(int value)
{
    typePrimElUni = value;
}

void Terminal::setNumInputUni(int value)
{
    numInputUni = value;
}

QList<Conductor *>& Terminal::conductors()
{
    return conductors_;
}

int Terminal::getTypePrimElUni() const
{
    return typePrimElUni;
}


int Terminal::getNumInputUni() const
{
    return numInputUni;
}


void Terminal::setUloDat(UloData *d)
{
    uloDat.setCodHex(d->getCodHex());
    uloDat.setCodOper(d->getCodOper());
    uloDat.setLogCellCommand(d->getLogCellCommand());
    uloDat.setNumCommandHex(d->getNumCommandHex());
    uloDat.setOperandCommand(d->getOperandCommand());
    uloDat.setOperCommand(d->getOperCommand());
}

