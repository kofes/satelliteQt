#include "levels.h"
#include "ui_levels.h"

Levels::Levels(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Levels)
{
    ui->setupUi(this);
    ui->label_image->setBackgroundRole(QPalette::Light);
    ui->label_image->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->label_image->setScaledContents(true);
    _sum = 0;
}

Levels::~Levels()
{
    delete ui;
}

void Levels::init(satellite::Image& img) {
    double central_val, average_disp_val;

    _max_val = 0;
    for (auto i = 0; i < img.height(); ++i)
        for (auto j = 0; j < img.width(); ++j)
            _max_val = (_max_val > img[i][j]) ? _max_val : img[i][j];

    _gist.clear();
    _gist.resize(_max_val+1, std::make_pair(0, 0));

    for (int i = 0; i < img.height(); ++i)
        for (int j = 0; j < img.width(); ++j)
            if (img[i][j] > 0) {
                _gist[img[i][j]].first = img[i][j];
                _gist[img[i][j]].second++;
            }

    central_val = satellite::math::first_row_moment(_gist);
    average_disp_val = satellite::math::central_moment(_gist);

    _left = central_val - std::sqrt(average_disp_val);
    _right = central_val + std::sqrt(average_disp_val);
    if (_left < 0) _left = 0;
    if (_right > _max_val) _right = _max_val;

    unit = Unit::RAW;

    double max_x, max_y;
    max_x = _gist.size();
    max_y = 0;
    _sum = 0;
    for (auto elem : _gist) {
        max_y = (max_y < elem.second) ? elem.second : max_y ;
        _sum += elem.second;
    }

    pxm = QPixmap(max_x, max_y*1.10);

    dx = std::max(1.0, max_x/1000);

    QPainter painter(&pxm);

    painter.fillRect(pxm.rect(), Qt::gray);

    painter.setPen(Qt::black);
    for (auto elem : _gist)
        painter.drawRect(elem.first, pxm.height() - elem.second, 1, elem.second);

    painter.fillRect((_left+_right)/2, 0, dx, pxm.height(), Qt::red);

    painter.fillRect((_left < 0 ? 0 : _left), 0, dx, pxm.height(), Qt::green);
    painter.fillRect((_right > pxm.width() ? pxm.width() : _right), 0, dx, pxm.height(), Qt::green);

    painter.end();

    ui->radio_button_raw->click();
    on_radio_button_raw_clicked();

    ui->label_image->setPixmap(pxm);
}

short Levels::left() {
    return _left;
}

short Levels::right() {
    return _right;
}

short Levels::max() {
    return _max_val;
}

void Levels::on_radio_button_raw_clicked()
{
    unit = Unit::RAW;
    (const_cast<QValidator *>(ui->line_left_val->validator()))->deleteLater();
    (const_cast<QValidator *>(ui->line_right_val->validator()))->deleteLater();
    ui->line_left_val->setValidator(new QIntValidator(0, _gist.size(), this));
    ui->line_right_val->setValidator(new QIntValidator(0, _gist.size(), this));
    ui->line_left_val->setText(QString::number(std::trunc(_left)));
    ui->line_right_val->setText(QString::number(std::trunc(_right)));
}

void Levels::on_radio_button_quantile_clicked()
{
    unit = Unit::QUANTILE;
    (const_cast<QValidator *>(ui->line_left_val->validator()))->deleteLater();
    (const_cast<QValidator *>(ui->line_right_val->validator()))->deleteLater();
    ui->line_left_val->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->line_right_val->setValidator(new QDoubleValidator(0, 100, 2, this));
    ui->line_left_val->setText(QString::number(sum_before(_left)/_sum*100, 'f', 2));
    ui->line_right_val->setText(QString::number(sum_before(_right)/_sum*100, 'f', 2));
}

double Levels::sum_before(short& end) {
    double res = 0;

    for (auto elem : _gist)
        if (elem.first <= end)
            res += elem.second;
        else
            break;

    return res;

}

unsigned long Levels::ind_before(double& sum) {
    double curr = 0;
    unsigned long res = 0;

    for (unsigned long i = 0; i < _gist.size(); ++i) {
        if (curr + _gist[i].second <= sum)
            res = i;
        else {
            sum = curr;
            break;
        }
        curr += _gist[i].second;
    }

    return res;
}

void Levels::on_line_left_val_editingFinished()
{
    uncheck_radio();
    QString arg1 = ui->line_left_val->text();
    QPainter painter(&pxm);

    for (long i = 0; i < dx; ++i) {
        painter.fillRect((_left < 0 ? 0 : _left)+i, 0, 1, pxm.height(), Qt::gray);
        painter.fillRect((_left < 0 ? 0 : _left)+i, pxm.height() - _gist[(_left < 0 ? 0 : _left)+i].second, 1, _gist[(_left < 0 ? 0 : _left)+i].second, Qt::black);

        painter.fillRect((_left+_right)/2+i, 0, 1, pxm.height(), Qt::gray);
        painter.fillRect((_left+_right)/2+i, pxm.height() - _gist[(_left+_right)/2+i].second, 1, _gist[(_left+_right)/2+i].second, Qt::black);
    }

    double buff;
    switch (unit) {
        case (Unit::RAW):
            _left = arg1.toLong();
            std::cout << _left << std::endl;
        break;
        case (Unit::QUANTILE):
            buff = arg1.toDouble();
            buff *= _sum/100;
            _left = ind_before(buff);
            ui->line_left_val->setText(QString::number(buff/_sum*100, 'f', 2));
        break;
    }

    painter.fillRect((_left+_right)/2, 0, dx, pxm.height(), Qt::red);

    painter.fillRect((_left < 0 ? 0 : _left), 0, dx, pxm.height(), Qt::green);
    painter.fillRect((_right > pxm.width() ? pxm.width() : _right), 0, dx, pxm.height(), Qt::green);

    painter.end();
    ui->label_image->setPixmap(pxm);
}

void Levels::on_line_right_val_editingFinished()
{
    uncheck_radio();
    QString arg1 = ui->line_right_val->text();
    QPainter painter(&pxm);

    for (long i = 0; i < dx; ++i) {
        painter.fillRect((_right > pxm.width() ? pxm.width() : _right)+i, 0, 1, pxm.height(), Qt::gray);
        painter.fillRect((_right > pxm.width() ? pxm.width() : _right)+i, pxm.height() - _gist[(_right > pxm.width() ? pxm.width() : _right)+i].second, 1, _gist[(_right > pxm.width() ? pxm.width() : _right)+i].second, Qt::black);

        painter.fillRect((_left+_right)/2+i, 0, 1, pxm.height(), Qt::gray);
        painter.fillRect((_left+_right)/2+i, pxm.height() - _gist[(_left+_right)/2+i].second, 1, _gist[(_left+_right)/2+i].second, Qt::black);
    }

    double buff;
    switch (unit) {
        case (Unit::RAW):
            _right = arg1.toLong();
            std::cout << _right << std::endl;
        break;
        case (Unit::QUANTILE):
            buff = arg1.toDouble();
            buff *= _sum/100;
            _right = ind_before(buff);
            ui->line_right_val->setText(QString::number(buff/_sum*100, 'f', 2));
        break;
    }

    painter.fillRect((_left+_right)/2, 0, dx, pxm.height(), Qt::red);

    painter.fillRect((_left < 0 ? 0 : _left), 0, dx, pxm.height(), Qt::green);
    painter.fillRect((_right > pxm.width() ? pxm.width() : _right), 0, dx, pxm.height(), Qt::green);

    painter.end();
    ui->label_image->setPixmap(pxm);
}

void Levels::on_buttonBox_clicked(QAbstractButton *button)
{
    if ( ui->buttonBox->standardButton(button) == QDialogButtonBox::RestoreDefaults) {
        double center, disp;
        QPainter painter(&pxm);

        for (long i = 0; i < dx; ++i) {
            painter.fillRect((_left < 0 ? 0 : _left)+i, 0, 1, pxm.height(), Qt::gray);
            painter.fillRect((_left < 0 ? 0 : _left)+i, pxm.height() - _gist[(_left < 0 ? 0 : _left)+i].second, 1, _gist[(_left < 0 ? 0 : _left)+i].second, Qt::black);

            painter.fillRect((_right > pxm.width() ? pxm.width() : _right)+i, 0, 1, pxm.height(), Qt::gray);
            painter.fillRect((_right > pxm.width() ? pxm.width() : _right)+i, pxm.height() - _gist[(_right > pxm.width() ? pxm.width() : _right)+i].second, 1, _gist[(_right > pxm.width() ? pxm.width() : _right)+i].second, Qt::black);

            painter.fillRect((_left+_right)/2+i, 0, 1, pxm.height(), Qt::gray);
            painter.fillRect((_left+_right)/2+i, pxm.height() - _gist[(_left+_right)/2+i].second, 1, _gist[(_left+_right)/2+i].second, Qt::black);
        }

        center = satellite::math::first_row_moment(_gist);
        disp = satellite::math::central_moment(_gist);

        _left = center - std::sqrt(disp);
        _right = center + std::sqrt(disp);

        switch (unit) {
            case (Unit::RAW):
                ui->line_left_val->setText(QString::number(std::trunc(_left)));
                ui->line_right_val->setText(QString::number(std::trunc(_right)));
            break;
            case (Unit::QUANTILE):
                ui->line_left_val->setText(QString::number(sum_before(_left)/_sum*100, 'f', 2));
                ui->line_right_val->setText(QString::number(sum_before(_right)/_sum*100, 'f', 2));
            break;
        }

        painter.fillRect((_left+_right)/2, 0, dx, pxm.height(), Qt::red);

        painter.fillRect((_left < 0 ? 0 : _left), 0, dx, pxm.height(), Qt::green);
        painter.fillRect((_right > pxm.width() ? pxm.width() : _right), 0, dx, pxm.height(), Qt::green);

        painter.end();

        uncheck_radio();

        ui->label_image->setPixmap(pxm);
    }
}

void Levels::on_radio_button_snow_clicked()
{
    double E, D,
           minus_m, plus_m,
           minus_d, plus_d;

    E = satellite::math::first_row_moment(_gist);
    D = satellite::math::central_moment(_gist);

    minus_m = 0;
    plus_m = std::trunc(E);

    for (unsigned long i = 0; i < std::trunc(E); ++i)
        minus_m = (_gist[minus_m].second > _gist[i].second) ? minus_m : i;

    for (unsigned short i = std::trunc(E); i < _gist.size(); ++i)
      plus_m = (_gist[plus_m].second > _gist[i].second) ? plus_m : i;

    minus_m = _gist[minus_m].first;
    minus_d = satellite::math::moment(_gist, minus_m, 2);

    plus_m = _gist[plus_m].first;
    plus_d = satellite::math::moment(_gist, plus_m, 2);

    if (D < minus_d + plus_d) {
      minus_d = minus_d * ( D / (minus_d + plus_d));
      plus_d = D - minus_d;
    }

    QPainter painter(&pxm);

    for (long i = 0; i < dx; ++i) {
        painter.fillRect((_left < 0 ? 0 : _left)+i, 0, 1, pxm.height(), Qt::gray);
        painter.fillRect((_left < 0 ? 0 : _left)+i, pxm.height() - _gist[(_left < 0 ? 0 : _left)+i].second, 1, _gist[(_left < 0 ? 0 : _left)+i].second, Qt::black);

        painter.fillRect((_right > pxm.width() ? pxm.width() : _right)+i, 0, 1, pxm.height(), Qt::gray);
        painter.fillRect((_right > pxm.width() ? pxm.width() : _right)+i, pxm.height() - _gist[(_right > pxm.width() ? pxm.width() : _right)+i].second, 1, _gist[(_right > pxm.width() ? pxm.width() : _right)+i].second, Qt::black);

        painter.fillRect((_left+_right)/2+i, 0, 1, pxm.height(), Qt::gray);
        painter.fillRect((_left+_right)/2+i, pxm.height() - _gist[(_left+_right)/2+i].second, 1, _gist[(_left+_right)/2+i].second, Qt::black);
    }

    _left = minus_m - std::sqrt(minus_d);
    _right = minus_m + std::sqrt(minus_d);

    switch (unit) {
        case (Unit::RAW):
            ui->line_left_val->setText(QString::number(std::trunc(_left)));
            ui->line_right_val->setText(QString::number(std::trunc(_right)));
        break;
        case (Unit::QUANTILE):
            ui->line_left_val->setText(QString::number(sum_before(_left)/_sum*100, 'f', 2));
            ui->line_right_val->setText(QString::number(sum_before(_right)/_sum*100, 'f', 2));
        break;
    }

    painter.fillRect((_left+_right)/2, 0, dx, pxm.height(), Qt::red);

    painter.fillRect((_left < 0 ? 0 : _left), 0, dx, pxm.height(), Qt::green);
    painter.fillRect((_right > pxm.width() ? pxm.width() : _right), 0, dx, pxm.height(), Qt::green);

    painter.end();
    ui->label_image->setPixmap(pxm);
}

void Levels::on_radio_button_cloud_clicked()
{
    double E, D,
           minus_m, plus_m,
           minus_d, plus_d;

    E = satellite::math::first_row_moment(_gist);
    D = satellite::math::central_moment(_gist);

    minus_m = 0;
    plus_m = std::trunc(E);

    for (unsigned long i = 0; i < std::trunc(E); ++i)
        minus_m = (_gist[minus_m].second > _gist[i].second) ? minus_m : i;

    for (unsigned short i = std::trunc(E); i < _gist.size(); ++i)
      plus_m = (_gist[plus_m].second > _gist[i].second) ? plus_m : i;

    minus_m = _gist[minus_m].first;
    minus_d = satellite::math::moment(_gist, minus_m, 2);

    plus_m = _gist[plus_m].first;
    plus_d = satellite::math::moment(_gist, plus_m, 2);

    if (D < minus_d + plus_d) {
      minus_d = minus_d * ( D / (minus_d + plus_d));
      plus_d = D - minus_d;
    }

    QPainter painter(&pxm);

    for (long i = 0; i < dx; ++i) {
        painter.fillRect((_left < 0 ? 0 : _left)+i, 0, 1, pxm.height(), Qt::gray);
        painter.fillRect((_left < 0 ? 0 : _left)+i, pxm.height() - _gist[(_left < 0 ? 0 : _left)+i].second, 1, _gist[(_left < 0 ? 0 : _left)+i].second, Qt::black);

        painter.fillRect((_right > pxm.width() ? pxm.width() : _right)+i, 0, 1, pxm.height(), Qt::gray);
        painter.fillRect((_right > pxm.width() ? pxm.width() : _right)+i, pxm.height() - _gist[(_right > pxm.width() ? pxm.width() : _right)+i].second, 1, _gist[(_right > pxm.width() ? pxm.width() : _right)+i].second, Qt::black);

        painter.fillRect((_left+_right)/2+i, 0, 1, pxm.height(), Qt::gray);
        painter.fillRect((_left+_right)/2+i, pxm.height() - _gist[(_left+_right)/2+i].second, 1, _gist[(_left+_right)/2+i].second, Qt::black);
    }

    _left = plus_m - std::sqrt(plus_d);
    _right = plus_m + std::sqrt(plus_d);

    switch (unit) {
        case (Unit::RAW):
            ui->line_left_val->setText(QString::number(std::trunc(_left)));
            ui->line_right_val->setText(QString::number(std::trunc(_right)));
        break;
        case (Unit::QUANTILE):
            ui->line_left_val->setText(QString::number(sum_before(_left)/_sum*100, 'f', 2));
            ui->line_right_val->setText(QString::number(sum_before(_right)/_sum*100, 'f', 2));
        break;
    }

    painter.fillRect((_left+_right)/2, 0, dx, pxm.height(), Qt::red);

    painter.fillRect((_left < 0 ? 0 : _left), 0, dx, pxm.height(), Qt::green);
    painter.fillRect((_right > pxm.width() ? pxm.width() : _right), 0, dx, pxm.height(), Qt::green);

    painter.end();
    ui->label_image->setPixmap(pxm);
}

void Levels::uncheck_radio() {
    ui->radio_button_cloud->setAutoExclusive(false);
    ui->radio_button_snow->setAutoExclusive(false);
    ui->radio_button_cloud->setChecked(false);
    ui->radio_button_snow->setChecked(false);
    ui->radio_button_cloud->setAutoExclusive(true);
    ui->radio_button_snow->setAutoExclusive(true);
}
