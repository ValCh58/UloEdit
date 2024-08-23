#ifndef DATAGRAHP_H
#define DATAGRAHP_H

#include <qvector.h>
#include <qpoint.h>
#include <qmap.h>
#include <qlist.h>
#include <qobject.h>
#include "bordertitleblock.h"
#include "vertex.h"
#include "terminal.h"
#include "schemealgo.h"

class Conductor;

class DataGraph:public QObject
{

public:
    DataGraph();
    ~DataGraph();
    int cntPoints;
    QMap<int, QList<int>> mapGraph;//Граф с ребрами//
    QMap<int, Vertex> vertex;//Карта схемы//
    void setPathFinder(QPoint p1, QPoint p2 , bool orto, bool updown);
    void setWalkability(QPoint p, QSize s);//Установить непроходимые вершины//
    void setWalkabilityArray(QPolygon& po);//Запись в 2д массив непроходимых вершин//
    QList<QPoint> listPath;//Возвращаемый путь//
    QList<QPoint> pathPoints;//Возвращаемый путь P(x,y)//
    bool isReverseShape(Conductor* co);
    void setTempWalkX(int x1, int x2, int y, int val);//Установка динамической непроходимости X//
    void setTempWalkY(int y1, int y2, int x, int val);//Установка динамической непроходимости Y//

private:
    BorderTitleBlock btBlock;
    int setGraphPoint();
    void makeArray(int row, int col);
    int getIndex(QPoint p);//Индекс 1д массива//
    int getArrX(int idx, int y);//Индекс Х 2д массива//
    int getArrY(int idx);//Индекс У 2д массива//
    int getIdxFrom2dArr(int x, int y);//Преобразуем индекс из 2д массива для 1д массива//
    void wayPoints();//Соотнесем точки пути с ключами карты//
    QPoint findPointMap(int ind);//Поиск точки ху в мапе по ключу//
    void setHightWalk(int x, int y, int hight=0);//Высота непроходимости//
    void setWidthWalk(const int x, const int y, const int width);//Ширина непроходимости//
    void setPointWalk(QPoint point, int val);
    void setPointWalkExt(QPoint point, int val);

    QVector<QPoint> points;
    int Row, Col;//Для двумерного массива//
//-----A*-------------------------------------------------------------------------------//
private:
    static const int mapWidth = 157, mapHeight = 300, tileSize = 1, numberPeople = 1;
    int onClosedList = 0;
    const int notfinished = 0, notStarted = 0;//Константы, связанные с путями
    const int found = 1, nonexistent = 2, NOTINDEX = -1;
    const int walkable = 0, unwalkable = 1;//Константы массива проходимости

    char walkability [mapWidth][mapHeight];//Массив содержит непроходимые вершины//
    int openList[mapWidth*mapHeight+2];   //Mассив cодержащий идентификаторы открытых элементов списка
    int whichList[mapWidth+1][mapHeight+1];  //Массив используемый для записи, находится ли ячейка
                                             //в открытом списке или в закрытом списке
    int openX[mapWidth*mapHeight+2]; //Массив сохраняет x-позицию элемента в открытом списке
    int openY[mapWidth*mapHeight+2]; //Массив сохраняет y-позицию элемента в открытом списке
    int parentX[mapWidth+1][mapHeight+1]; //Массив для хранения родителя каждой ячейки (x)
    int parentY[mapWidth+1][mapHeight+1]; //Массив для хранения родителя каждой ячейки (Y)
    int Fcost[mapWidth*mapHeight+2];	//Массив для хранения F стоимости ячейки в открытом списке
    int Gcost[mapWidth+1][mapHeight+1]; //Массив для хранения стоимости G для каждой ячейки
    int Hcost[mapWidth*mapHeight+2];	//Массив для хранения стоимости H для каждой ячейки
    int pathLength[numberPeople+1];     //Хранит длину найденного пути для объекта
    int pathLocation[numberPeople+1];   //Сохраняет текущее положение вдоль выбранного пути для объекта
    int* pathBank [numberPeople+1];


    //Переменные чтения пути
    int pathStatus[numberPeople+1];
    int xPath[numberPeople+1];
    int yPath[numberPeople+1];

    int xLoc [4]; int yLoc [4];

public:
    //-----------------------------------------------------------------------------
    int  FindPath (int startingX, int startingY, int targetX, int targetY, bool orto=true, bool upDown=true);
    void InitializePathfinder (void);
    void EndPathfinder (void);
    void getPath(int startX, int startY, int targetX, int targetY, bool orto, bool updown);
    //-----------------------------------------------------------------------------

};

#endif // DATAGRAHP_H
