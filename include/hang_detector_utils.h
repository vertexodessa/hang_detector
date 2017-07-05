#pragma once
#ifndef HANG_DETECTOR_UTILS
#define HANG_DETECTOR_UTILS

#include <hang_detector_glib.h>

#include <unistd.h>
#include <string>
#include <sstream>

namespace HangDetector {

inline int envVarToInt(const char* var) {
    if (!var) {
        return -1;
    }

    const char* val = getenv(var);
    if (!val) {
        return -1;
    }

    int ret = -1;
    std::istringstream iss (val);
    iss >> std::ws >> ret >> std::ws;

    return (iss.eof()) ? ret : -1;
}

inline bool initFromEnvironment(HangDetector::DetectorGlib& hd, const std::string& prefix = std::string("HANGDETECTOR")) {
    bool installed = false;

    int logTimeout       = envVarToInt((prefix + "_LOG_TIMEOUT_MS").c_str());
    int microhangTimeout = envVarToInt((prefix + "_MICROHANG_TIMEOUT_MS").c_str());
    int hangTimeout      = envVarToInt((prefix + "_HANG_TIMEOUT_MS").c_str());

    const int kDefaultInterval = 1000;
    int interval         = (envVarToInt((prefix + "_HANG_INTERVAL_MS").c_str()) == -1) ? kDefaultInterval : envVarToInt((prefix + "_HANG_INTERVAL_MS").c_str());

    if (logTimeout != -1 ) {
        printf("Installing hang log callback for %d milliseconds\n", logTimeout);
        auto logAction = std::make_shared<HangDetector::CallbackAction<void>>(HangDetector::ms(logTimeout), [=](void*) {
                printf("Main loop hung for %d milliseconds\n", logTimeout);
                fflush(stdout);
            }, nullptr);
        hd.addAction(logAction);
        installed = true;
    }

    if (microhangTimeout != -1) {
        printf("Installing microhang callback for %d milliseconds\n", microhangTimeout);

        std::string minidump_path("/opt/minidumps/");
        if (getenv((prefix + "_HANG_MINIDUMP_PATH").c_str())) {
            minidump_path = getenv((prefix + "_HANG_MINIDUMP_PATH").c_str());
        }

        auto minidumpAction = std::make_shared<HangDetector::WriteMinidumpAction>(HangDetector::ms(microhangTimeout), minidump_path.c_str());
        hd.addAction(minidumpAction);
        installed = true;
    }

    if (hangTimeout != -1) {
        printf("Installing hang callback for %d milliseconds\n", hangTimeout);
        auto hangAction = std::make_shared<HangDetector::CallbackAction<void>>(HangDetector::ms(hangTimeout), [=](void*) {
                fprintf(stderr, "Main loop hung for %d milliseconds, killing\n", hangTimeout);
                fflush(stdout);
                kill(getpid(), 8);
            }, nullptr);
        hd.addAction(hangAction);
        installed = true;
    }

    if (installed)
        hd.startWithInterval(HangDetector::ms(interval));

    return installed;
}

}// namespace HangDetector

#endif // HANG_DETECTOR_UTILS
