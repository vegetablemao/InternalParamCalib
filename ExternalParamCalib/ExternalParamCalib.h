
// ExternalParamCalib.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CExternalParamCalibApp:
// �йش����ʵ�֣������ ExternalParamCalib.cpp
//

class CExternalParamCalibApp : public CWinApp
{
public:
	CExternalParamCalibApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CExternalParamCalibApp theApp;