
#ifndef __MV_GIGE_DEVICE_H__
#define __MV_GIGE_DEVICE_H__

#include "MvDeviceBase.h"

namespace MvCamCtrl
{
    class CMvGigEDevice : public IMvDevice
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
         *  @brief  Get Device XML File
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


        virtual ~CMvGigEDevice( void );


        CMvGigEDevice( const MV_CC_DEVICE_INFO* pInfo );


        // Get Net Information
        virtual int     GetNetTransInfo(MV_NETTRANS_INFO* pstInfo);


        // Force IP
        virtual int     ForceIp(unsigned int nIP);


        // Configure IP Mode
        virtual int     SetIpConfig(unsigned int nType);


        // Get Various Types of Information
        virtual int     GetAllMatchInfo(MV_ALL_MATCH_INFO* pstInfo);
        // add new interface here...

        // Register Message Exception Callback
        virtual int     RegisterExceptionCallBack(void(__stdcall* cbException)(unsigned int nMsgType, void* pUser),
                                                    void* pUser);

        virtual int     SetSingleShot(void(__stdcall* cbSingleShot)(unsigned char* pData , unsigned int nDataLen, 
                                                                    MV_FRAME_OUT_INFO* pFrameInfo, void* pUser), 
                                        void* pUser);

        // Set Device Aquisition Mode
        virtual int     SetAcquisitionMode(MV_CAM_ACQUISITION_MODE enMode);


        // Device Local Update
        virtual int     LocalUpgrade(const void *pFilePathName);

        // Get Current Update Process
        virtual int     GetUpgradeProcess(unsigned int* pnProcess);

        virtual int     GetOptimalPacketSize();

        // Display One Frame Image
        virtual int     Display(void* hWnd);

        virtual int     SetNetTransMode(unsigned int nType);

        virtual int     ReadMemory(void *pBuffer, int64_t nAddress, int64_t nLength);

        virtual int     WriteMemory(const void *pBuffer, int64_t nAddress, int64_t nLength);

        // Return Value 0 driver exception, 1 driver work properly
        virtual int     IsDriverWorking();

        // 0: Do not throw; 1: Throw
        virtual int     SetThrowAbnormalImage(int bThrow);

        // Set the number of the internal image cache nodes in SDK, range [1, 30], called before capture
        virtual int     SetImageNodeNum(unsigned int nNum);

        // Set Gvcp Timeout
        virtual int     SetGvcpTimeout(unsigned int nTimeout);

        // Register Image Data Callback
        virtual int     RegisterImageCallBack(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO* pFrameInfo, void* pUser),
                                                    void* pUser);

        // Register Event Message Callback
        virtual int     RegisterEventCallBack(void(__stdcall* cbEvent)(unsigned int nUserDefinedId, void* pUser),
                                                void* pUser);

        // Register Image Data Callback, chunk
        virtual int     RegisterImageCallBackEx(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser),void* pUser);

		virtual int     RegisterImageCallBackForRGB(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser),void* pUser);
		virtual int     RegisterImageCallBackForBGR(void(__stdcall* cbOutput)(unsigned char * pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser),void* pUser);

        virtual int     GetOneFrameEx(unsigned char * pData , unsigned int nDataSize, MV_FRAME_OUT_INFO_EX* pFrameInfo);

        virtual int     GetImageForRGB(unsigned char * pData , unsigned int nDataSize, MV_FRAME_OUT_INFO_EX* pFrameInfo, int nMsec);
        virtual int     GetImageForBGR(unsigned char * pData , unsigned int nDataSize, MV_FRAME_OUT_INFO_EX* pFrameInfo, int nMsec);

        virtual int     GetOneFrameTimeout(unsigned char * pData , IN unsigned int nDataSize, IN OUT MV_FRAME_OUT_INFO_EX* pFrameInfo, int nMsec);

    private:
        CDevRefs*       m_pRefs;
    };


}

#endif /* __MV_GIGE_DEVICE_H__ */
