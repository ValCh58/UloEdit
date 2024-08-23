/*
;===================================================================
;A* Pathfinder (Version 1.71a) by Patrick Lester. Used by permission.
;===================================================================
;Last updated 06/16/03 -- Visual C++ version
 */

#include <QtCore>
#include <QtDebug>

    const int mapWidth = 157, mapHeight = 300, tileSize = 1, numberPeople = 1;//3;
    int onClosedList = 1;//0;
    const int notfinished = 0, notStarted = 0;//Константы, связанные с путями
    const int found = 1, nonexistent = 2;
    const int walkable = 0, unwalkable = 1;//Константы массива проходимости

    char walkability [mapWidth][mapHeight];
    int openList[mapWidth*mapHeight+2];   //Mассив cодержащий идентификаторы открытых элементов списка
    int whichList[mapWidth+1][mapHeight+1];  //Массив используемый для записи, находится ли ячейка
                                             //в открытом списке или в закрытом списке
    int openX[mapWidth*mapHeight+2]; //Массив сохраняет x-позицию элемента в открытом списке
    int openY[mapWidth*mapHeight+2]; //Массив сохраняет y-позицию элемента в открытом списке
    int parentX[mapWidth+1][mapHeight+1]; //Массив для хранения родителя каждой ячейки (x)
    int parentY[mapWidth+1][mapHeight+1]; //Массив для хранения родителя каждой ячейки (Y)
    int Fcost[mapWidth*mapHeight+2];	//Массив для хранения F стоимости ячейки в открытом списке
    int Gcost[mapWidth+1][mapHeight+1]; //Массив для хранения стоимости G для каждой ячейки
    int Hcost[mapWidth*mapHeight+2];	//Массив для хранения стоимости H для каждой ячейки
    int pathLength[numberPeople+1];     //Хранит длину найденного пути для объекта
    int pathLocation[numberPeople+1];   //Сохраняет текущее положение вдоль выбранного пути для объекта
	int* pathBank [numberPeople+1];

    //Переменные чтения пути
	int pathStatus[numberPeople+1];
	int xPath[numberPeople+1];
	int yPath[numberPeople+1];

    int xLoc [4]; int yLoc [4];
//-----------------------------------------------------------------------------
// Function Prototypes
//-----------------------------------------------------------------------------
void ReadPath(int pathfinderID,int currentX,int currentY, int pixelsPerFrame);
int  ReadPathX(int pathfinderID,int pathLocation);
int  ReadPathY(int pathfinderID,int pathLocation);
int  FindPath (int startingX, int startingY, int targetX, int targetY, bool orto);
void InitializePathfinder (void);
void EndPathfinder (void);
//-----------------------------------------------------------------------------
// Allocates memory for the pathfinder.
//-----------------------------------------------------------------------------
void InitializePathfinder (void)
{
	for (int x = 0; x < numberPeople+1; x++)
		pathBank [x] = (int*) malloc(4);
}


//-----------------------------------------------------------------------------
// Frees memory used by the pathfinder.
//-----------------------------------------------------------------------------
void EndPathfinder (void)
{
	for (int x = 0; x < numberPeople+1; x++)
	{
		free (pathBank [x]);
	}
}

//-----------------------------------------------------------------------------
// Finds a path using A*
//-----------------------------------------------------------------------------
int FindPath (int startingX, int startingY, int targetX, int targetY, bool orto)
{
    int pathfinderID=1;//Перенесен из первого параметра функции//

	int onOpenList=0, parentXval=0, parentYval=0,
    xa=0, yb=0, m=0, u=0, v=0, temp=0, corner=0, numberOfOpenListItems=0,
	addedGCost=0, tempGcost = 0, path = 0,
	tempx, pathX, pathY, cellPosition,
	newOpenListItemID=0;

    //walkability[4][8]=1;//Установка элемента стены//

//1.Преобразование координат местоположения (в пикселях) в координаты в массиве удобоукладываемости.
	int startX = startingX/tileSize;
	int startY = startingY/tileSize;	
	targetX = targetX/tileSize;
	targetY = targetY/tileSize;

//2.Быстрые проверки пути: при обстоятельствах купола никакой путь не должен быть сгенерированным
    //Если начальное местоположение и цель находятся в одном местоположении
	if (startX == targetX && startY == targetY && pathLocation[pathfinderID] > 0)
		return found;
	if (startX == targetX && startY == targetY && pathLocation[pathfinderID] == 0)
		return nonexistent;

    //Если целевой квадрат недостижим, вернем его как несуществующий путь
	if (walkability[targetX][targetY] == unwalkable)
		goto noPath;

//3.Сбросьте некоторые переменные, которые необходимо очистить
    if (onClosedList > 1000000) //переустановка списка время от времени
	{
		for (int x = 0; x < mapWidth;x++) {
			for (int y = 0; y < mapHeight;y++)
				whichList [x][y] = 0;
		}
		onClosedList = 10;	
	}
    onClosedList = onClosedList+2; //Изменение значений onOpenList и onClosed list выполняется быстрее, чем redimming массива whichList ()
	onOpenList = onClosedList-1;
	pathLength [pathfinderID] = notStarted;//i.e, = 0
	pathLocation [pathfinderID] = notStarted;//i.e, = 0
    Gcost[startX][startY] = 0; //Сбросить значение начального квадрата в G до 0

//4.Добавьте начальное местоположение (точку А) в открытый список проверяемых квадратов.
    numberOfOpenListItems = 1;//Кол. элементов открытого списка//
    openList[1] = 1;//Назначить его как верхний (и только в данный момент) элемент в открытом списке, который поддерживается как двоичная куча (объясняется ниже)
    openX[1] = startX ; openY[1] = startY;//Задание начальной точки пути А!!!//

//5.Цикл, пока путь не будет найден или будет считаться несуществующим.
	do
	{

//6.Если открытый список не пуст, извлеките первую ячейку из списка.
   // Это самая низкая ячейка стоимости F в открытом списке.
	if (numberOfOpenListItems != 0)
	{

//7.Вытаскивать первый элемент из открытого списка
	parentXval = openX[openList[1]];
    parentYval = openY[openList[1]]; //Запись координаты ячейки элемента
    whichList[parentXval][parentYval] = onClosedList;//Добавление элемента в закрытый список

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
		if (Fcost[openList[u]] >= Fcost[openList[2*u]]) 
			v = 2*u;
		if (Fcost[openList[v]] >= Fcost[openList[2*u+1]]) 
			v = 2*u+1;		
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
         if (xa == parentXval-1)
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
         }
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

           //Вычислите издержки H и F и родителя
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
             // стоимость записи элемента F и ее позицию в открытом списке, чтобы сделать
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

//10.Сохранть путь, если он существует
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
    pathBank[pathfinderID] = (int*) realloc (pathBank[pathfinderID], pathLength[pathfinderID]*8);

//c. Теперь скопируйте информацию о пути в банк данных. Так как мы
    // работаем в обратном направлении от цели до начальной позиции, мы копируем
    // информацию в банк данных в обратном порядке. Результатом является
    // правильно упорядоченный набор данных пути, от первого шага до последнего.
	pathX = targetX ; pathY = targetY;
    cellPosition = pathLength[pathfinderID]*2;//Начинать в конце

	do
	{
    cellPosition = cellPosition - 2;//ОтРаботаем назад на 2 целых числа
	pathBank[pathfinderID] [cellPosition] = pathX;
	pathBank[pathfinderID] [cellPosition+1] = pathY;

//d.Посмотрите родительский элемент текущей ячейки.
	tempx = parentX[pathX][pathY];		
	pathY = parentY[pathX][pathY];
	pathX = tempx;

//e.!!!!!!Если мы достигли начального квадрата, выходим из цикла!!!!!!!!
    qDebug()<<"path information ->>> pathX="<<pathX<<" pathY="<<pathY;
	}
	while (pathX != startX || pathY != startY);	

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




//==========================================================
//READ PATH DATA: These functions read the path data and convert
//it to screen pixel coordinates.
void ReadPath(int pathfinderID,int currentX,int currentY,int pixelsPerFrame)
{
/*
; Примечание по PixelsPerFrame: необходимости этого параметра, вероятно, нет
; Это очевидно, поэтому есть небольшое объяснение. Эта
; Параметр используется, чтобы определить, закрыл ли Pathfinder
; Достаточно, чтобы центр данного квадрата пути требовал поиска
; Следующий шаг на пути.

;  Это необходимо, потому что скорость некоторых спрайтов может быть
; Сделать достижение точного центра квадрата пути невозможно.
; В Demo # 2 у искателя есть скорость 3 пикселя за кадр. Наше
; Размер плитки составляет 50 пикселей, поэтому центр плитки будет располагаться
; 25, 75, 125 и т. Д. Некоторые из них не равномерно делятся на 3
; Наш следопыт должен знать, насколько близко он достаточно близко к центру.
; Он вычисляет это, видя, если pathfinder меньше
; PixelsPerFrame # пикселей от центра квадрата.

;  Это может вызвать проблемы, если у вас есть * действительно * быстро
; Спрайт и / или действительно маленькие плитки, в этом случае вам может понадобиться
; Немного измените формулу. Но это почти никогда не должно быть проблемой
; Для игр со стандартными размерами и нормальной скоростью. Наш смайлик
; В Demo # 4 движется довольно быстро, и это даже не близко
; К проблеме.
*/

    int ID = pathfinderID; //Избыточен, но облегчает чтение

    //Если найден путь для следопыта
	if (pathStatus[ID] == found)
	{
        // Если искатель пути только начинает новый путь или достиг
        // центра текущего квадрата пути (и конец пути
        // не достигнут), найдите следующий квадрат пути.
		if (pathLocation[ID] < pathLength[ID])
		{
            //Если он только начинается или находится достаточно близко к центру квадрата
			if (pathLocation[ID] == 0 || 
				(abs(currentX - xPath[ID]) < pixelsPerFrame && abs(currentY - yPath[ID]) < pixelsPerFrame))
					pathLocation[ID] = pathLocation[ID] + 1;
		}

        //Прочитайте данные пути.
		xPath[ID] = ReadPathX(ID,pathLocation[ID]);
		yPath[ID] = ReadPathY(ID,pathLocation[ID]);

        // Если центр последнего пути, указанного в пути, был достигнут, тогда сброс.
		if (pathLocation[ID] == pathLength[ID]) 
		{
			if (abs(currentX - xPath[ID]) < pixelsPerFrame 
                && abs(currentY - yPath[ID]) < pixelsPerFrame) //Если достаточно близко к центру квадрата
					pathStatus[ID] = notStarted; 
		}
	}

    // Если для этого pathfinder нет пути, просто оставайтесь в текущем месте.
	else
	{	
		xPath[ID] = currentX;
		yPath[ID] = currentY;
	}
}


// Следующие две функции считывают исходные данные пути из pathBank.
// Вы можете напрямую вызвать эти функции и, если хотите, пропустить функцию readPath
// выше. Убедитесь, что вы знаете, что ваш текущий pathLocation.
//-----------------------------------------------------------------------------
// Name: ReadPathX
// Desc: Reads the x coordinate of the next path step
//-----------------------------------------------------------------------------
int ReadPathX(int pathfinderID,int pathLocation)
{
	int x;
	if (pathLocation <= pathLength[pathfinderID])
	{

	//Read coordinate from bank
	x = pathBank[pathfinderID] [pathLocation*2-2];

    // Настройте координаты таким образом, чтобы они совпадали с центром
    // квадрата пути (необязательно). Это предполагает, что вы используете
    // спрайты, которые центрированы - то есть с помощью команды midHandle.
    // В противном случае вам нужно будет настроить это.
	x = tileSize*x + .5*tileSize;
	
	}
	return x;
}	


//-----------------------------------------------------------------------------
// Name: ReadPathY
// Desc: Reads the y coordinate of the next path step
//-----------------------------------------------------------------------------
int ReadPathY(int pathfinderID,int pathLocation)
{
	int y;
	if (pathLocation <= pathLength[pathfinderID])
	{

	//Read coordinate from bank
	y = pathBank[pathfinderID] [pathLocation*2-1];

	//Adjust the coordinates so they align with the center
	//of the path square (optional). This assumes that you are using
	//sprites that are centered -- i.e., with the midHandle command.
	//Otherwise you will want to adjust this.
	y = tileSize*y + .5*tileSize;
	
	}
	return y;
}


	

