#ifndef TABDIAGRAM_H
#define TABDIAGRAM_H

#include <QObject>
#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>

class TabDiagram : public QWidget
{
    Q_OBJECT
public:
    explicit TabDiagram(QWidget *parent = 0);
    ~TabDiagram();

    QTabWidget *getTabDiagram() const;
    void setTabDiagram(QTabWidget *value);

private:
    QTabWidget *tabDiagram;
    QVBoxLayout *vbox;

signals:


public slots:
};

#endif // TABDIAGRAM_H
