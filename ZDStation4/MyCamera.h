/**************************************************************************************/
/* Based on C++ interface, packed commonly used functions for convenience of the user */
/**************************************************************************************/

#ifndef _MY_CAMERA_H_
#define _MY_CAMERA_H_

#include <stdio.h>
#include "MvErrorDefine.h"
#include "CameraParams.h"
#include "MvCameraControl.h"

#define MY_OK           0
#define MY_FAIL         -1

class CMyCamera
{
public:
    // Constructor
    CMyCamera();
    // Destructor
    ~CMyCamera();

    static int EnumDevices(MV_CC_DEVICE_INFO_LIST* pstDevList);

    // Open Device
    int     Open(MV_CC_DEVICE_INFO* pstDeviceInfo);

    // Close Device
    int     Close();

    // Start Grabbing
    int     StartGrabbing();

    // Stop Grabbing
    int     StopGrabbing();

    // Get One Frame Image Data
    int     GetOneFrame(unsigned char* pData, unsigned int* pnDataLen, unsigned int nDataSize, MV_FRAME_OUT_INFO* pFrameInfo);

    // Get One Frame Image Data Timeout
    int     GetOneFrameTimeout(unsigned char* pData, unsigned int* pnDataLen, unsigned int nDataSize, MV_FRAME_OUT_INFO_EX* pFrameInfo, int nMsec);

	int     GetImageForBGR(unsigned char* pData, unsigned int* pnDataLen, unsigned int nDataSize, MV_FRAME_OUT_INFO_EX* pFrameInfo, int nMsec);
    // Set Display Window Handle
    int     Display(void* hWnd);

    // Save Image
    int     SaveImage(MV_SAVE_IMAGE_PARAM_EX* pstParam);


    // Register Image Data CallBack
    int     RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser),
                                    void* pUser);

    // Register Message Exception CallBack
    int     RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser),
                                        void* pUser);

    // Get and Set Int type parameters, such as Width and Height, for details please refer to HikCameraNode.xlsx file under SDK installation directory
    int     GetIntValue(IN const char* strKey, OUT unsigned int *pnValue);
    int     SetIntValue(IN const char* strKey, IN unsigned int nValue);

    // Get and Set Float type parameters, such as ExposureTime and Gain, for details please refer to HikCameraNode.xlsx file under SDK installation directory
    int     GetFloatValue(IN const char* strKey, OUT float *pfValue);
    int     SetFloatValue(IN const char* strKey, IN float fValue);

    // Get and Set Enum type parameters, such as PixelFormat, for details please refer to HikCameraNode.xlsx file under SDK installation directory
    int     GetEnumValue(IN const char* strKey, OUT unsigned int *pnValue);
    int     SetEnumValue(IN const char* strKey, IN unsigned int nValue);

    // Get and Set Bool type parameters, such as ReverseX, for details please refer to HikCameraNode.xlsx file under SDK installation directory
    int     GetBoolValue(IN const char* strKey, OUT bool *pbValue);
    int     SetBoolValue(IN const char* strKey, IN bool bValue);

    // Get and Set String type parameters, such as DeviceUserID, for details please refer to HikCameraNode.xlsx file under SDK installation directory
    int     GetStringValue(IN const char* strKey, IN OUT char* strValue, IN unsigned int nSize);
    int     SetStringValue(IN const char* strKey, IN const char * strValue);

    // Execute Command once, such as UserSetSave, for details please refer to HikCameraNode.xlsx file under SDK installation directory
    int     CommandExecute(IN const char* strKey);

    // Get statistical parameter
    int     GetAllMatchInfo(MV_ALL_MATCH_INFO* pstInfo);
private:

    void*               m_hDevHandle;

public:
    unsigned char*  m_pBufForSaveImage;         // Buffer to save image
    unsigned int    m_nBufSizeForSaveImage;

    unsigned char*  m_pBufForDriver;            // Buffer to get image from driver
    unsigned int    m_nBufSizeForDriver;


};

#endif
