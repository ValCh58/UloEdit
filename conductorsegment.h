#ifndef CONDUCTORSEGMENT_H
#define CONDUCTORSEGMENT_H

#include <QPointF>
#include "ulo.h"

class ConductorSegment
{

public:
     ConductorSegment(const QPointF &, const QPointF &);
     ConductorSegment(const QPointF &p1);
     virtual ~ConductorSegment();
private:
     //ConductorSegment(const ConductorSegment &);

private:
     QPointF point1;
     QPointF point2;

public:
     QPointF firstPoint() const;
     QPointF secondPoint() const;
     void setFirstPoint(const QPointF &);
     void setSecondPoint(const QPointF &);
     QPointF middle() const;
     bool isHorizontal() const;
     Ulo::ConductorSegmentType type() const;
     qreal length() const;
     bool isPoint() const;
};

#endif // CONDUCTORSEGMENT_H
