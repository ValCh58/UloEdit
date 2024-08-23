#ifndef CHILD_H
#define CHILD_H

#include <QWidget>
#include <QFileDialog>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QTableView>
#include <QPushButton>
#include <QMessageBox>
#include <QStatusBar>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QMainWindow>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QList>
#include <QModelIndexList>
#include <QModelIndex>
#include <QSizePolicy>
#include <QHeaderView>
#include <QCloseEvent>
#include <deque>

#include "mainwindow.h"
#include "ulomodeltable.h"
#include "cmdedit.h"


//---------------------------------------------------------------------------
#define MAX_CMD_COUNT (65536-6)/2
//---------------------------------------------------------------------------
#define CMP_FLG_NOEQ  0x00000001
//---------------------------------------------------------------------------
Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE(std::deque)

// массив команд
typedef cmd_t Cmd[MAX_CMD_COUNT];
// стек массивов команд
typedef std::deque<Cmd*> CmdStack;

class MainWindow;

class child : public QWidget
{
    Q_OBJECT

public:
    child(QWidget *parent = 0);
    ~child();
    QFileDialog dSave;
    bool Open(QString file, int ver);
    void SaveUndo();
    void wrapUpdateRowModel(cmd_t ct, int idx);
    bool SaveToFile(QString fName, int ver);
    void SaveAs();
    void setDataToModel(Cmd fcmd, int cnt);
    QString getFFileName() const;
    void setFFileName(const QString &value);
    int getFVersion() const;
    void setFVersion(int value);
    void removeMenu();
    bool getChange() const;
    void setChange(bool value);
    void CopyLineClick();
    void AddLine();
    void InsEmptyLine();
    void CutLines();
    void DelLines();
    void SelectAllLines();
    void UndoChange();
    void FindCommand();
    void FindOperand();
    void NextFindOperand();
    QTableView *getView() const;
    void setView(QTableView *value);
    UloModelTable *getUloModelTable() const;
    void setUloModelTable(UloModelTable *value);
    void createWindowMessage(const QIcon &icon,QString strTitle, QString winText);
    int getIx() const;
    void setIx(int value);
    long getValNext() const;
    void setValNext(long value);
    void nCommandClick();//Добавление имен файлов в меню сравнения//
    void nCmpClearClick();
    int getFirstRow() const;
    void setFirstRow(int value);
    int getNumRow() const;
    void setNumRow(int value);
    child *getFormCmp() const;
    void setFormCmp(child *value);
    void cmpClickView(QModelIndex index);
    void cmpKeyPressView(QModelIndex index/*,QModelIndex index1*/);
    void selectRow(child *frm, int row);
    void cmpClickSelectView(QModelIndex index, QModelIndex index1);
    void changeCursor(child *frm, QModelIndex idx);
    void eventDbClickedFormEdit(QModelIndex index);
    void showContextMenu(const QPoint &point);
    void updateCompareEdit();
    bool eventFilter(QObject *target, QEvent *event);
    int FormCloseQuery();
    void setConn();
    void setDisConn();
    int lastRowOk;
    int getLastRowOk() const;
    void setLastRowOk(int value);
    void replaceLine();
    void greateNewFile();

    int getIdxFindCmp() const;
    void setIdxFindCmp(int value);

    void findCmpClickFront();
    void findCmpClickBack();

private:
    MainWindow *mwnd;
    QGroupBox *tableGroupBox;
    UloModelTable *uloModelTable;
    QTableView *view;
    void createMenus();
    void headerSize(QTableView * tview);
    void dynResizeTable();
    void PrepareEdit(cmd_t *fcmd);
    void backToLine();
    void endChange(cmd_t *fcmd);
    // буфер обмена//
    Cmd FClipboard;
    int FClipboardCount;
    // информация для восстановления//
    CmdStack FUndo;
    // //
    Cmd FCmd;
    long FCmdSize;
    int FVersion;
    QString FFileName;
    WORD FCmdCRC;
    //
    QStatusBar *sbar;
    QLabel *labelStatus;
    // результат сравнения//
    int FCmpFlg[MAX_CMD_COUNT];
    child * FormCmp;
    // //
    bool Change;
    int firstRow,numRow;
    int firstRowEdit,numRowEdit;
    WORD FFindOper;
    void SetChange(bool p);
    int idxFindCmp;//Номер строки при поиске расхождений//
    int ix;//Номер строки
    long valNext;//и значение для следующего поиска операнда//
    void clearItemMenu(QMenu *menu);//Очистка элементов динамического меню//
    int Compare(child *fmAct, child *fmCmp);
    void deselectRows(child *frm);
    QShortcut *keyCtrlF;  //Сочетания клавиш вперед Ctrl + F
    QShortcut *keyCtrlD;  //Сочетания клавиш назад Ctrl + D
    void refreshFindCmpNo();


protected:
virtual void closeEvent(QCloseEvent *e);


private slots:
    void nCompareClick();
    void eventScrollTable(int UpDown);
    void refreshUndo();




};
Q_DECLARE_METATYPE(QWidget*)

#endif // CHILD_H
