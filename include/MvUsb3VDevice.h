
#ifndef __MV_USB3V_DEVICE_H__
#define __MV_USB3V_DEVICE_H__

#include "MvDeviceBase.h"

namespace MvCamCtrl
{
    class CMvUsb3VDevice : public IMvDevice
    {
    public:

        // Open Device
        virtual int     Open(unsigned int nAccessMode = MV_ACCESS_Exclusive, unsigned short nSwitchoverKey = 0);


        // Close Device
        virtual int     Close();


        // Determines the status of the device, only if false is returned, the device can be opened 
        virtual bool    IsOpen();


        // Start Grabbing
        virtual int     StartGrabbing();


        // Stop Grabbing
        virtual int     StopGrabbing();


        // Get Device Information
        virtual int     GetDeviceInfo(MV_CC_DEVICE_INFO&);


        /** @fn     GetGenICamXML(unsigned char* pData, unsigned int nDataSize, unsigned int* pnDataLen)
         *  @brief  Get Device XML Files
         *  @param  pData           [IN][OUT]   - Cache Address to copy in
                    nDataSize       [IN]        - Cache Size
                    pnDataLen       [OUT]       - XML File Data Length
         *  
         *  @return Success, return MV_OK; Fail, return Error Code
         *  @note   When pData is NULL or nDataSize is smaller than the actual xml file, do not copy data, return xml file size from the pnDataLen;
         *          When pData is a valid cache address and the cache is large enough, the full data is copied and the xml file size is returned by pnDataLen.
         */
        virtual int     GetGenICamXML(unsigned char* pData, unsigned int nDataSize, unsigned int* pnDataLen);


        /** @fn     GetOneFrame(unsigned char * pData , unsigned int nDataSize, MV_FRAME_OUT_INFO* pFrameInfo)
         *  @brief  Get One Frame Image Data
         *  @param  pData           [IN][OUT]   - Data Pointer
                    nDataLen        [IN]        - Data Length
                    pFrameInfo      [OUT]       - Output Frame Information
         *  
         *  @return Success, return MV_OK; Fail, return Error Code
         */
        virtual int     GetOneFrame(unsigned char * pData , unsigned int nDataSize, MV_FRAME_OUT_INFO* pFrameInfo);


        // Get the transport layer agent class used by GenICam
        virtual TlProxy     GetTlProxy();


        virtual ~CMvUsb3VDevice( void );


        CMvUsb3VDevice( const MV_CC_DEVICE_INFO* pInfo );

        // Display One Frame Image
        virtual int     Display(void* hWnd);


        // Get Various Types of Information
        virtual int     GetAllMatchInfo(MV_ALL_MATCH_INFO* pstInfo);

        // Register Message Exception Callback
        virtual int     RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser),
                                                    void* pUser);


        virtual int     SetSingleShot(void(__stdcall* cbSingleShot)(unsigned char* pData , unsigned int nDataLen, 
                                        MV_FRAME_OUT_INFO* pFrameInfo, void* pUser), 
                                        void* pUser);

        // Set Device Aquisition Mode
        virtual int     SetAcquisitionMode(MV_CAM_ACQUISITION_MODE enMode);


        // Device Local Upgrade
        virtual int     LocalUpgrade(const void *pFilePathName);

        // Get Current Upgrade Process
        virtual int     GetUpgradeProcess(unsigned int* pnProcess);

        virtual int     ReadMemory(void *pBuffer, int64_t nAddress, int64_t nLength);

        virtual int     WriteMemory(const void *pBuffer, int64_t nAddress, int64_t nLength);

        // Set the number of the internal image cache nodes in SDK, range [1, 30], called before capture
        virtual int     SetImageNodeNum(unsigned int nNum);

        // Register Image Data Callback
        virtual int     RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO* pFrameInfo, void* pUser),
                                                void* pUser);

    private:
        CDevRefs*       m_pRefs;
    };


}

#endif /* __MV_GIGE_DEVICE_H__ */
