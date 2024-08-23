#ifndef ABOUT_H
#define ABOUT_H

#ifdef _WIN32
 #include <QDialog.h>
 #include <QGroupBox.h>
 #include <QFormLayout.h>
 #include <QLabel.h>
 #include <QPushbutton.h>
 #include <QPixmap.h>
#elif __unix__
 #include <qdialog.h>
 #include <qgroupbox.h>
 #include <qformlayout.h>
 #include <qlabel.h>
 #include <qpushbutton.h>
 #include <qpixmap.h>
#else
 #error Unsupported Operating System
#endif


class About : public QDialog
{
public:
    About(QWidget *parent, Qt::WindowFlags f = Qt::CustomizeWindowHint);
    ~About();

private:
QLabel *imageLab;
QLabel *aboutName;
QLabel *ownerName;
QLabel *devName;
QLabel *qtName;



};

#endif // ABOUT_H
