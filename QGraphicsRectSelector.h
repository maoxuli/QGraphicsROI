#pragma once 

#include <QGraphicsView>
#include <QGraphicsScene>

/*!
 * This class show a graphics view that supports ROI selection with rectangle.
 * When the "shift" key is pressed, user may draw a rectangle by press the left 
 * mouse key and move in the view. The rectangle ROI will be added when the mouse 
 * key is released. Press "esc" key to cancel the rectangle drawing. 
 * User may click on a rectangle to move and resize the ROI.   
 * The view needs to get focus, by setFocus(), to capture the key press. 
 */
class QGraphicsRectSelector : public QGraphicsView
{
    Q_OBJECT
public:
    QGraphicsRectSelector(QWidget *parent = 0);
    ~QGraphicsRectSelector();

public slots:
    // add a rectangle
    void addRectItem(const QRectF& rect);

    // enable rectangle drawing with mouse 
    void setDrawingMode(bool drawing);

private slots:
    // press "shift" key to draw rectangle 
    void keyPressEvent(QKeyEvent *event);

    // on rubber band operation 
    void onRubberBandChanged(QRect rubberBandRect, QPointF fromScenePoint, QPointF toScenePoint);

    // on resrectangle moving and resizing 
    void onRectChanged(const QRectF&);

    // on selection of the rectangles 
    void onSelectionChanged();

private:
    QGraphicsScene _scene;
};
