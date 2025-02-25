#pragma once 

#include <QGraphicsView>
#include <QGraphicsScene>

class QGraphicsRectSelector : public QGraphicsView
{
    Q_OBJECT
public:
    QGraphicsRectSelector(QWidget *parent = 0);
    ~QGraphicsRectSelector();

public slots:
    // add a rect item 
    void addRectItem(const QRectF& rect);

    // enable drawing shape with mouse
    void setDrawingMode(bool drawing);

private slots:
    void keyPressEvent(QKeyEvent *event);
    // Response to rubber band operation
    void onRubberBandChanged(QRect rubberBandRect, QPointF fromScenePoint, QPointF toScenePoint);

    // Response to mouse operation on shape
    void onRectChanged(const QRectF&);

    // response to mouse click
    void onSelectionChanged();

private:
    QGraphicsScene _scene;
};
