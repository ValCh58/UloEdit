#ifndef TERMINAL_H
#define TERMINAL_H

#include <QtWidgets>
#include "ulo.h"
#include "ulodata.h"

class Conductor;
class UloData;
class CustomElement;

class Terminal : public QObject
{
    Q_OBJECT
public:
    Terminal(UloData *dat, int num = 0, QObject *parent = nullptr);
    Terminal(int logotg, UloData *dat, int num = 0, QObject *parent = nullptr);
    ~Terminal();
    static const qreal termLen;//Длина терминала//
    static const qreal deltaY;//Приращение следующей позиции терминала по Y//
    static const qreal deltaXText; //Сдвиг текста вправо от начала терминала P1//
    static const qreal deltaYText; //Сдвиг текста по Y//
    static const int textSize; //Размер шрифта//
    int number;//Внутрений номер терминала//
    UloData uloDat;
    bool typeInv;//Вход прямой/инверсный//
    Ulo::Orientation ori;//Ориентация терминала <-:West | East:->//
    QPointF getDesiredPoint();

    // методы управления соединениями, подключенными к терминалу
    Terminal* alignedWithTerminal () const;
    //------------------------------------------------------

    QPointF getP1() const;
    void setP1(const QPointF &value);
    void set_P1(QPointF value);
    void setP1Tg(const QPointF &value);

    QPointF getP2() const;
    void setP2(const QPointF &value);
    void set_P2(QPointF value);
    void setP2Tg(const QPointF &value);

    QString getName() const;
    void setName(const QString &value);

    int getNumber() const;
    void setNumber(int value);

    int getInpTg() const;
    void setInpTg(int value);

    bool operator == (Terminal t){
        return this->getName().indexOf(t.getName() == 0);
    }

    Ulo::Orientation getOri() const;
    QList<Conductor *> &conductors();

    //Дополнительные параметры для универсального элемента//////////////
    int getTypePrimElUni() const;
    void setTypePrimElUni(int value);

    int getNumInputUni() const;
    void setNumInputUni(int value);
    //----------------------------------------------------------------//

    int getKoMult() const;
    void setKoMult(int value);

private:
    //Дополнительные параметры для универсального элемента/////////////////////
    int typePrimElUni;//Логический тип первой части универсального элемента AND, AND-N, OR, OR-N////
    int numInputUni;//Номер входа первичного элемента в универсальный элемент (номер Л.Я.)//
    //-----------------------------------------------------------------------//

    QPointF p1;//Точка слева//
    QPointF p2;//Точка справа//
    QString name;//Надпись над терминалом
    void init();
    void initTg(int logo);
    int inpTg;
    bool getTypeInv();
    CustomElement *cElem;
    int koMult;//Коэф. множитель//
    QList<Conductor *> conductors_;//Список соединителей, подключенных к терминалу
    void setUloDat(UloData* d);
    QString operandToText(QString oprd);
    QString getNameBVR(QString fileName, QString addr);//Получим сокращенное название операции для адреса БВР//
    QString returnNameBVR(QString line, QString addr);//Поиск строки и возврат названия//


signals:

public slots:

};

#endif // TERMINAL_H
