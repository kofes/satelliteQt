#include "calculation.h"

Calculation::Calculation(QObject *parent) : QObject(parent) {}

void Calculation::set(std::vector<double>* _f, short x, short y, short dx, short dy, short dh, satellite::Image* img, Calc_type type) {
    func = _f;
    _x = x;
    _y = y;
    _dx = dx;
    _dy = dy;
    _dh = dh;
    _img = img;
    flag = false;
    _type = type;

    std::cout << "vvvvvvvvvvvvvvvv" << '\n'
              << "x: " << _x << '\n'
              << "y: " << _y << '\n'
              << "w: " << _dx << '\n'
              << "h: " << _dy << '\n'
//              << "t: " << ((_type == Ui::DATA_TYPE::IMG) ? "img" : "pro") << '\n'
              << "^^^^^^^^^^^^^^^^"
              << std::endl;
}

void Calculation::operator ()() {
    double h = 0;
    std::cout << _x << ':' << _x + _dx << ':' << _y << ':' << _y + _dy << std::endl;
    switch (_type) {
        case (Calc_type::VARIOGRAM):
            while (!flag && h < std::sqrt(_dx*_dx + _dy*_dy)) {
                func->push_back(satellite::math::g(
                                   _x,
                                   _y,
                                   _x + _dx,
                                   _y + _dy,
                                   h,
                                   *_img
                                   ));
                h += _dh;
                emit signal_progress((h-_dh)/std::sqrt(_dx*_dx+_dy*_dy) * 100);
            }
        break;
        case (Calc_type::COVARIANCE):
            while (!flag && h < std::sqrt(_dx*_dx + _dy*_dy)) {
                func->push_back(satellite::math::cov(
                                   _x,
                                   _y,
                                   _x + _dx,
                                   _y + _dy,
                                   h,
                                   *_img
                                   ));
                h += _dh;
                emit signal_progress((h-_dh)/std::sqrt(_dx*_dx+_dy*_dy) * 100);
            }
        break;
    }

    flag = false;
}

void Calculation::abort(bool val) {
    flag = val;
}
