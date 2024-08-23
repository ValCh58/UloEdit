#ifndef ULOMODELTABLE
#define ULOMODELTABLE

#include <QAbstractTableModel>
#include <QObject>
#include <QModelIndex>
#include <QVariant>

#include "ulodata.h"
#include "cmdedit.h"

#define MAX_CMD_COUNT (65536-6)/2
#define CMP_FLG_NOEQ  0x00000001

class UloModelTable : public QAbstractTableModel{

    Q_OBJECT

public:
     UloModelTable(QObject *parent, int, int *CmpFlg);
     int columnCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
     int rowCount(const QModelIndex & parent = QModelIndex()) const Q_DECL_OVERRIDE;
     QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
     QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const Q_DECL_OVERRIDE;
     Qt::ItemFlags flags (const QModelIndex & index) const Q_DECL_OVERRIDE;
     void addCommand(UloData& newUloData);
     void delCommand(const QModelIndex & index);
     UloData& getUloData(int index) const;
     int static const ERR_COLUMNCOUNT = -1;

     int getColumns() const;
     void setColumns(int value);
     void setDataToModel(cmd_t fcmd[], int cnt);
     void updateRowModel(cmd_t *de, int id);


private:
     int *FCmpFlg;
     QList<UloData> *listOfUloData;
     QVariant getData(int num, int position) const;
     int columns;

};

#endif // ULOMODELTABLE

