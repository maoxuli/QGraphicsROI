#include "QGraphicsRectSelector.h"
#include "QGraphicsViewZoom.h"
#include "QGraphicsRectObject.h"
#include <QKeyEvent>
#include <QDebug>

QGraphicsRectSelector::QGraphicsRectSelector(QWidget *parent)
    : QGraphicsView(parent)
{
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setDragMode(QGraphicsView::ScrollHandDrag);
    QGraphicsViewZoom* zoom = new QGraphicsViewZoom(this);
    zoom->set_modifiers(Qt::NoModifier);

    // draw rectangle with built-in rubber band operation 
    connect(this, SIGNAL(rubberBandChanged(QRect,QPointF,QPointF)),
            this, SLOT(onRubberBandChanged(QRect,QPointF,QPointF)));

    setScene(&_scene);
    connect(&_scene, &QGraphicsScene::selectionChanged, 
            this, &QGraphicsRectSelector::onSelectionChanged);

    // default scene 
    scene()->addPixmap(QPixmap::fromImage(QImage(1920, 1080, QImage::Format_RGB888)));
    scene()->setSceneRect(QRectF(0, 0, 1920, 1080));
}

QGraphicsRectSelector::~QGraphicsRectSelector()
{

}

// add a rectangle item
void QGraphicsRectSelector::addRectItem(const QRectF& rect)
{
    QGraphicsRectObject* item = new QGraphicsRectObject(rect);
    connect(item, SIGNAL(rectChanged(const QRectF&)), this, SLOT(onRectChanged(const QRectF&)));
    scene()->addItem(item); 
}

// enable drawing rectangle with mouse
void QGraphicsRectSelector::setDrawingMode(bool drawing)
{
    if(drawing) {
        setDragMode(QGraphicsView::RubberBandDrag);
        viewport()->setCursor(Qt::CrossCursor);
    }
    else {
        setDragMode(QGraphicsView::ScrollHandDrag);
    }
}

void QGraphicsRectSelector::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift) {
        setDrawingMode(true);
    }
    else if (event->key() == Qt::Key_Escape) {
        setDrawingMode(false);
    }
    QWidget::keyPressEvent(event);
}

// drag and draw a rectangle
void QGraphicsRectSelector::onRubberBandChanged(QRect rubberBandRect, 
                        QPointF fromScenePoint, QPointF toScenePoint)
{
    static QRectF rect;
    if (rubberBandRect.isEmpty()) { // complete rubber rect
        addRectItem(rect);
        setDrawingMode(false);
    }
    else {
        rect.setTopLeft(fromScenePoint);
        rect.setBottomRight(toScenePoint);
        // qDebug() << "Rubber band rect: " << rubberBandRect; // coords in view
        // qDebug() << "Rubber band from: " << fromScenePoint; // coords in scene
        // qDebug() << "Rubber band to: " << toScenePoint; // coords in scene
    }
}

// on rectangle moving and resizing 
void QGraphicsRectSelector::onRectChanged(const QRectF& rect)
{
    setDrawingMode(false);
}

// on selection of the rectangles 
void QGraphicsRectSelector::onSelectionChanged()
{
    QList<QGraphicsItem*> selections = _scene.selectedItems();
    if (selections.empty()) {
        //qDebug() << "No selected item";
    }
    else if (selections.size() == 1) {
        // one item is selected
        QGraphicsItem* item = selections[0];
    }
    else {
        //qDebug() << "Number of selected items is greater than 1";
    }
}
