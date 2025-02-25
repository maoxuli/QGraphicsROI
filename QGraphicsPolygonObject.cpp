#include "QGraphicsPolygonObject.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QDebug>
#include <cassert>

QGraphicsPolygonObject::QGraphicsPolygonObject(const QPolygonF& polygon, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , _handle_size(DEFAULT_HANDLE_SIZE)
    , _shape_pen(QBrush(Qt::red), 1, Qt::SolidLine) 
    , _handle_pen(QBrush(Qt::green), 1, Qt::SolidLine) 
{
    setFlags(QGraphicsItem::ItemSendsGeometryChanges |
             QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    _shape_pen.setCosmetic(true);
    _handle_pen.setCosmetic(true);

    _polygon = mapFromScene(polygon);
    _update_handles(); 
    _clear_mode(); 
}

QGraphicsPolygonObject::~QGraphicsPolygonObject()
{

}

void QGraphicsPolygonObject::setShapeLine(int width, Qt::PenStyle style)
{
    _shape_pen.setWidth(width); 
    _shape_pen.setStyle(style); 
}

void QGraphicsPolygonObject::setShapeColor(const QColor& color)
{
    _shape_pen.setColor(color); 
}

void QGraphicsPolygonObject::setHandleSize(int size)
{
    _handle_size = size; 
}

void QGraphicsPolygonObject::setHandleColor(const QColor& color)
{
    _handle_pen.setColor(color); 
}

// return the actual bounding area of the item
// include the resizing handles alwyas now.
// shoudl determine by the selected status though.
// Todo: should also include half of the line width for handles
QRectF QGraphicsPolygonObject::boundingRect() const
{
    auto t = this->scene()->views().at(0)->transform(); //get current sace factors
    qreal x_off = (_handle_size / t.m11()) / 2.0 + 1; 
    qreal y_off = (_handle_size / t.m22()) / 2.0 + 1; 
    // extend the area of polygon to include the handle rects
    QRectF rect = _polygon.boundingRect();
    return rect.adjusted(-x_off, -y_off, x_off, y_off);
}

// we try to update the handle size when the image size is changed
// should trigger by relevant event of scene scale (but not found).
// now update in paint function, this introduced non-necessary update,
// for example the update when the item is moving.
void QGraphicsPolygonObject::_update_handles()
{
    if (scene()) {
        // resize rect of handles based on current scale factors
        auto t = scene()->views().at(0)->transform();
        _handles.fill(QRectF(0, 0, _handle_size/t.m11(), _handle_size/t.m22()), _polygon.count());
    }
    else {
        _handles.fill(QRectF(0, 0, _handle_size, _handle_size), _polygon.count());
    }
    // move rect of handles to proper position 
    for (int i = 0; i < _polygon.count(); i++) {
        _handles[i].moveCenter(_polygon[i]);
    }
}

// customized painting
void QGraphicsPolygonObject::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(_shape_pen);
    painter->drawPolygon(_polygon);
    // draw resize handles if the item is currectly selected
    if (isSelected()) {
        _update_handles();
        painter->setPen(_handle_pen);
        painter->drawRects(_handles);
    }
}

int QGraphicsPolygonObject::_check_pos_in_handle(const QPointF& pos)
{
    for (int i = 0; i < _handles.size(); i++) {
        QRectF rect = _handles[i]; // local coords
        //qDebug() << "mouse on handle " << i << ": " << rc;
        rect.adjust(-2, -2, 2, 2); // enlarge to ease capture
        if (rect.contains(pos)) {
            return i;
        }
    }
    return -1; 
}

void QGraphicsPolygonObject::_clear_mode()
{
    _resizing = false; 
    _resizing_handle = -1; 
    if (scene()) {
        scene()->views()[0]->viewport()->setCursor(Qt::OpenHandCursor);
    }
}

void QGraphicsPolygonObject::_set_dragging_mode() 
{
    if (scene()) {
        scene()->views()[0]->viewport()->setCursor(Qt::ClosedHandCursor);
    }
}

bool QGraphicsPolygonObject::_set_resizing_mode(const QPointF& pos)
{
    int handle = _check_pos_in_handle(pos);
    if (handle < 0) {
        _clear_mode();
        return false; 
    }
    _resizing = true; 
    _resizing_handle = handle; 
    if (scene()) {
        scene()->views()[0]->viewport()->setCursor(Qt::CrossCursor);
    }
    return true; 
}

void QGraphicsPolygonObject::_resize_polygon(const QPointF& pos)
{
    assert(_resizing); 
    assert(_resizing_handle >= 0); 
    // here we use current mouse pos as new postion
    // should use the center of the handle, or the changing of mouse pos
    _polygon[_resizing_handle] = pos;
}

void QGraphicsPolygonObject::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    _clear_mode(); 
    QGraphicsObject::hoverEnterEvent(event);
}

void QGraphicsPolygonObject::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    _clear_mode(); 
    QGraphicsObject::hoverLeaveEvent(event);
}

void QGraphicsPolygonObject::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    if(isSelected()) {
        _set_resizing_mode(event->pos());
    }
    QGraphicsObject::hoverMoveEvent(event);
}

void QGraphicsPolygonObject::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (!_set_resizing_mode(event->pos())) {
        _set_dragging_mode(); 
    }
    QGraphicsObject::mousePressEvent(event);
}

void QGraphicsPolygonObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (_resizing) {
        if (scene()->sceneRect().contains(event->scenePos())) {
            _resize_polygon(event->pos()); 
            prepareGeometryChange();
            update();
        }
    }
    else {
        // move position done by default
        QGraphicsObject::mouseMoveEvent(event);
    }
    // trigger changing
    Q_EMIT polygonChanged(mapToScene(_polygon));
}

void QGraphicsPolygonObject::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    _clear_mode(); 
    QGraphicsObject::mouseReleaseEvent(event);
}

// special actions when item is changing
QVariant QGraphicsPolygonObject::itemChange(GraphicsItemChange change, const QVariant &value)
{
    // if (change == QGraphicsItem::ItemPositionChange) {
    //     // limit the moving of the polygon inside the scene region
    //     // re-set the new position if out of the scene
    //     QPointF pt = value.toPointF(); // new pos
    // }

    if (change == QGraphicsItem::ItemSelectedHasChanged) {
        // Adjust bounding rect for selected status
        // the bounding rect should re-calculate for selected change
        bool selected = value.toBool();
        setZValue(selected ? 1 : 0);
    }
    return QGraphicsItem::itemChange(change, value);
}
