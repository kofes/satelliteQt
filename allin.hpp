#ifndef ALLIN_HPP
#define ALLIN_HPP

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QImage>
#include <QColor>
#include <QPixmap>
#include <QPainter>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QValidator>
#include <QDialogButtonBox>
#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QAction>
#include <QProgressBar>
#include <QThreadPool>
#include <QFutureWatcher>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QPushButton>

#include <QDebug>


#include "inc/Satellite.hpp"

namespace Ui {
enum DATA_TYPE {
    NONE,
    PRO,
    VAR,
    IMG
};
}

#endif // ALLIN_HPP
