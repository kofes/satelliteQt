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

    void point_to_gist(std::vector< std::pair<double, unsigned long int> >* gist);

    long left();
    long right();

    void left(long _l);
    void right(long _r);
private slots:
    void on_radio_button_raw_clicked();

    void on_radio_button_quantile_clicked();

    void on_line_left_val_editingFinished();

    void on_line_right_val_editingFinished();

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_radio_button_snow_clicked();

    void on_radio_button_cloud_clicked();

private:
    double sum_before(long& end);
    unsigned long ind_before(double& end);

    Ui::Levels *ui;

    QPixmap pxm;

    std::vector< std::pair<double, unsigned long> >* _gist;
    long _left, _right, dx;

    double _sum;

    Unit unit;
};

#endif // LEVELS_H
