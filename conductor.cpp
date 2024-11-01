#include <QDebug>

#include "conductor.h"
#include "conductorsegment.h"
#include "terminal.h"
#include "customelement.h"
#include "datagrahp.h"


#define PR(x) qDebug() << #x " = " << x;


QPen Conductor::conductor_pen = QPen();
QBrush Conductor::conductor_brush = QBrush();

/**
 * @brief Conductor::Conductor
 * @param p1
 * @param p2
 * @param e1
 * @param e2
 * @param dg
 */
Conductor::Conductor(Terminal *p1, Terminal *p2, CustomElement *e1, CustomElement *e2, DataGraph& dg):QObject()
   ,QGraphicsPathItem(0)
   ,terminal1(p1)//Вход
   ,terminal2(p2)//Выход/Вход!
   ,elem1(e1)//Элемент выходной
   ,elem2(e2)//Элемент входной
   ,dgr(dg)
{
   conductor_pen.setColor(Qt::black);
   conductor_pen.setStyle(Qt::SolidLine);
   conductor_pen.setWidthF(1.0);
   conductor_pen.setCosmetic(true);
   conductor_brush.setStyle(Qt::NoBrush);
   setZValue(9);
   terminal1->conductors().append(this);//Запись кондуктора
   terminal2->conductors().append(this);//в список кондукторов терминала//
   connectOutInAStar();
   pointToPath();
}

/**
 * Not used
 */
 /*Conductor::Conductor(Terminal *p1, Terminal *p2, CustomElement *e1, CustomElement *e2):QObject(),QGraphicsPathItem(0),
           terminal1(p1),
           terminal2(p2),
           elem1(e1),
           elem2(e2)

{
    conductor_pen.setColor(Qt::black);
    conductor_pen.setStyle(Qt::SolidLine);
    conductor_pen.setWidthF(1.0);
    conductor_pen.setCosmetic(true);
    conductor_brush.setStyle(Qt::NoBrush);
    setZValue(9);

    connectOutIn();
    segmentsToPath();
}*/


/**
 * @brief Conductor::~Conductor
 */
Conductor::~Conductor()
{
    terminal1->conductors().clear();
    terminal2->conductors().clear();
    deleteSegments();
}

/**
 * @brief Conductor::getElem1
 * @return
 */
CustomElement *Conductor::getElem1() const
{
    return elem1;
}

/**
 * @brief Conductor::getTerminal2
 * @return
 */
Terminal *Conductor::getTerminal2() const
{
    return terminal2;
}

/**
 * @brief Conductor::getTerminal1
 * @return
 */
Terminal *Conductor::getTerminal1() const
{
    return terminal1;
}

/**
 * @brief Conductor::getElem2
 * @return
 */
CustomElement *Conductor::getElem2() const
{
    return elem2;
}

/**
 * @brief Conductor::paint
 * @param qp
 * @param sg
 * @param qw
 */
void Conductor::paint(QPainter *qp, const QStyleOptionGraphicsItem *sg, QWidget *qw)
{
    Q_UNUSED(qw);
    Q_UNUSED(sg);
    qp->save();
    qp->setRenderHint(QPainter::Antialiasing);
    qp->setBrush(Qt::NoBrush);

    QColor conductor_color(Qt::black);
    QPen pen;
    pen.setColor(conductor_color);
    pen.setWidth(1.0);

    qp->setPen(pen);
    qp->drawPath(path());
    qp->restore();
}

/**
 * @brief Conductor::boundingRect
 * @return
 */
QRectF Conductor::boundingRect() const
{
    QRectF br = shape().boundingRect();
    return br.adjusted(-10, -10, 10, 10);
}

/**
 * @brief Conductor::shape
 * @return
 */
QPainterPath Conductor::shape() const
{
    QPainterPathStroker pps;
    pps.setWidth(1);
    QPainterPath shape_(pps.createStroke(path()));

    return shape_;
}

/**
 * @brief Conductor::getDgr
 * @return
 */
DataGraph &Conductor::getDgr() const
{
    return dgr;
}

/**
 * Для типа соединения элементов OUT -> INPUT
 * @brief Conductor::getPoint
 * @param t
 * @return
 */
QPointF Conductor::getPoint(Terminal* t)
{
    if(t->ori==Ulo::East)
        return terminal1->getP2();
    if(t->ori==Ulo::West)
       return terminal2->getP1();
    return QPoint();
}

/**
 * @brief Conductor::connectOutIn
 */
void Conductor::connectOutIn()
{
    if(terminal1->getOri() == Ulo::West && terminal2->getOri() == Ulo::West){
        /** Соединение IUPUT -> INPUT */
        segments << ConductorSegment(terminal1->getP1(), terminal2->getP1());
    }else{
        /** Соединение OUT -> INPUT */
        segments << ConductorSegment(getPoint(terminal1),getPoint(terminal2));
    }
}


/**
 * Занесение точек для рисования линии соединения из А*
 * @brief Conductor::connectOutInAStar
 */
void Conductor::connectOutInAStar()
{
    QPoint p1;
    QPoint p2;

    if(terminal1->getOri() == Ulo::West && terminal2->getOri() == Ulo::West){
        /** Соединение IUPUT -> INPUT */
        p1.setX(terminal1->getP1().x());
        p1.setY(terminal1->getP1().y());
        p2.setX(terminal2->getP1().x());
        p2.setY(terminal2->getP1().y());
    }else{
        /** Соединение OUT -> INPUT */
        p1.setX(static_cast<int>(getPoint(terminal1).x()));
        p1.setY(static_cast<int>(getPoint(terminal1).y()));
        p2.setX(static_cast<int>(getPoint(terminal2).x()));
        p2.setY(static_cast<int>(getPoint(terminal2).y()));
    }
    dgr.setPathFinder(p1, p2, true, true);//Начало вниз//
    for(QPoint p : dgr.pathPoints){//Запись точек пути в сегмент соединения//
        segments << ConductorSegment(p);
    }
}

/**
 * Удаляет сегменты
 * @brief Conductor::deleteSegments
 */
void Conductor::deleteSegments()
{
    segments.clear();
}

/**
 * Строить сегменты
 * @brief Conductor::recreationSegments
 */
void Conductor::recreationSegments()
{
    connectOutInAStar();
    pointToPath();
}

/**
 * @brief Conductor::getSegments
 * @return
 */
QList<ConductorSegment> Conductor::getSegments() const
{
    return segments;
}

/**
 * @brief Conductor::getRefSegments
 * @return
 */
QList<ConductorSegment>& Conductor::getRefSegments()
{
    return segmentsRef;
}

/**
 * QPainterPath рисует из списка точек
 * @brief Conductor::segmentsToPath
 */

void Conductor::segmentsToPath()
{
    QPainterPath path;

    if (segments.isEmpty()){
        QPainterPath();
        setPath(path);
        return;
    }

    for(ConductorSegment cs : segments){
        path.moveTo(cs.firstPoint());
        path.lineTo(cs.secondPoint());
    }
    setPath(path);
}

/**
 * @brief Conductor::pointToPath
 */
void Conductor::pointToPath()
{
    QPainterPath path;

    if (segments.isEmpty()){
        setPath(path);  //Ломает программу Break Down programm!!!!
        //return;
    }

    path.moveTo(segments.first().firstPoint());
    for(int i = 1; i < segments.size(); i++){
        ConductorSegment cs = static_cast<ConductorSegment>(segments[i]);
        path.lineTo(cs.firstPoint());
    }
    setPath(path);
}
