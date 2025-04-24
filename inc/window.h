#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoWidget>
#include <QProgressBar>

class QPushButton;
class Window : public QWidget
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
        int m_counter;

        bool left_pressed, 
             right_pressed, 
             up_pressed, 
             down_pressed;

    protected:
        void setProgressBars();
        void updateProgressBars();
        void keyPressEvent(QKeyEvent *event);
        void keyReleaseEvent(QKeyEvent *event);
};

#endif // WINDOW_H