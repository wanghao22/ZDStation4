
// ZDStation4.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CZDStation4App: 
// �йش����ʵ�֣������ ZDStation4.cpp
//

class CZDStation4App : public CWinApp
{
public:
	CZDStation4App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CZDStation4App theApp;