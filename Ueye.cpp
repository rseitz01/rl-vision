#include "Ueye.h"

#include <ueye.h>
#include <uEye_tools.h>

#include <iostream>

namespace Ueye {

    Ueye::Ueye(void) {

    }

    int Ueye::connect(void) {
        if (!m_error && (m_error = is_InitCamera(&m_hCam, NULL))) {
            this->printError("failed opening camera");
        }
        if (!m_error && (m_error = is_GetCameraInfo(m_hCam, &m_camInfo))) {
            this->printError("failed getting camera info");
        }
        if (!m_error && (m_error = is_GetSensorInfo(m_hCam, &m_sensorInfo))) {
            this->printError("failed getting sensor info");
        }


        return m_error;
    }

    int Ueye::setColorMode(int color_mode) {

        if (!m_error && (m_error = is_SetColorMode(m_hCam, color_mode))) {
            this->printError("failed opening camera");
        }
        if (!m_error && !(m_bitsPerPixel = is_SetColorMode(m_hCam, IS_GET_BITS_PER_PIXEL))) {
            this->printError("should not expect 0 bits per pixel");
        }

        int w = m_sensorInfo.nMaxWidth;
        int h = m_sensorInfo.nMaxHeight;

        if (!m_error && (m_error = is_AllocImageMem(m_hCam, w, h, m_bitsPerPixel, &m_imgMem, &m_memId))) {
            this->printError("could not allocate image memory");
        }
        if (!m_error && (m_error = is_SetImageMem(m_hCam, m_imgMem, m_memId))) {
            this->printError("could not set image memory");
        }

        return m_error;
    }

    int Ueye::setDisplayMode(int display_mode) {
        /* display_mode possible values:
        IS_SET_DM_DIB, 
        IS_SET_DM_DIRECT3D
        IS_SET_DM_OPENGL
        IS_SET_DM_MONO
        IS_SET_DM_BAYER
        IS_SET_DM_YCBCR 
        */

        if (!m_error && (m_error = is_SetDisplayMode(m_hCam, display_mode))) {
            this->printError("could not set image memory");
        }
        
        return m_error;
    }

    int Ueye::setParam(Param par) {
        switch (par.id) {
            case PARAM::EXPOSURE: {

            } break;
        }
        return m_error;
    }

    void Ueye::printError(std::string msg) {
        //if (m_error == IS_SUCCESS) return;
        int lastError = IS_SUCCESS;
        char* errstr = 0;
        std::cerr << msg << " (" << m_error << ")" << std::endl;
        is_GetError(m_hCam, &lastError, &errstr);
        std::cerr << "last error " << lastError << ": " << (errstr ? errstr : "(null)") << std::endl;
    }

    void Ueye::printError(void) {
        this->printError("error");
    }

    Param::Param(PARAM id, bool automatic) 
        : id(id), automatic(automatic) {

    }

    Param::Param(PARAM id, UINT value, bool automatic)
        : id(id), automatic(automatic) {
        switch (id) {
        case PARAM::ROI: throw("ROI parameter needs 4 arguments");
        case PARAM::WHITEBALANCE: throw("WHITEBALANCE takes no arguments");
        }
        this->u = value;
        this->d = (double)value;
    }
    Param::Param(PARAM id, double value, bool automatic)
        : id(id), automatic(automatic) {
        switch(id) {
        case PARAM::ROI: throw("ROI parameter needs 4 arguments");
        case PARAM::WHITEBALANCE: throw("WHITEBALANCE takes no arguments");
        }
        this->u = value;
        this->d = (double)value;
    }

    Param::Param(PARAM id, INT x, INT y, INT w, INT h)
        : id(id), automatic(automatic) {
    }

};

