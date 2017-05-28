#include "graphic.h"
#include "ui_graphic.h"

Graphic::Graphic(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Graphic) {
    ui->setupUi(this);
    ui->checkBox->setChecked(false);
}

Graphic::~Graphic() {
    delete ui;
}

void Graphic::setVar(const std::vector<double>& src, double dh) {
    func = src;
    _dh = dh;
    QVector<double> x, y;
    std::vector<bool> serials;
    ui->checkBox->setChecked(false);
    extremes.clear();
    x.resize(src.size());
    y.resize(src.size());
    QString format;
    double coef;
    switch (_format_type) {
    case (Ui::FORMAT_TYPE::PX):
        coef = 1.0f;
        format = "px";
    break;
    case (Ui::FORMAT_TYPE::M):
        coef = (_secLat + _secLon)/720 * 111.111f;
        format = "m";
    break;
    case (Ui::FORMAT_TYPE::DEG):
        coef = (_secLat + _secLon)/720;
        format = "deg";
    break;
    }
    for (unsigned int i = 0; i < y.size(); ++i) {
        x[i] = dh*i*coef;
        y[i] = src[i]*coef*coef;
    }
    if (src.size() != 1) {
        serials.resize(src.size() - 1);
        for (unsigned int i = 0; i < src.size()-1; ++i)
            if (src[i+1] - src[i] == 0) {
                if (i > 0) serials[i] = serials[i-1];
                else serials[0] = true;
            } else
                serials[i] = (src[i+1] - src[i]) > 0;
        for (unsigned int i = 1; i < serials.size(); ++i)
            if (serials[i-1] != serials[i])
                extremes.push_back(std::make_pair(dh*i*coef, src[i]*coef*coef));
    }
    ui->widget->clearGraphs();//Если нужно, но очищаем все графики
    //Добавляем один график в widget
    ui->widget->addGraph();
    ui->widget->addGraph();
    QPen pen;
    pen.setColor(QColor(0, 0, 255, 255));
    pen.setWidthF(1);
    ui->widget->graph(0)->setPen(pen);
    //Говорим, что отрисовать нужно график по нашим двум массивам x и y
    ui->widget->graph(0)->setData(x, y);

    //Подписываем оси Ox и Oy
    ui->widget->xAxis->setLabel(QString("h, ") + format);
    ui->widget->yAxis->setLabel(QString("Var, ") + format + "^2");

    ui->widget->xAxis->setRange(0, x.last());//Для оси Ox

    double minY = y[0], maxY = y[0];
    for (unsigned int i = 1; i < src.size(); i++) {
        if (y[i] < minY) minY = y[i];
        if (y[i] > maxY) maxY = y[i];
    }
    ui->widget->yAxis->setRange(minY, maxY*1.1);//Для оси Oy

    ui->widget->replot();
}

void Graphic::on_buttonBox_clicked(QAbstractButton *button) {
    if (ui->buttonBox->standardButton(button) == QDialogButtonBox::Save) {
        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), "",
                                                        tr("Var files (*.var)"));
        if (fileName.isEmpty())
            return;
        std::ofstream file(fileName.toStdString());
        if (!file.is_open()) {
            QMessageBox::critical(this, tr("Error"), tr("Could not open file"));
            return;
        }
        file << func.size() << '\n';
        file << _dh << '\n';
        for (unsigned int i = 0; i < func.size(); ++i)
            file << func[i] << '\n';
    }
}

void Graphic::on_checkBox_stateChanged(int arg1) {
    if (arg1 == Qt::Checked) {
        QVector<double> x, y;
        x.resize(extremes.size());
        y.resize(extremes.size());
        for (unsigned int i = 0; i < y.size(); ++i) {
            x[i] = extremes[i].first;
            y[i] = extremes[i].second;
        }
        QPen pen;
        pen.setColor(QColor(255, 0, 0, 255));
//        pen.setStyle(Qt::DashDotDotLine);
        pen.setWidthF(2.5);
        ui->widget->graph(1)->setLineStyle(QCPGraph::lsNone);
        ui->widget->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
        ui->widget->graph(1)->setPen(pen);
        ui->widget->graph(1)->setData(x, y);
        ui->widget->replot();
        return;
    }
    if (arg1 == Qt::Unchecked) {
        QVector<double> x, y;
        x.resize(extremes.size());
        y.resize(extremes.size());
        for (unsigned int i = 0; i < y.size(); ++i) {
            x[i] = extremes[i].first;
            y[i] = extremes[i].second;
        }
        QPen pen;
        pen.setColor(QColor(0, 0, 255, 255));
//        pen.setWidthF(2.5);
        ui->widget->graph(1)->setLineStyle(QCPGraph::lsNone);
        ui->widget->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
        ui->widget->graph(1)->setPen(pen);
        ui->widget->graph(1)->setData(x, y);
        ui->widget->replot();
        return;
    }
}

void Graphic::setFormat(Ui::FORMAT_TYPE format_type) {
    _format_type = format_type;
}

void Graphic::setDeg(float secLat, float secLon){
    _secLat = secLat;
    _secLon = secLon;
}
