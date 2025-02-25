#include "MainWindow.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QTabWidget> 
#include "QGraphicsRectSelector.h"
#include "QGraphicsPolygonSelector.h"
#include "QGraphicsCircleSelector.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setCentralWidget(new QWidget());
    QVBoxLayout* layout = new QVBoxLayout(centralWidget());
    _tab = new QTabWidget();
    connect(_tab, SIGNAL(currentChanged(int)), this, SLOT(changeCurrent(int)));
    layout->addWidget((_tab));

    _tab->addTab(new QGraphicsRectSelector(), "Rectangle Selection");
    _tab->addTab(new QGraphicsPolygonSelector(), "Polygon Selection");
    _tab->addTab(new QGraphicsCircleSelector(), "Circle Selection");
}

MainWindow::~MainWindow() 
{

}

void MainWindow::changeCurrent(int index)
{
    _tab->currentWidget()->setFocus();
}
