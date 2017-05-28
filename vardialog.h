#ifndef VARDIALOG_H
#define VARDIALOG_H

#include "allin.hpp"

#include "calculation.h"

namespace Ui {
class varDialog;
}

class varDialog : public QDialog {
    Q_OBJECT

public:
    explicit varDialog(QWidget *parent = 0);
    ~varDialog();

    void setImage(satellite::Image* src);
    void setImageType(Ui::DATA_TYPE type);
    int dh();

    std::vector<double> var();

public slots:
    void slot_finished();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_buttonBox_rejected();

    void on_line_top_left_x_editingFinished();

    void on_line_bottom_right_x_editingFinished();

    void on_line_top_left_y_editingFinished();

    void on_line_bottom_right_y_editingFinished();

signals:
    void abort(bool);

    void format(Ui::FORMAT_TYPE format_type);

private:

    void reset(bool flag);

    Ui::varDialog *ui;

    satellite::Image *image;

    std::vector<double> func;

    Calculation cl;
    QFutureWatcher<void> fw;

    bool _cancel_flag;
    Ui::DATA_TYPE _type;

    short _max_x, _max_y;

    QIntValidator* log_valid;
};

#endif // VARDIALOG_H
