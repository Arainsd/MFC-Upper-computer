#include "StdAfx.h"
#include "OperateExcelFile.h"
#include "CommControl.h"
#include "PLCTransfer.h"
#include <math.h>
#include <string>

COleVariant
	covTrue((short)TRUE),
	covFalse((short)FALSE),
	covOptional((long)DISP_E_PARAMNOTFOUND, VT_ERROR);    

//
CApplication OperateExcelFile::excel_application_;


OperateExcelFile::OperateExcelFile():already_preload_(FALSE)
{

}

OperateExcelFile::~OperateExcelFile()
{
	//
	CloseExcelFile();
}


//��ʼ��EXCEL�ļ���
BOOL OperateExcelFile::InitExcel()
{

	CoUninitialize();
	if(CoInitialize(NULL)==S_FALSE) 
	{ 
		AfxMessageBox(_T("��ʼ��COM֧�ֿ�ʧ�ܣ�")); 
		return FALSE; 
	}
	//����Excel 2000������(����Excel) 
	if (!excel_application_.CreateDispatch(_T("Excel.Application"),NULL)) 
	{ 
		AfxMessageBox(_T("����Excel����ʧ��,�����û�а�װEXCEL������!")); 
		return FALSE;
	}

	excel_application_.put_DisplayAlerts(FALSE); 
	return TRUE;
}

//
void OperateExcelFile::ReleaseExcel()
{
	excel_application_.Quit();
	excel_application_.ReleaseDispatch();
	excel_application_=NULL;
}
//���ļ�(sample)ʱ���½�sheet
void OperateExcelFile::OpenWithoutSheet(CString file_name)
{
	//�ȹر�
	BOOL excel_status = FALSE;
	CloseExcelFile();

	LPDISPATCH lpDis = NULL;
	//WCHAR tmp_name[] = _T("�ɼ�����");
	WCHAR path_name[128];
	CString path;
	CString sheet_name;

	GetCurrentDirectory(128, path_name);
	path.Format(_T("%s%s"), path_name, file_name); //��õ�ǰ�򿪵�excel��·��

	//�õ�����������
	excel_books_.AttachDispatch(excel_application_.get_Workbooks());
// 	try
// 	{
		/*��һ��������*/
		lpDis = excel_books_.Open(path, 
			covOptional, covOptional, covOptional, covOptional, covOptional,
			covOptional, covOptional, covOptional, covOptional, covOptional, 
			covOptional, covOptional, covOptional, covOptional);
		excel_work_book_.AttachDispatch(lpDis);
// 	}
// 	catch(...)
// 	{
// 		/*����һ���µĹ�����*/
// 		lpDis = excel_books_.Add(covOptional);
// 		if (lpDis)
// 		{
// 			excel_work_book_.AttachDispatch(lpDis);
// 			excel_status = TRUE;
// 			//��¼�򿪵��ļ�����
// 		}
// 	}
	//�õ�Worksheets������ 
	excel_sheets_.AttachDispatch(excel_work_book_.get_Worksheets()); 

// 	open_excel_file_ = path;
// 	int count = GetSheetCount();
	/*
	try
	{
	//��һ�����е�Sheet
	lpDis = excel_sheets_.get_Item(_variant_t(sheet_name));
	excel_work_sheet_.AttachDispatch(lpDis);
	}
	catch(...)
	{*/
	//����һ���µ�Sheet
// 	sheet_name.Format(_T("%s%d"), tmp_name, count - 2);
// 	lpDis = excel_sheets_.Add(covOptional, covOptional, _variant_t((long)1), covOptional);
// 	excel_work_sheet_.AttachDispatch(lpDis);
// 	excel_work_sheet_.put_Name(sheet_name);
	//	}

	if(excel_status)
	{
		SaveasXSLFile(path);
	}
	else
	{
		SaveExcel();
	}	
}

//��excel�ļ�sample
void OperateExcelFile::OpenExcelFile(CString file_name)
{
	//�ȹر�
	BOOL excel_status = FALSE;
	CloseExcelFile();
	LPDISPATCH lpDis = NULL;
	WCHAR tmp_name[] = _T("�ɼ�����");
	WCHAR path_name[128];
	CString path;
	CString sheet_name;

	GetCurrentDirectory(128, path_name);
	path.Format(_T("%s%s"), path_name, file_name);

	/*�õ�����������*/
	excel_books_.AttachDispatch(excel_application_.get_Workbooks());

	try
	{
		/*��һ��������*/
		lpDis = excel_books_.Open(path, 
			covOptional, covOptional, covOptional, covOptional, covOptional,
			covOptional, covOptional, covOptional, covOptional, covOptional, 
			covOptional, covOptional, covOptional, covOptional);
		excel_work_book_.AttachDispatch(lpDis);
	}
	catch(...)
	{
		/*����һ���µĹ�����*/
		lpDis = excel_books_.Add(covOptional);
		if (lpDis)
		{
			excel_work_book_.AttachDispatch(lpDis);
			excel_status = TRUE;
			//��¼�򿪵��ļ�����
		}
	}
	//�õ�Worksheets������ 
	excel_sheets_.AttachDispatch(excel_work_book_.get_Worksheets()); 

	open_excel_file_ = path;
	int count = GetSheetCount();
	/*
	try
	{
	//��һ�����е�Sheet
	lpDis = excel_sheets_.get_Item(_variant_t(sheet_name));
	excel_work_sheet_.AttachDispatch(lpDis);
	}
	catch(...)
	{*/
	//����һ���µ�Sheet
	sheet_name.Format(_T("%s%d"), tmp_name, count - 2);
	lpDis = excel_sheets_.Add(covOptional, covOptional, _variant_t((long)1), covOptional);
	excel_work_sheet_.AttachDispatch(lpDis);
	excel_work_sheet_.put_Name(sheet_name);
	//	}

	if(excel_status)
	{
		SaveasXSLFile(path);
	}
	else
	{
		SaveExcel();
	}
	font.put_Bold(covFalse);
}

//�رմ򿪵�Excel �ļ�,Ĭ������������ļ�
void OperateExcelFile::CloseExcelFile(BOOL if_save)
{
	//����Ѿ��򿪣��ر��ļ�
	if (open_excel_file_.IsEmpty() == FALSE)
	{
		excel_work_book_.Close(COleVariant(short(FALSE)),COleVariant(open_excel_file_),covOptional);
		excel_books_.Close();
	}
	//���ļ����������
	open_excel_file_.Empty();

	excel_sheets_.ReleaseDispatch();
	excel_work_sheet_.ReleaseDispatch();
	excel_current_range_.ReleaseDispatch();
	excel_work_book_.ReleaseDispatch();
	excel_books_.ReleaseDispatch();
}

void OperateExcelFile::SaveExcel(void)  
{  
	excel_work_book_.Save();
}  

void OperateExcelFile::SaveasXSLFile(const CString &xls_file)
{
	excel_work_book_.SaveAs(COleVariant(xls_file),
		covOptional,
		covOptional,
		covOptional,
		covOptional,
		covOptional,
		0,
		covOptional,
		covOptional,
		covOptional,
		covOptional,
		covOptional);
	return;
}

int OperateExcelFile::GetSheetCount()
{
	return excel_sheets_.get_Count();
}

CString OperateExcelFile::GetSheetName(long table_index)
{
	CWorksheet sheet;
	sheet.AttachDispatch(excel_sheets_.get_Item(COleVariant((long)table_index)),true);
	CString name = sheet.get_Name();
	sheet.ReleaseDispatch();
	return name;
}

//������ż���Sheet���,������ǰ�������еı���ڲ�����
BOOL OperateExcelFile::LoadSheet(long table_index,BOOL pre_load)
{
	LPDISPATCH lpDis = NULL;
	excel_current_range_.ReleaseDispatch();
	excel_work_sheet_.ReleaseDispatch();
	lpDis = excel_sheets_.get_Item(COleVariant((long)table_index));
	if (lpDis)
	{
		excel_work_sheet_.AttachDispatch(lpDis,true);
		excel_current_range_.AttachDispatch(excel_work_sheet_.get_Cells(), true);
	}
	else
	{
		return FALSE;
	}

	already_preload_ = FALSE;
	//�������Ԥ�ȼ���
	if (pre_load)
	{
		PreLoadSheet();
		already_preload_ = TRUE;
	}

	return TRUE;
}

//�������Ƽ���Sheet���,������ǰ�������еı���ڲ�����
BOOL OperateExcelFile::LoadSheet(LPCTSTR sheet,BOOL pre_load)
{
	LPDISPATCH lpDis = NULL;
	excel_current_range_.ReleaseDispatch();
	excel_work_sheet_.ReleaseDispatch();
	lpDis = excel_sheets_.get_Item(COleVariant(sheet));
	if (lpDis)
	{
		excel_work_sheet_.AttachDispatch(lpDis,true);
		excel_current_range_.AttachDispatch(excel_work_sheet_.get_Cells(), true);

	}
	else
	{
		return FALSE;
	}
	//
	already_preload_ = FALSE;
	//�������Ԥ�ȼ���
	if (pre_load)
	{
		already_preload_ = TRUE;
		PreLoadSheet();
	}

	return TRUE;
}

//�õ��е�����
int OperateExcelFile::GetColumnCount()
{
	CRange range;
	CRange usedRange;
	usedRange.AttachDispatch(excel_work_sheet_.get_UsedRange(), true);
	range.AttachDispatch(usedRange.get_Columns(), true);
	int count = range.get_Count();
	usedRange.ReleaseDispatch();
	range.ReleaseDispatch();
	return count;
}

//�õ��е�����
int OperateExcelFile::GetRowCount()
{
	CRange range;
	CRange usedRange;
	usedRange.AttachDispatch(excel_work_sheet_.get_UsedRange(), true);
	range.AttachDispatch(usedRange.get_Rows(), true);
	int count = range.get_Count();
	usedRange.ReleaseDispatch();
	range.ReleaseDispatch();
	return count;
}

//���һ��CELL�Ƿ����ַ���
BOOL OperateExcelFile::IsCellString(long irow, long icolumn)
{
	CRange range;
	range.AttachDispatch(excel_current_range_.get_Item (COleVariant((long)irow),COleVariant((long)icolumn)).pdispVal, true);
	COleVariant vResult =range.get_Value2();
	//VT_BSTR��ʾ�ַ���
	if(vResult.vt == VT_BSTR)       
	{
		return TRUE;
	}
	return FALSE;
}

//���һ��CELL�Ƿ�����ֵ
BOOL OperateExcelFile::IsCellInt(long irow, long icolumn)
{
	CRange range;
	range.AttachDispatch(excel_current_range_.get_Item (COleVariant((long)irow),COleVariant((long)icolumn)).pdispVal, true);
	COleVariant vResult =range.get_Value2();
	//����һ�㶼��VT_R8
	if(vResult.vt == VT_INT || vResult.vt == VT_R8)       
	{
		return TRUE;
	}
	return FALSE;
}

//
CString OperateExcelFile::GetCellString(long irow, long icolumn)
{

	COleVariant vResult ;
	CString str;
	//�ַ���
	if (already_preload_ == FALSE)
	{
		CRange range;
		range.AttachDispatch(excel_current_range_.get_Item (COleVariant((long)irow),COleVariant((long)icolumn)).pdispVal, true);
		vResult =range.get_Value2();
		range.ReleaseDispatch();
	}
	//�����������Ԥ�ȼ�����
	else
	{
		long read_address[2];
		VARIANT val;
		read_address[0] = irow;
		read_address[1] = icolumn;
		ole_safe_array_.GetElement(read_address, &val);
		vResult = val;
	}

	if(vResult.vt == VT_BSTR)
	{
		str=vResult.bstrVal;
	}
	//����
	else if (vResult.vt==VT_INT)
	{
		str.Format(_T("%d"),vResult.pintVal);
	}
	//8�ֽڵ����� 
	else if (vResult.vt==VT_R8)     
	{
		str.Format(_T("%0.0f"),vResult.dblVal);
	}
	//ʱ���ʽ
	else if(vResult.vt==VT_DATE)    
	{
		SYSTEMTIME st;
		VariantTimeToSystemTime(vResult.date, &st);
		CTime tm(st); 
		str=tm.Format("%Y-%m-%d");

	}
	//��Ԫ��յ�
	else if(vResult.vt==VT_EMPTY)   
	{
		str="";
	}  

	return str;
}

double OperateExcelFile::GetCellDouble(long irow, long icolumn)
{
	double rtn_value = 0;
	COleVariant new_value(rtn_value);
	//�ַ���
	if (already_preload_ == FALSE)
	{
		CRange range;
		range.AttachDispatch(excel_current_range_.get_Item (COleVariant((long)irow),COleVariant((long)icolumn)).pdispVal, true);
		new_value =range.get_Value2();
		range.ReleaseDispatch();
	}
	//�����������Ԥ�ȼ�����
	else
	{
		long read_address[2];
		VARIANT val;
		read_address[0] = irow;
		read_address[1] = icolumn;
		ole_safe_array_.GetElement(read_address, &val);
		new_value = val;
	}

	if (new_value.vt==VT_R8)     
	{
		rtn_value = new_value.dblVal;
	}

	return rtn_value;
}

//VT_R8
int OperateExcelFile::GetCellInt(long irow, long icolumn)
{
	int num;
	COleVariant vresult;

	if (already_preload_ == FALSE)
	{
		CRange range;
		range.AttachDispatch(excel_current_range_.get_Item (COleVariant((long)irow),COleVariant((long)icolumn)).pdispVal, true);
		vresult = range.get_Value2();
		range.ReleaseDispatch();
	}
	else
	{
		long read_address[2];
		VARIANT val;
		read_address[0] = irow;
		read_address[1] = icolumn;
		ole_safe_array_.GetElement(read_address, &val);
		vresult = val;
	}
	//
	num = static_cast<int>(vresult.dblVal);

	return num;
}

void OperateExcelFile::SetCellString(long irow, long icolumn,CString new_string)
{
	COleVariant new_value(new_string);

	CRange start_range= excel_work_sheet_.get_Range(COleVariant(_T("A1")),covOptional);
	CRange write_range = start_range.get_Offset(COleVariant((long)irow -1),COleVariant((long)icolumn -1) );
	write_range.put_Value2(new_value);
	start_range.ReleaseDispatch();
	write_range.ReleaseDispatch();

}

void OperateExcelFile::SetCellInt(long irow, long icolumn,long new_int)
{
	COleVariant new_value((long)new_int);

	CRange start_range = excel_work_sheet_.get_Range(COleVariant(_T("A1")),covOptional);
	CRange write_range = start_range.get_Offset(COleVariant((long)irow -1),COleVariant((long)icolumn -1) );
	write_range.put_Value2(new_value);
	start_range.ReleaseDispatch();
	write_range.ReleaseDispatch();
}

void OperateExcelFile::SetCellFloat(long irow, long icolumn,float new_int)
{
	COleVariant new_value((float)new_int);

	CRange start_range = excel_work_sheet_.get_Range(COleVariant(_T("A1")),covOptional);
	CRange write_range = start_range.get_Offset(COleVariant((long)irow -1),COleVariant((long)icolumn -1) );
	write_range.put_Value2(new_value);
	start_range.ReleaseDispatch();
	write_range.ReleaseDispatch();
}

void OperateExcelFile::SetCellWchar(long irow, long icolumn,WCHAR* new_int)
{
	COleVariant new_value((WCHAR*)new_int);

	CRange start_range = excel_work_sheet_.get_Range(COleVariant(_T("A1")),covOptional);
	CRange write_range = start_range.get_Offset(COleVariant((long)irow -1),COleVariant((long)icolumn -1) );
	write_range.put_Value2(new_value);
	start_range.ReleaseDispatch();
	write_range.ReleaseDispatch();
}

//
void OperateExcelFile::ShowInExcel(BOOL bShow)
{
	excel_application_.put_Visible(bShow);
	excel_application_.put_UserControl(bShow);
}

//���ش򿪵�EXCEL�ļ�����
CString OperateExcelFile::GetOpenFileName()
{
	return open_excel_file_;
}

//ȡ�ô�sheet������
CString OperateExcelFile::GetLoadSheetName()
{
	return excel_work_sheet_.get_Name();
}

//ȡ���е����ƣ�����27->AA
char *OperateExcelFile::GetColumnName(long icolumn)
{   
	static char column_name[64];
	size_t str_len = 0;

	while(icolumn > 0)
	{
		int num_data = icolumn % 26;
		icolumn /= 26;
		if (num_data == 0)
		{
			num_data = 26;
			icolumn--;
		}
		column_name[str_len] = (char)((num_data-1) + 'A' );
		str_len ++;
	}
	column_name[str_len] = '\0';
	//��ת
	_strrev(column_name);

	return column_name;
}

//Ԥ�ȼ���
void OperateExcelFile::PreLoadSheet()
{

	CRange used_range;

	used_range = excel_work_sheet_.get_UsedRange();	


	VARIANT ret_ary = used_range.get_Value2();
	if (!(ret_ary.vt & VT_ARRAY))
	{
		return;
	}
	//
	ole_safe_array_.Clear();
	ole_safe_array_.Attach(ret_ary); 
}

//дtable head
void OperateExcelFile::WriteTableHead(int meter_num, MeterInfo* m_info)
{
	int i;
	char str[14];
	WCHAR t_id_dt[17];
	WCHAR t_id_tf[17];
	WCHAR t_id_vol[17];
	WCHAR t_id_twos[17];
	WCHAR t_id_taos[17];

	CRange start_range= excel_work_sheet_.get_Range(COleVariant(_T("A1")),COleVariant(_T("AZ1")));
	font = start_range.get_Font();
	font.put_Bold(covTrue);

	SetCellString(1, 1, _T("ʱ��"));
	SetCellString(1, 2, _T("��������"));
	SetCellString(1, 3, _T("˲ʱ������"));
	SetCellString(1, 4, _T("˲ʱС����"));
	SetCellString(1, 5, _T("��ˮ��ѹ��"));
	SetCellString(1, 6, _T("��ˮ��ѹ��"));
	SetCellString(1, 7, _T("��ˮ���¶�"));
	SetCellString(1, 8, _T("��ˮ���¶�"));
	SetCellString(1, 9, _T("��ˮ���¶�"));
	SetCellString(1, 10, _T("��Ƶ��Ƶ��"));
	SetCellString(1, 11, _T("����״̬"));

	for(i = 0;i < meter_num; i++)
	{
		str[12] = (m_info[i].meterid[0]>>4) + '0';
		str[13] = (m_info[i].meterid[0]&0x0f) + '0';
		str[10] = (m_info[i].meterid[1]>>4) + '0';
		str[11] = (m_info[i].meterid[1]&0x0f) + '0';
		str[8] = (m_info[i].meterid[2]>>4) + '0';
		str[9] = (m_info[i].meterid[2]&0x0f) + '0';
		str[6] = (m_info[i].meterid[3]>>4) + '0';
		str[7] = (m_info[i].meterid[3]&0x0f) + '0';
		str[4] = (m_info[i].meterid[4]>>4) + '0';
		str[5] = (m_info[i].meterid[4]&0x0f) + '0';
		str[2] = (m_info[i].meterid[5]>>4) + '0';
		str[3] = (m_info[i].meterid[5]&0x0f) + '0';
		str[0] = (m_info[i].meterid[6]>>4) + '0';
		str[1] = (m_info[i].meterid[6]&0x0f) + '0';
		mbstowcs(t_id_dt, str, 14);
		mbstowcs(t_id_tf, str, 14);
		mbstowcs(t_id_vol,str,14);
		mbstowcs(t_id_twos, str, 14);
		mbstowcs(t_id_taos,str,14);
		t_id_dt[14] = 'd'; 
		t_id_dt[15] = 't';
		t_id_dt[16] = '\0';

		t_id_tf[14] = 't'; 
		t_id_tf[15] = 'f';
		t_id_tf[16] = '\0';

		t_id_vol[14]='m';
		t_id_vol[15]='l';
		t_id_vol[16] = '\0';

		t_id_twos[14] = 't';
		t_id_twos[15] = 'w';
		t_id_twos[16] = '\0';

		t_id_taos[14] = 't';
		t_id_taos[15] = 'e';
		t_id_taos[16] = '\0';
		SetCellWchar(1, 12 + i * 5, t_id_vol);
		SetCellWchar(1, 13 + i * 5, t_id_dt);
		SetCellWchar(1, 14 + i * 5, t_id_tf);
		SetCellWchar(1, 15 + i * 5, t_id_twos);
		SetCellWchar(1, 16 + i * 5, t_id_taos);
	}
	font.ReleaseDispatch();
}

//д����¼table head
void OperateExcelFile::WriteErrTableHead(int meter_num, MeterInfo* m_info)
{
 	int i;
 	char str[14];
 	WCHAR t_id_ml0[18];
 	WCHAR t_id_ml1[18];
 	WCHAR t_id_ml[17];
 	WCHAR t_id_err[18];
 	WCHAR t_id_wt[17];//ˮ��

	CRange start_range= excel_work_sheet_.get_Range(COleVariant(_T("A1")),COleVariant(_T("AZ1")));
	font = start_range.get_Font();
	font.put_Bold(covTrue);

	SetCellString(1, 1, _T("ʱ��"));
	SetCellString(1, 2, _T("����"));
	SetCellString(1, 3, _T("��������ʼ"));
	SetCellString(1, 4, _T("����������"));
	SetCellString(1, 5, _T("���������ۻ�"));	

	for(i = 0;i < meter_num; i++)
	{
		str[12] = (m_info[i].meterid[0]>>4) + '0';
		str[13] = (m_info[i].meterid[0]&0x0f) + '0';
		str[10] = (m_info[i].meterid[1]>>4) + '0';
		str[11] = (m_info[i].meterid[1]&0x0f) + '0';
		str[8] = (m_info[i].meterid[2]>>4) + '0';
		str[9] = (m_info[i].meterid[2]&0x0f) + '0';
		str[6] = (m_info[i].meterid[3]>>4) + '0';
		str[7] = (m_info[i].meterid[3]&0x0f) + '0';
		str[4] = (m_info[i].meterid[4]>>4) + '0';
		str[5] = (m_info[i].meterid[4]&0x0f) + '0';
		str[2] = (m_info[i].meterid[5]>>4) + '0';
		str[3] = (m_info[i].meterid[5]&0x0f) + '0';
		str[0] = (m_info[i].meterid[6]>>4) + '0';
		str[1] = (m_info[i].meterid[6]&0x0f) + '0';

		mbstowcs(t_id_ml0, str, 14);
		mbstowcs(t_id_ml1, str, 14);
		mbstowcs(t_id_ml, str,14);
		mbstowcs(t_id_err, str, 14);
		mbstowcs(t_id_wt, str,14);
		t_id_ml0[14] = 'm'; 
		t_id_ml0[15] = 'l';
		t_id_ml0[16] = '0';
		t_id_ml0[17] = '\0';

		t_id_ml1[14] = 'm'; 
		t_id_ml1[15] = 'l';
		t_id_ml1[16] = '1';
		t_id_ml1[17] = '\0';

		t_id_ml[14]='m';
		t_id_ml[15]='l';
		t_id_ml[16] = '\0';

		t_id_err[14] = 'e';
		t_id_err[15] = 'r';
		t_id_err[16] = 'r';
		t_id_err[17] = '\0';

		t_id_wt[14] = 'w';
		t_id_wt[15] = 't';
		t_id_wt[16] = '\0';
		SetCellWchar(1, 6  + i * 5, t_id_ml0);
		SetCellWchar(1, 7  + i * 5, t_id_ml1);
		SetCellWchar(1, 8  + i * 5, t_id_ml);
		SetCellWchar(1, 9  + i * 5, t_id_err);
		SetCellWchar(1, 10 + i * 5, t_id_wt);
	}
	font.ReleaseDispatch();
}