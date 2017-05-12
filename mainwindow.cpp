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
    binary = false;

    set_visible_coords(false);

    this->setWindowTitle(appName);
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
                                                    tr("lab 34 files (*.pro);;Var files (*.var);;Generated images (*.img)"));
    if (fileName.isEmpty())
        return;
    qDebug() << fileName;
    std::ifstream file(fileName.toLocal8Bit().toStdString(), std::ifstream::binary);
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

        _fileName = fileInfo.fileName();
        this->setWindowTitle(appName + " - " + _fileName);

        passport = pass;

        image.read(passport.getCountPixelsInLine(), passport.getCountLines(), file);

        levels->init(image);
        QImage img(image.width(), image.height(), QImage::Format::Format_RGB32);

        std::cout << levels->max() << '\n'
                  << (levels->right()+levels->left())/2 << " : "
                  << (levels->right()-levels->left())/2
                  << std::endl;
        if (levels->max() == levels->min())
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
                        if (buff >= 0)
                            img.setPixel(j, (image.height()-1) - i, qRgb(buff * (255.0f / levels->max()),
                                                                         buff * (255.0f / levels->max()),
                                                                         buff * (255.0f / levels->max())));
                    break;
                    }
                }
        else {
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
        }
        //Actions enable
        ui->actionSave->setEnabled(true);
        if (levels->max() != levels->min())
            ui->actionLevels->setEnabled(true);
        else
            ui->actionLevels->setEnabled(false);
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

        _fileName = fileInfo.fileName();
        this->setWindowTitle(appName + " - " + _fileName);

        file.read(reinterpret_cast<char *>(&height), sizeof(height));
        file.read(reinterpret_cast<char *>(&width), sizeof(width));
        image.read(width, height, file);
        levels->init(image);
        //
        QImage img(image.width(), image.height(), QImage::Format::Format_RGB32);

        for (auto i = 0; i < image.height(); ++i)
            for (auto j = 0; j < image.width(); ++j)
                img.setPixel(j, i, qRgb(image[i][j] * (255.0f / levels->max()), image[i][j] * (255.0f / levels->max()), image[i][j] * (255.0f / levels->max())));

        //Actions enable
        ui->actionSave->setEnabled(true);
        ui->actionLevels->setEnabled(false);
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

void MainWindow::on_actionQuit_triggered() {
    QMessageBox messageBox;
    messageBox.setWindowTitle(tr("satellite"));
    messageBox.setText(tr("Do you teally want to quit?"));
    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox.setDefaultButton(QMessageBox::No);
    if (messageBox.exec() == QMessageBox::Yes)
        QApplication::quit();

}

void MainWindow::on_actionCreate_template_triggered() {
    short** buff;

    if (dialog->exec() != QDialog::Accepted)
        return;

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

    _fileName.clear();
    this->setWindowTitle(appName);

    QImage img(image.width(), image.height(), QImage::Format::Format_RGB32);

    levels->init(image);

    for (auto i = 0; i < image.height(); ++i)
        for (auto j = 0; j < image.width(); ++j)
            img.setPixel(j, i, qRgb(image[i][j] * (255.0f / levels->max()), image[i][j] * (255.0f / levels->max()), image[i][j] * (255.0f / levels->max())));

    //Actions enable
    ui->actionSave->setEnabled(true);
    ui->actionLevels->setEnabled(false);
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

void MainWindow::on_actionLevels_triggered() {
    if (levels->exec() != QDialog::Accepted)
        return;
    binary = levels->binary();
    this->setWindowTitle(appName + " - " + _fileName + "*");

    QImage img(image.width(), image.height(), QImage::Format::Format_RGB32);

    double left = levels->left(),
           right = levels->right();
    if (levels->max() == levels->min())
        left = 0;

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
                if (image[i][j] <= right && image[i][j] >= left) {
                   if (binary)
                       img.setPixel(j, (image.height()-1)-i, qRgb(255, 255, 255));
                   else
                       img.setPixel(j, (image.height()-1) - i, qRgb(buff-left * (255.0f / (right-left)),
                                                                    buff-left * (255.0f / (right-left)),
                                                                    buff-left * (255.0f / (right-left))));
                } else
                   img.setPixel(j, (image.height()-1) - i, qRgb(0, 0, 0));
            break;
            }
        }

    scene->addPixmap(QPixmap::fromImage(img));
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::on_actionCalc_triggered() {
    satellite::Image tmp(image);
    if (data_type == Ui::DATA_TYPE::PRO)
        tmp.cropColor(levels->left(), levels->right());
    var_d->setImage(&tmp);
    var_d->setImageType(data_type);
    if (var_d->exec() == QDialog::Rejected)
        return;
    graphic->setVar(var_d->var(), var_d->dh());
    graphic->show();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::MouseMove && watched->objectName() == "MainWindowWindow") {
        QPoint viewPoint = ui->graphicsView->mapFromGlobal(QCursor::pos());
        QPointF scenePoint = ui->graphicsView->mapToScene(viewPoint);

        long long x, y;
        x = scenePoint.x();
        y = scenePoint.y();

        if (x >= 0 && x < image.width() && y >= 0 && y < image.height()) {
            set_visible_coords(true);

            ui->label_x_coord->setText(QString::number(x));
            ui->label_y_coord->setText(QString::number(y));
            ui->label_value_set->setText(QString::number(image[image.height() - (y+1)][x]));
        } else
            set_visible_coords(false);
    }
      return false;
}

void MainWindow::set_visible_coords(bool flag) {
    ui->label_x->setVisible(flag);
    ui->label_x_coord->setVisible(flag);
    ui->label_y->setVisible(flag);
    ui->label_y_coord->setVisible(flag);
    ui->label_value->setVisible(flag);
    ui->label_value_set->setVisible(flag);
}

void MainWindow::on_actionSave_triggered() {
    QString fileName;
    switch (data_type) {
        case Ui::DATA_TYPE::IMG:
            fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "",
                                                  tr("Generated images (*.img)"));
        break;
        case Ui::DATA_TYPE::PRO:
            fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "",
                                                  tr("lab 34 files (*.pro)"));
        break;
        default: return;
    }

    if (fileName.isEmpty())
        return;

    std::ofstream file(fileName.toStdString(), std::ofstream::binary);

    if (!file.is_open()) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
        return;
    }

    QFileInfo fileInfo(fileName);
    _fileName = fileInfo.fileName();
    this->setWindowTitle(appName + " - " + _fileName);

    unsigned short height = image.height(),
                   width = image.width();

    if (data_type == Ui::DATA_TYPE::PRO)
        image.cropColor(levels->left(), levels->right());
    if (binary) image.binary(1);

    levels->init(image);

    switch (data_type) {
    case Ui::DATA_TYPE::IMG:
        file.write((char*)(&(height)), sizeof(height));
        file.write((char*)(&(width)), sizeof(width));
        file < image;
    break;
    case Ui::DATA_TYPE::PRO:

        file < passport;
        file < image;
    break;
    default:
        break;
    }
}
