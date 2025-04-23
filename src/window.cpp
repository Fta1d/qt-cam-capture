#include "inc/window.h"
#include <QPushButton>
#include <QApplication>

Window::Window(QWidget *parent) :
QWidget(parent) {
    setFixedSize(640, 500);

    m_videoWidget = new QVideoWidget(this);
    m_videoWidget->setGeometry(0, 0, 640, 390); // x, y, width, height

    m_button = new QPushButton("Press me", this);
    m_button->setGeometry((640 - 100) / 2, 450, 100, 50);
    m_button->setCheckable(true);

    connect(m_button, SIGNAL(clicked(bool)), this, SLOT(slotButtonClicked(bool)));

    m_camera = new QCamera();
    m_captureSession = new QMediaCaptureSession();
    m_captureSession->setCamera(m_camera);
    m_captureSession->setVideoOutput(m_videoWidget);
    m_camera->start();
}

void Window::slotButtonClicked(bool checked) {
    if (checked) {
        m_button->setText("Pressed");
    } else {
        m_button->setText("Unpressed");
    }
}
