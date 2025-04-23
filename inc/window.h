#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoWidget>

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
        int m_counter;
};

#endif // WINDOW_H