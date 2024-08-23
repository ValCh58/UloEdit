#include "ulodata.h"

UloData::UloData()
{


}

UloData::~UloData()
{

}
QString UloData::getNumCommandHex() const
{
    return numCommandHex;
}

void UloData::setNumCommandHex(QString value)
{
    numCommandHex = value;
}
QString UloData::getCodHex() const
{
    return codHex;
}

void UloData::setCodHex(QString value)
{
    codHex = value;
}
QString UloData::getCodOper() const
{
    return codOper;
}

void UloData::setCodOper(QString value)
{
    codOper = value;
}
QString UloData::getLogCellCommand() const
{
    return logCellCommand;
}

void UloData::setLogCellCommand(QString value)
{
    logCellCommand = value;
}
QString UloData::getOperandCommand() const
{
    return operandCommand;
}

void UloData::setOperandCommand(QString value)
{
    operandCommand = value;
}
QString UloData::getOperCommand() const
{
    return operCommand;
}

void UloData::setOperCommand(const QString &value)
{
    operCommand = value;
}









