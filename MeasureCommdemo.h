
// MeasureCommdemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMeasureCommdemoApp:
// �йش����ʵ�֣������ MeasureCommdemo.cpp
//

class CMeasureCommdemoApp : public CWinApp
{
public:
	CMeasureCommdemoApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMeasureCommdemoApp theApp;