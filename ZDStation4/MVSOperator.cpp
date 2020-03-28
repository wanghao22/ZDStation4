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


/********************* �����������******************************/

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
		MessageBox(NULL, TEXT("Error In WorkThread, NULL == pCam"), TEXT("����"), MB_OK | MB_ICONWARNING);
		return -1;
	}
	int nUsingCameraNum = pCam->m_nUsingCameraNum;
	pCam->m_nUsingCameraNum = -1;
	pCam->ThreadFun(nUsingCameraNum);

	return 0;
}


//ö�����
Status MVSOperator::EnumDevice(void)
{
	int nRet = CMyCamera::EnumDevices(&m_stDevList);
	if (MV_OK != nRet)
	{
		return STATUS_ERROR;
	}

	if (0 == m_stDevList.nDeviceNum)
	{
		MessageBox(NULL,TEXT("��������޷����ӣ������Դ����������!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		nRet = DestroyDevice();
		return STATUS_ERROR;
	}
	/*else if (m_stDevList.nDeviceNum < MAX_DEVICE_NUM)
	{
		MessageBox(NULL,TEXT("��������޷����ӣ������Դ����������!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		nRet = DestroyDevice();
		return STATUS_ERROR;
	}*/

	return STATUS_OK;
}

//�����
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
		//����������к�ȷ�������ţ����������λ�ã�
		string cameraSN = (char*)m_stDevList.pDeviceInfo[j]->SpecialInfo.stGigEInfo.chSerialNumber;
		int cameraNum = GetCameraNumBySN(cameraSN);
		if (cameraNum < 0)
		{
			/*MessageBox(NULL,TEXT("δʶ���������к�!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
			nRet = DestroyDevice();
			return STATUS_ERROR;*/
			continue;
		}

		for (size_t m = 0; m < index.size(); ++m){
			if (cameraNum == index[m]){
				//�����
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
		MessageBox(NULL,TEXT("���δ��ȫ�򿪣�������������!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		nRet = DestroyDevice();     // Re-check whether released buffer  
		return STATUS_ERROR;
	}
	m_nOnLineCameraNum = nCanOpenDeviceNum;
	m_bCreateDevice = TRUE;
	m_bOpenDevice = TRUE;

	return STATUS_OK;
}

//�����������ͼ��
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
			MessageBox(NULL,TEXT("����ͼ���߳�ʧ��!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		}
	}

	return STATUS_OK;
}

//��������
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

//�ر��������
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

//�ر����
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

//�����豸
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

//ץȡͼ���߳�ִ�к���
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
			//����ͼ��
			//EnterCriticalSection(&g_CriticalSection);       // �����ٽ������������Ȩ�������߳̾͵ȴ�
			memcpy(m_imgArr[nUsingDeviceNum].data, pData, nBufferSize);
			m_have_img_flags[nUsingDeviceNum] = true;
			//LeaveCriticalSection(&g_CriticalSection);   // �뿪�ٽ������ͷ�����Ȩ 
		}
	}

	TRACE("�ɼ�ͼ���߳��˳���%d\n", nUsingDeviceNum);
	return STATUS_OK;
}

//�������SN��ȷ��������
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
	//�ر����
	StopGrabbing(index);
	Sleep(2000);
	CloseDevice(index);
	DestroyDevice();
}

bool MVSOperator::Open(vector<int>& index){

	
	//��ʼ���������
	if (!EnumDevice())
	{
		AfxMessageBox(L"�޷��ҵ������");
		return false;
	}
	if (!OpenDevice(index))
	{
		AfxMessageBox(L"�޷������������Ѿ��򿪣�");
		return false;
	}

	if (!SetTriggerMode(index)){
		MessageBox(NULL, TEXT("�������ģʽ����ʧ��!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		return false;
	}
	
	if (!SetTriggerSource(index)){
		MessageBox(NULL, TEXT("����Ӳ����ʧ��"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		return false;
	}

	if (!StartGrabbing(index))
	{
		AfxMessageBox(L"�޷��������������Ѿ�������");
		return false;
	}

	if (!SetExposureTime(index)){
		MessageBox(NULL, TEXT("����ع�����ʧ��!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		return false;
	}
	if (!SetGain(index)){
		//MessageBox(NULL, TEXT("�����������ʧ��!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
		return false;
	}
	return true;
}

/*���á���ȡ��������*/
// ���ô���ģʽ
Status MVSOperator::SetTriggerMode(vector<int> index){

	for (size_t i = 0; i < m_nTriggerMode.size(); ++i){
		if (0 != index.size() && i == index[0]){
			int nRet = m_pcMyCamera[i]->SetEnumValue("TriggerMode", m_nTriggerMode[i]);
			if (MV_OK != nRet)
			{
				//MessageBox(NULL, TEXT("�������ģʽ����ʧ��!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
				return STATUS_ERROR;
			}
			vector<int>::iterator   iter = index.begin() ;
			//ɾ����һ��Ԫ��
			index.erase(iter);
		}	
	}
	return STATUS_OK;
}

//�����ع�ʱ��
Status MVSOperator::SetExposureTime(vector<int> index){

	for (size_t i = 0; i < m_dExposureEdit.size(); ++i){
		if (0 != index.size() && i == index[0]){

			/*�����������ع�ģʽ���������ع�ʱ����Ч*/
			int nRet = m_pcMyCamera[i]->SetEnumValue("ExposureMode", MV_EXPOSURE_MODE_TIMED);
			if (MV_OK != nRet)
			{
				//MessageBox(NULL, TEXT("����ع�����ʧ��!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
				return STATUS_ERROR;
			}

			nRet = m_pcMyCamera[i]->SetEnumValue("ExposureAuto", MV_EXPOSURE_AUTO_MODE_OFF);
			if (MV_OK != nRet)
			{
				// return STATUS_ERROR;      // ����ʧ���п����ǿ��˴���ģʽ���������践��
			}

			nRet = m_pcMyCamera[i]->SetFloatValue("ExposureTime", m_dExposureEdit[i]);
			if (MV_OK != nRet)
			{
				//MessageBox(NULL, TEXT("����ع�����ʧ��!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
				return STATUS_ERROR;
			}

			vector<int>::iterator   iter = index.begin();
			//ɾ����һ��Ԫ��
			index.erase(iter);
		}
		
	}
	return STATUS_OK;
}

//��������
Status MVSOperator::SetGain(vector<int> index){
	
	for (size_t i = 0; i < m_dGainEdit.size(); ++i){
		if (0 != index.size() && i == index[0]){

			int nRet = m_pcMyCamera[i]->SetEnumValue("GainAuto", 0);   // ��������ǰ�Ȱ��Զ�����رգ�ʧ�����践��

			nRet = m_pcMyCamera[i]->SetFloatValue("Gain", m_dGainEdit[i]);
			if (MV_OK != nRet)
			{
				//MessageBox(NULL, TEXT("�����������ʧ��!"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
				return STATUS_ERROR;
			}

			vector<int>::iterator   iter = index.begin();
			//ɾ����һ��Ԫ��
			index.erase(iter);
		}
	}

	return STATUS_OK;
}

//���ô���Դ
Status MVSOperator::SetTriggerSource(vector<int> index){
	
	for (size_t i = 0; i < m_nTriggerSource.size(); ++i){
		if (0 != index.size() && i == index[0]){

			int nRet = m_pcMyCamera[i]->SetEnumValue("TriggerSource", m_nTriggerSource[i]);
			if (MV_OK != nRet)
			{
				//MessageBox(NULL, TEXT("����Ӳ����ʧ��"), TEXT("Warn!"), MB_OK | MB_ICONWARNING);
				return STATUS_ERROR;
			}
			vector<int>::iterator   iter = index.begin();
			//ɾ����һ��Ԫ��
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