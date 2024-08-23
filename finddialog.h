#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QDialog>
#include <QPushButton>
#include "child.h"


class FindDialog : public QDialog
{
    Q_OBJECT

public:
    FindDialog(QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
    QString getFindText();
    void setFindText(QString str);
    void setFindLabel(QString str);

    QPushButton *getFindButton() const;
    void setFindButton(QPushButton *value);
    child *ch;

protected:
    int createCycleFind(int idx, int cell, long strToHex);
    void clearSelectedRows();
    QString strTitle;
    void setSelectRow(QItemSelectionModel *selectionMod, QModelIndex commandInd, int row);
    QLineEdit *lineEdit;

public slots:
    void findClicked();

private:
    QPushButton *findButton;
    QPushButton *cancelButton;
    QLabel *findLabel;

};

class FindCommandDialog : public FindDialog
{
    Q_OBJECT

public:
    FindCommandDialog(QWidget *parent = 0);


public slots:
    void findClicked();

private:


};

class FindOperDialog : public FindDialog
{
    Q_OBJECT

public:
    FindOperDialog(QWidget *parent = 0);
    void nextFind();


public slots:
    void findClicked();

private:


};


#endif // FINDDIALOG_H
