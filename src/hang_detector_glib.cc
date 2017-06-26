#include "hang_detector_glib.h"

#define log(...) do {} while(0)

namespace HangDetector {

void DetectorGlib::startWithInterval(ms interval, GMainContext *ctx) {
    stop();

    GSource *src = g_timeout_source_new(interval.count());

    auto cb = [](void* d_) -> gboolean {
        DetectorGlib* d = static_cast<DetectorGlib*>(d_);
        d->restart();
        return G_SOURCE_CONTINUE;
    };
    g_source_set_callback(src, cb, this, nullptr);

    m_srcId = g_source_attach(src, ctx);
    if (m_srcId == 0)
        log("%s Error attaching to GSource\n", __func__);

    start();
}

void DetectorGlib::stop() {
    if (m_srcId)
        g_source_remove(m_srcId);
    Detector::stop();
};

DetectorGlib::~DetectorGlib() {
    stop();
}

}
