#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMdiArea>
#include <QMenu>
#include <QMenuBar>
#include <QApplication>
#include <QStatusBar>
#include <QToolBar>
#include <QFileDialog>
#include <QDir>
#include <QShortcut>

#include "child.h"
#include "diagram.h"

class TabDiagram;
class child;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static QString cfgDir;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void createMainMenu();
    void createChildMenu();
    void useMenuMain();
    void useMenuChild();
    int getIndexFilter(QString strFilter);

    int getCntChild() const;
    void setCntChild(int value);
    void setAnableItemMenu(QAction *item, bool flag);

    QAction *getCancelAct() const;
    void setCancelAct(QAction *value);

    QMdiArea *getMdiArea() const;
    void setMdiArea(QMdiArea *value);

    QMenu *getMenuCompare() const;
    void setMenuCompare(QMenu *value);

    QMenu *getEditMenu() const;
    void setEditMenu(QMenu *value);
    void closeResize();

    QAction *getFindCompareForward() const;
    void setFindCompareForward(QAction *value);

    QAction *getFindCompareBack() const;
    void setFindCompareBack(QAction *value);

private slots:
    void nOpenClick();
    void nCloseClick();
    void nSaveClick();
    void saveAsFile();
    void nNewClick();
    void copyRow();
    void addLines();
    void insBlankLines();
    void cutLinesAndCopy();
    void delLines();
    void selectAllLines();
    void UndoClick();
    void findCommand();
    void findOperand();
    void nextFindOperand();
    void NCommandClick();
    void nCmpClearClick();
    void aboutEdit();
    void recentOpenFile();
    void replaceRow();
    void nextFindCmpFrw();
    void nextFindCmpBck();
    void createTabDiag();
    void delTabDiagram();

private:
    //Сохранение истории открытия файлов/////////
    QStringList recentFiles;
    void updateRecentFileItems();
    void clearItemMenuFiles(QMenu *menu);
    enum { MaxRecentFiles = 7 };
    QMenu *resentFiles;
    void openRecentFile(QString file, int ver);
    void loadRecentFile(QString str, int ver);
    //--------------------------------------------
    //Управление сохранением состояния программы//
    void writeSettings();
    void readSettings();
    //--------------------------------------------
    QMdiArea *mdiArea;
    void createActions();
    void createActionsFileMenu();
    void createActionsEditMenu();
    void createActionsSearchMenu();
    void createActionsCompareMenu();
    void createActionsAboutMenu();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    QFileDialog dOpen;
    QString currDirPath;
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *searchMenu;
    QMenu *operateMenu; QMenu *menuCompare;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    //menuFile/////////
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *closeAct;
    QAction *exitAct;
    //menuEdit/////////
    QAction *cancelAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *replaceAct;
    QAction *insAct;
    QAction *delAct;
    QAction *selectAllAct;
    QAction *insBlankAct;
    QAction *diagramFormAct;
    //menuSearch//////
    QAction *searchCommand;
    QAction *searchOperand;
    QAction *searchMore;
    QAction *findCompareForward;
    QAction *findCompareBack;
    //menuCompare/////
    QAction *compare;
    //menuAbout///////
    QAction *about;
    // //Счетчик форм child//
    int cntChild;
    TabDiagram *tdiag;
    bool tabOpen;
    Diagram *makeDiagram(QTableView *view, QSize *sz, QString *strTitle);
    bool chekCreateDiagram(Diagram *diag, TabDiagram *tdiag, QString *title, QTableView *view);

protected:
    virtual void closeEvent(QCloseEvent *e);

};

#endif // MAINWINDOW_H
