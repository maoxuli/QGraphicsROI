#include "QGraphicsRectObject.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPainter>
#include <QDebug>
#include <cassert>

QGraphicsRectObject::QGraphicsRectObject(const QRectF& rect, QGraphicsItem* parent)
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

    _rect = QRectF(mapFromScene(rect.topLeft()), mapFromScene(rect.bottomRight()));
    _update_handles(); 
    _clear_mode(); 
}

QGraphicsRectObject::~QGraphicsRectObject()
{

}

void QGraphicsRectObject::setShapeLine(int width, Qt::PenStyle style)
{
    _shape_pen.setWidth(width); 
    _shape_pen.setStyle(style); 
}

void QGraphicsRectObject::setShapeColor(const QColor& color)
{
    _shape_pen.setColor(color); 
}

void QGraphicsRectObject::setHandleSize(int size)
{
    _handle_size = size; 
}

void QGraphicsRectObject::setHandleColor(const QColor& color)
{
    _handle_pen.setColor(color); 
}

// Return the bounding area of the item
// the actual rect + the handles
QRectF QGraphicsRectObject::boundingRect() const
{
    auto t = this->scene()->views().at(0)->transform(); //get current sace factors
    qreal x_off = (_handle_size / t.m11()) / 2.0 + 1; 
    qreal y_off = (_handle_size / t.m22()) / 2.0 + 1; 
    return _rect.adjusted(-x_off, -y_off, x_off, y_off);
}

// update handles based on current rect 
void QGraphicsRectObject::_update_handles() 
{
    if (scene()) {
        // resize rect of handles based on current scale factors
        auto t = scene()->views().at(0)->transform();
        _handles.fill(QRectF(0, 0, _handle_size/t.m11(), _handle_size/t.m22()), 8); 
    }
    else {
        _handles.fill(QRectF(0, 0, _handle_size, _handle_size), 8); 
    }
    // move rect of handles to proper position 
    _handles[TOP_LEFT_HANDLE].moveCenter(_rect.topLeft()); 
    _handles[TOP_RIGHT_HANDLE].moveCenter(_rect.topRight()); 
    _handles[BOTTOM_LEFT_HANDLE].moveCenter(_rect.bottomLeft()); 
    _handles[BOTTOM_RIGHT_HANDLE].moveCenter(_rect.bottomRight());
    _handles[TOP_HANDLE].moveCenter(QPointF(_rect.center().x(), _rect.top()));
    _handles[BOTTOM_HANDLE].moveCenter(QPointF(_rect.center().x(), _rect.bottom()));
    _handles[LEFT_HANDLE].moveCenter(QPointF(_rect.left(), _rect.center().y()));
    _handles[RIGHT_HANDLE].moveCenter(QPointF(_rect.right(), _rect.center().y()));
}

// customized painting
void QGraphicsRectObject::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    //painter->fillRect(boundingRect(), QBrush(QColor(128, 128, 255, 128)));
    painter->setPen(_shape_pen);
    painter->drawRect(_rect);
    //draw handles for currently selected item 
    if(isSelected()) {
        _update_handles(); 
        painter->setPen(_handle_pen);
        painter->drawRects(_handles);
    }
}

int QGraphicsRectObject::_check_pos_in_handle(const QPointF& pos)
{
    for (int i = 0; i < _handles.size(); i++) {
        QRectF rect = _handles[i]; 
        rect.adjust(-2, -2, 2, 2); // extend size for easy grab 
        if (rect.contains(pos)) {
            return i; 
        }
    }
    return -1; 
}

void QGraphicsRectObject::_clear_mode()
{
    _resizing = false; 
    _resizing_handle = -1; 
    if (scene()) {
        scene()->views()[0]->viewport()->setCursor(Qt::OpenHandCursor);
    }
}

void QGraphicsRectObject::_set_dragging_mode() 
{
    if (scene()) {
        scene()->views()[0]->viewport()->setCursor(Qt::ClosedHandCursor);
    }
}

bool QGraphicsRectObject::_set_resizing_mode(const QPointF& pos)
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
        case TOP_LEFT_HANDLE:
        case BOTTOM_RIGHT_HANDLE:
            scene()->views()[0]->viewport()->setCursor(Qt::SizeFDiagCursor);
            break;
        case TOP_RIGHT_HANDLE:
        case BOTTOM_LEFT_HANDLE:
            scene()->views()[0]->viewport()->setCursor(Qt::SizeBDiagCursor);
            break;
        default:
            assert(false);
        }
    }
    return true; 
}

// handle pos in local coords
void QGraphicsRectObject::_resize_rect(const QPointF& pos)
{
    assert(_resizing); 
    assert(_resizing_handle >= 0); 
    switch (_resizing_handle) {
    case TOP_LEFT_HANDLE:
        _rect.setTopLeft(pos); 
        break;
    case TOP_RIGHT_HANDLE:
        _rect.setTopRight(pos);
        break;
    case BOTTOM_LEFT_HANDLE:
        _rect.setBottomLeft(pos);
        break;
    case BOTTOM_RIGHT_HANDLE:
        _rect.setBottomRight(pos);
        break;
    case TOP_HANDLE:
        _rect.setTop(pos.y());
        break;
    case BOTTOM_HANDLE:
        _rect.setBottom(pos.y());
        break;
    case RIGHT_HANDLE:
        _rect.setRight(pos.x());
        break;
    case LEFT_HANDLE:
        _rect.setLeft(pos.x());
        break;
    default: 
        assert(false); 
    }
    _rect = _rect.normalized();
}

void QGraphicsRectObject::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    _clear_mode(); 
    QGraphicsObject::hoverEnterEvent(event);
}

void QGraphicsRectObject::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    _clear_mode(); 
    QGraphicsObject::hoverLeaveEvent(event);
}

void QGraphicsRectObject::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
    if(isSelected()) {
        _set_resizing_mode(event->pos());
    }
    QGraphicsObject::hoverMoveEvent(event);
}

void QGraphicsRectObject::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (!_set_resizing_mode(event->pos())) {
        _set_dragging_mode(); 
    }
    QGraphicsObject::mousePressEvent(event);
}

void QGraphicsRectObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if(_resizing) {
        // resize the rect 
        if (scene()->sceneRect().contains(event->scenePos())) {
            _resize_rect(event->pos()); 
            prepareGeometryChange();
            update();
        }
    }
    else {
        // move the rect 
        QGraphicsObject::mouseMoveEvent(event);
    }
    // notify both resizing and moving 
    QPointF tl = mapToScene(_rect.topLeft());
    QPointF br = mapToScene(_rect.bottomRight());
    Q_EMIT rectChanged(QRectF(tl, br));
}

void QGraphicsRectObject::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    _clear_mode(); 
    QGraphicsObject::mouseReleaseEvent(event);
}

// hook changes from item 
// could emit position change here 
QVariant QGraphicsRectObject::itemChange(GraphicsItemChange change, const QVariant &value)
{
    // if(change == QGraphicsItem::ItemPositionChange) {
    //     // limit the moving inside the scene
    //     // reset position if out of the scene
    //     QPointF pt = value.toPointF();
    //     qDebug() << "position changed: " << pt;
    //     return QGraphicsItem::itemChange(change, pt);
    // }

    if(change == QGraphicsItem::ItemSelectedHasChanged) {
        // display handles for selection
        //prepareGeometryChange();
        bool selected = value.toBool();
        setZValue(selected ? 1 : 0);
    }
    return QGraphicsItem::itemChange(change, value);
}
