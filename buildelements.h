#ifndef BUILDELEMENTS_H
#define BUILDELEMENTS_H

#include <QMap>

#include "diagram.h"
#include "datagrahp.h"


class UloData;
class SchemeAlgo;
class CustomElement;
class Terminal;
class OrElement;
class UniElement;
class UniElement_LF_E8;
class UniElement_F9;
class TrgElement;
class TmrElement;
class BorderTitleBlock;
class Conductor;
class PointItem;


class BuildElements
{
public:
    BuildElements(SchemeAlgo *scheme, QMap<int, QMap<int, UloData> > *mapElem);
    ~BuildElements();

    void greateElements();
    void paintElements();
    QPolygon points;


private:
    void testIntersectionPointWithLine(QPointF pf);//Анализ пересечения точки терминала с линией кондуктора//
    void typeLineCorrection1(int cntPoint);//Тип коррекции линий соединения №1.//
    void typeLineCorrection2();//Тип коррекции линий соединения №2. Cоединение верхнего терминала пересекаются соединением нижнего.//
    QList<Conductor*> selectBusyTerminals(CustomElement *el);//Отбор терминалов имеющих соединения//
    void swapAndMakeCo(QList<Conductor*> list);//Замена терминалов и построение их соединений//
    void testDrawPoints();
    void drawElements();
    void drawConductors();
    void terminalCorrections();
    void terminalCorrectionFreeOut();//
    //Создание графа//
    DataGraph dGraph; //19.06!!!
    SchemeAlgo *scene;
    QMap<int, QMap<int, UloData>> *ulodata;
    QList<CustomElement*> listElem;
    QList<Conductor*> listCo;
    void createConductors();
    void createConductorsAndOr();
    void createConductorsAll();
    void createConductorsAllWest();//24-10-2024
    bool isTimersEl(CustomElement *el, CustomElement *elNext);
    void createConductorsTg();
    void createConductorsUni();
    bool isElTermOut(CustomElement *el);
    bool isElTermInp(const CustomElement *el);

    int getNumCell(CustomElement *el);
    int getNumCellTg(CustomElement *el, int cell);
    Terminal* getElTermInpTg(CustomElement *el, int cell) const;
    Terminal* getElTermForTg(CustomElement *el, int cell, CustomElement *elprev);
    Terminal* serchByNameTerm(CustomElement *el, QString name, Ulo::Orientation ori) const;
    Terminal* getP1Terminal(QList<Terminal*> term, CustomElement *el);
    Terminal* getP2Terminal(QList<Terminal*> term);
    void getP1P2Terminal(CustomElement *currEl, CustomElement *nextEl);
    void getP1P2TerminalWest(CustomElement *currEl, CustomElement *nextEl); //24-10-2024
    bool isShortCircTimer(Terminal *term);

    int getMaxLy(CustomElement *el);//Получим мах номер лог. ячейки//
    bool scanInputUniEl(CustomElement *el, int numCell);//Просмотрим первичные группы элем. универсального элемента на содержание К.О. INPUT//
    void createTermInputUni(int logo, UloData *dat, int num, int idx);//Создать недостающий вход унив. элемента//
    void setTypePrimElUni(CustomElement *el);//Установим типы (AND, OR) первичных элементов универсального элемента//
    bool createUniInp(int cell);
    void createTerminal(UloData *dat, int num, int idx);
    void createTerminal(int logo, UloData *dat, int num, int idx);
    void createMissingTerminal(UloData *dat, int num, int idx, QString codeOper);
    bool isNotInputTerm(QList<UloData> &va);
    void createMissingTermInput(int logo, UloData *dat, int num, int idx);
    int getTermINpTypeTg(int idx);
    int getTypeElement(QList<UloData> &va, int *typeTg = 0);
    CustomElement *buildElement(int type, int typeTg = 0);
    void calcSizeElements();
    QSizeF getSizeElem(CustomElement *elEl);
    QSizeF getSizeTg(CustomElement *elTg);
    void calcPosElements();
    QPointF setPointElem(CustomElement *el);
    QPointF setLayout1(CustomElement *el);
    QRectF paper;
    qreal indentEdge;//Отступ от края элемента//
    QPointF startPoint;
    QPointF tmpPoint;//Временные координаты point(x,y)//
    qreal tmpDeltaY;
    QString getNameUniEl(QList<UloData> v);
    qreal getCoordYForElemTg(CustomElement *el, int *cnt3);
    bool isCoordMove(qreal width);
    bool isNotTransferEl(CustomElement *el);
    bool isTransferEl(CustomElement *el, int cell);
    bool isCorrectEl(CustomElement *el);
    bool isElCell(CustomElement *el, int cellNum);
    void correctElementsAfter(CustomElement *el);
    int getLastPlusX(CustomElement *el, QPointF *point);
    void getCoordXForElemTg(CustomElement *el, qreal X, int *cnt1, int *cnt2);
    void correctElements(CustomElement *el, int cnt, int cnt2);
    qreal getHeightElemCellInpS(int index, int cell);
    int correctXY(int coord, int correct);//Коррекция координат точки фигуры. Вычислим ближайшую точку вершины//
    QPoint correctXY(QPoint p, int correct);

};

#endif // BUILDELEMENTS_H
