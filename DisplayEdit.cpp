// DisplayEdit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DisplayEdit.h"


// DisplayEdit

IMPLEMENT_DYNAMIC(DisplayEdit, CEdit)

DisplayEdit::DisplayEdit()
{

}

DisplayEdit::~DisplayEdit()
{
}


BEGIN_MESSAGE_MAP(DisplayEdit, CEdit)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()



// DisplayEdit ��Ϣ�������

void DisplayEdit::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CString str;
	str = "";
	this->SetWindowText(str);
	CEdit::OnLButtonDblClk(nFlags, point);
}
