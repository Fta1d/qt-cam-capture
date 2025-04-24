#include "inc/window.h"
#include <QPushButton>
#include <QApplication>
#include <QKeyEvent>
#include <QTimer>

Window::Window(QWidget *parent) :
QWidget(parent) {
    setFixedSize(640, 500);
    m_counter = 0;

    setFocusPolicy(Qt::StrongFocus);
    setProgressBars();

    m_videoWidget = new QVideoWidget(this);
    m_videoWidget->setGeometry(0, 0, 640, 390); // x, y, width, height

    m_button = new QPushButton("Start capturing", this);
    m_button->setGeometry((640 - 100) / 2, 450, 100, 50);
    m_button->setCheckable(true);

    connect(m_button, SIGNAL(clicked(bool)), this, SLOT(slotButtonClicked(bool)));
    connect(this, SIGNAL(maxPressesReached()), QApplication::instance(), SLOT(quit()));

    m_camera = new QCamera();
    m_captureSession = new QMediaCaptureSession();
    m_captureSession->setCamera(m_camera);
    m_captureSession->setVideoOutput(m_videoWidget);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Window::updateProgressBars);
    timer->start(50);

    setFocus();
}

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

    QWidget::keyPressEvent(event);
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
    
    QWidget::keyReleaseEvent(event);
}

void Window::updateProgressBars() {
    if (left_pressed) {
        int x_val = x_progress_bar->value();
        x_progress_bar->setValue(x_val - 10);
    }
    
    if (right_pressed) {
        int x_val = x_progress_bar->value();
        x_progress_bar->setValue(x_val + 10);
    }

    if (up_pressed) {
        int y_val = y_progress_bar->value();
        y_progress_bar->setValue(y_val + 10);
    }
    
    if (down_pressed) {
        int y_val = y_progress_bar->value();
        y_progress_bar->setValue(y_val - 10);
    }
}

void Window::setProgressBars() {
    x_progress_bar = new QProgressBar(this);
    x_progress_bar->setFormat("%v");
    x_progress_bar->setRange(-100, 100);
    x_progress_bar->setValue(0);
    x_progress_bar->setGeometry(50, 470, 200, 30);

    y_progress_bar = new QProgressBar(this);
    y_progress_bar->setFormat("%v");
    y_progress_bar->setRange(-100, 100);
    y_progress_bar->setValue(0);
    y_progress_bar->setGeometry(10, 400, 30, 100);
    y_progress_bar->setOrientation(Qt::Vertical);
}

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
