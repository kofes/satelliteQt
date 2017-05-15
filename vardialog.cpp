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
    log_valid = new QIntValidator(this);
}

varDialog::~varDialog() {
    fw.cancel();
    fw.waitForFinished();
    delete log_valid;
    delete ui;
}

void varDialog::setImage(satellite::Image* src) {
    image = src;

    _max_x = image->width()-1;
    _max_y = image->height()-1;

    log_valid->setBottom(1);
    log_valid->setTop(std::max(image->width()-1, image->height()-1));

    ui->line_log->setValidator(log_valid);
    ui->line_log->setText(QString::number(1));

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

int varDialog::dh() {
    return ui->line_log->text().toInt();
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
                    ui->line_log->text().toInt(),
                    image,
                    type
               );
        fw.cancel();
        fw.waitForFinished();
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

void varDialog::on_buttonBox_rejected() {
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

void varDialog::on_line_top_left_x_editingFinished() {
    short x0 = ui->line_top_left_x->text().toShort(),
          x1 = ui->line_bottom_right_x->text().toShort();
    if (x0 >= x1) x1 = x0+1;
    if (x1 > _max_x) x0 = (x1 = _max_x)-1;
    ui->line_top_left_x->setText(QString::number(x0));
    ui->line_bottom_right_x->setText(QString::number(x1));
}

void varDialog::on_line_bottom_right_x_editingFinished() {
    short x0 = ui->line_top_left_x->text().toShort(),
          x1 = ui->line_bottom_right_x->text().toShort();
    if (x1 <= x0) x0 = x1-1;
    if (x0 < 0) x1 = (x0 = 0)+1;
    ui->line_top_left_x->setText(QString::number(x0));
    ui->line_bottom_right_x->setText(QString::number(x1));
}

void varDialog::on_line_top_left_y_editingFinished() {
    short y0 = ui->line_top_left_y->text().toShort(),
          y1 = ui->line_bottom_right_y->text().toShort();
    if (y0 >= y1) y1 = y0+1;
    if (y1 > _max_y) y0 = (y1 = _max_y)-1;
    ui->line_top_left_y->setText(QString::number(y0));
    ui->line_bottom_right_y->setText(QString::number(y1));
}

void varDialog::on_line_bottom_right_y_editingFinished() {
    short y0 = ui->line_top_left_y->text().toShort(),
          y1 = ui->line_bottom_right_y->text().toShort();
    if (y1 <= y0) y0 = y1-1;
    if (y0 < 0) y1 = (y0 = 1)+1;
    ui->line_top_left_y->setText(QString::number(y0));
    ui->line_bottom_right_y->setText(QString::number(y1));
}
