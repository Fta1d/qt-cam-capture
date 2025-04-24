#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QCamera>
#include <QCameraDevice>
#include <QMediaDevices>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QProgressBar>

class QPushButton;
class Window : public QMainWindow
{
    Q_OBJECT
    public:
        explicit Window(QWidget *parent = 0);

    signals:
        void maxPressesReached();

    private slots:
        void slotButtonClicked(bool checked);

    private:
        QCamera *m_camera;
        QMediaCaptureSession *m_captureSession;
        QVideoWidget *m_videoWidget;
        QPushButton *m_button;
        QProgressBar *x_progress_bar, *y_progress_bar;
        QTabWidget *tabWidget;
        QWidget *mainTab, *logTab;
        int m_counter;
        bool left_pressed  = false, 
             right_pressed = false, 
             up_pressed    = false, 
             down_pressed  = false;

    protected:
        void setProgressBars();
        void updateProgressBars();
        void setMainTab();
        void setLogTab();
        void setCameraWidget();
        void setControlsWidget();
        void keyPressEvent(QKeyEvent *event);
        void keyReleaseEvent(QKeyEvent *event);
};

#endif // WINDOW_H