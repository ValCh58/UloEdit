#include <QGraphicsDropShadowEffect>
#include <QMenu>
#include <QAction>
#include <QMdiSubWindow>
#include <QScrollBar>
#include <QDebug>

#include "Style.h"
#include "child.h"
#include "ulomodeltable.h"
#include "cmdedit.h"
#include "crc16.h"
#include "finddialog.h"



child::child(QWidget *parent):QWidget(parent)
{
    mwnd = (MainWindow*)parent;
    FormCmp = 0;
    memset(FCmpFlg, 0, sizeof(int)*MAX_CMD_COUNT);

    setAttribute(Qt::WA_DeleteOnClose);
    createMenus();
    Change = false;
    tableGroupBox = new QGroupBox(tr("Таблица команд"));
    QFormLayout *layoutTable = new QFormLayout;
    uloModelTable = new UloModelTable(this, 6, FCmpFlg);
    view = new QTableView(this);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->setSelectionMode(QAbstractItemView::ContiguousSelection);
    view->setStyle(new Style);
    view->setAlternatingRowColors(true);
    view->setStyleSheet("QTableView::item {selection-color: yellow; selection-background-color: #2F4F4F;}");

    view->setStatusTip("Таблица команд");
    view->setCornerButtonEnabled(false);//Верхний левый угол - выбор всех ячеек ОТКЛЮЧЕН!//
    view->setModel(uloModelTable);
    headerSize(view);
    dynResizeTable();

    QGraphicsDropShadowEffect *gr = new QGraphicsDropShadowEffect(this);
    view->setGraphicsEffect(gr);
    view->show();
    layoutTable->addRow(view);
    tableGroupBox->setLayout(layoutTable);

    sbar = new QStatusBar(this);//Добавим статус бар в дочернее окно//
    sbar->setFixedHeight(18);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tableGroupBox);
    mainLayout->addWidget(sbar);
    labelStatus = new QLabel;
    sbar->addWidget(labelStatus);
    setLayout(mainLayout);
    setLastRowOk(0);
    setIx(0); //Установка номера строки для операций поиска//
    setValNext(0);// И значения для продолженного поиска операнда//
    setIdxFindCmp(0);

    //Создаем горячую клавишу для окна для поиска несовпадений вперед//
    keyCtrlF = new QShortcut(this); // Инициализируем объект
    keyCtrlF->setKey(Qt::ALT + Qt::Key_F); // Устанавливаем сочетание клавиш
    connect(keyCtrlF, &QShortcut::activated, this, &child::findCmpClickFront);

    //Создаем горячую клавишу для окна для поиска несовпадений назад//
    keyCtrlD = new QShortcut(this); // Инициализируем объект
    keyCtrlD->setKey(Qt::ALT + Qt::Key_D); // Устанавливаем сочетание клавиш
    connect(keyCtrlD, &QShortcut::activated, this, &child::findCmpClickBack);

    connect(view, &QTableView::doubleClicked, this, &child::eventDbClickedFormEdit);
    connect(view, &QTableView::clicked, this, &child::cmpClickView);
    connect(view, &QTableView::pressed, this, &child::cmpKeyPressView);
    connect(view->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &child::cmpClickSelectView);
    view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(view, &QTableView::customContextMenuRequested, this, &child::showContextMenu);
    view->viewport()->installEventFilter(this);

}

child::~child()
{
   removeMenu();
   while (!FUndo.empty())
   {
       delete FUndo.front();
       FUndo.pop_front();
   }
   if(FormCmp){
      FormCmp->setFormCmp(0);
   }
   FCmdSize = 0;
   FCmdCRC = 0;
   FormCmp = 0;
}

void child::setConn(){
    connect(view->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &child::cmpClickSelectView);
}
void child::setDisConn(){
    disconnect(view->selectionModel(), &QItemSelectionModel::currentRowChanged, this, &child::cmpClickSelectView);
}
int child::getLastRowOk() const
{
    return lastRowOk;
}

void child::setLastRowOk(int value)
{
    lastRowOk = value;
}

//Управление элементом меню поиск несовпадений//
void child::refreshFindCmpNo(){
    if(FormCmp){
        mwnd->setAnableItemMenu(mwnd->getFindCompareForward(),false);
        mwnd->setAnableItemMenu(mwnd->getFindCompareBack(),false);
    }else{
        mwnd->setAnableItemMenu(mwnd->getFindCompareForward(),true);
        mwnd->setAnableItemMenu(mwnd->getFindCompareBack(),true);
    }
}


//Управление элементом меню ОтменитЬ//
void child::refreshUndo(){
    if(getChange()){
        mwnd->setAnableItemMenu(mwnd->getCancelAct(),false);
      SetChange(true);
   }else{
       mwnd->setAnableItemMenu(mwnd->getCancelAct(),true);
   }
}

//Фильтр событий по колесику мыши//
bool child::eventFilter(QObject *target, QEvent *event){

    if (target == view->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
        if(wheelEvent->delta() > 0){
           eventScrollTable(-1);
        }else{
            eventScrollTable(+1);
        }
        return true;
    }else if(event->type() == QEvent::Paint){
             refreshUndo();
             refreshFindCmpNo();//Меню поиска несовпадений//
    }
    return QObject::eventFilter(target, event);
}

//Движение строки по таблице при вращении колесика мыши//
//UpDown значение на которое изменяется индекс в модели таблицы//
void child::eventScrollTable(int UpDown){

    QModelIndex modIdx = view->currentIndex();
    modIdx = uloModelTable->index(modIdx.row()+UpDown, 0, QModelIndex());

    if(modIdx.isValid()){
       //emit view->selectionModel()->currentChanged(modIdx, modIdx);//Выдаем сигнал currentChanged//
       view->setCurrentIndex(modIdx);
       if(FormCmp){//Для работы в режиме сравнения файлов//
          FormCmp->getView()->setCurrentIndex(modIdx);
       }
    }
}

//Вызов контекстного меню//
void child::showContextMenu(const QPoint &point){
    Q_UNUSED(point);
    QMenu *menu = mwnd->getEditMenu();//Вызываем готовое меню из главного окна//
    menu->exec(QCursor::pos());
}

//Вызов формы редактирования//
void child::eventDbClickedFormEdit(QModelIndex index)
{
     //QModelIndex index = view->selectionModel()->currentIndex();
     if(!index.isValid()){
         return;
     }
     firstRowEdit = index.row();//Дlя возврата на эту же строку - после отмены изменений//
     UloData& data = uloModelTable->getUloData(index.row());
     cmdedit *dialog = new cmdedit(this,Qt::WindowTitleHint | Qt::WindowSystemMenuHint,&data);
     dialog->dCmdEdit(index, &FCmd[index.row()]);
     dialog->show();
}

//Замена в строке//
void child::replaceLine(){
    QItemSelectionModel *selectionModel = view->selectionModel();
    QModelIndexList selRow = selectionModel->selectedRows();
    if(selRow.size() == 0){
        createWindowMessage(QIcon(":/iconsFree/table_multiple.png"),"Копирование","Выберите строку в таблице!");
        return;
    }
    firstRowEdit = selRow.first().row();
    numRowEdit   = selRow.size();
    if(!numRowEdit > 0) return;
    SaveUndo();
    int j = firstRowEdit + FClipboardCount > MAX_CMD_COUNT ? MAX_CMD_COUNT - firstRowEdit : FClipboardCount;
    // возьмем из буфера обмена
    memcpy(&FCmd[firstRowEdit], &FClipboard[0], j*sizeof(cmd_t));
    endChange(FCmd);
}

//Создание нового файла//
void child::greateNewFile()
{
    setFFileName("");
    memset(FCmd, -1, MAX_CMD_COUNT*sizeof(cmd_t));
    setWindowTitle("Новый файл");
    setDataToModel(FCmd, MAX_CMD_COUNT);
}

//Копировать строку//
void child::CopyLineClick(){
   QItemSelectionModel *selectionModel = view->selectionModel();
   QModelIndexList selRow = selectionModel->selectedRows();
   if(selRow.size() == 0){
       createWindowMessage(QIcon(":/iconsFree/table_multiple.png"),"Копирование","Выберите строку в таблице!");
       return;
   }
   firstRowEdit = selRow.first().row();
   numRowEdit   = selRow.size();
   // сохраним в буфере обмена
   memcpy(&FClipboard[0], &FCmd[firstRowEdit], sizeof(cmd_t)*numRowEdit);
   FClipboardCount = numRowEdit;
}

//Подготовка к выполнению команд редактирования//
void child::PrepareEdit(cmd_t *fcmd){
    if(!numRowEdit > 0) return;
    SaveUndo();
    // забьем все nop'ами
    memset(fcmd, -1        , sizeof(cmd_t)*MAX_CMD_COUNT);
    // скопируем начало
    memcpy(fcmd, &FCmd[0], sizeof(cmd_t)*firstRowEdit);
}

//Возврат на текущую строку - после выполнения команд редактирования//
void child::backToLine(){
    QModelIndex topLeft, bottomRight;
    QItemSelectionModel *selectionModel = view->selectionModel();
    topLeft = uloModelTable->index(firstRowEdit, 0, QModelIndex());
    bottomRight = uloModelTable->index(numRowEdit>1?firstRowEdit+numRowEdit-1:firstRowEdit, 5, QModelIndex());
    QItemSelection selection(topLeft, bottomRight);
    selectionModel->select(selection, QItemSelectionModel::Select);
}

//Завершение команд редактирования//
void child::endChange(cmd_t *fcmd){
    // вернем назад
    memcpy(&FCmd[0  ], fcmd, sizeof(cmd_t)*MAX_CMD_COUNT);
    setChange(true);
    setDataToModel(FCmd, MAX_CMD_COUNT);
    backToLine();
    updateCompareEdit();
    selectRow(this, firstRowEdit);
    if(FormCmp){
       FormCmp->setFocus();
       FormCmp->update();
       this->setFocus();
    }
}

//========================================//
//========================================//
child *child::getFormCmp() const
{
    return FormCmp;
}

void child::setFormCmp(child *value)
{
    FormCmp = value;
}

int child::getNumRow() const
{
    return numRow;
}

void child::setNumRow(int value)
{
    numRow = value;
}

int child::getFirstRow() const
{
    return firstRow;
}

void child::setFirstRow(int value)
{
    firstRow = value;
}
bool child::getChange() const
{
    return Change;
}

void child::setChange(bool value)
{
    Change = value;
}

int child::getFVersion() const
{
    return FVersion;
}

void child::setFVersion(int value)
{
    FVersion = value;
}

QString child::getFFileName() const
{
    return FFileName;
}

void child::setFFileName(const QString &value)
{
    FFileName = value;
}
void child::setValNext(long value)
{
    valNext = value;
}

int child::getIx() const
{
    return ix;
}

void child::setIx(int value)
{
    ix = value;
}

long child::getValNext() const
{
    return valNext;
}
//========================================//
//========================================//

//Вставка строки//
void child::AddLine(){
    cmd_t fcmd[MAX_CMD_COUNT];

    QItemSelectionModel *selectionModel = view->selectionModel();
    QModelIndexList selRow = selectionModel->selectedRows();
    if(selRow.size() == 0){
        createWindowMessage(QIcon(":/iconsFree/table_row_insert.png"),"Вставить","Выберите строку в таблице!");
        return;
    }
    firstRowEdit = selRow.first().row();
    PrepareEdit(fcmd);
    int j = firstRowEdit + FClipboardCount > MAX_CMD_COUNT ? MAX_CMD_COUNT - firstRowEdit : FClipboardCount;
    // возьмем из буфера обмена
    memcpy(&fcmd[firstRowEdit], &FClipboard[0], j*sizeof(cmd_t));
    // вставить остальное
    memcpy(&fcmd[firstRowEdit+j], &FCmd[firstRowEdit  ], sizeof(cmd_t)*(MAX_CMD_COUNT-firstRowEdit-j));
    endChange(fcmd);
}

//Вставка пустой строки//
void child::InsEmptyLine(){
    cmd_t fcmd[MAX_CMD_COUNT];

    QItemSelectionModel *selectionModel = view->selectionModel();
    QModelIndexList selRow = selectionModel->selectedRows();
    if(selRow.size() == 0){
        createWindowMessage(QIcon(":/iconsFree/table_replace.png"),"Вставить пустую","Выберите строку в таблице!");
        return;
    }
    firstRowEdit = selRow.first().row();
    numRowEdit   = selRow.size();
    PrepareEdit(fcmd);
    int j = numRowEdit;
    // вставить остальное
    memcpy(&fcmd[firstRowEdit+j], &FCmd[firstRowEdit  ], sizeof(cmd_t)*(MAX_CMD_COUNT-firstRowEdit-j));
    endChange(fcmd);
}

//Вырезать строку//
void child::CutLines(){
    cmd_t fcmd[MAX_CMD_COUNT];

    QItemSelectionModel *selectionModel = view->selectionModel();
    QModelIndexList selRow = selectionModel->selectedRows();
    if(selRow.size() == 0){
        createWindowMessage(QIcon(":/iconsFree/table_select.png"),"Вырезать","Выберите строку в таблице!");
        return;
    }
    firstRowEdit = selRow.first().row();
    numRowEdit   = selRow.size();
    PrepareEdit(fcmd);
    // сохраним в буфере обмена
    memcpy(&FClipboard[0], &FCmd[firstRowEdit], sizeof(cmd_t)*numRowEdit);
    FClipboardCount = numRowEdit;
    // вырезать
    memcpy(&fcmd[firstRowEdit], &FCmd[firstRowEdit+numRowEdit], sizeof(cmd_t)*(MAX_CMD_COUNT-firstRowEdit-numRowEdit));
    endChange(fcmd);
}

//Удалить строку//
void child::DelLines(){
    cmd_t fcmd[MAX_CMD_COUNT];

    QItemSelectionModel *selectionModel = view->selectionModel();
    QModelIndexList selRow = selectionModel->selectedRows();
    if(selRow.size() == 0){
        createWindowMessage(QIcon(":/iconsFree/table_row_delete.png"),"Удалить","Выберите строку в таблице!");
        return;
    }
    firstRowEdit = selRow.first().row();
    numRowEdit   = selRow.size();
    PrepareEdit(fcmd);
    // вырезать
    memcpy(&fcmd[firstRowEdit], &FCmd[firstRowEdit+numRowEdit], sizeof(cmd_t)*(MAX_CMD_COUNT-firstRowEdit-numRowEdit));
    endChange(fcmd);
}

//Выбрать все строки в таблице//
void child::SelectAllLines(){
    QItemSelectionModel *selectionModel = view->selectionModel();
    QModelIndex parent = QModelIndex();
    QModelIndex topLeft = uloModelTable->index(0, 0, parent);
    QModelIndex bottomRight = uloModelTable->index(uloModelTable->rowCount(parent)-1, uloModelTable->columnCount(parent)-1, parent);
    QItemSelection selection(topLeft, bottomRight);
    selectionModel->select(selection, QItemSelectionModel::Select);
}

//Отменить изменения//
void child::UndoChange(){
    if(!FUndo.empty()){
       memcpy(FCmd,FUndo.back(),sizeof(cmd_t)*MAX_CMD_COUNT);
       delete FUndo.back();
       FUndo.pop_back();
       setDataToModel(FCmd, MAX_CMD_COUNT);
       backToLine();
       updateCompareEdit();
       selectRow(this, firstRowEdit);
    }
    if (FUndo.empty()){
        mwnd->setAnableItemMenu(mwnd->getCancelAct(),true);
        setChange(false);
        SetChange(false);
    }
    if(FormCmp){
       FormCmp->setFocus();
       FormCmp->update();
       this->setFocus();
    }
}

//Открытие файла//
bool child::Open(QString fName, int ver){
     QByteArray ba = fName.toUtf8();//QString
     char const*file = ba.data();// в char*
     FILE *fp;
     long fsz;
     int c;
     WORD crc;
     fp = fopen(file, "rb");

     if(!fp){
         QString strMess = "Ошибка открытия файла " + fName;
         QMessageBox::critical(this, "Сообщение", strMess, QMessageBox::Ok );
         return false;
     }
     fseek(fp, 0, SEEK_END);
     fsz = ftell(fp);
     fseek(fp, 0, SEEK_SET);

     switch(ver){
     case 1:
           c = fsz/sizeof(cmd_t);
          if(fsz - c*sizeof(cmd_t) != 0)
          if(QMessageBox::question(this, "Сообщение", "Неверный формат файла "+fName+
                                         " Продолжить?",QMessageBox::Yes|QMessageBox::No)==QMessageBox::No){
             fclose(fp);
             return false;
          }

          FCmdSize = c;
          fread(FCmd, FCmdSize > MAX_CMD_COUNT ? MAX_CMD_COUNT*sizeof(cmd_t) : FCmdSize*sizeof(cmd_t), 1, fp);
          //crc = CRC16((WORD*)FCmd, MAX_CMD_COUNT);
     break;

     case 2:
         if(fsz < 6){
            fclose(fp);
            QMessageBox::critical(this, "Сообщение", "Размер файла не может быть < 6 байт", QMessageBox::Ok );
            return false;
         }
         fread(&FCmdCRC, sizeof(WORD), 1, fp);
         fread(&FCmdSize, sizeof(DWORD), 1, fp);
         if(FCmdSize != fsz-6 || fsz > 65536){
             if(QMessageBox::question(this, "Сообщение", "Неверный формат файла "+fName+
                                            " Продолжить?",QMessageBox::Yes|QMessageBox::No)==QMessageBox::No){
                fclose(fp);
                return false;
             }
         }
         FCmdSize = fsz-6;
         fread(FCmd, FCmdSize, 1, fp);
         crc = CRC16((WORD*)FCmd, MAX_CMD_COUNT);
         if(crc!=FCmdCRC){
            if(QMessageBox::question(this, "Сообщение", "Не соответствуют контрольные суммы файла "+fName+
                                           " Продолжить?",QMessageBox::Yes|QMessageBox::No)==QMessageBox::No){
               fclose(fp);
               return false;
            }
         }
         FCmdCRC = crc;
     break;

     case 3:
         if(fsz < 6){
            fclose(fp);
            QMessageBox::critical(this, "Сообщение", "Размер файла не может быть < 6 байт", QMessageBox::Ok );
            return false;
         }
         fread(&FCmdCRC, sizeof(WORD), 1, fp);
         fread(&FCmdSize, sizeof(DWORD), 1, fp);
         if(FCmdSize != fsz-6 || fsz > 65536){
             if(QMessageBox::question(this, "Сообщение", "Неверный формат файла "+fName+
                                            " Продолжить?",QMessageBox::Yes|QMessageBox::No)==QMessageBox::No){
               fclose(fp);
               return false;
            }
         }
         FCmdSize = fsz-6;
         fread(FCmd, FCmdSize, 1, fp);
         crc = CRC16modbus((BYTE*)FCmd, MAX_CMD_COUNT*sizeof(cmd_t));
         if(crc!=FCmdCRC){
             if(QMessageBox::question(this, "Сообщение", "Не соответствуют контрольные суммы файла "+fName+
                                            " Продолжить?",QMessageBox::Yes|QMessageBox::No)==QMessageBox::No){
               fclose(fp);
               return false;
            }
         }
         FCmdCRC = crc;
     break;

     default:
             fclose(fp);
             QMessageBox::critical(this, "Сообщение", "Неизвестный формат файла", QMessageBox::Ok );
             return false;
     }
     fclose(fp);
     FVersion = ver;
     FFileName = file;
     setWindowTitle(tr(file));
     QString strVer;
     labelStatus->setText("Версия файла: "+strVer.number(ver,10)+".0");
     //Загрузка данных в модель/////////////////////////
     uloModelTable->setDataToModel(FCmd, MAX_CMD_COUNT);
     ///////////////////////////////////////////////////
     return true;
}

//Установка данных в модель//
void child::setDataToModel(Cmd fcmd, int cnt){
    uloModelTable->setDataToModel(fcmd, cnt);
}

//Сохранить файл КаК ...//
void child::SaveAs(){
   dSave.setAcceptMode(QFileDialog::AcceptSave);
   dSave.setWindowTitle("Сохранить как...");
   dSave.setLabelText(QFileDialog::Reject, "Отменить");
   dSave.setLabelText(QFileDialog::Accept, "Сохранить");
   dSave.setViewMode(QFileDialog::List);
   dSave.setNameFilter(tr("Файл управления 3.0 (*.bin);;Файл управления 2.0 (*.bin);;Файл управления 1.0 (*.bin)"));

   if(dSave.exec()){
      QStringList fileNames = dSave.selectedFiles();
      int ver = mwnd->getIndexFilter(dSave.selectedNameFilter());
      if(SaveToFile(fileNames.at(0), ver)){
         FVersion = ver;
         FFileName = fileNames.at(0);
         setWindowTitle(FFileName);
         Change = false;
      }
   }
}

//Отменить изменения//
void child::SaveUndo(){
    if (FUndo.size() >= 64)
    {
      // необходимо достать и удалить самое первое изменение
       free(FUndo.front());
       FUndo.pop_front();
    }
    Cmd* cmd = (Cmd*)new BYTE[MAX_CMD_COUNT*sizeof(cmd_t)];
    memcpy(cmd,FCmd,sizeof(cmd_t)*MAX_CMD_COUNT);
    FUndo.push_back(cmd);
    mwnd->setAnableItemMenu(mwnd->getCancelAct(),false);
    SetChange(true);
}

//Обновить одну строку модели по индексу//
void child::wrapUpdateRowModel(cmd_t ct, int idx){
    uloModelTable->updateRowModel(&ct, idx);
}

//Сохранить файл//
bool child::SaveToFile(QString fName, int ver){
    QByteArray ba = fName.toUtf8();//QString
    char const*file = ba.data();// в char*
    FILE *fp;

        fp = fopen(file, "wb");
        if(!fp){
            QString strMess = "Ошибка открытия файла " + fName;
            QMessageBox::critical(this, "Сообщение", strMess, QMessageBox::Ok );
            return false;
        }

        switch(ver){
        case 1:

        break;
        case 2:
                FCmdCRC = CRC16((WORD*)FCmd, MAX_CMD_COUNT);
                FCmdSize = MAX_CMD_COUNT*sizeof(cmd_t);
                fwrite(&FCmdCRC, sizeof(WORD), 1, fp);
                fwrite(&FCmdSize, sizeof(DWORD), 1, fp);
        break;
            case 3:
                FCmdCRC = CRC16modbus((BYTE*)FCmd, MAX_CMD_COUNT*sizeof(cmd_t));
                FCmdSize = MAX_CMD_COUNT*sizeof(cmd_t);
                fwrite(&FCmdCRC, sizeof(WORD), 1, fp);
                fwrite(&FCmdSize, sizeof(DWORD), 1, fp);
            break;
        }

        fwrite(FCmd, MAX_CMD_COUNT*sizeof(cmd_t), 1, fp);
        if(ver==1){
        WORD add[3] = {(WORD)-1,(WORD)-1,(WORD)-1};
            fwrite(add, 6, 1, fp);
        }
        fclose(fp);
        QString strVer;
        labelStatus->setText("Версия файла: "+strVer.number(ver,10)+".0");
        setChange(false);
        return true;
}

//Изменение ширины столбцов//
void child::headerSize(QTableView *tview){
    QHeaderView *vheader = tview->verticalHeader();
    vheader->setDefaultSectionSize(23);
    vheader->setStyleSheet("QHeaderView::section { background-color: rgb(200,200,200);font: 11px;}");

    QHeaderView *hheader = tview->horizontalHeader();
   // hheader->setStyleSheet("QHeaderView::section { background-color: rgb(200,200,200);font: 11px;font-weight: normal;}");
    hheader->resizeSection(0,70);
    hheader->resizeSection(1,65);
    hheader->resizeSection(2,75);
    hheader->resizeSection(3,35);
    hheader->resizeSection(4,75);
    hheader->resizeSection(5,80);
    //Скроем вертикальные заголовки//
    //view->verticalHeader()->setVisible(false);
}

//Динамическое изменение размеров таблицы//
void child::dynResizeTable(){
    QSizePolicy policy = view->sizePolicy();
    policy.setVerticalStretch(1);
    policy.setHorizontalStretch(1);
    view->setSizePolicy(policy);
}

//Переключение меню на начальное при запуске программы//
void child::removeMenu(){
    if(mwnd->getCntChild() == 1)
       mwnd->useMenuMain();
    mwnd->setCntChild(mwnd->getCntChild()-1);
}

//Переключение меню на полное меню после отображения дочернего окна//
void child::createMenus(){
    if(mwnd->getCntChild() == 0)
       mwnd->useMenuChild();
    mwnd->setCntChild(mwnd->getCntChild()+1);
}

//Обработка закрытия дочернего окна//
int child::FormCloseQuery(){
   int retVal = QMessageBox::No;

   QString f;
    if(Change){
       if(FFileName.isEmpty()){
           f = windowTitle();
       }else{
           f = FFileName;
       }

       retVal = QMessageBox::question(this, "Сообщение", "Сохранить изменения в файле "+f+"?"
                                          , QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
       if(retVal == QMessageBox::Yes){
          if(f.compare("Новый файл")!=0){
             SaveToFile(f, getFVersion());
          }else{
             SaveAs();
          }
       }
    }
    return retVal;
}

//Отображение изменения файла в строке статуса дочернего окна//
void child::SetChange(bool p){
    QString ver;
    ver += "Версия файла: " + ver.number(getFVersion(),10)+".0";

    if(p){
       labelStatus->setText("Изменен    " + ver);
    }else{
       labelStatus->setText(ver);
    }
}
int child::getIdxFindCmp() const
{
    return idxFindCmp;
}

void child::setIdxFindCmp(int value)
{
    idxFindCmp = value;
}


//Создание диалога для вывода сообщения//
void child::createWindowMessage(const QIcon &icon, QString strTitle, QString winText){
    QMessageBox *qms = new QMessageBox(this);
    qms->setWindowTitle(strTitle);
    qms->setText(winText);
    qms->setWindowIcon(icon);
    qms->setIcon(QMessageBox::Information);
    qms->exec();
}

//Найти команду//
void child::FindCommand(){
   FindCommandDialog *fdialog = new FindCommandDialog(this);
   fdialog->show();
}

//Найти операнд//
void child::FindOperand(){
   FindOperDialog *fdialog = new FindOperDialog(this);
   fdialog->show();
}

//Найти следующий операнд//
void child::NextFindOperand(){
   FindOperDialog *fdialog = new FindOperDialog(this);
   fdialog->nextFind();
   //int i = 0;
}

//Получить указатель на представление//
QTableView *child::getView() const
{
    return view;
}

//Получить указатель на модель//
UloModelTable *child::getUloModelTable() const
{
    return uloModelTable;
}

//Вызывается при закрытии формы//
void child::closeEvent(QCloseEvent *e){
    int sel = FormCloseQuery();
    if(sel == QMessageBox::Yes || sel == QMessageBox::No){
       //mwnd->closeResize();
       e->accept();
    }else{
       e->ignore();
    }
}

//Сравнение файлов//
//Вставка элементов меню с названиями файлов для неактивных форм//
void child::nCommandClick(){
   QMenu *menu = mwnd->getMenuCompare();
   QList<QAction *> action;
   int j = 0;

   clearItemMenu(menu);
   QList<QMdiSubWindow *>  list = mwnd->getMdiArea()->subWindowList();
   refreshFindCmpNo();
   for(int i = 0; i < list.size(); i++){
     child *c = (child*)(list.at(i))->widget();
     if(c != this){
        QString title = c->windowTitle();
        action.append(new QAction(QIcon(":/iconsFree/application_lightning.png"), title, this));
        action.at(j)->setStatusTip(title);
        action.at(j)->setData( QVariant::fromValue(list.at(i)->widget()));//Связь указателя на форму и элемент меню//
        connect(action.at(j), SIGNAL(triggered()), this, SLOT(nCompareClick()));
        menu->addAction(action.at(j));
        j++;
     }
   }
}

//Очистка сравнения файлов//
void child::nCmpClearClick(){
    refreshFindCmpNo();
    QList<QMdiSubWindow *>  list = mwnd->getMdiArea()->subWindowList();
    int i;
    for(i=0; i<list.size(); i++){
        child *ch = (child*)list.at(i)->widget();
        memset(ch->FCmpFlg, 0, sizeof(int)*MAX_CMD_COUNT);
        ch->FormCmp = 0;
        ch->getView()->reset();
        ch->getView()->scrollToTop();
        selectRow(ch,0);
    }
}

//Очистка элементов динамического меню при выполнении операции сравнения//
void child::clearItemMenu(QMenu *menu){
    QList<QAction *> actions = menu->actions();
    if(actions.size() > 1){
        for(int a = 1; a < actions.size(); a++){
            actions.at(a)->deleteLater();
        }
    }
}

//Команда меню сравнить файлы//
void child::nCompareClick(){
    int cmp;

    QAction* a = qobject_cast< QAction* >( sender() );
    QWidget* clientWidget = a->data().value< QWidget* >();
    child *cmpForm = (child *)clientWidget;
    if(cmpForm){
       setIdxFindCmp(0);
       FormCmp = cmpForm;
       cmpForm->setFormCmp(this);
       cmp = Compare(this, cmpForm);
       cmpForm->getView()->reset();
       this->getView()->reset();
       selectRow(FormCmp, 0);
       selectRow(this, 0);
       createWindowMessage(QIcon(":/iconsFree/book_spelling.png "),
                                 "Результат сравнения","Найдено " + QString().number(cmp, 10) + " расхождений.");
    }
}

//Обновление таблиц в режиме сравнения при редактировании//
void child::updateCompareEdit(){
    if(FormCmp){
        FormCmp->setFormCmp(this);
        Compare(this, FormCmp);
    }
}

//Программно выбрать строку в таблице//
void child::selectRow(child *frm,int row){
    QModelIndex topLeft, bottomRight;
    QItemSelectionModel *selectionModelCmpFrm = frm->getView()->selectionModel();
    deselectRows(frm);
    topLeft = frm->getUloModelTable()->index(row, 0, QModelIndex());
    bottomRight = frm->getUloModelTable()->index(row, 5, QModelIndex());
    QItemSelection selection(topLeft, bottomRight);
    selectionModelCmpFrm->select(selection, QItemSelectionModel::SelectCurrent);
    frm->getView()->setCurrentIndex(topLeft);
}

//Обработка одиночного клика в строке//
void child::cmpClickView(QModelIndex index){

    if(!index.isValid()){return;}
    ix = numRow = firstRow = index.row();
    if(FormCmp){
       selectRow(FormCmp, firstRow);
       //Синхронизация передвижения курсора - таблиц представлений//
       FormCmp->getView()->verticalScrollBar()->setValue(view->verticalScrollBar()->value());
    }
}

//Обработка перемещения выделенной строки курсором от клавиатуры//
void child::cmpKeyPressView(QModelIndex index/*,QModelIndex index1*/){
    //Q_UNUSED(index1);
    cmpClickView(index);
}

//Обработка события при выборе нескольких строк//
void child::cmpClickSelectView(QModelIndex index, QModelIndex index1){
    Q_UNUSED(index);
    Q_UNUSED(index1);

    if(FormCmp){
       QItemSelectionModel *selectionModelCmpFrm = FormCmp->getView()->selectionModel();
       QItemSelectionModel *selectionModel = view->selectionModel();
       QModelIndexList selRow = selectionModel->selectedRows();
       QModelIndex topLeft, bottomRight;
       //if(selRow.size() < 2){
       //   return;
       //}
       firstRow = selRow.at(0).row();
       numRow   = selRow.size();
       topLeft = FormCmp->getUloModelTable()->index(firstRow, 0, QModelIndex());
       bottomRight = FormCmp->getUloModelTable()->index(numRow>1?firstRow+numRow-1:firstRow, 5, QModelIndex());
       QItemSelection selection(topLeft, bottomRight);
       selectionModelCmpFrm->select(selection, QItemSelectionModel::SelectCurrent);
       FormCmp->getView()->verticalScrollBar()->setValue(view->verticalScrollBar()->value());
    }
}

//Очистка выбранных раннее строк в таблице//
void child::deselectRows(child * frm){
    setIdxFindCmp(0);
    QModelIndex topLeft, bottomRight;
    QItemSelectionModel *selectionModelCmpFrm = frm->getView()->selectionModel();
    QModelIndexList items = selectionModelCmpFrm->selectedRows();
    if(items.size()>0){
       topLeft = frm->getUloModelTable()->index(items.at(0).row(), 0, QModelIndex());
       bottomRight = frm->getUloModelTable()->index(items.size()>1?items.at(0).row()+items.size()-1:items.at(0).row(), 5, QModelIndex());
       QItemSelection selection(topLeft, bottomRight);
       selectionModelCmpFrm->select(selection, QItemSelectionModel::Deselect);
    }
}

//Сравнение структур данных при операции сравнения//
int child::Compare(child *fmAct, child *fmCmp){
    int i, c;
    int *flg1, *flg2;
    cmd_t *c1, *c2;

    flg1 = fmAct->FCmpFlg;
    flg2 = fmCmp->FCmpFlg;
    c1 = fmAct->FCmd;
    c2 = fmCmp->FCmd;
    c = 0;

    for(i=0; i<MAX_CMD_COUNT; i++){
        if(c1[i].cmd != c2[i].cmd){
           flg1[i] |= CMP_FLG_NOEQ;
           flg2[i] |= CMP_FLG_NOEQ;
           c++;
        }else{
           flg1[i] &= ~CMP_FLG_NOEQ;
           flg2[i] &= ~CMP_FLG_NOEQ;
        }
    }
    return c;
}

void child::findCmpClickFront(){
    if(!FormCmp){return;}

    cmd_t *c1, *c2;
    c1 = this->FCmd;
    c2 = FormCmp->FCmd;
    bool flagDiff,  prevFlagDiff;
    setIdxFindCmp(view->currentIndex().row());

    if(c1[getIdxFindCmp()].cmd != c2[getIdxFindCmp()].cmd){
        prevFlagDiff = true;
    }else{
        prevFlagDiff = false;
    }

    for(int i=getIdxFindCmp(); i<MAX_CMD_COUNT; i++){
        if(c1[i].cmd != c2[i].cmd){
            flagDiff = true;
        }else{
            flagDiff = false;
        }
        if(!prevFlagDiff && flagDiff){
            selectRow(FormCmp, i);
            selectRow(this, i);
            setIdxFindCmp(i);
            break;
        }
        prevFlagDiff = flagDiff;
    }
}

void child::findCmpClickBack(){
    if(!FormCmp){
        return;
    }
    //Получить текущий индекс!!!!!
    cmd_t *c1, *c2;
    c1 = this->FCmd;
    c2 = FormCmp->FCmd;
    bool flagDiff, prevFlagDiff;
    setIdxFindCmp(view->currentIndex().row());

    if(c1[getIdxFindCmp()].cmd != c2[getIdxFindCmp()].cmd){
        prevFlagDiff = true;
    }else{
        prevFlagDiff = false;
    }

    for(int i=getIdxFindCmp(); i<MAX_CMD_COUNT && i > -1; i--){
        if(c1[i].cmd != c2[i].cmd){
            flagDiff = true;
        }else{
            flagDiff = false;
        }
        if(!prevFlagDiff && flagDiff){
            selectRow(FormCmp, i);
            selectRow(this, i);
            setIdxFindCmp(i);
            break;
        }
        prevFlagDiff = flagDiff;
    }
}

















