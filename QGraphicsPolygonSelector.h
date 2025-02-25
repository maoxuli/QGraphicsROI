#pragma once 

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsLineItem>
#include <QPen>

/*!
 * This class show a graphics view that supports ROI selection with polygon.
 * When the "shift" key is pressed, user may draw polygon point by press the 
 * left mouse key and complete the polygon by press the right mouse key. 
 * Press "esc" key to cancel the polygon drawing. 
 * User may click on a polygon to move and resize the ROI.   
 * The view needs to get focus, by setFocus(), to capture the key press. 
 */
class QGraphicsPolygonSelector : public QGraphicsView
{
    Q_OBJECT
public:
    QGraphicsPolygonSelector(QWidget *parent = 0);
    ~QGraphicsPolygonSelector();

public slots: 
    // add a polygon  
    void addPolygonItem(const QPolygonF& polygon);

    // enable polygon drawing with mouse
    virtual void setDrawingMode(bool drawing);

private slots:
    // press "shift" key to draw polygon 
    void keyPressEvent(QKeyEvent *event);

    // on mouse operations to draw polygon 
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

    // on polygon moving and resizing 
    void onPolygonChanged(const QPolygonF&);

    // on selection of the polygons 
    void onSelectionChanged();

private:
    QGraphicsScene _scene;
    bool _drawing_mode;
    QVector<QPointF> _drawing_points;
    QVector<QGraphicsItem*> _drawing_items;
    QGraphicsLineItem* _drawing_line;
    QPen _drawing_pen;

    void _clear_drawing(); 
    void _prepare_drawing(const QPointF& pos); 
};
