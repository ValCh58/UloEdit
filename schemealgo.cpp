#include "schemealgo.h"

const int   SchemeAlgo::xGrid  = 8;
const int   SchemeAlgo::yGrid  = 8;
const qreal SchemeAlgo::margin = 5.0;
QColor		SchemeAlgo::background_color = Qt::white;

/**
 * @brief SchemeAlgo::SchemeAlgo
 * @param parent
 */
SchemeAlgo::SchemeAlgo(QObject *parent) : QGraphicsScene(parent), draw_grid_ (true), use_border_(true)
{
   setBackgroundBrush(Qt::white);
   conductor_setter_ = new QGraphicsLineItem(nullptr);
   conductor_setter_ -> setZValue(1000000);
   QPen pen(Qt::NoBrush, 1.5, Qt::DashLine);
   pen.setColor(Qt::black);
   conductor_setter_ -> setPen(pen);
   connect(&border_and_titleblock, SIGNAL(borderChanged(QRectF,QRectF)), this, SLOT(adjustSceneRect()));
   adjustSceneRect();
}

/**
 * @brief SchemeAlgo::~SchemeAlgo
 */
SchemeAlgo::~SchemeAlgo(){}

/**
 * @brief SchemeAlgo::snapToGrid
 * Возврат ближайшей точки привязки p
 * @param p Указывается ближайшая точка привязки
 * @return
 */
QPointF SchemeAlgo::snapToGrid(const QPointF &p)
{
    /** Возвращает точку, округленную до ближайшего пикселя */
    if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier))
    {
        int p_x = qRound(p.x());
        int p_y = qRound(p.y());
        return (QPointF(p_x, p_y));
    }

    /** Вернуть точку, привязанную к сетке */
    int p_x = qRound(p.x() / SchemeAlgo::xGrid) * SchemeAlgo::xGrid;
    int p_y = qRound(p.y() / SchemeAlgo::yGrid) * SchemeAlgo::yGrid;
    return (QPointF(p_x, p_y));
}

/**
 * @brief SchemeAlgo::addItem
 * Добавим элемент
 * @param QGraphicsItem *item
 */
void SchemeAlgo::addItem(QGraphicsItem *item)
{
    QGraphicsScene::addItem(item);
}

/**
 * @brief SchemeAlgo::adjustSceneRect
 */
void SchemeAlgo::adjustSceneRect()
{
    QRectF old_rect = sceneRect();
    QRectF rect = border_and_titleblock.borderAndTitleBlockRect().united(itemsBoundingRect());
    setSceneRect(rect);
    update(old_rect.united(sceneRect()));
}

/**
 * @brief SchemeAlgo::drawBackground
 * @param QPainter *p
 * @param const QRectF &r
 */
void SchemeAlgo::drawBackground(QPainter *p, const QRectF &r)
{
    p->save();
    p->setRenderHint(QPainter::Antialiasing, false);
    p->setRenderHint(QPainter::TextAntialiasing, true);
    p->setRenderHint(QPainter::SmoothPixmapTransform, false);
    p->setPen(Qt::NoPen);
    p->setBrush(SchemeAlgo::background_color);
    p->drawRect(r);
    //-----------------------------------------------------------
    QPen pen;
    pen.setColor(Qt::black);
    pen.setCosmetic(true);
    p->setPen(pen);
    p->setBrush(Qt::NoBrush);

    /** Сетка обновляемая внутри бордюра */
    QRectF rect = border_and_titleblock.insideBorderRect().intersected(r);//rect=755.0 x 316.0+267.0+1057.0

    qreal limite_x = rect.x() + rect.width(); //limit_x=1022
    qreal limite_y = rect.y() + rect.height();//limit_y=1373

    int g_x = (int)ceil(rect.x());
    while (g_x % xGrid)
        ++g_x;
    int g_y = (int)ceil(rect.y());
    while (g_y % yGrid)
        ++g_y;

    QPolygon points; //points<p(x,y)>[3666]
    for (int gx = g_x ; gx < limite_x ; gx += xGrid) {
        for (int gy = g_y ; gy < limite_y ; gy += yGrid) {
            points << QPoint(gx, gy);/** Вектор точек(x,y) */
        }
    }

    /** Возвращает ограничивающий прямоугольник */
    QRect qr = points.boundingRect(); //745x305+272+1064; x1=272, y1=1064, x2=1016, y2=1368;


    p->drawPoints(points);

    if (use_border_)
        border_and_titleblock.draw(p);
    //-----------------------------------------------------------
    p->restore();
}

