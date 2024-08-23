#include "unielement_lf_e8.h"

UniElement_LF_E8::UniElement_LF_E8(QRectF rect, int logo, QPointF point, QGraphicsItem *qgi):AndElement(rect, point, qgi)
{
    typeEl = UNI_2_3;//Установим тип элемента//

    if(logo == 0xE8){
       logoType = "2/3";
    }
    else{
       logoType = "*";
    }
}

UniElement_LF_E8::~UniElement_LF_E8(){}

void UniElement_LF_E8::buildElement(DataGraph &dtg)
{
    QPainter qp;
    qp.begin(&drawing);
    makeAndElement(qp);// !!!test!!!
    adjustTermElement(qp,dtg);
    qp.end();
}

void UniElement_LF_E8::makeUniElement(QPainter &qp)
{

}
