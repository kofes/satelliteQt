#include "createimage.h"
#include "ui_createimage.h"

CreateImage::CreateImage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateImage) {
    ui->setupUi(this);
}

CreateImage::~CreateImage() {
    delete ui;
}

void CreateImage::on_buttonBox_accepted() {
    _width = ui->spinBox_width->value();
    _height = ui->spinBox_height->value();
    _space = ui->spinBox_space->value();
    _radius = ui->spinBox_radius->value();

    if (ui->checkBox_filled->isChecked())
        _filled = satellite::ShapeFill::SOLID;
    else
        _filled = satellite::ShapeFill::DEFAULT;

    if (ui->comboBox_shape->currentText() == "Circle")
        _type_shape = satellite::Shape::CIRCLE;
    else
        _type_shape = satellite::Shape::SQUARE;
    _epsilon = ui->doubleSpinBox->value();
}

void CreateImage::on_buttonBox_rejected()
{

}
