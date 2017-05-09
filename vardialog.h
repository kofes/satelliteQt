#ifndef VARDIALOG_H
#define VARDIALOG_H

#include "allin.hpp"

#include "calculation.h"

namespace Ui {
class varDialog;
}

class varDialog : public QDialog
{
    Q_OBJECT

public:
    explicit varDialog(QWidget *parent = 0);
    ~varDialog();

    void setImage(satellite::Image* src);

    double dh();

    std::vector<double> var();

public slots:
    void slot_finished();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_buttonBox_rejected();

signals:
    void abort(bool);

private:

    void reset(bool flag);

    Ui::varDialog *ui;

    satellite::Image *image;

    std::vector<double> func;

    Calculation cl;
    QFutureWatcher<void> fw;

    bool _cancel_flag;
};

#endif // VARDIALOG_H
