#pragma once

#include "CFont0.h"
#include "CRange.h" 
#include "Cnterior.h"
#include "CWorkbook.h"  
#include "CWorkbooks.h"  
#include "CWorksheet.h"  
#include "CWorksheets.h"  
#include "CApplication.h"
#include "include.h"
#include "StdAfx.h"

class OperateExcelFile
{
public:
	OperateExcelFile(void);
	~OperateExcelFile(void);

protected:  
    ///�򿪵�EXCEL�ļ�����  
    CString open_excel_file_;  
  
    ///EXCEL BOOK���ϣ�������ļ�ʱ��  
    CWorkbooks excel_books_;   
    ///��ǰʹ�õ�BOOK����ǰ������ļ�  
    CWorkbook excel_work_book_;   
    ///EXCEL��sheets����  
    CWorksheets excel_sheets_;   
    ///��ǰʹ��sheet  
    CWorksheet excel_work_sheet_;   
    ///��ǰ�Ĳ�������  
    CRange excel_current_range_;
	///��������
	CFont0 font;

    
    ///�Ƿ��Ѿ�Ԥ������ĳ��sheet������  
    BOOL          already_preload_;  
    ///Create the SAFEARRAY from the VARIANT ret.  
    COleSafeArray ole_safe_array_;  
  
protected:  
  
    ///EXCEL�Ľ���ʵ��  
    static CApplication excel_application_;  
public:  
 
    ///  
    void ShowInExcel(BOOL bShow);  
  
    ///���һ��CELL�Ƿ����ַ���  
    BOOL    IsCellString(long iRow, long iColumn);  
    ///���һ��CELL�Ƿ�����ֵ  
    BOOL    IsCellInt(long iRow, long iColumn);  
  
    ///�õ�һ��CELL��String  
    CString GetCellString(long iRow, long iColumn);  
    ///�õ�����  
    int     GetCellInt(long iRow, long iColumn);  
    ///�õ�double������  
    double  GetCellDouble(long iRow, long iColumn);  
  
    ///ȡ���е�����  
    int GetRowCount();  
    ///ȡ���е�����  
    int GetColumnCount();  
  
    ///ʹ��ĳ��shet��shit��shit  
    BOOL LoadSheet(long table_index,BOOL pre_load = FALSE);  
    ///ͨ������ʹ��ĳ��sheet��  
    BOOL LoadSheet(LPCTSTR sheet,BOOL pre_load = FALSE);  
    ///ͨ�����ȡ��ĳ��Sheet������  
    CString GetSheetName(long table_index);  
  
    ///�õ�Sheet������  
    int GetSheetCount();  
  
    ///���ļ�  
    void OpenExcelFile(CString file_name);  
    ///�رմ򿪵�Excel �ļ�����ʱ���EXCEL�ļ���Ҫ  
    void CloseExcelFile(BOOL if_save = FALSE);  
    //���Ϊһ��EXCEL�ļ�  
    void SaveasXSLFile(const CString &xls_file);  
    ///ȡ�ô��ļ�������  
    CString GetOpenFileName();  
    ///ȡ�ô�sheet������  
    CString GetLoadSheetName();
	//���ļ������ǲ��½�datasheet
	void OpenWithoutSheet(CString file_name);

	
  
    ///д��һ��CELLһ��int  
    void SetCellInt(long irow, long icolumn,long new_int);  
    ///д��һ��CELLһ��string  
    void SetCellString(long irow, long icolumn,CString new_string);  
	///д��һ��CELLһ��float 
	void SetCellFloat(long irow, long icolumn,float new_int);
	///д��һ��CELL��Wchar 
	void SetCellWchar(long irow, long icolumn,WCHAR* new_int);
	///д��ͷ
	void WriteTableHead(int meter_num, MeterInfo* m_info);
	///д����¼��ͷ
	void WriteErrTableHead(int meter_num, MeterInfo* m_info);
	///��������
	void OperateExcelFile::SaveExcel(void);
public:  
    ///��ʼ��EXCEL OLE  
    static BOOL InitExcel();  
    ///�ͷ�EXCEL�� OLE  
    static void ReleaseExcel();  
    ///ȡ���е����ƣ�����27->AA  
    static char *GetColumnName(long iColumn);  
	
protected:  
  
    //Ԥ�ȼ���  
    void PreLoadSheet();  

};

