#ifndef GSTREAMER_H
#define GSTREAMER_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/app/gstappsrc.h>

#include <QPixmap>
#include <QImage>
#include <QMutex>

#include <iostream>
#include <string>
#include <atomic>

using namespace cv;

class GstreamerCameraCapture {
    private:
        GstElement* pipeline;
        GstElement* source;
        GstElement* convert;
        GstElement* scale;
        GstElement* sink;

        Mat processedFrame;

        std::atomic<bool> frame_ready;
    
        GstBuffer *mat_to_gst_buffer(const Mat &frame);
        GstSample *mat_to_gst_sample(const Mat &frame, GstCaps *caps);
        Mat process_frame(const Mat &input_frame);
        Mat gst_sample_to_mat(GstSample* sample);
        void new_frame(GstElement *sink);

        friend GstFlowReturn new_sample_callback(GstElement *sink, gpointer data);

        QMutex m_mutex;

    public:
        
        QPixmap pull_pixmap_from_frame();
        GstreamerCameraCapture();
        ~GstreamerCameraCapture();

        
        void stop();
        void run();
};

#endif // GSTREAMER_Hs
