#ifndef CREATEIMAGE_H
#define CREATEIMAGE_H

#include "allin.hpp"

namespace Ui {
class CreateImage;
}

class CreateImage : public QDialog {
    Q_OBJECT

public:
    explicit CreateImage(QWidget *parent = 0);
    ~CreateImage();

    unsigned short width ();
    unsigned short height ();
    unsigned short space ();
    unsigned short radius ();
    satellite::Shape shape ();
    satellite::ShapeFill fill ();
    double epsilon ();

private slots:
    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

private:
    Ui::CreateImage *ui;

    unsigned short _width, _height, _space, _radius;
    satellite::Shape _type_shape;
    satellite::ShapeFill _filled;
    double _epsilon;
};

#endif // CREATEIMAGE_H
