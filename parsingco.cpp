#include "parsingco.h"
#include "geomet.h"
#include "schemealgo.h"
#include "ulo.h"


ParsingCo::ParsingCo(QList<Conductor *> &co):conductors(co)
{
    cntCo = conductors.size();
}

ParsingCo::~ParsingCo()
{

}

//Коррекция терминалов всех элементов////////////////////////////////////////////////////////
void ParsingCo::mainParsing()
{
    //Проcмотрим все соединения//
    for(int i=0; i<cntCo; i++){
        Conductor* cond = conductors.at(i);
        if(cond->getElem1() && cond->getElem2()){
           item_Parsing(cond, cond->getElem1(), cond->getElem2());//Разбор вых элем 1 и вх элем 2
        }
    }
}//----------------------------------------------------------------------------------------//


//Коррекция терминалов элемента в случае их пересечения линиуй от элемента///////////////////
void ParsingCo::item_Parsing(Conductor *cond, CustomElement* elem1, CustomElement* elem2)
{
    int factor;

    Terminal* term1 = cond->getTerminal1();//Вход элемента
    Terminal* term2 = cond->getTerminal2();//Выход элемента
    QList<ConductorSegment>& seg = cond->getRefSegments();//Сегменты линии соединения

    factor = getFactor(elem2, term2, seg);
    correctCo(seg, (SchemeAlgo::xGrid*factor), term2);
    cond->pointToPath();
    factor = getFactor(elem1, term1, seg);
    correctCo(seg, (SchemeAlgo::xGrid*factor), term1);
    cond->pointToPath();
}//----------------------------------------------------------------------------------------//

//Получение количества пересечений терминалов линией от элемента/////////////////////////////
int ParsingCo::getFactor(CustomElement* el, Terminal* term, QList<ConductorSegment>& seg)
{
    int factor = 0;//Множитель пересечений

    //Просмотрим все терминалы элемента///////
    for(Terminal* t : el->getListTerminals()){
       if((term->getOri() == t->getOri())&&(term->number != t->number) && isLineIntersection(seg, t)){
           factor++;
       }
    }
    return factor;
}//----------------------------------------------------------------------------------------//


//Определение местоположения точки относительно линии////////////////////////////////////////
bool ParsingCo::isLineIntersection(QList<ConductorSegment>& s, Terminal* t)
{
    bool ret = false;
    Ulo::Orientation orient = t->getOri();
    QLineF line = createLine(s, orient);
    QPointF pTemp;

    if(t->getOri() == Ulo::Orientation::West){//Вход
       pTemp = t->getP1();
    }
    else if(Ulo::East){//Выход
       pTemp = t->getP2();
    }

    if(line.p2().x() > 0){
       int range = Geomet::classify(line.p1(), line.p2(), pTemp);
       if(range == Geomet::BETWEEN || range == Geomet::ORIGIN || range == Geomet::DESTINATION){
          ret = true;//Точка pTemp находится на отрезке между начальной P1 и P2 конечной//
       }
    }

    return ret;
}//----------------------------------------------------------------------------------------//



//Получение горизонтальной линии/////////////////////////////////////////////////////////////
QLineF ParsingCo::createLine(QList<ConductorSegment> s, Ulo::Orientation ori)
{
    QLineF ln;
    int size = s.size();


    if(ori == Ulo::West){//Вход элемента
       ln.setP1(s.at(0).firstPoint());//Запишем первую точку линии//
       for(int i = 1; i < size; i++){//Идем по списку точек соединительной линии//
           QPointF p = s.at(i).firstPoint();//построеной алгоритмом А*//
           if(p.x() == ln.p1().x()){//Если значения Х равны - линия продолжается в горизонте//
              continue;
           }
           else{
               if(i > 1){
                  p = s.at(i-1).firstPoint();
                  ln.setP2(p); //Запишем вторую точку линии//
               }
               break;
           }
       }
    }
    else if(ori == Ulo::East){//Выход элемента
        ln.setP1(s.at(size-1).firstPoint());//Запишем первую точку линии//
        int x = 0;
        for(int i = size-1; i >= 0; i--){
            QPointF p = s.at(i).firstPoint();
            if(p.x() == ln.p1().x()){
               x++;
               continue;
            }
            else{
                if(x > 1){
                   p = s.at(i+1).firstPoint();
                   ln.setP2(p); //Запишем вторую точку линии//
                }
                break;
            }
        }
    }
    return ln;
}//----------------------------------------------------------------------------------------//

//Получение линиии от выхода элемента////////////////////////////////////////////////////////
QLineF ParsingCo::getLineFromEast(QList<ConductorSegment> s)
{
    QLineF ln;
    int size = s.size();
    Ulo::Direction direct = getDirectionLine(s, Ulo::East);//Получим направление линии//
    ln.setP1(s.at(size-1).firstPoint());//Запишем первую точку линии//
    int XorY = 0;

    for(int i = size-1; i >= 0; i--){
        QPointF p = s.at(i).firstPoint();
        //Идем по списку точек соединительной линии//
        //Если значения Х или У равны - линия продолжается в направлении direct//
        if(i == 0 && XorY > 1){//Конец прямой линии//
           p = s.at(i+1).firstPoint();
           ln.setP2(p); //Запишем вторую точку линии//
        }
        else if(direct == Ulo::Direction::RIGHT && p.y() == ln.p1().y()){
           XorY++;
           continue;
        }
        else if((direct == Ulo::Direction::UP || direct == Ulo::Direction::DOWN) && p.x() == ln.p1().x()){
            XorY++;
            continue;
        }
        else{
            if(XorY > 1){
               p = s.at(i+1).firstPoint();
               ln.setP2(p); //Запишем вторую точку линии//
            }
            break;
        }
    }
    return ln;

}//----------------------------------------------------------------------------------------//

//Получение линиии от входа элемента/////////////////////////////////////////////////////////
QLineF ParsingCo::getLineFromWest(QList<ConductorSegment> s)
{
    QLineF ln;
    int size = s.size();
    Ulo::Direction direct = getDirectionLine(s, Ulo::West);//Получим направление линии//
    ln.setP1(s.at(size-1).firstPoint());//Запишем первую точку линии//

    for(int i = 1; i < size; i++){//Идем по списку точек соединительной линии//
        QPointF p = s.at(i).firstPoint();//построеной алгоритмом А*//
        //Если значения Х или У равны - линия продолжается в напрпавлении direct//
        if(i == size-1){
           p = s.at(i-1).firstPoint();
           ln.setP2(p); //Запишем вторую точку линии//
        }
        else if(direct == Ulo::Direction::LEFT && p.y() == ln.p1().y()){
           continue;
        }
        else if((direct == Ulo::Direction::UP || direct == Ulo::Direction::DOWN) && p.x() == ln.p1().x()){
            continue;
         }
        else{
            if(i > 1){
               p = s.at(i-1).firstPoint();
               ln.setP2(p); //Запишем вторую точку линии//
            }
            break;
        }
    }
    return ln;
}//----------------------------------------------------------------------------------------//

//Создадим линию верт/гориз//////////////////////////////////////////////////////////////////
QLineF ParsingCo::createLineVorH(QList<ConductorSegment> s, Ulo::Orientation ori)
{
    QLineF ln;

    if(ori == Ulo::East)//Выход
     ln = getLineFromEast(s);
    else if(ori == Ulo::West)//Вход
     ln = getLineFromWest(s);

    return ln;
}//----------------------------------------------------------------------------------------//

//Определим направление линии////////////////////////////////////////////////////////////////
Ulo::Direction ParsingCo::getDirectionLine(QList<ConductorSegment> s, Ulo::Orientation ori)
{
    QPointF p1, p2;

    if(ori == Ulo::West && s.size() > 1){//Вход
        p1 = s.at(0).firstPoint();
        p2 = s.at(1).firstPoint();
    }
    else if(ori == Ulo::East && s.size() > 1){//Выход
        p1 = s.at(s.size()-1).firstPoint();
        p2 = s.at(s.size()-2).firstPoint();
    }

    if(p1.x() == p2.x() && p1.y() > p2.y())//Линия вверх//
       return Ulo::UP;
    else if(p1.x() == p2.x() && p1.y() < p2.y())//Линия вниз//
       return Ulo::DOWN;
    else if(p1.y() == p2.y() && p1.x() > p2.x())//Линия на лево//
       return Ulo::LEFT;
    else if(p1.y() == p2.y() && p1.x() < p2.x())//Линия на право//
       return Ulo::RIGHT;

    return Ulo::LINEERR;
}//----------------------------------------------------------------------------------------//

//Смещение линии соединения с терминалом для исключения пересечения//////////////////////////
void ParsingCo::correctCo(QList<ConductorSegment>& s, int shift, Terminal *t)
{   //Сохраним точку соединения с терминалом перед коррекцией//
    QPointF endP;
    int size = s.size();
    Ulo::Orientation ori = t->getOri();

    if(ori == Ulo::Orientation::West){//Вход элемента//
        endP = s.at(0).firstPoint();
        for(int i = 0; i < size; i++){
            QPointF p = s.at(i).firstPoint();
            if(p.x() == endP.x()){
               p.setX(p.x()-shift);
               s.replace(i, p);
            }
        }
        s.prepend(endP);
    }
    else if(ori == Ulo::Orientation::East){//Выход элемента//
        endP = s.at(size-1).firstPoint();
        for(int i = size-1; i >= 0; i--){
            QPointF p = s.at(i).firstPoint();
            if(p.x() == endP.x()){
               p.setX(p.x()+shift);
               s.replace(i, p);
            }
        }
        s.append(endP);
    }
}//----------------------------------------------------------------------------------------//



