#ifndef CONDUCTOR_H
#define CONDUCTOR_H

#include <QObject>
#include <QGraphicsPathItem>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QLinkedList>

class DataGraph;
class Terminal;
class ConductorSegment;
class CustomElement;

//Соединения между элементами//////////////////////////////
class Conductor : public QObject, public QGraphicsPathItem
{
    Q_OBJECT

public:
    //Conductor(Terminal *p1, Terminal *p2, CustomElement *e1, CustomElement *e2);
    Conductor(Terminal *p1, Terminal *p2, CustomElement *e1, CustomElement *e2, DataGraph &dg);
    virtual ~Conductor();

    Terminal *terminal1;// присоединен ко входу elem2
    Terminal *terminal2;// присоединен ко выходу elem1

    CustomElement *elem1;//Элемент выходной
    CustomElement *elem2;//Элемент входной

    void paint(QPainter *qp, const QStyleOptionGraphicsItem *sg, QWidget *qw);
    QRectF boundingRect() const;
    virtual QPainterPath shape() const;
    enum Highlight { None, Normal, Alert };
    DataGraph& dgr;

    QList<ConductorSegment> getSegments() const;
    QList<ConductorSegment> &getRefSegments();
    DataGraph &getDgr() const;
    CustomElement *getElem2() const;
    CustomElement *getElem1() const;
    void pointToPath();

    Terminal *getTerminal1() const;
    Terminal *getTerminal2() const;

    void deleteSegments();
    void recreationSegments();

private:
    QPointF getPoint(Terminal *t);
    void connectOutIn();
    void connectOutInAStar();
    void segmentsToPath();
    // Сегменты, составляющие проводник
    QList<ConductorSegment> segments;
    QList<ConductorSegment>& segmentsRef = segments;
    // QPen и QBrush используемые для рисования проводников
    static QPen conductor_pen;
    static QBrush conductor_brush;

};

#endif // CONDUCTOR_H
