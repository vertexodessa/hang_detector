#include "hang_detector.h"


#include <iostream>

using namespace std;
using namespace std::chrono;

time_t steady_clock_to_time_t( steady_clock::time_point t )
{
    return system_clock::to_time_t(system_clock::now()
                                   + (t - steady_clock::now()));
}


int main() {
    HangDetector::HangDetector hd;

    std::time_t ttp = steady_clock_to_time_t(std::chrono::steady_clock::now());
    cout << "MAIN time: " << ctime(&ttp);

    auto sec = std::chrono::milliseconds(1000);
    auto ms1 = std::chrono::milliseconds(1);

    hd.addAction(
        std::make_shared<HangDetector::CallbackAction>(sec + ms1 * 100, [](void*){
                printf("1100ms callback executed!\n");
            }, nullptr));
    hd.addAction(
        std::make_shared<HangDetector::CallbackAction>(sec + ms1 * 200, [](void*){
                printf("1200ms callback executed!\n");
            }, nullptr));
    hd.addAction(
        std::make_shared<HangDetector::CallbackAction>(2*sec + ms1 * 200, [](void*){
                printf("2200ms callback executed!\n");
            }, nullptr));

    hd.addAction(std::make_shared<HangDetector::KillAction>(sec*5));

    hd.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(15000));
    return 0;
}
