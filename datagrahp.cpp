
#include "datagrahp.h"
#include "conductor.h"


DataGraph::DataGraph():QObject()
{
   Row = mapWidth;
   Col = mapHeight;
   cntPoints = setGraphPoint();

}

DataGraph::~DataGraph()
{

}

//Установить путь для линии/////////////////////////////////////////////////////////////
void DataGraph::setPathFinder(QPoint p1, QPoint p2, bool orto, bool updown)
{
    int idx1 = getIndex(p1);//1435
    int idx2 = getIndex(p2);//1313
    int y1 = getArrY(idx1); //9
    int x1 = getArrX(idx1, y1);//22
    int y2 = getArrY(idx2);//8
    int x2 = getArrX(idx2, y2);//57
    getPath(x1, y1, x2, y2, orto, updown);//Ok
    wayPoints();
}//Установить путь для линии////////////////////////////////////////////////////////////


//Строим вершины графа//////////////////////////////////////////////////////////////////
int DataGraph::setGraphPoint()
{
    int keyMap =0;
    QRectF rect = btBlock.insideBorderRect();

    qreal limite_x = rect.x() + rect.width();
    qreal limite_y = rect.y() + rect.height();

    int g_x = static_cast<int>(ceil(rect.x()));
    while (g_x % SchemeAlgo::xGrid)
        ++g_x;
    int g_y = static_cast<int>(ceil(rect.y()));
    while (g_y % SchemeAlgo::yGrid)
        ++g_y;

    for(int gy = g_y ; gy < limite_y ; gy += SchemeAlgo::yGrid) {
         for(int gx = g_x ; gx < limite_x ; gx += SchemeAlgo::xGrid){
            //points << QPoint(gx, gy);//Вектор точек(x,y) для построения графа//
            //keyMap++;
            vertex.insert(keyMap++, QPoint(gx, gy));//Координаты точек схемы//
        }
    }

    //int row = rect.height()/SchemeAlgo::yGrid;
    //int col = rect.width()/SchemeAlgo::xGrid;

    return vertex.size();
}//Строим вершины графа/////////////////////////////////////////////////////////////////

//Индекс 1д массива/////////////////////////////////////////////////////////////////////
int DataGraph::getIndex(QPoint p)
{
    int retIndex = NOTINDEX;

    QMap<int, Vertex>::ConstIterator iterator = vertex.constBegin();
    while(iterator != vertex.constEnd()){
        //qDebug()<<((Vertex)iterator.value()).getPoint();
        if((static_cast<Vertex>(iterator.value()).getPoint()) == p){
           retIndex = iterator.key();
           break;
        }
        iterator++;
    }

    return retIndex;
}//Индекс 1д массива/////////////////////////////////////////////////////////////////////

//Индекс Х 2д массива////////////////////////////////////////////////////////////////////
int DataGraph::getArrX(int idx, int y)
{
    return abs(idx - (y * mapWidth));
}//Индекс Х 2д массива///////////////////////////////////////////////////////////////////

//Индекс У 2д массива////////////////////////////////////////////////////////////////////
int DataGraph::getArrY(int idx)
{
    return idx/mapWidth;
}//Индекс У 2д массива///////////////////////////////////////////////////////////////////

//Преобразуем индекс из 2д массива для 1д массива////////////////////////////////////////
int DataGraph::getIdxFrom2dArr(int x, int y)
{
    return (y * mapWidth + x);
}//Преобразуем индекс из 2д массива для 1д массива///////////////////////////////////////

//Соотнесем точки пути с ключами карты///////////////////////////////////////////////////
void DataGraph::wayPoints()
{
    pathPoints.clear();
    foreach(QPoint p, listPath){  //Пустой listPath !!!!!!   30-10-2024 16:20 !!! Проверить почему listPath пустой.
        int idx = getIdxFrom2dArr(p.x(), p.y());
        qDebug()<<"listPath "<<"X="<<p.x()<<"Y="<<p.y();
        qDebug()<<"wayPoints idx"<<idx;
        pathPoints.append(findPointMap(idx));
    }
}//Соотнесем точки пути с ключами карты/////////////////////////////////////////////////

//Поиск точки ху в мапе по ключу////////////////////////////////////////////////////////
QPoint DataGraph::findPointMap(int ind)
{
    QMap<int, Vertex>::const_iterator iter = vertex.find(ind);
    QPoint pxy;

    while(iter != vertex.end() && iter.key() == ind){
          pxy = (static_cast<Vertex>(iter.value()).getPoint());
          //qDebug()<<"Px="<<pxy.x(); qDebug()<<"Py="<<pxy.y();
        iter++;
    }
    return pxy;
}//Поиск точки ху в мапе по ключу///////////////////////////////////////////////////////

//Установить непроходимые вершины///////////////////////////////////////////////////////
void DataGraph::setWalkability(QPoint p, QSize s)
{
    int x32 = SchemeAlgo::xGrid*4, y16 = SchemeAlgo::yGrid;//*2;

    setWidthWalk(p.x()-x32, p.y()-y16, s.width()+(x32*2));
    setWidthWalk(p.x()-x32, p.y()+s.height(), s.width()+(x32*2));
    setHightWalk(p.x()-x32, p.y()-y16, s.height()+y16);
    setHightWalk(p.x()+s.width()+x32, p.y()-y16, s.height()+y16);
}//Установить непроходимые вершины//////////////////////////////////////////////////////

//Запись в 2д массив непроходимых вершин////////////////////////////////////////////////
void DataGraph::setWalkabilityArray(QPolygon &po)
{
    int idx=0;
    for(Vertex v: vertex) {
       if(v.getPassability() == 1){
          int y = getArrY(idx);
          int x = getArrX(idx, y);
          walkability[x][y] = 1;
          po << v.getPoint();//Только для отображения непроходимых вершин!!!//
       }
       idx++;
    }
}//Запись в 2д массив непроходимых вершин///////////////////////////////////////////////

//Перевернуть линию для элемента с обратной связью?/////////////////////////////////////
bool DataGraph::isReverseShape(Conductor *co)
{
    if(co->getElem1() == co->getElem2()){
       if(co->getTerminal1()->getP2().y() > co->getTerminal2()->getP1().y()){
          if(co->getTerminal1() == co->getTerminal2()){
             return true;
          }
       }
    }
    return false;
}//Перевернуть линию для элемента с обратной связью/////////////////////////////////////


//Ширина непроходимости/////////////////////////////////////////////////////////////////
void DataGraph::setWidthWalk(const int x, const int y, const int width)
{
    int i = x, deltaX = x+width;

    while( i <= deltaX){
        QPoint p(i, y);
        setPointWalk(p, 1);
        i+=SchemeAlgo::xGrid;
    }
}//Ширина непроходимости////////////////////////////////////////////////////////////////

//Установка динамической непроходимости X //////////////////////////////////////////////
void DataGraph::setTempWalkX(int x1, int x2, int y, int val)
{
    int i = x1, deltaX = x2;

    while( i <= deltaX){
        QPoint p(i, y);
        setPointWalkExt(p, val);
        i+=SchemeAlgo::xGrid;
    }
}//Установка динамической непроходимости X //////////////////////////////////////////////

//Установка динамической непроходимости Y ///////////////////////////////////////////////
void DataGraph::setTempWalkY(int y1, int y2, int x, int val)
{
    int i = y1, deltaY = y2;

    while( i <= deltaY){
        QPoint p(x, i);
        setPointWalkExt(p, val);
        i+=SchemeAlgo::yGrid;
    }
}//Установка динамической непроходимости Y ///////////////////////////////////////////////

//Высота непроходимости/////////////////////////////////////////////////////////////////
void DataGraph::setHightWalk(int x, int y, int hight)
{
    int i = y, deltaY = y+hight;

    while( i <= deltaY){
        QPoint p(x, i);
        setPointWalk(p, 1);
        i+=SchemeAlgo::yGrid;
    }
}//Высота непроходимости////////////////////////////////////////////////////////////////

//Установить точку непроходимости///////////////////////////////////////////////////////
void DataGraph::setPointWalk(QPoint point, int val)
{
    QMap<int, Vertex>::Iterator iterator = vertex.begin();
    while(iterator != vertex.end()){
        if((static_cast<Vertex>(iterator.value()).getPoint()) == point){
           Vertex &v = iterator.value();
           v.setPassability(val);
           //qDebug()<<((Vertex)iterator.value()).getPoint();
           break;
        }
        iterator++;
    }
}//Установить точку непроходимости//////////////////////////////////////////////////////

//Установить точку непроходимости с записью в массив непроходимости/////////////////////
void DataGraph::setPointWalkExt(QPoint point, int val)
{
    int ind=0;

    QMap<int, Vertex>::Iterator iterator = vertex.begin();
    while(iterator != vertex.end()){
        if((static_cast<Vertex>(iterator.value()).getPoint()) == point){
           Vertex &v = iterator.value();
           v.setPassability(val);
           int y = getArrY(ind);
           int x = getArrX(ind, y);
           walkability[x][y] = val;
           break;
        }
        iterator++;
        ind++;
    }
}//Установить точку непроходимости с записью в массив непроходимости////////////////////

void DataGraph::makeArray(int row, int col)
{
    int wArr[row][col];
    int cnt = 1, cntList = 0;

    for(int i=0; i<col; i++){
        for(int j=0; j<row; j++){
            wArr[j][i] = cnt++;
        }
    }

    for(int i=0; i<col; i++){
        for(int j=0; j<row; j++){
            cntList++;//Ключ map//
            QList<int> list;
            if(j-1 >= 0){
                list.append(wArr[j-1][i]);
            }
            if(j+1 < row){
                list.append(wArr[j+1][i]);
            }
            if(i-1 >= 0){
                list.append(wArr[j][i-1]);
            }
            if(i+1 < col){
                list.append(wArr[j][i+1]);
            }
            mapGraph.insert(cntList, list);
        }
    }

}



/**Поиск пути для проведения линии
 * brief DataGraph::FindPath
 * param startingX Точка Ах
 * param startingY Точка Ау
 * param targetX   Цель х
 * param targetY   Цель у
 * param orto      Исключить поиск по диагонали true
 * param upDown    Начать поиск вертикально вверх/вниз true-вниз
 * return
*/
int DataGraph::FindPath(int startingX, int startingY, int targetX, int targetY, bool orto, bool upDown)
{
    int pathfinderID=1;    //Отладка 30-10-2024 шаг 1 завершен

    int onOpenList=0, parentXval=0, parentYval=0,
    xa=0, yb=0, m=0, u=0, v=0, temp=0, corner=0, numberOfOpenListItems=0,
    addedGCost=0, tempGcost = 0, path = 0,
    tempx, pathX, pathY, cellPosition,
    newOpenListItemID=0;

    //walkability[4][8]=1;//Установка элемента стены//

//1.Преобразование координат местоположения (в пикселях) в координаты в массиве.
    int startX = startingX;// /tileSize;
    int startY = startingY;// /tileSize;
    //targetX = targetX/tileSize;
    //targetY = targetY/tileSize;

//2.Быстрые проверки пути: при обстоятельствах недостижимости цели, никакой путь не должен быть сгенерированным
    //Если начальное местоположение и цель находятся в одном местоположении
    if (startX == targetX && startY == targetY && pathLocation[pathfinderID] > 0)
        return found;
    if (startX == targetX && startY == targetY && pathLocation[pathfinderID] == 0)
        return nonexistent;

    //Если целевая точка недостижима, вернем его как несуществующий путь
    if (walkability[targetX][targetY] == unwalkable)
        goto noPath;

//3.Сбросим некоторые переменные, которые необходимо очистить

    if (onClosedList > 1000000) //переустановка списка время от времени
    {
        for (int x = 0; x < mapWidth; x++) {
            for (int y = 0; y < mapHeight; y++)
                whichList [x][y] = 0;
        }
        onClosedList = 10;
    }
    //whichList [mapWidth+1][mapHeight+1] = {0};
    //onClosedList = 10;

    onClosedList = onClosedList+2; //Изменение значений onOpenList и onClosed list выполняется быстрее, чем redimming массива whichList()
    onOpenList = onClosedList-1;
    //Хранит длину найденного пути для объекта
    pathLength [pathfinderID] = notStarted;//i.e, = 0
    //Сохраняет текущее положение вдоль выбранного пути для объекта
    pathLocation [pathfinderID] = notStarted;//i.e, = 0
    Gcost[startX][startY] = 0; //Сбросить значение начального квадрата в G до 0
//4.Добавьте начальное местоположение (точку А) в открытый список проверяемых квадратов.
    numberOfOpenListItems = 1;//Кол. элементов открытого списка//
    //Mассив cодержащий идентификаторы открытых элементов списка
    openList[1] = 1;//Назначить его как верхний (и только в данный момент) элемент в открытом списке, который поддерживается как двоичная куча
    //Массивы сохраняют x и у -позиции элемента в открытом списке
    openX[1] = startX ; openY[1] = startY;//Задание начальной точки пути А!!!//

//5.Цикл поиска пути. Пока путь не будет найден или будет считаться несуществующим.
    do
    {

//6.Если открытый список не пуст, извлеките первую ячейку из списка.
   // Это самая низкая ячейка стоимости F в открытом списке.
    if (numberOfOpenListItems != 0)
    {

//7.Вытаскивать первый элемент из открытого списка
    parentXval = openX[openList[1]];
    parentYval = openY[openList[1]]; //Запись координаты ячейки элемента
    whichList[parentXval][parentYval] = onClosedList;//Добавление элемента (точки А) в закрытый список

   //Открытый список = бинарная куча: удалить этот элемент из открытого списка, который поддерживается как двоичная куча
    numberOfOpenListItems = numberOfOpenListItems - 1;//Сократить количество элементов открытого списка на 1

//Удалите верхний элемент в двоичной куче и измените порядок кучи, при этом наименьший элемент затрат F поднимается вверх.
    openList[1] = openList[numberOfOpenListItems+1];//Переместите последний элемент в куче до слота № 1
    v = 1;

    //Повторяйте, пока новый элемент в слоте № 1 не опустится до своего правильного места в куче
    do
    {
    u = v;
    if (2*u+1 <= numberOfOpenListItems) //Если оба потомка существуют
    {
        //Проверьте, что стоимость F родителя больше, чем у каждого дочернего элемента. Выбираем младший из двух дочерних.
       if(upDown){//Путь строится вниз!!!
           if (Fcost[openList[u]] >= Fcost[openList[2*u]])
               v = 2*u;
           if (Fcost[openList[v]] >= Fcost[openList[2*u+1]])
            v = 2*u+1;
       }else{//Путь строится вверх!!!
           if (Fcost[openList[v]] >= Fcost[openList[2*u+1]])
               v = 2*u+1;
           if (Fcost[openList[u]] >= Fcost[openList[2*u]])
               v = 2*u;
       }
    }
    else
    {
        if (2*u <= numberOfOpenListItems) //Если существует только дочерний # 1
        {

            //Проверьте, что стоимость F родителя больше, чем дочернего # 1.
               if (Fcost[openList[u]] >= Fcost[openList[2*u]])
                   v = 2*u;
        }
    }

    if (u != v) //Если родительский F является > одного из его потомков, замените их
    {
        temp = openList[u];
        openList[u] = openList[v];
        openList[v] = temp;
    }
    else
        break;

    }
    while (/*!KeyDown(27)*/1);//reorder the binary heap


//7.Проверьте соседние квадраты. (Его потомки - эти дочерние пути
    // похожи, концептуально, на двоичную кучу потомков, упомянутых выше
    //, но не путайте их. Они разные. Добавьте эти смежные дочерние квадраты в открытый список
    // для последующего рассмотрения, если это необходимо (см. Различные операторы if ниже).
    for (yb = parentYval-1; yb <= parentYval+1; yb++){//Обход соседних клеток Y
     for (xa = parentXval-1; xa <= parentXval+1; xa++){//                     X
         if(orto && (((xa == parentXval-1)&&(yb == parentYval-1)) ||    //Обходим ортогонально!!! orto = true//
              ((xa == parentXval+1)&&(yb == parentYval-1)) ||
                ((xa == parentXval-1)&&(yb == parentYval+1)) ||
                  ((xa == parentXval+1)&&(yb == parentYval+1))))
                  continue;

    //Если не с карты (сначала сделайте это, чтобы избежать ошибок массива вне границ)
      if (xa != -1 && yb != -1 && xa != mapWidth && yb != mapHeight){

       //Если это еще не включено в закрытый список (элементы в закрытом списке
          // уже учтены и теперь могут быть проигнорированы).
       if (whichList[xa][yb] != onClosedList) {//Вершина не в закрытом списке?//

        if (walkability [xa][yb] != unwalkable) {//Вершина проходима?//

    //Не разрезайте углы
    corner = walkable;
         /*if (xa == parentXval-1)
         {
          if (yb == parentYval-1)
          {
            if (walkability[parentXval-1][parentYval] == unwalkable
                || walkability[parentXval][parentYval-1] == unwalkable) \
                corner = unwalkable;
          }
          else if (yb == parentYval+1)
          {
            if (walkability[parentXval][parentYval+1] == unwalkable
                || walkability[parentXval-1][parentYval] == unwalkable)
                corner = unwalkable;
          }
         }
         else if (xa == parentXval+1)
         {
          if (yb == parentYval-1)
          {
            if (walkability[parentXval][parentYval-1] == unwalkable
                || walkability[parentXval+1][parentYval] == unwalkable)
                corner = unwalkable;
          }
          else if (yb == parentYval+1)
          {
            if (walkability[parentXval+1][parentYval] == unwalkable
                || walkability[parentXval][parentYval+1] == unwalkable)
                corner = unwalkable;
          }
         }*/
         if (corner == walkable) {

          //Если этого еще нет в открытом списке, добавьте его в открытый список
          if (whichList[xa][yb] != onOpenList)
          {
           //Создать новый элемент списка в двоичной куче
           newOpenListItemID = newOpenListItemID + 1; //each new item has a unique ID #
           m = numberOfOpenListItems+1;
           openList[m] = newOpenListItemID;//place the new open list item (actually, its ID#) at the bottom of the heap
           openX[newOpenListItemID] = xa;
           openY[newOpenListItemID] = yb;//record the x and y coordinates of the new item

           //Вычислите стоимость G
           if (abs(xa-parentXval) == 1 && abs(yb-parentYval) == 1)
               addedGCost = 14;//Стоимость перехода на диагональные квадраты
           else
               addedGCost = 10;//Стоимость перехода на недиагональные квадраты
           Gcost[xa][yb] = Gcost[parentXval][parentYval] + addedGCost;

           //Вычислите издержки H  F и родителя
           Hcost[openList[m]] = 10*(abs(xa - targetX) + abs(yb - targetY));//Эвристика//
           Fcost[openList[m]] = Gcost[xa][yb] + Hcost[openList[m]];//Общая стоимость//
           parentX[xa][yb] = parentXval ; parentY[xa][yb] = parentYval;

           // Перемещаем новый элемент списка в нужное место в двоичной куче.
           // Начиная с нижней части, последовательно сравниваем с родительскими элементами,
           // сворачиваем по мере необходимости до тех пор, пока элемент не найдет свое место в куче
           // или пузырьках до самого верха (если он имеет самую низкую стоимость F).
           while (m != 1) // Пока элемент не всплыл на верх (m = 1)
           {
            // Проверяем, является ли стоимость F потомка < стоимости F родителя. Если да, заменим их.
            if (Fcost[openList[m]] <= Fcost[openList[m/2]])
            {
                temp = openList[m/2];
                openList[m/2] = openList[m];
                openList[m] = temp;
                m = m/2;
            }
            else
               break;
           }
           numberOfOpenListItems = numberOfOpenListItems+1;//Добавьте 1 к числу элементов в куче

           //Измените whichList, чтобы показать, что новый элемент находится в открытом списке.
           whichList[xa][yb] = onOpenList;
          }

//8.Если соседняя ячейка уже находится в открытом списке, проверьте, не является ли этот
    // путь к этой ячейке из начального местоположения лучшим.
    // Если это так, измените родительский элемент ячейки и ее стоимость G и F.
    else //If whichList(a,b) = onOpenList
    {

        //Вычислите стоимость G этого возможно нового пути
        if (abs(xa-parentXval) == 1 && abs(yb-parentYval) == 1)
            addedGCost = 14;//Стоимость перехода на диагональные плитки
        else
            addedGCost = 10;//Стоимость перехода на ортогональные плитки
        tempGcost = Gcost[parentXval][parentYval] + addedGCost;

        // Если этот путь короче (стоимость G меньше), измените
        // родительскую ячейку, стоимость G и стоимость F.
        if (tempGcost < Gcost[xa][yb]) //Если стоимость G меньше,
        {
            parentX[xa][yb] = parentXval; //изменить родительский квадрат.
            parentY[xa][yb] = parentYval;
            Gcost[xa][yb] = tempGcost;//Изменить стоимость G

             // Поскольку изменение цены G также изменяет стоимость F, если
             // элемент находится в открытом списке, нам нужно изменить
             // стоимость записи элемента F и ее позицию в открытом списке, чтобы быть
             // уверенным, что мы поддерживаем Правильно упорядоченный открытый список.
            for (int x = 1; x <= numberOfOpenListItems; x++) // ищем элемент в куче
            {
            if (openX[openList[x]] == xa && openY[openList[x]] == yb) //элемент найден
            {
                Fcost[openList[x]] = Gcost[xa][yb] + Hcost[openList[x]];// изменение стоимости F

                // Посмотрим, не изменит ли элемент F счет элемента с текущего местоположения в куче
                m = x;
                while (m != 1) // Пока элемент не всплыл на верх (m = 1)
                {
                    //Проверьте, является ли дочерний элемент < родительского. Если да, замените их
                    if (Fcost[openList[m]] < Fcost[openList[m/2]])
                    {
                        temp = openList[m/2];
                        openList[m/2] = openList[m];
                        openList[m] = temp;
                        m = m/2;
                    }
                    else
                        break;
                }
                break; //exit for x = loop
            } //If openX(openList(x)) = a
            } //For x = 1 To numberOfOpenListItems
        }//If tempGcost < Gcost(a,b)

    }//else If whichList(a,b) = onOpenList
    }//If not Не вырезать угол
    }//If not Не квадрат стены / препятствия
    }//If not Уже не в закрытом списке
    }//If not Не с карты
    }//for (a = parentXval-1; a <= parentXval+1; a++){
    }//for (b = parentYval-1; b <= parentYval+1; b++){

    }//if (numberOfOpenListItems != 0)

//9.Если открытый список пуст, то пути нет.
    else
    {
        path = nonexistent; break;
    }

    //Если цель добавляется в открытый список, то путь найден.
    if (whichList[targetX][targetY] == onOpenList)
    {
        path = found; break;
    }

    }
    while (1);//Выполняйте до тех пор, пока не будет найден/не найден путь

//10.Сохранить путь, если он существует
    if (path == found)
    {

//a.Отправляясь от цели к исходному местоположению, проверяя
   // родительский элемент каждой ячейки, определите длину пути
    pathX = targetX; pathY = targetY;
    do
    {
        //Посмотрите родительский элемент текущей ячейки.
        tempx = parentX[pathX][pathY];
        pathY = parentY[pathX][pathY];
        pathX = tempx;

        //Определите длину пути
        pathLength[pathfinderID] = pathLength[pathfinderID] + 1;
    }
    while (pathX != startX || pathY != startY);

//b.Измените размер банка данных до нужного размера в байтах
    pathBank[pathfinderID] = static_cast<int*> (realloc (pathBank[pathfinderID], pathLength[pathfinderID]*8));

//c. Теперь скопируйте информацию о пути в банк данных. Так как мы
    // работаем в обратном направлении от цели до начальной позиции, мы копируем
    // информацию в банк данных в обратном порядке. Результатом является
    // правильно упорядоченный набор данных пути, от первого шага до последнего.
    pathX = targetX ; pathY = targetY;
    cellPosition = pathLength[pathfinderID]*2;//Начинать в конце
    listPath.clear();
    do
    {
    cellPosition = cellPosition - 2;//ОтРаботаем назад на 2 целых числа
    pathBank[pathfinderID] [cellPosition] = pathX;
    pathBank[pathfinderID] [cellPosition+1] = pathY;
    listPath.append(QPoint(pathBank[pathfinderID] [cellPosition], pathBank[pathfinderID] [cellPosition+1]));
//d.Посмотрите родительский элемент текущей ячейки.
    tempx = parentX[pathX][pathY];
    pathY = parentY[pathX][pathY];
    pathX = tempx;

//e.!!!!!!Если мы достигли начального квадрата, выходим из цикла!!!!!!!!
    }
    while (pathX != startX || pathY != startY);
    listPath.append(QPoint(startX, startY));
    //Для отладки////////////////////////////////!
    //foreach (QPoint p, listPath) {
    //      qDebug()<<"listPath "<<"X="<<p.x()<<" Y="<<p.y();
    //}//+/////////////////////////////////////////!
//11.Прочитать первый шаг по пути в массивы xPath / yPath
    // !!!ReadPath(pathfinderID,startingX,startingY,1);

    }
    return path;


//13.Если путь к выбранному объекту отсутствует, установите для pathfinder
    // xPath и yPath равное его текущему местоположению и возвратите, что путь не существует.
noPath:
    xPath[pathfinderID] = startingX;
    yPath[pathfinderID] = startingY;
    return nonexistent;

}

void DataGraph::InitializePathfinder()
{
    for (int x = 0; x < numberPeople+1; x++)//numberPeople = 1 (+1)
        pathBank [x] = static_cast<int*> (malloc(4));
}

void DataGraph::EndPathfinder()
{
    for (int x = 0; x < numberPeople+1; x++)
    {
        free (pathBank [x]);
    }
}


void DataGraph::getPath(int startX, int startY, int targetX, int targetY, bool orto, bool updown)
{
    InitializePathfinder();//Ok
    FindPath(startX, startY, targetX, targetY, orto, updown);
    EndPathfinder();

}


