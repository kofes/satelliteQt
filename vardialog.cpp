#include "vardialog.h"
#include "ui_vardialog.h"

varDialog::varDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::varDialog)
{
    ui->setupUi(this);
}

varDialog::~varDialog()
{
    delete ui;
}

void varDialog::setImage(satellite::Image* src) {
    image = src;
    ui->line_log->setValidator(new QDoubleValidator(1, std::max(image->width(), image->height()), 2, this));
    ui->line_log->setText(QString::number(0.1, 'f', 2));

    ui->line_top_left_x->setValidator(new QIntValidator(0, image->width()-1, this));
    ui->line_top_left_x->setText(QString::number(0));

    ui->line_bottom_right_x->setValidator(new QIntValidator(1, image->width(), this));
    ui->line_bottom_right_x->setText(QString::number(image->width()));

    ui->line_top_left_y->setValidator(new QIntValidator(0, image->height()-1, this));
    ui->line_top_left_y->setText(QString::number(0));

    ui->line_bottom_right_y->setValidator(new QIntValidator(1, image->height(), this));
    ui->line_bottom_right_y->setText(QString::number(image->height()));

    func.clear();
    ui->progressBar->setValue(0);
}

double varDialog::dh() {
    return ui->line_log->text().toDouble();
}

std::vector<double> varDialog::var() {
    return func;
}

void varDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if (ui->buttonBox->standardButton(button) == QDialogButtonBox::Ok) {
        double h = 0, dh = ui->line_log->text().toDouble();
        unsigned short width = std::abs(ui->line_top_left_x->text().toDouble() - ui->line_bottom_right_x->text().toDouble()),
                       height = std::abs(ui->line_top_left_y->text().toDouble() - ui->line_bottom_right_y->text().toDouble());
        while (h < std::sqrt(width*width + height*height)) {
            func.push_back(satellite::math::g(
                               ui->line_top_left_x->text().toShort(),
                               ui->line_top_left_y->text().toShort(),
                               ui->line_bottom_right_x->text().toShort(),
                               ui->line_bottom_right_y->text().toShort(),
                               h,
                               *image
                               ));
            h += dh;
            ui->progressBar->setValue(h/std::sqrt(width*width+height*height) * 100);
        }
    }
}
