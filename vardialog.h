#ifndef VARDIALOG_H
#define VARDIALOG_H

#include <QDialog>

namespace Ui {
class varDialog;
}

class varDialog : public QDialog
{
    Q_OBJECT

public:
    explicit varDialog(QWidget *parent = 0);
    ~varDialog();

private:
    Ui::varDialog *ui;
};

#endif // VARDIALOG_H
