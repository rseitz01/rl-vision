
#include <stdlib.h>
#include <stdio.h>

#include <ueye.h>
#include <uEye_tools.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "Ueye.h"


#if 1

#define CHAR_ESC 27

int main(void)
{
#if 1
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
     char* imgMem; // Zeiger auf reservierten Speicherbereich für das Bild
     int memId; // ID für den Bildspeicher (mehrere Bildspeicher sind möglich)
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
    is_AllocImageMem(hCam, img_width, img_height, nBitsPerPixel, &imgMem, &memId);
    is_SetImageMem(hCam, imgMem, memId);
    is_SetDisplayMode(hCam, IS_SET_DM_DIB);

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


    // Save parameters to file (open filebox)
    //nRet = is_ParameterSet(hCam, IS_PARAMETERSET_CMD_SAVE_FILE, NULL, NULL);

    // Load parameters from file (open filebox)
    nRet = is_ParameterSet(hCam, IS_PARAMETERSET_CMD_LOAD_FILE, NULL, NULL);


    // is_AddToSequence
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
    while (1) {
        if (is_FreezeVideo(hCam, IS_WAIT) == IS_SUCCESS) {
            void* pMemVoid; //pointer to where the image is stored
            is_GetImageMem(hCam, &pMemVoid);
            auto frame = cv::Mat(img_height, img_width, CV_8UC1, pMemVoid);
            //cv::addWeighted(frame, 1, 0, 0, offset, frame);
            cv::imshow("frame", frame);
        }
        char c = cv::waitKey(1000.0 / fps);
        if (c == 'q' || c == CHAR_ESC) {
            break;
        }
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

