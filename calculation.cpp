#include "calculation.h"

Calculation::Calculation(QObject *parent) : QObject(parent)
{

}

void Calculation::set(std::vector<double>* _f, short x, short y, short dx, short dy, short dh, satellite::Image* img) {
    func = _f;
    _x = x;
    _y = y;
    _dx = dx;
    _dy = dy;
    _dh = dh;
    _img = img;
    flag = false;
}

void Calculation::operator ()() {
    double h = 0;
    while (!flag && h < std::sqrt(_dx*_dx + _dy*_dy)) {
        func->push_back(satellite::math::g(
                           _x,
                           _y,
                           _dx,
                           _dy,
                           h,
                           *_img
                           ));
        h += _dh;
        emit signal_progress(h/std::sqrt(_dx*_dx+_dy*_dy) * 100);
    }
    flag = false;
}

void Calculation::abort(bool val) {
    flag = val;
}
