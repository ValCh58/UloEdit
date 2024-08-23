#ifndef ULOEDITDIALOG_H
#define ULOEDITDIALOG_H

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
#include "ulodata.h"




class UloEditDialog : public QDialog
{
    Q_OBJECT

public:
    UloEditDialog(QWidget * parent = 0, Qt::WindowFlags f = 0, UloData *data=0);
    ~UloEditDialog();

private:
    QGroupBox *formGroupBox;
     QLabel *labCodeHex;
     QLabel *labValCodeHex;
     QLabel *labOperation;
     QComboBox *comboOperation;
     QLabel *labLogCell;
     QSpinBox *valLogCell;
     QLabel *labOperand;
     QLineEdit *valOperand;
    QGroupBox *formButton;
     QPushButton *save;
     QPushButton *cancel;


};

#endif // ULOEDITDIALOG_H
