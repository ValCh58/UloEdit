
#include <QDebug>

#include "diagram.h"
#include "schemealgo.h"
#include "schemeview.h"
#include "buildelements.h"

/**
 * Построение схемы
 * @brief Diagram::Diagram
 * @param view QTableView
 * @param firstRow первая строка
 * @param cntRow кол строк
 * @param parent
 */
Diagram::Diagram(QTableView *view, int firstRow, int cntRow, QWidget *parent) : QWidget(parent)
{
   View = view;
   setAttribute(Qt::WA_DeleteOnClose);

   /** Обработка выделенных записей */
   if(cntRow > 1){
      processSelectRecords(firstRow, cntRow);
   } else{
    /** Поиск связанных между собой записей алгоритма по одной выделенной строке в таблице */
        QModelIndex index = View->currentIndex();
        if(getFirstRow(index))
           getAllRows(index);
   }
}

/**
 * @brief Diagram::~Diagram
 */
Diagram::~Diagram(){}

/**
 * @brief Diagram::processSeltctRecords
 * @param firstRow первая выделенная запись
 * @param cntRow количество выделенных записей
 * @return
 */
bool Diagram::processSelectRecords(int firstRow, int cntRow)
{
    bool ok;
    bool retBool = false;
    int first = firstRow;
    /** Была ли выбрана строка в таблице */
    if(!(View->currentIndex()).isValid()) {return false;}
    if(cntRow > 1){
       UloData dt = ((UloModelTable*)View->model())->getUloData(first);
       /** Если операция = INPUT */
       if(dt.getCodOper().indexOf("6")==0){
          minIdx = dt.getNumCommandHex().toInt(&ok, 16);
          /** Заполним список QList<UloData> uloEdit данными */
          for(int i=0; i<cntRow; i++){
              UloData dat = ((UloModelTable*)View->model())->getUloData(first);
              first++;
              /** IF Прoчитан NOP, игнорируем */
              if(dat.getCodOper().toInt()!=7)
                 uloEdit << dat;
          }
          maxIdx = uloEdit.last().getNumCommandHex().toInt(&ok, 16);
          /**Для отладки!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
          foreach(UloData ue, uloEdit){
              qDebug()<<"K.O. "<<ue.getCodOper()<<"ЛЯ "<<ue.getLogCellCommand()<<" Operation "<<ue.getOperCommand();
          }
          retBool = true;
       }
    }
    return retBool;
}

/**
 * @brief Diagram::getFirstRow
 * @param QModelIndex idx индекс в таблице
 * @return
 */
bool Diagram::getFirstRow(QModelIndex idx)
{
    UloData dt;
    bool ok;

    if(idx.isValid()){
       dt = ((UloModelTable*)View->model())->getUloData(idx.row());
    }else{
       return false;
    }
    if(dt.getCodOper().indexOf("6")==0){
       if(getPrevRow(idx)){
          uloEdit.append(dt);
          minIdx = dt.getNumCommandHex().toInt(&ok, 16);
       }else{
           return false;
        }
    }else{
       return false;
    }
    return true;
}

/**
 * @brief Diagram::getAllRows
 * @param idx
 */
void Diagram::getAllRows(QModelIndex idx)
{
    QModelIndex tmpIdx = idx;
    if((tmpIdx = getNextRow(tmpIdx, "5")).isValid()){
        if(getNextRow(tmpIdx, "6").isValid()){
           delUndesiredRows(); /** Delete this a row from uloEdit */
        }
    }
}

/**
 * Смотрим последнюю операцию в предыдущей записи,
 * если недопустимая команда между INPUT и OUT - Ошибка.
 * @brief Diagram::getPrevRow
 * @param idx
 * @return
 */
bool Diagram::getPrevRow(QModelIndex idx)
{
    bool ret = false;

    if(idx.isValid()){
       if(idx.row()>0){
          int row = idx.row();
          while(row >0){
             UloData dat = ((UloModelTable*)View->model())->getUloData(--row);
             /** IF Прочитан OUT */
             if(dat.getCodOper().indexOf("5")==0){
                ret = true;
                break;
             }else if(dat.getCodOper().indexOf("7")==0){//Прoчитан NOP, игнорируем//
                      continue;
             }else{
                 ret =false;/** Ошибка, недопустимая команда между INPUT и OUT */
                 break; /** Индекс неправильный */
             }
          }
       }else{
           ret = true;/** Стоим на первой строке таблицы */
       }
    }

    return ret;
}

/**
 * Go to next row
 * @brief Diagram::getNextRow
 * @param idx
 * @param k_o
 * @return
 */
QModelIndex Diagram::getNextRow(QModelIndex idx, QString k_o)
{
    QModelIndex ret;
    UloData datPrev;

    if(idx.isValid()){
        if(idx.row() <= 0x7FFC){
           int row = idx.row();
           while(row >=0 && row <= 0x7FFC){
             UloData dat = ((UloModelTable*)View->model())->getUloData(++row);
             if(dat.getCodOper().indexOf("7")==0){
                if(row == 0x7FFC){
                   uloEdit.append(dat);
                   break;
                }
                continue;
             }

             UloData tmp = ((UloModelTable*)View->model())->getUloData(row-1);
             if(!tmp.getCodOper().indexOf("7")==0){
                datPrev = ((UloModelTable*)View->model())->getUloData(row-1);
             }

             if(dat.getCodOper().indexOf(k_o)==0){/** Прочитан K.O. */
                uloEdit.append(dat);
                if(k_o.toInt()==5 && dat.getLogCellCommand().toInt()==0){//Найдем первый OUT//
                    ret = ((UloModelTable*)View->model())->index(row, 0);
                    break;
                }
                if(datPrev.getLogCellCommand().toInt()==0 &&
                        k_o.toInt()==6 &&
                                 dat.getLogCellCommand().toInt()==1){/** Найдем INPUT после OUT */
                   ret = ((UloModelTable*)View->model())->index(row, 0);
                   break;
                }
             }else{
                if(!(dat.getCodOper().indexOf("7")==0)){
                   uloEdit.append(dat);
                }
             }
           }
        }
    }

    return ret;
}

/**
 * Удалим последний INPUT для случая автоматического поиска конца алгоритма
 * @brief Diagram::delUndesiredRows
 */
void Diagram::delUndesiredRows()
{
  bool ok;

  if(!uloEdit.isEmpty()){
     if(uloEdit.last().getCodOper().toInt()==6 || uloEdit.last().getCodOper().toInt()==7)
        uloEdit.removeLast();
     maxIdx = uloEdit.last().getNumCommandHex().toInt(&ok, 16);
  }

  foreach(UloData ue, uloEdit){
      qDebug()<<"K.O. "<<ue.getCodOper()<<" Operation "<<ue.getOperCommand();
  }
}

/**
 * Разбор элементов алгоритма
 * @brief Diagram::parserItemAlgo
 * @param sz
 * @param title
 */
void Diagram::parserItemAlgo(QSize *sz, QString *title)
{
   *title=uloEdit.at(0).getNumCommandHex()+"-"+uloEdit.last().getNumCommandHex();//Заголовок для вкладки//

   foreach(UloData d, uloEdit){

       switch(d.getCodOper().toInt()){
       case O_UT://5
            processOut(d);//Создание таймера//
           break;
       case INP://6
            processInput(d);//Обработка INPUT всех элементов//
           break;

       case AND://2
       case AND_N://3
            processAnd(d);
           break;

       case OR://0
       case OR_N://1
            processOr(d);
           break;

       case UNI://4
           if(isTg(d.getOperandCommand()))//триггеры={0xB2, 0xBA, 0x32}//
              processUniTg(d);
           else if(isLogFunc(d.getOperandCommand()))//Лoг. Функция={FE, EE, 80, 88}//
              processUni(d);
           else if(isLogFunc_E8(d.getOperandCommand()))//Лoг. Функция={E8 -> 2/3}//
              processUni_E8(d);
           else if(isLogFunc_F9(d.getOperandCommand()))//L.F.={F9, F6} цепочка компарaторов равенство/неравенство //
              processUni_F9(d);
           else if(isLogFunc_99(d.getOperandCommand()))//Лoг. Функция={99, 66}//
              processUni_E8(d);
           else if(isLogFunc_41(d.getOperandCommand()))//Лoг. Функция={41}//
              processUni_E8(d);
           break;
       }
   }
   createScheme(sz);
}

/**
 * Создание таймера/БВРа
 * @brief Diagram::processOut
 * @param ud
 */
void Diagram::processOut(UloData ud)
{
    bool ok;

    if(ud.getOperandCommand().toInt(&ok, 16)>=0x200 && ud.getOperandCommand().toInt(&ok, 16) <= 0x2FF){//timer
       processTimer(ud);
    }
    else if((ud.getOperandCommand().toInt(&ok, 16)>=0x400 && ud.getOperandCommand().toInt(&ok, 16) <= 0x41F) ||
            (ud.getOperandCommand().toInt(&ok, 16)>=0x500 && ud.getOperandCommand().toInt(&ok, 16) <= 0x51F) ||
            (ud.getOperandCommand().toInt(&ok, 16)>=0x520 && ud.getOperandCommand().toInt(&ok, 16) <= 0x53F))  {//bvr
       processBVR(ud);
    }

}

/**
 * @brief Diagram::processTimer
 * @param ud
 */
void Diagram::processTimer(UloData ud)
{
    bool ok;

    mapCnt = createMapElements(&ud);
    UloData tmp = ud;
    tmp.setCodOper("6");
    tmp.setOperCommand("TIMER");
    tmp.setLogCellCommand("0");
    addElementToMap(mapCnt, &tmp);
    QString s=QString("%1").arg(tmp.getNumCommandHex().toInt(&ok, 16)+1, 4, 16);
    tmp.setNumCommandHex(s);
    tmp.setCodOper("5");
    tmp.setLogCellCommand("1");
    addElementToMap(mapCnt, &tmp);
}

/**
 * @brief Diagram::processBVR
 * @param ud
 */
void Diagram::processBVR(UloData ud)
{
    bool ok;

    mapCnt = createMapElements(&ud);
    UloData tmp = ud;
    tmp.setCodOper("6");
    tmp.setOperCommand("BVR");
    tmp.setLogCellCommand("1");
    addElementToMap(mapCnt, &tmp);
    QString s=QString("%1").arg(tmp.getNumCommandHex().toInt(&ok, 16)+1, 4, 16);
    tmp.setNumCommandHex(s);
    tmp.setCodOper("5");
    tmp.setLogCellCommand("0");
    addElementToMap(mapCnt, &tmp);
}

/**
 * @brief Diagram::processInput
 * @param ud
 */
void Diagram::processInput(UloData ud)
{
    bool ok;

    if(itemAlgo.isEmpty()){
       mapCnt = createMapElements(&ud);//Карта пустая, создадим первый элемент//
    }else {
       UloData dt = getPrevElement(&ud);//шаг назад к предыдущему элементу списка//
       if(isKoEquKo(&ud, &dt)){//Если предыдущий codOper и текущий codOper равны то это
          addElementToMap(mapCnt, &ud);//один и тот же элемент//
          QMap<int, UloData> tMap = itemAlgo.value(mapCnt);
          //Если это триггер -> Требуется ли корректировка по входam триггера?//
          if(tMap.size()==2 && tMap.first().getLogCellCommand().toInt()==2
                            && isTrg(tMap.first().getNumCommandHex().toInt(&ok, 16)+1)//Проверка типа триггера//
            ){
              correctTrig1();//Перед входом есть промежуточные элементы//
          }
       }else{
             mapCnt = createMapElements(&ud);//Создается следующий новый элемент//
             QMap<int, UloData> tMap = itemAlgo.value(mapCnt);//Текущий элемент коллекции//

             //Если это триггер -> Требуется ли корректировка по входam триггера?//
             if(tMap.size()==1 && tMap.first().getLogCellCommand().toInt()==3
                               && isTrg(tMap.first().getNumCommandHex().toInt(&ok, 16))){
                correctTrig1();//Перед входом есть промежуточные элементы//
             }
       }
    }
    isOutKo(&ud);//Добавляем все OUT map элементов//
}


/**
 * Создадим AND элемент
 * @brief Diagram::processAnd
 * @param ud
 */
void Diagram::processAnd(UloData ud)
{
     UloData dt = getPrevElement(&ud);
     if((isLyEquLy(&ud, &dt)&&isElemAnd(&dt))||isElemInput(&dt)){
        addElementToMap(mapCnt, &ud);//К предыдущему АND или INPUT добавится текущий элемент//
     }else{
         mapCnt = createMapElements(&ud);
     }
     isOutKo(&ud);
}

/**
 * Создадим OR элемент
 * @brief Diagram::processOr
 * @param ud
 */
void Diagram::processOr(UloData ud)
{
    UloData dt = getPrevElement(&ud);
    if((isLyEquLy(&ud, &dt)&&isElemOr(&dt))||isElemInput(&dt)){
       addElementToMap(mapCnt, &ud);//Добавляем записи к элементу//
    }else{
        mapCnt = createMapElements(&ud);//Создаем первую запись элемента//
    }
    isOutKo(&ud);
}

/**
 * Универсальный элемент
 * @brief Diagram::processUni
 * @param ud
 */
void Diagram::processUni(UloData ud)
{
    UloData dt = getPrevElement(&ud);
    if(isElemInput(&dt)&&(dt.getLogCellCommand().toInt()==3
       || dt.getLogCellCommand().toInt()==2 || dt.getLogCellCommand().toInt()==1)){
     addElementToMap(mapCnt, &ud);
    }else{
          mapCnt = createMapElements(&ud);
          //correctUni();
    }
    isOutKo(&ud);
    //Вызов функции обработки карты элементов  QMap <int, QMap<int, UloData>> itemAlgo ЛФ{FE,EE,80,88}//

    processElemMap();

}

/**
 * @brief Diagram::processUni_E8
 * @param ud
 */
void Diagram::processUni_E8(UloData ud)
{
    UloData dt = getPrevElement(&ud);
    if(isElemInput(&dt)&&(dt.getLogCellCommand().toInt()==3
       || dt.getLogCellCommand().toInt()==2 || dt.getLogCellCommand().toInt()==1)){
     addElementToMap(mapCnt, &ud);
    }else{
          mapCnt = createMapElements(&ud);
    }
    isOutKo(&ud);
}

/**
 * Построение компаратора равенства/неравенство
 * @brief Diagram::processUni_F9
 * @param ud
 */
void Diagram::processUni_F9(UloData ud)
{
    UloData dt = getPrevElement(&ud);
    if(isElemInput(&dt)&&(dt.getLogCellCommand().toInt()==3
       || dt.getLogCellCommand().toInt()==2 || dt.getLogCellCommand().toInt()==1)){
     addElementToMap(mapCnt, &ud);
    }else{
          mapCnt = createMapElements(&ud);
    }
    isOutKo(&ud);
    //Искать вход №3 и удалить его//
    processElemMap_F9();
}

/**
 * Искать вход №3 и удалить его
 * @brief Diagram::processElemMap_F9
 */
void Diagram::processElemMap_F9()
{
    QMap <int, QMap<int, UloData>>::Iterator iter = itemAlgo.end();

    if(iter == itemAlgo.end()){
       iter--;
    }
    else
       return;

    QList<int> lKey(iter.value().keys());//Ключи подкарты элемента//
    //Ищем Input 3 //
    for(int k : lKey){
        if(iter.value().value(k).getLogCellCommand() == "3"){
           iter.value().remove(k);
           break;
        }
    }

}

/**
 * Oбработкa карты элементов  QMap <int, QMap<int, UloData>> itemAlgo
 * @brief Diagram::processElemMap
 */
void Diagram::processElemMap()
{
   QMap <int, QMap<int, UloData>>::Iterator mainIter = itemAlgo.end();
   QList<int> keysMap;

   searchUniItemsMap(mainIter, keysMap);
   mapUniItemsInsDel(keysMap);//Откорректировать Л.Ф. = Е8!!!-???
}

/**
 * Поиск карты первого элемента UniElement
 * @brief Diagram::searchUniItemsMap
 * @param it
 * @param l
 */
void Diagram::searchUniItemsMap(QMap <int, QMap<int, UloData>>::Iterator& it, QList<int> &l)
{
    //Ищем начало элемента////////////////////////
    while(it != itemAlgo.begin()){
        it--;

        l.append(it.key());//Сохраним ключи подкарт элементов//
        if(it.value().first().getLogCellCommand()=="1")
           break;
    }
}

/**
 * Перекомпановка элементов путем их объединения
 * Вставка в первую карту элемента и удаление карт из которых сделана вставка
 * @brief Diagram::mapUniItemsInsDel
 * @param l
 */
void Diagram::mapUniItemsInsDel(QList<int>& l)
{
    int keyMap = 0;
    if(l.isEmpty()) return;

    keyMap = l.last();//Ключ первой подкарты//
    for(int sz=l.size()-2; sz >= 0; sz--){//Цикл по подкартам которые нужно скопировать//
        QMap<int, UloData> map = itemAlgo.value(l.at(sz));//Подкарта для копирования//
        for(UloData d : map){
            addElementToMap(keyMap, &d);//Запись из map в первую подкарту элемента//
        }
        itemAlgo.remove(l.at(sz));//Удалим подкарту из которой копировали данные//
    }
}

/**
 * Не используется !!!
 * @brief Diagram::correctUni
 */
void Diagram::correctUni()
{
    int flagCell1=0,flagCell2=0, flagCell3=0;

     for(int i=uloEdit.size()-1; i>=0; i--){
         UloData d = uloEdit.at(i);
         if(d.getCodOper().toInt()==6){
             if(d.getLogCellCommand().toInt()==3 && flagCell3==0){
                d.setOperandCommand("");
                addElementToMap(mapCnt, &d);
                flagCell3++;
             }else if(d.getLogCellCommand().toInt()==2 && flagCell2==0){
                 d.setOperandCommand("");
                 addElementToMap(mapCnt, &d);
                 flagCell2++;
              }else if(d.getLogCellCommand().toInt()==1 && flagCell1==0){
                 d.setOperandCommand("");
                 addElementToMap(mapCnt, &d);
                 flagCell1++;
              }
         }
     }
}

/**
 * @brief Diagram::processUniTg
 * @param ud
 */
void Diagram::processUniTg(UloData ud)
{
     UloData dt = getPrevElement(&ud);
     if(isElemInput(&dt)&&dt.getLogCellCommand().toInt()==3){//Триггер//
        addElementToMap(mapCnt, &ud);
        isOutKo(&ud);
     }
}

/**
 * is trigger
 * @brief Diagram::isTg
 * @param operand
 * @return
 */
bool Diagram::isTg(QString operand)
{
    bool retVal = false, ok;

     if(operand.toInt(&ok, 16)==0xB2 || operand.toInt(&ok, 16)==0xBA || operand.toInt(&ok, 16)==0x32)
     {
        retVal = true;
     }
     return retVal;
}

/**
 * Логическая функуция ?
 * @brief Diagram::isLogFunc
 * @param operand
 * @return
 */
bool Diagram::isLogFunc(QString operand)
{
    bool retVal = false, ok;

     if( operand.toInt(&ok, 16)==0xFE || operand.toInt(&ok, 16)==0xEE ||
         operand.toInt(&ok, 16)==0x80 || operand.toInt(&ok, 16)==0x88 )
     {
         retVal = true;
     }
     return retVal;
}

/**
 * Логическая функуция E8 ?
 * @brief Diagram::isLogFunc_E8
 * @param operand
 * @return
 */
bool Diagram::isLogFunc_E8(QString operand)
{
    bool retVal = false, ok;

     if(operand.toInt(&ok, 16) == 0xE8/*LF 2/3*/ )
     {
         retVal = true;
     }
     return retVal;
}

/**
 * Логическая функуция F9 ?
 * @brief Diagram::isLogFunc_F9
 * @param operand
 * @return
 */
bool Diagram::isLogFunc_F9(QString operand)
{
    bool retVal = false, ok;

     if(operand.toInt(&ok, 16) == 0xF9 || operand.toInt(&ok, 16) == 0xF6)
     {
        retVal = true;
     }
     return retVal;
}

/**
 * Логическая функуция 99 ?
 * @brief Diagram::isLogFunc_99
 * @param operand
 * @return
 */
bool Diagram::isLogFunc_99(QString operand)
{
    bool retVal = false, ok;

     if(operand.toInt(&ok, 16) == 0x99 || operand.toInt(&ok, 16) == 0x66)
     {
         retVal = true;
     }
     return retVal;
}

/**
 * Логическая функуция 41 ?
 * @brief Diagram::isLogFunc_41
 * @param operand
 * @return
 */
bool Diagram::isLogFunc_41(QString operand)
{
    bool retVal = false, ok;

     if(operand.toInt(&ok, 16) == 0x41)
     {
        retVal = true;
     }
     return retVal;
}

/**
 * Создать подкарту элементов
 * @brief Diagram::createMapElements
 * @param d
 * @return
 */
int Diagram::createMapElements(UloData *d)
{
    bool ok;
    int mapKey = d->getNumCommandHex().toInt(&ok, 16);
    QMap<int, UloData> map;
    map.insert(mapKey, *d);
    itemAlgo.insert(mapKey,map);
    return mapKey;
}

/**Движение по элементам коллекции-----------------------------*/
/**
 * Получим предыдущий элемент
 * @brief Diagram::getPrevElement
 * @param d
 * @return
 */
UloData Diagram::getPrevElement(UloData *d)
{
    bool ok;
    UloData dataRet;

    if(d->getNumCommandHex().toInt(&ok, 16) > minIdx)
        if(uloEdit.contains(*d)){
           int i = uloEdit.indexOf(*d);
           dataRet = uloEdit.at(i-1);
        }
    return dataRet;
}

/**
 * Получим следующий элемент
 * @brief Diagram::getNextElement
 * @param d
 * @return
 */
UloData Diagram::getNextElement(UloData *d)
{
    bool ok;
    UloData dataRet;

    if(d->getNumCommandHex().toInt(&ok, 16) < maxIdx)
        if(uloEdit.contains(*d)){
           int i = uloEdit.indexOf(*d);
           dataRet = uloEdit.at(i+1);
        }

    return dataRet;
}

/**
 * Сравнение КОД ОПЕР предыдущего и текущего элементов
 * @brief Diagram::isKoEquKo
 * @param curr
 * @param prev
 * @return
 */
bool Diagram::isKoEquKo(UloData *curr, UloData *prev)
{
    bool ok;
    if(!prev->getCodOper().isEmpty() && !curr->getCodOper().isEmpty())
       return prev->getCodOper().toInt(&ok, 16)==curr->getCodOper().toInt(&ok, 16);
    return false;
}

/**
 * Сравнение Л.Я. предыдущего и текущего элементов
 * @brief Diagram::isLyEquLy
 * @param curr
 * @param prev
 * @return
 */
bool Diagram::isLyEquLy(UloData *curr, UloData *prev)
{
    if(!prev->getLogCellCommand().isEmpty() && !curr->getLogCellCommand().isEmpty())
       return prev->getLogCellCommand().toInt()==curr->getLogCellCommand().toInt();
    return false;
}

/**
 * Добавляем все OUT map элементов
 * @brief Diagram::isOutKo
 * @param d
 */
void Diagram::isOutKo(UloData *d)
{
     UloData next = getNextElement(d);//Получим следующий элелемент//
     if(!next.getNumCommandHex().isEmpty()){
        if(next.getCodOper().toInt()==5){//Если команда OUT?//
           addElementToMap(mapCnt, &next);
           isOutKo(&next);
        }
     }
}

/**
 * Элемент OR ?
 * @brief Diagram::isElemOr
 * @param d
 * @return
 */
bool Diagram::isElemOr(UloData *d)
{
    return d->getCodOper().toInt()==OR || d->getCodOper().toInt()==OR_N;
}

/**
 * Элемент AND ?
 * @brief Diagram::isElemAnd
 * @param d
 * @return
 */
bool Diagram::isElemAnd(UloData *d)
{
    return d->getCodOper().toInt()==AND || d->getCodOper().toInt()==AND_N;
}

/**
 * Элемент INPUT ?
 * @brief Diagram::isElemInput
 * @param d
 * @return
 */
bool Diagram::isElemInput(UloData *d)
{
    return d->getCodOper().toInt()==INP;
}

/**
 * Вставка элемента в подкарту
 * @brief Diagram::addElementToMap
 * @param key
 * @param d
 */
void Diagram::addElementToMap(int key, UloData *d)
{
    bool ok;
    QMap <int, QMap<int, UloData>>::iterator i;
    i=itemAlgo.find(key);//Ключ подкарты//
    i->insert(d->getNumCommandHex().toInt(&ok, 16),*d);//ВСтавка в подкарту//
}

/**
 * Коррекция тригера
 * @brief Diagram::correctTrig1
 */
void Diagram::correctTrig1()
{
     int input1=0, input2=0;
     bool ok;
     UloData tmpDat;
     QMap <int, QMap<int, UloData>>::iterator i=itemAlgo.end();
     i--;
     while(i != itemAlgo.begin()){
         i--;
         QList<int> keys = i->keys();
         int locInput1=0, locInput2=0;
         //for(int k:keys){
         for(int id=keys.size()-1; id >=0; id--){
             //UloData val = i->value(k);
             UloData val = i->value(keys.at(id));
             if(val.getCodOper().toInt()==4){
                 return; //Обнаружен следующий триггер//
             }
             if(val.getCodOper().toInt() == 6 && val.getLogCellCommand().toInt() == 1){
                locInput1++;
                input1 = locInput1;
                tmpDat = val;
             }else if(val.getCodOper().toInt() == 6 && val.getLogCellCommand().toInt() == 2){
                locInput2++;
                input2 = locInput2;
             }
             //Если в одном блоке два INPUT//
             if(locInput1 > 0 && locInput2 > 0){
                QMap <int, QMap<int, UloData>>::iterator iter=itemAlgo.end();
                iter--;
                iter->insert(tmpDat.getNumCommandHex().toInt(&ok, 16),tmpDat);
                QMap<int, UloData>::iterator tmp = i->begin();
                i->erase(tmp);
                return;
             }
         }
         if(input1 > 0 && input2 > 0)
            return;
     }
}

/**
 *  Возврат типа триггера
 * @brief Diagram::isTrg
 * @param comHex
 * @return
 */
bool Diagram::isTrg(int comHex)
{
    bool ret = true, ok;
    int idx = 0;

    if(uloEdit.size() == 0){
       return ret;
    }

    for(UloData d:uloEdit){
        if(d.getNumCommandHex().toInt(&ok, 16) == comHex){
           UloData dat = uloEdit.at(idx+1);
           if(dat.getCodOper().toInt()==4 &&
                     (dat.getOperandCommand().toInt(&ok, 16) == 0xBA
                   || dat.getOperandCommand().toInt(&ok, 16) == 0xB2
                   || dat.getOperandCommand().toInt(&ok, 16) == 0x32)){
               ret = false;
               break;
           }
        }
        idx++;
    }
    return ret;
}

/**
 * Создание схемы алгоритма
 * @brief Diagram::createScheme
 * @param sz
 */
void Diagram::createScheme(QSize *sz)
{
   /** Сцена */
   SchemeAlgo *sa = new SchemeAlgo(this);
   SchemeView *sv = new SchemeView(sa);

   QHBoxLayout *hbox = new QHBoxLayout(this);
   hbox->addWidget(sv);
   setLayout(hbox);
   sz->setWidth(sa->border_and_titleblock.diagramWidth()+70.0);
   sz->setHeight(sa->border_and_titleblock.diagramHeight());

   BuildElements *buildElm = new BuildElements(sa, &itemAlgo);
   buildElm->greateElements();

}

/**
 * @brief Diagram::printGroupAlgo
 */
void Diagram::printGroupAlgo()
{
    QMap <int, QMap<int, UloData>>::iterator it=itemAlgo.begin();

    while(it != itemAlgo.end()){
          QList<int> keys = it->keys();
          qDebug()<<"------------New Elements-------------";
          for(int idx:keys){
              UloData map = it->value(idx);
              qDebug()<<map.getNumCommandHex()<<" "<<map.getCodHex()<<" "<<map.getCodOper()<<" "<<map.getLogCellCommand()
                     <<" "<<map.getOperandCommand()<<" "<<map.getOperCommand();
          }
          it++;
    }
}

