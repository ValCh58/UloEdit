#ifndef PARSINGCO_H
#define PARSINGCO_H

#include <qlist.h>
#include <qlineF>
#include "datagrahp.h"
#include "conductor.h"
#include "customelement.h"
#include "conductorsegment.h"
#include "terminal.h"

//class Conductor;

class ParsingCo
{
public:
    ParsingCo(QList<Conductor *> &co);
    ~ParsingCo();

    void mainParsing();
    QLineF createLine(QList<ConductorSegment> s, Ulo::Orientation ori);
    static QLineF createLineVorH(QList<ConductorSegment> s, Ulo::Orientation ori);
    static Ulo::Direction getDirectionLine(QList<ConductorSegment> s, Ulo::Orientation ori);
    static QLineF getLineFromEast(QList<ConductorSegment> s);
    static QLineF getLineFromWest(QList<ConductorSegment> s);

private:
    void item_Parsing(Conductor *cond, CustomElement *elem1, CustomElement *elem2);
    int getFactor(CustomElement *el2, Terminal *term, QList<ConductorSegment> &seg);
    QList<Conductor*>& conductors;
    bool isLineIntersection(QList<ConductorSegment> &s, Terminal* t);

    void correctCo(QList<ConductorSegment> &s, int shift, Terminal* t);

    int cntCo;

};

#endif // PARSINGCO_H
