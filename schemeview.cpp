#include "schemeview.h"
#include "schemealgo.h"

SchemeView::SchemeView(SchemeAlgo *scheme, QWidget *parent) : QGraphicsView(parent),
    scene(scheme)
{
    grabGesture(Qt::PinchGesture);
    setAttribute(Qt::WA_DeleteOnClose, true);
    setInteractive(true);
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::TextAntialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setScene(scene);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setSelectionMode();
    connect(scene, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(adjustSceneRect()));
    adjustSceneRect();

}

SchemeView::~SchemeView()
{

}

void SchemeView::setSelectionMode()
{
    setDragMode(RubberBandDrag);
    setInteractive(true);
    applyReadOnly();
}

void SchemeView::applyReadOnly()
{
    if (!scene) return;

    bool is_writable = true;
    setInteractive(is_writable);
    setAcceptDrops(is_writable);
}

void SchemeView::adjustSceneRect()
{
    QRectF scene_rect = scene->sceneRect();
    scene_rect.adjust(-SchemeAlgo::margin, -SchemeAlgo::margin, SchemeAlgo::margin, SchemeAlgo::margin);
    setSceneRect(scene_rect);
}



