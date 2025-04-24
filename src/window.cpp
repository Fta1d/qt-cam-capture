#include "inc/window.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QKeyEvent>
#include <QTimer>

Window::Window(QWidget *parent) :
QMainWindow(parent) {
    setFocusPolicy(Qt::StrongFocus);
    resize(640, 445);
    m_counter = 0;

    m_tab_widget = new QTabWidget(this);

    setMainTab();
    setLogTab();
    setCentralWidget(m_tab_widget);

    connect(m_button, SIGNAL(clicked(bool)), this, SLOT(slotButtonClicked(bool)));
    connect(this, SIGNAL(maxPressesReached()), QApplication::instance(), SLOT(quit()));

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Window::updateProgressBars);
    timer->start(50);

    setFocus();
}

/*----------------TABS INITIATION-------------*/

void Window::setMainTab() {
    m_main_tab = new QWidget();

    setCameraWidget();
    setControlsWidget();
    setProgressBars();

    QVBoxLayout *mainLayout = new QVBoxLayout(m_main_tab);
    QHBoxLayout *horizontalLayout = new QHBoxLayout();

    horizontalLayout->addWidget(y_progress_bar);

    QVBoxLayout *rightLayout = new QVBoxLayout();

    rightLayout->addWidget(m_videoWidget, 1);
    rightLayout->addWidget(m_button);
    rightLayout->addWidget(x_progress_bar);

    horizontalLayout->addLayout(rightLayout, 1);

    mainLayout->addLayout(horizontalLayout);

    m_tab_widget->addTab(m_main_tab, "Main");
}


void Window::setLogTab() {
    m_log_tab = new QWidget();

    setTextWidget();

    QVBoxLayout *mainLayout = new QVBoxLayout(m_log_tab);

    mainLayout->addWidget(m_text_edit);
    
    m_tab_widget->addTab(m_log_tab, "Log");
}
/*--------------------------------------------*/

/*----------------EVENTS----------------------*/

void Window::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Left) {
        left_pressed = true;
    } else if (event->key() == Qt::Key_Right) {
        right_pressed = true;
    } else if (event->key() == Qt::Key_Up) {
        up_pressed = true;
    } else if (event->key() == Qt::Key_Down) {
        down_pressed = true;
    }

    QMainWindow::keyPressEvent(event);
}

void Window::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Left) {
        left_pressed = false;
    } else if (event->key() == Qt::Key_Right) {
        right_pressed = false;
    } else if (event->key() == Qt::Key_Up) {
        up_pressed = false;
    } else if (event->key() == Qt::Key_Down) {
        down_pressed = false;
    }
    
    QMainWindow::keyReleaseEvent(event);
}
/*--------------------------------------------*/

/*----------------UPDATE BARS-----------------*/

void Window::updateProgressBars() {
    if (left_pressed) {
        x_val = x_progress_bar->value();

        if (!(x_val == x_progress_bar->minimum())) {
            x_val -= 10;
            x_progress_bar->setValue(x_val);
        } else {
            return;
        }

        m_text_edit->appendPlainText(QString("Position: x = %1, y = %2").arg(x_val).arg(y_val));
    }
    
    if (right_pressed) {
        x_val = x_progress_bar->value();

        if (!(x_val == x_progress_bar->maximum())) {
            x_val += 10;
            x_progress_bar->setValue(x_val);
        } else {
            return;
        }

        m_text_edit->appendPlainText(QString("Position: x = %1, y = %2").arg(x_val).arg(y_val));
    }

    if (up_pressed) {
        y_val = y_progress_bar->value();

        if (!(y_val == y_progress_bar->maximum())) {
            y_val += 10;
            y_progress_bar->setValue(y_val);
        } else {
            return;
        }
        
        m_text_edit->appendPlainText(QString("Position: x = %1, y = %2").arg(x_val).arg(y_val));
    }
    
    if (down_pressed) {
        y_val = y_progress_bar->value();

        if (!(y_val == y_progress_bar->minimum())) {
            y_val -= 10;
            y_progress_bar->setValue(y_val);
        } else {
            return;
        }

        m_text_edit->appendPlainText(QString("Position: x = %1, y = %2").arg(x_val).arg(y_val));
    }
}
/*--------------------------------------------*/

/*----------------WIDGETS INITIATION----------*/
void Window::setProgressBars() {
    x_progress_bar = new QProgressBar();
    x_progress_bar->setFormat("%v");
    x_progress_bar->setRange(-100, 100);
    x_progress_bar->setValue(0);

    y_progress_bar = new QProgressBar();
    y_progress_bar->setFormat("%v");
    y_progress_bar->setRange(-100, 100);
    y_progress_bar->setValue(0);
    y_progress_bar->setOrientation(Qt::Vertical);
}

void Window::setCameraWidget() {
    m_videoWidget = new QVideoWidget();

    QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (!cameras.isEmpty()) {
        m_camera = new QCamera(cameras.first());
    } else {
        qDebug() << "Camera not found!";
        m_camera = new QCamera();
    }
    
    m_captureSession = new QMediaCaptureSession();
    m_captureSession->setCamera(m_camera);
    m_captureSession->setVideoOutput(m_videoWidget);
}

void Window::setControlsWidget() {
    m_button = new QPushButton("Start capturing", this);
    m_button->setCheckable(true);
}

void Window::setTextWidget() {
    m_text_edit = new QPlainTextEdit();
}

/*--------------------------------------------*/

/*----------------CUSTOM SLOTS----------------*/

void Window::slotButtonClicked(bool checked) {
    if (checked) {
        m_button->setText("Stop capturing");
        m_camera->start();
    } else {
        m_button->setText("Start capturing");
        m_camera->stop();
    }

    if (++m_counter == 5) {
        emit maxPressesReached();
    }
}
/*--------------------------------------------*/
