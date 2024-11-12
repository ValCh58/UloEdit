#include "buildelements.h"

#include <QPainter>

#include "schemealgo.h"
#include "ulodata.h"
#include "customelement.h"
#include "terminal.h"
#include "orelement.h"
#include "unielement.h"
#include "unielement_lf_e8.h"
#include "unielement_f9.h"
#include "unielement_99.h"
#include "unielement_41.h"
#include "tgrelement.h"
#include "tmrelement.h"
#include "bvrelement.h"
#include "bordertitleblock.h"
#include "conductor.h"
#include "geomet.h"
#include "parsingco.h"
#include "pointitem.h"

/**
 * @brief BuildElements::BuildElements
 * @param scheme
 * @param mapElem
 */
BuildElements::BuildElements(SchemeAlgo *scheme, QMap<int, QMap<int, UloData> > *mapElem)
{
    scene = scheme;
    ulodata = mapElem;
    paper = scene->border_and_titleblock.insideBorderRect();//размер области рисования схемы//
    indentEdge = 64;//60;//Отступ от края элемента по всему пиремeтру//
    startPoint.setX(-96);//-95);//(105);
    startPoint.setY(80);
    tmpPoint = startPoint;
    tmpDeltaY = 0;

}

/**
 * @brief BuildElements::~BuildElements
 */
BuildElements::~BuildElements(){}

/**
 * Построение элементов схемы
 * @brief BuildElements::greateElements
 */
void BuildElements::greateElements()
{
    QMap <int, QMap<int, UloData>>::const_iterator it = ulodata->begin();
    int idx = 0;

     while(it != ulodata->end()){/**Проход по внешней коллeкции*/
          /**Один элемент внешней коллекции = QMap<int, UloData>*/
          QList<int> keys = it->keys();/** Получим ключи для QMap<int, UloData> */
          QList<UloData> vls = it->values();/**UloData*/
          int typeLogo = 0;
          int typeElm = getTypeElement(vls, &typeLogo);/**Получим тип элемента*/
          listElem << buildElement(typeElm, typeLogo);/**Создание и загрузка элемента**/
          QString nameEl;
          if(typeElm == TMR)
             nameEl = vls.at(0).getOperandCommand();/**Название для таймера*/
          else
             nameEl = vls.at(0).getNumCommandHex();/**Остальные элементы*/
          if(typeElm == UNI && vls.at(0).getOperandCommand().isEmpty()){
             nameEl = getNameUniEl(vls);/**Получим имя универсального элемента*/
          }
          listElem.at(idx)->setNameEl(nameEl);/**Зададим имя элемента*/
          int numTerm = 1;/**Начальный номер терминала в элементе*/
          int copyId=0;
          /**Создадим терминалы элемента из данных таблицы*/
          for(int id : keys){
              UloData dat = it->value(id);
              if(typeElm == UNI_TG){
                 createTerminal(typeLogo, &dat, numTerm, idx);/**Создадим терминал для элемента Tq/Tr/Ts */
              }else if(typeElm == UNI || typeElm == UNI_2_3 || typeElm == UNI_F9  || typeElm == UNI_99 || typeElm == UNI_41){// Проверить !!!
                  /**Для данного элемента дополнительно создаются два параметра*/
                  /**тип логики и количество входов на этот тип элемента*/
                  if(dat.getCodOper().toInt()!=UNI)/**UNI элемент*/
                     createTerminal(&dat, numTerm, idx);
              }else{
                 createTerminal(&dat, numTerm, idx);/** Создадим терминал для элемента AND/OR */
              }
              copyId = id;
              numTerm++;
          }
          /** Триггер без ВХОДОВ? достроим!!! */
          if(listElem.at(idx)->getTypeEl() == UNI_TG && listElem.at(idx)->getCntTermWest()==0){
             UloData dat = it->value(copyId);
             createMissingTermInput(typeLogo, &dat, 1, idx);
             createMissingTermInput(typeLogo, &dat, 2, idx);
          }else if(listElem.at(idx)->getTypeEl() == UNI_TG && listElem.at(idx)->getCntTermWest()==1){
              UloData dat = it->value(copyId);
              if(getTermINpTypeTg(idx) == IN_S)
                 createMissingTermInput(typeLogo, &dat, 2, idx);
              else if(getTermINpTypeTg(idx) == IN_R)
                 createMissingTermInput(typeLogo, &dat, 1, idx);
          }
          /**Проверка универсального элемента на соответствие количества входов*/
          if(listElem.at(idx)->getTypeEl() == UNI || listElem.at(idx)->getTypeEl() == UNI_2_3
                  || listElem.at(idx)->getTypeEl() == UNI_F9 || listElem.at(idx)->getTypeEl() == UNI_99
                  || listElem.at(idx)->getTypeEl() == UNI_41){
             int maxNumCell = getMaxLy(listElem.at(idx));/**Получим кол. входов ун. элемента, которое должно быть*/
             /** Построим заданное количество элементов по maxNumCell */
             while(maxNumCell > 0){
                 if(!scanInputUniEl(listElem.at(idx), maxNumCell)){
                     UloData dat = it->value(copyId);
                     createTermInputUni(typeLogo, &dat, maxNumCell, idx);
                 }
                 maxNumCell--;
             }
             /**Установим типы (AND, OR) первичных элементов*/
             setTypePrimElUni(listElem.at(idx));
          }

          /** Элемент без OUT? Проверим!!! */
          if(listElem.at(idx)->getCntTermEast()==0){
             UloData dat = it->value(copyId);
             createMissingTerminal(&dat, numTerm, idx, O_UT_STR);
          }
          /** Элемент без INPUT? Проверим!!! */
          if(isNotInputTerm(vls) && listElem.at(idx)->getTypeEl() != UNI && listElem.at(idx)->getTypeEl() != UNI_2_3
                   && listElem.at(idx)->getTypeEl() != UNI_F9 && listElem.at(idx)->getTypeEl() != UNI_99
                   && listElem.at(idx)->getTypeEl() != UNI_41){
             UloData dat = it->value(copyId);
             createMissingTerminal(&dat, numTerm, idx, INP_STR);
          }
          it++;idx++;
    }
    paintElements();
}

/**
 * Получим мах номер лог. ячейки
 * @brief BuildElements::getMaxLy
 * @param el
 * @return
 */
 int BuildElements::getMaxLy(CustomElement *el)
{
    int retVal = 0;

    for(Terminal* t : el->getListTerminals()){
        retVal = t->uloDat.getLogCellCommand().toInt() > retVal ? t->uloDat.getLogCellCommand().toInt() : retVal;
    }

    return retVal;
}
 /**
 * Просмотрим первичные группы элем. универсального элемента на содержание К.О. INPUT
 * @brief BuildElements::scanInputUniEl
 * @param el
 * @param numCell
 * @return
 */
bool BuildElements::scanInputUniEl(CustomElement *el, int numCell)
{
    bool retVal = false;

    for(Terminal* t : el->getListTerminals()){
        if(t->uloDat.getLogCellCommand().toInt() == numCell && t->uloDat.getCodOper().toInt() == INP){
            retVal = true;
            break;
        }
    }

    return retVal;
}

/**
 * Создать недостающий вход унив. элемента
 * @brief BuildElements::createTermInputUni
 * @param logo
 * @param dat
 * @param num
 * @param idx
 */
void BuildElements::createTermInputUni(int logo, UloData *dat, int num, int idx)
{
    dat->setCodOper(INP_STR);
    dat->setOperandCommand("");
    dat->setOperCommand("INPUT");
    QString s;
    s = QString("%1").arg(num);
    dat->setLogCellCommand(s);
    listElem.at(idx)->listTerminals << (new Terminal(logo, dat, num, listElem.at(idx)));
    int from = listElem.at(idx)->listTerminals.size()-1;
    listElem.at(idx)->listTerminals.at(from)->setNumInputUni(num);
    int to = -1;

    for(Terminal* t : listElem.at(idx)->listTerminals){
        to++;
        if(t->uloDat.getLogCellCommand().toInt() == num && t->uloDat.getCodOper().toInt() != INP)
           break;
    }
    listElem.at(idx)->listTerminals.move(from, to);//Передвинем INPUT на первую позицию первичного элемента//

    int komultWest=0,komultEast=0, number=0;
    for(Terminal* t : listElem.at(idx)->listTerminals){//Пересчитаем number,koMult,numInputUni//
        if(t->getOri()==Ulo::West){//Если входной терминал//
           t->setKoMult(++komultWest);
           t->setNumber(++number);
        }else if(t->getOri()==Ulo::East){//Если Выходной терминал//
                 t->setKoMult(++komultEast);
                 t->setNumber(++number);
        }
    }
}

/**
 * Установим типы (AND, OR) первичных элементов
 * @brief BuildElements::setTypePrimElUni
 * @param el
 */
void BuildElements::setTypePrimElUni(CustomElement *el)
{
    int ly=0;//Номер Л.Я.//
    int ko=0;//Код операции//

    for(int size=el->listTerminals.size()-1; size >=0; size-- ){
        Terminal* t=el->listTerminals.at(size);
        if(ly == t->uloDat.getLogCellCommand().toInt()){
           if(ly==0) continue;//Пропустим OUTы и *//
        }else{
            ly = t->uloDat.getLogCellCommand().toInt();//Смена Л.Я.//
            int sw = t->uloDat.getCodOper().toInt();
            switch(sw){
            case OR:
            case OR_N:
                ko=OR;
                break;
            case AND:
            case AND_N:
                ko=AND;
                break;
            }
        }
        t->setTypePrimElUni(ko);
    }
}

/**
 * Отрисовка элементов схемы
 * @brief BuildElements::paintElements
 */
void BuildElements::paintElements()
{
    calcSizeElements();/** Определение размеров элементов */
    calcPosElements(); /** Определение позиций элементов */
    for(CustomElement *el : listElem){
        el->buildElement(dGraph);
        QPoint p(el->getLocalPos().x(), el->getLocalPos().y());
        QSize s(el->getSizeElem().width(), el->getSizeElem().height());
        dGraph.setWalkability(p, s);/** Построение непроходимых вершин */
        //scene->addItem(el);
    }

    dGraph.setWalkabilityArray(points);/** Установим непроходимые вершины */
    createConductors();/** Создадим соединения между элементами */
    typeLineCorrection1(26);/** Тип коррекции линии №1 */
    typeLineCorrection2();
    terminalCorrections();/** Коррекция терминалов элементов */
    terminalCorrectionFreeOut();//??Доделать анализ пересечений точки терминала линией??
    drawElements();/** Отрисовка элементов */
    drawConductors();/** Отрисовка соединителей */
    testDrawPoints();/** Отрисовка непроходимых вершин. Для отладки */

}

/**
 * Тип коррекции линии №1
 * @brief BuildElements::typeLineCorrection1
 * @param cntPoint
 */
void BuildElements::typeLineCorrection1(int cntPoint)
{
    int dltX = cntPoint*SchemeAlgo::xGrid;

    for(Conductor *co:listCo){
        CustomElement* el1 = co->getElem1();
        Terminal* t1 = co->getTerminal1();
        Terminal* t2 = co->getTerminal2();

        if((t2->getP1().x() - t1->getP2().x()) > dltX && t2->getP2().y() < t1->getP1().y()){
            co->deleteSegments();//Удалим сегмент соединения//
            //Поставим точки непроходимости вершин//
            co->getDgr().setTempWalkX(t1->getP2().x(), t1->getP2().x()+SchemeAlgo::xGrid,
                                      t1->getP2().y()+SchemeAlgo::xGrid, 1);
            co->getDgr().setTempWalkY(el1->getLocalPos().y()-SchemeAlgo::xGrid,
                                      t1->getP2().y(), t1->getP2().x()+SchemeAlgo::xGrid, 1);
            co->recreationSegments();//Построим заново сегмент соединения//
            //Уберем точки непроходимости вершин//
            co->getDgr().setTempWalkX(t1->getP2().x(), t1->getP2().x()+SchemeAlgo::xGrid,
                                      t1->getP2().y()+SchemeAlgo::xGrid, 0);
            co->getDgr().setTempWalkY(el1->getLocalPos().y()-SchemeAlgo::xGrid,
                                      t1->getP2().y(), t1->getP2().x()+SchemeAlgo::xGrid, 0);
        }
    }

}

/**
 * Cоединение верхнего терминала пересекаются соединением нижнего
 * @brief BuildElements::typeLineCorrection2
 */
void BuildElements::typeLineCorrection2()
{
    for(CustomElement *el:listElem){
        if(el->getCntTermEast() > 1){//Выходы элементов//
           //Список соединителей связаных с выходами элементов//
           QList<Conductor*> list = selectBusyTerminals(el);
           swapAndMakeCo(list);//Проверка на пересечение и обработка пересечений терминалов//
        }
    }
}

/**
 * Отбор соединений связаных с выходами элемента
 * @brief BuildElements::selectBusyTerminals
 * @param el
 * @return
 */
QList<Conductor *> BuildElements::selectBusyTerminals(CustomElement *el)
{
    QList<Conductor*> list;

    for(Conductor*co:listCo){
        //Отбор соединителей из списка всех соединителей//
        if(co->getElem1()->getNameEl().indexOf(el->getNameEl())==0 && co->getTerminal1()->ori==Ulo::East){
           list.append(co);
        }
    }
    return list;
}

/**
 * Замена терминалов и построение их соединений
 * @brief BuildElements::swapAndMakeCo
 * @param list
 */
void BuildElements::swapAndMakeCo(QList<Conductor *> list)
{
      int i;
      for(i = list.size()-1; i>=0; i--){
          for(int j=i-1; j>=0; j--){
              QLineF line1 = ParsingCo::createLineVorH(list.at(i)->getSegments(), Ulo::East);
              QLineF line2 = ParsingCo::createLineVorH(list.at(j)->getSegments(), Ulo::East);
              int intersect = Geomet::segmentsIntersect(line1, line2);
              if(intersect == Geomet::INTERSECTION || intersect == Geomet::ON_SEGMENT_D1 ||
                           intersect == Geomet::ON_SEGMENT_D2 ||
                                     intersect == Geomet::ON_SEGMENT_D3 ||
                                               intersect == Geomet::ON_SEGMENT_D4){
                  Terminal* t1 = list.at(i)->getTerminal1();
                  Terminal* t2 = list.at(j)->getTerminal1();
                  CustomElement* el = list.at(i)->getElem1();

                  QPainter qp;
                  qp.begin(&el->drawing);
                  el->makeAnyElement(qp);
                  el->swapCoordTerm(t1, t2, qp);
                  qp.end();
                  Conductor *co_i = list.at(i);
                  Conductor *co_j = list.at(j);
                  //Удаление соединителей
                  co_i->deleteSegments();
                  co_j->deleteSegments();
                  //Построение новых соеденителей
                  co_j->recreationSegments();
                  co_j->getDgr().setTempWalkX(t1->getP2().x(), t1->getP2().x()+SchemeAlgo::xGrid,
                                            t1->getP2().y()+SchemeAlgo::xGrid, 1);
                  co_j->getDgr().setTempWalkY(el->getLocalPos().y()-SchemeAlgo::xGrid,
                                            t1->getP2().y(), t1->getP2().x()+SchemeAlgo::xGrid, 1);
                  co_i->recreationSegments();
                 /* co_j->getDgr().setTempWalkX(t1->getP2().x(), t1->getP2().x()+SchemeAlgo::xGrid,
                                            t1->getP2().y()+SchemeAlgo::xGrid, 0);
                  co_j->getDgr().setTempWalkY(el->getLocalPos().y()-SchemeAlgo::xGrid,
                                            t1->getP2().y(), t1->getP2().x()+SchemeAlgo::xGrid, 0);*/
              }
          }
      }
}

/**
 * Установка вершин непроходимости на концах неподключенных терминалов
 * во избежании их замыкания
 * @brief BuildElements::terminalCorrectionFreeOut
 */
void BuildElements::terminalCorrectionFreeOut()
{
    for(CustomElement *el : listElem){
        QList<Terminal*> tList = el->getListTerminals();
        for(Terminal* t : tList){
            if(t->conductors().size() == 0){
               QPointF pf = t->getDesiredPoint();
               if(pf.isNull())
                  continue;
               dGraph.setTempWalkX(pf.x(), pf.x(), pf.y(), 1);
               testIntersectionPointWithLine(pf);
            }
        }
    }
}

/**
 * Анализ пересечения токи терминала с линией кондуктора
 * @brief BuildElements::testIntersectionPointWithLine
 * @param pf
 */
void BuildElements::testIntersectionPointWithLine(QPointF pf)
{
    for(Conductor *co : listCo){
        for(ConductorSegment cs : co->getSegments()){
            if(pf == cs.firstPoint()){
               co->deleteSegments();
               co->recreationSegments();
            }
        }
    }

}

/**
 * Отрисовка непроходимых вершин. Для отладки
 * @brief BuildElements::testDrawPoints
 */
void BuildElements::testDrawPoints()
{
    for(QPoint p:points){
        PointItem *pi=new PointItem();
        pi->setPos(p);
        scene->addItem(pi);
    }
}

/**
 * Отрисовка элементов
 * @brief BuildElements::drawElements
 */
void BuildElements::drawElements()
{
    for(CustomElement *el : listElem){
        scene->addItem(el);
    }
}

/**
 * Отрисовка соединителей
 * @brief BuildElements::drawConductors
 */
void BuildElements::drawConductors()
{
    for(Conductor *co:listCo){
        scene->addItem(co);
    }
}

/**
 * Коррекция терминалов элементов
 * @brief BuildElements::terminalCorrections
 */
void BuildElements::terminalCorrections()
{
    ParsingCo* pc = new ParsingCo(listCo);
    pc->mainParsing();/** Изменение длинны терминалов в зависимости от кол. вх/вых */
    delete pc;
}

/**
 * Создание отсутствующих входов УНиверсального элемента
 * @brief BuildElements::createUniInp
 * @param cell
 * @return
 */
bool BuildElements::createUniInp(int cell)
{
    bool flagBrk=false, ret=false;

    for(int i=listElem.size()-1; i>=0; i--){
        CustomElement *ce = listElem.at(i);
        for(Terminal *t:ce->listTerminals){//Проход по всем терминалам элементов//
            if(t->ori==Ulo::West && t->uloDat.getCodOper().toInt()==INP && !t->getName().isEmpty() && getNumCell(ce)==cell){
               ret=true;
               flagBrk=true;
               break;
            }
        }
        if(flagBrk)
           break;
    }

    return ret;
}


/**
 * Создание соединителей
 * @brief BuildElements::createConductors
 */
void BuildElements::createConductors()
{
    createConductorsAll();/** Соединитель nameOUT -> nameINPUT */
    createConductorsAndOr();/** Соединитель nameOUT -> NOnameINPUT */
    createConductorsAllWest();/** Соединитель nameINPUT -> nameINPUT */ //Замыкает таймер!

    createConductorsTg();
    createConductorsUni();

}


/**
 * Создание соединителей для И и ИЛИ элементов.
 * Соединитель nameOUT -> NOnameINPUT.
 * @brief BuildElements::createConductorsAndOr
 */
void BuildElements::createConductorsAndOr()
{
    int idx=0;
    Terminal *t1, *t2;
    t1=t2=nullptr;

    for(CustomElement *el : listElem){
        if((listElem.size()-1) == idx){break;}
        CustomElement *elNext = listElem.at(idx+1);
        if(!isElTermInp(elNext) && elNext->getTypeEl() != UNI_TG && elNext->getTypeEl() != UNI && elNext->getTypeEl() != UNI_2_3
                 && elNext->getTypeEl() != UNI_F9  && elNext->getTypeEl() != UNI_99  && elNext->getTypeEl() != UNI_41){
            t1 = getP1Terminal(el->listTerminals, el);//Выход текущего эл//
            t2 = getP2Terminal(elNext->listTerminals);//Вход следующего эл//
            listCo << (new Conductor(t1, t2, el, elNext, dGraph));
        }
        idx++;
    }
}

/**
 * Создание соединений с соседними элементами.
 * Соединитель nameOUT -> nameINPUT.
 * @brief BuildElements::createConductorsAll
 */
void BuildElements::createConductorsAll()
{
    CustomElement *currEl = nullptr;
    CustomElement *nextEl = nullptr;

    for(int i=0; i < listElem.size(); i++){
        currEl=listElem.at(i);
        for(int j=i; j < listElem.size(); j++){
            nextEl=listElem.at(j);
            if(isTimersEl(currEl, nextEl)) { continue; }
            getP1P2Terminal(currEl, nextEl);
        }
    }
}


/**
 * Создание соединений с соседними элементами со сторонами WEST
 * @brief BuildElements::createConductorsAll
 */
void BuildElements::createConductorsAllWest()
{
    CustomElement *currEl = nullptr;
    CustomElement *nextEl = nullptr;

    for(int i=0; i < listElem.size(); i++){
        currEl=listElem.at(i);
        for(int j=i+1; j < listElem.size(); j++){
            nextEl=listElem.at(j);
            if(isTimersEl(currEl, nextEl)){ continue; } /** Обход элемента таймер */
            getP1P2TerminalWest(currEl, nextEl);
        }
    }
}


/**
 * Получим терминалы для построения соединений по одноименным адресам
 *  INPUT -> INPUT
 * @brief BuildElements::getP1P2TerminalWest
 * @param currEl
 * @param nextEl
 */
void BuildElements::getP1P2TerminalWest(CustomElement *currEl, CustomElement *nextEl)
{
    const QList<Terminal *> curr = currEl->listTerminals;
    const QList<Terminal *> next = nextEl->listTerminals;
    Terminal *t1 = nullptr;
    Terminal *t2 = nullptr;

    /** Сравниваем каждый из терминалов текущего элемента(curr) с каждым терминалов следующих элементов(next) */
    for(int i=0; i<curr.size(); i++){
        t1=curr.at(i);
        if(t1->ori==Ulo::West && currEl->getTypeEl() != TMR){/** Ulo::West Сторона INPUT. Ignored timer */
           for(int j=0; j<next.size(); j++){
               t2=next.at(j);
               if(t2->ori==Ulo::West && t1->getName().indexOf(t2->getName())==0 && !(t1->getName().isEmpty() || t2->getName().isEmpty())){
                  if(isShortCircTimer(t1)){
                     listCo << (new Conductor(t1, t2, currEl, nextEl ,dGraph));
                  }
               }
           }
        }
    }
}


/**
 * Один и тот же элемент таймер
 * @brief BuildElements::isTimersEl
 * @param el
 * @param elNext
 * @return
 */
bool BuildElements::isTimersEl(CustomElement *el, CustomElement *elNext)
{
    return (el->getNameEl().indexOf(elNext->getNameEl())==0 && el->getTypeEl()==TMR);
}


/**
 * Поиск терминала по имени
 * @brief BuildElements::serchByNameTerm
 * @param el
 * @param name
 * @param ori
 * @return
 */
Terminal *BuildElements::serchByNameTerm(CustomElement *el, QString name, Ulo::Orientation ori) const
{
    Terminal *tmp = nullptr;

    for(Terminal *t : el->listTerminals){
        if(t->ori == ori && t->getName().indexOf(name) == 0){
           tmp = t;
           break;
        }
    }

    return tmp;
}

/**
 * Создание соединителя для триггера
 * @brief BuildElements::createConductorsTg
 */
void BuildElements::createConductorsTg()
{
    int cell;
    Terminal *t1=nullptr, *t2=nullptr;
    CustomElement *elPrev;// = nullptr;

     for(CustomElement *el:listElem){
         if(el->getTypeEl() != UNI_TG)
            continue;

         cell = getNumCellTg(el,LY_1);
         if(cell != 0){
            t1 = getElTermInpTg(el, cell);
            t2 = getElTermForTg(el, cell, elPrev);
            if(t1 && t2){
               listCo << (new Conductor(t2, t1, elPrev, el, dGraph));
            }
         }

         cell = getNumCellTg(el, LY_2);
         if(cell != 0){
            t1 = getElTermInpTg(el, cell);
            t2 = getElTermForTg(el, cell, elPrev);
            if(t1 && t2){
               listCo << (new Conductor(t2, t1, elPrev, el, dGraph));
            }
         }
     }
}

/**
 * Создание соединения для универсального элемента
 * @brief BuildElements::createConductorsUni
 */
void BuildElements::createConductorsUni()
{
    int cell;
    Terminal *t1=nullptr, *t2=nullptr;
    CustomElement *elPrev;// = nullptr;

     for(CustomElement *el:listElem){
         if(el->getTypeEl() != UNI && el->getTypeEl() != UNI_2_3 && el->getTypeEl() != UNI_F9
                                     && el->getTypeEl() != UNI_99 && el->getTypeEl() != UNI_41)
            continue;

         cell = getNumCellTg(el, LY_1);
         if(cell != 0){
            t1 = getElTermInpTg(el, cell);
            t2 = getElTermForTg(el, cell, elPrev);
            if(t1 && t2){
               listCo << (new Conductor(t2, t1, elPrev, el, dGraph));
            }
         }

         cell = getNumCellTg(el, LY_2);
         if(cell != 0){
            t1 = getElTermInpTg(el, cell);
            t2 = getElTermForTg(el, cell, elPrev);
            if(t1 && t2){
               listCo << (new Conductor(t2, t1, elPrev, el, dGraph));
            }
         }

         cell = getNumCellTg(el, LY_3);
         if(cell != 0){
            t1 = getElTermInpTg(el, cell);
            t2 = getElTermForTg(el, cell, elPrev);
            if(t1 && t2){
               listCo << (new Conductor(t2, t1, elPrev, el, dGraph));
            }
         }
     }
}

/**
 * Получим элементы на входе триггера входящих в триггер
 * @brief BuildElements::getElTermForTg
 * @param el
 * @param cell
 * @param elprev
 * @return
 */
Terminal *BuildElements::getElTermForTg(CustomElement *el, int cell, CustomElement *elprev)
{
    if(listElem.size() < 2)
       return nullptr;
    int idx = listElem.indexOf(el)-1;
    CustomElement *ce;

    for(int i=idx; i >=0; i--){
        ce = listElem.at(i);
        if(ce->getTypeEl() != UNI_TG){
            if(cell == getNumCell(ce)){
               for(Terminal *t:ce->listTerminals){
                   if(t->ori==Ulo::East && t->getName().isEmpty()){
                      elprev = ce;
                      return t;
                   }else if(t->ori==Ulo::East && t->uloDat.getCodOper().toInt()==O_UT && !t->getName().isEmpty()){
                       elprev = ce;
                       return t;
                    }
               }
            }
        }
    }
    return nullptr;
}


/**
 * Есть ли у элемента выход OUT?
 * @brief BuildElements::isElTermOut
 * @param el
 * @return
 */
bool BuildElements::isElTermOut(CustomElement *el)
{
    bool ret=false;
    QList<Terminal*> term = el->listTerminals;

    for(Terminal *t:term){
        if(t->ori==Ulo::East && t->uloDat.getCodOper().toInt()==O_UT && t->uloDat.getLogCellCommand().toInt()>1){
                   ret = false;
                   break;
        }else if(t->ori==Ulo::East && t->uloDat.getCodOper().toInt()==O_UT && !t->getName().isEmpty()){
            ret = true;
            break;
         }
    }

    return ret;
}

/**
 * Есть ли у следующего элемента вход INPUT?
 * @brief BuildElements::isElTermInp
 * @param el
 * @return
 */
bool BuildElements::isElTermInp(const CustomElement *el)
{
    bool ret=false;

    for(Terminal *t:el->listTerminals){
        if(t->ori==Ulo::West && t->uloDat.getCodOper().toInt()==INP && !t->getName().isEmpty()){
           ret = true;
           break;
        }
    }

    return ret;
}


/**
 * Получим лoгическую ячейку элемента
 * @brief BuildElements::getNumCell
 * @param el
 * @return
 */
int BuildElements::getNumCell(CustomElement *el)
{
    return el->listTerminals.at(0)->uloDat.getLogCellCommand().toInt();
}

/**
 * Получим ячейку триггера без адреса
 * @brief BuildElements::getNumCellTg
 * @param el
 * @param cell
 * @return
 */
int BuildElements::getNumCellTg(CustomElement *el, int cell)
{
    int ret=0;

    for(Terminal *t:el->listTerminals){
        if(t->ori==Ulo::West && t->uloDat.getLogCellCommand().toInt()==cell && t->getName().isEmpty()){
           ret = cell;
        }
    }
    return ret;
}

/**
 * Получим вход триггера без адреса
 * @brief BuildElements::getElTermInpTg
 * @param el
 * @param cell
 * @return
 */
Terminal *BuildElements::getElTermInpTg(CustomElement *el, int cell) const
{
    Terminal *p=nullptr;

    for(Terminal *t : el->listTerminals){
        if(t->ori==Ulo::West && t->uloDat.getCodOper().toInt()==INP && t->getName().isEmpty()){
           if(t->uloDat.getLogCellCommand().toInt()==cell)
              p = t;
        }
    }
    return p;
}

/**
 * Получить одиночный терминал на выходе элемента
 * @brief BuildElements::getP1Terminal
 * @param term
 * @param el
 * @return
 */
Terminal *BuildElements::getP1Terminal(QList<Terminal *> term, CustomElement *el)
{
    Terminal *tmp = nullptr;

    for(Terminal* t : term){
        if(t->ori == Ulo::East && el->getCntTermEast() == 1){//Выход
           tmp=t;
           break;
        }
    }
    return tmp;
}

/**
 * Получить входной терминал без имени/адреса
 * @brief BuildElements::getP2Terminal
 * @param term
 * @return
 */
Terminal *BuildElements::getP2Terminal(QList<Terminal *> term)
{
    Terminal *tmp = nullptr;

    for(Terminal* t : term){
        if(t->ori == Ulo::West && t->getName().isEmpty()){//Вход
           tmp=t;
           break;
        }
    }
    return tmp;
}


/**
 * Получим терминалы для построения соединений по одноименным адресам
 *  OUT -> INPUT
 * @brief BuildElements::getP1P2Terminal
 * @param currEl
 * @param nextEl
 */
void BuildElements::getP1P2Terminal(CustomElement* currEl, CustomElement*  nextEl)
{
    const QList<Terminal *> curr = currEl->listTerminals;
    const QList<Terminal *> next = nextEl->listTerminals;
    Terminal *t1 = nullptr;
    Terminal *t2 = nullptr;

    for(int i=0; i<curr.size(); i++){
        t1=curr.at(i);
        if(t1->ori==Ulo::East){/** Ulo::East Сторона OUT */
           for(int j=0; j<next.size(); j++){
               t2=next.at(j);
               if(t2->ori==Ulo::West && /** Сторона INPUT */
                       t1->getName().indexOf(t2->getName())==0 &&
                                       !(t1->getName().isEmpty() || t2->getName().isEmpty())){
                  if(isShortCircTimer(t1)){
                     listCo << (new Conductor(t1, t2, currEl, nextEl ,dGraph));
                  }
               }
           }
        }
    }
}


/**
 * Предотвращение замыкания таймера
 * @brief BuildElements::isShortCircTimer
 * @param term
 * @return
 */
bool BuildElements::isShortCircTimer(Terminal *term)
{
    bool ret = true;
    QString nameTerm = term->getName();

    if(nameTerm.left(1)!="T" || term->uloDat.getOperCommand()=="TIMER")
       return ret;

    for(Conductor *co:listCo){
        if(nameTerm == co->terminal1->getName()){
           ret = false;
        }
    }

    return ret;
}


/**
 * Создание соединителей *******************************************************************
 *
 */

/**
 * @brief BuildElements::getNameUniEl
 * @param v
 * @return
 */
QString BuildElements::getNameUniEl(QList<UloData> v)
{
    QString name;

    for(UloData d:v){
        if(d.getCodOper().toInt()==4){
            name = d.getNumCommandHex();
            break;
        }
    }
    return name;
}

/**
 * @brief BuildElements::createTerminal
 * @param dat
 * @param num
 * @param idx
 */
void BuildElements::createTerminal(UloData *dat, int num, int idx)
{
    listElem.at(idx)->listTerminals << (new Terminal(dat, num, listElem.at(idx)));
}

/**
 * @brief BuildElements::createTerminal
 * @param logo
 * @param dat
 * @param num
 * @param idx
 */
void BuildElements::createTerminal(int logo, UloData *dat, int num, int idx)
{
    if(dat->getLogCellCommand().toInt() == 3 && dat->getCodOper().toInt() == 6){//3 вход не отображаем//
       return;
    }else if(dat->getCodOper().toInt() == 4){//* не отображаем//
       return;
    }else{
       listElem.at(idx)->listTerminals << (new Terminal(logo, dat, num, listElem.at(idx)));
    }
}

/**
 * @brief BuildElements::createMissingTerminal
 * @param dat
 * @param num
 * @param idx
 * @param codeOper
 */
void BuildElements::createMissingTerminal(UloData *dat, int num, int idx, QString codeOper)
{

    dat->setCodOper(codeOper);//Обманем конструктор//
    dat->setOperandCommand("");
    listElem.at(idx)->listTerminals << (new Terminal(dat, num, listElem.at(idx)));
}

/**
 * Проверим есть ли INPUT вход у элемента AND/OR
 * @brief BuildElements::isNotInputTerm
 * @param va
 * @return
 */
bool BuildElements::isNotInputTerm(QList<UloData> &va)
{
    bool ret = true;

    for(UloData d:va){
        if(d.getCodOper().toInt()==INP){
           ret = false;
           break;
        }
    }

    return ret;
}

/**
 * @brief BuildElements::createMissingTermInput
 * @param logo
 * @param dat
 * @param num
 * @param idx
 */
void BuildElements::createMissingTermInput(int logo, UloData *dat, int num, int idx)
{
    dat->setCodOper(INP_STR);
    dat->setOperandCommand("");
    dat->setOperCommand("INPUT");
    QString s;
    s = QString("%1").arg(num);
    dat->setLogCellCommand(s);
    listElem.at(idx)->listTerminals << (new Terminal(logo, dat, num, listElem.at(idx)));
}

/**
 * @brief BuildElements::getTermINpTypeTg
 * @param idx
 * @return
 */
int BuildElements::getTermINpTypeTg(int idx){

    int retVal = 0;

    for(Terminal *t : listElem.at(idx)->listTerminals){
        if(t->ori == Ulo::Orientation::West){
           retVal = t->getInpTg();
        }
    }

    return retVal;
}

/**
 * Получим тип элемента
 * @brief BuildElements::getTypeElement
 * @param va
 * @param typeTg
 * @return
 */
int BuildElements::getTypeElement(QList<UloData> &va, int *typeTg)
{
   int retType = -1;
   bool ok;

   for(UloData d:va){
       switch(d.getCodOper().toInt()){
       case 2:
       case 3:
           retType = AND;
           break;
       case 0:
       case 1:
           retType = OR;
           break;
       case 4:
           if(d.getOperandCommand().toInt(&ok, 16)==0xB2){
              *typeTg = 0xB2;
              retType = UNI_TG;
           }else if(d.getOperandCommand().toInt(&ok, 16)==0xBA){
              *typeTg = 0xBA;
              retType = UNI_TG;
           }else if(d.getOperandCommand().toInt(&ok, 16)==0x32){
              *typeTg = 0x32;
              retType = UNI_TG;
           }else if(d.getOperandCommand().toInt(&ok, 16)==0xFE){
              *typeTg = 0xFE;
              retType = UNI;
           }else if(d.getOperandCommand().toInt(&ok, 16)==0xEE){
              *typeTg = 0xEE;
              retType = UNI;
           }else if(d.getOperandCommand().toInt(&ok, 16)==0x80){
              *typeTg = 0x80;
               retType = UNI;
           }else if(d.getOperandCommand().toInt(&ok, 16)==0x88){
              *typeTg = 0x88;
              retType = UNI;
           }else if(d.getOperandCommand().toInt(&ok, 16)==0xE8){
               *typeTg = 0xE8;
               retType = UNI_2_3;
            }else if(d.getOperandCommand().toInt(&ok, 16)==0xF9){
               *typeTg = 0xF9;
               retType = UNI_F9;
            }else if(d.getOperandCommand().toInt(&ok, 16)==0xF6){
               *typeTg = 0xF6;
               retType = UNI_F9;
            }else if(d.getOperandCommand().toInt(&ok, 16)==0x99){
               *typeTg = 0x99;
               retType = UNI_99;
            }else if(d.getOperandCommand().toInt(&ok, 16)==0x66){
               *typeTg = 0x66;
               retType = UNI_99;
            }else if(d.getOperandCommand().toInt(&ok, 16)==0x41){
               *typeTg = 0x41;
               retType = UNI_41;
            }
           else{
              retType = 0;
           }
           break;
       case 6:
              if(d.getOperandCommand().toInt(&ok, 16)>=0x200
                      && d.getOperandCommand().toInt(&ok, 16) <= 0x2FF
                      && d.getOperCommand().indexOf("TIMER")==0)
                  retType = TMR;
              else if((d.getOperandCommand().toInt(&ok, 16)>=0x400 && d.getOperandCommand().toInt(&ok, 16) <= 0x41F) ||
                      (d.getOperandCommand().toInt(&ok, 16)>=0x500 && d.getOperandCommand().toInt(&ok, 16) <= 0x51F) ||
                      (d.getOperandCommand().toInt(&ok, 16)>=0x520 && d.getOperandCommand().toInt(&ok, 16) <= 0x53F))
                  retType = BVR;
           break;
       }
   }

   return retType;
}

/**
 * Фабрика элементов
 * @brief BuildElements::buildElement
 * @param type
 * @param typeTg
 * @return
 */
CustomElement *BuildElements::buildElement(int type, int typeTg)
{
    switch(type){
    case AND:
        return (new AndElement(paper));

    case OR:
        return (new OrElement(paper));

    case UNI_TG:
        return (new TgrElement(paper, typeTg));

    case UNI:
        return (new UniElement(paper, typeTg));

    case UNI_2_3:
        return (new UniElement_LF_E8(paper, typeTg));

    case UNI_F9:
        return (new UniElement_F9(paper, typeTg));

    case UNI_99:
        return (new UniElement_99(paper, typeTg));

    case UNI_41:
        return (new UniElement_41(paper, typeTg));

    case TMR:
        return (new TmrElement(paper));

    case BVR:
        return (new BvrElement(paper));

    }

    return (new OrElement(paper));
}

/**
 * @brief BuildElements::calcSizeElements
 */
void BuildElements::calcSizeElements()
{
    for(CustomElement *el:listElem){
        if(el->getTypeEl() == UNI_TG){
           el->setSizeElem(getSizeTg(el));
        }else{
           el->setSizeElem(getSizeElem(el));
        }
    }
}


/**
 * @brief BuildElements::getSizeElem
 * @param elEl
 * @return
 */
QSizeF BuildElements::getSizeElem(CustomElement *elEl)
{
    qreal num = 0;

    if(elEl->getCntTermWest() > elEl->getCntTermEast())
       num =  elEl->getCntTermWest();
    else
       num =  elEl->getCntTermEast();

    qreal h = num * Terminal::deltaY;
    h+=Terminal::deltaY;
    elEl->getRefElem().setHeight(h);//Высота элемента//

    return elEl->getSizeElem();
}

/**
 * @brief BuildElements::getSizeTg
 * @param elTg
 * @return
 */
QSizeF BuildElements::getSizeTg(CustomElement *elTg)
{
    qreal num =  elTg->getCntTermEast();

    num = num < 8 ? 7 : num;
    qreal h = num * Terminal::deltaY;
    h+=Terminal::deltaY;
    elTg->getRefElem().setHeight(h);//Высота элемента//
    elTg->getRefElem().setWidth(h*0.5);//Ширина элемента триггер//

    return elTg->getSizeElem();
}

/**
 * Установка начальных позиций (Х,У) элементов
 * для их отрисовки на схеме.
 * @brief BuildElements::calcPosElements
 */
void BuildElements::calcPosElements()
{
    QPointF p;

    if(listElem.size()==0){
       return;
    }else if(listElem.size()==1){ /** Один элемент на схеме */
       startPoint.setX(112.0);
       listElem.at(0)->setLocalPos(startPoint);
    }else if(listElem.size()>1 && isOutGreatOne(listElem)){/** Построение цепочки элементов по каждому OUT */

         for(int iList = 0; iList < listElem.size(); iList++){ /** Больше одного элемента на схеме */
             p = setPointElemOutGreatOne(listElem.at(iList)); /** Установка начальных точек X,Y элементов для отрисовки их на схеме */

             p.setX(correctXY(p.x(),SchemeAlgo::xGrid)); /** Дополнительная коррекция точки Х */
             p.setY(correctXY(p.y(),SchemeAlgo::yGrid)); /** Дополнительная коррекция точки Y */
             listElem.at(iList)->setLocalPos(p);/** Позиция размещения элемента на схеме */
         }
    }else{
          for(CustomElement *el:listElem){ /** Больше одного элемента на схеме */
              p = setPointElem(el); /** Установка начальных точек X,Y элементов для отрисовки их на схеме */
              p.setX(correctXY(p.x(),SchemeAlgo::xGrid));
              p.setY(correctXY(p.y(),SchemeAlgo::yGrid));
              el->setLocalPos(p);/** Позиция размещения элемента на схеме */
          }
    }

}

QPointF BuildElements::setPointElemOutGreatOne(CustomElement *el){
    QPointF p;

    if(el && el->getCntTermEast() > 1){
        //1-найдем элемент с выходами > 1
        //2-делаем цикл по выходам OUT
        //3-в каждой итерации ищем цепочку элементов
        //4-вставляем в карту
        //5-установим координаты начальной точки p(x,y) элемента
    }else if(el){
        p = setPointElem(el);
    }

    return p;
}

bool BuildElements::isOutGreatOne(QList<CustomElement *> listElem){

    bool retVal = false;
    for(CustomElement *ce : listElem){
        if(retVal = ce->getCntTermEast() > 1 ? true : false)
           break;
    }
    return retVal;
}



/**
 * @brief BuildElements::setPointElem
 * @param el
 * @return
 */
QPointF BuildElements::setPointElem(CustomElement *el)
{
    QPointF p;
    p=setLayout1(el);
    return p;
}


/**
 * Установка начальных точек X,Y элементов для отрисовки их на схеме
 * @brief BuildElements::setLayout1
 * @param el
 * @return
 */
QPointF BuildElements::setLayout1(CustomElement *el)
{
    QPointF retVal;
    QSizeF locSize = el->getSizeElem();
    qreal dX = 0.0;
    //retVal = tmpPoint;

    /** Отслеживание макс высоты элемента */
    if(tmpDeltaY < locSize.height()){
       tmpDeltaY = locSize.height();
    }

    isCoordMove(locSize.width());

    if(el->getTypeEl() == UNI_TG && getNumCellTg(el, 2) == IN_R){
        el->setLocalPos(tmpPoint);/** Установим позицию триггера(х,у) */
        if(!isCorrectEl(el)){
           /** При переносе (х,у)=(105,220) */
           int cntEl=0;/** All Elem */
           int cntRev=0;/** INP S */
           int cnt3=0;/** INP R */
           dX = getCoordYForElemTg(el, &cnt3);
           if(el->getTypeEl() == UNI_TG && getNumCellTg(el, 1) == IN_S){/** Изменим координату Х элемента */
              getCoordXForElemTg(el, dX, &cntEl, &cntRev);
              /** Коррекция элементов после сдвига по первой ячейке */
              cnt3 = cnt3 > cntRev ? cntRev : cnt3;
              correctElements(el, cntEl, cnt3);
           }
           tmpPoint.setX(el->getLocalPos().x());
        }
    }

    tmpPoint.setX(correctXY(tmpPoint.x(), SchemeAlgo::xGrid));
    tmpPoint.setY(correctXY(tmpPoint.y(), SchemeAlgo::yGrid));

    retVal = tmpPoint;

    return retVal;
}



/**
 * @brief BuildElements::isCoordMove
 * @param width
 * @return
 */
bool BuildElements::isCoordMove(qreal width)
{
    bool ret=false;

    if(tmpPoint.x()+width+Terminal::termLen*2+indentEdge*2 < paper.width()){
       tmpPoint.setX(tmpPoint.x()+Terminal::termLen*2+indentEdge*2);/** По умолчанию последовательное размещение */
    }else{ /** Перенос на одну позицию элементов по У вниз */
        startPoint.setX(112.0);
        tmpPoint.setX(startPoint.x());
        tmpPoint.setY(tmpPoint.y() + tmpDeltaY + indentEdge/* - 4.0*/);
        tmpPoint.setX(correctXY(tmpPoint.x(), SchemeAlgo::xGrid));
        tmpPoint.setY(correctXY(tmpPoint.y(), SchemeAlgo::yGrid));
        tmpDeltaY = 0;
        ret=true;
    }
    return ret;
}

/**
 * @brief BuildElements::isNotTransferEl
 * @param el
 * @return
 */
bool BuildElements::isNotTransferEl(CustomElement *el)
{
    return (tmpPoint.x()+el->getSizeElem().width()+Terminal::termLen*2+indentEdge*2 < paper.width());
}

/**
 * @brief BuildElements::isTransferEl
 * @param el
 * @param cell
 * @return
 */
bool BuildElements::isTransferEl(CustomElement *el, int cell)
{
    bool ret = false;
    int idx = listElem.indexOf(el)-1;
    QPointF pTg = el->getLocalPos();/** Позиция триггера */

    for(int i=idx; i>=0; i--){
        CustomElement *ce = listElem.at(i);
        if(isElCell(ce, cell) && ce->getLocalPos().y() < pTg.y()){/** Перенос элемента? */
           ret = true;
           break;
        }
        if(isElCell(ce, cell) && ce->isInputElem()){/** Последний элемент в цепочке? */
           break;
        }
    }

    return ret;
}


/**
 * Если какой нибудь из элементов триггера перенесен?
 * @brief BuildElements::isCorrectEl
 * @param el
 * @return
 */
bool BuildElements::isCorrectEl(CustomElement *el)
{
    bool flagTransfer = false;

    if(getNumCellTg(el, 2) == IN_R){
       if(isTransferEl(el, IN_R))
          flagTransfer = true;
    }

    if(!flagTransfer && getNumCellTg(el, 1) == IN_S){
       if(isTransferEl(el, IN_S))
          flagTransfer = true;
    }

    return flagTransfer;
}

/**
 * @brief BuildElements::isElCell
 * @param el
 * @param cellNum
 * @return
 */
bool BuildElements::isElCell(CustomElement *el, int cellNum)
{
    int ret=0;

    for(Terminal *t:el->listTerminals){
        if(t->ori==Ulo::West && t->uloDat.getLogCellCommand().toInt()==cellNum){
           ret = cellNum;
           break;
        }
    }
    return ret;
}


/**
 * @brief BuildElements::correctElementsAfter
 * @param el
 */
void BuildElements::correctElementsAfter(CustomElement *el)
{
    CustomElement *curr = nullptr;
    QPointF point;
    int idxTg = listElem.indexOf(el);
    int idxLast = getLastPlusX(el, &point);//Последнияя координата  -> +Х -> -X//
    tmpPoint = point;//Новая текущая координата//
    for(int i=idxLast; i < idxTg; i++){
        curr = listElem.at(i);
        if(isCoordMove(curr->getSizeElem().width())){
           int u=0;
           u++;
        }else{
            tmpPoint.setX(correctXY(tmpPoint.x(), SchemeAlgo::xGrid));
            tmpPoint.setY(correctXY(tmpPoint.y(), SchemeAlgo::yGrid));
            curr->setLocalPos(tmpPoint);
        }
    }
}


/**
 * @brief BuildElements::getLastPlusX
 * @param el
 * @param point
 * @return
 */
int BuildElements::getLastPlusX(CustomElement *el, QPointF *point)
{
    int i=0;
    int idx = listElem.indexOf(el)-1;

    for(i=idx; i>=0; i--){
        CustomElement *ce = listElem.at(i);
        if(ce->getLocalPos().x() > 0){
          (*point) = ce->getLocalPos();
           break;
        }
    }
    return (i+1);
}


/**
 * Получение обновленной точки(Х,У) предшествующего элемента триггера по входу R
 * @brief BuildElements::getCoordYForElemTg
 * @param el
 * @param cnt3
 * @return
 */
qreal  BuildElements::getCoordYForElemTg(CustomElement *el, int *cnt3)
{
    QPointF retPoint;
    qreal y = 0.0, retX = 0.0, maxY = 0.0;

    if(listElem.size() < 2)
       return -1;
    int idx = listElem.indexOf(el)-1;/** Индекс предшествующего элемента триггера */
    CustomElement *ce;

    /** Есть ли элементы связанные с входом триггера S? */
    if(getNumCellTg(el, 1) == IN_S){
       y = getHeightElemCellInpS(idx, IN_S);/** Сдвиг по У */
    }

    for(int i=idx; i >=0; i--){
        ce = listElem.at(i);

        if(getNumCell(ce) == IN_R){
           (*cnt3)++;
           if(y > 0){
              retPoint.setY(y+ce->getLocalPos().y()+20.0);
              if(retX == 0.0){/** Сохраним коорд. Х первого элемента ячейки 2 */
                 retX = ce->getLocalPos().x();/** для сдвига элементов ячейки 1 */
              }
              if(maxY < ce->getSizeElem().height()+ce->getLocalPos().y()){
                 maxY = ce->getSizeElem().height()+ce->getLocalPos().y();

              }
           }else{
              retPoint.setY(el->getSizeElem().height()/2 + ce->getLocalPos().y()+20.0);
           }

           retPoint.setX(ce->getLocalPos().x());
           retPoint.setX(correctXY(retPoint.x(), SchemeAlgo::xGrid));
           retPoint.setY(correctXY(retPoint.y(), SchemeAlgo::yGrid));
           ce->setLocalPos(retPoint);

           if(tmpDeltaY < (el->getSizeElem().height() - maxY)+el->getSizeElem().height()){
              tmpDeltaY = (el->getSizeElem().height() - maxY)+el->getSizeElem().height();
           }
           //if(tmpDeltaY < (maxY)/2)
           //   tmpDeltaY = (maxY)/2;

           if(ce->isInputElem()){/** Последний элемент в цепочке? */
              break;
           }
        }
    }
    return retX;
}

/**
 * Получение обновленной точки(Х,У) предшествующего элемента триггера по входу S
 * @brief BuildElements::getCoordXForElemTg
 * @param el
 * @param X
 * @param cnt1
 * @param cnt2
 */
void BuildElements::getCoordXForElemTg(CustomElement *el, qreal X, int *cnt1, int *cnt2)
{
    int idx = listElem.indexOf(el)-1;
    CustomElement *ce;
    qreal dtX = 0.0;

    for(int i=idx; i >=0; i--){
        (*cnt1)++;/** Счетчик всех элементов для коррекции */
        ce = listElem.at(i);
        if(getNumCell(ce) == IN_S){
            if(dtX == 0.0){
               dtX = X - ce->getLocalPos().x();/** Смещение по Х элементов */
            }
            QPoint p(ce->getLocalPos().x()+dtX, ce->getLocalPos().y());
            ce->setLocalPos(p);
            (*cnt2)++;/** Счетчик передвинутых элементов S входе */
            if(ce->isInputElem()){/** Последний элемент в цепочке? */
               break;
            }
        }
    }
}

/**
 * @brief BuildElements::correctElements
 * @param el
 * @param cnt1
 * @param cnt2
 */
void BuildElements::correctElements(CustomElement *el, int cnt1, int cnt2)
{
    int idx = listElem.indexOf(el);
    CustomElement *ce;
    cnt1++;
    qreal rev = cnt2 * ((el->getSizeElem().width()+Terminal::termLen*2+indentEdge*2)-60.0);

    for(int i=idx; i > (idx-cnt1); i--){
        ce = listElem.at(i);
        QPointF corr(ce->getLocalPos().x() - rev, ce->getLocalPos().y());
        corr.setX(correctXY(corr.x(), SchemeAlgo::xGrid));
        corr.setY(correctXY(corr.y(), SchemeAlgo::yGrid));
        ce->setLocalPos(corr);
    }
}

/**
 * @brief BuildElements::getHeightElemCellInpS
 * @param index
 * @param cell
 * @return
 */
qreal BuildElements::getHeightElemCellInpS(int index, int cell)
{
    qreal height=0;
    CustomElement *ce;

    for(int i=index; i >=0; i--){
        ce = listElem.at(i);
        if(getNumCell(ce) == cell){
            if(height < ce->getSizeElem().height())
               height = ce->getSizeElem().height();
            if(ce->isInputElem())/** Последний элемент в цепочке? */
               break;
        }
    }
    return height;
}

/**
 * Если х,у элемента не находятся в точке вершины схемы - сделаем коррекцию
 * @brief BuildElements::correctXY
 * @param coord
 * @param correct
 * @return
 */
int BuildElements::correctXY(int coord, int correct)
{
    return (coord/correct)*correct;
}

/**
 * Если х,у элемента не находятся в точке вершины схемы - сделаем коррекцию
 * @brief BuildElements::correctXY
 * @param p
 * @param correct
 * @return
 */
QPoint BuildElements::correctXY(QPoint p, int correct)
{
    p.setX((p.x()/correct)*correct);
    p.setY((p.y()/correct)*correct);

    return p;
}


