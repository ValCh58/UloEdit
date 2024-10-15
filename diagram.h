#ifndef DIAGRAM_H
#define DIAGRAM_H

#include <QObject>
#include <QWidget>
#include <QTableView>
#include <QMap>

#include "ulodata.h"
#include "ulo.h"
#include "ulomodeltable.h"
#include "andelement.h"



class SchemeAlgo;
class SchemeView;
class BuildElements;


class Diagram : public QWidget
{
    Q_OBJECT
public:
    explicit Diagram(QTableView *view, int firstRow = -1, int cntRow = 0, QWidget *parent = 0);
    ~Diagram();
    void parserItemAlgo(QSize *sz, QString *title);
    void printGroupAlgo();


private:
    QTableView *View;
    /** Записи алгоритма */
    QList<UloData> uloEdit;
    UloData beginRow;
    int cntRows;
    void getRelatedRecords(QModelIndex idx, QList<QModelIndex> *jumpNextEl);
    bool getFirstRow(QModelIndex idx);
    void getAllRows(QModelIndex idx, QList<UloData> *coUlo);
    bool getPrevRow(QModelIndex idx);
    QModelIndex getNextRow(QModelIndex idx, QString k_o, QList<UloData> *coUlo);
    void delUndesiredRows();
    /** Элементы алгоритма (строки из файла) */
    QMap <int, QMap<int, UloData>> itemAlgo;
    void processOut(UloData ud);
    void processTimer(UloData ud);
    void processBVR(UloData ud);
    void processInput(UloData ud);
    void processAnd(UloData ud);
    void processOr(UloData ud);
    void processUni(UloData ud);
    void processUni_E8(UloData ud);
    void processUni_F9(UloData ud);
    void processUniTg(UloData ud);
    void processElemMap();
    void processElemMap_F9();
    /** Поиск карты первого элемента UniElement */
    void searchUniItemsMap(QMap <int, QMap<int, UloData>>::Iterator &it, QList<int> &l);
    /** Вставка в первую карту элемента и удаление карт из которых сделана вставка */
    void mapUniItemsInsDel(QList<int> &l);
    bool isTg(QString operand);
    bool isLogFunc(QString operand);
    bool isLogFunc_E8(QString operand);
    bool isLogFunc_F9(QString operand);
    bool isLogFunc_99(QString operand);
    bool isLogFunc_41(QString operand);
    int mapCnt;
    int createMapElements(UloData *d);
    UloData getPrevElement(UloData *d);
    UloData getNextElement(UloData *d);
    bool isKoEquKo(UloData *curr, UloData *prev);
    bool isLyEquLy(UloData *curr, UloData *prev);
    void isOutKo(UloData *d);
    bool isElemOr(UloData *d);
    bool isElemAnd(UloData *d);
    bool isElemInput(UloData *d);
    void addElementToMap(int key, UloData *d);
    void correctTrig1();
    void correctUni();
    bool isTrg(int comHex);
    int minIdx;
    int maxIdx;
    void createScheme(QSize *sz);
    bool processSelectRecords(int firstRow = -1, int cntRow = 0);
    void clearSelectedRows(); //add 09-10-2024//
    QModelIndex findModIndex(QString operand, int cell4, QString codeHex, int cell0); //add 10-10-2024//
    void setSelectRow(QItemSelectionModel *selectionMod, QAbstractItemModel *myModel, QModelIndex commandInd, int row);

signals:

public slots:
};

#endif // DIAGRAM_H
