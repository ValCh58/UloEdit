#ifndef GEOMET_H
#define GEOMET_H

#include <QPointF>
#include <QObject>
#include <QLineF>

class Geomet
{
public:
    Geomet();
    ~Geomet();

    Q_ENUMS(OrientPoint)
    enum OrientPoint {LEFT, RIGHT, BEYOND, BEHIND, BETWEEN, ORIGIN, DESTINATION};
                    //Слева справа впереди сзади   между     начало  конец//

    static int classify(QPointF p0, QPointF p1, QPointF p2);
    static int classify(QPoint p0, QPoint p1, QPoint p2);

    Q_ENUMS(Intersect)
    enum Intersect{
                   NOT_INTERSECT=0,//Нет пересечения//
                   INTERSECTION =1,//Пересечение//
                   ON_SEGMENT_D1=2,
                   ON_SEGMENT_D2=3,
                   ON_SEGMENT_D3=4,
                   ON_SEGMENT_D4=5,
                   LINE_ON_LINE =6 //Линия на линии//
                  };

    static int direction(QPoint p0, QPoint p1, QPoint p2);
    static bool onSegment(QPoint p1, QPoint p2, QPoint p3);
    static int segmentsIntersect(QPoint p1, QPoint p2, QPoint p3, QPoint p4);
    static int segmentsIntersect(QLineF line1, QLineF line2);

};

#endif // GEOMET_H
