
#ifndef _MV_CAMERA_PARAMS_H_
#define _MV_CAMERA_PARAMS_H_

#include "PixelType.h"

// Device Type Definition
#define MV_UNKNOW_DEVICE        0x00000000          // Unknown Device Type, Reserved 
#define MV_GIGE_DEVICE          0x00000001          // GigE Device
#define MV_1394_DEVICE          0x00000002          // 1394-a/b Device
#define MV_USB_DEVICE           0x00000004          // USB3.0 Device
#define MV_CAMERALINK_DEVICE    0x00000008          // CameraLink Device

typedef struct _MV_GIGE_DEVICE_INFO_
{
    unsigned int        nIpCfgOption;        
    unsigned int        nIpCfgCurrent;       //IP configuration:bit31-static bit30-dhcp bit29-lla          
    unsigned int        nCurrentIp;          //Curtent ip          
    unsigned int        nCurrentSubNetMask;  //Curtent subnet mask             
    unsigned int        nDefultGateWay;      //Current gateway
    unsigned char       chManufacturerName[32];
    unsigned char       chModelName[32];
    unsigned char       chDeviceVersion[32];
    unsigned char       chManufacturerSpecificInfo[48];
    unsigned char       chSerialNumber[16];
    unsigned char       chUserDefinedName[16];      

    unsigned int        nNetExport;         // GIGE IP Address

    unsigned int        nReserved[4];

}MV_GIGE_DEVICE_INFO;

#define INFO_MAX_BUFFER_SIZE 64
typedef struct _MV_USB3_DEVICE_INFO_
{
    unsigned char           CrtlInEndPoint;                        //Control input endpoint
    unsigned char           CrtlOutEndPoint;                       //Control output endpoint
    unsigned char           StreamEndPoint;                        //Flow endpoint
    unsigned char           EventEndPoint;                         //Event endpoint
    unsigned short          idVendor;                              //Vendor ID Number
    unsigned short          idProduct;                             //Device ID Number
    unsigned int            nDeviceNumber;                         //Device Serial Number
    unsigned char           chDeviceGUID[INFO_MAX_BUFFER_SIZE];    //Device GUID Number
    unsigned char           chVendorName[INFO_MAX_BUFFER_SIZE];    //Vendor Name
    unsigned char           chModelName[INFO_MAX_BUFFER_SIZE];     //Model Name
    unsigned char           chFamilyName[INFO_MAX_BUFFER_SIZE];    //Family Name
    unsigned char           chDeviceVersion[INFO_MAX_BUFFER_SIZE];  //Device Version
    unsigned char           chManufacturerName[INFO_MAX_BUFFER_SIZE]; //Manufacturer Name
    unsigned char           chSerialNumber[INFO_MAX_BUFFER_SIZE];      //Serial Number
    unsigned char           chUserDefinedName[INFO_MAX_BUFFER_SIZE];  //User Defined Name
    unsigned int            nbcdUSB;                               //Support USB Protocol
    unsigned int            nReserved[3];                             //Reserved bytes
}MV_USB3_DEVICE_INFO;

// Device Infomation
typedef struct _MV_CC_DEVICE_INFO_
{
    // common info 
    unsigned short      nMajorVer;
    unsigned short      nMinorVer;
    unsigned int        nMacAddrHigh;               // MAC Address
    unsigned int        nMacAddrLow;

    unsigned int        nTLayerType;                // Device Transport Layer Protocol Type, e.g. MV_GIGE_DEVICE

    unsigned int        nReserved[4];

    union
    {
        MV_GIGE_DEVICE_INFO stGigEInfo;
        MV_USB3_DEVICE_INFO stUsb3VInfo; 
        // more ...
    }SpecialInfo;

}MV_CC_DEVICE_INFO;

// Network transmission information
typedef struct _MV_NETTRANS_INFO_
{
    int64_t         nReviceDataSize;    // Received Data Size  [Calculate the Data Size between StartGrabbing and StopGrabbing]
    int             nThrowFrameCount;   // Throw frame number
    unsigned int    nReserved[5];

}MV_NETTRANS_INFO;


// The maximum number of supported transport layer instances
#define MV_MAX_TLS_NUM          8
// The maximum number of supported devices
#define MV_MAX_DEVICE_NUM      256

// Device Information List
typedef struct _MV_CC_DEVICE_INFO_LIST_
{
    unsigned int            nDeviceNum;                         // Online Device Number
    MV_CC_DEVICE_INFO*      pDeviceInfo[MV_MAX_DEVICE_NUM];     // Support up to 256 devices

}MV_CC_DEVICE_INFO_LIST;


// Output Frame Information
typedef struct _MV_FRAME_OUT_INFO_
{
    unsigned short      nWidth;             // Image Width
    unsigned short      nHeight;            // Image Height
    enum MvGvspPixelType     enPixelType;        // Pixel Type

    /*The following fields are not supported now*/
    unsigned int        nFrameNum;          // Frame Number
    unsigned int        nDevTimeStampHigh;  // Timestamp high 32 bits
    unsigned int        nDevTimeStampLow;   // Timestamp low 32 bits
    unsigned int        nReserved0;         // Reserved, 8-byte aligned
    int64_t             nHostTimeStamp;     // Host-generated timestamp

    unsigned int        nFrameLen;

    unsigned int        nReserved[3];       // Reserved
}MV_FRAME_OUT_INFO;

// Output Frame Information
typedef struct _MV_FRAME_OUT_INFO_EX_
{
    unsigned short      nWidth;             // Image Width
    unsigned short      nHeight;            // Image Height
    enum MvGvspPixelType     enPixelType;        // Pixel Type

    /*The following fields are not supported now*/
    unsigned int        nFrameNum;          // Frame Number
    unsigned int        nDevTimeStampHigh;  // Timestamp high 32 bits
    unsigned int        nDevTimeStampLow;   // Timestamp low 32 bits
    unsigned int        nReserved0;         // Reserved, 8-byte aligned
    int64_t             nHostTimeStamp;     // Host-generated timestamp

    unsigned int        nFrameLen;

    // The followings are chunk add frame-specific information
    // Device frame-specific time scale
    unsigned int        nSecondCount;
    unsigned int        nCycleCount;
    unsigned int        nCycleOffset;

    float               fGain;
    float               fExposureTime;
    unsigned int        nAverageBrightness;     //Average brightness

    // White balance
    unsigned int        nRed;
    unsigned int        nGreen;
    unsigned int        nBlue;

    unsigned int        nFrameCounter;
    unsigned int        nTriggerIndex;      //Trigger Counting

    //Line Input/Output
    unsigned int        nInput;        //Input
    unsigned int        nOutput;       //Output

    // ROI Region
    unsigned short      nOffsetX;
    unsigned short      nOffsetY;

    unsigned int        nReserved[41];       // Reserved
}MV_FRAME_OUT_INFO_EX;

typedef struct _MV_DISPLAY_FRAME_INFO_
{
    void*                    hWnd;
    unsigned char*           pData;
    unsigned int             nDataLen;
    unsigned short           nWidth;             // Width
    unsigned short           nHeight;            // Height
    enum MvGvspPixelType     enPixelType;        // Pixel format
    unsigned int             nRes[4];

}MV_DISPLAY_FRAME_INFO;

// Save image type
enum MV_SAVE_IAMGE_TYPE
{
    MV_Image_Undefined                 = 0,
    MV_Image_Bmp                       = 1,
    MV_Image_Jpeg                      = 2,
    MV_Image_Png                       = 3,     //Not support
    MV_Image_Tif                       = 4,     //Not support
};
// Save Image Parameters
typedef struct _MV_SAVE_IMAGE_PARAM_T_
{
    unsigned char*        pData;              // [IN]     Input Data Buffer
    unsigned int          nDataLen;           // [IN]     Input Data Size
    enum MvGvspPixelType       enPixelType;        // [IN]     Input Data Pixel Format
    unsigned short        nWidth;             // [IN]     Image Width
    unsigned short        nHeight;            // [IN]     Image Height

    unsigned char*        pImageBuffer;       // [OUT]    Output Image Buffer
    unsigned int          nImageLen;          // [OUT]    Output Image Size
    unsigned int          nBufferSize;        // [IN]     Output buffer size provided
    enum MV_SAVE_IAMGE_TYPE    enImageType;        // [IN]     Output Image Format

}MV_SAVE_IMAGE_PARAM;

// Save Image Parameters
typedef struct _MV_SAVE_IMAGE_PARAM_T_EX_
{
    unsigned char*      pData;              // [IN]     Input Data Buffer
    unsigned int        nDataLen;           // [IN]     Input Data Size
    enum MvGvspPixelType     enPixelType;        // [IN]     Input Data Pixel Format
    unsigned short      nWidth;             // [IN]     Image Width
    unsigned short      nHeight;            // [IN]     Image Height

    unsigned char*      pImageBuffer;       // [OUT]    Output Image Buffer
    unsigned int        nImageLen;          // [OUT]    Output Image Size
    unsigned int        nBufferSize;        // [IN]     Output buffer size provided
    enum MV_SAVE_IAMGE_TYPE  enImageType;        // [IN]     Output Image Format
    unsigned int        nJpgQuality;        // [IN]     Encoding quality, (50-99]
	
    // [IN]     Interpolation method of convert Bayer to RGB24  0-nearest neighbour 1-bilinearity 2-Hamilton
    unsigned int        iMethodValue;
    unsigned int        nReserved[3];

}MV_SAVE_IMAGE_PARAM_EX;

// Pixel convert structure
typedef struct _MV_PIXEL_CONVERT_PARAM_T_
{
    unsigned short      nWidth;             // [IN]     Width
    unsigned short      nHeight;            // [IN]     Height

    enum MvGvspPixelType    enSrcPixelType;     // [IN]     Source pixel format
    unsigned char*      pSrcData;           // [IN]     Input data buffer
    unsigned int        nSrcDataLen;        // [IN]     Input data size

    enum MvGvspPixelType    enDstPixelType;     // [IN]     Destination pixel format
    unsigned char*      pDstBuffer;         // [OUT]    Output data buffer
    unsigned int        nDstLen;            // [OUT]    Output data size
    unsigned int        nDstBufferSize;     // [IN]     Provided outbut buffer size
    unsigned int        nRes[4];
}MV_CC_PIXEL_CONVERT_PARAM;



// Acquisition mode
typedef enum _MV_CAM_ACQUISITION_MODE_
{
    MV_ACQ_MODE_SINGLE      = 0,            // Single Mode
    MV_ACQ_MODE_MUTLI       = 1,            // Multi Mode
    MV_ACQ_MODE_CONTINUOUS  = 2,            // Continuous Mode

}MV_CAM_ACQUISITION_MODE;

// Gain Mode
typedef enum _MV_CAM_GAIN_MODE_
{
    MV_GAIN_MODE_OFF        = 0,            // Off
    MV_GAIN_MODE_ONCE       = 1,            // Once
    MV_GAIN_MODE_CONTINUOUS = 2,            // Continuous

}MV_CAM_GAIN_MODE;

// Exposure Mode
typedef enum _MV_CAM_EXPOSURE_MODE_
{
    MV_EXPOSURE_MODE_TIMED          = 0,            // Timed
    MV_EXPOSURE_MODE_TRIGGER_WIDTH  = 1,            // TriggerWidth
}MV_CAM_EXPOSURE_MODE;

// Auto Exposure Mode
typedef enum _MV_CAM_EXPOSURE_AUTO_MODE_
{
    MV_EXPOSURE_AUTO_MODE_OFF        = 0,            // Off
    MV_EXPOSURE_AUTO_MODE_ONCE       = 1,            // Once
    MV_EXPOSURE_AUTO_MODE_CONTINUOUS = 2,            // Continuous

}MV_CAM_EXPOSURE_AUTO_MODE;

typedef enum _MV_CAM_TRIGGER_MODE_
{
    MV_TRIGGER_MODE_OFF         = 0,            // Off
    MV_TRIGGER_MODE_ON          = 1,            // ON

}MV_CAM_TRIGGER_MODE;

typedef enum _MV_CAM_GAMMA_SELECTOR_
{
    MV_GAMMA_SELECTOR_USER      = 1,
    MV_GAMMA_SELECTOR_SRGB      = 2,

}MV_CAM_GAMMA_SELECTOR;

typedef enum _MV_CAM_BALANCEWHITE_AUTO_
{
    MV_BALANCEWHITE_AUTO_OFF            = 0,
    MV_BALANCEWHITE_AUTO_ONCE           = 2,
    MV_BALANCEWHITE_AUTO_CONTINUOUS     = 1,            // Continuous

}MV_CAM_BALANCEWHITE_AUTO;

typedef enum _MV_CAM_TRIGGER_SOURCE_
{
    MV_TRIGGER_SOURCE_LINE0             = 0,
    MV_TRIGGER_SOURCE_LINE1             = 1,
    MV_TRIGGER_SOURCE_LINE2             = 2,
    MV_TRIGGER_SOURCE_LINE3             = 3,
    MV_TRIGGER_SOURCE_COUNTER0          = 4,

    MV_TRIGGER_SOURCE_SOFTWARE          = 7,
    MV_TRIGGER_SOURCE_FrequencyConverter= 8,

}MV_CAM_TRIGGER_SOURCE;


// GigEVision IP Configuration
#define MV_IP_CFG_STATIC        0x05000000
#define MV_IP_CFG_DHCP          0x06000000
#define MV_IP_CFG_LLA           0x04000000

// GigEVision Net Transfer Mode
#define MV_NET_TRANS_DRIVER     0x00000001
#define MV_NET_TRANS_SOCKET     0x00000002


// Information Type
#define MV_MATCH_TYPE_NET_DETECT             0x00000001      // Network traffic and packet loss information
#define MV_MATCH_TYPE_USB_DETECT             0x00000002      // The total number of bytes host received from U3V device


// The maximum number of child nodes corresponding to a node
#define MV_MAX_XML_NODE_NUM_C       128

// The maximum length of node name string
#define MV_MAX_XML_NODE_STRLEN_C    64

// The maximum length of Node String
#define MV_MAX_XML_STRVALUE_STRLEN_C 64

// The maximum length of the node description field
#define MV_MAX_XML_DISC_STRLEN_C    512

// The maximum number of units
#define MV_MAX_XML_ENTRY_NUM        10

// The maximum number of parent nodes
#define MV_MAX_XML_PARENTS_NUM      8

// The length of the name of each unit that has been implemented
#define MV_MAX_XML_SYMBOLIC_STRLEN_C 64

#define MV_MAX_XML_SYMBOLIC_NUM      64

// The default maximum number of retransmission packets

// A fully matched information structure
typedef struct _MV_ALL_MATCH_INFO_
{
    unsigned int    nType;          // Information type need to output ,e.g. MV_MATCH_TYPE_NET_DETECT
    void*           pInfo;          // Output information cache, which is allocated by the caller
    unsigned int    nInfoSize;      // Information cache size

}MV_ALL_MATCH_INFO;

// Network traffic and packet loss feedback structure, the corresponding type is MV_MATCH_TYPE_NET_DETECT
typedef struct _MV_MATCH_INFO_NET_DETECT_
{
    int64_t         nReviceDataSize;    // Received data size  [Counts the amount of data between StartGrabbing and StopGrabbing]
    int64_t         nLostPacketCount;   // Number of packets lost
    unsigned int    nLostFrameCount;    // Number of frames lost
    unsigned int    nReserved[5];          // Reserved
}MV_MATCH_INFO_NET_DETECT;

// The total number of bytes host received from the u3v device side, the corresponding type is MV_MATCH_TYPE_USB_DETECT
typedef struct _MV_MATCH_INFO_USB_DETECT_
{
    int64_t         nReviceDataSize;      // Received data size    [Counts the amount of data between OpenDevice and CloseDevice]
    unsigned int    nRevicedFrameCount;   // Number of frames received
    unsigned int    nErrorFrameCount;     // Number of error frames
    unsigned int    nReserved[2];         // Reserved
}MV_MATCH_INFO_USB_DETECT;

typedef struct _MV_IMAGE_BASIC_INFO_
{
    // width
    unsigned short      nWidthValue;
    unsigned short      nWidthMin;
    unsigned int        nWidthMax;
    unsigned int        nWidthInc;

    // height
    unsigned int        nHeightValue;
    unsigned int        nHeightMin;
    unsigned int        nHeightMax;
    unsigned int        nHeightInc;

    // framerate
    float               fFrameRateValue;
    float               fFrameRateMin;
    float               fFrameRateMax;

    // pixel format
    unsigned int        enPixelType;                // Current pixel format
    unsigned int        nSupportedPixelFmtNum;      // Support pixel format
    unsigned int        enPixelList[MV_MAX_XML_SYMBOLIC_NUM];
    unsigned int        nReserved[8];

}MV_IMAGE_BASIC_INFO;

//  Exception message type
#define MV_EXCEPTION_DEV_DISCONNECT     0x00008001      // The device is disconnected
#define MV_EXCEPTION_VERSION_CHECK      0x00008002      // SDK does not match the driver version


// Device Access Mode
// Exclusive authority, other APP is only allowed to read the CCP register
#define MV_ACCESS_Exclusive                                         1
// You can seize the authority from the 5 mode, and then open with exclusive authority
#define MV_ACCESS_ExclusiveWithSwitch                               2
// Control authority, allows other APP reading all registers
#define MV_ACCESS_Control                                           3
// You can seize the authority from the 5 mode, and then open with control authority
#define MV_ACCESS_ControlWithSwitch                                 4
// Open with seized control authority
#define MV_ACCESS_ControlSwitchEnable                               5
// You can seize the authority from the 5 mode, and then open with seized control authority
#define MV_ACCESS_ControlSwitchEnableWithKey                        6
// Open with read mode and is available under control authority
#define MV_ACCESS_Monitor                                           7


/************************************************************************/
/* Package of GenICam C interface-related parameters definition         */
/************************************************************************/

// Interface type corresponds to each node 
enum MV_XML_InterfaceType
{
    IFT_IValue,         //!> IValue interface
    IFT_IBase,          //!> IBase interface
    IFT_IInteger,       //!> IInteger interface
    IFT_IBoolean,       //!> IBoolean interface
    IFT_ICommand,       //!> ICommand interface
    IFT_IFloat,         //!> IFloat interface
    IFT_IString,        //!> IString interface
    IFT_IRegister,      //!> IRegister interface
    IFT_ICategory,      //!> ICategory interface
    IFT_IEnumeration,   //!> IEnumeration interface
    IFT_IEnumEntry,     //!> IEnumEntry interface
    IFT_IPort,          //!> IPort interface
};

// Node Access Mode
enum MV_XML_AccessMode
{
    AM_NI,          //!< Not implemented
    AM_NA,          //!< Not available
    AM_WO,          //!< Write Only
    AM_RO,          //!< Read Only
    AM_RW,          //!< Read and Write
    AM_Undefined,   //!< Object is not yet initialized
    AM_CycleDetect, //!< used internally for AccessMode cycle detection

};

enum MV_XML_Visibility
{
    V_Beginner      = 0,    //!< Always visible
    V_Expert        = 1,    //!< Visible for experts or Gurus
    V_Guru          = 2,    //!< Visible for Gurus
    V_Invisible     = 3,    //!< Not Visible
    V_Undefined     = 99    //!< Object is not yet initialized
};

typedef enum _MV_GIGE_EVENT_
{
    MV_EVENT_ExposureEnd                 = 1,           // Exposure ends of each frame
    MV_EVENT_FrameStartOvertrigger       = 2,           // Frame trigger start 
    MV_EVENT_AcquisitionStartOvertrigger = 3,           // Acquisition start over trigger
    MV_EVENT_FrameStart                  = 4,           // Start of each frame
    MV_EVENT_AcquisitionStart            = 5,           // Acquisition start ( Continuous or Single Frame Mode)
    MV_EVENT_EventOverrun                = 6            // Event over trigger
}MV_GIGE_EVENT;


//

// Single Node Basic Attributes
typedef struct _MV_XML_NODE_FEATURE_
{
    enum MV_XML_InterfaceType    enType;                             // Node Type
    enum MV_XML_Visibility       enVisivility;                       // Visible
    char                    strDescription[MV_MAX_XML_DISC_STRLEN_C];// Node Description, NOT SUPPORT NOW
    char                    strDisplayName[MV_MAX_XML_NODE_STRLEN_C];// Display Name
    char                    strName[MV_MAX_XML_NODE_STRLEN_C];  // Node Name
    char                    strToolTip[MV_MAX_XML_DISC_STRLEN_C];  // Notice

    unsigned int            nReserved[4];
}MV_XML_NODE_FEATURE;

// Node List
typedef struct _MV_XML_NODES_LIST_
{
    unsigned int            nNodeNum;       // Node Number
    MV_XML_NODE_FEATURE     stNodes[MV_MAX_XML_NODE_NUM_C];
}MV_XML_NODES_LIST;



typedef struct _MV_XML_FEATURE_Value_
{
    enum MV_XML_InterfaceType    enType;                             // Node Type
    char                    strDescription[MV_MAX_XML_DISC_STRLEN_C];// Node Description, NOT SUPPORT NOW
    char                    strDisplayName[MV_MAX_XML_NODE_STRLEN_C];// Display Name
    char                    strName[MV_MAX_XML_NODE_STRLEN_C];  // Node Name
    char                    strToolTip[MV_MAX_XML_DISC_STRLEN_C];  //Notice
    unsigned int            nReserved[4];
}MV_XML_FEATURE_Value;

typedef struct _MV_XML_FEATURE_Base_
{
    enum MV_XML_AccessMode   enAccessMode;       // Access Mode
}MV_XML_FEATURE_Base;

typedef struct _MV_XML_FEATURE_Integer_
{
    char                strName[MV_MAX_XML_NODE_STRLEN_C];
    char                strDisplayName[MV_MAX_XML_NODE_STRLEN_C];
    char                strDescription[MV_MAX_XML_DISC_STRLEN_C];   // NOT SUPPORT NOW
    char                strToolTip[MV_MAX_XML_DISC_STRLEN_C];

    enum MV_XML_Visibility   enVisivility;                       // Visible
    enum MV_XML_AccessMode   enAccessMode;       // Access Mode
    int                 bIsLocked;          // Locked. 0-NO; 1-YES, NOT SUPPORT NOW
    int64_t             nValue;             // Current Value
    int64_t             nMinValue;          // Min Value
    int64_t             nMaxValue;          // Max Value
    int64_t             nIncrement;         // Increment

    unsigned int        nReserved[4];

}MV_XML_FEATURE_Integer;

typedef struct _MV_XML_FEATURE_Boolean_
{
    char                strName[MV_MAX_XML_NODE_STRLEN_C];
    char                strDisplayName[MV_MAX_XML_NODE_STRLEN_C];
    char                strDescription[MV_MAX_XML_DISC_STRLEN_C];   // NOT SUPPORT NOW
    char                strToolTip[MV_MAX_XML_DISC_STRLEN_C];

    enum MV_XML_Visibility   enVisivility;                       //Visible
    enum MV_XML_AccessMode   enAccessMode;       // Access Mode
    int                 bIsLocked;          // Locked. 0-NO; 1-YES, NOT SUPPORT NOW
    bool                bValue;             // Current Value

    unsigned int        nReserved[4];
}MV_XML_FEATURE_Boolean;

typedef struct _MV_XML_FEATURE_Command_
{
    char                strName[MV_MAX_XML_NODE_STRLEN_C];
    char                strDisplayName[MV_MAX_XML_NODE_STRLEN_C];
    char                strDescription[MV_MAX_XML_DISC_STRLEN_C];   // NOT SUPPORT NOW
    char                strToolTip[MV_MAX_XML_DISC_STRLEN_C];

    enum MV_XML_Visibility   enVisivility;                       //Visible
    enum MV_XML_AccessMode   enAccessMode;       // Access Mode
    int                 bIsLocked;          // Locked. 0-NO; 1-YES, NOT SUPPORT NOW

    unsigned int        nReserved[4];
}MV_XML_FEATURE_Command;

typedef struct _MV_XML_FEATURE_Float_
{
    char                strName[MV_MAX_XML_NODE_STRLEN_C];
    char                strDisplayName[MV_MAX_XML_NODE_STRLEN_C];
    char                strDescription[MV_MAX_XML_DISC_STRLEN_C];   // NOT SUPPORT NOW
    char                strToolTip[MV_MAX_XML_DISC_STRLEN_C];

    enum MV_XML_Visibility       enVisivility;                       //Visible
    enum MV_XML_AccessMode   enAccessMode;       // Access Mode
    int                 bIsLocked;          // Locked. 0-NO; 1-YES, NOT SUPPORT NOW
    double              dfValue;             // Current Value
    double              dfMinValue;          // Min Value
    double              dfMaxValue;          // Max Value
    double              dfIncrement;         // Increment

    unsigned int        nReserved[4];
}MV_XML_FEATURE_Float;

typedef struct _MV_XML_FEATURE_String_
{
    char                strName[MV_MAX_XML_NODE_STRLEN_C];
    char                strDisplayName[MV_MAX_XML_NODE_STRLEN_C];
    char                strDescription[MV_MAX_XML_DISC_STRLEN_C];   // NOT SUPPORT NOW
    char                strToolTip[MV_MAX_XML_DISC_STRLEN_C];

    enum MV_XML_Visibility       enVisivility;                       //Visible
    enum MV_XML_AccessMode   enAccessMode;       // Access Mode
    int                 bIsLocked;          // Locked. 0-NO; 1-YES, NOT SUPPORT NOW
    char                strValue[MV_MAX_XML_STRVALUE_STRLEN_C];// Current Value

    unsigned int        nReserved[4];
}MV_XML_FEATURE_String;

typedef struct _MV_XML_FEATURE_Register_
{
    char                strName[MV_MAX_XML_NODE_STRLEN_C];
    char                strDisplayName[MV_MAX_XML_NODE_STRLEN_C];
    char                strDescription[MV_MAX_XML_DISC_STRLEN_C];   // NOT SUPPORT NOW
    char                strToolTip[MV_MAX_XML_DISC_STRLEN_C];

    enum MV_XML_Visibility       enVisivility;                       //Visible
    enum MV_XML_AccessMode   enAccessMode;       // Access Mode
    int                 bIsLocked;          // Locked. 0-NO; 1-YES, NOT SUPPORT NOW
    int64_t             nAddrValue;             // Current Value

    unsigned int        nReserved[4];
}MV_XML_FEATURE_Register;

typedef struct _MV_XML_FEATURE_Category_
{
    char                    strDescription[MV_MAX_XML_DISC_STRLEN_C];//Node Description, NOT SUPPORT NOW
    char                    strDisplayName[MV_MAX_XML_NODE_STRLEN_C];//Display Name
    char                    strName[MV_MAX_XML_NODE_STRLEN_C];  // Node Name
    char                    strToolTip[MV_MAX_XML_DISC_STRLEN_C];  //Notice

    enum MV_XML_Visibility       enVisivility;                       // Visible

    unsigned int            nReserved[4];
}MV_XML_FEATURE_Category;

typedef struct _MV_XML_FEATURE_EnumEntry_
{
    char                strName[MV_MAX_XML_NODE_STRLEN_C];
    char                strDisplayName[MV_MAX_XML_NODE_STRLEN_C];
    char                strDescription[MV_MAX_XML_DISC_STRLEN_C];   // NOT SUPPORT NOW
    char                strToolTip[MV_MAX_XML_DISC_STRLEN_C];
    int                 bIsImplemented;
    int                 nParentsNum;
    MV_XML_NODE_FEATURE stParentsList[MV_MAX_XML_PARENTS_NUM];

    enum MV_XML_Visibility       enVisivility;                       // Visible
    int64_t             nValue;             // Current Value
    enum MV_XML_AccessMode   enAccessMode;       // Access Mode
    int                 bIsLocked;          // Locked. 0-NO; 1-YES, NOT SUPPORT NOW
    int                 nReserved[8];

}MV_XML_FEATURE_EnumEntry;


typedef struct _MV_XML_FEATURE_Enumeration_
{
    enum MV_XML_Visibility       enVisivility;                       // Visible
    char                    strDescription[MV_MAX_XML_DISC_STRLEN_C];// Node Description, NOT SUPPORT NOW
    char                    strDisplayName[MV_MAX_XML_NODE_STRLEN_C];// Display Name
    char                    strName[MV_MAX_XML_NODE_STRLEN_C];  // Node Name
    char                    strToolTip[MV_MAX_XML_DISC_STRLEN_C];  // Notice

    int                 nSymbolicNum;          // Symbolic Number
    char                strCurrentSymbolic[MV_MAX_XML_SYMBOLIC_STRLEN_C];          // Current Symbolic Index
    char                strSymbolic[MV_MAX_XML_SYMBOLIC_NUM][MV_MAX_XML_SYMBOLIC_STRLEN_C];
    enum MV_XML_AccessMode   enAccessMode;       // Access Mode
    int                 bIsLocked;          // Locked. 0-NO; 1-YES, NOT SUPPORT NOW
    int64_t             nValue;             // Current Value

    unsigned int        nReserved[4];
}MV_XML_FEATURE_Enumeration;


typedef struct _MV_XML_FEATURE_Port_
{
    enum MV_XML_Visibility       enVisivility;                       // Visible
    char                    strDescription[MV_MAX_XML_DISC_STRLEN_C];// Node Description, NOT SUPPORT NOW
    char                    strDisplayName[MV_MAX_XML_NODE_STRLEN_C];// Display Name
    char                    strName[MV_MAX_XML_NODE_STRLEN_C];  // Node Name
    char                    strToolTip[MV_MAX_XML_DISC_STRLEN_C];  // Notice

    enum MV_XML_AccessMode       enAccessMode;       // Access Mode
    int                     bIsLocked;          // Locked. 0-NO; 1-YES, NOT SUPPORT NOW

    unsigned int            nReserved[4];
}MV_XML_FEATURE_Port;



typedef struct _MV_XML_CAMERA_FEATURE_
{
    enum MV_XML_InterfaceType    enType;
    union
    {
        MV_XML_FEATURE_Integer      stIntegerFeature;
        MV_XML_FEATURE_Float        stFloatFeature;
        MV_XML_FEATURE_Enumeration  stEnumerationFeature;
        MV_XML_FEATURE_String       stStringFeature;
    }SpecialFeature;

}MV_XML_CAMERA_FEATURE;


typedef struct _MVCC_ENUMVALUE_T
{
    unsigned int    nCurValue;      // Current Value
    unsigned int    nSupportedNum;  // Number of valid data
    unsigned int    nSupportValue[MV_MAX_XML_SYMBOLIC_NUM];

    unsigned int    nReserved[4];
}MVCC_ENUMVALUE;

typedef struct _MVCC_INTVALUE_T
{
    unsigned int    nCurValue;      // Current Value
    unsigned int    nMax;
    unsigned int    nMin;
    unsigned int    nInc;

    unsigned int    nReserved[4];
}MVCC_INTVALUE;

typedef struct _MVCC_FLOATVALUE_T
{
    float           fCurValue;      // Current Value
    float           fMax;
    float           fMin;

    unsigned int    nReserved[4];
}MVCC_FLOATVALUE;

typedef struct _MVCC_STRINGVALUE_T
{
    char   chCurValue[256];      // Current Value

    unsigned int    nReserved[4];
}MVCC_STRINGVALUE;

#endif /* _MV_CAMERA_PARAMS_H_ */
