#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
}

MainWindow::~MainWindow()
{
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
        file >> passport;

        if (passport.DATA_TYPE[0] != (uint8_t)(satellite::passport::DATA_TYPE::PROECTION))
            QMessageBox::critical(this, tr("Error"), tr("Image isn't proection"));

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
        scene->addPixmap(QPixmap::fromImage(img));
        scene->setSceneRect(img.rect());
        ui->graphicsView->setScene(scene);
        return;
    }
    if (fileInfo.suffix() == "img") {
        unsigned short width, height;
        file.read(reinterpret_cast<char *>(&height), sizeof(height));
        file.read(reinterpret_cast<char *>(&width), sizeof(width));
        image.read(width, height, file);
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
        scene->addPixmap(QPixmap::fromImage(img));
        scene->setSceneRect(img.rect());
        ui->graphicsView->setScene(scene);
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
