#include <ueye.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include "Cam.hpp"

#define CHAR_ESC 27

int main(void)
{
    auto ueye = Cam::Ueye();
    ueye.m_suppress_warnings = true;
    Cam::UEYE::Framestats stats = {0};

    if (ueye.connect()) return -1;
    ueye.setColorMode(IS_CM_BGR8_PACKED);
    ueye.setDisplayMode(IS_SET_DM_DIB);

    ueye.setParamPixelClock(128);
    //ueye.setParamRoi(0, 0, 1000, 1000);
    ueye.setParamExposure(50);
    ueye.setParamFramerate(60);
    ueye.setParamGamma(2.2);
    ueye.setParamShutterMode(IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_GLOBAL);
    ueye.setParamGain(100);
    ueye.setParamOffset(10);

    ueye.rebuildFramebuffer();

    //ueye.paramFileLoad();

    ueye.videoStart();
    // is_GetFramesPerSecond

    cv::String fps_str = "FPS : ...";
    cv::Mat frame;

    while (1) {
        if (!ueye.videoFrame(frame, stats)) {
            auto drop_frames_str = cv::String("Drop Frames : " + std::to_string(stats.drop_frames));
            if (stats.fps) {
                char fps_buf[32] = { 0 };
                snprintf(fps_buf, 32, "%.2f", stats.fps);
                fps_str = cv::String("FPS : " + std::string(fps_buf));
            }
            cv::putText(frame, drop_frames_str, cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
            cv::putText(frame, fps_str, cv::Point(10, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
            cv::imshow("frame", frame);
        }

        char c = cv::waitKey(1);
        if (c == 'q' || c == CHAR_ESC) {
            break;
        }
    }

    ueye.videoStop();

	printf("done\n");
    return 0;
}
