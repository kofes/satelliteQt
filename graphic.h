#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "allin.hpp"

namespace Ui {
class Graphic;
}

class Graphic : public QDialog
{
    Q_OBJECT

public:
    explicit Graphic(QWidget *parent = 0);
    ~Graphic();

    void setVar(const std::vector<double>& src, double dh);

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::Graphic *ui;

    std::vector<double> func;
    double _dh;
};

#endif // GRAPHIC_H
