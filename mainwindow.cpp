#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    levels = new Levels(this);
    dialog = new CreateImage(this);
    var_d = new varDialog(this);
    graphic = new Graphic(this);
    data_type = Ui::DATA_TYPE::NONE;
    zoom = 0;
    MIN_ZOOM = 10;
    MAX_ZOOM = 4000;
    ui->label_x->setVisible(false);
    ui->label_x_coord->setVisible(false);
    ui->label_y->setVisible(false);
    ui->label_y_coord->setVisible(false);
    ui->label_value->setVisible(false);
    ui->label_value_set->setVisible(false);
    ui->graphicsView->setDragMode(QGraphicsView::NoDrag);
    ui->graphicsView->setCursor(Qt::CrossCursor);
    qApp->installEventFilter(this);
    //Actions disable
    ui->actionSave->setEnabled(false);
    ui->actionLevels->setEnabled(false);
    ui->actionCalc->setEnabled(false);
    //
}

MainWindow::~MainWindow()
{
    levels->close();
    delete levels;
    dialog->close();
    delete dialog;
    var_d->close();
    delete var_d;
    graphic->close();
    delete graphic;
    delete scene;
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                                                    tr("lab34 files (*.pro);;Var files (*.var);;Generated images (*.img)"));
    if (fileName.isEmpty())
        return;

    std::ifstream file(fileName.toStdString());
    if (!file.is_open()) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
        return;
    }

    QFileInfo fileInfo(fileName);
    if (fileInfo.suffix() == "pro") {
        satellite::passport::Proection pass;
        file >> pass;

        if (pass.DATA_TYPE[0] != (uint8_t)(satellite::passport::DATA_TYPE::PROECTION)) {
            QMessageBox::critical(this, tr("Error"), tr("Image isn't proection"));
            file.close();
            return;
        }

        data_type = Ui::DATA_TYPE::PRO;

        passport = pass;

        image.read(passport.getCountPixelsInLine(), passport.getCountLines(), file);

        levels->init(image);
        QImage img(image.width(), image.height(), QImage::Format::Format_RGB32);

        std::cout << levels->max() << '\n'
                  << (levels->right()+levels->left())/2 << " : "
                  << (levels->right()-levels->left())/2
                  << std::endl;

        double left = levels->left(),
               right = levels->right();
        for (auto i = 0; i < image.height(); ++i)
            for (auto j = 0; j < image.width(); ++j) {
                short buff = image[i][j];
                switch (buff) {
                case -2:
                    img.setPixel(j, (image.height()-1) - i, qRgb(255,255,0));
                    break;
                case -5:
                    img.setPixel(j, (image.height()-1) - i, qRgb(255, 255/2, 255));
                    break;
                case -7:
                    img.setPixel(j, (image.height()-1) - i, qRgb(0, 255, 255));
                    break;
                default:
                    if (image[i][j] <= right && image[i][j] >= left)
                       img.setPixel(j, (image.height()-1) - i, qRgb(buff-left * (255.0f / (right-left)),
                                                                    buff-left * (255.0f / (right-left)),
                                                                    buff-left * (255.0f / (right-left))));
                    else
                       img.setPixel(j, (image.height()-1) - i, qRgb(0, 0, 0));
                break;
                }
            }
        //Actions disable
        ui->actionSave->setEnabled(false);
        //
        //Actions enable
        ui->actionLevels->setEnabled(true);
        ui->actionCalc->setEnabled(true);
        //
        scene->clear();
        ui->graphicsView->setScene(scene);
        scene->setSceneRect(img.rect());
        scene->addPixmap(QPixmap::fromImage(img));
        ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        zoom = ui->graphicsView->rect().width()*100.0/image.width();
        ui->label_zoom->setText(QString::number(zoom) + "%");
        return;
    }
    if (fileInfo.suffix() == "img") {
        unsigned short width, height;

        data_type = Ui::DATA_TYPE::IMG;

        file.read(reinterpret_cast<char *>(&height), sizeof(height));
        file.read(reinterpret_cast<char *>(&width), sizeof(width));
        image.read(width, height, file);
        levels->init(image);
        //
        QImage img(image.width(), image.height(), QImage::Format::Format_RGB32);

        for (auto i = 0; i < image.height(); ++i)
            for (auto j = 0; j < image.width(); ++j)
                img.setPixel(j, i, qRgb(image[i][j] * (255.0f / levels->max()), image[i][j] * (255.0f / levels->max()), image[i][j] * (255.0f / levels->max())));

        //Actions disable
        ui->actionLevels->setEnabled(false);
        ui->actionSave->setEnabled(false);
        //
        //Actions enable
        ui->actionCalc->setEnabled(true);
        //
        scene->clear();
        ui->graphicsView->setScene(scene);
        scene->setSceneRect(img.rect());
        scene->addPixmap(QPixmap::fromImage(img));
        ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        zoom = ui->graphicsView->rect().width()*100.0/image.width();
        ui->label_zoom->setText(QString::number(zoom) + "%");
        return;
    }
    if (fileInfo.suffix() == "var") {
        std::vector<double> var;
        double dh;
        unsigned long size;
        file >> size >> dh;
        for (unsigned long i = 0; i < size; ++i) {
            double buff;
            file >> buff;
            var.push_back(buff);
        }
        graphic->setVar(var, dh);
        graphic->show();
    }
}

void MainWindow::on_actionQuit_triggered()
{
    QMessageBox messageBox;
    messageBox.setWindowTitle(tr("satellite"));
    messageBox.setText(tr("Do you teally want to quit?"));
    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox.setDefaultButton(QMessageBox::No);
    if (messageBox.exec() == QMessageBox::Yes)
        QApplication::quit();

}

void MainWindow::on_actionCreate_template_triggered()
{
    short** buff;

    //Actions disable
    ui->actionCreate_template->setEnabled(false);
    //
    dialog->show();
    if (dialog->exec() != QDialog::Accepted)
        return;
    //Actions enable
    ui->actionCreate_template->setEnabled(true);
    //
    buff = new short* [dialog->height()];
    for (unsigned short i = 0; i < dialog->height(); ++i)
        buff[i] = new short [dialog->width()];

    for (unsigned short i = 0; i < dialog->height(); ++i)
        for (unsigned short j = 0; j < dialog->width(); ++j)
            buff[i][j] = 0x00;

    image.copy(dialog->width(), dialog->height(), buff);

    for (unsigned short i = 0; i < dialog->height(); ++i)
        delete[] buff[i];
    delete[] buff;

    image.setShapes(0, 0, image.width(), image.height(), dialog->radius(),
                    dialog->space(), dialog->epsilon(), dialog->shape(), dialog->fill());

    data_type = Ui::DATA_TYPE::IMG;
    QImage img(image.width(), image.height(), QImage::Format::Format_RGB32);

    levels->init(image);

    for (auto i = 0; i < image.height(); ++i)
        for (auto j = 0; j < image.width(); ++j)
            img.setPixel(j, i, qRgb(image[i][j] * (255.0f / levels->max()), image[i][j] * (255.0f / levels->max()), image[i][j] * (255.0f / levels->max())));

    //Actions disable
    ui->actionLevels->setEnabled(false);
    ui->actionSave->setEnabled(false);
    //
    //Actions enable
    ui->actionCalc->setEnabled(true);
    //

    scene->clear();
    ui->graphicsView->setScene(scene);
    scene->setSceneRect(img.rect());
    scene->addPixmap(QPixmap::fromImage(img));
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    zoom = ui->graphicsView->rect().width()*100.0/image.width();
    ui->label_zoom->setText(QString::number(zoom) + "%");
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    // Scale the view / do the zoom
    double scaleFactor = 1.15;
    switch (event->key()) {
        case (Qt::Key_Plus)://Zoom in
            if (zoom < MAX_ZOOM) {
                zoom *= scaleFactor;
                ui->graphicsView->scale(scaleFactor, scaleFactor);
            }
        break;
        case (Qt::Key_Minus)://Zoom out
            if (zoom > MIN_ZOOM) {
                zoom /= scaleFactor;
                ui->graphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
            }
        break;
        default: break;
    }
    ui->label_zoom->setText(QString::number(zoom) + "%");
}

void MainWindow::on_actionLevels_triggered()
{
    //Actions disable
    ui->actionLevels->setEnabled(false);
    //
    levels->show();
    if (levels->exec() != QDialog::Accepted)
        return;
    //Actions enable
    ui->actionCalc->setEnabled(true);
    ui->actionLevels->setEnabled(true);
    //
    QImage img(image.width(), image.height(), QImage::Format::Format_RGB32);

    double left = levels->left(),
           right = levels->right();

    for (auto i = 0; i < image.height(); ++i)
        for (auto j = 0; j < image.width(); ++j) {
            short buff = image[i][j];
            switch (buff) {
            case -2:
                img.setPixel(j, (image.height()-1) - i, qRgb(255,255,0));
                break;
            case -5:
                img.setPixel(j, (image.height()-1) - i, qRgb(255, 255/2, 255));
                break;
            case -7:
                img.setPixel(j, (image.height()-1) - i, qRgb(0, 255, 255));
                break;
            default:
                if (image[i][j] <= right && image[i][j] >= left)
                   img.setPixel(j, (image.height()-1) - i, qRgb(buff-left * (255.0f / (right-left)),
                                                                buff-left * (255.0f / (right-left)),
                                                                buff-left * (255.0f / (right-left))));
                else
                   img.setPixel(j, (image.height()-1) - i, qRgb(0, 0, 0));
            break;
            }
        }

    scene->addPixmap(QPixmap::fromImage(img));
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::on_actionCalc_triggered() {
    //Actions disable
    ui->actionCalc->setEnabled(false);
    //
    satellite::Image tmp(image);
    tmp.changeMaxMin(levels->left(), levels->right());
    var_d->setImage(&tmp);
    var_d->setImageType(data_type);
    var_d->show();
    if (var_d->exec() == QDialog::Rejected)
        return;
    //Actions enable
    ui->actionCalc->setEnabled(true);
    //
    graphic->setVar(var_d->var(), var_d->dh());
    graphic->show();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseMove) {
        QPoint viewPoint = ui->graphicsView->mapFromGlobal(QCursor::pos());
        QPointF scenePoint = ui->graphicsView->mapToScene(viewPoint);

        long long x, y;
        x = scenePoint.x();
        y = scenePoint.y();

        if (x >= 0 && x < image.width() && y >= 0 && y < image.height()) {
            ui->label_x->setVisible(true);
            ui->label_x_coord->setVisible(true);
            ui->label_y->setVisible(true);
            ui->label_y_coord->setVisible(true);
            ui->label_value->setVisible(true);
            ui->label_value_set->setVisible(true);

            ui->label_x_coord->setText(QString::number(x));
            ui->label_y_coord->setText(QString::number(y));
            ui->label_value_set->setText(QString::number(image[image.height() - (y+1)][x]));
        } else {
            ui->label_x->setVisible(false);
            ui->label_x_coord->setVisible(false);
            ui->label_y->setVisible(false);
            ui->label_y_coord->setVisible(false);
            ui->label_value->setVisible(false);
            ui->label_value_set->setVisible(false);
        }
    }
      return false;
}
