#include "inc/window.h"
#include <QPushButton>
#include <QApplication>

Window::Window(QWidget *parent) :
QWidget(parent) {
    setFixedSize(1000, 500);

    m_button = new QPushButton("Press me", this);
    m_button->setGeometry((1000 - 100) / 2, (500 - 50) / 2, 100, 50);
    m_button->setCheckable(true);

    connect(m_button, SIGNAL(clicked(bool)), this, SLOT(slotButtonClicked(bool)));
}

void Window::slotButtonClicked(bool checked) {
    if (checked) {
        m_button->setText("Pressed");
    } else {
        m_button->setText("Unpressed");
    }
}
