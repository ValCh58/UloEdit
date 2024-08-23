#ifndef CMDEDIT_H
#define CMDEDIT_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QSpinBox>
#include <QTextStream>
#include <QFont>

#include "ulodata.h"
#ifdef _WIN32
 #include "windef.h"
#elif __unix__
 typedef unsigned short WORD;
 typedef unsigned int DWORD;
 typedef unsigned char BYTE;
#else
 #error Unsupported Operating System
#endif




/**
 * Количество операций КОП
 */
#define CNT_OPERS		8

/**
 * тип команды управления массив ПЛО
 */
typedef struct{
    union{
        WORD cmd;
        struct{
            WORD code:8;
            WORD a8:1;
            WORD nb:2;
            WORD nl:2;
            WORD opcode :3;
        }xcmd;
        struct{
            WORD addr:11;
            WORD nl:2;
            WORD opcode :3;
        }ncmd;
    };
}cmd_t;  /** <cmd_t> size = 2 bytes */



class cmdedit : public QDialog
{
    Q_OBJECT

public:
    cmdedit(QWidget * parent = 0, Qt::WindowFlags f = 0, UloData *data=0);
    ~cmdedit();
    cmd_t TmpCmd;
    void dCmdEdit(QModelIndex ix, cmd_t *cmd);

private:
    QGroupBox *formGroupBox;
    QLabel *labCodeHex;
    QLabel *lCode;
    QLabel *labOperation;
    QComboBox *cbOper;
    QLabel *labLogCell;
    QSpinBox *upLja;
    QLabel *labOperand;
    QLineEdit *eAddr;
    QGroupBox *formButton;
    QPushButton *save;
    QPushButton *cancel;
    void getInitList(int);
    void Update();
    bool FStart;
    QModelIndex index;

private slots:
    void saveOK();
    void cbOperChange();
    void eLjaChange();
    void eAddrChange();
    void eAddrExit();

};
extern char *OpNames[CNT_OPERS];
#endif // CMDEDIT_H
