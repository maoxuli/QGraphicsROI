#include "QGraphicsCircleObject.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QDebug>
#include <math.h>
#include <cassert>

QGraphicsCircleObject::QGraphicsCircleObject(const QPointF& center, qreal radius, QGraphicsItem* parent)
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

    _center = mapFromScene(center);
    _radius = radius; 
    _update_handles(); 
    _clear_mode(); 
}

QGraphicsCircleObject::~QGraphicsCircleObject()
{

}

void QGraphicsCircleObject::setShapeLine(int width, Qt::PenStyle style)
{
    _shape_pen.setWidth(width); 
    _shape_pen.setStyle(style); 
}

void QGraphicsCircleObject::setShapeColor(const QColor& color)
{
    _shape_pen.setColor(color); 
}

void QGraphicsCircleObject::setHandleSize(int size)
{
    _handle_size = size; 
}

void QGraphicsCircleObject::setHandleColor(const QColor& color)
{
    _handle_pen.setColor(color); 
}

// return the actual bounding area of the item
// include the resizing handles alwyas now.
// shoudl determine by the selected status though.
// Todo: should also include half of the line width for handles
QRectF QGraphicsCircleObject::boundingRect() const
{
    auto t = this->scene()->views().at(0)->transform(); //get current sace factors
    qreal x_off = (_handle_size / t.m11()) / 2.0 + 1; 
    qreal y_off = (_handle_size / t.m22()) / 2.0 + 1; 
    // extend the area of polygon to include the handle rects
    QPointF off(_radius, _radius); 
    QRectF rect(_center - off, _center + off); 
    return rect.adjusted(-x_off, -y_off, x_off, y_off);
}

// we try to update the handle size when the image size is changed
// should trigger by relevant event of scene scale (but not found).
// now update in paint function, this introduced non-necessary update,
// for example the update when the item is moving.
void QGraphicsCircleObject::_update_handles()
{
    if (scene()) {
        // resize rect of handles based on current scale factors
        auto t = scene()->views().at(0)->transform();
        _handles.fill(QRectF(0, 0, _handle_size/t.m11(), _handle_size/t.m22()), 4);
    }
    else {
        _handles.fill(QRectF(0, 0, _handle_size, _handle_size), 4);
    }
    // move rect of handles to proper position 
    _handles[TOP_HANDLE].moveCenter(QPointF(_center.x(), _center.y() - _radius)); 
    _handles[BOTTOM_HANDLE].moveCenter(QPointF(_center.x(), _center.y() + _radius));
    _handles[LEFT_HANDLE].moveCenter(QPointF(_center.x() - _radius, _center.y())); 
    _handles[RIGHT_HANDLE].moveCenter(QPointF(_center.x() + _radius, _center.y())); 
}

// customized painting
void QGraphicsCircleObject::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setPen(_shape_pen);
    painter->drawEllipse(_center, _radius, _radius); 
    painter->drawLine(_center - QPointF(_radius, 0), _center + QPointF(_radius, 0)); 
    painter->drawLine(_center - QPointF(0, _radius), _center + QPointF(0, _radius)); 
    // draw resize handles if the item is currectly selected
    if (isSelected()) {
        _update_handles();
        painter->setPen(_handle_pen);
        painter->drawRects(_handles);
    }
}

int QGraphicsCircleObject::_check_pos_in_handle(const QPointF& pos)
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

void QGraphicsCircleObject::_clear_mode()
{
    _resizing = false; 
    _resizing_handle = -1; 
    if (scene()) {
        scene()->views()[0]->viewport()->setCursor(Qt::OpenHandCursor);
    }
}

void QGraphicsCircleObject::_set_dragging_mode() 
{
    if (scene()) {
        scene()->views()[0]->viewport()->setCursor(Qt::ClosedHandCursor);
    }
}

bool QGraphicsCircleObject::_set_resizing_mode(const QPointF& pos)
{
    int handle = _check_pos_in_handle(pos);
    if (handle < 0) {
        _clear_mode();
        return false; 
    }
    _resizing = true; 
    _resizing_handle = handle; 
    if (scene()) {
        switch (_resizing_handle) {
        case TOP_HANDLE:
        case BOTTOM_HANDLE:
            scene()->views()[0]->viewport()->setCursor(Qt::SizeVerCursor);
            break;
        case LEFT_HANDLE:
        case RIGHT_HANDLE:
            scene()->views()[0]->viewport()->setCursor(Qt::SizeHorCursor);
            break;
        default:
            assert(false);
        }
    }
    return true; 
}

void QGraphicsCircleObject::_resize_circle(const QPointF& pos)
{
    assert(_resizing); 
    assert(_resizing_handle >= 0); 
    // here we use current mouse pos as new postion
    // should use the center of the handle, or the changing of mouse pos
    _radius = sqrt(pow(pos.x() - _center.x(), 2) + pow(pos.y() - _center.y(), 2)); 
}

void QGraphicsCircleObject::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    _clear_mode(); 
    QGraphicsObject::hoverEnterEvent(event);
}

void QGraphicsCircleObject::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    _clear_mode(); 
    QGraphicsObject::hoverLeaveEvent(event);
}

void QGraphicsCircleObject::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    if(isSelected()) {
        _set_resizing_mode(event->pos());
    }
    QGraphicsObject::hoverMoveEvent(event);
}

void QGraphicsCircleObject::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (!_set_resizing_mode(event->pos())) {
        _set_dragging_mode(); 
    }
    QGraphicsObject::mousePressEvent(event);
}

void QGraphicsCircleObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (_resizing) {
        if (scene()->sceneRect().contains(event->scenePos())) {
            _resize_circle(event->pos());
            prepareGeometryChange();
            update();
        }
    }
    else {
        // move position done by default
        QGraphicsObject::mouseMoveEvent(event);
    }
    // trigger changing
    Q_EMIT circleChanged(mapToScene(_center), _radius);
}

void QGraphicsCircleObject::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    _clear_mode(); 
    QGraphicsObject::mouseReleaseEvent(event);
}

// special actions when item is changing
QVariant QGraphicsCircleObject::itemChange(GraphicsItemChange change, const QVariant &value)
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
