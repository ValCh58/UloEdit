#include <QItemSelectionModel>
//#include <QDebug>

#include "finddialog.h"

FindDialog::FindDialog(QWidget *parent, Qt::WindowFlags f):QDialog(parent, f)
{
    ch = (child *)parent;
    strTitle = ch->windowTitle();
    findLabel = new QLabel(tr("Название для поиска:"));
    lineEdit = new QLineEdit;
    lineEdit->setInputMask(">HHHH");
    findButton = new QPushButton(tr("Найти"));
    findButton->setDefault(true);
    cancelButton = new QPushButton(tr("Отменить"));

    QHBoxLayout *layoutH = new QHBoxLayout;
    layoutH->addWidget(findLabel);
    layoutH->addWidget(lineEdit);

    QHBoxLayout *layoutH1 = new QHBoxLayout;
    layoutH1->addWidget(findButton);
    layoutH1->addWidget(cancelButton);

    QVBoxLayout *mainLayOut = new QVBoxLayout(this);
    mainLayOut->addLayout(layoutH);
    mainLayOut->addLayout(layoutH1);
    setLayout(mainLayOut);

    setWindowIcon(QIcon(":/iconsFree/find.png"));
    setWindowTitle(tr("Базовый диалог поиска!"));

    connect(findButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

}

QString FindDialog::getFindText()
{
    return lineEdit->text();
}

void FindDialog::setFindText(QString str)
{
    return lineEdit->setText(str);
}


void FindDialog::findClicked(){}

QPushButton *FindDialog::getFindButton() const
{
    return findButton;
}

//Установить название, поля поиска//
void FindDialog::setFindLabel(QString str)
{
    findLabel->setText(str);
}

int FindDialog::createCycleFind(int idx, int cell, long strToHex)
{
    int flagOk = 0;
    bool ok;
    /**Временно отключим сигнал*/
    ch->setDisConn();
    QItemSelectionModel *selectionModel = ch->getView()->selectionModel();
    /**Найдем совпадение в  таблицe*/
    for(int i = idx; i < MAX_CMD_COUNT; i++){
        QModelIndex commandIndex0 = ch->getUloModelTable()->index(i, cell, QModelIndex());
        QString fromTable = commandIndex0.data().toString();
        if(fromTable.toLong(&ok, 16) == strToHex){
            setSelectRow(selectionModel, commandIndex0, i);
            flagOk = commandIndex0.row();
            ch->setLastRowOk(i);
            break;
        }
    }
    QModelIndex commandIndex0 = ch->getUloModelTable()->index(flagOk, cell, QModelIndex());
    setSelectRow(selectionModel, commandIndex0, flagOk);
    ch->setConn();//Включим//
    return flagOk;
}

void FindDialog::setSelectRow(QItemSelectionModel *selectionMod, QModelIndex commandInd, int row){
    selectionMod->setCurrentIndex(commandInd, QItemSelectionModel::Select);
    QModelIndex left = ch->getUloModelTable()->index(row, 0, QModelIndex());//Правый нижний
    QModelIndex right = ch->getUloModelTable()->index(row, 5, QModelIndex());//Правый нижний
    QItemSelection selection(left, right);
    selectionMod->select(selection, QItemSelectionModel::Select);
}


void FindDialog::clearSelectedRows()
{
    QItemSelectionModel *selectionModel = ch->getView()->selectionModel();
    /**Если есть выделенные строки в таблице - очистим их*/
    QModelIndexList indexes = selectionModel->selectedIndexes();
    if(indexes.size() != 0){
       foreach(QModelIndex im, indexes){
            //if(ch->getView()->currentIndex().row() != im.row())
               selectionModel->setCurrentIndex(im, QItemSelectionModel::Deselect);

        }
    }
}

//---------------------------------------------------------------------//
//FindCommandDialog//////////////////////////////////////////////////////
//---------------------------------------------------------------------//
FindCommandDialog::FindCommandDialog(QWidget *parent):FindDialog(parent)
{
    this->setFindLabel(tr("Найти команду:"));
    setWindowTitle(tr("Поиск команды"));

    connect(this->getFindButton(), SIGNAL(clicked()), this, SLOT(findClicked()));
}


void FindCommandDialog::findClicked()
{
    bool ok;

    ch->setDisConn();
    long strToHex = this->getFindText().toLong(&ok, 16);
    this->clearSelectedRows();
    if(this->createCycleFind(0, 0, strToHex) == 0){
        ch->selectRow(ch, ch->getLastRowOk());
        this->ch->createWindowMessage(QIcon(":/iconsFree/find.png"),"Поиск команды","Команда " + this->getFindText() + " не найдена!");
    }
    ch->setConn();
}

//---------------------------------------------------------------------//
//FindOperDialog/////////////////////////////////////////////////////////
//---------------------------------------------------------------------//
FindOperDialog::FindOperDialog(QWidget *parent):FindDialog(parent){

    this->setFindLabel(tr("Найти операнд:"));
    setWindowTitle(tr("Поиск операнда"));
    if(ch->getValNext() > 0){
        QString str;
        this->lineEdit->setText(str.setNum(ch->getValNext(), 16));
        this->lineEdit->selectAll();
    }
    connect(this->getFindButton(), SIGNAL(clicked()), this, SLOT(findClicked()));
}

void FindOperDialog::nextFind()
{
    int i = ch->getIx()+1;
    //bool ok;
    int result = 0;
    ch->setDisConn();
    long strToHex = ch->getValNext();
    this->clearSelectedRows();
    result = this->createCycleFind(i, 4, strToHex);
    if(result == 0){
        ch->selectRow(ch, ch->getLastRowOk());
        this->ch->createWindowMessage(QIcon(":/iconsFree/folder_find.png"),
                                            "Поиск Операнда","Операнд " + QString::number(strToHex, 16).toUpper() + " не найден!");
    }
    ch->setValNext(strToHex);
    ch->setIx(result);
    ch->setFocus();
    ch->update();
    ch->setConn();
}

void FindOperDialog::findClicked()
{
    bool ok;
    int result = 0;
    ch->setDisConn();
    long strToHex = this->getFindText().toLong(&ok, 16);
    this->clearSelectedRows();
    result = this->createCycleFind(0, 4, strToHex);
    if(result == 0){
        this->ch->createWindowMessage(QIcon(":/iconsFree/folder_find.png"),"Поиск Операнда","Операнд " + this->getFindText().toUpper() + " не найдена!");
    }
    ch->setIx(result);
    ch->setValNext(strToHex);
    ch->setConn();
}



