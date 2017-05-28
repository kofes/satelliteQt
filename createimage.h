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

    inline unsigned short width() { return _width; }

    inline unsigned short height() { return _height; }

    inline unsigned short space() { return _space; }

    inline unsigned short radius() { return _radius; }

    inline satellite::Shape shape() { return _type_shape; }

    inline satellite::ShapeFill fill() { return _filled; }

    inline double epsilon() { return _epsilon; }

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
