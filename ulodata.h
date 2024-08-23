#ifndef ULODATA_H
#define ULODATA_H

#include <QString>

class UloData
{

public:
    UloData();
    ~UloData();

    QString getNumCommandHex() const;
    void setNumCommandHex(QString value);

    QString getCodHex() const;
    void setCodHex(QString value);

    QString getCodOper() const;
    void setCodOper(QString value);

    QString getLogCellCommand() const;
    void setLogCellCommand(QString value);

    QString getOperandCommand() const;
    void setOperandCommand(QString value);

    QString getOperCommand() const;
    void setOperCommand(const QString &value);

    bool operator==(UloData d){
        return this->getNumCommandHex()==d.getNumCommandHex();
    }

private:
    QString numCommandHex;
    QString codHex;
    QString codOper;
    QString logCellCommand;
    QString operandCommand;
    QString operCommand;

};

#endif // ULODATA_H
