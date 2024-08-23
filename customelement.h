#ifndef CUSTOMELEMENT_H
#define CUSTOMELEMENT_H

#include <QGraphicsObject>
#include <QPicture>

#include "ulo.h"
#include "datagrahp.h"

class Terminal;


class CustomElement : public QGraphicsObject
{
    Q_OBJECT
public:
    CustomElement(QGraphicsItem * qgi = 0);
    ~CustomElement();

    QPicture drawing;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *options, QWidget *widget=0);
    QList<Terminal*> listTerminals;

    int getCntTermWest() const;
    void setCntTermWest(int value);

    int getCntTermEast() const;
    void setCntTermEast(int value);

    int getTypeEl() const;

    QSizeF getSizeElem() const;
    void setSizeElem(const QSizeF &value);

    QPointF getLocalPos() const;
    void setLocalPos(const QPointF &value);

    QSizeF &getRefElem() const;
    virtual void buildElement(DataGraph &dtg);

    QString getNameEl() const;
    void setNameEl(const QString &value);

    bool operator==(CustomElement c){
        return this->getNameEl().indexOf(c.getNameEl())==0;
    }

    //Указатель на функцию вызывающий - функцию построения элемента//
    //void (*crElem)(QPainter&);
    virtual void makeAnyElement(QPainter& qp);
    bool isInputElem();

    QList<Terminal *> getListTerminals() const;
    void setListTerminals(const QList<Terminal *> &value);
    //void sortListTerminals();//Не используется
    //void adjustTermElement(CustomElement *el);//Перенесено в AndElement
    //void adjustElem3Term(CustomElement *el);//Перенесено в AndElement
    //int  elemTypeAdust(CustomElement *el);//Перенесено в AndElement
    // void swapCoordTerm(Terminal* term1, Terminal* term2);//Перенесено в AndElement
    virtual void swapCoordTerm(Terminal *term1, Terminal *term2, QPainter &qp);
    QPicture getDrawing() const;

    int getCodLF() const;
    void setCodLF(int value);

protected:
    QSize setSize(int wid, int hei);
    QPoint setHotspot(QPoint hs);
    void makeLine(QPointF p1, QPointF p2, QPainter &qp, Qt::GlobalColor color=Qt::GlobalColor::black);
    bool makeRect(qreal x, qreal y, qreal w, qreal h, QPainter &qp);
    bool makeRectRounded(qreal x, qreal y, qreal w, qreal h, QPainter &qp, qreal xRadius, qreal yRadius, Qt::SizeMode mode = Qt::AbsoluteSize);
    void makeCircle(QPointF p, QPainter &qp, Qt::GlobalColor color=Qt::GlobalColor::black);
    void makeText(qreal x, qreal y, QPainter &qp, QString text, int size, Qt::GlobalColor color=Qt::GlobalColor::black);
    QString operandToText(QString oprd);
    QString nameEl;
    int codLF;//Код логической функции//
    QString logoType;
    int cntTermWest;
    int cntTermEast;
    int typeEl;//Тип элемента//
    QPointF localPos;//Позиция размещения элемента на схеме//
    QSizeF  sizeElem;//Размер элемента//
    QSizeF  &refElem=sizeElem;

private:
    QSize   dimensions;
    QPoint  hotspot_coord;

    //int getIndex(Terminal *t); Перенесено в AndElement



};



#endif // CUSTOMELEMENT_H
