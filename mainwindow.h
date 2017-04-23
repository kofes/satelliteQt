#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsView>
#include <QImage>
#include <QGraphicsPixmapItem>
#include <QColor>
#include <QPixmap>
#include <QWheelEvent>

#include "inc/Satellite.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionQuit_triggered();

//    void wheelEvent(QWheelEvent *event);
private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene;

    satellite::passport::Proection passport;
    satellite::Image image;
};

#endif // MAINWINDOW_H
