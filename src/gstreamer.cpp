#include "inc/gstreamer.h"

using namespace cv;

static gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer data);
GstFlowReturn new_sample_callback(GstElement *sink, gpointer data);

GstreamerCameraCapture::GstreamerCameraCapture() :
    pipeline(nullptr),
    source(nullptr),
    convert(nullptr),
    scale(nullptr),
    sink(nullptr),
    frame_ready(false)
{
    gst_init(NULL, NULL);

    // Create source pipeline
    this->pipeline = gst_pipeline_new("src_pipeline");
    this->source = gst_element_factory_make("v4l2src", "src_source");
    this->convert = gst_element_factory_make("videoconvert", "src_convert");
    this->scale = gst_element_factory_make("videoscale", "src_scale");
    this->sink = gst_element_factory_make("appsink", "src_sink");
    
    // Check src pipeline elements
    if (!this->pipeline || !this->source || !this->convert || 
        !this->scale || !this->sink) {
        std::cerr << "Failed to create src pipeline elements!" << std::endl;
        return;
    }
    
    // Configure appsink to receive frames
    g_object_set(G_OBJECT(this->sink), "emit-signals", TRUE, NULL);
    g_object_set(G_OBJECT(this->sink), "max-buffers", 1, NULL);
    g_object_set(G_OBJECT(this->sink), "drop", TRUE, NULL);
    
    // Set video format
    GstCaps *caps = gst_caps_new_simple("video/x-raw",
                                       "format", G_TYPE_STRING, "RGB",
                                       "width", G_TYPE_INT, 640,
                                       "height", G_TYPE_INT, 480,
                                       "framerate", GST_TYPE_FRACTION, 30, 1,
                                       NULL);
    
    // Set caps for appsink and appsrc
    gst_app_sink_set_caps(GST_APP_SINK(this->sink), caps);
    g_signal_connect(this->sink, "new-sample", G_CALLBACK(new_sample_callback), this);
    
    // Add elements to pipelines
    gst_bin_add_many(GST_BIN(this->pipeline), this->source, this->convert, this->scale, this->sink, NULL);
    
    // Link src pipeline elements
    if (!gst_element_link_many(this->source, this->convert, this->scale, this->sink, NULL)) {
        std::cerr << "Src pipeline elements cannot be linked!" << std::endl;
        gst_object_unref(this->pipeline);
        return;
    }
     
    GstBus *src_bus = gst_element_get_bus(this->pipeline);
    gst_bus_add_watch(src_bus, bus_callback, NULL);
    gst_object_unref(src_bus);

    gst_caps_unref(caps);

    std::cout << "Pipeline initialized" << std::endl;
}

GstreamerCameraCapture::~GstreamerCameraCapture() {
    if (this->pipeline) {
        gst_element_set_state(this->pipeline, GST_STATE_NULL);
        gst_object_unref(GST_OBJECT(this->pipeline));
    }
}

void GstreamerCameraCapture::run() {
    // Start pipeline
    GstStateChangeReturn src_ret = gst_element_set_state(this->pipeline, GST_STATE_PLAYING);
    
    if (src_ret == GST_STATE_CHANGE_FAILURE) {
        std::cerr << "Failed to start pipeline!" << std::endl;
        return;
    }
    
    std::cout << "Pipeline started, capturing video..." << std::endl;
}

void GstreamerCameraCapture::stop() {
    GstStateChangeReturn src_ret = gst_element_set_state(this->pipeline, GST_STATE_PAUSED);

    if (src_ret == GST_STATE_CHANGE_FAILURE) {
        std::cerr << "Failed to stop pipeline!" << std::endl;
        return;
    }

    std::cout << "Pipeline stoped..." << std::endl;
}

// Message handler from GStreamer bus
static gboolean bus_callback(GstBus *bus, GstMessage *message, gpointer data) {
    Q_UNUSED(bus)
    Q_UNUSED(data)

    switch (GST_MESSAGE_TYPE(message)) {
        case GST_MESSAGE_ERROR: {
            GError *err;
            gchar *debug;
            gst_message_parse_error(message, &err, &debug);
            std::cerr << "Error: " << err->message << std::endl;
            g_error_free(err);
            g_free(debug);
            break;
        }
        case GST_MESSAGE_EOS:
            std::cout << "End of stream" << std::endl;
            break;
        default:
            break;
    }
    return TRUE;
}

// Convert mat to gst buffer function
GstBuffer* GstreamerCameraCapture::mat_to_gst_buffer(const Mat &frame) {
    gsize size = frame.step[0] * frame.rows;

    GstBuffer *buffer = gst_buffer_new_allocate(NULL, size, NULL);

    if (!buffer) {
        std::cerr << "Couldn't create buffer!" << std::endl;
        return nullptr;
    }

    GstMapInfo map;

    if (!gst_buffer_map(buffer, &map, GST_MAP_WRITE)) {
        std::cerr << "Couldn't map buffer!" << std::endl;
        return nullptr;
    }

    if (frame.isContinuous()) {
        memcpy(map.data, frame.data, size);
    } else {
        size_t row_size = frame.cols * frame.elemSize();
        for (int i = 0; i < frame.rows; ++i) {
            memcpy(map.data + i * row_size, frame.ptr(i), row_size);
        }
    }

    gst_buffer_unmap(buffer, &map);
    return buffer;
}

// Convert mat to gst sample function
GstSample* GstreamerCameraCapture::mat_to_gst_sample(const Mat &frame, GstCaps *caps) {
    GstBuffer *buffer = mat_to_gst_buffer(frame);

    if (!buffer) {
        return nullptr;
    }

    GstClockTime timestamp = gst_util_get_timestamp();

    GST_BUFFER_PTS(buffer) = timestamp;
    GST_BUFFER_DTS(buffer) = timestamp;

    GstSample *sample = gst_sample_new(buffer, caps, NULL, NULL);

    gst_buffer_unref(buffer);

    return sample;
}

// Function to convert gst sample to mat
Mat GstreamerCameraCapture::gst_sample_to_mat(GstSample* sample) {
    Mat result;

    GstBuffer* buffer = gst_sample_get_buffer(sample);
    GstMapInfo map;
    
    if (!gst_buffer_map(buffer, &map, GST_MAP_READ)) {
        std::cerr << "Cannot map gstreamer buffer" << std::endl;
        return Mat();
    }
    
    GstCaps* caps = gst_sample_get_caps(sample);
    if (!caps) {
        gst_buffer_unmap(buffer, &map);
        std::cerr << "Cannot get caps from sample" << std::endl;
        return result;
    }

    GstStructure* structure = gst_caps_get_structure(caps, 0);
    if (!structure) {
        gst_buffer_unmap(buffer, &map);
        gst_caps_unref(caps);
        std::cerr << "Cannot get structure from caps" << std::endl;
        return result;
    }
    
    int width = 0, height = 0;
    if (!gst_structure_get_int(structure, "width", &width) || 
        !gst_structure_get_int(structure, "height", &height) ||
        width <= 0 || height <= 0) {
        gst_buffer_unmap(buffer, &map);
        gst_caps_unref(caps);
        std::cerr << "Invalid dimensions" << std::endl;
        return result;
    }

    const char* format = gst_structure_get_string(structure, "format");
    int type;

    if (g_str_equal(format, "RGB")) {
        type = CV_8UC3;
    } else if (g_str_equal(format, "BGR")) {
        type = CV_8UC3;
    } else if (g_str_equal(format, "RGBA")) {
        type = CV_8UC4;
    } else if (g_str_equal(format, "BGRA")) {
        type = CV_8UC4;
    } else if (g_str_equal(format, "GRAY8")) {
        type = CV_8UC1;
    } else {
        type = CV_8UC3;
    }

    Mat frame(height, width, type, (void*)map.data);

    if (g_str_equal(format, "RGB")) {
        cvtColor(frame, frame, COLOR_RGB2BGR);
    }

    result = frame.clone();

    gst_buffer_unmap(buffer, &map);
    //gst_caps_unref(caps);
    
    return result;
}

void GstreamerCameraCapture::new_frame(GstElement *sink) {
    GstSample *sample = gst_app_sink_pull_sample(GST_APP_SINK(sink));
    
    if (!sample) {
        std::cerr << "Couldn't acquire sample" << std::endl;
        frame_ready.store(false);
        return;
    }

    QMutexLocker locker(&m_mutex);
    // Convert sample to Mat
    Mat frame = this->gst_sample_to_mat(sample);
    gst_sample_unref(sample); // Only unref once

    if (frame.empty()) {
        std::cerr << "Empty frame!" << std::endl;
        // Don't call gst_sample_unref(sample) again here
        frame_ready.store(false);
        return;
    }
    
    // Process the image
    this->processedFrame = frame;
    cvtColor(processedFrame, processedFrame, COLOR_BGR2RGB);

    frame_ready.store(true);
}

QPixmap GstreamerCameraCapture::pull_pixmap_from_frame() {
    QMutexLocker locker(&m_mutex);

    if (!this->frame_ready.load() || processedFrame.empty())
        return QPixmap();

    try {
        QImage image(
            processedFrame.data,
            processedFrame.cols,
            processedFrame.rows,
            processedFrame.step,
            QImage::Format_RGB888
        );

        if (image.isNull()) {
            return QPixmap();
        }

        return QPixmap::fromImage(image.copy());
    } catch (const std::exception& e) {
        std::cerr << "Exception in pull_pixmap_from_frame: " << e.what() << std::endl;
        return QPixmap();
    }
}

// Function registered for 
GstFlowReturn new_sample_callback(GstElement *sink, gpointer data) {
    GstreamerCameraCapture *instance = static_cast<GstreamerCameraCapture*>(data);
    instance->new_frame(sink);

    return GST_FLOW_OK;
}
