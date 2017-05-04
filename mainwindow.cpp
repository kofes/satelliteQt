#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    item = new QGraphicsPixmapItem();
    data_type = Ui::DATA_TYPE::NONE;
    zoom = 0;
    MIN_ZOOM = 10;
    MAX_ZOOM = 4000;
}

MainWindow::~MainWindow()
{
    delete item;
    delete scene;
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
                                                    tr("lab34 files (*.pro);;Var files (*.var);;Generated images (*.img)"));
    if (fileName == "")
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

        if (pass.DATA_TYPE[0] != (uint8_t)(satellite::passport::DATA_TYPE::PROECTION))
            QMessageBox::critical(this, tr("Error"), tr("Image isn't proection"));

        passport = pass;

        data_type = Ui::DATA_TYPE::PRO;
        image.read(passport.getCountPixelsInLine(), passport.getCountLines(), file);

        QImage img(image.width(), image.height(), QImage::Format::Format_RGB32);

        short max = SHRT_MIN;
        for (auto i = 0; i < image.height(); ++i)
            for (auto j = 0; j < image.width(); ++j)
                max = (max > image[i][j]) ? max : image[i][j];
        for (auto i = 0; i < image.height(); ++i)
            for (auto j = 0; j < image.width(); ++j)
                img.setPixel(j, (image.height()-1) - i, qRgb(image[i][j] * (255.0f / max), image[i][j] * (255.0f / max), image[i][j] * (255.0f / max)));

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
        file.read(reinterpret_cast<char *>(&height), sizeof(height));
        file.read(reinterpret_cast<char *>(&width), sizeof(width));
        image.read(width, height, file);
        data_type = Ui::DATA_TYPE::IMG;
        //
        QImage img(image.width(), image.height(), QImage::Format::Format_RGB32);

        short max = SHRT_MIN;
        for (auto i = 0; i < image.height(); ++i)
            for (auto j = 0; j < image.width(); ++j)
                max = (max > image[i][j]) ? max : image[i][j];
        for (auto i = 0; i < image.height(); ++i)
            for (auto j = 0; j < image.width(); ++j)
                img.setPixel(j, i, qRgb(image[i][j] * (255.0f / max), image[i][j] * (255.0f / max), image[i][j] * (255.0f / max)));

        scene->clear();
        ui->graphicsView->setScene(scene);
        scene->setSceneRect(img.rect());
        scene->addPixmap(QPixmap::fromImage(img));
        ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
        zoom = ui->graphicsView->rect().width()*100.0/image.width();
        ui->label_zoom->setText(QString::number(zoom) + "%");
        return;
    }
    //TODO: VAR FILES!
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

    dialog.show();

    if (dialog.exec() == QMessageBox::Cancel)
        return;

    buff = new short* [dialog.height()];
    for (unsigned short i = 0; i < dialog.height(); ++i)
        buff[i] = new short [dialog.width()];

    for (unsigned short i = 0; i < dialog.height(); ++i)
        for (unsigned short j = 0; j < dialog.width(); ++j)
            buff[i][j] = 0x00;

    image.copy(dialog.width(), dialog.height(), buff);

    for (unsigned short i = 0; i < dialog.height(); ++i)
        delete[] buff[i];
    delete[] buff;

    image.setShapes(0, 0, image.width(), image.height(), dialog.radius(),
                    dialog.space(), dialog.epsilon(), dialog.shape(), dialog.fill());

    data_type = Ui::DATA_TYPE::IMG;
    QImage img(image.width(), image.height(), QImage::Format::Format_RGB32);

    short max = SHRT_MIN;
    for (auto i = 0; i < image.height(); ++i)
        for (auto j = 0; j < image.width(); ++j)
            max = (max > image[i][j]) ? max : image[i][j];

    for (auto i = 0; i < image.height(); ++i)
        for (auto j = 0; j < image.width(); ++j)
            img.setPixel(j, i, qRgb(image[i][j] * (255.0f / max), image[i][j] * (255.0f / max), image[i][j] * (255.0f / max)));

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
