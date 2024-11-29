#include "cellgrid.h"

CellGrid::CellGrid(QObject *parent, QPointF outerP1, QPointF innerP2) : QObject(parent)
{
    cellBusy = false;
    cellOuter_P1 = outerP1;
    cellInner_P2 = innerP2;
    cellOuterSize = QSize(61.0, 66.0);
    cellInnerSize = QSize(48.0, 56.0);

}

CellGrid::~CellGrid(){}

void CellGrid::setCellOuter_P1(QPointF P1)
{
     cellOuter_P1 = P1; /** Точка ячейки внешняя */
}

void CellGrid::setCellInner_P2(QPointF P2)
{
     cellInner_P2 = P2; /** Точка внутри ячейки, точка фигуры */
}

void CellGrid::setCellBusy(bool flag)
{
     cellBusy = flag; /** Ячейка занята/свободна true/false */
}

QPoint CellGrid::getCellOuter_P1()
{
     return cellOuter_P1;
}

QPoint CellGrid::getCellInner_P2()
{
     return cellInner_P2;
}

bool CellGrid::getCellBusy()
{
     return cellBusy;
}
