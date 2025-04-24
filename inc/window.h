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
#include <QPlainTextEdit>

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
        QPushButton *m_button;
        QProgressBar *x_progress_bar, *y_progress_bar;
        QPlainTextEdit *m_text_edit;

        QCamera *m_camera;
        QMediaCaptureSession *m_captureSession;
        QVideoWidget *m_videoWidget;
        
        QTabWidget *m_tab_widget;
        QWidget *m_main_tab, *m_log_tab;
        int m_counter;
        short x_val = 0, y_val = 0;
        bool left_pressed  = false, 
             right_pressed = false, 
             up_pressed    = false, 
             down_pressed  = false;

    protected:
        void setProgressBars();
        void setMainTab();
        void setLogTab();
        void setCameraWidget();
        void setControlsWidget();
        void setTextWidget();
        void keyPressEvent(QKeyEvent *event);
        void keyReleaseEvent(QKeyEvent *event);
        void updateProgressBars();
};

#endif // WINDOW_H