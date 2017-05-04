#ifndef LEVELS_H
#define LEVELS_H

#include <QDialog>

namespace Ui {
class Levels;
}

class Levels : public QDialog
{
    Q_OBJECT

public:
    explicit Levels(QWidget *parent = 0);
    ~Levels();

private:
    Ui::Levels *ui;
};

#endif // LEVELS_H
