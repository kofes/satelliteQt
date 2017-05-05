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
    data_type = Ui::DATA_TYPE::NONE;
    zoom = 0;
    MIN_ZOOM = 10;
    MAX_ZOOM = 4000;
}

MainWindow::~MainWindow()
{
    levels->close();
    delete levels;
    dialog->close();
    delete dialog;
    var_d->close();
    delete var_d;
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

    //Actions disable
    ui->actionLevels->setEnabled(false);
    ui->actionCalc->setEnabled(false);
    ui->actionShowExtremes->setEnabled(false);
    //

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

        MAX_VALUE = MIN_VALUE = 0;
        for (auto i = 0; i < image.height(); ++i)
            for (auto j = 0; j < image.width(); ++j)
                MAX_VALUE = (MAX_VALUE > image[i][j]) ? MAX_VALUE : image[i][j];

        gist.clear();
        gist.resize(MAX_VALUE-MIN_VALUE+1, std::make_pair(0, 0));

        for (int i = 0; i < image.height(); ++i)
            for (int j = 0; j < image.width(); ++j)
                if (image[i][j] > 0) {
                    gist[image[i][j]].first = image[i][j];
                    gist[image[i][j]].second++;
                }

        CENTRAL_VALUE = satellite::math::first_row_moment(gist);
        AVERAGE_DISP_VALUE = satellite::math::central_moment(gist);

        std::cout << gist.size() << std::endl;
        std::cout << CENTRAL_VALUE << " : " << std::sqrt(AVERAGE_DISP_VALUE) << std::endl;

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
                    if (std::fabs(image[i][j] - CENTRAL_VALUE) <= std::sqrt(AVERAGE_DISP_VALUE))
                       img.setPixel(j, (image.height()-1) - i, qRgb(buff-(CENTRAL_VALUE - std::sqrt(AVERAGE_DISP_VALUE)) * (255.0f / (2*std::sqrt(AVERAGE_DISP_VALUE))),
                                                                    buff-(CENTRAL_VALUE - std::sqrt(AVERAGE_DISP_VALUE)) * (255.0f / (2*std::sqrt(AVERAGE_DISP_VALUE))),
                                                                    buff-(CENTRAL_VALUE - std::sqrt(AVERAGE_DISP_VALUE)) * (255.0f / (2*std::sqrt(AVERAGE_DISP_VALUE)))));
                    else
                       img.setPixel(j, (image.height()-1) - i, qRgb(0, 0, 0));
                break;
                }
            }

        //Levels init
        levels->left(CENTRAL_VALUE - std::sqrt(AVERAGE_DISP_VALUE));
        levels->right(CENTRAL_VALUE + std::sqrt(AVERAGE_DISP_VALUE));
        levels->point_to_gist(&gist);
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
        file.read(reinterpret_cast<char *>(&height), sizeof(height));
        file.read(reinterpret_cast<char *>(&width), sizeof(width));
        image.read(width, height, file);
        data_type = Ui::DATA_TYPE::IMG;
        //
        QImage img(image.width(), image.height(), QImage::Format::Format_RGB32);

        gist.clear();
        short max = SHRT_MIN;
        for (auto i = 0; i < image.height(); ++i)
            for (auto j = 0; j < image.width(); ++j)
                max = (max > image[i][j]) ? max : image[i][j];
        for (auto i = 0; i < image.height(); ++i)
            for (auto j = 0; j < image.width(); ++j)
                img.setPixel(j, i, qRgb(image[i][j] * (255.0f / max), image[i][j] * (255.0f / max), image[i][j] * (255.0f / max)));
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

    dialog->show();
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
    QImage img(image.width(), image.height(), QImage::Format::Format_RGB32);

    gist.clear();
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

void MainWindow::on_actionLevels_triggered()
{
    levels->show();
    if (levels->exec() != QDialog::Accepted)
        return;

    CENTRAL_VALUE = (levels->left() + levels->right())/2.0;
    AVERAGE_DISP_VALUE = ((levels->right() - levels->left())/2)*((levels->right() - levels->left())/2);

    QImage img(image.width(), image.height(), QImage::Format::Format_RGB32);

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
                if (std::fabs(image[i][j] - CENTRAL_VALUE) <= std::sqrt(AVERAGE_DISP_VALUE))
                   img.setPixel(j, (image.height()-1) - i, qRgb(buff-(CENTRAL_VALUE - std::sqrt(AVERAGE_DISP_VALUE)) * (255.0f / (2*std::sqrt(AVERAGE_DISP_VALUE))),
                                                                buff-(CENTRAL_VALUE - std::sqrt(AVERAGE_DISP_VALUE)) * (255.0f / (2*std::sqrt(AVERAGE_DISP_VALUE))),
                                                                buff-(CENTRAL_VALUE - std::sqrt(AVERAGE_DISP_VALUE)) * (255.0f / (2*std::sqrt(AVERAGE_DISP_VALUE)))));
                else
                   img.setPixel(j, (image.height()-1) - i, qRgb(0, 0, 0));
            break;
            }
        }

    scene->addPixmap(QPixmap::fromImage(img));
    ui->graphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::on_actionCalc_triggered()
{
    var_d->show();
}
