#include "vertex.h"

Vertex::Vertex(QPoint p)
{
  point = p;
}

Vertex::Vertex(int x, int y)
{
  point.setX(x);
  point.setY(y);
}

Vertex::~Vertex()
{

}
int Vertex::getPassability() const
{
    return passability;
}

void Vertex::setPassability(int value)
{
    passability = value;
}
int Vertex::getBusy() const
{
    return busy;
}

void Vertex::setBusy(int value)
{
    busy = value;
}
QPoint Vertex::getPoint() const
{
    return point;
}

void Vertex::setPoint(const QPoint &value)
{
    point = value;
}




