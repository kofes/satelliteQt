#include "levels.h"
#include "ui_levels.h"

Levels::Levels(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Levels)
{
    ui->setupUi(this);
}

Levels::~Levels()
{
    delete ui;
}
