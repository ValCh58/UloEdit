#include <QMdiSubWindow>
#include <QAction>
#include <QSettings>
#include <QTableView>
#include "mainwindow.h"
#include "about.h"
#include "tabdiagram.h"

QString MainWindow::cfgDir;

/**
 * @brief MainWindow::MainWindow
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    mdiArea = new QMdiArea;
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    tdiag = nullptr;
    setWindowIcon(QIcon(":/iconsFree/page_gear.png"));
    createActions();
    createMenus();
    useMenuMain();
    //createToolBars();
    createStatusBar();
    cntChild = 0;
    setWindowTitle("Редактор алгоритма управления");
    setUnifiedTitleAndToolBarOnMac(true);
    readSettings();
    tabOpen = false;
}

/**
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{

}

/**
 * @brief MainWindow::createToolBars
 */
void MainWindow::createToolBars(){
    fileToolBar = addToolBar(tr("Файл"));
    fileToolBar->setFixedHeight(26);
    fileToolBar->setStyleSheet("QToolBar { border: 1px solid #aabbaa;background-color: #aabbaa; }");
    fileToolBar->addAction(newAct);
}

/**
 * @brief MainWindow::createStatusBar
 */
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Готов"));
}


/**
 * @brief MainWindow::getFindCompareBack
 * @return
 */
QAction *MainWindow::getFindCompareBack() const
{
    return findCompareBack;
}

/**
 * @brief MainWindow::getFindCompareForward
 * @return
 */
QAction *MainWindow::getFindCompareForward() const
{
    return findCompareForward;
}

/**
 * Проверим нет ли изменений в файлах при закрытии главного окна
 * @brief MainWindow::closeEvent
 * @param e
 */
void MainWindow::closeEvent(QCloseEvent *e)
{
    QList<QMdiSubWindow *>  list = getMdiArea()->subWindowList();
    int sel;

    for(int i = 0; i < list.size(); i++){
      if((list.at(i))->widget()->objectName().indexOf("Diagram")){
          continue;
      }
      child *c = (child*)(list.at(i))->widget();
      sel = c->FormCloseQuery();
      if(sel == QMessageBox::Cancel){
         e->ignore();
         break;
      }
    }

    writeSettings();
    //if(sel != QMessageBox::Cancel)
    e->accept();
}

/**
 * @brief MainWindow::writeSettings
 */
void MainWindow::writeSettings(){
    QSettings settings("NPO EIS", "UloEdit");

    settings.beginGroup("MainWindow");
    settings.setValue("recentFiles", recentFiles);
    settings.setValue("currDirPath", currDirPath);
    settings.endGroup();
}

/**
 * @brief MainWindow::readSettings
 */
void MainWindow::readSettings(){
    QSettings settings("NPO EIS", "UloEdit");

    settings.beginGroup("MainWindow");
    recentFiles = settings.value("recentFiles").toStringList();
    currDirPath = settings.value("currDirPath").toString();
    updateRecentFileItems();
    settings.endGroup();
}

/**
 * @brief MainWindow::getEditMenu
 * @return
 */
QMenu *MainWindow::getEditMenu() const
{
    return editMenu;
}

/**
 * @brief MainWindow::getMenuCompare
 * @return
 */
QMenu *MainWindow::getMenuCompare() const
{
    return menuCompare;
}

/**
 * @brief MainWindow::createMenus
 */
void MainWindow::createMenus(){
    fileMenu = menuBar()->addMenu(tr("Файл"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addAction(closeAct);
    fileMenu->addSeparator();
    resentFiles = new QMenu(tr("Недавние файлы"));
    fileMenu->insertMenu(exitAct, resentFiles);
    fileMenu->insertSeparator(exitAct);
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("Редактировать"));
    editMenu->addAction(cancelAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(replaceAct);
    editMenu->addAction(insAct);
    editMenu->addAction(delAct);
    editMenu->addSeparator();
    editMenu->addAction(selectAllAct);
    editMenu->addAction(insBlankAct);
    editMenu->addAction(diagramFormAct);


    searchMenu = menuBar()->addMenu(tr("Поиск"));
    searchMenu->addAction(searchCommand);
    searchMenu->addSeparator();
    searchMenu->addAction(searchOperand);
    searchMenu->addAction(searchMore);
    searchMenu->addSeparator();
    searchMenu->addAction(findCompareForward);
    searchMenu->addAction(findCompareBack);

    operateMenu = menuBar()->addMenu(tr("Операции"));
    menuCompare = operateMenu->addMenu(QIcon(":/iconsFree/page_find.png"),tr("Сравнить"));
    connect(menuCompare, SIGNAL(aboutToShow()), this, SLOT(NCommandClick()));
    menuCompare->addAction(compare);


    helpMenu = menuBar()->addMenu(tr("Помощь"));
    helpMenu->addAction(about);
}

/**
 * @brief MainWindow::createActions
 */
void MainWindow::createActions(){
    createActionsFileMenu();
    createActionsEditMenu();
    createActionsSearchMenu();
    createActionsCompareMenu();
    createActionsAboutMenu();
    //this->menuBar()->setFixedHeight(26);
    //this->menuBar()->setStyleSheet("QMenuBar { border: 1px solid #aabbaa;background-color: #aabbbb; }");
}

/**
 * @brief MainWindow::loadRecentFile
 * @param str
 * @param ver
 */
void MainWindow::loadRecentFile(QString str, int ver){
    QString file = str + "   ver#"+(QString::number(ver,10));
    if(!recentFiles.contains(file)){
        if(recentFiles.size() > MaxRecentFiles){
           recentFiles.pop_back();
        }
        recentFiles.push_front(file);
    }
}

/**
 * @brief MainWindow::updateRecentFileItems
 */
void MainWindow::updateRecentFileItems()
{
  if(recentFiles.size() > 0){//если история есть//
     clearItemMenuFiles(resentFiles);//resentFiles -> QMenu!!!//
     foreach (QString s, recentFiles) {
       QAction *action = new QAction(s, this);
       resentFiles->insertAction(exitAct,action);
       connect(action, SIGNAL(triggered()), this, SLOT(recentOpenFile()));
     }

  }
}

/**
 * @brief MainWindow::recentOpenFile
 */
void MainWindow::recentOpenFile(){
    QStringList list;

    QAction* a = qobject_cast< QAction* >(sender());
    QString str = a->text();
    int ver = str.at(str.count()-1).Number_DecimalDigit;
    str = str.left(str.length()-8);
    /**Задает каталог для открытия cfg файла БВР!*/
    list = str.split('/');
    if(!list.isEmpty()){
       MainWindow::cfgDir = str;
       MainWindow::cfgDir = MainWindow::cfgDir.left(MainWindow::cfgDir.length()-list.last().length());
    }
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
    openRecentFile(str, ver);
}

/**
 * @brief MainWindow::clearItemMenuFiles
 * @param menu
 */
void MainWindow::clearItemMenuFiles(QMenu *menu)
{
    QList<QAction *> actions = menu->actions();
    if(actions.size() > 0){
        for(int a = 0; a < actions.size(); a++){
            actions.at(a)->deleteLater();
        }
    }
}

/**
 * @brief MainWindow::createActionsAboutMenu
 */
void MainWindow::createActionsAboutMenu(){
    about = new QAction(QIcon(":/iconsFree/ask_and_answer.png"),tr("О программе"), this);
    about->setStatusTip("Сведения о программе");
    connect(about, SIGNAL(triggered()), this, SLOT(aboutEdit()));
}

/**
 * @brief MainWindow::createActionsCompareMenu
 */
void MainWindow::createActionsCompareMenu(){
    compare = new QAction(QIcon(":/iconsFree/page_white.png"),tr("Очистить сравнение"), this);
    compare->setStatusTip("Очистить сравнение");
    connect(compare, SIGNAL(triggered()), this, SLOT(nCmpClearClick()));

    //findCompare = new QAction(QIcon(":/iconsFree/find.png"),tr("Поиск различий"), this);
    //findCompare->setShortcuts(QKeySequence("CTRL+O"));
    //findCompare->setStatusTip("Поиск различий в режиме сравнения файлов");
    //connect(findCompare, SIGNAL(triggered()), this, SLOT(findCmpClick()));


}

/**
 * @brief MainWindow::createActionsSearchMenu
 */
void MainWindow::createActionsSearchMenu(){
    searchCommand = new QAction(QIcon(":/iconsFree/find.png"),tr("Найти команду"), this);
    //searchCommand->setShortcuts(QKeySequence::Find);
    searchCommand->setStatusTip("Найти команду");
    connect(searchCommand, SIGNAL(triggered()), this, SLOT(findCommand()));

    searchOperand = new QAction(QIcon(":/iconsFree/folder_find.png"),tr("Найти операнд"), this);
    searchOperand->setShortcuts(QKeySequence::Find);
    searchOperand->setStatusTip("Найти операнд");
    connect(searchOperand, SIGNAL(triggered()), this, SLOT(findOperand()));

    searchMore = new QAction(QIcon(":/iconsFree/folder_search.png"),tr("Найти далее"), this);
    searchMore->setShortcuts(QKeySequence::FindNext);
    searchMore->setStatusTip("Найти далее");
    connect(searchMore, SIGNAL(triggered()), this, SLOT(nextFindOperand()));

    findCompareForward = new QAction(QIcon(":/iconsFree/folder_search.png"),tr("Найти несовпадения вперед   Alt+F"), this);
    findCompareForward->setStatusTip("Найти несовпадения вперед");
    connect(findCompareForward, SIGNAL(triggered()), this, SLOT(nextFindCmpFrw()));

    findCompareBack = new QAction(QIcon(":/iconsFree/folder_search.png"),tr("Найти несовпадения назад   Alt+D"), this);
    findCompareBack->setStatusTip("Найти несовпадения назад");
    connect(findCompareBack, SIGNAL(triggered()), this, SLOT(nextFindCmpBck()));

}

/**
 * @brief MainWindow::createActionsEditMenu
 */
void MainWindow::createActionsEditMenu(){
    cancelAct = new QAction(QIcon(":/iconsFree/table_refresh.png"),tr("Отмена"), this);
    cancelAct->setShortcuts(QKeySequence::Undo);
    cancelAct->setStatusTip(tr("Отменить изменения"));
    connect(cancelAct, SIGNAL(triggered()), this, SLOT(UndoClick()));

    cutAct = new QAction(QIcon(":/iconsFree/table_select.png"),tr("Вырезать"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip("Вырезать");
    connect(cutAct, SIGNAL(triggered()), this, SLOT(cutLinesAndCopy()));

    copyAct = new QAction(QIcon(":/iconsFree/table_multiple.png"),tr("Копировать"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip("Копировать");
    connect(copyAct, SIGNAL(triggered()), this, SLOT(copyRow()));

    replaceAct = new QAction(QIcon(":/iconsFree/table_replace.png"),tr("Заменить"), this);
    replaceAct->setShortcut(QKeySequence::Replace);
    replaceAct->setStatusTip("Замена в строке");
    connect(replaceAct, SIGNAL(triggered()), this, SLOT(replaceRow()));

    insAct = new QAction(QIcon(":/iconsFree/table_row_insert.png"),tr("Вставить"), this);
    insAct->setShortcuts(QKeySequence::Paste);
    insAct->setStatusTip("Вставить");
    connect(insAct, SIGNAL(triggered()), this, SLOT(addLines()));

    delAct = new QAction(QIcon(":/iconsFree/table_row_delete.png"),tr("Удалить"), this);
    delAct->setShortcuts(QKeySequence::Delete);
    delAct->setStatusTip("Удалить");
    connect(delAct, SIGNAL(triggered()), this, SLOT(delLines()));

    selectAllAct = new QAction(QIcon(":/iconsFree/table_select_all.png"),tr("Выделить все"), this);
    selectAllAct->setShortcuts(QKeySequence::SelectAll);
    selectAllAct->setStatusTip("Выбрать все");
    connect(selectAllAct, SIGNAL(triggered()), this, SLOT(selectAllLines()));

    insBlankAct = new QAction(QIcon(":/iconsFree/table_select_row.png"),tr("Добавить пустую строку"), this);
    insBlankAct->setShortcuts(QKeySequence::InsertLineSeparator);
    insBlankAct->setStatusTip("Вставить пустую строку");
    connect(insBlankAct, SIGNAL(triggered()), this, SLOT(insBlankLines()));

    diagramFormAct = new QAction(QIcon(":/iconsFree/table_chart.png"),tr("Построить схему алгоритма"), this);
    diagramFormAct->setStatusTip("Построить схему алгоритма");
    connect(diagramFormAct, SIGNAL(triggered()), this, SLOT(createTabDiag()));
}

/**
 * @brief MainWindow::createActionsFileMenu
 */
void MainWindow::createActionsFileMenu(){
    newAct = new QAction(QIcon(":/iconsFree/page.png"), tr("Создать"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Создать новый файл"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(nNewClick()));
    
    openAct = new QAction(QIcon(":/iconsFree/page_edit.png"), tr("Открыть"), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Открыть существующий файл"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(nOpenClick()));
    
    saveAct = new QAction(QIcon(":/iconsFree/page_save.png"), tr("Сохранить"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Сохранить файл на диск"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(nSaveClick()));
    
    saveAsAct = new QAction(QIcon(":/iconsFree/page_white_stack.png"),tr("Сохранить как ..."), this);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Сохранить файл под новым именем"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAsFile()));

    closeAct = new QAction(QIcon(":/iconsFree/page_white_tux.png"),tr("Закрыть"), this);
    closeAct->setShortcuts(QKeySequence::Close);
    closeAct->setStatusTip(tr("Закрыть редактируемый файл"));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(nCloseClick()));

    exitAct = new QAction(QIcon(":/iconsFree/door_in.png"),tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Завершить программу"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));
}

/**
 * @brief MainWindow::getIndexFilter
 * @param strFilter
 * @return
 */
int MainWindow::getIndexFilter(QString strFilter)
{
    int retVal = 0;

    if(strFilter.contains("1.0"))
       retVal = 1;
    else if(strFilter.contains("2.0"))
       retVal = 2;
    else if(strFilter.contains("3.0"))
       retVal = 3;

    return retVal;
}

/**
 * @brief MainWindow::getCntChild
 * @return
 */
int MainWindow::getCntChild() const
{
    return cntChild;
}

/**
 * @brief MainWindow::setCntChild
 * @param value
 */
void MainWindow::setCntChild(int value)
{
    cntChild = value;
}

/**
 * Возврат в начальное меню
 * @brief MainWindow::useMenuMain
 */
void MainWindow::useMenuMain()
{
  fileMenu->menuAction()->setVisible(true);
  helpMenu->menuAction()->setVisible(true);
  /////////////////////////////////////////
  editMenu->menuAction()->setVisible(false);
  searchMenu->menuAction()->setVisible(false);
  operateMenu->menuAction()->setVisible(false);
  menuCompare->menuAction()->setVisible(false);
  ////////////////////////////////////////////
  saveAct->setDisabled(true);
  saveAsAct->setDisabled(true);
  closeAct->setDisabled(true);
}

/**
 * Отображение полного меню
 * @brief MainWindow::useMenuChild
 */
void MainWindow::useMenuChild()
{
    fileMenu->menuAction()->setVisible(true);
    editMenu->menuAction()->setVisible(true);
    searchMenu->menuAction()->setVisible(true);
    operateMenu->menuAction()->setVisible(true);
    menuCompare->menuAction()->setVisible(true);
    helpMenu->menuAction()->setVisible(true);
    ///////////////////////////////////////////
    saveAct->setDisabled(false);
    saveAsAct->setDisabled(false);
    closeAct->setDisabled(false);
    setAnableItemMenu(cancelAct, true);
}

/**
 * @brief MainWindow::getCancelAct
 * @return
 */
QAction *MainWindow::getCancelAct() const
{
    return cancelAct;
}

/**
 * @brief MainWindow::setAnableItemMenu
 * @param item
 * @param flag
 */
void MainWindow::setAnableItemMenu(QAction *item, bool flag){
     item->setDisabled(flag);
}

/**
 * @brief MainWindow::nNewClick
 */
void MainWindow::nNewClick(){
    child *chld = new child(this);
    mdiArea->addSubWindow(chld)->setMinimumSize(520, 439);
    chld->greateNewFile();
    chld->show();
    chld->selectRow(chld,0);
    chld->setFocusPolicy(Qt::WheelFocus);
}

/**
 * @brief MainWindow::aboutEdit
 */
void MainWindow::aboutEdit(){
    About *ab = new About(this);
    ab->show();

}

/**
 * @brief MainWindow::copyRow
 */
void MainWindow::copyRow(){
    child *chld = (child *)mdiArea->activeSubWindow()->widget();
    chld->CopyLineClick();
}

/**
 * @brief MainWindow::addLines
 */
void MainWindow::addLines()
{
    child *chld = (child *)mdiArea->activeSubWindow()->widget();
    chld->AddLine();
}

/**
 * @brief MainWindow::insBlankLines
 */
void MainWindow::insBlankLines()
{
    child *chld = (child *)mdiArea->activeSubWindow()->widget();
    chld->InsEmptyLine();
}

/**
 * @brief MainWindow::cutLinesAndCopy
 */
void MainWindow::cutLinesAndCopy()
{
    child *chld = (child *)mdiArea->activeSubWindow()->widget();
    chld->CutLines();
}

/**
 * @brief MainWindow::delLines
 */
void MainWindow::delLines()
{
    child *chld = (child *)mdiArea->activeSubWindow()->widget();
    chld->DelLines();
}

/**
 * @brief MainWindow::selectAllLines
 */
void MainWindow::selectAllLines()
{
    child *chld = (child *)mdiArea->activeSubWindow()->widget();
    chld->SelectAllLines();
}

/**
 * @brief MainWindow::UndoClick
 */
void MainWindow::UndoClick()
{
    child *chld = (child *)mdiArea->activeSubWindow()->widget();
    chld->UndoChange();
}

/**
 * @brief MainWindow::findCommand
 */
void MainWindow::findCommand()
{
    child *chld = (child *)mdiArea->activeSubWindow()->widget();
    chld->FindCommand();
}

/**
 * @brief MainWindow::findOperand
 */
void MainWindow::findOperand()
{
    child *chld = (child *)mdiArea->activeSubWindow()->widget();
    chld->FindOperand();
}

/**
 * @brief MainWindow::nextFindOperand
 */
void MainWindow::nextFindOperand(){
    child *chld = (child *)mdiArea->activeSubWindow()->widget();
    chld->NextFindOperand();
}

/**
 * @brief MainWindow::nextFindCmpFrw
 */
void MainWindow::nextFindCmpFrw(){
    child *chld = (child *)mdiArea->activeSubWindow()->widget();
    chld->findCmpClickFront();
}

/**
 * @brief MainWindow::nextFindCmpBck
 */
void MainWindow::nextFindCmpBck(){
    child *chld = (child *)mdiArea->activeSubWindow()->widget();
    chld->findCmpClickBack();
}

/**
 * Создание формы вкладок и
 * построения диаграм.
 * @brief MainWindow::createTabDiag
 */
void MainWindow::createTabDiag()
{
     QTableView *view = ((child *)mdiArea->activeSubWindow()->widget())->getView();
     QSize sz;
     QString *title = new QString();

     if(!tabOpen){
         tabOpen = true;
         QSize size(520, 439);
         tdiag = new TabDiagram(this);
         tdiag->setWindowTitle("Схема алгоритма");
         Diagram *diag = makeDiagram(view, &sz, title);
         if(chekCreateDiagram(diag, tdiag, title, view)){
             return;
         }
         tdiag->getTabDiagram()->addTab(diag, QIcon(":/iconsFree/recycle.png"), *title);
         QSize mainSize = this->size();
         if((mainSize.width() - sz.width()) < 100){
            mainSize.setWidth(mainSize.width()+sz.width()*0.617);
            resize(mainSize);
         }
         QRect rectView = view->geometry();
         mdiArea->addSubWindow(tdiag)->setGeometry(rectView.width()+53, rectView.y()-23, sz.width()*0.617, size.height());
         tdiag->show();
         this->setFocusProxy(tdiag);
     }else{
         Diagram *diag = makeDiagram(view, &sz, title);
         if(chekCreateDiagram(diag, tdiag, title, view)){
             return;
         }
         tdiag->getTabDiagram()->addTab(diag, QIcon(":/iconsFree/recycle.png"), *title);
         /** Установим выбранную вкладку активной */
         tdiag->getTabDiagram()->setCurrentIndex(tdiag->getTabDiagram()->count()-1);
     }
}

/**
 * Failure to create 'Diagram *diag'
 * @brief MainWindow::chekCreateDiagram
 * @param diag
 * @return bool
 */
bool MainWindow::chekCreateDiagram(Diagram *diag, TabDiagram *tdiag, QString *title, QTableView *view){
    bool retVal = false;
    if(!diag){
        if(tdiag){delete tdiag;}
        if(title){delete title;}
        if(view){delete view;}
        QMessageBox::critical(this,"Ошибка", "Вкладка схемы не создана.", QMessageBox::Ok, 0 );
        retVal = true;
    }
    return retVal;
}

/**
 * Флаг для управления созданием формы вкладок
 * @brief MainWindow::delTabDiagram
 */
void MainWindow::delTabDiagram()
{
     tabOpen = false;
}

/**
 * Вкладка формы с диаграмой
 * @brief MainWindow::makeDiagram
 * @param view
 * @param sz
 * @param strTitle
 * @return
 */
Diagram* MainWindow::makeDiagram(QTableView *view, QSize *sz, QString *strTitle)
{
    int firstRow = 0;
    int numRow = -1;
    Diagram *diagram = nullptr;
   /** Если строки алгоритма выбраны передадим их в конструктор */
   QItemSelectionModel *selectionModel = view->selectionModel();
   QModelIndexList selRow = selectionModel->selectedRows();
   firstRow = selRow.at(0).row();/** Номер первой выделенной строки */
   numRow   = selRow.size(); /** Кол. выделенных строк  */
   //numRow = 0;//!!!!!Debug!!!!!!!!//
   if(firstRow >= 0 && numRow > 0){
      diagram = new Diagram(view, firstRow, numRow,this);
      diagram->parserItemAlgo(sz, strTitle);
      diagram->printGroupAlgo();
   }

   return diagram;
}

/** +++++++++++++++++++++Схема алгоритма++++++++++++++++++++ */

/**
 * @brief MainWindow::NCommandClick
 */
void MainWindow::NCommandClick()
{
    child *chld = (child *)mdiArea->activeSubWindow()->widget();
    chld->nCommandClick();
}

/**
 * @brief MainWindow::nCmpClearClick
 */
void MainWindow::nCmpClearClick()
{
    child *chld = (child *)mdiArea->activeSubWindow()->widget();
    chld->nCmpClearClick();
}

/**
 * @brief MainWindow::getMdiArea
 * @return
 */
QMdiArea *MainWindow::getMdiArea() const
{
    return mdiArea;
}

/**
 * Открыть существующий файл *.bin
 * @brief MainWindow::nOpenClick
 */
void  MainWindow::nOpenClick(){
    QString ver = 0;
    QString titleDialog = "Выберите файл";
    if(currDirPath.isEmpty()){
       currDirPath = QDir::currentPath();
    }

    dOpen.setLabelText(QFileDialog::Reject, "Отменить");
    dOpen.setLabelText(QFileDialog::Accept, "Выбрать");
    dOpen.setWindowTitle(titleDialog);
    dOpen.setDirectory(currDirPath);
    dOpen.setViewMode(QFileDialog::List);
    dOpen.setNameFilter(tr("Файл управления 3.0 (*.bin);;Файл управления 2.0 (*.bin);;Файл управления 1.0 (*.bin)"));

    if (dOpen.exec()) {
       QStringList fileNames = dOpen.selectedFiles();
       //namFile = fileNames.at(0);
       int ver = getIndexFilter(dOpen.selectedNameFilter());
       child *chld = new child(this);
       chld->dSave.setDirectory(currDirPath);
       if(!chld->Open(fileNames.at(0), ver)){
           chld->removeMenu();
           return;
       }
       chld->setFVersion(ver);
       QSize size(520, 439);
       QSize mainSize = this->size();
       if(cntChild > 1 && cntChild <4 && mainSize.width() < (size.width()*cntChild)){
          mainSize.setWidth(mainSize.width()+size.width());
          resize(mainSize);
       }
       mdiArea->addSubWindow(chld)->setMinimumSize(size);
       chld->show();
       chld->selectRow(chld,0);
       //chld->setFocusPolicy(Qt::WheelFocus);
       this->setFocusProxy(chld);
       //======================================
       loadRecentFile(fileNames.at(0),ver);
       updateRecentFileItems();
       //======================================
    }
    /**Сохраним текущий путь к файлам*/
    currDirPath = dOpen.directory().absolutePath();
}

/**
 * @brief MainWindow::openRecentFile
 * @param file
 * @param ver
 */
void MainWindow::openRecentFile(QString file, int ver){

    child *chld = new child(this);
    if(!chld->Open(file, ver)){
        chld->removeMenu();
        return;
    }
    QSize size(520, 439);
    QSize mainSize = this->size();
    if(cntChild > 1 && cntChild <4 && mainSize.width() < (size.width()*cntChild)){
       mainSize.setWidth(mainSize.width()+size.width());
       resize(mainSize);
    }
    mdiArea->addSubWindow(chld)->setMinimumSize(size);
    chld->show();
    chld->selectRow(chld,0);
    //chld->setFocusPolicy(Qt::WheelFocus);
    this->setFocusProxy(chld);

}

/**
 * @brief MainWindow::nCloseClick
 */
void MainWindow::nCloseClick()
{
    if(((child *)mdiArea->activeSubWindow()->widget())->FormCloseQuery() != QMessageBox::Cancel){
        mdiArea->activeSubWindow()->deleteLater();
        //closeResize();
    }
}

/**
 * @brief MainWindow::closeResize
 */
void MainWindow::closeResize(){
    if(cntChild > 1 && cntChild <4){
       QSize size(520, 439);
       QSize mainSize = this->size();
       mainSize.setWidth(mainSize.width()-size.width());
       resize(mainSize);
    }
}

/**
 * @brief MainWindow::replaceRow
 */
void MainWindow::replaceRow(){
    child *ch = (child *)mdiArea->activeSubWindow()->widget();
    ch->replaceLine();
}

/**
 * @brief MainWindow::nSaveClick
 */
void MainWindow::nSaveClick()
{
    child *ch = (child *)mdiArea->activeSubWindow()->widget();

    if(ch->getFFileName().isEmpty()){
       saveAsFile();
    }else{
          if(ch->SaveToFile(ch->getFFileName(), ch->getFVersion())){
             ch->setChange(false);
          }
    }
}

/**
 * @brief MainWindow::saveAsFile
 */
void MainWindow::saveAsFile(){
    child *ch = (child *)mdiArea->activeSubWindow()->widget();
    ch->SaveAs();
}
