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

    ueye.setParamPixelClock(-1);
    //ueye.setParamRoi(0, 0, 1000, 1000);
    ueye.setParamExposure(50);
    ueye.setParamFramerate(60);
    ueye.setParamGamma(2.2);
    ueye.setParamShutterMode(IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_GLOBAL);
    ueye.setParamGain(100);
    ueye.setParamOffset(10);
    //ueye.paramFileLoad();

    ueye.videoStart();
    // is_GetFramesPerSecond

    cv::String str_fps = "FPS : ...";
    cv::Mat frame;

    int text_thickness = 2;
    int text_border = 8;
    int y_delta = 50;
    int y_zero = y_delta;
    double text_size = 1.2;

    while (1) {
        if (!ueye.videoFrame(frame, stats)) {

            /* create drop frames string */
            auto str_drop_frames = cv::String("Drop Frames : " + std::to_string(stats.drop_frames));

            /* create timestamp */
            char str_timestamp[32] = { 0 };
            snprintf(str_timestamp, 32, "%02d.%02d.%04d, %02d:%02d:%02d:%03d",
                stats.info.TimestampSystem.wDay,
                stats.info.TimestampSystem.wMonth,
                stats.info.TimestampSystem.wYear,
                stats.info.TimestampSystem.wHour,
                stats.info.TimestampSystem.wMinute,
                stats.info.TimestampSystem.wSecond,
                stats.info.TimestampSystem.wMilliseconds);

            /* create fps string */
            if (stats.fps) {
                char fps_buf[32] = { 0 };
                snprintf(fps_buf, 32, "%.2f", stats.fps);
                str_fps = cv::String("FPS : " + std::string(fps_buf));
            }

            /* output all texts */
            int y = y_zero;
            cv::putText(frame, str_timestamp, cv::Point(10, y), cv::FONT_HERSHEY_SIMPLEX, text_size, cv::Scalar(0, 0, 0), text_border);
            cv::putText(frame, str_timestamp, cv::Point(10, y), cv::FONT_HERSHEY_SIMPLEX, text_size, cv::Scalar(255, 255, 255), text_thickness);
            y += y_delta;
            cv::putText(frame, str_drop_frames, cv::Point(10, y), cv::FONT_HERSHEY_SIMPLEX, text_size, cv::Scalar(0, 0, 0), text_border);
            cv::putText(frame, str_drop_frames, cv::Point(10, y), cv::FONT_HERSHEY_SIMPLEX, text_size, cv::Scalar(255, 255, 255), text_thickness);
            y += y_delta;
            cv::putText(frame, str_fps, cv::Point(10, y), cv::FONT_HERSHEY_SIMPLEX, text_size, cv::Scalar(0, 0, 0), text_border);
            cv::putText(frame, str_fps, cv::Point(10, y), cv::FONT_HERSHEY_SIMPLEX, text_size, cv::Scalar(255, 255, 255), text_thickness);

            /* show final frame */
            cv::namedWindow("frame", cv::WINDOW_NORMAL);
            cv::imshow("frame", frame);
        }

        char c = cv::waitKey(1);
        if (c > 0) {
            if (c == 'q' || c == 'Q' || c == CHAR_ESC) {
                break;
            }
            else if (c == 'l' || c == 'L') {
                ueye.paramFileLoad();
            }
            else if (c == 's' || c == 'S') {
                ueye.paramFileSave();
            }
        }
    }

    ueye.videoStop();

	printf("done\n");
    return 0;
}
