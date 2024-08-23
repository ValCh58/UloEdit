#include "tabdiagram.h"
#include "mainwindow.h"

TabDiagram::TabDiagram(QWidget *parent) : QWidget(parent)
{
  MainWindow *mw = (MainWindow*)parent;
  setAttribute(Qt::WA_DeleteOnClose);
  tabDiagram = new QTabWidget(this);
  vbox = new QVBoxLayout(this);
  vbox->addWidget(tabDiagram);
  setLayout(vbox);

  connect(this, SIGNAL(destroyed()), mw, SLOT(delTabDiagram()));
}

TabDiagram::~TabDiagram()
{
  delete tabDiagram;
  delete vbox;

}
QTabWidget *TabDiagram::getTabDiagram() const
{
    return tabDiagram;
}

void TabDiagram::setTabDiagram(QTabWidget *value)
{
    tabDiagram = value;
}


