#ifndef SCHEMEALGO_H
#define SCHEMEALGO_H

#include <QObject>
#include <QtWidgets>
#include "bordertitleblock.h"

class SchemeAlgo : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit SchemeAlgo(QObject *parent=nullptr);
    virtual ~SchemeAlgo();
    // margin around the diagram
    static const qreal margin;
    static QColor background_color;
    static const int xGrid;
    static const int yGrid;
    BorderTitleBlock border_and_titleblock;
    static QPointF snapToGrid(const QPointF &p);

    void addItem(QGraphicsItem *item);

private:
    QGraphicsLineItem *conductor_setter_;


    bool draw_grid_;
    bool use_border_;

protected:
    virtual void drawBackground(QPainter *p, const QRectF &r);


signals:

public slots:
    void adjustSceneRect();
};

#endif // SCHEMEALGO_H
