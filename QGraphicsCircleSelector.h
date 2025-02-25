#pragma once 

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QPen>

/*!
 * This class show a graphics view that supports ROI selection with circle.
 * When the "shift" key is pressed, user may draw circle by press the left 
 * mouse key and move in the view. Press "esc" key to cancel the drawing. 
 * User may click on a circle to move and resize the ROI.   
 * The view needs to get focus, by setFocus(), to capture the key press. 
 */
class QGraphicsCircleSelector : public QGraphicsView
{
    Q_OBJECT
public:
    QGraphicsCircleSelector(QWidget *parent = 0);
    virtual ~QGraphicsCircleSelector();

public slots: 
    // add a circle  
    void addCircleItem(const QPointF& center, qreal radius);

    // enable circle drawing with mouse
    virtual void setDrawingMode(bool drawing);

private slots:
    // press "shift" key to draw circle 
    void keyPressEvent(QKeyEvent *event);

    // on mouse operations to draw a circle 
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);

    // on circle moving and resizing 
    void onCircleChanged(const QPointF&, qreal);

    // on selection of circles 
    void onSelectionChanged();

private:
    QGraphicsScene _scene;

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
