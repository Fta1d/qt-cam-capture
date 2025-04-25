#include "inc/window.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QApplication>
#include <QKeyEvent>
#include <QTimer>
#include <QDebug>

Window::Window(QWidget *parent) : 
QMainWindow(parent),
    m_buttonPressCounter(0),
    m_xPosition(0),
    m_yPosition(0) {
    m_keyStates = {false, false, false, false};
    
    setFocusPolicy(Qt::StrongFocus);
    resize(640, 445);
    
    setupUI();
    setupConnections();
    
    // Start the timer for progress bar updates
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Window::updateProgressBars);
    timer->start(50);
    
    setFocus();
}

void Window::setupUI() {
    m_tabWidget = new QTabWidget(this);
    
    setupMainTab();
    setupLogTab();
    setupSettingsTab();
    
    setCentralWidget(m_tabWidget);
}

void Window::setupMainTab() {
    m_mainTab = new QWidget();
    
    setupCameraWidget();
    setupControlsWidget();
    setupProgressBars();
    
    // Create layouts
    QVBoxLayout *mainLayout = new QVBoxLayout(m_mainTab);
    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    QVBoxLayout *rightLayout = new QVBoxLayout();
    
    // Add widgets to layouts
    horizontalLayout->addWidget(m_yProgressBar);
    
    rightLayout->addWidget(m_videoWidget);
    rightLayout->addWidget(m_captureButton);
    rightLayout->addWidget(m_xProgressBar);
    
    horizontalLayout->addLayout(rightLayout, 1);
    mainLayout->addLayout(horizontalLayout);
    
    m_tabWidget->addTab(m_mainTab, "Main");
}

void Window::setupLogTab() {
    m_logTab = new QWidget();
    
    setupTextWidget();

    QPushButton *clearLogButton = new QPushButton("Clear", this);
    QPushButton *copyLogButton = new QPushButton("Copy", this);
    
    connect(clearLogButton, &QPushButton::clicked,  m_logTextEdit, &QPlainTextEdit::clear);
    connect(copyLogButton, &QPushButton::clicked, m_logTextEdit, [this]() {
        m_logTextEdit->selectAll();
        m_logTextEdit->copy();
        m_logTextEdit->moveCursor(QTextCursor::End); 
    });
    
    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(clearLogButton);
    buttonsLayout->addWidget(copyLogButton);

    QVBoxLayout *mainLayout = new QVBoxLayout(m_logTab);
    
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(m_logTextEdit);
    
    m_tabWidget->addTab(m_logTab, "Log");
}

void Window::setupSettingsTab() {
    m_settingsTab = new QWidget();

    QBoxLayout *mainLayout = new QBoxLayout(QBoxLayout::LeftToRight, m_settingsTab);

    setupSettingsBoxes(mainLayout);

    m_tabWidget->addTab(m_settingsTab, "Settings");
}

void Window::setupProgressBars() {
    // Setup X progress bar
    m_xProgressBar = new QProgressBar();
    m_xProgressBar->setFormat("%v");
    m_xProgressBar->setRange(-100, 100);
    m_xProgressBar->setValue(0);
    
    // Setup Y progress bar
    m_yProgressBar = new QProgressBar();
    m_yProgressBar->setFormat("%v");
    m_yProgressBar->setRange(-100, 100);
    m_yProgressBar->setValue(0);
    m_yProgressBar->setOrientation(Qt::Vertical);
}

void Window::setupCameraWidget() {
    m_videoWidget = new QVideoWidget();
    
    // Find available cameras
    QList<QCameraDevice> cameras = QMediaDevices::videoInputs();
    if (!cameras.isEmpty()) {
        m_camera = new QCamera(cameras.first());
    } else {
        m_logTextEdit->appendPlainText("Camera not found!");
        m_camera = new QCamera();
    }
    
    // Set up capture session
    m_captureSession = new QMediaCaptureSession();
    m_captureSession->setCamera(m_camera);
    m_captureSession->setVideoOutput(m_videoWidget);
}

void Window::setupControlsWidget() {
    m_captureButton = new QPushButton("Start capturing", this);
    m_captureButton->setCheckable(true);
}

void Window::setupTextWidget() {
    m_logTextEdit = new QPlainTextEdit();
    m_logTextEdit->setReadOnly(true);
}

void Window::setupSettingsBoxes(QBoxLayout *mainLayout) {
    QGroupBox *turrertSettingsBox = new QGroupBox(tr("Turret Settings"));
    QGroupBox *loggerSettingsBox = new QGroupBox(tr("Logger Settings"));
    QGroupBox *appSettingsBox = new QGroupBox(tr("App Settings"));

    mainLayout->addWidget(turrertSettingsBox);
    mainLayout->addWidget(loggerSettingsBox);
    mainLayout->addWidget(appSettingsBox);
}

void Window::setupConnections() {
    connect(m_captureButton, &QPushButton::clicked, this, &Window::slotButtonClicked);
    connect(this, &Window::maxPressesReached, QApplication::instance(), &QApplication::quit);
}

void Window::keyPressEvent(QKeyEvent *event) {
    setFocus();
    switch (event->key()) {
    case Qt::Key_Left:
        m_keyStates.left = true;
        break;
    case Qt::Key_Right:
        m_keyStates.right = true;
        break;
    case Qt::Key_Up:
        m_keyStates.up = true;
        break;
    case Qt::Key_Down:
        m_keyStates.down = true;
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}

void Window::keyReleaseEvent(QKeyEvent *event) {
    switch (event->key()) {
    case Qt::Key_Left:
        m_keyStates.left = false;
        break;
    case Qt::Key_Right:
        m_keyStates.right = false;
        break;
    case Qt::Key_Up:
        m_keyStates.up = false;
        break;
    case Qt::Key_Down:
        m_keyStates.down = false;
        break;
    default:
        QMainWindow::keyReleaseEvent(event);
    }
}

void Window::updateProgressBars() {
    bool valueChanged = false;
    
    // Handle X-axis movement
    if (m_keyStates.left && !m_keyStates.right && m_xProgressBar->value() > m_xProgressBar->minimum()) {
        m_xPosition = m_xProgressBar->value() - 10;
        m_xProgressBar->setValue(m_xPosition);
        valueChanged = true;
    } else if (m_keyStates.right && !m_keyStates.left && m_xProgressBar->value() < m_xProgressBar->maximum()) {
        m_xPosition = m_xProgressBar->value() + 10;
        m_xProgressBar->setValue(m_xPosition);
        valueChanged = true;
    }
    
    // Handle Y-axis movement - only process if one direction is pressed, not both
    if (m_keyStates.down && !m_keyStates.up && m_yProgressBar->value() > m_yProgressBar->minimum()) {
        m_yPosition = m_yProgressBar->value() - 10;
        m_yProgressBar->setValue(m_yPosition);
        valueChanged = true;
    } else if (m_keyStates.up && !m_keyStates.down && m_yProgressBar->value() < m_yProgressBar->maximum()) {
        m_yPosition = m_yProgressBar->value() + 10;
        m_yProgressBar->setValue(m_yPosition);
        valueChanged = true;
    }
    
    // Log position changes
    if (valueChanged) {
        m_logTextEdit->appendPlainText(
            QString("Position: x = %1, y = %2").arg(m_xPosition).arg(m_yPosition)
        );
    }
}

void Window::slotButtonClicked(bool checked) {
    if (checked) {
        m_captureButton->setText("Stop capturing");
        m_camera->start();
    } else {
        m_captureButton->setText("Start capturing");
        m_camera->stop();
    }
    
    if (++m_buttonPressCounter == 5) {
        emit maxPressesReached();
    }
}