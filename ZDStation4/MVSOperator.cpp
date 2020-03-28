#include "stdafx.h"
#include "MVSOperator.h"

using namespace std;
using namespace cv;

CRITICAL_SECTION g_CriticalSection;	

MVSOperator::MVSOperator()
{
	m_bSystemState = true;
	m_nOnLineCameraNum = 0;
	m_nUsingCameraNum = 0;
	m_bOpenDevice = false;
	m_bRetStatus = false;
	m_bCreateDevice = false;
	m_bOpenDevice = false;
	m_bStartGrabbing = false;
	m_captureImgCnt = 0;
	m_bStartCaptureImg = false;
	m_nCaptureImgCostTime = 0;
	for (int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		m_pcMyCamera[i] = NULL;
		m_hGetOneFrameHandle[i] = NULL;
		m_bCaptureImg[i] = false;
		m_have_img_flags[i] = false;
		m_imgArr.push_back(Mat::zeros(IMG_HEIGHT, IMG_WIDTH, IMG_CHANNEL));
	}
	snArr.push_back(CAMERA_SN1);
}


MVSOperator::~MVSOperator()
{
}


/********************* 相机操作函数******************************/

/*Working thread*/
unsigned int    __stdcall   WorkThread(void* pUser)
{
	if (NULL == pUser)
	{
		return -1;
	}

	MVSOperator* pCam = (MVSOperator*)pUser;
	if (NULL == pCam)
	{
		MessageBox(NULL, TEXT("Error In WorkThread, NULL == pCam"), TEXT("警告"), MB_OK | MB_ICONWARNING);
		return -1;
	}
	int nUsingCameraNum = pCam->m_nUsingCameraNum;
	pCam->m_nUsingCameraNum = -1;
	pCam->ThreadFun(nUsingCameraNum);

	return 0;
}


//枚举相机
Status MVSOperator::EnumDevice(void)
{
	int nRet = CMyCamera::EnumDevices(&m_stDevList);
	if (MV_OK != nRet)
	{
		return STATUS_ERROR;
	}

	if (0 == m_stDevList.nDeviceNum)
	{
		MessageBox(NULL,TEXT("所有相机无法连接，请检查电源和网络连接!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		nRet = DestroyDevice();
		return STATUS_ERROR;
	}
	/*else if (m_stDevList.nDeviceNum < MAX_DEVICE_NUM)
	{
		MessageBox(NULL,TEXT("部分相机无法连接，请检查电源和网络连接!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		nRet = DestroyDevice();
		return STATUS_ERROR;
	}*/

	return STATUS_OK;
}

//打开相机
Status MVSOperator::OpenDevice(vector<int>& index)
{
	if (TRUE == m_bOpenDevice)
	{
		return STATUS_ERROR;
	}

	int nRet = MV_OK;
	int nCanOpenDeviceNum = 0;

	for (unsigned int j = 0; j < m_stDevList.nDeviceNum; j++)
	{
		//根据相机序列号确定相机编号（即相机所在位置）
		string cameraSN = (char*)m_stDevList.pDeviceInfo[j]->SpecialInfo.stGigEInfo.chSerialNumber;
		int cameraNum = GetCameraNumBySN(cameraSN);
		if (cameraNum < 0)
		{
			/*MessageBox(NULL,TEXT("未识别的相机序列号!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
			nRet = DestroyDevice();
			return STATUS_ERROR;*/
			continue;
		}

		for (size_t m = 0; m < index.size(); ++m){
			if (cameraNum == index[m]){
				//打开相机
				m_pcMyCamera[cameraNum] = new CMyCamera;
				m_pcMyCamera[cameraNum]->m_pBufForDriver = NULL;
				m_pcMyCamera[cameraNum]->m_pBufForSaveImage = NULL;
				m_pcMyCamera[cameraNum]->m_nBufSizeForDriver = 0;
				m_pcMyCamera[cameraNum]->m_nBufSizeForSaveImage = 0;
				nRet = m_pcMyCamera[cameraNum]->Open(m_stDevList.pDeviceInfo[j]);
				if (MV_OK != nRet)
				{
					delete(m_pcMyCamera[cameraNum]);
					m_pcMyCamera[cameraNum] = NULL;
				}
				else
				{
					nCanOpenDeviceNum++;
				}
				break;
			}
		}
		
	}

	if (nCanOpenDeviceNum < index.size())     // Number of camera can be opened is 0
	{
		MessageBox(NULL,TEXT("相机未完全打开，请检查网络连接!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		nRet = DestroyDevice();     // Re-check whether released buffer  
		return STATUS_ERROR;
	}
	m_nOnLineCameraNum = nCanOpenDeviceNum;
	m_bCreateDevice = TRUE;
	m_bOpenDevice = TRUE;

	return STATUS_OK;
}

//开启相机捕获图像
Status MVSOperator::StartGrabbing(vector<int>& index)
{
	if (FALSE == m_bOpenDevice || TRUE == m_bStartGrabbing)
	{
		return STATUS_ERROR;
	}
	int nRet = MV_OK;
	int i = 0;

	for (i = 0; i < m_nOnLineCameraNum; i++)
	{
		nRet = m_pcMyCamera[index[i]]->StartGrabbing();
		if (MV_OK != nRet)
		{
			return STATUS_ERROR;
		}
		m_bStartGrabbing = TRUE;

		/*Create workthread after start grabbing*/
		unsigned int nThreadID = 0;
		m_nUsingCameraNum = index[i];
		m_hGetOneFrameHandle[index[i]] = (void*)_beginthreadex(NULL, 0, WorkThread, this, 0, &nThreadID);
		if (NULL == m_hGetOneFrameHandle[index[i]])
		{
			MessageBox(NULL,TEXT("创建图像线程失败!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		}
	}

	return STATUS_OK;
}

//软触发拍照
Status MVSOperator::DoSoftwareOnce(vector<int>& index)
{
	if (FALSE == m_bOpenDevice)
	{
		return STATUS_ERROR;
	}
	int nRet = MV_OK;
	int i = 0;

	for (i = 0; i < m_nOnLineCameraNum; i++)
	{
		nRet = m_pcMyCamera[index[i]]->CommandExecute("TriggerSoftware");
	}

	return STATUS_OK;
}

Status MVSOperator::DoSoftwareOnce(int index)
{
	if (FALSE == m_bOpenDevice)
	{
		return STATUS_ERROR;
	}
	int nRet = MV_OK;
	nRet = m_pcMyCamera[index]->CommandExecute("TriggerSoftware");

	return STATUS_OK;
}

//关闭相机捕获
Status MVSOperator::StopGrabbing(vector<int>& index)
{
	if (FALSE == m_bOpenDevice || FALSE == m_bStartGrabbing)
	{
		return STATUS_ERROR;
	}
	m_bRetStatus = STATUS_OK;
	int i = 0;
	int nRet = MV_OK;
	for (i = 0; i < m_nUsingCameraNum; i++)
	{
		nRet = m_pcMyCamera[index[i]]->StopGrabbing();
		if (MV_OK != nRet)
		{
			m_bRetStatus = STATUS_ERROR;
		}
	}

	m_bStartGrabbing = FALSE;
	for (i = 0; i < m_nUsingCameraNum; i++)
	{
		if (m_hGetOneFrameHandle[index[i]])
		{
			CloseHandle(m_hGetOneFrameHandle[index[i]]);
			m_hGetOneFrameHandle[index[i]] = NULL;
		}
	}
	return m_bRetStatus;
}

//关闭相机
Status MVSOperator::CloseDevice(vector<int>& index)
{
	int i = 0;
	int nRet = MV_OK;
	m_bRetStatus = STATUS_OK;
	for (i = 0; i < m_nUsingCameraNum; i++)
	{
		nRet = m_pcMyCamera[index[i]]->Close();
		if (MV_OK != nRet)
		{
			m_bRetStatus = STATUS_ERROR;
		}
	}
	m_bStartGrabbing = FALSE;
	m_bOpenDevice = FALSE;
	return m_bRetStatus;
}

//销毁设备
Status MVSOperator::DestroyDevice(void)
{
	for (int i = 0; i < MAX_DEVICE_NUM; i++)
	{
		if (m_hGetOneFrameHandle[i])
		{
			CloseHandle(m_hGetOneFrameHandle[i]);
			m_hGetOneFrameHandle[i] = NULL;
		}

		if (m_pcMyCamera[i] && m_pcMyCamera[i]->m_pBufForDriver)
		{
			free(m_pcMyCamera[i]->m_pBufForDriver);
			m_pcMyCamera[i]->m_pBufForDriver = NULL;
		}

		if (m_pcMyCamera[i] && m_pcMyCamera[i]->m_pBufForSaveImage)
		{
			free(m_pcMyCamera[i]->m_pBufForSaveImage);
			m_pcMyCamera[i]->m_pBufForSaveImage = NULL;
		}

		if (m_pcMyCamera[i])
		{
			delete(m_pcMyCamera[i]);
			m_pcMyCamera[i] = NULL;
		}
	}

	m_bCreateDevice = FALSE;
	return STATUS_OK;
}

//抓取图像线程执行函数
Status MVSOperator::ThreadFun(int nUsingDeviceNum)
{
	int nRet = 0;

	MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
	memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
	unsigned int nBufferSize = 0;
	unsigned char * pData = NULL;
	unsigned int nDataLen = 0;

	nBufferSize = BUF_SIZE;

	pData = (unsigned char *)malloc(sizeof(unsigned char) * nBufferSize);

	// Get camera net flow and lsot packet information
	MV_ALL_MATCH_INFO struMatchInfo = { 0 };
	MV_MATCH_INFO_NET_DETECT stMatchInfoNetDetect;
	struMatchInfo.pInfo = &stMatchInfoNetDetect;

	struMatchInfo.nType = MV_MATCH_TYPE_NET_DETECT; // Net flow and lsot packet information
	memset(struMatchInfo.pInfo, 0, sizeof(MV_MATCH_INFO_NET_DETECT));
	struMatchInfo.nInfoSize = sizeof(MV_MATCH_INFO_NET_DETECT);

	while (1)
	{
		Sleep(50);
		if (FALSE == m_bStartGrabbing)
		{
			free(pData);
			break;
		}
		nRet = m_pcMyCamera[nUsingDeviceNum]->GetAllMatchInfo(&struMatchInfo);
		if (MV_OK != nRet)
		{
			memset(&stMatchInfoNetDetect, 0, sizeof(MV_MATCH_INFO_NET_DETECT));
		}
		nRet = m_pcMyCamera[nUsingDeviceNum]->GetOneFrameTimeout(pData, &nDataLen, nBufferSize, &stImageInfo, 150);
		if (MV_OK == nRet)
		{
			//拷贝图像
			//EnterCriticalSection(&g_CriticalSection);       // 进入临界区，获得所有权，其他线程就等待
			memcpy(m_imgArr[nUsingDeviceNum].data, pData, nBufferSize);
			m_have_img_flags[nUsingDeviceNum] = true;
			//LeaveCriticalSection(&g_CriticalSection);   // 离开临界区，释放所有权 
		}
	}

	TRACE("采集图像线程退出：%d\n", nUsingDeviceNum);
	return STATUS_OK;
}

//根据相机SN号确定相机编号
int MVSOperator::GetCameraNumBySN(string cameraSN)
{
	int cameraNum = -1;

	for (int i = 0; i < MAX_DEVICE_NUM; ++i)
	{
		if (snArr[i] == cameraSN)
		{
			cameraNum = i;
			break;
		}
	}

	return cameraNum;
}

bool MVSOperator::GetMatPic(Mat &img, int station){

	if (m_have_img_flags[station] && NULL != m_imgArr[station].data)
	{
		img = m_imgArr[station].clone();
		//cvtColor(m_imgArr[station], img, CV_RGB2BGR);
		m_have_img_flags[station] = false;
		return true;
	}
	else
		return false;
}

void MVSOperator::Close(vector<int>& index){
	//关闭相机
	StopGrabbing(index);
	Sleep(2000);
	CloseDevice(index);
	DestroyDevice();
}

bool MVSOperator::Open(vector<int>& index){

	
	//初始化并打开相机
	if (!EnumDevice())
	{
		AfxMessageBox(L"无法找到相机！");
		return false;
	}
	if (!OpenDevice(index))
	{
		AfxMessageBox(L"无法打开相机或相机已经打开！");
		return false;
	}

	if (!SetTriggerMode(index)){
		MessageBox(NULL, TEXT("相机触发模式设置失败!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		return false;
	}
	
	if (!SetTriggerSource(index)){
		MessageBox(NULL, TEXT("设置硬触发失败"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		return false;
	}

	if (!StartGrabbing(index))
	{
		AfxMessageBox(L"无法启动相机或相机已经启动！");
		return false;
	}

	if (!SetExposureTime(index)){
		MessageBox(NULL, TEXT("相机曝光设置失败!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		return false;
	}
	if (!SetGain(index)){
		//MessageBox(NULL, TEXT("相机增益设置失败!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		return false;
	}
	return true;
}

/*设置、获取参数操作*/
// 设置触发模式
Status MVSOperator::SetTriggerMode(vector<int> index){

	for (size_t i = 0; i < m_nTriggerMode.size(); ++i){
		if (0 != index.size() && i == index[0]){
			int nRet = m_pcMyCamera[i]->SetEnumValue("TriggerMode", m_nTriggerMode[i]);
			if (MV_OK != nRet)
			{
				//MessageBox(NULL, TEXT("相机触发模式设置失败!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
				return STATUS_ERROR;
			}
			vector<int>::iterator   iter = index.begin() ;
			//删除第一个元素
			index.erase(iter);
		}	
	}
	return STATUS_OK;
}

//设置曝光时间
Status MVSOperator::SetExposureTime(vector<int> index){

	for (size_t i = 0; i < m_dExposureEdit.size(); ++i){
		if (0 != index.size() && i == index[0]){

			/*调节这两个曝光模式，才能让曝光时间生效*/
			int nRet = m_pcMyCamera[i]->SetEnumValue("ExposureMode", MV_EXPOSURE_MODE_TIMED);
			if (MV_OK != nRet)
			{
				//MessageBox(NULL, TEXT("相机曝光设置失败!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
				return STATUS_ERROR;
			}

			nRet = m_pcMyCamera[i]->SetEnumValue("ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
			if (MV_OK != nRet)
			{
				// return STATUS_ERROR;      // 设置失败有可能是开了触发模式，所以无需返回
			}

			nRet = m_pcMyCamera[i]->SetFloatValue("ExposureTime", m_dExposureEdit[i]);
			if (MV_OK != nRet)
			{
				//MessageBox(NULL, TEXT("相机曝光设置失败!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
				return STATUS_ERROR;
			}

			vector<int>::iterator   iter = index.begin();
			//删除第一个元素
			index.erase(iter);
		}
		
	}
	return STATUS_OK;
}

//设置增益
Status MVSOperator::SetGain(vector<int> index){
	
	for (size_t i = 0; i < m_dGainEdit.size(); ++i){
		if (0 != index.size() && i == index[0]){

			int nRet = m_pcMyCamera[i]->SetEnumValue("GainAuto", 0);   // 设置增益前先把自动增益关闭，失败无需返回

			nRet = m_pcMyCamera[i]->SetFloatValue("Gain", m_dGainEdit[i]);
			if (MV_OK != nRet)
			{
				//MessageBox(NULL, TEXT("相机增益设置失败!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
				return STATUS_ERROR;
			}

			vector<int>::iterator   iter = index.begin();
			//删除第一个元素
			index.erase(iter);
		}
	}

	return STATUS_OK;
}

//设置触发源
Status MVSOperator::SetTriggerSource(vector<int> index){
	
	for (size_t i = 0; i < m_nTriggerSource.size(); ++i){
		if (0 != index.size() && i == index[0]){

			int nRet = m_pcMyCamera[i]->SetEnumValue("TriggerSource", m_nTriggerSource[i]);
			if (MV_OK != nRet)
			{
				//MessageBox(NULL, TEXT("设置硬触发失败"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
				return STATUS_ERROR;
			}
			vector<int>::iterator   iter = index.begin();
			//删除第一个元素
			index.erase(iter);
		}
	}
	return STATUS_OK;
}

bool MVSOperator::isHaveImg(int station)
{
	if (m_bStartGrabbing)
		return m_have_img_flags[station];
	else
		return false;
}