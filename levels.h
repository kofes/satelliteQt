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

class Levels : public QDialog {
    Q_OBJECT

public:
    explicit Levels(QWidget *parent = 0);
    ~Levels();

    void init(satellite::Image& img);

    inline short left() { return _left; }

    inline short right() { return _right; }

    inline short max() { return _max_val; }

    inline short min() { return _min_val; }

    inline bool binary() { return _binary; }

signals:
    void levels_calc_progress(int);

    void set_text_progress(QString);

private slots:
    void on_radio_button_raw_clicked();

    void on_radio_button_quantile_clicked();

    void on_line_left_val_editingFinished();

    void on_line_right_val_editingFinished();

    void on_radio_button_snow_clicked();

    void on_radio_button_cloud_clicked();

    void on_radio_button_default_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    void uncheck_radio();

    double sum_before(short& end);
    unsigned long ind_before(double& end);

    void reset_lines();
    void draw_lines();

    Ui::Levels *ui;

    QIntValidator* int_valid;
    QDoubleValidator* double_valid;

    QPixmap pxm;

    std::vector< std::pair<double, unsigned long> > _gist;
    std::vector<double> _sum_before;

    short _max_val, _min_val, _left, _right, _tmp_left, _tmp_right, dx, _threshold;
    double _sum, _m, _d, _left_m, _right_m, _left_d, _right_d;
    bool _binary;

    Unit unit;
};

#endif // LEVELS_H
