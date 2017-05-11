#ifndef LEVELS_H
#define LEVELS_H

#include "allin.hpp"

namespace Ui {
class Levels;
}

enum class Unit {
    QUANTILE,
    RAW
};

class Levels : public QDialog
{
    Q_OBJECT

public:
    explicit Levels(QWidget *parent = 0);
    ~Levels();

    void init(satellite::Image& img);

    short left();
    short right();
    short max();

private slots:
    void on_radio_button_raw_clicked();

    void on_radio_button_quantile_clicked();

    void on_line_left_val_editingFinished();

    void on_line_right_val_editingFinished();

    void on_radio_button_snow_clicked();

    void on_radio_button_cloud_clicked();

    void on_radio_button_default_clicked();

private:
    void uncheck_radio();

    double sum_before(short& end);
    unsigned long ind_before(double& end);

    Ui::Levels *ui;

    QPixmap pxm;

    std::vector< std::pair<double, unsigned long> > _gist;
    std::vector<double> _sum_before;
    short _max_val, _left, _right, dx, _threshold;
    double _sum;

    Unit unit;
};

#endif // LEVELS_H
