#include "vardialog.h"
#include "ui_vardialog.h"

varDialog::varDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::varDialog) {
    ui->setupUi(this);
    connect(&this->fw, SIGNAL(finished()), this, SLOT(slot_finished()));
    connect(&this->cl, SIGNAL(signal_progress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(this, SIGNAL(abort(bool)), &this->cl, SLOT(abort(bool)));
    ui->buttonBox->button(QDialogButtonBox::Abort)->setEnabled(false);
    _cancel_flag = false;
}

varDialog::~varDialog() {
    delete ui;
}

void varDialog::setImage(satellite::Image* src) {
    image = src;
    ui->line_log->setValidator(new QDoubleValidator(1, std::max(image->width()-1, image->height()-1), 2, this));
    ui->line_log->setText(QString::number(0.1, 'f', 2));

    ui->line_top_left_x->setValidator(new QIntValidator(0, image->width()-1, this));
    ui->line_top_left_x->setText(QString::number(0));

    ui->line_bottom_right_x->setValidator(new QIntValidator(1, image->width()-1, this));
    ui->line_bottom_right_x->setText(QString::number(image->width()-1));

    ui->line_top_left_y->setValidator(new QIntValidator(0, image->height()-1, this));
    ui->line_top_left_y->setText(QString::number(0));

    ui->line_bottom_right_y->setValidator(new QIntValidator(1, image->height()-1, this));
    ui->line_bottom_right_y->setText(QString::number(image->height()-1));

    func.clear();
    ui->progressBar->setValue(0);
}

void varDialog::setImageType(Ui::DATA_TYPE type) {
    _type = type;
}

double varDialog::dh() {
    return ui->line_log->text().toDouble();
}

std::vector<double> varDialog::var() {
    return func;
}

void varDialog::on_buttonBox_clicked(QAbstractButton *button) {
    if (ui->buttonBox->standardButton(button) == QDialogButtonBox::Apply) {

        button->setCheckable(false);
        unsigned short width = std::abs(ui->line_top_left_x->text().toDouble() - ui->line_bottom_right_x->text().toDouble()),
                       height = std::abs(ui->line_top_left_y->text().toDouble() - ui->line_bottom_right_y->text().toDouble());
        Calc_type type;
        std::cout << "-----------------" << '\n'
                  << "x: " << ui->line_top_left_x->text().toDouble() << '\n'
                  << "y: " << ui->line_top_left_y->text().toDouble() << '\n'
                  << "w: " << width << '\n'
                  << "h: " << height << '\n'
                  << "t: " << ((_type == Ui::DATA_TYPE::IMG) ? "img" : "pro") << '\n'
                  << "-----------------"
                  << std::endl;
        if (ui->comboBox_type->currentText() == "Covariance")
            type = Calc_type::COVARIANCE;
        else if (ui->comboBox_type->currentText() == "Variogram")
            type = Calc_type::VARIOGRAM;
        cl.set(
                    &func,
                    ui->line_top_left_x->text().toDouble(),
                    (_type == Ui::DATA_TYPE::PRO) ?
                        image->height() - ui->line_bottom_right_y->text().toDouble()-1
                      :
                        ui->line_top_left_y->text().toDouble(),
                    width,
                    height,
                    ui->line_log->text().toDouble(),
                    image,
                    type
               );
        QFuture<void> future = QtConcurrent::run(&this->cl, &Calculation::operator ());
        fw.setFuture(future);
        reset(false);
        return;
    }
    if (ui->buttonBox->standardButton(button) == QDialogButtonBox::Cancel)
        _cancel_flag = true;
    reset(true);
    abort(true);
}

void varDialog::slot_finished() {
    ui->progressBar->setValue(0);
    if (!ui->buttonBox->button(QDialogButtonBox::Apply)->isEnabled()) {
        reset(true);
        this->done(QDialogButtonBox::Ok);
    }
}

void varDialog::on_buttonBox_rejected()
{
    if (_cancel_flag) {
        _cancel_flag = false;
        fw.waitForFinished();
        reject();
    }
}

void varDialog::reset(bool flag) {
    ui->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(flag);
    ui->buttonBox->button(QDialogButtonBox::Abort)->setEnabled(!flag);
    ui->comboBox_type->setEnabled(flag);
    ui->line_bottom_right_x->setEnabled(flag);
    ui->line_bottom_right_y->setEnabled(flag);
    ui->line_log->setEnabled(flag);
    ui->line_top_left_x->setEnabled(flag);
    ui->line_top_left_y->setEnabled(flag);
}
