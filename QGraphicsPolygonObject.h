#pragma once 

#include <QGraphicsObject>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPen>

#define DEFAULT_HANDLE_SIZE 10

class QGraphicsPolygonObject : public QGraphicsObject
{
    Q_OBJECT
public:
    QGraphicsPolygonObject(const QPolygonF& polygon, QGraphicsItem* parent = 0);
    ~QGraphicsPolygonObject(); 

     // set pen for shape 
    void setShapeLine(int width, Qt::PenStyle style = Qt::SolidLine);
    void setShapeColor(const QColor& color); 

    // set pen for handle 
    void setHandleSize(int size); 
    void setHandleColor(const QColor& color); 

signals:
    void polygonChanged(const QPolygonF&);

protected:
    QRectF boundingRect() const;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

    // response to shape changing by mouse dragging
    void hoverEnterEvent(QGraphicsSceneHoverEvent*);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent*);
    void hoverMoveEvent(QGraphicsSceneHoverEvent*);

    void mousePressEvent(QGraphicsSceneMouseEvent*);
    void mouseMoveEvent(QGraphicsSceneMouseEvent*);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*);

    // hook item change, move to front when selected 
    QVariant itemChange(GraphicsItemChange change, const QVariant &value); 

    void _update_handles(); 
    void _resize_polygon(const QPointF& pos); 
    int _check_pos_in_handle(const QPointF& pos); 
    bool _set_resizing_mode(const QPointF& pos); 
    void _set_dragging_mode(); 
    void _clear_mode(); 

protected:
    // Polygon and handles 
    int _handle_size; 
    QPolygonF _polygon;
    QVector<QRectF> _handles;

    // Keep track of resizing 
    bool _resizing;
    int _resizing_handle; 

    // pens
    QPen _shape_pen; 
    QPen _handle_pen; 
};
