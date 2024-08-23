#include "conductorsegment.h"


/**
    @param p1 Точка
    @param p2
    @param cs1 Предыдущий сегмент
    @param cs2 Следующий
*/
ConductorSegment::ConductorSegment(const QPointF &p1, const QPointF &p2):point1(p1), point2(p2)
{}

ConductorSegment::ConductorSegment(const QPointF &p1):point1(p1)
{}

ConductorSegment::~ConductorSegment()
{

}


/**
    @return первая точка отрезка
*/
QPointF ConductorSegment::firstPoint() const {
    return(point1);
}

/**
    @return вторая точка отрезка
*/
QPointF ConductorSegment::secondPoint() const {
    return(point2);
}

/**
     Изменение положения первой точки отрезка
     @param р новое положение первой точки
*/
void ConductorSegment::setFirstPoint(const QPointF &p) {
    point1 = p;
}

/**
     Изменение положения второй точки отрезка
     @param р новое положение второй точки
*/
void ConductorSegment::setSecondPoint(const QPointF &p) {
    point2 = p;
}



/**
    @return Центр прямоугольника
*/
QPointF ConductorSegment::middle() const {
    return(
        QPointF(
            (point1.x()+ point2.x()) / 2.0,
            (point1.y()+ point2.y()) / 2.0
        )
                );
}

bool ConductorSegment::isHorizontal() const
{
    return(secondPoint().x() > firstPoint().x());
}

/**
    @return Длина проводника
*/
qreal ConductorSegment::length() const {
    if (isHorizontal()) {
        return(secondPoint().x() - firstPoint().x());
    } else {
        return(secondPoint().y() - firstPoint().y());
    }
}

// @return QET::Horizontal если сегмент является горизонтальным, QET::Vertical вертикальным//
Ulo::ConductorSegmentType ConductorSegment::type() const {
    return(isHorizontal() ? Ulo::Horizontal : Ulo::Vertical);
}

// @return true если две точки, составляющие сегмент равны
bool ConductorSegment::isPoint() const {
    return(point1 == point2);
}
