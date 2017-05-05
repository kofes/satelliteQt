#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsView>
#include <QImage>
#include <QColor>
#include <QPixmap>
#include <QWheelEvent>
#include <QLabel>

#include "inc/Satellite.hpp"

#include "createimage.h"
#include "levels.h"

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
    virtual void keyPressEvent(QKeyEvent*);

private slots:
    void on_actionOpen_triggered();

    void on_actionQuit_triggered();

    void on_actionCreate_template_triggered();

    void on_actionLevels_triggered();

private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene;

    CreateImage *dialog;
    Levels *levels;

    Ui::DATA_TYPE data_type;

    satellite::passport::Proection passport;
    satellite::Image image;

    std::vector< std::pair<double, unsigned long> > gist;

    short MIN_VALUE, MAX_VALUE;
    double CENTRAL_VALUE, AVERAGE_DISP_VALUE;

    double MIN_ZOOM, MAX_ZOOM;
    double zoom;
};

#endif // MAINWINDOW_H
