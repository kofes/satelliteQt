#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "allin.hpp"

#include "createimage.h"
#include "levels.h"
#include "vardialog.h"
#include "graphic.h"

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

    void on_actionCalc_triggered();

private:
    Ui::MainWindow *ui;

    QGraphicsScene *scene;

    CreateImage *dialog;
    Levels *levels;
    varDialog *var_d;
    Graphic *graphic;

    Ui::DATA_TYPE data_type;

    satellite::passport::Proection passport;
    satellite::Image image;

    double MIN_ZOOM, MAX_ZOOM;
    double zoom;
};

#endif // MAINWINDOW_H
