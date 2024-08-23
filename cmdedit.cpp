
#include "child.h"
#include "cmdedit.h"

//------------------------------------------------------------------------------------
char *OpNames[CNT_OPERS] = {(char *)"OR", (char *)"OR-N", (char *)"AND", (char *)"AND-N", (char *)"*", (char *)"OUT", (char *)"INPUT", (char *)"NOP"};
//------------------------------------------------------------------------------------
child *ch;
cmd_t *t_cmd;
cmdedit::cmdedit(QWidget *parent, Qt::WindowFlags f, UloData *data):QDialog(parent,f)
{
    ch = (child*)parent;
    setModal(true);
    labCodeHex = new QLabel(tr("Код HEX:"));
    QString str = data->getCodHex();
    lCode = new QLabel(str);
    lCode->setFont(QFont("Helvetica", 10, QFont::Bold));
    labOperation = new QLabel("Операция:");
    getInitList(data->getCodOper().toInt());

    labLogCell = new QLabel("Логическая ячейка:");
    upLja = new QSpinBox();
    upLja->setMaximum(3);
    upLja->setMinimum(0);
    upLja->setValue(data->getLogCellCommand().toInt());
    labOperand = new QLabel("Операнд:");
    eAddr = new QLineEdit();
    eAddr->setInputMask(">HHH");
    eAddr->setText(data->getOperandCommand());

    formGroupBox = new QGroupBox();
    QFormLayout *topLeftLayout = new QFormLayout;
    topLeftLayout->addRow(labCodeHex,lCode);
    topLeftLayout->addRow(labOperation, cbOper);
    topLeftLayout->addRow(labLogCell, upLja);
    topLeftLayout->addRow(labOperand, eAddr);
    formGroupBox->setLayout(topLeftLayout);

    formButton = new QGroupBox();
    QVBoxLayout *rightLayout = new QVBoxLayout;
    save = new QPushButton("Сохранить");
    cancel = new QPushButton("Отменить");
    rightLayout->addWidget(save);
    rightLayout->addWidget(cancel);
    formButton->setLayout(rightLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(formGroupBox);
    mainLayout->addWidget(formButton);
    setLayout( mainLayout);
    setWindowTitle(tr("Изменение команды ") + data->getNumCommandHex());
    setFixedHeight(sizeHint().height());
    setFixedWidth(sizeHint().width());

    ch->setChange(false);
    FStart = false;
    connect(save, SIGNAL(clicked()), this, SLOT(saveOK()));
    connect(cbOper, SIGNAL(currentIndexChanged(int)), this, SLOT(cbOperChange()));
    connect(upLja, SIGNAL(valueChanged(int)), this, SLOT(eLjaChange()));
    connect(eAddr, SIGNAL(textChanged(QString)), this, SLOT(eAddrChange()));
    connect(eAddr, SIGNAL(editingFinished()), this, SLOT(eAddrExit()));
    connect(cancel, SIGNAL(clicked()), this, SLOT(close()));

}

void cmdedit::getInitList(int idx){
    QStringList list;
    for(int i= 0; i < CNT_OPERS; i++) {
        QString str;
        QTextStream(&str) << "[" << i << "]    " << OpNames[i];//Сборка строки//
        list << str;
    }
    cbOper = new QComboBox();
    cbOper->addItems(list);
    cbOper->setCurrentIndex(idx);//Выбор элемента для отображения//
}

cmdedit::~cmdedit()
{

}

void cmdedit::dCmdEdit(QModelIndex ix, cmd_t *cmd)
{
    index = ix;
    t_cmd = cmd;
    memcpy(&TmpCmd, cmd, sizeof(cmd_t));
}

void cmdedit::Update()
{
    char str[128];

    str[0]=0;
    sprintf(str, "%04X", *((WORD*)&TmpCmd));
    lCode->setText(str);
    cbOper->setCurrentIndex(TmpCmd.ncmd.opcode);
    upLja->setValue(TmpCmd.ncmd.nl);
    str[0]=0;
    sprintf(str, "%03X", TmpCmd.ncmd.addr);
    eAddr->setText(str);
}

void cmdedit::saveOK()
{
    if(ch->getChange()){
       ch->SaveUndo();
       memcpy(t_cmd, &TmpCmd, sizeof(cmd_t));
       ch->wrapUpdateRowModel(*t_cmd, index.row());
       //ch->SaveToFile(ch->getFFileName(), ch->getFVersion());
       ch->setChange(true);
    }
    ch->updateCompareEdit();
    ch->setIdxFindCmp(index.row());
    if(ch->getFormCmp())
       ch->getFormCmp()->setIdxFindCmp(index.row());
    this->close();
}

void cmdedit::cbOperChange()
{
    if(!FStart){
       TmpCmd.ncmd.opcode = cbOper->currentIndex();
       ch->setChange(true);
       Update();
    }
}

void cmdedit::eLjaChange()
{
    if(!FStart){
        TmpCmd.ncmd.nl = upLja->value();
        ch->setChange(true);
        Update();
    }
}

void cmdedit::eAddrChange()
{
    if(!FStart){
        ch->setChange(true);
    }
}

void cmdedit::eAddrExit()
{
    char *endptr, str[64];
    if(ch->getChange()){
       str[0]=0;
       strcat(str, eAddr->displayText().toUtf8());
       TmpCmd.ncmd.addr = strtol(str, &endptr, 16);
       Update();
    }
}


