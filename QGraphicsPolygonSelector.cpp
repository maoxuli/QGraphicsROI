#include "QGraphicsPolygonSelector.h"
#include "QGraphicsViewZoom.h"
#include "QGraphicsPolygonObject.h"
#include <QKeyEvent>
#include <QDebug>

QGraphicsPolygonSelector::QGraphicsPolygonSelector(QWidget* parent)
    : QGraphicsView(parent)
    , _drawing_line(NULL)
    , _drawing_pen(QBrush(Qt::red), 1, Qt::SolidLine) 
{
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setDragMode(QGraphicsView::ScrollHandDrag);
    QGraphicsViewZoom* zoom = new QGraphicsViewZoom(this);
    zoom->set_modifiers(Qt::NoModifier);

    setScene(&_scene);
    connect(&_scene, &QGraphicsScene::selectionChanged, 
            this, &QGraphicsPolygonSelector::onSelectionChanged);

    // default scene 
    scene()->addPixmap(QPixmap::fromImage(QImage(1920, 1080, QImage::Format_RGB888)));
    scene()->setSceneRect(QRectF(0, 0, 1920, 1080));
}

QGraphicsPolygonSelector::~QGraphicsPolygonSelector()
{

}

// add a polygon item
void QGraphicsPolygonSelector::addPolygonItem(const QPolygonF& polygon)
{
    QGraphicsPolygonObject* item = new QGraphicsPolygonObject(polygon);
    connect(item, SIGNAL(polygonChanged(const QPolygonF&)), this, SLOT(onPolygonChanged(const QPolygonF&)));
    scene()->addItem(item); 
}

// enable drawing polygon with mouse
void QGraphicsPolygonSelector::setDrawingMode(bool drawing)
{
    _drawing_mode = drawing;
    if(_drawing_mode) {
        setDragMode(QGraphicsView::NoDrag);
        viewport()->setCursor(Qt::CrossCursor);
    }
    else {
        setDragMode(QGraphicsView::ScrollHandDrag);
        _clear_drawing(); 
    }
}

void QGraphicsPolygonSelector::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Shift) {
        setDrawingMode(true);
    }
    else if (event->key() == Qt::Key_Escape) {
        setDrawingMode(false);
    }
    // forward key press anyway
    QWidget::keyPressEvent(event);
}

void QGraphicsPolygonSelector::_clear_drawing()
{
    if (_drawing_line) {
        _scene.removeItem(_drawing_line);
        delete _drawing_line;
        _drawing_line = NULL;
    }
    for (int i = 0; i < _drawing_items.size(); i++) {
        QGraphicsItem* item = _drawing_items[i];
        _scene.removeItem(item);
        delete item;
    }
    _drawing_items.clear();
    _drawing_points.clear();
}

void QGraphicsPolygonSelector::_prepare_drawing(const QPointF& pos)
{
    _clear_drawing(); 
    assert(!_drawing_line);
    assert(_drawing_items.size() == 0); 
    assert(_drawing_points.size() == 0); 
    // first point 
    _drawing_points.append(pos);
    _drawing_line = scene()->addLine(pos.x(), pos.y(), pos.x(), pos.y(), _drawing_pen);
}

// polygon drawing with mouse:
// adding the first point with left press;
// adding following point with left release; 
// drawing lines between added points and
// from the last point to current mouse position;
// complete polygon with right click;
// cancel drawing when drawing mode is disabled;
void QGraphicsPolygonSelector::mousePressEvent(QMouseEvent* event)
{
    if (_drawing_mode) {
        if (event->button() == Qt::LeftButton) {
            QPointF mouse_point = mapToScene(event->pos());
            if (_drawing_points.empty()) {
                // add first point with left mouse press 
                _prepare_drawing(mouse_point);
            }
            // else { 
            //     // add following points with left mouse press 
            //     QPointF last_point = _drawing_points.last(); 
            //     if (mouse_point != last_point) {
            //         // add a point 
            //         _drawing_points.append(mouse_point); 
            //         assert(_drawing_line); 
            //         _drawing_line->setLine(last_point.x(), last_point.y(), mouse_point.x(), mouse_point.y());
            //         _drawing_items.append(_drawing_line); 
            //         _drawing_line = scene()->addLine(mouse_point.x(), mouse_point.y(),
            //                                 mouse_point.x(), mouse_point.y(), _drawing_pen);
            //     }
            // }
        }
        else if (event->button() == Qt::RightButton) {
            // complete drawing
            if (_drawing_points.size() > 3) {
                addPolygonItem(QPolygonF(_drawing_points));
            }
            _clear_drawing(); 
            setDrawingMode(false);
        }
    }
    else {
        QGraphicsView::mousePressEvent(event);
    }
}

// update line from last point to current mouse position 
void QGraphicsPolygonSelector::mouseMoveEvent(QMouseEvent* event)
{
    if (_drawing_mode) {
        if (_drawing_line) {
            // set the line from last point to mouse
            assert(_drawing_points.size() > 0); 
            QPointF mouse_point = mapToScene(event->pos());
            QPointF last_point = _drawing_points.last();
            _drawing_line->setLine(last_point.x(), last_point.y(), mouse_point.x(), mouse_point.y());
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}

void QGraphicsPolygonSelector::mouseReleaseEvent(QMouseEvent* event)
{
    if (_drawing_mode) {
        if (event->button() == Qt::LeftButton) {
            // add following points with left mouse release 
            QPointF mouse_point = mapToScene(event->pos());
            assert(_drawing_points.size() > 0); 
            QPointF last_point = _drawing_points.last(); 
            if (mouse_point != last_point) {
                // add a point 
                _drawing_points.append(mouse_point); 
                assert(_drawing_line); 
                _drawing_line->setLine(last_point.x(), last_point.y(), mouse_point.x(), mouse_point.y());
                _drawing_items.append(_drawing_line); 
                _drawing_line = scene()->addLine(mouse_point.x(), mouse_point.y(),
                                         mouse_point.x(), mouse_point.y(), _drawing_pen);
            }
        }
    }
    QGraphicsView::mouseReleaseEvent(event);
}

// on polygon moving and resizing 
void QGraphicsPolygonSelector::onPolygonChanged(const QPolygonF& polygon)
{
    setDrawingMode(false);
}

// response to mouse click or set selection
void QGraphicsPolygonSelector::onSelectionChanged()
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
