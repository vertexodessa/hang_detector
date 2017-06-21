#ifndef HANGDETECTOR_H
#define HANGDETECTOR_H

#include "hang_action.h"

#include <condition_variable>
#include <queue>

namespace HangDetector {

class HangDetectorImpl;

class Detector {
public:
    Detector();
    ~Detector();

    void start();
    void stop();
    void restart();
    void addAction(std::shared_ptr<HangAction> a);
    void clearActions();
private:
    std::unique_ptr<HangDetectorImpl> m_impl;
};

}
#endif
