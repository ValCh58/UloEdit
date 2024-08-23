#ifndef SCHEMEVIEW_H
#define SCHEMEVIEW_H

#include <QObject>
#include <QtWidgets>

class SchemeAlgo;

class SchemeView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit SchemeView(SchemeAlgo *scheme, QWidget *parent = 0);
    ~SchemeView();


private:
    void setSelectionMode();
    void applyReadOnly();

    SchemeAlgo *scene;

signals:

public slots:
    void adjustSceneRect();
};

#endif // SCHEMEVIEW_H
