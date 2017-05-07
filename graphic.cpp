#include "graphic.h"
#include "ui_graphic.h"

Graphic::Graphic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Graphic)
{
    ui->setupUi(this);
}

Graphic::~Graphic()
{
    delete ui;
}

void Graphic::setVar(const std::vector<double>& src, double dh) {
    QVector<double> x, y;
    x.resize(src.size());
    y.resize(src.size());
    for (unsigned int i = 0; i < y.size(); ++i) {
        x[i] = dh*i;
        y[i] = src[i];
    }

    ui->widget->clearGraphs();//Если нужно, но очищаем все графики
    //Добавляем один график в widget
    ui->widget->addGraph();
    //Говорим, что отрисовать нужно график по нашим двум массивам x и y
    ui->widget->graph(0)->setData(x, y);

    //Подписываем оси Ox и Oy
    ui->widget->xAxis->setLabel("h");
    ui->widget->yAxis->setLabel("Var");

    ui->widget->xAxis->setRange(0, x.last());//Для оси Ox

    double minY = y[0], maxY = y[0];
    for (int i = 1; i < src.size(); i++)
    {
        if (y[i] < minY) minY = y[i];
        if (y[i] > maxY) maxY = y[i];
    }
    ui->widget->yAxis->setRange(minY, maxY);//Для оси Oy

    ui->widget->replot();
}
