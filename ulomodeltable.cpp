#include <cstddef>
#include <Qt>
//#include <QDebug>

#include "ulomodeltable.h"
#include "child.h"


UloModelTable::UloModelTable(QObject *parent, int colCnt, int *CmpFlg) : QAbstractTableModel(parent)
{
    listOfUloData = new QList<UloData>();
    setColumns(colCnt);
    FCmpFlg = CmpFlg;
}



int UloModelTable::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return getColumns();
}


int UloModelTable::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return listOfUloData->size();
}



QVariant UloModelTable::data(const QModelIndex &index, int role) const
{
    QVariant value;

    switch(role){

    case Qt::DisplayRole: // Данные для отображения
    case Qt::EditRole: {   // Данные для редактирования
        if (index.isValid() && role == Qt::DisplayRole)
            return getData(index.row(), index.column());
     }

    case Qt::TextAlignmentRole: // Выравнивание
        return int(Qt::AlignHCenter | Qt::AlignVCenter);

    case Qt::BackgroundColorRole:{   // Цвет фона
        if(FCmpFlg[index.row()]&CMP_FLG_NOEQ){
           return qVariantFromValue(QColor(255, 48, 48));
        }
        if(index.row() % 2){
           return qVariantFromValue(QColor(255, 250, 250));
        }else{
           return qVariantFromValue(QColor(238, 233, 233));
        }
     }

    case Qt::FontRole:{ // Шрифт
         QFont font = QFont("MS Sans Serif", -11, QFont::StyleItalic);
         return qVariantFromValue(font);

     }

    case Qt::TextColorRole:{
            if(FCmpFlg[index.row()]&CMP_FLG_NOEQ){
               return qVariantFromValue(QColor(255, 255, 248));
            }else{
               return qVariantFromValue(QColor(0, 0, 0));
            }
        }
    }

    return value;
}


QVariant UloModelTable::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant value = QVariant();

    if (role != Qt::DisplayRole)
        return value;

    if (orientation == Qt::Vertical){
        char str[10];
        sprintf(str, "%04X  ", section);
        value = QVariant(str);
    }
    else
        switch (section) {

            case 0:
                return QVariant("Команда");
            case 1:
                return QVariant("Код HEX");
            case 2:
                return QVariant("Код опер.");
            case 3:
                return QVariant("ЛЯ");
            case 4:
                return QVariant("Операнд");
            case 5:
                return QVariant("Операция");
            default:
                return QVariant();
        }

    return value;
}



Qt::ItemFlags UloModelTable::flags(const QModelIndex &index) const
{
    if (!index.isValid())
            return Qt::ItemIsEnabled;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}


void UloModelTable::addCommand(UloData &newUloData)
{
     beginInsertRows(QModelIndex(), listOfUloData->size(), listOfUloData->size());
     listOfUloData->append(newUloData);
     endInsertRows();
}


void UloModelTable::delCommand(const QModelIndex &index)
{
     beginRemoveRows(QModelIndex(), index.row(), index.row());
     listOfUloData->removeAt(index.row());
     endRemoveRows();
}


UloData &UloModelTable::getUloData(int index) const
{
     return (*listOfUloData)[index];
}


QVariant UloModelTable::getData(int num, int position) const
{
    switch (position) {
            case 0:
                return QVariant(listOfUloData->at(num).getNumCommandHex());
            case 1:
                return QVariant(listOfUloData->at(num).getCodHex());
            case 2:
                return QVariant(listOfUloData->at(num).getCodOper());
            case 3:
                return QVariant(listOfUloData->at(num).getLogCellCommand());
            case 4:
                return QVariant(listOfUloData->at(num).getOperandCommand());
            case 5:
                return QVariant(listOfUloData->at(num).getOperCommand());
            default:
                return QVariant();
    }

}
int UloModelTable::getColumns() const
{
    return columns;
}

void UloModelTable::setColumns(int value)
{
    columns = value;
}

void UloModelTable::setDataToModel(cmd_t fcmd[], int cnt)
{
  cmd_t *cm;
  UloData data;
  char str[128];

  listOfUloData->clear();
  beginResetModel();
  for(int i=0; i < cnt; i++){
   cm = &fcmd[i];
   str[0]=0;
   sprintf(str, "%04X", i);
   data.setNumCommandHex(str);
   str[0]=0;
   sprintf(str, "%04X", *(WORD*)cm);
   data.setCodHex(str);
   str[0]=0;
   sprintf(str, "%d", cm->ncmd.opcode);
   data.setCodOper(str);
   str[0]=0;
   sprintf(str, "%d", cm->ncmd.nl);
   data.setLogCellCommand(str);
   str[0]=0;
   sprintf(str, "%03X", cm->ncmd.addr);
   data.setOperandCommand(str);
   str[0]=0;
   if(cm->ncmd.opcode < CNT_OPERS)
       sprintf(str, "%s", OpNames[cm->ncmd.opcode]);
   else
       sprintf(str, " ? операнд %d", cm->ncmd.opcode);
   data.setOperCommand(str);
   listOfUloData->append(data);
  }
  endResetModel();

}

void UloModelTable::updateRowModel(cmd_t *de, int id)
{
    char str[128];
    //cmd_t *cm = &so[id.row()];
    cmd_t *cm = de;
    UloData &data = getUloData(id);

    str[0]=0;
    sprintf(str, "%04X", *(WORD*)cm);
    data.setCodHex(str);
    str[0]=0;
    sprintf(str, "%d", cm->ncmd.opcode);
    data.setCodOper(str);
    str[0]=0;
    sprintf(str, "%d", cm->ncmd.nl);
    data.setLogCellCommand(str);
    str[0]=0;
    sprintf(str, "%03X", cm->ncmd.addr);
    data.setOperandCommand(str);
    str[0]=0;
    if(cm->ncmd.opcode < CNT_OPERS)
        sprintf(str, "%s", OpNames[cm->ncmd.opcode]);
    else
        sprintf(str, " ? операнд %d", cm->ncmd.opcode);
    data.setOperCommand(str);
}



