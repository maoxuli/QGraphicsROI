#pragma once

#include <QGraphicsObject>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPen>

#define DEFAULT_HANDLE_SIZE 10

class QGraphicsRectObject : public QGraphicsObject
{
    Q_OBJECT
public:
    // HANDLE ID 
    //  0   1   2
    //  7       3
    //  6   5   4
    enum HANDLE_ID
    {
        TOP_LEFT_HANDLE = 0,
        TOP_HANDLE = 1,
        TOP_RIGHT_HANDLE = 2,
        RIGHT_HANDLE = 3,
        BOTTOM_RIGHT_HANDLE = 4,
        BOTTOM_HANDLE = 5,
        BOTTOM_LEFT_HANDLE = 6,
        LEFT_HANDLE = 7,
    };

    QGraphicsRectObject(const QRectF& rect, QGraphicsItem* parent = 0);
    ~QGraphicsRectObject();

    // set pen for rect
    void setShapeLine(int width, Qt::PenStyle style = Qt::SolidLine);
    void setShapeColor(const QColor& color); 

    // set pen for handle 
    void setHandleSize(int size);
    void setHandleColor(const QColor& color); 

signals:
    void rectChanged(const QRectF&);

protected:
    QRectF boundingRect() const;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*);

    // mouse operations 
    void hoverEnterEvent(QGraphicsSceneHoverEvent*);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent*);
    void hoverMoveEvent(QGraphicsSceneHoverEvent*);

    void mousePressEvent(QGraphicsSceneMouseEvent*);
    void mouseMoveEvent(QGraphicsSceneMouseEvent*);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*);

    // hook item change 
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void _update_handles();
    void _resize_rect(const QPointF& pos); 
    int _check_pos_in_handle(const QPointF& pos);
    bool _set_resizing_mode(const QPointF& pos); 
    void _set_dragging_mode(); 
    void _clear_mode(); 

protected:
    // rect and handles 
    int _handle_size;
    QRectF _rect; 
    QVector<QRectF> _handles;

    // keep track of resizing 
    bool _resizing; 
    int _resizing_handle; 

    // pens
    QPen _shape_pen; 
    QPen _handle_pen; 
};
