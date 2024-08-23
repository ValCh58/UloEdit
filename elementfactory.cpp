#include "elementfactory.h"
#include "customelement.h"

CustomElement *ElementFactory::createElement(int type, QGraphicsItem *)
{
    switch(type){
    case TYPE_AND:
        //Элемент И//
        break;
    case TYPE_OR:
        //Элемент ИЛИ//
        break;
    case TYPE_TG:
        //Элемент Триггер//
        break;
    case TYPE_UNI:
        //Элемент Универсальный//
        break;
    }

    return nullptr;
}

