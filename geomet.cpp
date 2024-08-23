#include "geomet.h"

Geomet::Geomet()
{

}

Geomet::~Geomet()
{

}


//Определяет положение точки р2 относительно прямой р0 р1//
int Geomet::classify(QPointF p0, QPointF p1, QPointF p2)
{
    QPointF a = p1-p0;
    QPointF b = p2-p0;
    double sa = a.x()*b.y() - b.x()*a.y();

    if(sa > 0.0)
        return LEFT;
    if(sa < 0.0)
        return RIGHT;
    if((a.x()*b.x() < 0.0)||(a.y()*b.y() < 0.0))
        return BEHIND;
    if(a.manhattanLength() < b.manhattanLength())
        return BEYOND;
    if(p0 == p2)
        return ORIGIN;
    if(p1 == p2)
        return DESTINATION;
    return BETWEEN;
}

int Geomet::classify(QPoint p0, QPoint p1, QPoint p2)
{
    QPoint a = p1-p0;
    QPoint b = p2-p0;
    double sa = a.x()*b.y() - b.x()*a.y();

    if(sa > 0.0)
        return LEFT;
    if(sa < 0.0)
        return RIGHT;
    if((a.x()*b.x() < 0.0)||(a.y()*b.y() < 0.0))
        return BEHIND;
    if(a.manhattanLength() < b.manhattanLength())
        return BEYOND;
    if(p0 == p2)
        return ORIGIN;
    if(p1 == p2)
        return DESTINATION;
    return BETWEEN;
}


//Относительное расположение отрезков//////////
int Geomet::direction(QPoint p0, QPoint p1, QPoint p2){
    return(p1.x()- p0.x())*(p2.y()- p0.y())-(p2.x()- p0.x())*(p1.y()- p0.y());
}


//Лежит ли на отрезке точка//
bool Geomet::onSegment(QPoint p1, QPoint p2, QPoint p3){
    bool retVal = false;

    if((std::min(p1.x(), p2.x()) <= p3.x()) && (p3.x() <= std::max(p1.x(), p2.x()))){
       if((std::min(p1.y(), p2.y()) <= p3.y()) && (p3.y() <= std::max(p1.y(), p2.y()))){
          retVal = true;
       }
    }

    return retVal;
}

//Пересекаются ли отрезки/////////////////////////////////////////
int Geomet::segmentsIntersect(QPoint p1, QPoint p2, QPoint p3, QPoint p4){
    Intersect retVal = NOT_INTERSECT;//Нет пересечения//

    int d1=direction(p3, p4, p1);
    int d2=direction(p3, p4, p2);
    int d3=direction(p1, p2, p3);
    int d4=direction(p1, p2, p4);

    if(((d1>0 && d2<0)||(d1<0 && d2>0)) && ((d3>0 && d4<0)||(d3<0 && d4>0))){
       retVal = INTERSECTION; //Пересечение//
    }
    else if((d1==0 && onSegment(p3, p4, p1))&&(d2==0 && onSegment(p3, p4, p2))){
        retVal = LINE_ON_LINE; //Линия на линии//
    }
    else if(d1==0 && onSegment(p3, p4, p1)){
        retVal = ON_SEGMENT_D1; //Линия р3-р4 пересекается линией р1-р2 по левой начальной точке//
    }
    else if(d2==0 && onSegment(p3, p4, p2)){
        retVal = ON_SEGMENT_D2;  //Линия р3-р4 пересекается линией р1-р2 по правой конечной точке//
    }
    else if(d3==0 && onSegment(p1, p2, p3)){
        retVal = ON_SEGMENT_D3; //Линия р1-р2 пересекается линией р3-р4 по левой начальной точке//
    }
    else if(d4==0 && onSegment(p1, p2, p4)){
        retVal = ON_SEGMENT_D4; //Линия р1-р2 пересекается линией р3-р4 по правой конечной точке//
    }

    return retVal;
}

int Geomet::segmentsIntersect(QLineF line1, QLineF line2)
{
    QLine ln1 = line1.toLine();
    QLine ln2 = line2.toLine();

    return segmentsIntersect(ln1.p1(), ln1.p2(), ln2.p1(), ln2.p2());
}


