
// MeasureCommdemoDlg.cpp : ʵ���ļ�
//
#include "stdafx.h"
#include "MeasureCommdemo.h"
#include "MeasureCommdemoDlg.h"
#include "afxdialogex.h"
#include "CommControl.h"
#include "PLCTransfer.h"
#include "include.h"
#include <map>
#include "stdlib.h"
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HANDLE Meterhdev;
HANDLE Meterhdev_com21;
HANDLE Meterhdev_com22;
HANDLE Meterhdev_com23;
HANDLE Meterhdev_com24;
HANDLE Meterhdev_com25;
HANDLE Meterhdev_com26;
HANDLE Meterhdev_com27;
HANDLE Meterhdev_com28;
HANDLE Meterhdev_com29;
HANDLE Meterhdev_com30;

static HANDLE PLChdev;
static HANDLE Weighthdev;
static HANDLE Danfosshdev;
static HANDLE Flowhdev;
static HANDLE plcMutex = NULL;
static HANDLE xxMutex = NULL;
static BOOL SampleFlag = FALSE;
static int SampleFlagHZ1 = 0;
static BOOL InitFlag = FALSE;
static BOOL RecordFlag = FALSE;		//ˮ���ȶ�״̬��־
static BOOL TmpFlag = FALSE;		//��¼���ݱ�־
static BOOL PIDFlag = FALSE;
static BOOL ErrTestFlag = TRUE;  //�����Ա�־
SampleInfo sinfotmp;
SampleInfo sinfoend;
int calcnum = 0;
CString str_err;
CString t_str_err;
CString str_hz;

static int PID_dflow;
static int SampleType = 0x0;
static int tick;
static int tick1;
static int tick_second = 0;
static short Spercent;
std::map <int, float> lflowmap;
static int flowcount;
static int hzpercent;
static float flowoffset;
static int vavlestate = 0x0;
static float t_f;

static bool compareflow(std::map <int, float> *t_map,bool PIDFlag);
static float EndianConvert32(unsigned char *buffer);
static UINT ReadWight(LPVOID pParam);
static UINT ReadPLCThread(LPVOID pParam);
static UINT ReadFlow(LPVOID pParam);
static UINT ReadMeter(LPVOID pParam);
static UINT PIDControl(LPVOID pParam);
static void PID_init();
static int  PID_realized(float setflowrate);
static CWinThread *PIDThread;
static CWinThread *ReadMeterThread;
static CWinThread *PLCthread;
static CWinThread *Weightthread;
static CWinThread *FlowThread;

PID pid;
SampleInfo sinfo;
OperateExcelFile MeterIDFile;
OperateExcelFile ErrRcrdFile;
FlowPram flowpara;
/*static void impmeterid();*/

unsigned short timeremain;

static int vavles[12] = {0x01,0x01,0x02,0x03,\
	0x04,0x0B,0x0B,0x0B,\
	0x0B,0x0B,0x0B,0x0B};
static int hz[12] = {18 * 200, 29 * 200, 29 * 200,	29 * 200,\
	9.5 * 200, 11 * 200, 15 * 200,	20 * 200, \
	27 * 200,	35 * 200, 41 * 200, 48 * 200};
static int hz_pt;

static int ErrTestVavles[10] = {0x01,		0x01,			0x02,		0x02,		0x04,\
								0x04,		0x08,			0x08,		0x0C,		0x0C};

static int ErrTestHz[10] = { 9.5 * 200,		10.5 * 200,		9.3 * 200,  13 * 200,	10 * 200,\
							  13 * 200,		13 * 200,		15 * 200,	23 * 200,   36 * 200};

static int ErrTestTime[10] = {	5 * 30,		5 * 30,			4 * 30,		 4 * 30,	3 * 30,\
								3 * 30,		1 * 30,			1 * 30,		1 * 30,		 1 * 30};
static int ErrTest_pt;

static double Density_water[410] = {	999.840,999.846,999.853,999.859,999.865,999.871,999.877,999.883,999.888,999.893,\
									999.898,999.904,999.908,999.913,999.917,999.921,999.925,999.929,999.933,999.937,\
									999.940,999.943,999.946,999.949,999.952,999.954,999.956,999.959,999.961,999.962,\
									999.964,999.966,999.967,999.968,999.969,999.970,999.971,999.971,999.972,999.972,\
									999.972,999.972,999.972,999.971,999.971,999.970,999.969,999.968,999.967,999.965,\
									999.964,999.962,999.960,999.958,999.956,999.954,999.951,999.949,999.946,999.943,\
									999.940,999.937,999.934,999.930,999.926,999.923,999.919,999.915,999.910,999.906,\
									999.901,999.897,999.892,999.887,999.882,999.877,999.871,999.866,999.880,999.854,\
									999.848,999.842,999.836,999.829,999.823,999.816,999.809,999.802,999.795,999.788,\
									999.781,999.773,999.765,999.758,999.750,999.742,999.734,999.725,999.717,999.708,\
									999.699,999.691,999.682,999.672,999.663,999.654,999.644,999.634,999.625,999.615,\
									999.605,999.595,999.584,999.574,999.563,999.553,999.542,999.531,999.520,999.508,\
									999.497,999.486,999.474,999.462,999.450,999.439,999.426,999.414,999.402,999.389,\
									999.377,999.384,999.351,999.338,999.325,999.312,999.299,999.285,999.271,999.258,\
									999.244,999.230,999.216,999.202,999.187,999.173,999.158,999.144,999.129,999.114,\
									999.099,999.084,999.069,999.053,999.038,999.022,999.006,998.991,998.975,998.959,\
									998.943,998.926,998.910,998.893,998.876,998.860,998.843,998.826,998.809,998.792,\
									998.774,998.757,998.739,998.722,998.704,998.686,998.668,998.650,998.632,998.613,\
									998.595,998.576,998.557,998.539,998.520,998.501,998.482,998.463,998.443,998.424,\
									998.404,998.385,998.365,998.345,998.325,998.305,998.285,998.265,998.244,998.224,\
									998.203,998.182,998.162,998.141,998.120,998.099,998.077,998.056,998.035,998.013,\
									997.991,997.970,997.948,997.926,997.904,997.882,997.859,997.837,997.815,997.792,\
									997.769,997.747,997.724,997.701,997.678,997.655,997.631,997.608,997.584,997.561,\
									997.537,997.513,997.490,997.466,997.442,997.417,997.393,997.396,997.344,997.320,\
									997.295,997.270,997.246,997.221,997.195,997.170,997.145,997.120,997.094,997.069,\
									997.043,997.018,996.992,996.966,996.940,996.914,996.888,996.861,996.835,996.809,\
									996.782,996.755,996.729,996.702,996.675,996.648,996.621,996.594,996.566,996.539,\
									996.511,996.484,996.456,996.428,996.401,996.373,996.344,996.316,996.288,996.260,\
									996.231,996.203,996.174,996.146,996.117,996.088,996.059,996.030,996.001,996.972,\
									995.943,995.913,995.884,995.854,995.825,995.795,995.765,995.753,995.705,995.675,\
									995.645,995.615,995.584,995.554,995.523,995.493,995.462,995.431,995.401,995.370,\
									995.339,995.307,995.276,995.245,995.214,995.182,995.151,995.119,995.087,995.055,\
									995.024,994.992,994.960,994.927,994.895,994.863,994.831,994.798,994.766,994.733,\
									994.700,994.667,994.635,994.602,994.569,994.535,994.502,994.469,994.436,994.402,\
									994.369,994.335,994.301,994.267,994.234,994.200,994.166,994.132,994.098,994.063,\
									994.029,993.994,993.960,993.925,993.891,993.856,993.821,993.786,993.751,993.716,\
									993.681,993.646,993.610,993.575,993.540,993.504,993.469,993.433,993.397,993.361,\
									993.325,993.280,993.253,993.217,993.181,993.144,993.108,993.072,993.035,992.999,\
									992.962,992.925,992.888,992.851,992.814,992.777,992.740,992.703,992.665,992.628,\
									992.591,992.553,992.516,992.478,992.440,992.402,992.364,992.326,992.288,992.250,\
									992.212,991.826,991.432,991.031,990.623,990.208,989.786,987.358,988.922,988.479,\
};
// CMeasureCommdemoDlg �Ի���

CMeasureCommdemoDlg::CMeasureCommdemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMeasureCommdemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMeasureCommdemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1,m_combo1);
	DDX_Control(pDX, IDC_COMBO2,m_combo2);
	DDX_Control(pDX, IDC_COMBO3,m_combo3);
	DDX_Control(pDX, IDC_COMBO4,m_combo4);
	DDX_Control(pDX, IDC_COMBO5,m_combo5);
	DDX_Control(pDX, IDC_COMBO6,m_combo6);
	DDX_Control(pDX, IDC_EDIT3,displayEdit);
	DDX_Control(pDX, IDC_EDIT4,displayResult);	
	DDX_Control(pDX, IDC_EDIT9,displayFlowRate);
	DDX_Control(pDX, IDC_COMBO7, m_combo7);
	DDX_Control(pDX, IDC_COMBO8, m_combo8);
	DDX_Control(pDX, IDC_COMBO9, m_combo9_meter6);
}

BEGIN_MESSAGE_MAP(CMeasureCommdemoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON1, &CMeasureCommdemoDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMeasureCommdemoDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CMeasureCommdemoDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CMeasureCommdemoDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CMeasureCommdemoDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CMeasureCommdemoDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CMeasureCommdemoDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON8, &CMeasureCommdemoDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CMeasureCommdemoDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CMeasureCommdemoDlg::OnBnClickedButton10)
	ON_BN_CLICKED(IDC_BUTTON11, &CMeasureCommdemoDlg::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON12, &CMeasureCommdemoDlg::OnBnClickedButton12)
	ON_BN_CLICKED(IDC_BUTTON13, &CMeasureCommdemoDlg::OnBnClickedButton13)
	ON_BN_CLICKED(IDC_BUTTON14, &CMeasureCommdemoDlg::OnBnClickedButton14)
	ON_BN_CLICKED(IDC_BUTTON15, &CMeasureCommdemoDlg::OnBnClickedButton15)
	ON_BN_CLICKED(IDC_BUTTON17, &CMeasureCommdemoDlg::OnBnClickedButton17)
	ON_BN_CLICKED(IDC_BUTTON18, &CMeasureCommdemoDlg::OnBnClickedButton18)
	ON_BN_CLICKED(IDC_BUTTON19, &CMeasureCommdemoDlg::OnBnClickedButton19)
	ON_BN_CLICKED(IDC_BUTTON20, &CMeasureCommdemoDlg::OnBnClickedButton20)
	/*ON_BN_CLICKED(IDC_BUTTON21, &CMeasureCommdemoDlg::OnBnClickedButton21)*/
/*	ON_BN_CLICKED(IDC_BUTTON16, &CMeasureCommdemoDlg::OnBnClickedButton16)*/
/*	ON_BN_CLICKED(IDC_BUTTON22, &CMeasureCommdemoDlg::OnBnClickedButton22)*/
	ON_BN_CLICKED(IDC_BUTTON23, &CMeasureCommdemoDlg::OnBnClickedButton23)
	ON_BN_CLICKED(IDC_BUTTON24, &CMeasureCommdemoDlg::OnBnClickedButton24)
	ON_BN_CLICKED(IDC_BUTTON25, &CMeasureCommdemoDlg::OnBnClickedButton25)
	ON_BN_CLICKED(IDC_BUTTON26, &CMeasureCommdemoDlg::OnBnClickedButton26)
	
	ON_BN_CLICKED(IDC_BUTTON28, &CMeasureCommdemoDlg::OnBnClickedButton28)
	ON_BN_CLICKED(IDC_BUTTON27, &CMeasureCommdemoDlg::OnBnClickedButton27)
	ON_BN_CLICKED(IDC_BUTTON29, &CMeasureCommdemoDlg::OnBnClickedButton29)

	ON_BN_CLICKED(IDC_BUTTON30, &CMeasureCommdemoDlg::OnBnClickedButton30)
	ON_BN_CLICKED(IDC_BUTTON31, &CMeasureCommdemoDlg::OnBnClickedButton31)
	ON_BN_CLICKED(IDC_BUTTON32, &CMeasureCommdemoDlg::OnBnClickedButton32)
	ON_BN_CLICKED(IDC_BUTTON33, &CMeasureCommdemoDlg::OnBnClickedButton33)
END_MESSAGE_MAP()
void CMeasureCommdemoDlg::OnOK()
{

}
void CMeasureCommdemoDlg::OnCancel()
{

}

float EndianConvert32(unsigned char *buffer)
{
	float t_float;
	unsigned char*pt;

	pt = (unsigned char*)&t_float;
	*pt++ = buffer[3];
	*pt++ = buffer[2];
	*pt++ = buffer[1];
	*pt++ = buffer[0];

	return  t_float; 
}

bool compareflow(std::map <int, float> *t_map,BOOL PIDFlag)
{
	int i, j;
	float t_flow1, t_flow2;
	if (PIDFlag)
	{
		for(i = 1; i <= 5; i++)
		{
			t_flow1 = (t_map->find(i))->second;
			if((t_flow1 > (pid.setflowrate *(1 + flowoffset))) || (t_flow1 < (pid.setflowrate *(1-  flowoffset))))
			{
				return FALSE;
			}			
		}
	} 
	else
	{
		for(i = 1; i <= 4; i++)
		{
			t_flow1 = (t_map->find(i))->second;
			for(j = i +1; j <= 5; j++)
			{
				t_flow2 = (t_map->find(j))->second;
				if((t_flow2 > (t_flow1 * (1 +  flowoffset))) || (t_flow2 < (t_flow1 * (1 -  flowoffset))))
				{
					return FALSE;
				}				
			}
		}
	}	
	return TRUE;
}

//PID�������
int PID_realized(float setflowrate )
{
	int Hz = 0;
	pid.err = setflowrate - (pid.acflowratepre + pid.actualflowrate)/2;
	Hz = pid.KP * (pid.err - pid.err_pre)  + pid.KI * pid.err + \
		pid.KD * (pid.err - 2 * pid.err_pre + pid.err_ppre);
	pid.err_ppre = pid.err_pre;
	pid.err_pre  = pid.err;	
	return Hz;
}
//PID�㷨��ʼ��
void PID_init()
{
	pid.setflowrate    =0.0;
	pid.actualflowrate =0.0;
	pid.acflowratepre =0.0;
	pid.err			=0.0;
	pid.err_pre     =0.0;
	pid.err_ppre    =0.0;
	pid.Hz   =0;	
	pid.KP   =0.2871;/*0.2*//*1.5;*///0.12/
	pid.KI   =1.4875;
	pid.KD   =0;
	pid.valvell =1;
	pid.valvels =1;
	pid.valvesl =1;
	pid.valvess =1;
}
// CMeasureCommdemoDlg ��Ϣ�������

BOOL CMeasureCommdemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_combo1.AddString(_T("COM0"));
	m_combo1.AddString(_T("COM1"));
	m_combo1.AddString(_T("COM2"));
	m_combo1.AddString(_T("COM3"));
	m_combo1.AddString(_T("COM4"));
	m_combo1.AddString(_T("COM5"));
	m_combo1.AddString(_T("COM6"));
	m_combo1.AddString(_T("COM7"));
	m_combo1.AddString(_T("COM8"));
	m_combo1.AddString(_T("COM9"));
	m_combo1.AddString(_T("COM10"));
	m_combo1.SetCurSel(0);

	m_combo2.AddString(_T("COM0"));
	m_combo2.AddString(_T("COM1"));
	m_combo2.AddString(_T("COM2"));
	m_combo2.AddString(_T("COM3"));
	m_combo2.AddString(_T("COM4"));
	m_combo2.AddString(_T("COM5"));
	m_combo2.AddString(_T("COM6"));
	m_combo2.AddString(_T("COM7"));
	m_combo2.AddString(_T("COM8"));
	m_combo2.AddString(_T("COM9"));
	m_combo2.AddString(_T("COM10"));
	m_combo2.SetCurSel(0);

	//meter1
	m_combo3.AddString(_T("COM0"));
	m_combo3.AddString(_T("COM1"));
	m_combo3.AddString(_T("COM2"));
	m_combo3.AddString(_T("COM3"));
	m_combo3.AddString(_T("COM4"));
	m_combo3.AddString(_T("COM5"));
	m_combo3.AddString(_T("COM6"));
	m_combo3.AddString(_T("COM7"));
	m_combo3.AddString(_T("COM8"));
	m_combo3.AddString(_T("COM9"));
	m_combo3.AddString(_T("COM10"));
	m_combo3.SetCurSel(10);
	
	//meter2
	m_combo4.AddString(_T("COM0"));
	m_combo4.AddString(_T("COM1"));
	m_combo4.AddString(_T("COM2"));
	m_combo4.AddString(_T("COM3"));
	m_combo4.AddString(_T("COM4"));
	m_combo4.AddString(_T("COM5"));
	m_combo4.AddString(_T("COM6"));
	m_combo4.AddString(_T("COM7"));
	m_combo4.AddString(_T("COM8"));
	m_combo4.AddString(_T("COM9"));
	m_combo4.AddString(_T("COM10"));
	m_combo4.SetCurSel(4);

	//meter3
	m_combo5.AddString(_T("COM0"));
	m_combo5.AddString(_T("COM1"));
	m_combo5.AddString(_T("COM2"));
	m_combo5.AddString(_T("COM3"));
	m_combo5.AddString(_T("COM4"));
	m_combo5.AddString(_T("COM5"));
	m_combo5.AddString(_T("COM6"));
	m_combo5.AddString(_T("COM7"));
	m_combo5.AddString(_T("COM8"));
	m_combo5.AddString(_T("COM9"));
	m_combo5.AddString(_T("COM10"));
	m_combo5.SetCurSel(6);
		
	m_combo6.AddString(_T("COM0"));
	m_combo6.AddString(_T("COM1"));
	m_combo6.AddString(_T("COM2"));
	m_combo6.AddString(_T("COM3"));
	m_combo6.AddString(_T("COM4"));
	m_combo6.AddString(_T("COM5"));
	m_combo6.AddString(_T("COM6"));
	m_combo6.AddString(_T("COM7"));
	m_combo6.AddString(_T("COM8"));
	m_combo6.AddString(_T("COM9"));
	m_combo6.AddString(_T("COM10"));
	m_combo6.SetCurSel(0);

	//meter4
	m_combo7.AddString(_T("COM0"));
	m_combo7.AddString(_T("COM1"));
	m_combo7.AddString(_T("COM2"));
	m_combo7.AddString(_T("COM3"));
	m_combo7.AddString(_T("COM4"));
	m_combo7.AddString(_T("COM5"));
	m_combo7.AddString(_T("COM6"));
	m_combo7.AddString(_T("COM7"));
	m_combo7.AddString(_T("COM8"));
	m_combo7.AddString(_T("COM9"));
	m_combo7.AddString(_T("COM10"));
	m_combo7.SetCurSel(5);

	//meter5
	m_combo8.AddString(_T("COM0"));
	m_combo8.AddString(_T("COM1"));
	m_combo8.AddString(_T("COM2"));
	m_combo8.AddString(_T("COM3"));
	m_combo8.AddString(_T("COM4"));
	m_combo8.AddString(_T("COM5"));
	m_combo8.AddString(_T("COM6"));
	m_combo8.AddString(_T("COM7"));
	m_combo8.AddString(_T("COM8"));
	m_combo8.AddString(_T("COM9"));
	m_combo8.AddString(_T("COM10"));
	m_combo8.SetCurSel(7);

	//meter6
	m_combo9_meter6.AddString(_T("COM0"));
	m_combo9_meter6.AddString(_T("COM1"));
	m_combo9_meter6.AddString(_T("COM2"));
	m_combo9_meter6.AddString(_T("COM3"));
	m_combo9_meter6.AddString(_T("COM4"));
	m_combo9_meter6.AddString(_T("COM5"));
	m_combo9_meter6.AddString(_T("COM6"));
	m_combo9_meter6.AddString(_T("COM7"));
	m_combo9_meter6.AddString(_T("COM8"));
	m_combo9_meter6.AddString(_T("COM9"));
	m_combo9_meter6.AddString(_T("COM10"));
	m_combo9_meter6.SetCurSel(8);

	
	pmtdialog.Create(IDD_PARAMETERTOOl,this);
	SetDlgItemInt(IDC_EDIT9,0);
// 	ErrList.SetExtendedStyle(/*m_List.GetExtendedStyle() |*/ LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT); // �����+��ѡ��
// 	ErrList.InsertColumn(0,_T("����"),LVCFMT_LEFT, 60,-1);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMeasureCommdemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}

}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CMeasureCommdemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
//�ش���
void CMeasureCommdemoDlg::OnClose()
{
	opexf.SaveExcel();
	opexf.CloseExcelFile();
	opexf.ReleaseExcel();
	if (!InitFlag)
	{
		
	}
	else
	{
		PIDThread->SuspendThread();
		PLCthread->SuspendThread();
		Weightthread->SuspendThread();		
		FlowThread->SuspendThread();
		ReadMeterThread->SuspendThread();

		PIDThread->Delete();
		PLCthread->Delete();
		Weightthread->Delete();				
		FlowThread->Delete();		
		ReadMeterThread->Delete();

		ExitComm((int)PLChdev);
		ExitComm((int)Danfosshdev);	
		ExitComm((int)Weighthdev);		
		ExitComm((int)Flowhdev);
		/*ExitComm((int)Meterhdev);*/

		ExitComm((int)Meterhdev_com21);
		ExitComm((int)Meterhdev_com22);
		ExitComm((int)Meterhdev_com23);
		ExitComm((int)Meterhdev_com24);
		ExitComm((int)Meterhdev_com25);
		ExitComm((int)Meterhdev_com26);
		ExitComm((int)Meterhdev_com27);
		ExitComm((int)Meterhdev_com28);
		ExitComm((int)Meterhdev_com29);
		ExitComm((int)Meterhdev_com30);
	}	

	DestroyWindow();
}
//д������
void CMeasureCommdemoDlg::OnTimer(UINT nIDEvent)
{
	int t_num;
	int row;
	CString t_str;
	CString str;
	int i;
	int tick30_n = 0;

	PLCFrame pf;
	u8 tmp_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	memset(&pf, 0, sizeof(PLCFrame));
	tmp_data[3] = 1;

	//tick_second ++;
	if((SampleType == 0) && (SampleFlag == TRUE))
	{
		tick_second ++;
		tick++;		
		if(tick % 2)
		{
			//SampleType |= 0x04;
			switch (SampleFlagHZ1)
			{
			case 1://��ͨ����
				{
					SampleType = 0x07;
// 					row = opexf.GetRowCount();						
// 					opexf.SetCellInt(row + 1, 1, sinfo.time);	    //ʱ��
// 					opexf.SetCellFloat(row + 1, 2, sinfo.weight);	//����Ͱ����
// 					opexf.SetCellFloat(row + 1, 3, sinfo.lflow);	//˲ʱ����20mm
// 					opexf.SetCellFloat(row + 1, 4, sinfo.mflow);	//˲ʱ����6mm
// 					opexf.SetCellFloat(row + 1, 5, sinfo.inpessure);	//��ˮ��ѹ��
// 					opexf.SetCellFloat(row + 1, 6, sinfo.outpesure);	//��ˮ��ѹ��
// 					opexf.SetCellFloat(row + 1, 7, sinfo.intempr);		//��ˮ���¶�
// 					opexf.SetCellFloat(row + 1, 8, sinfo.outtempr);		//��ˮ���¶�
// 					opexf.SetCellFloat(row + 1, 9, sinfo.boxtempr);		//��ˮ���¶�
// 					opexf.SetCellFloat(row + 1, 10, Spercent);		    //��ǰƵ��ֵ
// 					opexf.SetCellFloat(row + 1, 11, vavlestate);		    //��ǰ���ط���Ϣ
// 					for(t_num = 0; t_num < sinfo.meternum; t_num++)		//������
// 					{
// 						opexf.SetCellInt(row + 1, 12 + t_num * 5, sinfo.minfo[t_num].ml);
// 						opexf.SetCellInt(row + 1, 13 + t_num * 5, sinfo.minfo[t_num].dt);
// 						opexf.SetCellInt(row + 1, 14 + t_num * 5, sinfo.minfo[t_num].tf);
// 						opexf.SetCellInt(row + 1, 15 + t_num * 5, sinfo.minfo[t_num].tmpr1);
// 						opexf.SetCellInt(row + 1, 16 + t_num * 5, sinfo.minfo[t_num].tmpr2);
// 					}				
// 					opexf.SaveExcel();					
					break;
				}
			case 2:
				{
					SampleType = 0x07;
					if (tick1)
					{						
						row = opexf.GetRowCount();						
						opexf.SetCellInt(row + 1, 1, sinfo.time);	    //ʱ��
						opexf.SetCellFloat(row + 1, 2, sinfo.weight);	//����Ͱ����
						opexf.SetCellFloat(row + 1, 3, sinfo.lflow);	//˲ʱ����20mm
						opexf.SetCellFloat(row + 1, 4, sinfo.mflow);	//˲ʱ����6mm
						opexf.SetCellFloat(row + 1, 5, sinfo.inpessure);	//��ˮ��ѹ��
						opexf.SetCellFloat(row + 1, 6, sinfo.outpesure);	//��ˮ��ѹ��
						opexf.SetCellFloat(row + 1, 7, sinfo.intempr);		//��ˮ���¶�
						opexf.SetCellFloat(row + 1, 8, sinfo.outtempr);		//��ˮ���¶�
						opexf.SetCellFloat(row + 1, 9, sinfo.boxtempr);		//��ˮ���¶�
						opexf.SetCellFloat(row + 1, 10, Spercent);		    //��ǰƵ��ֵ
						opexf.SetCellFloat(row + 1, 11, vavlestate);		    //��ǰ���ط���Ϣ
						for(t_num = 0; t_num < sinfo.meternum; t_num++)		//������
						{
							opexf.SetCellInt(row + 1, 12 + t_num * 5, sinfo.minfo[t_num].ml);
							opexf.SetCellInt(row + 1, 13 + t_num * 5, sinfo.minfo[t_num].dt);
							opexf.SetCellInt(row + 1, 14 + t_num * 5, sinfo.minfo[t_num].tf);
							opexf.SetCellInt(row + 1, 15 + t_num * 5, sinfo.minfo[t_num].tmpr1);
							opexf.SetCellInt(row + 1, 16 + t_num * 5, sinfo.minfo[t_num].tmpr2);
						}
						opexf.SaveExcel();
						////////////////b
						tick1--;
					}
					else				
						/*if(!(tick % 10))  //ʱ��Ϊ10�ı���ʱ*/
						///////////end
					{
						tick1 = 40;
						if(Spercent < 9000)
						{
							Spercent += 200;///////////////////
							if(!DanfossSet(Danfosshdev, &Spercent))
							{
								AfxMessageBox(_T("��Ƶ��дʧ��!")); 
								return;
							}
							else
							{				
								str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));								
							}
						}
						else
						{
							::KillTimer(m_hWnd,1);
							Spercent = 0;
							if(!DanfossSet(Danfosshdev, &Spercent))
							{
								AfxMessageBox(_T("��Ƶ��дʧ��!")); 
								return;
							}
							else
							{				
								str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
							}
							SampleType = 0;
							SampleFlag = FALSE;
							SetDlgItemText(IDC_BUTTON13,_T("Ƶ�ʵ�������"));
							((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(TRUE); 
							((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(TRUE);
							((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(TRUE); 
							((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(TRUE); 
							/*((CButton*)GetDlgItem(IDC_BUTTON16))->EnableWindow(TRUE);*/
							((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(TRUE);
						}
					}
					SampleType = 0x07;
					break;
				}
			case 3://30�����
				{
					SampleType = 0x07;
					if(tick == 10)
					{
						Spercent -= 100;
						if(!DanfossSet(Danfosshdev, &Spercent))
						{
							AfxMessageBox(_T("��Ƶ��дʧ��!")); 
							return;
						}
						else
						{				
							str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
						}
					}
					else if((tick > 19) && (tick < 30))
					{
						row = opexf.GetRowCount();		
						opexf.SetCellInt(row + 1, 1, sinfo.time);		//ʱ��
						opexf.SetCellFloat(row + 1, 2, sinfo.weight);	//����Ͱ����
						opexf.SetCellFloat(row + 1, 3, sinfo.lflow);	//˲ʱ����20mm
						opexf.SetCellFloat(row + 1, 4, sinfo.mflow);	//˲ʱ����6mm
						opexf.SetCellFloat(row + 1, 5, sinfo.inpessure);	//��ˮ��ѹ��
						opexf.SetCellFloat(row + 1, 6, sinfo.outpesure);	//��ˮ��ѹ��
						opexf.SetCellFloat(row + 1, 7, sinfo.intempr);		//��ˮ���¶�
						opexf.SetCellFloat(row + 1, 8, sinfo.outtempr);		//��ˮ���¶�
						opexf.SetCellFloat(row + 1, 9, sinfo.boxtempr);		//��ˮ���¶�
						opexf.SetCellFloat(row + 1, 10, Spercent);		    //��ǰƵ��ֵ
						opexf.SetCellFloat(row + 1, 11, vavlestate);		    //��ǰ���ط���Ϣ
						for(t_num = 0; t_num < sinfo.meternum; t_num++)		//������
						{
							opexf.SetCellInt(row + 1, 12 + t_num * 5, sinfo.minfo[t_num].ml);
							opexf.SetCellInt(row + 1, 13 + t_num * 5, sinfo.minfo[t_num].dt);
							opexf.SetCellInt(row + 1, 14 + t_num * 5, sinfo.minfo[t_num].tf);
							opexf.SetCellInt(row + 1, 15 + t_num * 5, sinfo.minfo[t_num].tmpr1);
							opexf.SetCellInt(row + 1, 16 + t_num * 5, sinfo.minfo[t_num].tmpr2);
						}
						opexf.SaveExcel();
					}
					else if(tick == 30)
					{
						if(Spercent < 9000)
						{
							tick = 0;
							tick30_n ++;
							Spercent += 400;  
							if(!DanfossSet(Danfosshdev, &Spercent))
							{
								AfxMessageBox(_T("��Ƶ��дʧ��!")); 
								return;
							}
							else
							{				
								str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
							}
						}
						else
						{
							::KillTimer(m_hWnd,1);
							Spercent = 0;
							if(!DanfossSet(Danfosshdev, &Spercent))
							{
								AfxMessageBox(_T("��Ƶ��дʧ��!")); 
								return;
							}
							else
							{				
								str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
							}
							SampleType = 0;
							SampleFlag = FALSE;
							SampleFlagHZ1 = 0;
							SetDlgItemText(IDC_BUTTON14,_T("30�����"));
							((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(TRUE); 
							((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(TRUE);
							((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(TRUE); 
							((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(TRUE);
							/*((CButton*)GetDlgItem(IDC_BUTTON16))->EnableWindow(TRUE);*/
							((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(TRUE);
						}
					}
					break;
				}
			case 4://У����Լ���
				{
					SampleType = 0x07;
					if(RecordFlag)
					{
						tick1++;
						row = opexf.GetRowCount();				
						opexf.SetCellInt(row + 1, 1, sinfo.time);		//ʱ��
						opexf.SetCellFloat(row + 1, 2, sinfo.weight);	//����Ͱ����
						opexf.SetCellFloat(row + 1, 3, sinfo.lflow);	//˲ʱ����20mm
						opexf.SetCellFloat(row + 1, 4, sinfo.mflow);	//˲ʱ����6mm
						opexf.SetCellFloat(row + 1, 5, sinfo.inpessure);	//��ˮ��ѹ��
						opexf.SetCellFloat(row + 1, 6, sinfo.outpesure);	//��ˮ��ѹ��
						opexf.SetCellFloat(row + 1, 7, sinfo.intempr);		//��ˮ���¶�
						opexf.SetCellFloat(row + 1, 8, sinfo.outtempr);		//��ˮ���¶�
						opexf.SetCellFloat(row + 1, 9, sinfo.boxtempr);		//��ˮ���¶�
						opexf.SetCellFloat(row + 1, 10, Spercent);		    //��ǰƵ��ֵ
						opexf.SetCellFloat(row + 1, 11, vavlestate);		    //��ǰ���ط���Ϣ
						for(t_num = 0; t_num < sinfo.meternum; t_num++)		//������
						{
							opexf.SetCellInt(row + 1, 12 + t_num * 5, sinfo.minfo[t_num].ml);
							opexf.SetCellInt(row + 1, 13 + t_num * 5, sinfo.minfo[t_num].dt);
							opexf.SetCellInt(row + 1, 14 + t_num * 5, sinfo.minfo[t_num].tf);
							opexf.SetCellInt(row + 1, 15 + t_num * 5, sinfo.minfo[t_num].tmpr1);
							opexf.SetCellInt(row + 1, 16 + t_num * 5, sinfo.minfo[t_num].tmpr2);
						}
						opexf.SaveExcel();
						if(tick1 == 5)
						{
							WaitForSingleObject(xxMutex, INFINITE);
							RecordFlag = FALSE;
							lflowmap.clear();

							flowcount = 1;
							ReleaseMutex(xxMutex);
							tick1 = 0;
							if (!PIDFlag)
							{
								Spercent += hzpercent; 
								if(Spercent < 9000)
								{
									if(!DanfossSet(Danfosshdev, &Spercent))
									{
										AfxMessageBox(_T("��Ƶ��дʧ��!")); 
										return;
									}
									else
									{				
										str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
									}
								}
								else
								{
									::KillTimer(m_hWnd,1);
									Spercent = 0;
									if(!DanfossSet(Danfosshdev, &Spercent))
									{
										AfxMessageBox(_T("��Ƶ��дʧ��!")); 
										return;
									}
									else
									{				
										str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
									}
									SampleType = 0;
									SampleFlag = FALSE;
									RecordFlag = FALSE;
									SetDlgItemText(IDC_BUTTON15,_T("��ʼУ��"));
									((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(TRUE); 
									((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(TRUE);
									((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(TRUE); 
									((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(TRUE);
									/*((CButton*)GetDlgItem(IDC_BUTTON16))->EnableWindow(TRUE);*/
									((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(TRUE);
								}
							}
							else//�����PIDУ��
							{
								pid.setflowrate += PID_dflow;								
							}
						}
					}					
					break;
				}
			case 6://��Ƶ����
				{
					SampleType = 0x07;
					if(RecordFlag)
					{
						if(tick1)
						{
							row = opexf.GetRowCount();						
							opexf.SetCellInt(row + 1, 1, sinfo.time);	    //ʱ��
							opexf.SetCellFloat(row + 1, 2, sinfo.weight);	//����Ͱ����
							opexf.SetCellFloat(row + 1, 3, sinfo.lflow);	//˲ʱ����20mm
							opexf.SetCellFloat(row + 1, 4, sinfo.mflow);	//˲ʱ����6mm
							opexf.SetCellFloat(row + 1, 5, sinfo.inpessure);	//��ˮ��ѹ��
							opexf.SetCellFloat(row + 1, 6, sinfo.outpesure);	//��ˮ��ѹ��
							opexf.SetCellFloat(row + 1, 7, sinfo.intempr);		//��ˮ���¶�
							opexf.SetCellFloat(row + 1, 8, sinfo.outtempr);		//��ˮ���¶�
							opexf.SetCellFloat(row + 1, 9, sinfo.boxtempr);		//��ˮ���¶�
							opexf.SetCellFloat(row + 1, 10, Spercent);		    //��ǰƵ��ֵ
							opexf.SetCellFloat(row + 1, 11, vavlestate);		    //��ǰ���ط���Ϣ
							for(t_num = 0; t_num < sinfo.meternum; t_num++)		//������
							{
								opexf.SetCellInt(row + 1, 12 + t_num * 5, sinfo.minfo[t_num].ml);
								opexf.SetCellInt(row + 1, 13 + t_num * 5, sinfo.minfo[t_num].dt);
								opexf.SetCellInt(row + 1, 14 + t_num * 5, sinfo.minfo[t_num].tf);
								opexf.SetCellInt(row + 1, 15 + t_num * 5, sinfo.minfo[t_num].tmpr1);
								opexf.SetCellInt(row + 1, 16 + t_num * 5, sinfo.minfo[t_num].tmpr2);
							}				
							opexf.SaveExcel();
							tick1--;
						}
						else
						{
							RecordFlag = FALSE;
							tick1 = 60;
							hz_pt++;
						}					
					}
					
					break;
				}
			case 7:
				{
					if (ErrTest_pt > calcnum )
					{
						SetDlgItemText(IDC_EDIT4,str_err);
// 
// 						ErrRcrdFile.InitExcel();
// 						ErrRcrdFile.OpenWithoutSheet(_T("\\ErrRecord.xls"));
// 						ErrRcrdFile.WriteTableHead(sinfo.meternum, sinfo.minfo);
// 						ErrRcrdFile.SaveExcel();
						ErrRcrdFile.InitExcel();
						ErrRcrdFile.OpenWithoutSheet(_T("\\ErrRecord.xls"));
						ErrRcrdFile.LoadSheet(1,TRUE);

						row = ErrRcrdFile.GetRowCount();
						ErrRcrdFile.SetCellInt(  row + 1,1,sinfoend.time - sinfotmp.time);
						ErrRcrdFile.SetCellFloat(row + 1,2,sinfotmp.lflow + sinfotmp.mflow);
						sinfotmp.lflow = 0;
						sinfotmp.mflow = 0;
						ErrRcrdFile.SetCellFloat(row + 1,3,sinfotmp.weight);
						ErrRcrdFile.SetCellFloat(row + 1,4,sinfoend.weight);
						ErrRcrdFile.SetCellFloat(row + 1,5,sinfoend.weight - sinfotmp.weight);
						for (i = 0;i < sinfo.meternum;i++)
						{
// 							sinfotmp.minfo[i].errnum = ((sinfoend.minfo[i].total_f - sinfotmp.minfo[i].total_f) / 1000.0 -\
// 								(sinfoend.weight - sinfotmp.weight)) / (sinfoend.weight - sinfotmp.weight) * 100;
							
							ErrRcrdFile.SetCellFloat(row + 1,6  + i * 5,sinfotmp.minfo[i].total_f);
							ErrRcrdFile.SetCellFloat(row + 1,7  + i * 5,sinfoend.minfo[i].total_f);
							ErrRcrdFile.SetCellFloat(row + 1,8  + i * 5,sinfoend.minfo[i].total_f - sinfotmp.minfo[i].total_f);
							ErrRcrdFile.SetCellFloat(row + 1,9  + i * 5,sinfotmp.minfo[i].errnum);
							ErrRcrdFile.SetCellFloat(row + 1,10 + i * 5,sinfoend.minfo[i].tmpr1);
						}
						ErrRcrdFile.SaveExcel();
						ErrRcrdFile.CloseExcelFile();
						ErrRcrdFile.ReleaseExcel();
						calcnum ++;
					}
					if (tick1)
					{
						tick1--;
					}
					SampleType = 0x03;
					break;
				}
			default:
				SampleType = 0x07;
				break;
			}	
		}			
		else
		{
			SampleType = 0x03;	
		}

		if(sinfo.inpessure > 500)
		{
			Spercent = 1000;
			if(!DanfossSet(Danfosshdev, &Spercent))
			{
				AfxMessageBox(_T("��Ƶ��дʧ��!")); 
			}
			else
			{				
				str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
			}
		}
		//��ʱ�޸�����ʾȫ������
// 		GetDlgItemText(IDC_EDIT3, str);
// 		t_str.Format(_T("%s%d'%d:%s%.3f;%s%.2f;%s%.2f;%s%.2f;%s%.2f%s"),_T("����ʱ��"),(int)((tick + tick30_n * 30) / 60),\
// 			(tick + tick30_n * 30) % 60,_T("����Ͱ:"),sinfo.weight,_T("������:"),sinfo.lflow, _T("С����:"),sinfo.mflow,\
// 			_T("��ѹ��:"), sinfo.inpessure, _T("��ѹ��:"), sinfo.outpesure, _T("\r\n"));
// 		str += t_str;
// 		SetDlgItemText(IDC_EDIT3, str);

// 		i = ((CEdit*)GetDlgItem(IDC_EDIT3))->GetLineCount();
// 		((CEdit*)GetDlgItem(IDC_EDIT3))->LineScroll(i,0);
		str.Format(_T("%.2f"),sinfo.lflow + sinfo.mflow);//��ʾ����
		SetDlgItemText(IDC_EDIT15, str);
		str.Format(_T("%.3f"),sinfo.weight);//��ʾ����Ͱ����
		SetDlgItemText(IDC_EDIT22, str);
		SetDlgItemInt(IDC_EDIT25, tick_second );//���

// 		short t_percent;
// 		float t_hz;
// 
// 		if(!DAnfossRead(Danfosshdev, &t_percent))
// 		{
// 			//AfxMessageBox(_T("��Ƶ����ʧ��!")); 
// 			return;
// 		}
// 		t_hz = STDHZ * t_percent / 10000.0;	
// 		str.Format(_T("%.1f"),(float)t_hz);
     	SetDlgItemText(IDC_EDIT1, str_hz);
	}
}
//��ʼ������
void CMeasureCommdemoDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DCB dcb;
	CString com;
	int item;
	int status;
	DWORD w;
	COMMTIMEOUTS timeout;
	CString str_com;

	if (!InitFlag)
	{
		//plc
		//item = m_combo1.GetCurSel();
		//m_combo1.GetLBText(item, com);
		com = "COM1";
		PLChdev = CreateFile(com, GENERIC_READ|GENERIC_WRITE, NULL,NULL,
		OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if(PLChdev == INVALID_HANDLE_VALUE)
		{
		AfxMessageBox(_T("��PLCͨ�Ŵ���ʧ��!")); 
		return;
		}

		status = SetupComm(PLChdev,128,128); //������
		GetCommMask(PLChdev,&w);
		SetCommMask(PLChdev, EV_RXCHAR|EV_TXEMPTY);//�����¼�����������
		GetCommState(PLChdev, &dcb);
		BuildCommDCB(_T("9600,n,8,1 "), &dcb);
		SetCommState(PLChdev, &dcb);

		PurgeComm(PLChdev, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���

		//danfoss��Ƶ��
		//item = m_combo6.GetCurSel();
		//m_combo6.GetLBText(item, com);
		com = "COM2";
		Danfosshdev = CreateFile(com, GENERIC_READ|GENERIC_WRITE, NULL,NULL,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if(Danfosshdev == INVALID_HANDLE_VALUE)
		{
			AfxMessageBox(_T("�򿪱�Ƶ��ͨ�Ŵ���ʧ��!")); 
			return;
		}

		status = SetupComm(Danfosshdev,128,128); //������
		SetCommMask(Danfosshdev, EV_RXCHAR|EV_TXEMPTY);//�����¼�����������
		GetCommState(Danfosshdev, &dcb);
		BuildCommDCB(_T("9600,e,8,1 "), &dcb);
		SetCommState(Danfosshdev, &dcb);

		PurgeComm(Danfosshdev, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���

		//weight
		//item = m_combo2.GetCurSel();
		//m_combo2.GetLBText(item, com);
		com = "COM3";
		Weighthdev = CreateFile(com, GENERIC_READ|GENERIC_WRITE, NULL,NULL,
		OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if(Weighthdev == INVALID_HANDLE_VALUE)
		{
		AfxMessageBox(_T("�򿪳���Ͱͨ�Ŵ���ʧ��!")); 
		return;
		}

		//�����ֽ���������
		GetCommTimeouts(Weighthdev, &timeout);
		timeout.ReadIntervalTimeout = MAXDWORD;
		timeout.ReadTotalTimeoutConstant = MAXDWORD;
		timeout.ReadTotalTimeoutConstant = 0;
		SetCommTimeouts(Weighthdev, &timeout);

		SetupComm(Weighthdev,128,128); //������
		SetCommMask(Weighthdev, EV_RXCHAR|EV_TXEMPTY );//�����¼�����������
		GetCommState(Weighthdev, &dcb);
		BuildCommDCB(_T("9600,n,8,1 "), &dcb);
		SetCommState(Weighthdev, &dcb);

		PurgeComm(Weighthdev, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���		

		//�����ƴ���
		com = "COM9";
		Flowhdev = CreateFile(com, GENERIC_READ|GENERIC_WRITE, NULL,NULL,
		OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if(Flowhdev == INVALID_HANDLE_VALUE)
		{
		AfxMessageBox(_T("��������ͨ�Ŵ���ʧ��!")); 
		return;
		}

		status = SetupComm(Flowhdev,128,128); //������
		SetCommMask(Flowhdev, EV_RXCHAR|EV_TXEMPTY);//�����¼�����������
		GetCommState(Flowhdev, &dcb);
		BuildCommDCB(_T("115200,e,8,1 "), &dcb);
		SetCommState(Flowhdev, &dcb);

		PurgeComm(Flowhdev, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���

		//����
		//item = m_combo3.GetCurSel();
		//m_combo3.GetLBText(item, wcom);
// 		com = "COM6";
// 		Meterhdev = CreateFile(com, GENERIC_READ|GENERIC_WRITE, NULL,NULL,
// 		OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
// 		if(Meterhdev == INVALID_HANDLE_VALUE)
// 		{
// 		AfxMessageBox(_T("�򿪱�ͨ�Ŵ���ʧ��!")); 
// 		return;
// 		}
// 
// 		status = SetupComm(Meterhdev,128,128); //������
// 		SetCommMask(Meterhdev, EV_RXCHAR|EV_TXEMPTY);//�����¼�����������
// 		GetCommState(Meterhdev, &dcb);
// 		BuildCommDCB(_T("115200,e,8,1 "), &dcb);//ԭ115200
// 		SetCommState(Meterhdev, &dcb);
// 
// 		PurgeComm(Meterhdev, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���		
		
		//�������ڵı�
// 		item = m_combo3.GetCurSel();
// 		m_combo3.GetLBText(item, com);
// 		com = "\\\\.\\COM10";
// 		if (com == "COM0")
// 		{
// 			AfxMessageBox(_T("��1�޴�������"));
// 		} 
// 		else
// 		{
// 			Meterhdev_com10 = CreateFile(com, GENERIC_READ|GENERIC_WRITE, 0,NULL,
// 				OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
// 			if(Meterhdev_com10 == INVALID_HANDLE_VALUE)
// 			{
// 				AfxMessageBox(_T("�򿪱�1ͨ�Ŵ���ʧ��!")); 
// 				return;
// 			}
// 
// 			status = SetupComm(Meterhdev_com10,128,128); //������
// 			GetCommMask(Meterhdev_com10,&w);
// 			SetCommMask(Meterhdev_com10, EV_RXCHAR|EV_TXEMPTY);//�����¼�����������
// 			GetCommState(Meterhdev_com10, &dcb);
// 			BuildCommDCB(_T("115200,e,8,1 "), &dcb);
// 			SetCommState(Meterhdev_com10, &dcb);
// 
// 			PurgeComm(Meterhdev_com10, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���
// 		}

//1111
		com = "\\\\.\\COM21";
		Meterhdev_com21 = CreateFile(com, GENERIC_READ|GENERIC_WRITE, 0,NULL,
				OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if(Meterhdev_com21 == INVALID_HANDLE_VALUE)
			{
				AfxMessageBox(_T("�򿪱�1ͨ�Ŵ���ʧ��!")); 
				return;
			}
		status = SetupComm(Meterhdev_com21,128,128); //������
		GetCommMask(Meterhdev_com21,&w);
		SetCommMask(Meterhdev_com21, EV_RXCHAR|EV_TXEMPTY);//�����¼�����������
		GetCommState(Meterhdev_com21, &dcb);
		BuildCommDCB(_T("115200,e,8,1 "), &dcb);
		SetCommState(Meterhdev_com21, &dcb);

		PurgeComm(Meterhdev_com21, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���
//2222	
		com = "\\\\.\\COM22";
		Meterhdev_com22 = CreateFile(com, GENERIC_READ|GENERIC_WRITE, 0,NULL,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if(Meterhdev_com22 == INVALID_HANDLE_VALUE)
		{
			AfxMessageBox(_T("�򿪱�2ͨ�Ŵ���ʧ��!")); 
			return;
		}
		status = SetupComm(Meterhdev_com22,128,128); //������
		GetCommMask(Meterhdev_com22,&w);
		SetCommMask(Meterhdev_com22, EV_RXCHAR|EV_TXEMPTY);//�����¼�����������
		GetCommState(Meterhdev_com22, &dcb);
		BuildCommDCB(_T("115200,e,8,1 "), &dcb);
		SetCommState(Meterhdev_com22, &dcb);

		PurgeComm(Meterhdev_com22, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���
//3333
		com = "\\\\.\\COM23";
		Meterhdev_com23 = CreateFile(com, GENERIC_READ|GENERIC_WRITE, 0,NULL,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if(Meterhdev_com23 == INVALID_HANDLE_VALUE)
		{
			AfxMessageBox(_T("�򿪱�3ͨ�Ŵ���ʧ��!")); 
			return;
		}
		status = SetupComm(Meterhdev_com23,128,128); //������
		GetCommMask(Meterhdev_com23,&w);
		SetCommMask(Meterhdev_com23, EV_RXCHAR|EV_TXEMPTY);//�����¼�����������
		GetCommState(Meterhdev_com23, &dcb);
		BuildCommDCB(_T("115200,e,8,1 "), &dcb);
		SetCommState(Meterhdev_com23, &dcb);

		PurgeComm(Meterhdev_com23, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���
//4444
		com = "\\\\.\\COM24";
		Meterhdev_com24 = CreateFile(com, GENERIC_READ|GENERIC_WRITE, 0,NULL,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if(Meterhdev_com24 == INVALID_HANDLE_VALUE)
		{
			AfxMessageBox(_T("�򿪱�4ͨ�Ŵ���ʧ��!")); 
			return;
		}
		status = SetupComm(Meterhdev_com24,128,128); //������
		GetCommMask(Meterhdev_com24,&w);
		SetCommMask(Meterhdev_com24, EV_RXCHAR|EV_TXEMPTY);//�����¼�����������
		GetCommState(Meterhdev_com24, &dcb);
		BuildCommDCB(_T("115200,e,8,1 "), &dcb);
		SetCommState(Meterhdev_com24, &dcb);

		PurgeComm(Meterhdev_com24, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���
//5555
		com = "\\\\.\\COM25";
		Meterhdev_com25 = CreateFile(com, GENERIC_READ|GENERIC_WRITE, 0,NULL,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if(Meterhdev_com25 == INVALID_HANDLE_VALUE)
		{
			AfxMessageBox(_T("�򿪱�5ͨ�Ŵ���ʧ��!")); 
			return;
		}
		status = SetupComm(Meterhdev_com25,128,128); //������
		GetCommMask(Meterhdev_com25,&w);
		SetCommMask(Meterhdev_com25, EV_RXCHAR|EV_TXEMPTY);//�����¼�����������
		GetCommState(Meterhdev_com25, &dcb);
		BuildCommDCB(_T("115200,e,8,1 "), &dcb);
		SetCommState(Meterhdev_com25, &dcb);

		PurgeComm(Meterhdev_com25, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���
//6666
		com = "\\\\.\\COM26";
		Meterhdev_com26 = CreateFile(com, GENERIC_READ|GENERIC_WRITE, 0,NULL,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if(Meterhdev_com26 == INVALID_HANDLE_VALUE)
		{
			AfxMessageBox(_T("�򿪱�6ͨ�Ŵ���ʧ��!")); 
			return;
		}
		status = SetupComm(Meterhdev_com26,128,128); //������
		GetCommMask(Meterhdev_com26,&w);
		SetCommMask(Meterhdev_com26, EV_RXCHAR|EV_TXEMPTY);//�����¼�����������
		GetCommState(Meterhdev_com26, &dcb);
		BuildCommDCB(_T("115200,e,8,1 "), &dcb);
		SetCommState(Meterhdev_com26, &dcb);

		PurgeComm(Meterhdev_com26, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���
//7777
		com = "\\\\.\\COM27";
		Meterhdev_com27 = CreateFile(com, GENERIC_READ|GENERIC_WRITE, 0,NULL,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if(Meterhdev_com27 == INVALID_HANDLE_VALUE)
		{
			AfxMessageBox(_T("�򿪱�7ͨ�Ŵ���ʧ��!")); 
			return;
		}
		status = SetupComm(Meterhdev_com27,128,128); //������
		GetCommMask(Meterhdev_com27,&w);
		SetCommMask(Meterhdev_com27, EV_RXCHAR|EV_TXEMPTY);//�����¼�����������
		GetCommState(Meterhdev_com27, &dcb);
		BuildCommDCB(_T("115200,e,8,1 "), &dcb);
		SetCommState(Meterhdev_com27, &dcb);

		PurgeComm(Meterhdev_com27, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���
//8888
		com = "\\\\.\\COM28";
		Meterhdev_com28 = CreateFile(com, GENERIC_READ|GENERIC_WRITE, 0,NULL,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if(Meterhdev_com28 == INVALID_HANDLE_VALUE)
		{
			AfxMessageBox(_T("�򿪱�8ͨ�Ŵ���ʧ��!")); 
			return;
		}
		status = SetupComm(Meterhdev_com28,128,128); //������
		GetCommMask(Meterhdev_com28,&w);
		SetCommMask(Meterhdev_com28, EV_RXCHAR|EV_TXEMPTY);//�����¼�����������
		GetCommState(Meterhdev_com28, &dcb);
		BuildCommDCB(_T("115200,e,8,1 "), &dcb);
		SetCommState(Meterhdev_com28, &dcb);

		PurgeComm(Meterhdev_com28, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���
//9999
		com = "\\\\.\\COM29";
		Meterhdev_com29 = CreateFile(com, GENERIC_READ|GENERIC_WRITE, 0,NULL,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if(Meterhdev_com29 == INVALID_HANDLE_VALUE)
		{
			AfxMessageBox(_T("�򿪱�9ͨ�Ŵ���ʧ��!")); 
			return;
		}
		status = SetupComm(Meterhdev_com29,128,128); //������
		GetCommMask(Meterhdev_com29,&w);
		SetCommMask(Meterhdev_com29, EV_RXCHAR|EV_TXEMPTY);//�����¼�����������
		GetCommState(Meterhdev_com29, &dcb);
		BuildCommDCB(_T("115200,e,8,1 "), &dcb);
		SetCommState(Meterhdev_com29, &dcb);

		PurgeComm(Meterhdev_com29, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���
//1010
		com = "\\\\.\\COM30";
		Meterhdev_com30 = CreateFile(com, GENERIC_READ|GENERIC_WRITE, 0,NULL,
			OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
		if(Meterhdev_com30 == INVALID_HANDLE_VALUE)
		{
			AfxMessageBox(_T("�򿪱�10ͨ�Ŵ���ʧ��!")); 
			return;
		}
		status = SetupComm(Meterhdev_com30,128,128); //������
		GetCommMask(Meterhdev_com30,&w);
		SetCommMask(Meterhdev_com30, EV_RXCHAR|EV_TXEMPTY);//�����¼�����������
		GetCommState(Meterhdev_com30, &dcb);
		BuildCommDCB(_T("115200,e,8,1 "), &dcb);
		SetCommState(Meterhdev_com30, &dcb);

		PurgeComm(Meterhdev_com30, PURGE_TXCLEAR | PURGE_RXCLEAR);//��ջ���


		//�����߳�
		plcMutex = CreateMutex(NULL, FALSE, NULL);
		xxMutex = CreateMutex(NULL, FALSE, NULL);
		PLCthread = AfxBeginThread(ReadPLCThread, NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		Weightthread = AfxBeginThread(ReadWight, NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		FlowThread = AfxBeginThread(ReadFlow,  NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		PIDThread = AfxBeginThread(PIDControl,NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		ReadMeterThread = AfxBeginThread(ReadMeter, NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
		SetDlgItemText(IDC_BUTTON1,_T("�رմ���"));
		InitFlag = TRUE;
		//((CButton*)GetDlgItem(IDC_BUTTON1))->EnableWindow(FALSE); 
		PIDThread->SuspendThread();	
	} 
	else
	{	
		PIDThread->SuspendThread();
		PLCthread->SuspendThread();
		Weightthread->SuspendThread();		
		FlowThread->SuspendThread();
		ReadMeterThread->SuspendThread();

		PIDThread->Delete();
		PLCthread->Delete();
		Weightthread->Delete();				
		FlowThread->Delete();		
		ReadMeterThread->Delete();
// 		���߳̽���UI�̵߳Ĵ���  
// 						if(PLCthread)   
// 						{  
// 							// 1. ��һ��WM_QUIT����Ϣ�ᡡUI���߳�  ;
// 							PLCthread->PostThreadMessage(WM_QUIT, NULL, NULL); 
// 							// 2. �ȴ���UI���߳������˳� ; 
// 							WaitForSingleObject(plcMutex, INFINITE); 
// 							//{
// 							// 3. ɾ�� UI �̶߳���ֻ�е���������m_bAutoDelete = FALSE;��ʱ�ŵ���;
// 							 delete   PLCthread;
// 							 ReleaseMutex(plcMutex);
// 						}
// 								if(Weightthread)   
// 								{  
// 									Weightthread->PostThreadMessage(WM_QUIT, NULL, NULL);
// 									//Weightthread = AfxEndThread(ReadWight, NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
// 									
// 						 			 WaitForSingleObject(Weightthread->m_hThread, INFINITE);  
// 						// 			{
// 						 				delete   Weightthread;
// 						// 			} 
// 								} 
// 								if(FlowThread)   
// 								{  
// 									FlowThread->PostThreadMessage(WM_QUIT, NULL, NULL);
// 									FlowThread = AfxEndThread(ReadFlow,  NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
// 									
// 									if (WAIT_OBJECT_0 == WaitForSingleObject(FlowThread, INFINITE))  
// 									{
// 										delete   FlowThread;
// 									} 
// 								}
// 								if(PIDThread)   
// 								{  
// 									PIDThread->PostThreadMessage(WM_QUIT, NULL, NULL);
// 									PIDThread = AfxEndThread(PIDControl,NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
// 									
// 									if (WAIT_OBJECT_0 == WaitForSingleObject(PIDThread->m_hThread, INFINITE))  
// 									{
// 						 				delete   PIDThread;
// 									} 
// 								}
// 								if(ReadMeterThread)   
// 								{  
// 									ReadMeterThread->PostThreadMessage(WM_QUIT, NULL, NULL);
// 									ReadMeterThread = AfxEndThread(ReadMeter, NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
// 									if (WAIT_OBJECT_0 == WaitForSingleObject(ReadMeterThread->m_hThread, INFINITE))  
// 									{
// 										delete   ReadMeterThread;
// 									} 
// 								}

		ExitComm((int)PLChdev);
		ExitComm((int)Danfosshdev);	
		ExitComm((int)Weighthdev);		
		ExitComm((int)Flowhdev);
		/*ExitComm((int)Meterhdev);*/

		ExitComm((int)Meterhdev_com21);
		ExitComm((int)Meterhdev_com22);
		ExitComm((int)Meterhdev_com23);
		ExitComm((int)Meterhdev_com24);
		ExitComm((int)Meterhdev_com25);
		ExitComm((int)Meterhdev_com26);
		ExitComm((int)Meterhdev_com27);
		ExitComm((int)Meterhdev_com28);
		ExitComm((int)Meterhdev_com29);
		ExitComm((int)Meterhdev_com30);

		sinfo.meternum = 0;
		InitFlag = FALSE;
		SetDlgItemText(IDC_BUTTON1,_T("��ʼ������"));
	}
	
}
//PID�����߳�
UINT PIDControl(LPVOID pParam)
{
	int i = 0;
	while (1)
	{ 		
		if (i % 2)
		{
			pid.Hz = PID_realized(pid.setflowrate);
			Spercent += pid.Hz;
			if (Spercent < 0 )
			{
				Spercent = 0;
				if(!DanfossSet(Danfosshdev, &Spercent))
				{
					AfxMessageBox(_T("��Ƶ��дʧ��!")); 
				}
				else
				{				
					str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
				}
			}
			else if(Spercent < 9000)
			{
				if(!DanfossSet(Danfosshdev, &Spercent))
				{
					AfxMessageBox(_T("��Ƶ��дʧ��!")); 
				}
				else
				{				
					str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
				}
			}
			else
			{
				Spercent = 8800;
				if(!DanfossSet(Danfosshdev, &Spercent))
				{
					AfxMessageBox(_T("��Ƶ��дʧ��!")); 
				}
				else
				{				
					str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
				}
			}
			i = 0;
			Sleep(1000);
		}
		else
		{
			i++;
			Sleep(1000);
		}		
	}		
}
//��PLC�߳�
UINT ReadPLCThread(LPVOID pParam)
{
	PLCFrame pf;
	u8 tmp_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	memset(&pf, 0, sizeof(PLCFrame));

	while(1)
	{
		if((SampleType & 0x01))
		{
			WaitForSingleObject(plcMutex, INFINITE);
			/*
			//��˲ʱ����
			Sleep(4);
			PLCTransfer(PLChdev, READREG, LFLOWCOUNT, tmp_data, &pf);
			if(pf.result == 1)
			{
			sinfo.lflow = EndianConvert32(pf.data);
			pf.result = 0;
			}
			else
			{
			sinfo.lflow = 0;
			}
			Sleep(4);
			PLCTransfer(PLChdev, READREG, MFLOWCOUNT, tmp_data, &pf);
			if(pf.result == 1)
			{
			sinfo.mflow = EndianConvert32(pf.data);
			pf.result = 0;
			}
			else
			{
			sinfo.mflow = 0;
			}
			*/
			Sleep(4);
			//����ˮ��ѹ��
			PLCTransfer(PLChdev, READREG, INPRESSURE, tmp_data, &pf);
			if(pf.result)
			{
				sinfo.inpessure = EndianConvert32(pf.data);
				sinfo.inpessure += 0/*20.354*/;
				pf.result = 0;
			}
			else
			{
				sinfo.inpessure -= 1/*0*/;
			}
			Sleep(4);
			//����ˮ��ѹ��
			PLCTransfer(PLChdev, READREG, OUTPRESSURE, tmp_data, &pf);
			if(pf.result == 1)
			{
				sinfo.outpesure = EndianConvert32(pf.data);
				sinfo.outpesure += 0.1;
				pf.result = 0;
			}
			else
			{
				sinfo.outpesure = 0;
			}
			Sleep(4);
			//����ˮ���¶�
			PLCTransfer(PLChdev, READREG, INTEMPR, tmp_data, &pf);
			if(pf.result == 1)
			{
				sinfo.intempr = EndianConvert32(pf.data);
				pf.result = 0;
			}
			else
			{
				sinfo.intempr = 0;
			}
			Sleep(4);
			//����ˮ���¶�
			PLCTransfer(PLChdev, READREG, OUTTEMPR, tmp_data, &pf);
			if(pf.result == 1)
			{
				sinfo.outtempr = EndianConvert32(pf.data);
				pf.result = 0;
			}
			else
			{
				sinfo.outtempr = 0;
			}
			Sleep(4);
			//����ˮ����¶�
			PLCTransfer(PLChdev, READREG, BOXTEMPR, tmp_data, &pf);
			if(pf.result == 1)
			{
				sinfo.boxtempr = EndianConvert32(pf.data);
				pf.result = 0;
			}
			else
			{
				sinfo.boxtempr = 0;
			}
			SampleType ^= 0x01;
			ReleaseMutex(plcMutex);
		}
		Sleep(500);
	}
	return 0;
}
//������Ͱ�߳�
UINT ReadWight(LPVOID pParam)
{
	time_t time_last;
	time_t time_current;
	char t_buffer[64];
	int i;
	int rlen;

	time(&time_current); 
	time_last = time_current;

	while(1)
	{
		rlen = commRead(Weighthdev, (unsigned char*)t_buffer, 64,100);
		if(rlen)
		{
			for(i = 0;i < rlen; i++)
			{
				if(((t_buffer[i] == '+') || (t_buffer[i] == '-')) && (rlen - i) < 31)
					break;
			}
			time(&time_current);
			if(time_current == (time_last + 1))
			{
				sinfo.weight = strtod(&(t_buffer[i + 1]), NULL);
				sinfo.time = time_current;
				time_last = time_current;
				SampleType &= (~0x02);
			}
		}
		Sleep(50);
	}
	return 0;
}
//�������߳�
UINT ReadFlow(LPVOID pParam)
{
	PLCFrame pf;
	u8 tmp_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	memset(&pf, 0, sizeof(PLCFrame));
	typedef std::pair <int, float> Int_Pair;
	u8 cmd[6] = {0xfe,0xfe,0x68,0x80,0x20,0x16};
	u8 t_frame[13] = {0,};
	flowcount = 1;
	int i,j;
	BOOL DataFlag = FALSE;//����Ͱ�����ȶ�״̬

	while(1)
	{
		//��ȡ��������
		if(commWrite(Flowhdev, cmd, 6, 50))
		{
			Sleep(30);
			if(commRead(Flowhdev, t_frame, 13,200))
			{
				if(t_frame[11] == lrc(t_frame, 11))
				{
					t_f = *(int*)(t_frame + 3);
					if(t_f != 0)
					{
						sinfo.lflow = 1000000 / t_f * 3.6;
					}
					else
					{
						sinfo.lflow = 0;
					}
					t_f = *(int*)(t_frame + 7);
					if(t_f != 0)
					{
						sinfo.mflow = 1000000 / t_f * 3.6;
					}
					else
					{
						sinfo.mflow = 0;
					}
				}else//PLC������������У���ִ���
				{
					sinfo.mflow = 0;
					sinfo.lflow = 0;
				}
			}
			else//��PLC��ָ�������ʧ��
			{
				sinfo.mflow = 0;
				sinfo.lflow = 0;
			}
		}
		else//��PLC��ָ���ʧ��
		{
			sinfo.mflow = 0;
			sinfo.lflow = 0;
		}
		t_f = sinfo.lflow + sinfo.mflow;
		pid.acflowratepre = pid.actualflowrate;
		pid.actualflowrate = t_f;
		sinfotmp.lflow = max(sinfotmp.lflow,sinfo.lflow);
		sinfotmp.mflow = max(sinfotmp.mflow,sinfo.mflow);

		if((!RecordFlag) && (SampleFlagHZ1 == 4))
		{
			if (!PIDFlag)//�Զ���Ƶ�ʿ�������
			{
				if(t_f < 50)
				{
					if(vavlestate == 0x0)
					{
						tmp_data[3] = 1;
						Sleep(6);
						WaitForSingleObject(plcMutex, INFINITE);
						if(PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
						{
							vavlestate |= 0x01;
						}
						else
						{
							ReleaseMutex(plcMutex);
							continue;
						}
						ReleaseMutex(plcMutex);
						hzpercent = 20;
						flowoffset = 0.02;
					}
				}
				else if(t_f < 500)
				{
					if(vavlestate ==  0x01)
					{
						tmp_data[3] = 1;
						Sleep(6);
						WaitForSingleObject(plcMutex, INFINITE);
						if(PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
						{
							vavlestate |= 0x20;
						}
						else
						{
							ReleaseMutex(plcMutex);
							continue;
						}
						tmp_data[3] = 0;
						Sleep(6);
						if(!PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
						{
							vavlestate &= 0xfe;
						}
						ReleaseMutex(plcMutex);
						hzpercent = 50;
						flowoffset = 0.02;
					}
				}
				else if(t_f < 1000)
				{
					if(vavlestate == 0x02)
					{
						tmp_data[3] = 1;
						Sleep(6);
						WaitForSingleObject(plcMutex, INFINITE);
						if(PLCTransfer(PLChdev, WRITEOP, LMFLOWVAVLE, tmp_data, &pf))
						{
							vavlestate |= 0x04;	
						}
						else
						{
							ReleaseMutex(plcMutex);
							continue;
						}
						tmp_data[3] = 0;
						Sleep(6);
						if(!PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
						{
							vavlestate &= 0xfd;	
						}
						ReleaseMutex(plcMutex);
						hzpercent = 100;
						flowoffset = 0.02;
					}
				}
				else if(t_f < 2000)
				{
					tmp_data[3] = 1;
					if(vavlestate == 0x04)
					{
						Sleep(6);
						WaitForSingleObject(plcMutex, INFINITE);
						if(PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
						{
							vavlestate |= 0x02;
						}
						else
						{
							ReleaseMutex(plcMutex);
							continue;
						}
						ReleaseMutex(plcMutex);
						hzpercent = 100;
						flowoffset = 0.02;
					}
				}
				else if(t_f < 3000)
				{
					if(vavlestate == 0x06)
					{
						tmp_data[3] = 1;
						Sleep(6);
						WaitForSingleObject(plcMutex, INFINITE);
						if(PLCTransfer(PLChdev, WRITEOP, LFLOWVAVLE, tmp_data, &pf))
						{
							vavlestate |= 0x08;
						}
						else
						{
							ReleaseMutex(plcMutex);
							continue;
						}
						tmp_data[3] = 0;
						Sleep(6);
						if(!PLCTransfer(PLChdev, WRITEOP, LMFLOWVAVLE, tmp_data, &pf))
						{

						}
						else
						{
							vavlestate &= 0xfb;
						}
						tmp_data[3] = 0;
						Sleep(6);
						if(!PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
						{

						}
						else
						{
							vavlestate &= 0xfd;
						}
						ReleaseMutex(plcMutex);
						hzpercent = 100;
						flowoffset = 0.02;
					}
				}
				else if(t_f < 4000)
				{
					if(vavlestate == 0x08)
					{
						tmp_data[3] = 1;
						Sleep(6);
						WaitForSingleObject(plcMutex, INFINITE);
						if(PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
						{
							vavlestate |= 0x02;
						}
						else
						{
							ReleaseMutex(plcMutex);
							continue;
						}
						ReleaseMutex(plcMutex);
						hzpercent = 200;
						flowoffset = 0.02;
					}
				}
				else
				{
					if(vavlestate == 0x0A)
					{
						tmp_data[3] = 1;
						Sleep(6);
						WaitForSingleObject(plcMutex, INFINITE);
						if(PLCTransfer(PLChdev, WRITEOP, LMFLOWVAVLE, tmp_data, &pf))
						{
							vavlestate |= 0x04;
						}
						else
						{
							ReleaseMutex(plcMutex);
							continue;
						}
						ReleaseMutex(plcMutex);
						hzpercent = 200;
						flowoffset = 50;
					}
				}
				if(lflowmap.size() == 0)
				{
					Sleep(5000);
				}
				if(lflowmap.size() == 5)
				{
					lflowmap.erase(flowcount);
					lflowmap.insert(Int_Pair(flowcount, t_f));
					RecordFlag = compareflow(&lflowmap,PIDFlag);
				}
				else
				{
					lflowmap.insert(Int_Pair(flowcount, t_f));
					RecordFlag = false;
				}
				if(flowcount == 5)
				{
					flowcount = 0;
				}
				flowcount++;
			}
			else//PID���������ƿ���
			{
				if(pid.setflowrate < 61)
				{
					pid.KP = 0.2871;
					pid.KI = 1.4875;
					if(vavlestate == 0x0)
					{
						tmp_data[3] = 1;
						Sleep(6);
						WaitForSingleObject(plcMutex, INFINITE);
						if(PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
						{

							vavlestate |= 0x01;
						}
						else
						{
							ReleaseMutex(plcMutex);
							continue;
						}
						ReleaseMutex(plcMutex);
						PID_dflow = 5.0;
						flowoffset = 0.02;
					}
				}
				else if(pid.setflowrate < 650)
				{
					pid.KP = 0.2871;
					pid.KI = 1.4875;
					if(vavlestate == 0x01)
					{
						tmp_data[3] = 1;
						Sleep(6);
						WaitForSingleObject(plcMutex, INFINITE);
						if(PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
						{
							vavlestate |= 0x02;
						}
						else
						{
							ReleaseMutex(plcMutex);
							continue;
						}
						tmp_data[3] = 0;
						Sleep(50);
						if(!PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
						{

						}
						else
						{
							vavlestate &= (~0x01);
						}
						ReleaseMutex(plcMutex);
						PID_dflow =  50.0;
						flowoffset = 0.015;
					}
				}
				else if(pid.setflowrate < 2200)
				{
					pid.KP = 0.10187;
					pid.KI = 0.68014;
					if(vavlestate == 0x02)
					{
						tmp_data[3] = 1;
						Sleep(6);
						WaitForSingleObject(plcMutex, INFINITE);
						if(PLCTransfer(PLChdev, WRITEOP, LMFLOWVAVLE, tmp_data, &pf))
						{
							vavlestate |= (0x04);	
						}
						else
						{
							ReleaseMutex(plcMutex);
							continue;
						}

						ReleaseMutex(plcMutex);
						PID_dflow =  150.0;
						flowoffset = 0.01;
					}
				}
				else 
				{
					pid.KP = 0.10187;
					pid.KI = 0.68014;
					tmp_data[3] = 1;
					if(vavlestate == 0x06)
					{
						Sleep(6);
						WaitForSingleObject(plcMutex, INFINITE);
						if(PLCTransfer(PLChdev, WRITEOP, LFLOWVAVLE, tmp_data, &pf))
						{
							vavlestate |= 0x08;
						}
						else
						{
							ReleaseMutex(plcMutex);
							continue;
						}
						ReleaseMutex(plcMutex);
						PID_dflow =  200.0;
						flowoffset = 0.006;
					}
				}
				if(lflowmap.size() == 0)
				{
					Sleep(5000);
				}
				if(lflowmap.size() == 5)
				{
					lflowmap.erase(flowcount);
					lflowmap.insert(Int_Pair(flowcount, t_f));
					RecordFlag = compareflow(&lflowmap,PIDFlag);
				}
				else
				{
					lflowmap.insert(Int_Pair(flowcount, t_f));
					RecordFlag = false;
				}
				if(flowcount == 5)
				{
					flowcount = 0;
				}
				flowcount++;
			}
		}
		else if ((!RecordFlag) && SampleFlagHZ1 == 6)//��Ƶ����
		{
			BOOL WaitFlag = FALSE;
			BOOL Waitmore = FALSE;
			if (hz_pt < 12)
			{	
				if (hz_pt == 0)
				{
					Waitmore = TRUE;
				} 
				if (vavlestate != vavles[hz_pt])
				{
					WaitFlag = TRUE;
				}
				//���ط����
				if (vavles[hz_pt] == 0x01)
				{
					tmp_data[3] = 1;
// 					Sleep(5);
// 					WaitForSingleObject(plcMutex, INFINITE);
// 					if(!PLCTransfer(PLChdev, WRITEOP, MAINVAVLE, tmp_data, &pf))
// 					{
// 						//AfxMessageBox(_T("��������ʧ��!")); 
// 						//return;
// 					}
					Sleep(50);
					WaitForSingleObject(plcMutex, INFINITE);
					if(PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
					{
						vavlestate |= 0x01;
					}
					else
					{
						ReleaseMutex(plcMutex);
						continue;
					}
					ReleaseMutex(plcMutex);
				} 
// 				else if (vavles[hz_pt] == 0x02)
// 				{
// 					tmp_data[3] = 1;
// 					Sleep(5);
// 					WaitForSingleObject(plcMutex, INFINITE);
// 					if(PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
// 					{
// 						vavlestate |= 0x02;
// 					}
// 					else
// 					{
// 						ReleaseMutex(plcMutex);
// 						continue;
// 					}
// 					
// 					tmp_data[3] = 0;
// 					Sleep(6);					
// 					if(PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
// 					{
// 						vavlestate &= (~0x01);
// 					}
// 					else
// 					{
// 						ReleaseMutex(plcMutex);
// 						continue;
// 					}
// 					ReleaseMutex(plcMutex);
// 				}
				else if (vavles[hz_pt] == 0x02)
				{
					tmp_data[3] = 1;
					Sleep(5);
					WaitForSingleObject(plcMutex, INFINITE);
					if(PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
					{
						vavlestate |= 0x02;
					}
					else
					{
						ReleaseMutex(plcMutex);
						continue;
					}
					ReleaseMutex(plcMutex);
					tmp_data[3] = 0;
					Sleep(5);
					WaitForSingleObject(plcMutex, INFINITE);
					if(PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
					{
						vavlestate &= ~(0x01);
					}
					else
					{
						ReleaseMutex(plcMutex);
						continue;
					}
					ReleaseMutex(plcMutex);
				}
				else if (vavles[hz_pt] == 0x03)
				{
					tmp_data[3] = 1;
					Sleep(5);
					WaitForSingleObject(plcMutex, INFINITE);
					if(PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
					{
						vavlestate |= 0x01;
					}
					else
					{
						ReleaseMutex(plcMutex);
						continue;
					}
					ReleaseMutex(plcMutex);
				}
				else if (vavles[hz_pt] == 0x04)
				{
					tmp_data[3] = 1;
					Sleep(5);	
					WaitForSingleObject(plcMutex, INFINITE);
					if(PLCTransfer(PLChdev, WRITEOP, LMFLOWVAVLE, tmp_data, &pf))
					{
						vavlestate |= 0x04;
					}
					else
					{
						ReleaseMutex(plcMutex);
						continue;
					}

					tmp_data[3] = 0;
					Sleep(50);					
					if(PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
					{
						vavlestate &= (~0x01);
					}
					else
					{
						ReleaseMutex(plcMutex);
						continue;
					}
					
					tmp_data[3] = 0;
					Sleep(5);					
					if(PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
					{
						vavlestate &= (~0x02);
					}
					else
					{
						ReleaseMutex(plcMutex);
						continue;
					}
					ReleaseMutex(plcMutex);
				}
// 				else if(vavles[hz_pt] == 0x08)
// 				{
// 					tmp_data[3] = 1;
// 					Sleep(5);		
// 					WaitForSingleObject(plcMutex, INFINITE);
// 					if(PLCTransfer(PLChdev, WRITEOP, LFLOWVAVLE, tmp_data, &pf))
// 					{
// 						vavlestate |= 0x08;
// 					}
// 					else
// 					{
// 						ReleaseMutex(plcMutex);
// 						continue;
// 					}
// 					
// 					tmp_data[3] = 0;
// 					Sleep(500);					
// 					if(PLCTransfer(PLChdev, WRITEOP, LMFLOWVAVLE, tmp_data, &pf))
// 					{
// 						vavlestate &= (~0x04);
// 					}
// 					else
// 					{
// 						ReleaseMutex(plcMutex);
// 						continue;
// 					}
// 					ReleaseMutex(plcMutex);
// 				}
				else if (vavles[hz_pt] == 0x0B)
				{
					tmp_data[3] = 1;
					Sleep(5);
					WaitForSingleObject(plcMutex, INFINITE);
					if(PLCTransfer(PLChdev, WRITEOP, LFLOWVAVLE, tmp_data, &pf))
					{
						vavlestate |= 0x08;
					}
					else
					{
						ReleaseMutex(plcMutex);
						continue;
					}
					ReleaseMutex(plcMutex);

					tmp_data[3] = 0;
					Sleep(5);	
					WaitForSingleObject(plcMutex, INFINITE);
					if(PLCTransfer(PLChdev, WRITEOP, LMFLOWVAVLE, tmp_data, &pf))
					{
						vavlestate &= (~0x04);
					}
					else
					{
						ReleaseMutex(plcMutex);
						continue;
					}

					tmp_data[3] = 1;
					Sleep(50);					
					if(PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
					{
						vavlestate |= 0x01;
					}
					else
					{
						ReleaseMutex(plcMutex);
						continue;
					}

					tmp_data[3] = 1;
					Sleep(5);					
					if(PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
					{
						vavlestate |= 0x02;
					}
					else
					{
						ReleaseMutex(plcMutex);
						continue;
					}
					ReleaseMutex(plcMutex);


				}
				else
				{

				}//���ط��������
				Spercent = hz[hz_pt];
				CString str;
				if(!DanfossSet(Danfosshdev, &Spercent))
				{
					AfxMessageBox(_T("��Ƶ��дʧ��!")); 
					continue;;
				}
				else
				{				
					str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
				}
				Sleep(15000);//��Ƶ���15��
				if (Waitmore)
				{
					Sleep(1000);					
				}
				if (WaitFlag)
				{
					Sleep(1000);
				}
				RecordFlag = TRUE;
			}
			else if (hz_pt >= 12)
			{					
				Spercent = 0;
				if(!DanfossSet(Danfosshdev, &Spercent))
				{
					AfxMessageBox(_T("��Ƶ��дʧ��!")); 
					continue;;
				}
				else
				{				
					str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
				}
			}
			else{}
		}
		else if (SampleFlagHZ1 ==7)
		{
			if (!RecordFlag)//����¼���ݣ����ڷ��ź�Ƶ�ʣ�
			{
				if (ErrTest_pt <= 10)
				{
// 					sinfotmp.lflow = 0;
// 					sinfotmp.mflow = 0;
					if (ErrTestVavles[ErrTest_pt] == 0x1)
					{
						if (vavlestate != 0x01)
						{
							tmp_data[3] = 1;
							Sleep(6);
							WaitForSingleObject(plcMutex, INFINITE);
							if(PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
							{
								vavlestate |= 0x01;
							}else
							{
								ReleaseMutex(plcMutex);
								continue;
							}
							ReleaseMutex(plcMutex);
						}														
					}
					else if (ErrTestVavles[ErrTest_pt] == 0x02)
					{
						if (vavlestate != 0x02)
						{
							tmp_data[3] = 1;
							Sleep(6);
							WaitForSingleObject(plcMutex, INFINITE);
							if(PLCTransfer(PLChdev, WRITEOP, LMFLOWVAVLE, tmp_data, &pf))
							{
								vavlestate |= 0x02;
							}
							else
							{
								ReleaseMutex(plcMutex);
								continue;
							}								

							tmp_data[3] = 0;
							Sleep(60);
							if(PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
							{
								vavlestate &= ~0x01;
							}
							else
							{
								ReleaseMutex(plcMutex);
								continue;
							}								
							ReleaseMutex(plcMutex);	
						}
					}
					else if (ErrTestVavles[ErrTest_pt] == 0x04)
					{
						if (vavlestate != 0x04)
						{
							tmp_data[3] = 1;
							Sleep(6);
							WaitForSingleObject(plcMutex, INFINITE);
							if(PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
							{
								vavlestate |= 0x04;
							}
							else
							{
								ReleaseMutex(plcMutex);
								continue;
							}
							tmp_data[3] = 0;
							Sleep(50);
							if(PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
							{
								vavlestate &= ~0x01;
							}
							else
							{
								ReleaseMutex(plcMutex);
								continue;
							}
							ReleaseMutex(plcMutex);
						}
							
					} 
					else if (ErrTestVavles[ErrTest_pt] == 0x08)
					{
						if (vavlestate != 0x08)
						{
							tmp_data[3] = 1;
							Sleep(5);
							WaitForSingleObject(plcMutex, INFINITE);
							if(PLCTransfer(PLChdev, WRITEOP, LFLOWVAVLE, tmp_data, &pf))
							{
								vavlestate |= 0x08;
							}
							else
							{
								ReleaseMutex(plcMutex);
								continue;
							}
							tmp_data[3] = 0;
							Sleep(5);

							if(PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
							{
								vavlestate &= ~0x04;
							}
							else
							{
								ReleaseMutex(plcMutex);
								continue;
							}
							ReleaseMutex(plcMutex);
						}
					}
					else if (ErrTestVavles[ErrTest_pt] == 0x0C)
					{
						if (vavlestate != 0x0C)
						{
							tmp_data[3] = 1;
							Sleep(5);
							WaitForSingleObject(plcMutex, INFINITE);
							if(PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
							{
								vavlestate |= 0x04;
							}
							else
							{
								ReleaseMutex(plcMutex);
								continue;
							}								
							ReleaseMutex(plcMutex);
						}
					}						

					tick1 = ErrTestTime[ErrTest_pt];

					if(sinfo.weight < 5.0)
					{
						tmp_data[3] = 0;
						Sleep(500);
						WaitForSingleObject(plcMutex, INFINITE);
						if(!PLCTransfer(PLChdev, WRITEOP, WEIGHTVALE, tmp_data, &pf))
						{
							ReleaseMutex(plcMutex);
							continue;
						}
						ReleaseMutex(plcMutex);
					}
					else
					{
						Sleep(2000);
						continue;
					}
// 					Sleep(3000);//�ȴ�����Ͱ�����ȶ�
// 
// 					if (ErrTest_pt > 8)//�����������ȴ�5��
// 					{
// 						Sleep(5000);
// 					}					

					j = 3;
					while (!sinfo.weight && j) 
					{						
						SampleType |= 0x02;//������Ͱ
						Sleep(1000);
						j--;
					} 
					if (!DataFlag)
					{
						sinfotmp.weight = sinfo.weight;
						Sleep(2000);
						if (abs(sinfo.weight - sinfotmp.weight) < 0.01) 
						{
							sinfotmp.weight = sinfo.weight;
							DataFlag = TRUE;
						}
						else
						{
							continue;
						}
					}					

					for (i = 0; i < sinfo.meternum;i++)
					{
						j = 3;//�����ݴ���ʱ,������3��
						do 
						{						
							ReadMeterTotal((int)sinfo.minfo[i].mhandle,sinfo.minfo[i].meterid,\
								(__int64 *)&sinfo.minfo[i].total_f,(__int64 *)&sinfo.minfo[i].total_r);
							Sleep(50);
							j--;						
						} while (!sinfo.minfo[i].total_f && j);
						sinfotmp.minfo[i].total_f = sinfo.minfo[i].total_f;								
					}//��¼������

					Spercent = ErrTestHz[ErrTest_pt];
					if (!DanfossSet(Danfosshdev,&Spercent))
					{
						continue;
					}					
					else
					{				
						str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
					}
					sinfotmp.time = sinfo.time;

					RecordFlag = TRUE;
					DataFlag = FALSE;
				}
				else//�����Խ���(���ݼ������)
				{
					AfxMessageBox(_T("���Խ���"));

					Spercent = 0;
					if(!DanfossSet(Danfosshdev, &Spercent))
					{
						AfxMessageBox(_T("��Ƶ��дʧ��!")); 
						continue;
					}
					else
					{				
						str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
					}

					SampleType = 0;
					SampleFlag = FALSE;
					SampleFlagHZ1=0;
					RecordFlag = FALSE;
					//ErrTestFlag = TRUE;			
				}	
			}
			else
			{	
				if (ErrTest_pt <= 10)
				{
					if (tick1 == 0)
					{
						Spercent = 0;
						if(!DanfossSet(Danfosshdev, &Spercent))
						{
							AfxMessageBox(_T("��Ƶ��дʧ��!")); 
							continue;
						}
						else
						{				
							str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
						}

						//�ȳ���Ͱ�����ȶ�
						//Sleep(5000);
						sinfoend.time = sinfo.time;
						if (!DataFlag)
						{
							sinfoend.weight = sinfo.weight;
							Sleep(2000);
							if (abs(sinfo.weight - sinfoend.weight) < 0.01) 
							{
								sinfoend.weight = sinfo.weight;
								DataFlag = TRUE;
							}
							else
							{
								continue;
							}
							for (i = 0; i < sinfo.meternum;i++)
							{
								j = 3;
								do 
								{								
									ReadMeterTotal((int)sinfo.minfo[i].mhandle,sinfo.minfo[i].meterid,\
										(__int64 *)&sinfo.minfo[i].total_f,(__int64 *)&sinfo.minfo[i].total_r);
									Sleep(50);
									j--;
								} while (!sinfo.minfo[i].total_f && j);
								sinfoend.minfo[i].total_f = sinfo.minfo[i].total_f;								
							}//��¼������

							t_str_err.Format(_T("%s%.1f	"),_T("����:"),sinfotmp.lflow + sinfotmp.mflow);
							str_err += t_str_err;
							t_str_err.Format(_T("%s%.3f	"),_T("��ʼkg:"),sinfotmp.weight);
							str_err += t_str_err;
							t_str_err.Format(_T("%s%.3f	"),_T("��ֹkg:"),sinfoend.weight);
							str_err += t_str_err;
							t_str_err.Format(_T("%s%.3f%s"),_T("�ۼ�kg:"),sinfoend.weight - sinfotmp.weight,_T("\r\n"));
							str_err += t_str_err;
						}
						
						tmp_data[3] = 1;
						Sleep(500);
						WaitForSingleObject(plcMutex, INFINITE);
						if(!PLCTransfer(PLChdev, WRITEOP, WEIGHTVALE, tmp_data, &pf))
						{
							ReleaseMutex(plcMutex);
							continue;
						}
						ReleaseMutex(plcMutex);//���Խ���������ˮ

						ParaMeterTool PtFun;
						t_str_err.Format(_T("%s	%s	%s	%s	%s%s"),_T("�� ��"),_T("��ʼml"),_T("��ֹml"),_T("�ۼ�ml"),_T("���"),_T("\r\n"));
						str_err += t_str_err;	
						for (i = 0;i < sinfo.meternum;i++)
						{
							sinfotmp.minfo[i].errnum = ((sinfoend.minfo[i].total_f - sinfotmp.minfo[i].total_f) / 1000.0 - (sinfoend.weight - sinfotmp.weight)/0.996) / ((sinfoend.weight - sinfotmp.weight)/0.996) * 100;
							t_str_err.Format(_T("%s	%lld	%lld	%lld	%.2f%s"),\
								PtFun.ReverseMeterID(i),\
								sinfotmp.minfo[i].total_f,sinfoend.minfo[i].total_f,\
								sinfoend.minfo[i].total_f - sinfotmp.minfo[i].total_f,\
								sinfotmp.minfo[i].errnum,_T("\r\n"));
							str_err += t_str_err;
						}						

						ErrTest_pt ++;	
						RecordFlag = FALSE;
						DataFlag = FALSE;
					}
					else
					{
						if(sinfo.weight > 100)
						{
							tmp_data[3] = 1;
							Sleep(500);
							WaitForSingleObject(plcMutex, INFINITE);
							if(!PLCTransfer(PLChdev, WRITEOP, WEIGHTVALE, tmp_data, &pf))
							{
								ReleaseMutex(plcMutex);
								continue;
							}
							ReleaseMutex(plcMutex);
							Spercent = 1000;
							if(!DanfossSet(Danfosshdev, &Spercent))
							{
								AfxMessageBox(_T("��Ƶ��дʧ��!")); 
								continue;
							}
							else
							{				
								str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
							}
						}
					}
				}
				else
				{
					AfxMessageBox(_T("���Խ���"));
					/*Sleep(10000);*/
				}	
			}
		}
		else 
		{ 

		}

		Sleep(500);
	}
	return 0;
}
//�����߳�
UINT ReadMeter(LPVOID pParam)
{
	//MeterInfo *minfo;
	//minfo = (MeterInfo*)pParam;
	while(1)
	{
		if(SampleType & 0x04)
		{
			int i;
			for (i = 0;i < sinfo.meternum;i++)
			{
				if(!ReadMeterFly((int)sinfo.minfo[i].mhandle,sinfo.minfo[i].meterid, \
					(unsigned char*)&(sinfo.minfo[i].ml), (unsigned char*)&(sinfo.minfo[i].dt), (unsigned char*)&(sinfo.minfo[i].tf)))
				{
					sinfo.minfo[i].ml = 0;
					sinfo.minfo[i].dt = 0;
					sinfo.minfo[i].tf = 0;
				}	
				if (!ReadStateTmpr((int)sinfo.minfo[i].mhandle,sinfo.minfo[i].meterid,\
					(signed short*)&(sinfo.minfo[i].tmpr1),(signed short*)&(sinfo.minfo[i].tmpr2),(unsigned short*)&timeremain))
				{
					sinfo.minfo[i].tmpr1 = 0;
					sinfo.minfo[i].tmpr2 = 0;
				}
// 				if (!ReadMeterTotal((int)sinfo.minfo[i].mhandle,sinfo.minfo[i].meterid,\
// 					(__int64 *)&sinfo.minfo[i].total_f,(__int64 *)&sinfo.minfo[i].total_r))
// 				{
// 					sinfo.minfo[i].total_f = 0;
// 					sinfo.minfo[i].total_r = 0;
// 				}
			}
			SampleType ^= 0x04;
			Sleep(250);
		}
	}
	return 0;
}
//20mm����
void CMeasureCommdemoDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PLCFrame pf;
	u8 tmp_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};

	WaitForSingleObject(plcMutex, INFINITE);
	if(((CButton *)GetDlgItem(IDC_CHECK1))->GetCheck())
	{
		//�ط�
		tmp_data[3] = 0;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, LFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("PLCͨ��ʧ��!")); 
			ReleaseMutex(plcMutex);
			return;
		}

		if(pf.result != 0x01)
		{
			AfxMessageBox(_T("20mm���Ź�ʧ��!"));
			ReleaseMutex(plcMutex);
			return;
		}
		((CButton *)GetDlgItem(IDC_CHECK1))->SetCheck(FALSE);
		SetDlgItemText(IDC_CHECK1, _T("��"));
	}
	else
	{
		//����
		tmp_data[3] = 0x01;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, LFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("PLCͨ��ʧ��!")); 
			ReleaseMutex(plcMutex);
			return;
		}

		if(pf.result != 0x01)
		{
			AfxMessageBox(_T("20mm���ſ�ʧ��!"));
			ReleaseMutex(plcMutex);
			return;
		}
		((CButton *)GetDlgItem(IDC_CHECK1))->SetCheck(TRUE);
		SetDlgItemText(IDC_CHECK1, _T("��"));
	}
	ReleaseMutex(plcMutex);
}
//20mmС����   //16.8.3����Ϊ6mm����
void CMeasureCommdemoDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PLCFrame pf;
	u8 tmp_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};

	WaitForSingleObject(plcMutex, INFINITE);
	if(((CButton *)GetDlgItem(IDC_CHECK2))->GetCheck())
	{
		//�ط�
		tmp_data[3] = 0;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, LMFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("PLCͨ��ʧ��!")); 
			ReleaseMutex(plcMutex);
			return;
		}

		if(pf.result != 0x01)
		{
			AfxMessageBox(_T("20mmС���Ź�ʧ��!"));
			ReleaseMutex(plcMutex);
			return;
		}
		((CButton *)GetDlgItem(IDC_CHECK2))->SetCheck(FALSE);
		SetDlgItemText(IDC_CHECK2, _T("��"));
	}
	else
	{
		//����
		tmp_data[3] = 0x01;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, LMFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("PLCͨ��ʧ��!")); 
			ReleaseMutex(plcMutex);
			return;
		}

		if(pf.result != 0x01)
		{
			AfxMessageBox(_T("20mmС���ſ�ʧ��!"));
			ReleaseMutex(plcMutex);
			return;
		}
		((CButton *)GetDlgItem(IDC_CHECK2))->SetCheck(TRUE);
		SetDlgItemText(IDC_CHECK2, _T("��"));
	}
	ReleaseMutex(plcMutex);
}
//6mm����  //16.8.3����Ϊ20mmС����
void CMeasureCommdemoDlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PLCFrame pf;
	u8 tmp_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};

	WaitForSingleObject(plcMutex, INFINITE);
	if(((CButton *)GetDlgItem(IDC_CHECK3))->GetCheck())
	{
		//�ط�
		tmp_data[3] = 0;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("PLCͨ��ʧ��!")); 
			ReleaseMutex(plcMutex);
			return;
		}

		if(pf.result != 0x01)
		{
			AfxMessageBox(_T("6mm���Ź�ʧ��!"));
			ReleaseMutex(plcMutex);
			return;
		}
		((CButton *)GetDlgItem(IDC_CHECK3))->SetCheck(FALSE);
		SetDlgItemText(IDC_CHECK3, _T("��"));
	}
	else
	{
		//����
		tmp_data[3] = 1;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("PLCͨ��ʧ��!")); 
			ReleaseMutex(plcMutex);
			return;
		}

		if(pf.result != 0x01)
		{
			AfxMessageBox(_T("6mm���ſ�ʧ��!"));
			ReleaseMutex(plcMutex);
			return;
		}
		((CButton *)GetDlgItem(IDC_CHECK3))->SetCheck(TRUE);
		SetDlgItemText(IDC_CHECK3, _T("��"));
	}
	ReleaseMutex(plcMutex);
}
//6mmС����
void CMeasureCommdemoDlg::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PLCFrame pf;
	u8 tmp_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};

	WaitForSingleObject(plcMutex, INFINITE);
	if(((CButton *)GetDlgItem(IDC_CHECK4))->GetCheck())
	{
		//�ط�
		tmp_data[3] = 0;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("PLCͨ��ʧ��!")); 
			ReleaseMutex(plcMutex);
			return;
		}

		if(pf.result != 0x01)
		{
			AfxMessageBox(_T("6mmС���Ź�ʧ��!"));
			ReleaseMutex(plcMutex);
			return;
		}
		((CButton *)GetDlgItem(IDC_CHECK4))->SetCheck(FALSE);
		SetDlgItemText(IDC_CHECK4, _T("��"));
	}
	else
	{
		//����
		tmp_data[3] = 1;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("PLCͨ��ʧ��!")); 
			ReleaseMutex(plcMutex);
			return;
		}

		if(pf.result != 0x01)
		{
			AfxMessageBox(_T("6mmС���ſ�ʧ��!"));
			ReleaseMutex(plcMutex);
			return;
		}
		((CButton *)GetDlgItem(IDC_CHECK4))->SetCheck(TRUE);
		SetDlgItemText(IDC_CHECK4, _T("��"));
	}
	ReleaseMutex(plcMutex);
}
//����Ͱ����
void CMeasureCommdemoDlg::OnBnClickedButton6()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PLCFrame pf;
	u8 tmp_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};

	WaitForSingleObject(plcMutex, INFINITE);
	if(((CButton *)GetDlgItem(IDC_CHECK5))->GetCheck())
	{
		//�ط�
		tmp_data[3] = 0;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, WEIGHTVALE, tmp_data, &pf))
		{
			AfxMessageBox(_T("PLCͨ��ʧ��!")); 
			ReleaseMutex(plcMutex);
			return;
		}

		if(pf.result != 0x01)
		{
			AfxMessageBox(_T("����Ͱ���Ź�ʧ��!"));
			ReleaseMutex(plcMutex);
			return;
		}
		((CButton *)GetDlgItem(IDC_CHECK5))->SetCheck(FALSE);
		SetDlgItemText(IDC_CHECK5, _T("��"));
	}
	else
	{
		//����
		tmp_data[3] = 1;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, WEIGHTVALE, tmp_data, &pf))
		{
			AfxMessageBox(_T("PLCͨ��ʧ��!")); 
			ReleaseMutex(plcMutex);
			return;
		}

		if(pf.result != 0x01)
		{
			AfxMessageBox(_T("����Ͱ���ſ�ʧ��!"));
			ReleaseMutex(plcMutex);
			return;
		}
		((CButton *)GetDlgItem(IDC_CHECK5))->SetCheck(TRUE);
		SetDlgItemText(IDC_CHECK5, _T("��"));
	}
	ReleaseMutex(plcMutex);
}
//��ͨ������
void CMeasureCommdemoDlg::OnBnClickedButton7()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PLCFrame pf;
	u8 tmp_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};

	WaitForSingleObject(plcMutex, INFINITE);
	if(((CButton *)GetDlgItem(IDC_CHECK6))->GetCheck())
	{
		//�ط�
		tmp_data[3] = 0;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, MAINVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("PLCͨ��ʧ��!")); 
			ReleaseMutex(plcMutex);
			return;
		}

		if(pf.result != 0x01)
		{
			AfxMessageBox(_T("�����Ź�ʧ��!"));
			ReleaseMutex(plcMutex);
			return;
		}

		((CButton *)GetDlgItem(IDC_CHECK6))->SetCheck(FALSE);
		SetDlgItemText(IDC_CHECK6, _T("��"));
	}
	else
	{
		//����
		tmp_data[3] = 1;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, MAINVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("PLCͨ��ʧ��!")); 
			ReleaseMutex(plcMutex);
			return;
		}

		if(pf.result != 0x01)
		{
			AfxMessageBox(_T("�����ſ�ʧ��!"));
			ReleaseMutex(plcMutex);
			return;
		}
		((CButton *)GetDlgItem(IDC_CHECK6))->SetCheck(TRUE);
		SetDlgItemText(IDC_CHECK6, _T("��"));
	}
	ReleaseMutex(plcMutex);
}
//����
void CMeasureCommdemoDlg::OnBnClickedButton8()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(!SampleFlag)
	{
		SetDlgItemText(IDC_BUTTON8,_T("������"));
		tick = 0;
		SampleType = 0x07;//0x07;
		::SetTimer(m_hWnd,1,100,NULL);
		SampleFlag = TRUE;
		SampleFlagHZ1 = 1;
		((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(FALSE);
		/*((CButton*)GetDlgItem(IDC_BUTTON16))->EnableWindow(FALSE);*/
		((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(FALSE);
	}
	else
	{
		::KillTimer(m_hWnd,1);
		SampleType = 0;
		SampleFlag = FALSE;
		SampleFlagHZ1 = 0;
		SetDlgItemText(IDC_BUTTON8,_T("��������"));
		((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(TRUE); 
		/*((CButton*)GetDlgItem(IDC_BUTTON16))->EnableWindow(TRUE);*/
		((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(TRUE);
	}
}
//��Ƶ������
void CMeasureCommdemoDlg::OnBnClickedButton9()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(((CButton *)GetDlgItem(IDC_CHECK7))->GetCheck())
	{
		//ֹͣ
		if(!DanfossAction(Danfosshdev, DANFOSSSTOP))
		{
			AfxMessageBox(_T("��Ƶ��ֹͣʧ��!")); 
			return;
		}

		((CButton *)GetDlgItem(IDC_CHECK7))->SetCheck(FALSE);
		SetDlgItemText(IDC_CHECK7, _T("ֹͣ"));
	}
	else
	{
		//����
		if(!DanfossAction(Danfosshdev, DANFOSSSTART))
		{
			AfxMessageBox(_T("��Ƶ������ʧ��!")); 
			return;
		}

		((CButton *)GetDlgItem(IDC_CHECK7))->SetCheck(TRUE);
		SetDlgItemText(IDC_CHECK7, _T("����"));
	}
}
//��Ƶ��
void CMeasureCommdemoDlg::OnBnClickedButton10()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	short t_percent;
	/*u16 t_hz;*/
	float t_hz;
	CString str;

	if(!DAnfossRead(Danfosshdev, &t_percent))
	{
		AfxMessageBox(_T("��Ƶ����ʧ��!")); 
		return;
	}

	t_hz = STDHZ * t_percent / 10000.0;	
	str.Format(_T("%.1f"),(float)t_hz);
	SetDlgItemText(IDC_EDIT1, str);
	str_hz = str; 
	/*SetDlgItemInt(IDC_EDIT1, t_hz);*/
	//Spercent = t_percent;
}
//дƵ��
void CMeasureCommdemoDlg::OnBnClickedButton11()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	float t_hz;
	short t_percent;
	CString str;
	char strtmp[4];
	int i;

	//t_hz = GetDlgItemInt(IDC_EDIT2);
	GetDlgItemText(IDC_EDIT2,str);
	for (i = 0;i < min(4,str.GetLength());i++)
	{
		strtmp[i] = str[i];
	}
	t_hz = atof(strtmp);

	if(t_hz > 50)
	{
		AfxMessageBox(_T("��Ƶ��Ƶ��ֵΪ0��50HZ!")); 
		return;
	}
	t_percent = t_hz / STDHZ * 10000;

	if(!DanfossSet(Danfosshdev, &t_percent))
	{
		AfxMessageBox(_T("��Ƶ��дʧ��!")); 
		return;
	}
	else
	{
		SetDlgItemText(IDC_EDIT1, str);
		str_hz = str;
	}
	Spercent = t_percent;
}
//�����������
void CMeasureCommdemoDlg::OnBnClickedButton12()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PLCFrame pf;
	u8 tmp_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};

	WaitForSingleObject(plcMutex, INFINITE);
	if(((CButton *)GetDlgItem(IDC_CHECK8))->GetCheck())
	{
		//���������
		tmp_data[3] = 0;
		Sleep(4);
		if(!PLCTransfer(PLChdev, WRITEOP, PLUSSTART, tmp_data, &pf))
		{
			AfxMessageBox(_T("PLCͨ��ʧ��!")); 
			ReleaseMutex(plcMutex);
			return;
		}

		if(pf.result != 0x01)
		{
			AfxMessageBox(_T("��������ʧ��!"));
			ReleaseMutex(plcMutex);
			return;
		}
		((CButton *)GetDlgItem(IDC_CHECK8))->SetCheck(FALSE);
		SetDlgItemText(IDC_CHECK8, _T("��"));
	}
	else
	{
		//���������
		tmp_data[3] = 1;
		if(!PLCTransfer(PLChdev, WRITEOP, PLUSSTART, tmp_data, &pf))
		{
			AfxMessageBox(_T("PLCͨ��ʧ��!")); 
			ReleaseMutex(plcMutex);
			return;
		}

		if(pf.result != 0x01)
		{
			AfxMessageBox(_T("��������ʧ��!"));
			ReleaseMutex(plcMutex);
			return;
		}
		((CButton *)GetDlgItem(IDC_CHECK8))->SetCheck(TRUE);
		SetDlgItemText(IDC_CHECK8, _T("��"));
	}
	ReleaseMutex(plcMutex);
}
//Ƶ�ʽ������
void CMeasureCommdemoDlg::OnBnClickedButton13()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(!SampleFlagHZ1 && !SampleFlag)
	{
		opexf.InitExcel();
		opexf.OpenExcelFile(_T("\\sample.xls"));
		opexf.WriteTableHead(sinfo.meternum, sinfo.minfo);
		opexf.SaveExcel();

		SetDlgItemText(IDC_BUTTON13,_T("������"));
		tick = 0;
		//////
		tick1 = 40;//ÿ�μ�Ƶ�ʻ�ȡ40������
		////////
		Spercent = 2400;
		if(!DanfossSet(Danfosshdev, &Spercent))
		{
			AfxMessageBox(_T("��Ƶ��дʧ��!")); 
			return;
		}
		else
		{				
			str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
		}
		SampleType = 0x07;//0x07;
		::SetTimer(m_hWnd,1,100,NULL);
		SampleFlag = TRUE;
		SampleFlagHZ1 = 2;
		((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(FALSE); 
		((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(FALSE);
		/*((CButton*)GetDlgItem(IDC_BUTTON16))->EnableWindow(FALSE);*/
		((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON19))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON20))->EnableWindow(FALSE);
	}
	else
	{
		::KillTimer(m_hWnd,1);
		SampleType = 0;
		SampleFlag = FALSE;
		Spercent = 0;
		if(!DanfossSet(Danfosshdev, &Spercent))
		{
			AfxMessageBox(_T("��Ƶ��дʧ��!")); 
			return;
		}
		else
		{				
			str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
		}

		opexf.SaveExcel();
		opexf.CloseExcelFile();
		opexf.ReleaseExcel();
		SetDlgItemText(IDC_BUTTON13,_T("Ƶ�ʵ�������"));
		((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(TRUE);
		/*((CButton*)GetDlgItem(IDC_BUTTON16))->EnableWindow(TRUE);*/
		((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON19))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON20))->EnableWindow(TRUE);
	}
}
//30�����
void CMeasureCommdemoDlg::OnBnClickedButton14()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(!SampleFlagHZ1 && !SampleFlag)
	{
		opexf.InitExcel();
		opexf.OpenExcelFile(_T("\\sample.xls"));
		opexf.WriteTableHead(sinfo.meternum, sinfo.minfo);
		opexf.SaveExcel();

		SetDlgItemText(IDC_BUTTON14,_T("������"));
		tick = 0;
		Spercent = 1600;
		if(!DanfossSet(Danfosshdev, &Spercent))
		{
			AfxMessageBox(_T("��Ƶ��дʧ��!")); 
			return;
		}
		else
		{				
			str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
		}
		SampleType = 0x07;//0x07;
		::SetTimer(m_hWnd,1,100,NULL);
		SampleFlag = TRUE;
		SampleFlagHZ1 = 3;
		((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(FALSE); 
		((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(FALSE);
		/*((CButton*)GetDlgItem(IDC_BUTTON16))->EnableWindow(FALSE);*/
		((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON19))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON20))->EnableWindow(FALSE);
	}
	else
	{
		::KillTimer(m_hWnd,1);
		SampleType = 0;
		SampleFlag = FALSE;
		SampleFlagHZ1 = 0;
		SetDlgItemText(IDC_BUTTON14,_T("30�����"));
		Spercent = 0;
		if(!DanfossSet(Danfosshdev, &Spercent))
		{
			AfxMessageBox(_T("��Ƶ��дʧ��!")); 
			return;
		}
		else
		{				
			str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
		}

		opexf.SaveExcel();
		opexf.CloseExcelFile();
		opexf.ReleaseExcel();
		((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(TRUE);
		/*((CButton*)GetDlgItem(IDC_BUTTON16))->EnableWindow(TRUE);*/
		((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON19))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON20))->EnableWindow(TRUE);
	}
}
//��ʼУ��
void CMeasureCommdemoDlg::OnBnClickedButton15()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str;
	CString t_str=_T("");
	int i;
	PLCFrame pf;
	u8 tmp_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	memset(&pf, 0, sizeof(PLCFrame));

	if (!SampleFlagHZ1 && !SampleFlag)
	{
		tmp_data[3] = 1;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, MAINVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��������ʧ��!")); 
			return ;
		}
		Sleep(500);
		if(!PLCTransfer(PLChdev, WRITEOP, WEIGHTVALE, tmp_data, &pf))
		{
			AfxMessageBox(_T("������Ͱʧ��!")); 
			return ;
		}
		Sleep(500);
		vavlestate = 1;
		if(!PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
		{
			vavlestate = 0;
			AfxMessageBox(_T("��6mmС��ʧ��!")); 
			return ;
		}
		tmp_data[3] = 0;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��6mm��ʧ��!")); 
			return ;
		}
		tmp_data[3] = 0;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, LMFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��20mm��ʧ��!")); 
			return ;
		}
		tmp_data[3] = 0;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, LFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��20mmС��ʧ��!")); 
			return ;
		}
		opexf.InitExcel();
		opexf.OpenExcelFile(_T("\\sample.xls"));
		opexf.WriteTableHead(sinfo.meternum, sinfo.minfo);
		opexf.SaveExcel();

		for (i=0;i<sinfo.meternum;i++)
		{
			if (WriteMeterState((int)sinfo.minfo[i].mhandle,sinfo.minfo[i].meterid,3600))
			{
				str.Format(_T("%d"),i);
				str+=_T("����У����\r\n");
				t_str+=str;
			}
			else
			{
				str.Format(_T("%d"),i);
				str+=_T("δ����У����\r\n");
				t_str+=str;
			}
		}	
		tick1 = 0;
		Spercent =1600;
		if(!DanfossSet(Danfosshdev, &Spercent))
		{
			AfxMessageBox(_T("��Ƶ��дʧ��!")); 
			return;
		}
		else
		{				
			str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
		}
		SampleType = 0x07;//0x07;
		SampleFlag = TRUE;
		SampleFlagHZ1 = 4;
		RecordFlag = FALSE;
		hzpercent = 100;
		flowoffset = 0.02;
		::SetTimer(m_hWnd,1,100,NULL);

		SetDlgItemText(IDC_BUTTON15,_T("У�������"));		
		((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(FALSE); 
		((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(FALSE);
		/*((CButton*)GetDlgItem(IDC_BUTTON16))->EnableWindow(FALSE);*/
		((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON19))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON20))->EnableWindow(FALSE); 

		SetDlgItemText(IDC_EDIT4,t_str);
	} 
	else
	{
		::KillTimer(m_hWnd,1);
		SampleType = 0;
		SampleFlag = FALSE;
		SampleFlagHZ1=0;
		RecordFlag = TRUE;
		Spercent = 0;
		if(!DanfossSet(Danfosshdev, &Spercent))
		{
			AfxMessageBox(_T("��Ƶ��дʧ��!")); 
			return;
		}
		else
		{				
			str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
		}

		opexf.SaveExcel();
		opexf.CloseExcelFile();
		opexf.ReleaseExcel();

		SetDlgItemText(IDC_EDIT4,t_str);
		SetDlgItemText(IDC_BUTTON15,_T("У�����"));
		((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(TRUE);	
		/*((CButton*)GetDlgItem(IDC_BUTTON16))->EnableWindow(FALSE);*/
		((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON19))->EnableWindow(TRUE);	
		((CButton*)GetDlgItem(IDC_BUTTON20))->EnableWindow(TRUE);	
	}
}
//��У��Ի���
void CMeasureCommdemoDlg::OnBnClickedButton17()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	pmtdialog.ShowWindow(true);	
}
//PID���Ƽ���
void CMeasureCommdemoDlg::OnBnClickedButton18()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PLCFrame pf;
	u8 tmp_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	memset(&pf, 0, sizeof(PLCFrame));

	if (!SampleFlagHZ1 && !SampleFlag)
	{
		tmp_data[3] = 1;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, MAINVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��������ʧ��!")); 
			return ;
		}
		Sleep(500);
		if(!PLCTransfer(PLChdev, WRITEOP, WEIGHTVALE, tmp_data, &pf))
		{
			AfxMessageBox(_T("������Ͱʧ��!")); 
			return ;
		}
		Sleep(500);
		if(!PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��6mmС��ʧ��!")); 
			return ;
		}		
		tmp_data[3] = 0;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��6mm��ʧ��!")); 
			return ;
		}
		tmp_data[3] = 0;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, LMFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��20mm��ʧ��!")); 
			return ;
		}
		tmp_data[3] = 0;
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, LFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��20mmС��ʧ��!")); 
			return ;
		}
		vavlestate = 0x01;
		opexf.InitExcel();
		opexf.OpenExcelFile(_T("\\sample.xls"));
		opexf.WriteTableHead(sinfo.meternum, sinfo.minfo);
		opexf.SaveExcel();

		tick1 = 0;
		Spercent =1550;
		SampleType = 0x07;//0x07;
		SampleFlag = TRUE;
		SampleFlagHZ1 = 4;
		RecordFlag = FALSE;
		PIDFlag = TRUE;

		PID_init();
		pid.setflowrate = 10;
		/*pid.KP = 2.5;*/
		PID_dflow = 5.0;
		flowoffset = 0.02;

		if(!DanfossSet(Danfosshdev, &Spercent))
		{
			AfxMessageBox(_T("��Ƶ��дʧ��!")); 
			return;
		}
		else
		{				
			str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
		}
		::SetTimer(m_hWnd,1,100,NULL);
		PIDThread->ResumeThread();	

		SetDlgItemText(IDC_BUTTON19,_T("PID������"));		
		((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(FALSE); 
		((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(FALSE);  
		/*((CButton*)GetDlgItem(IDC_BUTTON16))->EnableWindow(FALSE);*/
		((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(FALSE); 
		((CButton*)GetDlgItem(IDC_BUTTON20))->EnableWindow(FALSE); 

	} 
	else
	{
		::KillTimer(m_hWnd,1);		
		PIDThread->SuspendThread();
		Spercent = 0;
		if(!DanfossSet(Danfosshdev, &Spercent))
		{
			AfxMessageBox(_T("��Ƶ��дʧ��!")); 
			return;
		}
		else
		{				
			str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
		}

		SampleType = 0;
		SampleFlag = FALSE;
		SampleFlagHZ1=0;
		RecordFlag = TRUE;
		PIDFlag = FALSE;
		opexf.SaveExcel();
		opexf.CloseExcelFile();
		opexf.ReleaseExcel();

		SetDlgItemText(IDC_BUTTON19,_T("PID����"));
		((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(TRUE);	
		((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON20))->EnableWindow(TRUE);
	}
}
//PID��������
void CMeasureCommdemoDlg::OnBnClickedButton19()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PID_init();
	pid.setflowrate = GetDlgItemInt(IDC_EDIT9);

	::SetTimer(m_hWnd,1,100,NULL);
	PIDThread->ResumeThread();	

	((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(FALSE); 
	((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(FALSE); 

}
//ֹͣPID����
void CMeasureCommdemoDlg::OnBnClickedButton20()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PIDThread->SuspendThread();
}
//�������
// void CMeasureCommdemoDlg::OnBnClickedButton21()
// {
// 	// TODO: �ڴ���ӿؼ�֪ͨ����������
// 	char meterid_new[7];
// 	CString str;
// 	char strtmp[2];
// 	int i;
// 	unsigned short j;
// 
// 	GetDlgItemText(IDC_EDIT11,str);
// 
// 	j = str.GetLength();
// 	if (j != 14)
// 	{
// 		AfxMessageBox(_T("��Ŵ���"));
// 		return;
// 	}
// 	else
// 	{
// 		j = 6;
// 		for(i = 0;i < 14;i++)
// 		{	
// 			if(str[i] >= '0' && str[i] <= '9')
// 			{
// 				if(i % 2)
// 				{
// 					meterid_new[j] += (str[i] - '0');
// 					j--;
// 				}
// 				else
// 				{
// 					meterid_new[j] = (str[i] - '0') * 0x10;
// 				}
// 			}
// 		}
// 		
// 		memcpy(sinfo.minfo[sinfo.meternum].meterid, meterid_new, 7);	
// 		sinfo.minfo[sinfo.meternum].mhandle = Meterhdev;
// 		sinfo.meternum += 1;
// 
// 		MeterIDFile.InitExcel();
// 		MeterIDFile.OpenWithoutSheet(_T("\\meterid.xls"));
// 		MeterIDFile.LoadSheet(1,TRUE);
// 		MeterIDFile.SaveExcel();
// 
// 		int row = MeterIDFile.GetRowCount();
// 		MeterIDFile.SetCellString(row + 1,1,str);
// 
// 		MeterIDFile.SaveExcel();		
// 		MeterIDFile.CloseExcelFile();
// 		MeterIDFile.ReleaseExcel();
// 	}	
// }
//��ָ��Ƶ�ʼ���
void CMeasureCommdemoDlg::OnBnClickedButton23()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PLCFrame pf;
	u8 tmp_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	memset(&pf, 0, sizeof(PLCFrame));

	if (!SampleFlagHZ1 && !SampleFlag)
	{
		tmp_data[3] = 1;
		Sleep(500);
		if(!PLCTransfer(PLChdev, WRITEOP, WEIGHTVALE, tmp_data, &pf))
		{
			AfxMessageBox(_T("������Ͱʧ��!")); 
			return ;
		}
		
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, MAINVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��������ʧ��!")); 
			return ;
		}	
		tmp_data[3] = 0;
		Sleep(500);
		if(!PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��6mmС��ʧ��!")); 
			return ;
		}		
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��6mm��ʧ��!")); 
			return ;
		}
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, LMFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��20mm��ʧ��!")); 
			return ;
		}
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, LFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��20mmС��ʧ��!")); 
			return ;
		}
		vavlestate = 0x00;
		opexf.InitExcel();
		opexf.OpenExcelFile(_T("\\sample.xls"));
		opexf.WriteTableHead(sinfo.meternum, sinfo.minfo);
		opexf.SaveExcel();

		tick1 = 60;
		SampleType = 0x07;//0x07;
		SampleFlag = TRUE;	
		RecordFlag = TRUE;
		SampleFlagHZ1 = 6;
		hz_pt = -1;

		SetDlgItemText(IDC_BUTTON23,_T("��Ƶ������"));		
		((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(FALSE); 
		((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(FALSE);  
		((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(FALSE); 
		((CButton*)GetDlgItem(IDC_BUTTON19))->EnableWindow(FALSE); 
		((CButton*)GetDlgItem(IDC_BUTTON20))->EnableWindow(FALSE); 
		((CButton*)GetDlgItem(IDC_BUTTON24))->EnableWindow(FALSE);

		::SetTimer(m_hWnd,1,100,NULL);
				
	} 
	else
	{
		::KillTimer(m_hWnd,1);	
		opexf.SaveExcel();
		opexf.CloseExcelFile();
		opexf.ReleaseExcel();
		Spercent = 0;
		if(!DanfossSet(Danfosshdev, &Spercent))
		{
			AfxMessageBox(_T("��Ƶ��дʧ��!")); 
			return;
		}
		else
		{				
			str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
		}

		SampleType = 0;
		SampleFlag = FALSE;
		SampleFlagHZ1=0;
		RecordFlag = FALSE;

		SetDlgItemText(IDC_BUTTON23,_T("��Ƶ����"));
		((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(TRUE);	
		((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON19))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON20))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON24))->EnableWindow(TRUE);
	}

}
//��ָ��Ƶ�ʲ������
void CMeasureCommdemoDlg::OnBnClickedButton24()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PLCFrame pf;
	u8 tmp_data[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	memset(&pf, 0, sizeof(PLCFrame));

	if (ErrTestFlag)
	{		
		tmp_data[3] = 1;
		Sleep(500);
		if(!PLCTransfer(PLChdev, WRITEOP, WEIGHTVALE, tmp_data, &pf))
		{
			AfxMessageBox(_T("������Ͱʧ��!")); 
			return ;
		}

		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, MAINVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��������ʧ��!")); 
			return ;
		}	
		tmp_data[3] = 0;
		Sleep(500);
		if(!PLCTransfer(PLChdev, WRITEOP, MLFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��6mmС��ʧ��!")); 
			return ;
		}		
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, MFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��6mm��ʧ��!")); 
			return ;
		}
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, LMFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��20mm��ʧ��!")); 
			return ;
		}
		Sleep(5);
		if(!PLCTransfer(PLChdev, WRITEOP, LFLOWVAVLE, tmp_data, &pf))
		{
			AfxMessageBox(_T("��20mmС��ʧ��!")); 
			return ;
		}

		ErrRcrdFile.InitExcel();
		ErrRcrdFile.OpenExcelFile(_T("\\ErrRecord.xls"));
		ErrRcrdFile.WriteErrTableHead(sinfo.meternum, sinfo.minfo);
		ErrRcrdFile.SaveExcel();

		vavlestate = 0x00;
		tick1 = 3 * 30;
		SampleType = 0x03;//�������߳�;
		/*ReadMeterThread->SuspendThread();*/
		SampleFlag = TRUE;	
		RecordFlag = FALSE;
		SampleFlagHZ1 = 7;
		ErrTestFlag = FALSE;
		ErrTest_pt = 0;
		calcnum = 0;

		ErrRcrdFile.SaveExcel();
		ErrRcrdFile.CloseExcelFile();
		ErrRcrdFile.ReleaseExcel();

		SetDlgItemText(IDC_BUTTON24,_T("��������"));		
		((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(FALSE); 
		((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(FALSE);
		((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(FALSE);  
		((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(FALSE); 
		((CButton*)GetDlgItem(IDC_BUTTON19))->EnableWindow(FALSE); 
		((CButton*)GetDlgItem(IDC_BUTTON20))->EnableWindow(FALSE); 
		((CButton*)GetDlgItem(IDC_BUTTON23))->EnableWindow(FALSE); 

		::SetTimer(m_hWnd,1,100,NULL);
	} 
	else
	{
		ErrRcrdFile.SaveExcel();
		ErrRcrdFile.CloseExcelFile();
		ErrRcrdFile.ReleaseExcel();

		::KillTimer(m_hWnd,1);	
		Spercent = 0;
		if(!DanfossSet(Danfosshdev, &Spercent))
		{
			AfxMessageBox(_T("��Ƶ��дʧ��!")); 
			return;
		}
		else
		{				
			str_hz.Format(_T("%.1f"),(float)(STDHZ * Spercent / 10000.0));
		}

		tick1 = 0;
		SampleType = 0;
		SampleFlag = FALSE;
		SampleFlagHZ1=0;
		RecordFlag = FALSE;
		ErrTestFlag = TRUE;
		/*ReadMeterThread->ResumeThread();*/

		SetDlgItemText(IDC_BUTTON24,_T("��ʼ����"));
		((CButton*)GetDlgItem(IDC_BUTTON8))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON13))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON14))->EnableWindow(TRUE);	
		((CButton*)GetDlgItem(IDC_BUTTON15))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON18))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON19))->EnableWindow(TRUE); 
		((CButton*)GetDlgItem(IDC_BUTTON20))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON23))->EnableWindow(TRUE);
	}

}
//��¼���Գ�ʼ����
void CMeasureCommdemoDlg::OnBnClickedButton25()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString t_str;
	int i;

	SampleType |= 0x02;//��ȡ����Ͱ����
	Sleep(1000);						
	for (i = 0;i < sinfo.meternum;i++)
	{
		sinfo.minfo[i].total_f = 0;
		ReadMeterTotal((int)sinfo.minfo[i].mhandle,sinfo.minfo[i].meterid,(__int64*)&sinfo.minfo[i].total_f,(__int64*)&sinfo.minfo[i].total_r);
		
	}
	sinfotmp = sinfo;
	sinfotmp.lflow = 0;
	sinfotmp.mflow = 0;
	GetDlgItemText(IDC_EDIT4,str_err);
	t_str_err.Format(_T("%s%.3f%s"),_T("��ʼkg:"),sinfotmp.weight,_T("\r\n"));
	str_err += t_str_err;	
	t_str_err.Format(_T("%s	%s	%s%s"),_T("��           ��"),_T("�����ʼml"),_T("�����ʼml"),_T("\r\n"));
	str_err += t_str_err;	
	ParaMeterTool PtFun;
	for (i = 0;i <sinfo.meternum;i++)
	{

		t_str_err.Format(_T("%s	%8lld	%8lld%s"),\
			PtFun.ReverseMeterID(i),sinfotmp.minfo[i].total_f,sinfotmp.minfo[i].total_r,_T("\r\n"));
		str_err += t_str_err;
	}
	SetDlgItemText(IDC_EDIT4,str_err);

	i = ((CEdit*)GetDlgItem(IDC_EDIT4))->GetLineCount();
	((CEdit*)GetDlgItem(IDC_EDIT4))->LineScroll(i,0);
}
//�鿴���
void CMeasureCommdemoDlg::OnBnClickedButton26()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString t_str;
	int i;
	double Density_now ;
	SampleType |= 0x02;//��ȡ����Ͱ����
	Sleep(1000);						
	for (i = 0;i < sinfo.meternum;i++)
	{
		sinfo.minfo[i].total_f = 0;
		ReadMeterTotal((int)sinfo.minfo[i].mhandle,sinfo.minfo[i].meterid,(__int64*)&sinfo.minfo[i].total_f,(__int64*)&sinfo.minfo[i].total_r);
		ReadStateTmpr((int)sinfo.minfo[i].mhandle,sinfo.minfo[i].meterid,(signed short*)&sinfo.minfo[i].tmpr1,(signed short*)&sinfo.minfo[i].tmpr2,(unsigned short*)&timeremain);
	}
	sinfoend = sinfo;

	GetDlgItemText(IDC_EDIT4,str_err);
	t_str_err.Format(_T("%s%.3f	%s%.3f	%s%.3f%s"),_T("��ֹ(kg):"),sinfoend.weight,_T("�ۼ�(kg):"),sinfoend.weight -  sinfotmp.weight,_T("�¶�(��):"),sinfo.intempr,_T("\r\n"));
	str_err += t_str_err;	
	t_str_err.Format(_T("%s	%s	%s	%s%s"),_T("��           ��"),_T("��    ֹ(ml)"),_T("��    ��(ml)"),_T("��  ��"),_T("\r\n"));
	str_err += t_str_err;

	ParaMeterTool PtFun;

	ErrRcrdFile.InitExcel();
	ErrRcrdFile.OpenWithoutSheet(_T("\\ErrRecord.xls"));
	ErrRcrdFile.LoadSheet(1,TRUE);

	int row = ErrRcrdFile.GetRowCount();
	ErrRcrdFile.SetCellInt(  row + 1,1,sinfoend.time - sinfotmp.time);
	ErrRcrdFile.SetCellFloat(row + 1,2,sinfotmp.lflow + sinfotmp.mflow);
	ErrRcrdFile.SetCellFloat(row + 1,3,sinfotmp.weight);
	ErrRcrdFile.SetCellFloat(row + 1,4,sinfoend.weight);
	ErrRcrdFile.SetCellFloat(row + 1,5,sinfoend.weight - sinfotmp.weight);

	Density_now = Density_water[int(sinfo.intempr * 10)]/1000;
	for (i = 0;i < sinfo.meternum;i++)
	{
		sinfotmp.minfo[i].errnum = ((sinfoend.minfo[i].total_f - sinfotmp.minfo[i].total_f) / 1000.0 -\
			(sinfoend.weight - sinfotmp.weight)/0.996) / ((sinfoend.weight - sinfotmp.weight)/0.996) * 100;
		sinfo.minfo[i].errnum = ((sinfoend.minfo[i].total_f - sinfotmp.minfo[i].total_f) / 1000.0 -\
			(sinfoend.weight - sinfotmp.weight)/Density_now) / ((sinfoend.weight - sinfotmp.weight)/Density_now) * 100;
		sinfoend.minfo[i].errnum = ((sinfoend.minfo[i].total_f - sinfotmp.minfo[i].total_f) / 1000.0 -\
			(sinfoend.weight - sinfotmp.weight)/Density_now * 1.0011) / ((sinfoend.weight - sinfotmp.weight)/Density_now * 1.0011) * 100;

		t_str_err.Format(_T("%s	%8lld	%8lld	%.2f	%.2f	%.2f%s"),\
			PtFun.ReverseMeterID(i),sinfoend.minfo[i].total_f,\
			sinfoend.minfo[i].total_f - sinfotmp.minfo[i].total_f,sinfotmp.minfo[i].errnum,sinfo.minfo[i].errnum,sinfoend.minfo[i].errnum,_T("\r\n"));
		str_err += t_str_err;
		
		ErrRcrdFile.SetCellFloat(row + 1,6  + i * 5,sinfotmp.minfo[i].total_f);
		ErrRcrdFile.SetCellFloat(row + 1,7  + i * 5,sinfoend.minfo[i].total_f);
		ErrRcrdFile.SetCellFloat(row + 1,8  + i * 5,sinfoend.minfo[i].total_f - sinfotmp.minfo[i].total_f);
		ErrRcrdFile.SetCellFloat(row + 1,9  + i * 5,sinfotmp.minfo[i].errnum);
		ErrRcrdFile.SetCellFloat(row + 2,9  + i * 5,sinfo.minfo[i].errnum);
		ErrRcrdFile.SetCellFloat(row + 3,9  + i * 5,sinfoend.minfo[i].errnum);
		ErrRcrdFile.SetCellFloat(row + 1,10 + i * 5,sinfoend.minfo[i].tmpr1);
	}
	ErrRcrdFile.SaveExcel();
	ErrRcrdFile.CloseExcelFile();
	ErrRcrdFile.ReleaseExcel();

	SetDlgItemText(IDC_EDIT4,str_err);
	i = ((CEdit*)GetDlgItem(IDC_EDIT4))->GetLineCount();
	((CEdit*)GetDlgItem(IDC_EDIT4))->LineScroll(i,0);

}
//�½�����¼��
void CMeasureCommdemoDlg::OnBnClickedButton28()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int i;
	ErrRcrdFile.InitExcel();
	ErrRcrdFile.OpenExcelFile(_T("\\ErrRecord.xls"));
	ErrRcrdFile.WriteErrTableHead(sinfo.meternum, sinfo.minfo);
	ErrRcrdFile.SaveExcel();
	
	int row = ErrRcrdFile.GetRowCount();
	ErrRcrdFile.SetCellString(row + 1,5,_T("����0"));
	ErrRcrdFile.SetCellString(row + 2,5,_T("����1"));
	ErrRcrdFile.SetCellString(row + 3,5,_T("����2"));
	ErrRcrdFile.SetCellString(row + 4,5,_T("ʱ���"));

	for (i = 0;i < sinfo.meternum;i++)
	{
		ReadFlowP((int)sinfo.minfo[i].mhandle,sinfo.minfo[i].meterid,(unsigned char*)&flowpara);
		ErrRcrdFile.SetCellFloat(row + 1,6 + i * 5,flowpara.P00);
		ErrRcrdFile.SetCellFloat(row + 1,7 + i * 5,flowpara.P01);
		ErrRcrdFile.SetCellFloat(row + 1,8 + i * 5,flowpara.P02);
		ErrRcrdFile.SetCellFloat(row + 2,6 + i * 5,flowpara.P10);
		ErrRcrdFile.SetCellFloat(row + 2,7 + i * 5,flowpara.P11);
		ErrRcrdFile.SetCellFloat(row + 2,8 + i * 5,flowpara.P12);
		ErrRcrdFile.SetCellFloat(row + 3,6 + i * 5,flowpara.P20);
		ErrRcrdFile.SetCellFloat(row + 3,7 + i * 5,flowpara.P21);
		ErrRcrdFile.SetCellFloat(row + 3,8 + i * 5,flowpara.P22);
		ErrRcrdFile.SetCellFloat(row + 4,6 + i * 5,flowpara.Td );
	}

	ErrRcrdFile.SaveExcel();
	ErrRcrdFile.CloseExcelFile();
	ErrRcrdFile.ReleaseExcel();	
}

//�����ַ
void CMeasureCommdemoDlg::OnBnClickedButton27()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//��ų�ʼ��
// 	MeterIDFile.InitExcel();
// 	MeterIDFile.OpenWithoutSheet(_T("\\meterid.xls"));
// 	MeterIDFile.LoadSheet(1,TRUE);
// 	MeterIDFile.SaveExcel();

// 	int row = MeterIDFile.GetRowCount();
// 	int i,k;
// 	CString str;
	sinfo.meternum = 0;

// 	for (i = 1;i <= row;i++)
// 	{
// 		str = MeterIDFile.GetCellString(i,1);
// 		char meterid_new[7];		
// 		unsigned short j;
// 
// 		j = str.GetLength();
// 		if (j != 14)
// 		{
// 			str.Format(_T("%d%s"),i,_T("��Ŵ���"));
// 			AfxMessageBox(str);
// 			return;
// 		}
// 		else
// 		{
// 			j = 6;
// 			for(k = 0;k < 14;k++)
// 			{	
// 				if(str[k] >= '0' && str[k] <= '9')
// 				{
// 					if(k % 2)
// 					{
// 						meterid_new[j] += (str[k] - '0');
// 						j--;
// 					}
// 					else
// 					{
// 						meterid_new[j] = (str[k] - '0') * 0x10;
// 					}
// 				}
// 			}
// 		}
// 		memcpy(sinfo.minfo[sinfo.meternum].meterid, meterid_new, 7);	
// 		sinfo.minfo[sinfo.meternum].mhandle = Meterhdev;
// 		sinfo.meternum += 1;		
// 	}
// 	MeterIDFile.SaveExcel();
// 	MeterIDFile.CloseExcelFile();
// 	MeterIDFile.ReleaseExcel();

	//��ȡ��com21�ı��
	if (!ReadMeterId((int)Meterhdev_com21,sinfotmp.minfo[sinfo.meternum].meterid))
	{
		AfxMessageBox(_T("��com21��ַʧ��!")); 
	}
	else
	{
		memcpy(sinfo.minfo[sinfo.meternum].meterid, sinfotmp.minfo[sinfo.meternum].meterid, 7);
		sinfo.minfo[sinfo.meternum].mhandle = Meterhdev_com21;
		sinfo.meternum += 1;
	}////////////
	//��ȡ��com22�ı��
	if (!ReadMeterId((int)Meterhdev_com22,sinfotmp.minfo[sinfo.meternum].meterid))
	{
		AfxMessageBox(_T("��com22��ַʧ��!")); 
	}
	else
	{
		memcpy(sinfo.minfo[sinfo.meternum].meterid, sinfotmp.minfo[sinfo.meternum].meterid, 7);
		sinfo.minfo[sinfo.meternum].mhandle = Meterhdev_com22;
		sinfo.meternum += 1;
	}////////////
	//��ȡ��com23�ı��
	if (!ReadMeterId((int)Meterhdev_com23,sinfotmp.minfo[sinfo.meternum].meterid))
	{
		AfxMessageBox(_T("��com23��ַʧ��!")); 
	}
	else
	{
		memcpy(sinfo.minfo[sinfo.meternum].meterid, sinfotmp.minfo[sinfo.meternum].meterid, 7);
		sinfo.minfo[sinfo.meternum].mhandle = Meterhdev_com23;
		sinfo.meternum += 1;
	}////////////
	//��ȡ��com24�ı��
	if (!ReadMeterId((int)Meterhdev_com24,sinfotmp.minfo[sinfo.meternum].meterid))
	{
		AfxMessageBox(_T("��com24��ַʧ��!")); 
	}
	else
	{
		memcpy(sinfo.minfo[sinfo.meternum].meterid, sinfotmp.minfo[sinfo.meternum].meterid, 7);
		sinfo.minfo[sinfo.meternum].mhandle = Meterhdev_com24;
		sinfo.meternum += 1;
	}////////////��ȡ��com25�ı��
	if (!ReadMeterId((int)Meterhdev_com25,sinfotmp.minfo[sinfo.meternum].meterid))
	{
		AfxMessageBox(_T("��com25��ַʧ��!")); 
	}
	else
	{
		memcpy(sinfo.minfo[sinfo.meternum].meterid, sinfotmp.minfo[sinfo.meternum].meterid, 7);
		sinfo.minfo[sinfo.meternum].mhandle = Meterhdev_com25;
		sinfo.meternum += 1;
	}///////////��ȡ��com26�ı��
	if (!ReadMeterId((int)Meterhdev_com26,sinfotmp.minfo[sinfo.meternum].meterid))
	{
		AfxMessageBox(_T("��com26��ַʧ��!")); 
	}
	else
	{
		memcpy(sinfo.minfo[sinfo.meternum].meterid, sinfotmp.minfo[sinfo.meternum].meterid, 7);
		sinfo.minfo[sinfo.meternum].mhandle = Meterhdev_com26;
		sinfo.meternum += 1;
	}

	//��ȡ��com27�ı��
	if (!ReadMeterId((int)Meterhdev_com27,sinfotmp.minfo[sinfo.meternum].meterid))
	{
		AfxMessageBox(_T("��com27��ַʧ��!")); 
	}
	else
	{
		memcpy(sinfo.minfo[sinfo.meternum].meterid, sinfotmp.minfo[sinfo.meternum].meterid, 7);
		sinfo.minfo[sinfo.meternum].mhandle = Meterhdev_com27;
		sinfo.meternum += 1;
	}////////////
	//��ȡ��com28�ı��
	if (!ReadMeterId((int)Meterhdev_com28,sinfotmp.minfo[sinfo.meternum].meterid))
	{
		AfxMessageBox(_T("��com28��ַʧ��!")); 
	}
	else
	{
		memcpy(sinfo.minfo[sinfo.meternum].meterid, sinfotmp.minfo[sinfo.meternum].meterid, 7);
		sinfo.minfo[sinfo.meternum].mhandle = Meterhdev_com28;
		sinfo.meternum += 1;
	}////////////��ȡ��com29�ı��
	if (!ReadMeterId((int)Meterhdev_com29,sinfotmp.minfo[sinfo.meternum].meterid))
	{
		AfxMessageBox(_T("��com29��ַʧ��!")); 
	}
	else
	{
		memcpy(sinfo.minfo[sinfo.meternum].meterid, sinfotmp.minfo[sinfo.meternum].meterid, 7);
		sinfo.minfo[sinfo.meternum].mhandle = Meterhdev_com29;
		sinfo.meternum += 1;
	}///////////��ȡ��com30�ı��
	if (!ReadMeterId((int)Meterhdev_com30,sinfotmp.minfo[sinfo.meternum].meterid))
	{
		AfxMessageBox(_T("��com30��ַʧ��!")); 
	}
	else
	{
		memcpy(sinfo.minfo[sinfo.meternum].meterid, sinfotmp.minfo[sinfo.meternum].meterid, 7);
		sinfo.minfo[sinfo.meternum].mhandle = Meterhdev_com30;
		sinfo.meternum += 1;
	}




}

//��ʱ
void CMeasureCommdemoDlg::OnBnClickedButton29()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	tick_second = 0;
	SetDlgItemInt(IDC_EDIT25, tick_second);//���
}

//���㷴���������
void CMeasureCommdemoDlg::OnBnClickedButton31()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString t_str;
	int i;
	SampleType |= 0x02;//��ȡ����Ͱ����
	Sleep(1000);						
	for (i = 0;i < sinfo.meternum;i++)
	{
		sinfo.minfo[i].total_f = 0;
		ReadMeterTotal((int)sinfo.minfo[i].mhandle,sinfo.minfo[i].meterid,(__int64*)&sinfo.minfo[i].total_f,(__int64*)&sinfo.minfo[i].total_r);
		ReadStateTmpr((int)sinfo.minfo[i].mhandle,sinfo.minfo[i].meterid,(signed short*)&sinfo.minfo[i].tmpr1,(signed short*)&sinfo.minfo[i].tmpr2,(unsigned short*)&timeremain);
	}
	sinfoend = sinfo;

	GetDlgItemText(IDC_EDIT4,str_err);
	t_str_err.Format(_T("%s%.3f	%s%.3f%s"),_T("��ֹkg:"),sinfoend.weight,_T("�ۼ�kg:"),sinfoend.weight -  sinfotmp.weight,_T("\r\n"));
	str_err += t_str_err;	
	t_str_err.Format(_T("%s	%s	%s	%s%s"),_T("��          ��"),_T("������ֹml"),_T("�����ۼ�ml"),_T("�������"),_T("\r\n"));
	str_err += t_str_err;

	ParaMeterTool PtFun;

	ErrRcrdFile.InitExcel();
	ErrRcrdFile.OpenWithoutSheet(_T("\\ErrRecord.xls"));
	ErrRcrdFile.LoadSheet(1,TRUE);

	int row = ErrRcrdFile.GetRowCount();
	ErrRcrdFile.SetCellInt(  row + 1,1,sinfoend.time - sinfotmp.time);
	ErrRcrdFile.SetCellFloat(row + 1,2,sinfotmp.lflow + sinfotmp.mflow);
	ErrRcrdFile.SetCellFloat(row + 1,3,sinfotmp.weight);
	ErrRcrdFile.SetCellFloat(row + 1,4,sinfoend.weight);
	ErrRcrdFile.SetCellFloat(row + 1,5,sinfoend.weight - sinfotmp.weight);
	for (i = 0;i < sinfo.meternum;i++)
	{
		sinfotmp.minfo[i].errnum = ((sinfoend.minfo[i].total_r - sinfotmp.minfo[i].total_r) / 1000.0 -\
			(sinfoend.weight - sinfotmp.weight)/0.996) / ((sinfoend.weight - sinfotmp.weight)/0.996) * 100;

		t_str_err.Format(_T("%s	%lld	%lld	%.2f%s"),\
			PtFun.ReverseMeterID(i),sinfoend.minfo[i].total_r,\
			sinfoend.minfo[i].total_r - sinfotmp.minfo[i].total_r,sinfotmp.minfo[i].errnum,_T("\r\n"));
		str_err += t_str_err;

		ErrRcrdFile.SetCellFloat(row + 1,6  + i * 5,sinfotmp.minfo[i].total_r);
		ErrRcrdFile.SetCellFloat(row + 1,7  + i * 5,sinfoend.minfo[i].total_r);
		ErrRcrdFile.SetCellFloat(row + 1,8  + i * 5,sinfoend.minfo[i].total_r - sinfotmp.minfo[i].total_r);
		ErrRcrdFile.SetCellFloat(row + 1,9  + i * 5,sinfotmp.minfo[i].errnum);
		ErrRcrdFile.SetCellFloat(row + 1,10 + i * 5,sinfoend.minfo[i].tmpr1);
	}
	ErrRcrdFile.SaveExcel();
	ErrRcrdFile.CloseExcelFile();
	ErrRcrdFile.ReleaseExcel();

	SetDlgItemText(IDC_EDIT4,str_err);
	i = ((CEdit*)GetDlgItem(IDC_EDIT4))->GetLineCount();
	((CEdit*)GetDlgItem(IDC_EDIT4))->LineScroll(i,0);
}
// ������ʱ����¶�
void CMeasureCommdemoDlg::OnBnClickedButton30()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString str_sample;
	CString t_str_sample;
	opexf.InitExcel();
	opexf.OpenWithoutSheet(_T("\\sample.xls"));
	opexf.LoadSheet(1,TRUE);
	
	GetDlgItemText(IDC_EDIT4,str_sample);
	t_str_sample.Format(_T("%s	%s	%s	%s%s"),_T("��          ��"),_T("ˮ��"),_T("ʱ���"),_T("��һ������ʱ��"),_T("\r\n"));
	str_sample += t_str_sample;

	int i;
	int t_num;
	int row;
	ParaMeterTool PtFun;

	for (i = 0;i < sinfo.meternum;i++)
	{
		if(!ReadMeterFly((int)sinfo.minfo[i].mhandle,sinfo.minfo[i].meterid, \
			(unsigned char*)&(sinfo.minfo[i].ml), (unsigned char*)&(sinfo.minfo[i].dt), (unsigned char*)&(sinfo.minfo[i].tf)))
		{
			sinfo.minfo[i].ml = 0;
			sinfo.minfo[i].dt = 0;
			sinfo.minfo[i].tf = 0;
		}	
		if (!ReadStateTmpr((int)sinfo.minfo[i].mhandle,sinfo.minfo[i].meterid,\
			(signed short*)&(sinfo.minfo[i].tmpr1),(signed short*)&(sinfo.minfo[i].tmpr2),(unsigned short*)&timeremain))
		{
			sinfo.minfo[i].tmpr1 = 0;
			sinfo.minfo[i].tmpr2 = 0;
		}
	}

	row = opexf.GetRowCount();						
	opexf.SetCellInt(row + 1, 1, sinfo.time);	    //ʱ��
	opexf.SetCellFloat(row + 1, 2, sinfo.weight);	//����Ͱ����
	opexf.SetCellFloat(row + 1, 3, sinfo.lflow);	//˲ʱ����20mm
	opexf.SetCellFloat(row + 1, 4, sinfo.mflow);	//˲ʱ����6mm
	opexf.SetCellFloat(row + 1, 5, sinfo.inpessure);	//��ˮ��ѹ��
	opexf.SetCellFloat(row + 1, 6, sinfo.outpesure);	//��ˮ��ѹ��
	opexf.SetCellFloat(row + 1, 7, sinfo.intempr);		//��ˮ���¶�
	opexf.SetCellFloat(row + 1, 8, sinfo.outtempr);		//��ˮ���¶�
	opexf.SetCellFloat(row + 1, 9, sinfo.boxtempr);		//��ˮ���¶�
	opexf.SetCellFloat(row + 1, 10, Spercent);		    //��ǰƵ��ֵ
	opexf.SetCellFloat(row + 1, 11, vavlestate);		    //��ǰ���ط���Ϣ
	for(t_num = 0; t_num < sinfo.meternum; t_num++)		//������
	{
		opexf.SetCellInt(row + 1, 12 + t_num * 5, sinfo.minfo[t_num].ml);
		opexf.SetCellInt(row + 1, 13 + t_num * 5, sinfo.minfo[t_num].dt);
		opexf.SetCellInt(row + 1, 14 + t_num * 5, sinfo.minfo[t_num].tf);
		opexf.SetCellInt(row + 1, 15 + t_num * 5, sinfo.minfo[t_num].tmpr1);
		opexf.SetCellInt(row + 1, 16 + t_num * 5, sinfo.minfo[t_num].tmpr2);

		t_str_sample.Format(_T("%s	%d	%d	%d%s"),PtFun.ReverseMeterID(t_num),sinfo.minfo[t_num].tmpr1,sinfo.minfo[t_num].dt,sinfo.minfo[t_num].tf,_T("\r\n"));
		str_sample += t_str_sample;
	}
	opexf.SaveExcel();
	opexf.CloseExcelFile();
	opexf.ReleaseExcel();

	SetDlgItemText(IDC_EDIT4,str_sample);
	i = ((CEdit*)GetDlgItem(IDC_EDIT4))->GetLineCount();
	((CEdit*)GetDlgItem(IDC_EDIT4))->LineScroll(i,0);
}
//�½��ɼ���
void CMeasureCommdemoDlg::OnBnClickedButton32()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	opexf.InitExcel();
	opexf.OpenExcelFile(_T("\\sample.xls"));
	opexf.WriteTableHead(sinfo.meternum, sinfo.minfo);
	opexf.SaveExcel();
	
	opexf.CloseExcelFile();
	opexf.ReleaseExcel();
}

//����������
void CMeasureCommdemoDlg::OnBnClickedButton33()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int i;
	ErrRcrdFile.InitExcel();
	ErrRcrdFile.OpenWithoutSheet(_T("\\ErrRecord.xls"));
	ErrRcrdFile.SaveExcel();
	ErrRcrdFile.LoadSheet(1,TRUE);

	int row = ErrRcrdFile.GetRowCount();
	ErrRcrdFile.SetCellString(row + 1,5,_T("����0"));
	ErrRcrdFile.SetCellString(row + 2,5,_T("����1"));
	ErrRcrdFile.SetCellString(row + 3,5,_T("����2"));
	ErrRcrdFile.SetCellString(row + 4,5,_T("ʱ���"));

	for (i = 0;i < sinfo.meternum;i++)
	{
		ReadFlowP((int)sinfo.minfo[i].mhandle,sinfo.minfo[i].meterid,(unsigned char*)&flowpara);
		ErrRcrdFile.SetCellFloat(row + 1,6 + i * 5,flowpara.P00);
		ErrRcrdFile.SetCellFloat(row + 1,7 + i * 5,flowpara.P01);
		ErrRcrdFile.SetCellFloat(row + 1,8 + i * 5,flowpara.P02);
		ErrRcrdFile.SetCellFloat(row + 2,6 + i * 5,flowpara.P10);
		ErrRcrdFile.SetCellFloat(row + 2,7 + i * 5,flowpara.P11);
		ErrRcrdFile.SetCellFloat(row + 2,8 + i * 5,flowpara.P12);
		ErrRcrdFile.SetCellFloat(row + 3,6 + i * 5,flowpara.P20);
		ErrRcrdFile.SetCellFloat(row + 3,7 + i * 5,flowpara.P21);
		ErrRcrdFile.SetCellFloat(row + 3,8 + i * 5,flowpara.P22);
		ErrRcrdFile.SetCellFloat(row + 4,6 + i * 5,flowpara.Td );
	}

	ErrRcrdFile.SaveExcel();
	ErrRcrdFile.CloseExcelFile();
	ErrRcrdFile.ReleaseExcel();	
}


