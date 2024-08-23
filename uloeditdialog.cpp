#include "uloeditdialog.h"

UloEditDialog::UloEditDialog(QWidget *parent, Qt::WindowFlags f, UloData * data):QDialog(parent,f)
{
    setModal(true);
    labCodeHex = new QLabel(tr("Код HEX:"));
    QString str = data->getCodHex();
    labValCodeHex = new QLabel(str);
    labOperation = new QLabel("Операция:");
    comboOperation = new QComboBox();

    labLogCell = new QLabel("Логическая ячейка:");
    valLogCell = new QSpinBox();
    valLogCell->setMaximum(3);
    valLogCell->setMinimum(0);
    labOperand = new QLabel("Операнд:");
    valOperand = new QLineEdit();

    formGroupBox = new QGroupBox();
    QFormLayout *topLeftLayout = new QFormLayout;
    topLeftLayout->addRow(labCodeHex,labValCodeHex);
    topLeftLayout->addRow(labOperation, comboOperation);
    topLeftLayout->addRow(labLogCell, valLogCell);
    topLeftLayout->addRow(labOperand, valOperand);
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
    setWindowTitle(tr("Изменение команды"));
    setFixedHeight(sizeHint().height());
    setFixedWidth(sizeHint().width());

}

UloEditDialog::~UloEditDialog()
{

}

