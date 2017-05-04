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
#include <QLabel>

#include "inc/Satellite.hpp"

#include "createimage.h"

namespace Ui {

enum DATA_TYPE {
    NONE,
    PRO,
    VAR,
    IMG
};

class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
//    virtual void wheelEvent(QWheelEvent* event);

    virtual void keyPressEvent(QKeyEvent*);

private slots:
    void on_actionOpen_triggered();

    void on_actionQuit_triggered();

    void on_actionCreate_template_triggered();

private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene;
    QGraphicsPixmapItem* item;

    CreateImage dialog;

    Ui::DATA_TYPE data_type;

    satellite::passport::Proection passport;
    satellite::Image image;

    double MIN_ZOOM, MAX_ZOOM;
    double zoom;
};

#endif // MAINWINDOW_H
