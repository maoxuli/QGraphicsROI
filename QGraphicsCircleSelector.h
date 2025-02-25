#pragma once 

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

class QGraphicsCircleSelector : public QGraphicsView
{
    Q_OBJECT
public:
    QGraphicsCircleSelector(QWidget *parent = 0);
    virtual ~QGraphicsCircleSelector();

public slots: 
    // add a polygon item 
    void addCircleItem(const QPointF& center, qreal radius);

    // enable drawing polygon with mouse
    virtual void setDrawingMode(bool drawing);

private slots:
    void keyPressEvent(QKeyEvent *event);
    // drawing polygon with mouse:
    // adding a point with left click;
    // drawing lines between added points and
    // from the last point to current mouse position;
    // complete with right click, and trigger ObjectAdding event;
    // stop drawing when drawing mode is disabled;
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

    // Response to mouse operation on shape
    void onCircleChanged(const QPointF&, qreal);

    // response to mouse click
    void onSelectionChanged();

private:
    QGraphicsScene _scene;

private:
    bool _drawing_mode;
    QPointF _drawing_center; 
    qreal _drawing_radius; 
    QGraphicsEllipseItem* _drawing_circle;
    QGraphicsLineItem* _v_line;
    QGraphicsLineItem* _h_line;
    QPen _drawing_pen; 

    void _clear_drawing(); 
    void _prepare_drawing(const QPointF& pos); 
};
