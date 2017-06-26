#ifndef HANG_DETECTOR_GLIB
#define HANG_DETECTOR_GLIB

#include <hang_detector.h>
#include <glib.h>

namespace HangDetector {

class DetectorGlib : public Detector {
public:
    void startWithInterval(ms interval,
                           GMainContext *ctx = g_main_context_get_thread_default());
    void stop();
    ~DetectorGlib();
private:
    int m_srcId {0};
};

}

#endif
