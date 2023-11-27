#pragma once

#include <string>

#include <ueye.h>
#include <uEye_tools.h>

namespace Ueye {

	enum PARAM {
		NONE,
		GAIN, // mod, auto, UINT
		WHITEBALANCE, // auto, NONE
		FRAMERATE, // mod, auto, double
		SHUTTERMODE, // mod, auto, INT ; ROLLING/GLOBAL
		PIXELCLOCK, // mod, UINT
		ROI, // mod (AOI), RECT ; X,Y,W,H
		EXPOSURE, // mod, double
		GAMMA, // mod, UINT
		OFFSET, // mod (BLACKLEVEL), (U)INT
	};

	class Param
	{
	public:
		PARAM id = PARAM::NONE;
		bool automatic = false;
		double d = 0;
		UINT u = 0;
		IS_RECT r = { 0 };

		Param(PARAM id, bool automatic=false);
		Param(PARAM id, UINT value, bool automatic=false);
		Param(PARAM id, double value, bool automatic = false);
		Param(PARAM id, INT x, INT y, INT w, INT h);
	};

	class Ueye
	{
	private:
		int m_error = 0;
		int m_memId = 0; // ID für den Bildspeicher (mehrere Bildspeicher sind möglich)
		int m_bitsPerPixel;
		char* m_imgMem = 0; // Zeiger auf reservierten Speicherbereich für das Bild
		HIDS m_hCam = {0};
		CAMINFO m_camInfo = { 0 }; // Kamera-Daten
		SENSORINFO m_sensorInfo = { 0 }; // Sensor-Daten
	public:
		Ueye(void);
		[[nodiscard]] int connect(void);
		[[nodiscard]] int setColorMode(int color_mode);
		[[nodiscard]] int setDisplayMode(int display_mode);
		[[nodiscard]] int setParam(Param par);
		[[nodiscard]] int setParamGain(UINT gain, bool automatic = false);
		[[nodiscard]] int setParamWhiteBalance(bool automatic);
		[[nodiscard]] int setParamFrameRate(bool automatic);
		[[nodiscard]] int setParamShutterMode(bool automatic);
		[[nodiscard]] int setParamPixelClock(bool automatic);
		[[nodiscard]] int setParamRoi(bool automatic);
		[[nodiscard]] int setParamExposure(bool automatic);
		[[nodiscard]] int setParamGamma(bool automatic);
		[[nodiscard]] int setParamOffset(bool automatic);
		void printError(std::string msg);
		void printError(void);
	};

}


