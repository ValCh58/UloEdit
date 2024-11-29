#ifndef CELLGRID_H
#define CELLGRID_H

#include <QObject>

class CellGrid : public QObject
{
    Q_OBJECT
public:
    explicit CellGrid(QObject *parent = nullptr, QPointF outerP1, QPointF innerP2);
    virtual ~CellGrid();

    void setCellOuter_P1(QPointF P1);
    void setCellInner_P2(QPointF P2);
    void setCellBusy(bool flag);
    QPoint getCellOuter_P1();
    QPoint getCellInner_P2();
    bool getCellBusy();
    QSizeF cellOuterSize;
    QSizeF cellInnerSize;

private:
    QPointF cellOuter_P1; /** Точка ячейки внешняя */
    QPointF cellInner_P2; /** Точка внутри ячейки, точка фигуры */
    bool cellBusy; /** Ячейка занята/свободна true/false */

signals:

public slots:

};

#endif // CELLGRID_H
