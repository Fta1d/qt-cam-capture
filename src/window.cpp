#include "inc/window.h"
#include <QPushButton>

Window::Window(QWidget *parent) :
QWidget(parent) {
    setFixedSize(1000, 500);

    QPushButton *m_button = new QPushButton("Hello world", this);
    m_button->setGeometry((1000 - 100) / 2, (500 - 50) / 2, 100, 50);
}