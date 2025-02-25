#pragma once 

#include <QWidget> 
#include <QMainWindow>
#include <QTabWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

public slots:
    void changeCurrent(int index);

private:
     QTabWidget* _tab;
};
