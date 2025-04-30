#ifndef WINDOW_H
#define WINDOW_H

#include "inc/gstreamer.h"

#include <QMainWindow>
#include <QCamera>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QProgressBar>
#include <QPlainTextEdit>
#include <QTabWidget>
#include <QBoxLayout>
#include <QGroupBox>
#include <QMutex>
#include <QSlider>
#include <QLabel>
#include <QPixmap>
#include <QImage>

class QPushButton;
class QKeyEvent;

class Window : public QMainWindow
{
    Q_OBJECT

public:
    explicit Window(QWidget *parent = nullptr);

signals:

private slots:
    void slotButtonClicked(bool checked);
    void updateProgressBars();
    void setZoom(int val);
    void setCameraFocus(int val);
    void updateFrame();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    // UI setup methods
    void setupUI();
    void setupMainTab();
    void setupLogTab();
    void setupSettingsTab();
    void setupCameraWidget();
    void setupControlsWidget();
    void setupTextWidget();
    void setupProgressBars();
    void setupZoomAndFocusControl(QSlider *zoomSlider, QSlider *foucsSlider);
    void setupSettingsBoxes(QBoxLayout *mainLayout);
    void setupTurretSettingsBox(QGroupBox *settingsBox);
    void setupConnections();

    // Help methods
    void getCameraFeatures();

    // Setters
    void setSpeed(int val);

    // UI components
    QTabWidget* m_tabWidget;
    QWidget* m_mainTab;
    QWidget* m_logTab;
    QWidget* m_settingsTab;
    
    QPushButton* m_captureButton;
    QProgressBar* m_xProgressBar;
    QProgressBar* m_yProgressBar;
    QPlainTextEdit* m_logTextEdit;
    
    // Camera components
    GstreamerCameraCapture *camera;
    QPixmap frame;
    QLabel *frameDisplayLabel;
    QTimer *frameTimer;

    // State variables
    int m_buttonPressCounter;
    int m_xPosition;
    int m_yPosition;
    int m_speed;
    
    struct {
        bool left;
        bool right;
        bool up;
        bool down;
    } m_keyStates;

    // Synchronization primitives
    mutable QMutex m_logMutex;
};

#endif // WINDOW_H