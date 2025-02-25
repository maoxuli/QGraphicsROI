#include "QGraphicsPolygonSelector.h"
#include "QGraphicsViewZoom.h"
#include "QGraphicsPolygonObject.h"
#include <QKeyEvent>
#include <QDebug>

QGraphicsPolygonSelector::QGraphicsPolygonSelector(QWidget* parent)
    : QGraphicsView(parent)
    , _drawing_line(NULL)
{
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setDragMode(QGraphicsView::ScrollHandDrag);
    QGraphicsViewZoom* zoom = new QGraphicsViewZoom(this);

    setScene(&_scene);
    connect(&_scene, &QGraphicsScene::selectionChanged, this, &QGraphicsPolygonSelector::onSelectionChanged);

    scene()->addPixmap(QPixmap::fromImage(QImage(1920, 1080, QImage::Format_RGB888)));
    scene()->setSceneRect(QRectF(0, 0, 1920, 1080));
}

QGraphicsPolygonSelector::~QGraphicsPolygonSelector()
{

}

// Create a polygon item
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

// drawing polygon with mouse:
// adding a point with left click;
// drawing lines between added points and
// from the last point to current mouse position;
// complete with right click, and trigger ObjectAdding event;
// stop drawing when drawing mode is disabled;
void QGraphicsPolygonSelector::mousePressEvent(QMouseEvent* event)
{
    if (_drawing_mode) {
        if (event->button() == Qt::LeftButton) {
            QPointF mouse_point = mapToScene(event->pos());
            if (_drawing_line != NULL) {
                Q_ASSERT(_drawing_points.size() > 0);
                QPoint last_point = _drawing_points.last();
                _drawing_line->setLine(0, 0, mouse_point.x() - last_point.x(),
                                       mouse_point.y() - last_point.y());
                // add line for last two points
                _drawing_items.append(_drawing_line);
            }
            // add point
            _drawing_points.append(QPoint(mouse_point.x(), mouse_point.y()));
            // add line for last point to mouse
            _drawing_line = new QGraphicsLineItem();
            _scene.addItem(_drawing_line);
            QPen line_pen;
            line_pen.setCosmetic(true); //to maintain same width of pen across zoom levels
            line_pen.setColor(Qt::red);
            line_pen.setStyle(Qt::SolidLine);
            _drawing_line->setPen(line_pen);
            _drawing_line->setPos(mouse_point);
        }
        else if (event->button() == Qt::RightButton) {
            // complete drawing
            if (_drawing_points.size() > 3) {
                addPolygonItem(QPolygonF(_drawing_points));
            }
            setDrawingMode(false);
        }
    }
    else {
        QGraphicsView::mousePressEvent(event);
    }
}

void QGraphicsPolygonSelector::mouseMoveEvent(QMouseEvent* event)
{
    if (_drawing_mode) {
        if (event->button() == Qt::NoButton && _drawing_line != NULL) {
            // set the line from last point to mouse
            QPointF mouse_point = mapToScene(event->pos());
            QPoint last_point = _drawing_points.last();
            _drawing_line->setLine(0, 0, mouse_point.x() - last_point.x(),
                                   mouse_point.y() - last_point.y());
        }
    }
    QGraphicsView::mouseMoveEvent(event);
}

// changing of the polygon by mouse dragging
// shape change or position change by mouse dragging
// represented by points in scene coordinate
void QGraphicsPolygonSelector::onPolygonChanged(const QPolygonF& polygon)
{
    setDrawingMode(false);
    // polygon.translate(-sceneRect().left(), -sceneRect().top());
    // Q_EMIT ObjectChanged(oid, ObjectShape(polygon));
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
    else
    {
        //qDebug() << "Number of selected items is greater than 1";
    }
}
