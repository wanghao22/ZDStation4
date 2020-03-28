#pragma once

#include "MyCamera.h"
#include <fstream>
#include <vector>
#include <chrono>  
#include "allDefine.h"

typedef int Status;

class MVSOperator
{
public:
	MVSOperator();
	~MVSOperator();

	int m_nOnLineCameraNum; // on line camera quantities
	int m_nUsingCameraNum;  // indicate which camera when create work thread
	CMyCamera*   m_pcMyCamera[MAX_DEVICE_NUM];      // CMyCamera packed normal used interface
	void*  m_hGetOneFrameHandle[MAX_DEVICE_NUM]; // getoneframe thread handle
	BOOL m_bCaptureImgFinished[MAX_DEVICE_NUM];
	MV_CC_DEVICE_INFO_LIST m_stDevList;             // Device information list structure variable, to save device list
	int m_captureImgCnt;
	bool m_bCaptureImg[MAX_DEVICE_NUM];

	//��ʱ����
	bool m_bStartCaptureImg;
	int  m_nCaptureImgCostTime;
	bool m_bSystemState;

	/*Initialization*/
	::Status EnumDevice(void);
	::Status OpenDevice(std::vector<int>& index);                     // Open device

	/*Set parameter*/
	::Status DoSoftwareOnce(std::vector<int>& index);                    // Software trigger once
	::Status DoSoftwareOnce(int index);

	/*Basic function*/
	::Status StartGrabbing(std::vector<int>& index);                     // Start grabbing
	::Status StopGrabbing(std::vector<int>& index);                      // Stop grabbing

	/*Close and destroy*/
	::Status CloseDevice(std::vector<int>& index);                       // Close device
	::Status DestroyDevice(void);                     // Destroy device

	::Status ThreadFun(int nUsingDeviceNum);

	int GetCameraNumBySN(std::string cameraSN);

	
	bool GetMatPic(cv::Mat &img, int station);
	void Close(std::vector<int>& index);
	bool Open(std::vector<int>& index);

	void SetTriggerMode(std::vector<MV_CAM_TRIGGER_MODE>& mode){
		m_nTriggerMode = mode;
	}
	void SetExposureTime(std::vector<float> time){
		m_dExposureEdit = time;
	}
	void SetGain(std::vector<float> gain){
		m_dGainEdit = gain;
	}
	void SetTriggerSource(std::vector<MV_CAM_TRIGGER_SOURCE> source){
		m_nTriggerSource = source;
	}

	::Status SetTriggerMode(std::vector<int> index);                // ���ô���ģʽ
	::Status SetExposureTime(std::vector<int> index);               // �����ع�ʱ��                     
	::Status SetGain(std::vector<int> index);                       // ��������
	::Status SetTriggerSource(std::vector<int> index);              // ���ô���Դ

	bool isHaveImg(int station);
	

	//���������ر����ͺ���
private:
	BOOL  m_bRetStatus;                             // Function return state
	BOOL  m_bCreateDevice;                          // Create
	BOOL  m_bOpenDevice;                            // Open
	BOOL  m_bStartGrabbing;                         // Start grabbing
	std::vector<std::string> snArr;
	std::vector<cv::Mat> m_imgArr;
	bool m_have_img_flags[MAX_DEVICE_NUM];				//ͼ����û�б���ȡ��

	std::vector<MV_CAM_TRIGGER_MODE>   m_nTriggerMode;                       // ����ģʽ
	std::vector<MV_CAM_TRIGGER_SOURCE>   m_nTriggerSource;                     // ����Դ
	std::vector<float>  m_dExposureEdit;                    //�ع�ʱ��
	std::vector<float>  m_dGainEdit;                        // ����   
};

