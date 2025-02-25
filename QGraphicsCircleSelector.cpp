#include "QGraphicsCircleSelector.h"
#include "QGraphicsViewZoom.h"
#include "QGraphicsCircleObject.h"
#include <QKeyEvent>
#include <QDebug>
#include <math.h>
#include <cassert>

QGraphicsCircleSelector::QGraphicsCircleSelector(QWidget* parent)
    : QGraphicsView(parent)
    , _drawing_mode(false)
    , _drawing_radius(0)
    , _drawing_circle(NULL)
    , _drawing_pen(QBrush(Qt::red), 1, Qt::SolidLine) 
{
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setDragMode(QGraphicsView::ScrollHandDrag);
    QGraphicsViewZoom* zoom = new QGraphicsViewZoom(this);

    setScene(&_scene);
    connect(&_scene, &QGraphicsScene::selectionChanged, this, &QGraphicsCircleSelector::onSelectionChanged);

    scene()->addPixmap(QPixmap::fromImage(QImage(1920, 1080, QImage::Format_RGB888)));
    scene()->setSceneRect(QRectF(0, 0, 1920, 1080));
}

QGraphicsCircleSelector::~QGraphicsCircleSelector()
{

}
// Create a polygon item
void QGraphicsCircleSelector::addCircleItem(const QPointF& center, qreal radius)
{
    QGraphicsCircleObject* item = new QGraphicsCircleObject(center, radius);
    connect(item, SIGNAL(circleChanged(const QPointF&, qreal)), this, SLOT(onCircleChanged(const QPointF&, qreal)));
    scene()->addItem(item); 
}

// enable drawing polygon with mouse
void QGraphicsCircleSelector::setDrawingMode(bool drawing)
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

void QGraphicsCircleSelector::_clear_drawing()
{
    _drawing_radius = 0; 
    if (_drawing_circle) {
        scene()->removeItem(_drawing_circle);
        delete _drawing_circle;
        _drawing_circle = NULL;
        scene()->removeItem(_h_line);
        delete _h_line;
        _h_line = NULL;
        scene()->removeItem(_v_line);
        delete _v_line;
        _v_line = NULL;
    }
}

void QGraphicsCircleSelector::_prepare_drawing(const QPointF& pos)
{
    _clear_drawing(); 
    _drawing_center = pos; 
    assert(!_drawing_circle); 
    _drawing_circle = scene()->addEllipse(0, 0, 0, 0, _drawing_pen); 
    _h_line = scene()->addLine(0, 0, 0, 0, _drawing_pen);
    _v_line = scene()->addLine(0, 0, 0, 0, _drawing_pen);
}

void QGraphicsCircleSelector::keyPressEvent(QKeyEvent *event)
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

// drawing circle with mouse:
// set center with left button click 
// set radius when moving mouse and draw the circle with crossing lines 
// stop drawing when left button released 
void QGraphicsCircleSelector::mousePressEvent(QMouseEvent* event)
{
    if (_drawing_mode) {
        if (event->button() == Qt::LeftButton) {
            _prepare_drawing(mapToScene(event->pos()));
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void QGraphicsCircleSelector::mouseMoveEvent(QMouseEvent* event)
{
    if (_drawing_mode) {
        if (event->buttons() == Qt::LeftButton) {
            // calculate new radius
            QPointF mouse_point = mapToScene(event->pos());
            // qDebug() << "mouse: " << mouse_point;
            _drawing_radius = sqrt(pow(mouse_point.x() - _drawing_center.x(), 2) + pow(mouse_point.y() - _drawing_center.y(), 2));
            // qDebug() << "radius: " << _drawing_radius;
            assert(_drawing_circle); 
            _drawing_circle->setRect(QRectF(_drawing_center - QPointF(_drawing_radius, _drawing_radius), _drawing_center + QPointF(_drawing_radius, _drawing_radius)));
            // qDebug() << "rect: " << _drawing_circle->rect();
            assert(_h_line);
            assert(_v_line);
            _h_line->setLine(QLineF(_drawing_center - QPointF(_drawing_radius, 0), _drawing_center + QPointF(_drawing_radius, 0)));
            _v_line->setLine(QLineF(_drawing_center - QPointF(0, _drawing_radius), _drawing_center + QPointF(0, _drawing_radius)));
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}

void QGraphicsCircleSelector::mouseReleaseEvent(QMouseEvent* event)
{
    if (_drawing_mode) {
        if (event->button() == Qt::LeftButton) {
            if (_drawing_radius > 0) {
                addCircleItem(_drawing_center, _drawing_radius);
            }
            _clear_drawing(); 
            setDrawingMode(false);
        }
    }
    QGraphicsView::mouseReleaseEvent(event);
}

// changing of the polygon by mouse dragging
// shape change or position change by mouse dragging
// represented by points in scene coordinate
void QGraphicsCircleSelector::onCircleChanged(const QPointF& center, qreal radius)
{
    setDrawingMode(false);
    // polygon.translate(-sceneRect().left(), -sceneRect().top());
    // Q_EMIT ObjectChanged(oid, ObjectShape(polygon));
}

// response to mouse click or set selection
void QGraphicsCircleSelector::onSelectionChanged()
{
    QList<QGraphicsItem*> selections = _scene.selectedItems();
    if (selections.empty()) {
        //qDebug() << "No selected item";
    }
    else if (selections.size() == 1) {
        // one item is selected
        QGraphicsItem* item = selections[0];
    }
    else
    {
        //qDebug() << "Number of selected items is greater than 1";
    }
}
