#include "vardialog.h"
#include "ui_vardialog.h"

varDialog::varDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::varDialog)
{
    ui->setupUi(this);
}

varDialog::~varDialog()
{
    delete ui;
}
