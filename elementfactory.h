#ifndef ELEMENTFACTORY_H
#define ELEMENTFACTORY_H

#include <QMutex>

class CustomElement;
class QGraphicsItem;

class ElementFactory
{
public:
    // return instance of factory
    static ElementFactory* Instance() {
    static QMutex mutex;
    if (!factory_) {
        mutex.lock();
        if (!factory_) factory_ = new ElementFactory();
        mutex.unlock();
    }
    return factory_;
    }

    // delete the instance of factory
    static void dropInstance () {
    static QMutex mutex;
    if (factory_) {
        mutex.lock();
        delete factory_;
        factory_ = 0;
        mutex.unlock();
    }
    }

private:
    static ElementFactory* factory_;
    ElementFactory();
    ~ElementFactory();
public:
    CustomElement * createElement (int type, QGraphicsItem * = 0);

};

#endif // ELEMENTFACTORY_H
