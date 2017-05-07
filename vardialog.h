#ifndef VARDIALOG_H
#define VARDIALOG_H

#include "allin.hpp"

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

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::varDialog *ui;

    satellite::Image *image;

    std::vector<double> func;
};

#endif // VARDIALOG_H
