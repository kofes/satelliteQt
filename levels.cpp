#include "levels.h"
#include "ui_levels.h"

Levels::Levels(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Levels) {
    ui->setupUi(this);
    ui->label_image->setBackgroundRole(QPalette::Light);
    ui->label_image->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->label_image->setScaledContents(true);
    ui->checkBox_binary->setChecked(false);
    _sum = 0;
}

Levels::~Levels()
{
    delete ui;
}

void Levels::init(satellite::Image& img) {
    _max_val = 0;
    _min_val = SHRT_MAX;
    for (auto i = 0; i < img.height(); ++i)
        for (auto j = 0; j < img.width(); ++j) {
            _max_val = (_max_val > img[i][j]) ? _max_val : img[i][j];
            _min_val = (_min_val > img[i][j] && img[i][j] > 0) ? img[i][j] : _min_val;
        }
    std::cout << "MIN_VAL: " << _min_val << '\n'
              << "MAX_VAL: " << _max_val
              << std::endl;
    _gist.clear();
    _gist.resize(_max_val-_min_val+1, std::make_pair(0, 0));

    for (int i = 0; i < img.height(); ++i)
        for (int j = 0; j < img.width(); ++j)
            if (img[i][j] > 0) {
                _gist[img[i][j]-_min_val].first = img[i][j];
                _gist[img[i][j]-_min_val].second++;
            }

    _m = satellite::math::first_row_moment(_gist);
    _d = satellite::math::central_moment(_gist);

    _left = _m - std::sqrt(_d);
    _right = _m + std::sqrt(_d);
    if (_left < _min_val) _left = _min_val;
    if (_right > _max_val) _right = _max_val;

    _tmp_left = _left;
    _tmp_right = _right;

    unit = Unit::RAW;
    _binary = false;

    double max_x, max_y;
    max_x = _gist.size();
    max_y = 0;
    _sum_before.clear();
    _sum = 0;
    for (auto elem : _gist) {
        max_y = (max_y < elem.second) ? elem.second : max_y ;
        _sum_before.push_back(_sum);
        _sum += elem.second;
    }
    _sum_before.push_back(_sum);
    //Otsu
    std::pair<size_t, double> res;
    res = satellite::math::threshold_Otsu(_gist);
    _threshold = res.first;
    std::cout << "Threshold: " << res.first + _min_val << '\n'
              << "SC: " << res.second
              << std::endl;
    //
    //Snow/Clouds

    _left_m = satellite::math::first_row_moment(_gist, 1, 0, res.first);
    _left_d = (_threshold + _min_val - _left_m)*(_threshold + _min_val - _left_m);
    _right_m = satellite::math::first_row_moment(_gist, 1, res.first, _gist.size());
    _right_d = (_threshold + _min_val - _right_m)*(_threshold + _min_val - _right_m);
    std::cout << "left_m: " << _left_m << '\n'
              << "left_d: " << std::sqrt(_left_d) << '\n'
              << "right_m: " << _right_m << '\n'
              << "right_d: " << std::sqrt(_right_d) << '\n'
              << std::endl;
    //
    pxm = QPixmap(max_x, max_y*1.10);

    dx = std::max(1.0, max_x/1000);

    QPainter painter(&pxm);

    painter.fillRect(pxm.rect(), Qt::gray);

    painter.setPen(Qt::black);
    for (size_t i = 0; i < _gist.size(); ++i)
        painter.drawRect(i, pxm.height() - _gist[i].second, 1, _gist[i].second);

    painter.fillRect(std::trunc(_left-_min_val), 0, dx, pxm.height(), Qt::green);
    painter.fillRect(std::trunc(_right-_min_val), 0, dx, pxm.height(), Qt::green);
    painter.fillRect(std::trunc((_left+_right)/2-_min_val), 0, dx, pxm.height(), Qt::red);

    painter.end();

    ui->radio_button_raw->click();
    on_radio_button_raw_clicked();
    ui->radio_button_default->setChecked(true);
    ui->checkBox_binary->setChecked(false);

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

short Levels::min() {
    return _min_val;
}

bool Levels::binary() {
    return _binary;
}

void Levels::on_radio_button_raw_clicked()
{
    unit = Unit::RAW;
    (const_cast<QValidator *>(ui->line_left_val->validator()))->deleteLater();
    (const_cast<QValidator *>(ui->line_right_val->validator()))->deleteLater();
    ui->line_left_val->setValidator(new QIntValidator(_min_val, _max_val, this));
    ui->line_right_val->setValidator(new QIntValidator(_min_val, _max_val, this));
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
    ui->line_left_val->setText(QString::number(_sum_before[std::trunc(_left-_min_val)]/_sum*100, 'f', 2));
    ui->line_right_val->setText(QString::number(_sum_before[std::trunc(_right-_min_val)]/_sum*100, 'f', 2));
}

double Levels::sum_before(short& end) {
    double res = 0;

    for (auto elem : _gist)
        if (elem.first-_min_val <= end)
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

void Levels::on_line_left_val_editingFinished() {
    uncheck_radio();
    QString arg1 = ui->line_left_val->text();
    QPainter painter(&pxm);

    painter.fillRect(std::trunc(_left-_min_val), 0, dx, pxm.height(), Qt::gray);
    painter.fillRect(std::trunc(_right-_min_val), 0, dx, pxm.height(), Qt::gray);
    painter.fillRect(std::trunc((_left+_right)/2-_min_val), 0, dx, pxm.height(), Qt::gray);
    for (long i = 0; i < dx; ++i) {
        painter.fillRect(std::trunc(_left-_min_val)+i,            pxm.height() - _gist[std::trunc(_left-_min_val)+i].second,            1, _gist[std::trunc(_left-_min_val)+i].second, Qt::black);
        painter.fillRect(std::trunc(_right-_min_val)+i,           pxm.height() - _gist[std::trunc(_right-_min_val)+i].second,           1, _gist[std::trunc(_right-_min_val)+i].second, Qt::black);
        painter.fillRect(std::trunc((_left+_right)/2-_min_val)+i, pxm.height() - _gist[std::trunc((_left+_right)/2-_min_val)+i].second, 1, _gist[std::trunc((_left+_right)/2-_min_val)+i].second, Qt::black);
    }

    double buff;
    switch (unit) {
        case (Unit::RAW):
            _left = arg1.toLong();
            std::cout << _left << std::endl;

            if (_left >= _right) _right = _left+1;
            if (_right > _max_val) _left = (_right = _max_val)-1;

            ui->line_left_val->setText(QString::number(std::trunc(_left)));
            ui->line_right_val->setText(QString::number(std::trunc(_right)));
        break;
        case (Unit::QUANTILE):
            buff = arg1.toDouble();
            buff *= _sum/100;
            _left = _gist[ind_before(buff)].first;//TODO: sum_before_bisect

            if (_left > _right) _right = _left+1;
            if (_right > _max_val) _left = (_right = _max_val)-1;

            buff = _sum_before[std::trunc(_left-_min_val+1)];
            ui->line_left_val->setText(QString::number(buff/_sum*100, 'f', 2));
            buff = _sum_before[std::trunc(_right-_min_val+1)];
            ui->line_right_val->setText(QString::number(buff/_sum*100, 'f', 2));
        break;
    }

    painter.fillRect(std::trunc(_left-_min_val), 0, dx, pxm.height(), Qt::green);
    painter.fillRect(std::trunc(_right-_min_val), 0, dx, pxm.height(), Qt::green);
    painter.fillRect(std::trunc((_left+_right)/2-_min_val), 0, dx, pxm.height(), Qt::red);

    painter.end();
    ui->label_image->setPixmap(pxm);
}

void Levels::on_line_right_val_editingFinished() {
    uncheck_radio();
    QString arg1 = ui->line_right_val->text();
    QPainter painter(&pxm);

    painter.fillRect(std::trunc(_left-_min_val), 0, dx, pxm.height(), Qt::gray);
    painter.fillRect(std::trunc(_right-_min_val), 0, dx, pxm.height(), Qt::gray);
    painter.fillRect(std::trunc((_left+_right)/2-_min_val), 0, dx, pxm.height(), Qt::gray);
    for (long i = 0; i < dx; ++i) {
        painter.fillRect(std::trunc(_left-_min_val)+i,            pxm.height() - _gist[std::trunc(_left-_min_val)+i].second,            1, _gist[std::trunc(_left-_min_val)+i].second, Qt::black);
        painter.fillRect(std::trunc(_right-_min_val)+i,           pxm.height() - _gist[std::trunc(_right-_min_val)+i].second,           1, _gist[std::trunc(_right-_min_val)+i].second, Qt::black);
        painter.fillRect(std::trunc((_left+_right)/2-_min_val)+i, pxm.height() - _gist[std::trunc((_left+_right)/2-_min_val)+i].second, 1, _gist[std::trunc((_left+_right)/2-_min_val)+i].second, Qt::black);
    }

    double buff;
    switch (unit) {
        case (Unit::RAW):
            _right = arg1.toLong();
            std::cout << _right << std::endl;

            if (_right <= _left) _left = _right - 1;
            if (_left < 0) _right = (_left = 0)+1;

            ui->line_left_val->setText(QString::number(std::trunc(_left)));
            ui->line_right_val->setText(QString::number(std::trunc(_right)));
        break;
        case (Unit::QUANTILE):
            buff = arg1.toDouble();
            buff *= _sum/100;
            _right = _gist[ind_before(buff)].first;//TODO: sum_before_bisect

            if (_right <= _left) _left = _right - 1;
            if (_left < 0) _right = (_left = 0)+1;

            buff = _sum_before[std::trunc(_right-_min_val+1)];
            ui->line_right_val->setText(QString::number(buff/_sum*100, 'f', 2));
            buff = _sum_before[std::trunc(_left-_min_val+1)];
            ui->line_left_val->setText(QString::number(buff/_sum*100, 'f', 2));
        break;
    }

    painter.fillRect(std::trunc(_left-_min_val), 0, dx, pxm.height(), Qt::green);
    painter.fillRect(std::trunc(_right-_min_val), 0, dx, pxm.height(), Qt::green);
    painter.fillRect(std::trunc((_left+_right)/2-_min_val), 0, dx, pxm.height(), Qt::red);

    painter.end();
    ui->label_image->setPixmap(pxm);
}

void Levels::on_radio_button_snow_clicked() {

    QPainter painter(&pxm);

    painter.fillRect(std::trunc(_left-_min_val), 0, dx, pxm.height(), Qt::gray);
    painter.fillRect(std::trunc(_right-_min_val), 0, dx, pxm.height(), Qt::gray);
    painter.fillRect(std::trunc((_left+_right)/2-_min_val), 0, dx, pxm.height(), Qt::gray);
    for (long i = 0; i < dx; ++i) {
        painter.fillRect(std::trunc(_left-_min_val)+i,            pxm.height() - _gist[std::trunc(_left-_min_val)+i].second,            1, _gist[std::trunc(_left-_min_val)+i].second, Qt::black);
        painter.fillRect(std::trunc(_right-_min_val)+i,           pxm.height() - _gist[std::trunc(_right-_min_val)+i].second,           1, _gist[std::trunc(_right-_min_val)+i].second, Qt::black);
        painter.fillRect(std::trunc((_left+_right)/2-_min_val)+i, pxm.height() - _gist[std::trunc((_left+_right)/2-_min_val)+i].second, 1, _gist[std::trunc((_left+_right)/2-_min_val)+i].second, Qt::black);
    }

    _left = _left_m - std::sqrt(_left_d);
    _right = _left_m + std::sqrt(_left_d);
    if (_left < _min_val) _left = _min_val;
    if (_right > _max_val) _right = _max_val;

    switch (unit) {
        case (Unit::RAW):
            ui->line_left_val->setText(QString::number(std::trunc(_left)));
            ui->line_right_val->setText(QString::number(std::trunc(_right)));
        break;
        case (Unit::QUANTILE):
            ui->line_left_val->setText(QString::number(_sum_before[std::trunc(_left-_min_val)]/_sum*100, 'f', 2));
            ui->line_right_val->setText(QString::number(_sum_before[std::trunc(_right-_min_val)]/_sum*100, 'f', 2));
        break;
    }

    painter.fillRect(std::trunc(_left-_min_val), 0, dx, pxm.height(), Qt::green);
    painter.fillRect(std::trunc(_right-_min_val), 0, dx, pxm.height(), Qt::green);
    painter.fillRect(std::trunc((_left+_right)/2-_min_val), 0, dx, pxm.height(), Qt::red);

    painter.end();
    ui->label_image->setPixmap(pxm);
}

void Levels::on_radio_button_cloud_clicked() {

    QPainter painter(&pxm);

    painter.fillRect(std::trunc(_left-_min_val), 0, dx, pxm.height(), Qt::gray);
    painter.fillRect(std::trunc(_right-_min_val), 0, dx, pxm.height(), Qt::gray);
    painter.fillRect(std::trunc((_left+_right)/2-_min_val), 0, dx, pxm.height(), Qt::gray);
    for (long i = 0; i < dx; ++i) {
        painter.fillRect(std::trunc(_left-_min_val)+i,            pxm.height() - _gist[std::trunc(_left-_min_val)+i].second,            1, _gist[std::trunc(_left-_min_val)+i].second, Qt::black);
        painter.fillRect(std::trunc(_right-_min_val)+i,           pxm.height() - _gist[std::trunc(_right-_min_val)+i].second,           1, _gist[std::trunc(_right-_min_val)+i].second, Qt::black);
        painter.fillRect(std::trunc((_left+_right)/2-_min_val)+i, pxm.height() - _gist[std::trunc((_left+_right)/2-_min_val)+i].second, 1, _gist[std::trunc((_left+_right)/2-_min_val)+i].second, Qt::black);
    }

    _left = _right_m - std::sqrt(_right_d);
    _right = _right_m + std::sqrt(_right_d);
    if (_left < _min_val) _left = _min_val;
    if (_right > _max_val) _right = _max_val;


    switch (unit) {
        case (Unit::RAW):
            ui->line_left_val->setText(QString::number(std::trunc(_left)));
            ui->line_right_val->setText(QString::number(std::trunc(_right)));
        break;
        case (Unit::QUANTILE):
            ui->line_left_val->setText(QString::number(_sum_before[std::trunc(_left-_min_val)]/_sum*100, 'f', 2));
            ui->line_right_val->setText(QString::number(_sum_before[std::trunc(_right-_min_val)]/_sum*100, 'f', 2));
        break;
    }

    painter.fillRect(std::trunc(_left-_min_val), 0, dx, pxm.height(), Qt::green);
    painter.fillRect(std::trunc(_right-_min_val), 0, dx, pxm.height(), Qt::green);
    painter.fillRect(std::trunc((_left+_right)/2-_min_val), 0, dx, pxm.height(), Qt::red);

    painter.end();
    ui->label_image->setPixmap(pxm);
}

void Levels::uncheck_radio() {
    ui->radio_button_cloud->setAutoExclusive(false);
    ui->radio_button_snow->setAutoExclusive(false);
    ui->radio_button_default->setAutoExclusive(false);

    ui->radio_button_cloud->setChecked(false);
    ui->radio_button_snow->setChecked(false);
    ui->radio_button_default->setCheckable(false);

    ui->radio_button_cloud->setAutoExclusive(true);
    ui->radio_button_snow->setAutoExclusive(true);
    ui->radio_button_default->setAutoExclusive(true);
}

void Levels::on_radio_button_default_clicked() {
    QPainter painter(&pxm);

    painter.fillRect(std::trunc(_left-_min_val), 0, dx, pxm.height(), Qt::gray);
    painter.fillRect(std::trunc(_right-_min_val), 0, dx, pxm.height(), Qt::gray);
    painter.fillRect(std::trunc((_left+_right)/2-_min_val), 0, dx, pxm.height(), Qt::gray);
    for (long i = 0; i < dx; ++i) {
        painter.fillRect(std::trunc(_left-_min_val)+i,            pxm.height() - _gist[std::trunc(_left-_min_val)+i].second,            1, _gist[std::trunc(_left-_min_val)+i].second, Qt::black);
        painter.fillRect(std::trunc(_right-_min_val)+i,           pxm.height() - _gist[std::trunc(_right-_min_val)+i].second,           1, _gist[std::trunc(_right-_min_val)+i].second, Qt::black);
        painter.fillRect(std::trunc((_left+_right)/2-_min_val)+i, pxm.height() - _gist[std::trunc((_left+_right)/2-_min_val)+i].second, 1, _gist[std::trunc((_left+_right)/2-_min_val)+i].second, Qt::black);
    }

    _left = _m - std::sqrt(_d);
    _right = _m + std::sqrt(_d);
    if (_left < _min_val) _left = _min_val;
    if (_right > _max_val) _right = _max_val;

    switch (unit) {
        case (Unit::RAW):
            ui->line_left_val->setText(QString::number(std::trunc(_left)));
            ui->line_right_val->setText(QString::number(std::trunc(_right)));
        break;
        case (Unit::QUANTILE):
            ui->line_left_val->setText(QString::number(_sum_before[std::trunc(_left-_min_val)]/_sum*100, 'f', 2));
            ui->line_right_val->setText(QString::number(_sum_before[std::trunc(_right-_min_val)]/_sum*100, 'f', 2));
        break;
    }

    painter.fillRect(std::trunc(_left-_min_val), 0, dx, pxm.height(), Qt::green);
    painter.fillRect(std::trunc(_right-_min_val), 0, dx, pxm.height(), Qt::green);
    painter.fillRect(std::trunc((_left+_right)/2-_min_val), 0, dx, pxm.height(), Qt::red);

    painter.end();
    ui->label_image->setPixmap(pxm);
}

void Levels::on_buttonBox_clicked(QAbstractButton *button) {
    if (ui->buttonBox->standardButton(button) == QDialogButtonBox::Ok) {
        _binary = ui->checkBox_binary->isChecked();
        ui->checkBox_binary->setChecked(false);
        _tmp_left = _left;
        _tmp_right = _right;
        return;
    }
    ui->checkBox_binary->setChecked(false);
    uncheck_radio();

    QPainter painter(&pxm);

    painter.fillRect(std::trunc(_left-_min_val), 0, dx, pxm.height(), Qt::gray);
    painter.fillRect(std::trunc(_right-_min_val), 0, dx, pxm.height(), Qt::gray);
    painter.fillRect(std::trunc((_left+_right)/2-_min_val), 0, dx, pxm.height(), Qt::gray);
    for (long i = 0; i < dx; ++i) {
        painter.fillRect(std::trunc(_left-_min_val)+i,            pxm.height() - _gist[std::trunc(_left-_min_val)+i].second,            1, _gist[std::trunc(_left-_min_val)+i].second, Qt::black);
        painter.fillRect(std::trunc(_right-_min_val)+i,           pxm.height() - _gist[std::trunc(_right-_min_val)+i].second,           1, _gist[std::trunc(_right-_min_val)+i].second, Qt::black);
        painter.fillRect(std::trunc((_left+_right)/2-_min_val)+i, pxm.height() - _gist[std::trunc((_left+_right)/2-_min_val)+i].second, 1, _gist[std::trunc((_left+_right)/2-_min_val)+i].second, Qt::black);
    }

    _left = _tmp_left;
    _right = _tmp_right;

    switch (unit) {
        case (Unit::RAW):
            ui->line_left_val->setText(QString::number(std::trunc(_left)));
            ui->line_right_val->setText(QString::number(std::trunc(_right)));
        break;
        case (Unit::QUANTILE):
            ui->line_left_val->setText(QString::number(_sum_before[std::trunc(_left-_min_val)]/_sum*100, 'f', 2));
            ui->line_right_val->setText(QString::number(_sum_before[std::trunc(_right-_min_val)]/_sum*100, 'f', 2));
        break;
    }

    painter.fillRect(std::trunc(_left-_min_val), 0, dx, pxm.height(), Qt::green);
    painter.fillRect(std::trunc(_right-_min_val), 0, dx, pxm.height(), Qt::green);
    painter.fillRect(std::trunc((_left+_right)/2-_min_val), 0, dx, pxm.height(), Qt::red);

    painter.end();
    ui->label_image->setPixmap(pxm);
}
