#pragma once

#include <string>

#include <ueye.h>
#include <uEye_tools.h>

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

#include <vector>
#include <chrono>

namespace Cam {

	namespace UEYE {
		typedef struct Framestats {
			uint64_t drop_frames;
			uint64_t frame_count;
			UEYEIMAGEINFO  info;
			//std::chrono::steady_clock::time_point chr_now;
			//std::chrono::steady_clock::time_point chr_prev;
			double fps;
		} Framestats;
	};
	
	class Ueye
	{
	private:
		int m_error = 0;
		int m_bitsPerPixel = 0;
		std::vector<char *> m_imgMem = {0}; // Zeiger auf reservierten Speicherbereich f�r das Bild
		HIDS m_hCam = {0};
		CAMINFO m_camInfo = {0}; // Kamera-Daten
		SENSORINFO m_sensorInfo = {0}; // Sensor-Daten
		IS_RECT m_roi = {0};

		UEYE::Framestats m_framestats = { 0 };
		UEYE::Framestats m_framestats_prev = { 0 };
		bool m_past_first_cycle = false;

	public:
		bool m_suppress_warnings;
		bool m_suppress_infos;

		Ueye(void);
		//~Ueye(void);

		[[nodiscard]] int connect(void);
		int disconnect(void);

		int setColorMode(int color_mode);
		int setDisplayMode(int display_mode);

		int setParamPixelClock(unsigned int pixelclock_MHz, bool choose_nearest=true);
		int setParamGain(unsigned int gain, bool automatic = false);
		int setParamFramerate(double framerate, bool automatic = false);
		int setParamShutterMode(int shuttermode, bool automatic = false);
		int setParamWhiteBalance(bool automatic);
		int setParamRoi(int x, int y, int w, int h);
		int setParamExposure(double exposure_ms);
		int setParamOffset(int offset, bool automatic = false);
		int setParamGamma(double gamma);

		int rebuildFramebuffer(double bufferlen_s = 1.0);

		int paramFileLoad(std::string file);
		int paramFileLoad(void);
		int paramFileSave(std::string file);
		int paramFileSave(void);

		int videoStart(void);
		int videoFrame(cv::Mat &frame, UEYE::Framestats &stats);
		int videoStop(void);

		void printError(std::string msg);
		void printError(void);
		void printWarning(std::string msg);
		void printWarning(void);
		void printInfo(std::string msg);
	};

}


