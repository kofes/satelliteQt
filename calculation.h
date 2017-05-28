#ifndef CALCULATION_H
#define CALCULATION_H

#include <allin.hpp>

#include "inc/Satellite.hpp"

class Calculation;

enum class Calc_type {
    VARIOGRAM,
    COVARIANCE
};

class Calculation : public QObject {
    Q_OBJECT

public:
    explicit Calculation(QObject *parent = 0);

    void set(std::vector<double>* _f, short x, short y, short dx, short dy, short dh, satellite::Image* img, Calc_type type);

    void operator()();

signals:
    void signal_progress(int);

public slots:
    void abort(bool val);

private:
    short _x, _y, _dx, _dy, _dh;
    satellite::Image* _img;

    std::vector<double>* func;
    bool flag;
    Calc_type _type;
};

#endif // CALCULATION_H
