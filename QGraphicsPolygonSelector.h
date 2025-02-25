#pragma once 

#include <QGraphicsView>
#include <QGraphicsScene>

// for annotation with polygon shape
class QGraphicsPolygonSelector : public QGraphicsView
{
    Q_OBJECT
public:
    QGraphicsPolygonSelector(QWidget *parent = 0);
    ~QGraphicsPolygonSelector();

public slots: 
    // add a polygon item 
    void addPolygonItem(const QPolygonF& polygon);

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

    // Response to mouse operation on shape
    void onPolygonChanged(const QPolygonF&);

    // response to mouse click
    void onSelectionChanged();

private:
    QGraphicsScene _scene;

private:
    bool _drawing_mode;
    QVector<QPoint> _drawing_points;
    QVector<QGraphicsItem*> _drawing_items;
    QGraphicsLineItem* _drawing_line;
    void clearDrawing();
};
