#ifndef VERTEX_H
#define VERTEX_H
#include <QPoint>

class Vertex
{
public:
    Vertex(QPoint p);
    Vertex(int x, int y);
    ~Vertex();


    int getPassability() const;
    void setPassability(int value);

    int getBusy() const;
    void setBusy(int value);

    QPoint getPoint() const;
    void setPoint(const QPoint &value);

protected:
    QPoint point;//Координаты точки, через которую проходит линия//
    int passability = 0;
    int busy = 0;
};

#endif // VERTEX_H
