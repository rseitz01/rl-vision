
#include <stdlib.h>
#include <stdio.h>

#include <ueye.h>
//#include <uEye_tools.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
//#include "Ueye.h"

#include <vector>
#include <chrono>

#if 1

#define CHAR_ESC 27



int main(void)
{
#if 0
    auto ueye = Ueye::Ueye();

    if (ueye.connect()) return -1;
    if (ueye.setColorMode(IS_CM_MONO8)) return -1;
    if (ueye.setDisplayMode(IS_SET_DM_MONO)) return -1;

    if (ueye.setParam(Ueye::Param(Ueye::PARAM::GAIN, (UINT)10))) return -1;
    

#else

    HIDS hCam = { 0 }; // IDS VideoCapture Device
    int exp_time = 50; // Belichtungszeit
    CAMINFO camInfo; // Kamera-Daten
    SENSORINFO sensorInfo; // Sensor-Daten
    std::vector<char*>imgMems;
    std::vector<int>memIds;
    //char* imgMem; // Zeiger auf reservierten Speicherbereich für das Bild
    //int memId; // ID für den Bildspeicher (mehrere Bildspeicher sind möglich)
    char* errstr = 0;

    if (is_InitCamera(&hCam, NULL) == IS_SUCCESS) {
        printf("opened camera\n");
    }
    else {
        printf("failed opening camera\n");
    }

    // If successful, fetch camera and sensor info
    int nRet;
    nRet = is_GetCameraInfo(hCam, &camInfo);
    nRet = is_GetSensorInfo(hCam, &sensorInfo);

    int nColorMode = IS_CM_MONO8;
    int nBitsPerPixel = 8;

    // c)
    is_SetColorMode(hCam, nColorMode);
    int img_width = 1600;
    int img_height = 1200;
#if 0 //do this below
    is_AllocImageMem(hCam, img_width, img_height, nBitsPerPixel, &imgMem, &memId);
    is_SetImageMem(hCam, imgMem, memId);
    is_SetDisplayMode(hCam, IS_SET_DM_DIB);
#endif

    // parameters
    double exposure = (double)exp_time;
    double fps = 60; // ...60 fps
    UINT pixelclock = 128; // 10 - 128 (MHz)
    UINT gamma = 220; // 1.6 (160) ... 2.2 (220)
    UINT gain = 100; // 0...100
    // IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_ROLLING
    // IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_GLOBAL
    INT shuttermode = IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_GLOBAL;
    INT offset = 10;
    // roi
    IS_RECT rectAOI;
    rectAOI.s32X = 0;
    rectAOI.s32Y = 0;
    rectAOI.s32Width = img_width;
    rectAOI.s32Height = img_height;


    // d)
    double enable = 1;
    double disable = 0;
    is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_GAIN, &disable, 0);
    //is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_WHITEBALANCE, &disable, 0);
    is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_FRAMERATE, &disable, 0);
    is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_SHUTTER, &disable, 0);
    //is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_SENSOR_GAIN, &disable, 0);
    //is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_SENSOR_WHITEBALANCE, &disable, 0);
    //is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_SENSOR_SHUTTER, &disable, 0);

#if 1
    UINT nNumberOfSupportedPixelClocks = 0;
    nRet = is_PixelClock(hCam, IS_PIXELCLOCK_CMD_GET_NUMBER,
        (void*)&nNumberOfSupportedPixelClocks,
        sizeof(nNumberOfSupportedPixelClocks));
    if ((nRet == IS_SUCCESS) && (nNumberOfSupportedPixelClocks > 0))
    {
        // No camera has more than 150 different pixel clocks.
        // Of course, the list can be allocated dynamically
        UINT nPixelClockList[150];
        ZeroMemory(&nPixelClockList, sizeof(nPixelClockList));

        nRet = is_PixelClock(hCam, IS_PIXELCLOCK_CMD_GET_LIST,
            (void*)nPixelClockList,
            nNumberOfSupportedPixelClocks * sizeof(UINT));
        for (int i = 0; i < nNumberOfSupportedPixelClocks; i++) {
            printf("pixelclock : %d\n", nPixelClockList[i]);
        }
    }
#endif

    // set parameters
    int error = IS_SUCCESS;
    int lastError = IS_SUCCESS;
    if ((error = is_PixelClock(hCam, IS_PIXELCLOCK_CMD_SET, &pixelclock, sizeof(pixelclock)))) {
        fprintf(stderr, "could not set pixel clock (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        fprintf(stderr, "error %d: %s\n", lastError, errstr);
    }
    if ((error = is_AOI(hCam, IS_AOI_IMAGE_SET_AOI, &rectAOI, sizeof(rectAOI)))) {
        printf("could not set region of interest (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    }
    if ((error = is_Exposure(hCam, IS_EXPOSURE_CMD_SET_EXPOSURE, &exposure, sizeof(exposure)))) {
        printf("could not set exposure time (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    }
    if ((error = is_SetFrameRate(hCam, fps, &fps))) {
        printf("could not set frame rate (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    } else {
        printf("set frame rate: %.2f\n", fps);
    }
    if ((error = is_Gamma(hCam, IS_GAMMA_CMD_SET, &gamma, sizeof(gamma)))) {
        printf("could not set gamma (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    } else {
        printf("set gamma: %.2f\n", (double)gamma / 100);
    }
    if ((error = is_DeviceFeature(hCam, IS_DEVICE_FEATURE_CMD_SET_SHUTTER_MODE, &shuttermode, sizeof(shuttermode)))) {
        printf("could not set shutter mode (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    }
    // IS_GET_MASTER_GAIN
    // IS_SET_ENABLE_AUTO_GAIN
    if ((error = is_SetHardwareGain(hCam, IS_GET_MASTER_GAIN, gain, gain, gain))) {
        printf("could not set gain (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    }
    if ((error = is_Blacklevel(hCam, IS_BLACKLEVEL_CMD_SET_OFFSET, &offset, sizeof(offset)))) {
        printf("could not set offset (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    }



    for (double f = 0; f < 1.0; f += 1.0 / fps) {
        size_t i = imgMems.size();
        imgMems.resize(i + 1);
        memIds.resize(i + 1);
        char* imgMem = imgMems.back();
        error = is_AllocImageMem(hCam, img_width, img_height, nBitsPerPixel, &imgMem, &memIds[i]);
        if (error) break;
        error = is_AddToSequence(hCam, imgMem, memIds[i]);
        if (error) break;
        //printf("memids[%d] = %d\n", i, memIds[i]);
    }
    if (error) {
        printf("error while allocating or setting image memory (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    }
    if ((error = is_ImageQueue(hCam, IS_IMAGE_QUEUE_CMD_INIT, 0, 0))) {
        printf("could not enable image queue (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    }


    if ((error = is_SetDisplayMode(hCam, IS_SET_DM_DIB))) {
        printf("could not set display mode (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    }

    // setLongExposure bei > 80 irgendwas?


    // Save parameters to file (open filebox)
    //nRet = is_ParameterSet(hCam, IS_PARAMETERSET_CMD_SAVE_FILE, NULL, NULL);

    // Load parameters from file (open filebox)
    nRet = is_ParameterSet(hCam, IS_PARAMETERSET_CMD_LOAD_FILE, NULL, NULL);


    // is_AddToSequence
    // is_FreezeVideo
    // is_GetImageInfo (timestamps)
    // nMemId / dwImageBuffers 

#if 0
    // Get offset range
    IS_RANGE_S32 nRange;
    nRet = is_Blacklevel(hCam, IS_BLACKLEVEL_CMD_GET_OFFSET_RANGE, (void*)&nRange, sizeof(nRange));
    INT nOffsetMin = nRange.s32Min;
    INT nOffsetMax = nRange.s32Max;
    INT nOffsetInc = nRange.s32Inc;
    printf("offset %d..%d (inc : %d)\n", nOffsetMin, nOffsetMax, nOffsetInc);
#endif

    // e)
    // is_CaptureVideo

    if ((error = is_SetExternalTrigger(hCam, IS_SET_TRIGGER_OFF))) {
        printf("could not disable external trigger (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    }

    if ((error = is_CaptureVideo(hCam, IS_DONT_WAIT))) {
        printf("could not enable video capture (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    }

    //clock_t frame_t_last = clock();
    //clock_t frame_t_now = clock();
    uint64_t drop_frames = 0;
    uint64_t frame_count = 0;
    std::chrono::steady_clock::time_point chr_now = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point chr_prev;
    cv::String fps_str = "FPS : ...";
   
    while (1) {
#if 1
        char* pcMem = 0;
        int nMemId = 0;
        unsigned int pending = 0;
        if ((error = is_ImageQueue(hCam, IS_IMAGE_QUEUE_CMD_GET_PENDING, &pending, sizeof(pending)))) {
            printf("could not get pending image queue (%d)\n", error);
            is_GetError(hCam, &lastError, &errstr);
            printf("error %d: %s\n", lastError, errstr);
        }
        printf("pending : %d\n", pending);
        
        if (pending) {

            is_GetImageMem(hCam, (void**)&pcMem);
            is_LockSeqBuf(hCam, 0, pcMem);
            is_UnlockSeqBuf(hCam, 0, pcMem);

            //if (first_tp == std::chrono::steady_clock::time_point{}) {
            //first_tp = std::chrono::steady_clock::now();
            //}
            //frame_t_last = frame_t_now;
            //frame_t_now = clock();
            //clock_t frame_t_delta = frame_t_now - frame_t_last;
            
            chr_now = std::chrono::steady_clock::now();
            frame_count++;
            if (chr_now - chr_prev >= std::chrono::seconds{1}) {
                /* Do something with the fps in frame_counter */
                long long frame_delta = (std::chrono::duration_cast<std::chrono::nanoseconds>(chr_now - chr_prev)).count();
                double fps_actual = frame_delta ? 1.0e9 / (double)frame_delta * frame_count : 0;
                char fps_str_buf[32] = { 0 };
                std::snprintf(fps_str_buf, 32, "%.2f", fps_actual);
                fps_str = cv::String("FPS : " + std::string(fps_str_buf));
                chr_prev = chr_now;
                frame_count = 0;
            }

            auto frame = cv::Mat(img_height, img_width, CV_8UC1, pcMem);
            auto drop_frames_str = cv::String("Drop Frames : " + std::to_string(drop_frames));

            cv::putText(frame, drop_frames_str, cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
            cv::putText(frame, fps_str, cv::Point(10, 100), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
            cv::imshow("frame", frame);


            drop_frames += (pending - 1);
            printf("pending : %d\n", pending);
            if ((error = is_ImageQueue(hCam, IS_IMAGE_QUEUE_CMD_DISCARD_N_ITEMS, &pending, sizeof(pending)))) {
                printf("could not discard n items in image queue (%d)\n", error);
                is_GetError(hCam, &lastError, &errstr);
                printf("error %d: %s\n", lastError, errstr);
            }
        }
#else
        //if (is_FreezeVideo(hCam, IS_WAIT) == IS_SUCCESS) {
            void* pMemVoid; //pointer to where the image is stored
            is_GetImageMem(hCam, &pMemVoid);
            auto frame = cv::Mat(img_height, img_width, CV_8UC1, pMemVoid);
            //cv::addWeighted(frame, 1, 0, 0, offset, frame);
            cv::imshow("frame", frame);
        }
#endif
        //char c = cv::waitKey((int)(1000.0 / fps));
        char c = cv::waitKey(1);
        if (c == 'q' || c == CHAR_ESC) {
            break;
        }
    }

    if ((error = is_StopLiveVideo(hCam, IS_FORCE_VIDEO_STOP))) {
        printf("could not stop video capture (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    }

    if ((error = is_ImageQueue(hCam, IS_IMAGE_QUEUE_CMD_EXIT, 0, 0))) {
        printf("could not stop video capture (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    }

    if ((error = is_ClearSequence(hCam))) {
        printf("could not stop video capture (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    }

    for (size_t i = 0; i < imgMems.size(); i++) {
        char* pcMem = imgMems[i];
        error = is_FreeImageMem(hCam, pcMem, memIds[i]);
        if (error) break;
    }
    if (error) {
        printf("error while freeing image memory (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    }

    if ((error = is_ExitCamera(hCam))) {
        printf("could not close camera (%d)\n", error);
        is_GetError(hCam, &lastError, &errstr);
        printf("error %d: %s\n", lastError, errstr);
    }


    // is_ParameterSet (write settings to file)

#endif

	printf("Done\n");
	return 0;
}
#else

// include the basic windows header file
#include <windows.h>
#include <windowsx.h>

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam);

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    // the handle for the window, filled by a function
    HWND hWnd;
    // this struct holds information for the window class
    WNDCLASSEX wc;

    // clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = L"WindowClass1";

    // register the window class
    RegisterClassEx(&wc);

    // create the window and use the result as the handle
    hWnd = CreateWindowEx(NULL,
        L"WindowClass1",    // name of the window class
        L"Our First Windowed Program",   // title of the window
        WS_OVERLAPPEDWINDOW,    // window style
        300,    // x-position of the window
        300,    // y-position of the window
        500,    // width of the window
        400,    // height of the window
        NULL,    // we have no parent window, NULL
        NULL,    // we aren't using menus, NULL
        hInstance,    // application handle
        NULL);    // used with multiple windows, NULL

    // display the window on the screen
    ShowWindow(hWnd, nCmdShow);

    // enter the main loop:

    // this struct holds Windows event messages
    MSG msg;

    HIDS phCam = 0;
    if (is_InitCamera(&phCam, hWnd) == IS_SUCCESS) {
        OutputDebugStringA("opened camera\n");
    }
    else {
        OutputDebugStringA("skill issue\n");
        return -1;
    }

    // wait for the next message in the queue, store the result in 'msg'
    while (GetMessage(&msg, NULL, 0, 0))
    {
        // translate keystroke messages into the right format
        TranslateMessage(&msg);

        is_CaptureVideo(phCam, 0);

        // send the message to the WindowProc function
        DispatchMessage(&msg);
    }

    // return this part of the WM_QUIT message to Windows
    return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // sort through and find what code to run for the message given
    switch (message)
    {
        // this message is read when the window is closed
    case WM_DESTROY:
    {
        // close the application entirely
        PostQuitMessage(0);
        return 0;
    } break;
    }

    // Handle any messages the switch statement didn't
    return DefWindowProc(hWnd, message, wParam, lParam);
}
#endif

