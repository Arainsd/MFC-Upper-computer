#ifndef _INCLUDE_H_
#define _INCLUDE_H_

extern "C"
{
	typedef unsigned char u8;
	typedef unsigned short u16;
	typedef unsigned long u32;

#pragma pack(1)
	typedef struct
	{
		unsigned char Second;
		unsigned char Minute;
		unsigned char Hour;
		unsigned char Day;
		unsigned char Month;
		unsigned short Year;
	}OprtTime;

	typedef struct
	{
		signed short P00;
		signed short P01;
		signed short P02;
		signed short P10;
		signed short P11;		
		signed short P12;
		signed short P20;
		signed short P21;		
		signed short P22;
		signed short Td;
		unsigned char Xrc;
	}FlowPram;

	typedef struct
	{
		HANDLE mhandle;			//���
		unsigned char meterid[7];//���ַ
		int	ml;
		short dt;
		unsigned short tf;
		__int64 total_f;
		__int64 total_r;
		FlowPram flowpram;
		signed char twos;
		signed char taos;
		signed short tmpr1;
		signed short tmpr2;
		float errnum;
		short flow;
	}MeterInfo;
	//�����ź�;
	typedef struct 
	{
		int valvetype;
		float weight;  //����Ͱ����;
		LONG64 time;   //��ǰʱ��;
		float lflow;   //˲ʱ����;
		float mflow;   //˲ʱС��;
		float inpessure;//���ѹ��;
		float outpesure;//����ѹ��;
		float intempr;  //����¶�;
		float outtempr; //�����¶�;
		float boxtempr; //��ˮ��ˮ��;
		int meternum;   //������;
		MeterInfo minfo[20];//��ദ��20ֻ��;
	}SampleInfo;

	typedef struct
	{
		float setflowrate;
		float actualflowrate;
		float acflowratepre;
		float err;
		float err_pre;
		float err_ppre;
		float KP;
		float KI;
		float KD;
		short Hz;
		int valvell;
		int valvels;
		int valvesl;
		int valvess;
	}PID;


#pragma pack()
	/**
	*��ʼ��ͨ�Ŷ˿�
	*@param comm:��������
	*@return �ɹ����ش��ھ��������Ϊ0
	*/
	__declspec(dllexport) HANDLE __stdcall InitComm(WCHAR* comm);
	/**
	*��ʼ��ͨ�Ŷ˿�
	*@param handle:���ھ��
	*@return �ɹ��Ǹ�������Ϊ0
	*/
	__declspec(dllexport) int __stdcall ExitComm(int handle);
	/**
	*����� 7�ֽ�
	*@param handle:���ھ��
	*@param id:���ر��
	*@return �ɹ��Ǹ�������Ϊ0
	*/
	__declspec(dllexport) int __stdcall ReadMeterId(int handle, unsigned char *id);
	/**
	*���ۼ�����
	*@param handle:���ھ��
	*@param id:���
	*@return �ɹ������ۼ�����������Ϊ0
	*/
	__declspec(dllexport) int __stdcall ReadMeterTotal(int handle, unsigned char *id,__int64 * m3_f,__int64 * m3_r);
	/**
	*��˲ʱ����
	*@param handle:���ھ��
	*@param id:���
	*@return �ɹ�����˲ʱ����������Ϊ0
	*/
	__declspec(dllexport) int __stdcall ReadMeterInstant(int handle, unsigned char *id);
}
#endif