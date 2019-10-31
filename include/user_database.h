//-------------����1.5MW˫�������鲢���������������������----201005atcpc-201007atbjtu--------------------------------
#ifndef DCDC_database_H
#define DCDC_database_H

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------//�������Զ���
#define	WR0		1						//write only
#define	RDO		2						//read only
#define WR1		4						//write enable when stop
#define	WR2		8						//write enable with password
#define	WR3		16						//write on RAM without eerom
#define	MAX_PT	32						//���ֵΪָ��
#define	MIN_PT	64						//��СֵΪָ��
#define	SIG		128						//�з�����
#define	VA0		256						//��ʾ������ֵ0(�൱�ڽ�ֹ��Ӧ�Ĺ���)
#define	UNS		512						//��ֹ�洢
#define	KEY		1024						//�鿴���޸���Ҫ����������
#define	DOT1	2048					//��������1λС����
#define	DOT2	4096					//��������2λС����
#define	DOT3	8192					//��������3λС����

//--------------------------------------//��ʾ��Ϣ���Զ���
#define	NONE	0						//��
#define	SOFT	1						//��ֹͣ
#define	SHUT	2						//��ͣ
#define	N_RCVR	4						//���ָܻ�
#define	D_RCVR	8						//��ʱ�ָ�
#define	I_RCVR	16						//�����ָ�
#define	CNT		32						//������ϴ���
#define	OT_SER	64						//��ʱ1min�������ع���
#define	SAVE	128						//��Ϣ��Ҫ����
#define	OFFCB	256						//������
#define	WARNING	512						//����

//--------------------------�궨��-----------------------------------------------
				
//-----------------------������غ궨��------------------------------------------
//�����������������(�����������ֻ��Ҫ�������ݿ��ڵ����ݺʹ˴�����Ӧ�������弴��)
#define NUM_GROUP1			29				//���鹦����ĸ�����'��������'��
#define	NUM_GROUP2			42				//�ڶ��鹦����ĸ�����'PI����'��
#define NUM_GROUP3			43				//�����鹦����ĸ�����'������ֵ'��201005atcpc
#define NUM_GROUP4			14				//�����鹦����ĸ�����'����ģʽ')
#define NUM_TIME			6				//ʱ���趨������ĸ���
#define NUM_WATCHLCD		53				//���м�ع�����ĸ���
#define NUM_ERRINF			42				//������Ϣ������ĸ���
#define NUM_ERROR			35				//��ʷ���¹�����Ϣ

//����������������
#define PARA_NUM			NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4+NUM_TIME+NUM_WATCHLCD+NUM_ERRINF+NUM_ERROR

//���ϲ���������
#define ERRO_NUM			NUM_ERROR+NUM_ERRINF

//���ϲ������һ�����������		
#define TAB_ERR_FIRST		NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4+NUM_TIME+NUM_WATCHLCD

//���ϲ����������������		
#define TAB_ERR_END			NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4+NUM_TIME+NUM_WATCHLCD+NUM_ERRINF+NUM_ERROR-1

//��ز������һ�����������(����ʱ�����)
#define BANK_FIRST			NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4+NUM_TIME				

//��ز��������һ�����������
#define BANK_END			NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4+NUM_TIME+NUM_WATCHLCD-1				

//ʱ��������һ�����������
#define TIME_FIRST			NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4				

//ʱ����������һ�����������
#define TIME_END			NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4+NUM_TIME-1	

//����дָ��������һ�����������		
#define TAB_COMSTATE_FIRST		NUM_GROUP1+NUM_GROUP2+NUM_GROUP3

//����дָ������������������		
#define TAB_COMSTATE_END			NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4-1


//���и�����λ��//
#define NO_URF				NUM_GROUP1-16	
#define NO_MIDRF			NUM_GROUP1-15
#define NO_NIQRF			NUM_GROUP1-14
#define NO_MIQRF			NUM_GROUP1-13
#define NO_TOQRF            NUM_GROUP1-12
#define NO_AGLRF			NUM_GROUP1-11

//--------------------------------------//��������ͽṹ��
struct PARA
{
 Uint16 *para_add;						//������ַָ��
 Uint16 init;							//��ʼֵ
 Uint16	min,max;						//��Сֵ ���ֵ
 Uint16	*min_add,*max_add;				//��Сֵ ���ֵָ��
 Uint16 attr;							//����
};

struct MESSAGE
{
 Uint16	name[3];						//����
 Uint16 rank;							//��Ϣ����
 Uint16 attr;							//����
};

//---------------------------������Ϣ���-systest----------------------------------------------
#define MSG_NUM				45				//������Ϣ����

#define	MSG_NONE			0				//����Ϣ
#define	MSG_CODEOK			1				//EEPROM����
#define	MSG_EE_FAIL			2				//EEPROM����
#define	MSG_ESTOP			3				//��ͣ����
#define	MSG_CBTRIP			4				//�����ѿ۹���
#define	MSG_EXFAULT 		5				//�������ⲿӲ������
#define	MSG_CAN_FAIL		6				//CANͨѶ����
#define	MSG_ENCODFAULT		7				//����������
#define	MSG_CBERROR 		8				//���Ϻ�բ����
#define	MSG_PREERROR 		9				//Ԥ������
#define	MSG_MAINKERROR 		10				//���Ӵ����պϹ���
#define	MSG_MAINFERROR 		11				//���˲����պϹ���
#define	MSG_STACERROR 		12				//���ӽӴ����պϹ���
#define	MSG_HIA1			13				//����,A��Ӳ������
#define	MSG_HIB1			14				//����,B��Ӳ������
#define	MSG_HIC1			15				//����,C��Ӳ������
#define	MSG_HIA2			16				//�����,A��Ӳ������
#define	MSG_HIB2			17  			//�����,B��Ӳ������
#define	MSG_HIC2			18				//�����,C��Ӳ������
#define	MSG_UDCOV           19	            //Ӳ������м�ֱ����ѹ��ѹ
#define	MSG_HIAC1			20				//����Ӳ������
#define	MSG_HIAC2			21				//�����Ӳ������
#define MSG_PDPASERIES      22            	//�������ع���
#define MSG_PDPBSERIES      23            	//��������ع���
#define	MSG_MSPEEDOUT 		24				//ת�ٳ�����Χ����
#define	MSG_SIAC1			25				//�����������
#define	MSG_SIAC2			26				//������������
#define	MSG_FE1				27				//����Ƶ�ʴ���
#define	MSG_SUDCOV			28				//����м�ֱ����ѹ��ѹ
#define	MSG_SUACOV1			29				//�����ѹ������ѹ
#define	MSG_SIDCOV			30				//��Ϊdc-chopper��������BJTULVRT201204
//#define	MSG_SUDCLV			30				//����м�ֱ����ѹǷѹ
#define	MSG_SUACLV1			31				//�����ѹ����Ƿѹ��ʧ��
#define	MSG_UDCWAVE			32				//�м��ѹ��������
#define	MSG_SIOVST	    	33				//���ӵ�����������
#define	MSG_GFAULT1			34				//����ӵع���
#define	MSG_GFAULT2		   	35				//����ӵع���
#define	MSG_TOV1			36				//����SKIIP����
#define	MSG_TOV2			37  			//�����SKIIP����
#define	MSG_SKTOV			38  			//����ж�SKIIP����
#define	MSG_TLOV1			39				//�����г���
#define MSG_TLOV2           40            	//�����г���
#define	MSG_PGOV			41				//�����й�����
#define MSG_QGOV            42            	//�����޹�����
#define	MSG_STASYNFAIL 		43				//����ͬ������ʧ�ܹ���
#define	MSG_PHORDE			44				//�����������

//--------------------------------------//�����ܱ�
const struct PARA FUNC[PARA_NUM]={
//		����   		��ֵ		��Сֵ		���ֵ		��Сֵָ��	���ֵָ��	����		˵��
//-------------------------------'��������'-------------------------------------------------------
//--29/0-28
	{&_MC_TYPE,		MC_TYPE,	MC_TYPE,	MC_TYPE,	&_NULL_VAL,	&_NULL_VAL,	RDO},		//���Ͳ� 
	{&_MC_REV, 		MC_REV,		MC_REV,		MC_REV,		&_NULL_VAL,	&_NULL_VAL,	KEY},		//����汾��
	{&_MC_DATE,		MC_DATE,	0,	    	65535,		&_NULL_VAL,	&_NULL_VAL,	KEY},		//�������� 
	{&_MC_ID,		MC_ID,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	KEY},		//��Ʒ��� 
	{&_SW_FR,		210,		200,		1800,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT2},	//����Ƶ��(0.01kHz)
	
	{&_DEADTIME,	8,			0,			20,			&_NULL_VAL,	&_NULL_VAL,	WR1},		//����ʱ��(us)
	{&_COMPTIME,	45,			0,			50,			&_NULL_VAL,	&_NULL_VAL,	WR1+DOT1},	//��������ʱ��(ms)
	{&_MINONTIME,	4,			0,			20,			&_NULL_VAL,	&_NULL_VAL,	WR1},		//��С��ͨʱ��(us)
	{&_RU_UDCT,		10000,		1,			20000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//��ѹָ�������������/�½�ʱ��(ms)
	{&_RU_IDT,		2,			1,			20000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//����ָ�������������/�½�ʱ��(ms)
	
	{&_RU_IQT,		10,			1,			20000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//����ָ�������������/�½�ʱ��(ms)
	{&_RU_TOQT,		500,		1,			20000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//ת��ָ�������������/�½�ʱ��(ms)
	{&_RU_AGLT,		500,		1,			20000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//����������ָ�������������/�½�ʱ��(ms)
	{&_URF,			1100,		0,			1200,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//�м�ֱ����ѹ����ֵ
	{&_MIDRF,		0,			0,		    550,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//�����޹���������ֵ
	
	{&_NIQRF,		65,			-200,		200,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//�����޹���������ֵ 20140503LJD��������
	{&_MIQRF,		0,			0,			700,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//�����й���������ֵ
    {&_TOQRF,		0,			0,			9000,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},   //����ת�ظ���ֵ
    {&_AGLRF,		0,			-36,		36,	   		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},   //���๦�������Ǹ���ֵ
	{&_EIDCO,		1000,		0,			10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//��������ϵ��

	{&_ENCODPOS,	4503,		0,			6283,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//�����ʼλ�ýǶ� 0--2PIE 201005atcpc 20140503LJD��������	
	{&_STDBY1,		5000,  	    0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//����1
	{&_STDBY2,		135,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	//����2 20140503LJD��������
	{&_STDBY3,		5500,	    0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//����3
	{&_STDBY4,		5,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//����4

	{&_STDBY5,		10,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//����5
	{&_STDBY6,		604,		-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//����6
	{&_STDBY7,		10,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//����7
//	{&_STDBY8,		0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//����8
	{&_MC_OK,		1,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//����8������ɱ�־MagnetCurve2013-12-13--ZZJ
	
//-------------------------------'PI����'---------------------------------------------------------
//--42/29-70
	{&_NPR_U_Kp,			40,		1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	//�����ѹ������ϵ��
	{&_NPR_U_Ki,			1000,	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1+VA0},//�����ѹ������ϵ��
	{&_NPR_U_Kd,			0,		0,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3+VA0},//�����ѹ������ϵ��
	{&_NPR_U_outmax,		600,	1,		1000,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//�����ѹ�ջ�����޷�
	{&_NPR_U_errmax,		1000,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//�����ѹ�ջ���������޷�
	{&_NPR_U_errmin,		0,		0,		2000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//�����ѹ���������
	{&_NPR_U_incrementmax,  1000,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//�����ѹ�������޷�

	{&_NPR_ID_Kp,			7000, 	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	//��������ջ�����ϵ��
	{&_NPR_ID_Ki,			1000,	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1+VA0},//��������ջ�����ϵ��
	{&_NPR_ID_Kd,			0,		0,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3+VA0},//��������ջ�����ϵ��
	{&_NPR_ID_outmax,		10,		1,		500,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//��������ջ�����޷�
	{&_NPR_ID_errmax,		6200,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//��������ջ���������޷�
	{&_NPR_ID_errmin,		1200,	0,		2000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//��������ջ���������޷�
	{&_NPR_ID_incrementmax,	4000, 	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//��������ջ������޷�

	{&_NPR_IQ_Kp,			40, 	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	//��������ջ�����ϵ�� 201005atcpc
	{&_NPR_IQ_Ki,			1670,	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1+VA0},//��������ջ�����ϵ��
	{&_NPR_IQ_Kd,			0,		0,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3+VA0},//��������ջ�����ϵ��
	{&_NPR_IQ_outmax,		200,	1,		500,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//��������ջ�����޷�
	{&_NPR_IQ_errmax,		500,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//��������ջ���������޷�
	{&_NPR_IQ_errmin,		0,		0,		2000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//��������ջ���������޷�
	{&_NPR_IQ_incrementmax,	550, 	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//��������ջ������޷�

	{&_MPR_ID_Kp,			40, 	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	//��������������ջ�����ϵ��
	{&_MPR_ID_Ki,			1600,	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1+VA0},//��������������ջ�����ϵ��
	{&_MPR_ID_Kd,			0,		0,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3+VA0},//��������������ջ�����ϵ��
	{&_MPR_ID_outmax,		200,	1,		500,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//��������������ջ�����޷�
	{&_MPR_ID_errmax,		500,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//��������������ջ�����޷�
	{&_MPR_ID_errmin,		0,		0,		2000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//��������������ջ�����޷�
	{&_MPR_ID_incrementmax,	550, 	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//��������������ջ������޷�

	{&_MPR_IQ_Kp,			15, 	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	//��������������ջ�����ϵ��
	{&_MPR_IQ_Ki,			2000,	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1+VA0},//��������������ջ�����ϵ��
	{&_MPR_IQ_Kd,			0,		0,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3+VA0},//��������������ջ�����ϵ��
	{&_MPR_IQ_outmax,		200,	1,		500,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//��������������ջ�����޷�
	{&_MPR_IQ_errmax,		500,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//��������������ջ�����޷�
	{&_MPR_IQ_errmin,		0,		0,		2000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//��������������ջ�����޷�
	{&_MPR_IQ_incrementmax,	550, 	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//��������������ջ������޷�
/*
	{&_DYN_U_Kp,			40,	    1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	  //��̬��ѹ�ջ�����ϵ��
	{&_DYN_U_Ki,			50,		1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1+VA0},//��̬��ѹ�ջ�����ϵ��//201007BJTULVRT
	{&_DYN_U_Kd,			0,		0,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3+VA0},//��̬��ѹ�ջ�����ϵ��
	{&_DYN_U_outmax,		600,	1,		1000,		&_NULL_VAL,	&_NULL_VAL,	WR0},		  //��̬��ѹ�ջ�����޷� 
	{&_DYN_U_errmax,		1000,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	  //��̬��ѹ�ջ�����޷� 
	{&_DYN_U_errmin,		0,		0,		2000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	  //��̬��ѹ�ջ������С� 
	{&_DYN_U_incrementmax,	1000, 	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	  //��̬��ѹ�ջ������޷�
*/
	{&_DYN_U_Kp,			40,		1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	  //��̬��ѹ�ջ�����ϵ��
	{&_DYN_U_Ki,			1000,	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1+VA0},//��̬��ѹ�ջ�����ϵ��//201007BJTULVRT
	{&_DYN_U_Kd,			0,		0,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3+VA0},//��̬��ѹ�ջ�����ϵ��
	{&_DYN_U_outmax,		600,	1,		1000,		&_NULL_VAL,	&_NULL_VAL,	WR0},		  //��̬��ѹ�ջ�����޷� 
	{&_DYN_U_errmax,		1000,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	  //��̬��ѹ�ջ�����޷� 
	{&_DYN_U_errmin,		0,		0,		2000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	  //��̬��ѹ�ջ������С� 
	{&_DYN_U_incrementmax,	1000, 	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	  //��̬��ѹ�ջ������޷�

//-------------------------------'������ֵ'---201007BJTULVRT------------------------------------------------------
//--35/71-113		
	{&_SC_RTRN,		5,			0,			20,			&_NULL_VAL,	&_NULL_VAL,	WR1},		//����������Դ���
	{&_SC_RTRT,		15,			10,			600,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//���ι������Լ��ʱ��(s)����ʱ�ָ�ʱ��
	{&_SC_UDCOV,	1300,		1130,       1300,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//�м�ֱ����ѹ ��ѹ����ֵ(V) 
	{&_SC_IDCOV,	650,    	0,       	1100,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//�м�ֱ����ѹ Ƿѹ����ֵ(V) 
	{&_SC_VDCON,	750,		700,       	900,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//�м�ֱ����ѹ �ϵ���ɱȽ���ֵ(V) 	

	{&_SC_IACOV1,	180, 		20,			200,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//���ཻ������ ��������ֵ(��׼�����İٷ�ֵ)
	{&_SC_UACOV1,	770,		690,		828,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//�����ѹ ��ѹ����ֵ(V) 20140503LJD��������
	{&_SC_UACLV1,	610,	   	0,			690,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//�����ѹ Ƿѹ����ֵ(V) 20140503LJD��������
//	{&_SC_UACLV1,	100,	   	0,			690,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//�����ѹ Ƿѹ����ֵ(V) 201005atcpc
	{&_SC_IACOV2,	180, 		20,			200,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//����ཻ������ ��������ֵ(��׼�����İٷ�ֵ)
	{&_SC_PGOV,		1800,		0,			3000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//�������й����ʹ�����ֵ(kW) 201005atcpc	

	{&_SC_QGOV,		1000,		0,			3000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//�������޹����ʹ�����ֵ(kW)
	{&_SC_IACOVST,	150, 		20,			200,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//���Ӳཻ������ ��������ֵ(��׼�����İٷ�ֵ)
	{&_SC_NPR_TLOV,	160, 		20,	    	200,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//������PT100���±���ֵ����λ���϶�
	{&_SC_MPR_TLOV,	160, 		20,	    	200,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//�������PT100���±���ֵ����λ���϶�
	{&_SC_LVIAC2,	190, 		20,			240,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//SCR����ཻ������ ��������ֵ(��׼�����İٷ�ֵ) 201005atcpc

	{&_SC_LVUDC1,	1180,		1100,       1350,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//��������� �м�ֱ����ѹ1 ��ѹ����ֵ(V)
	{&_SC_LVUDC2,	1200,		1100,       1350,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//SCR���� �м�ֱ����ѹ2 ��ѹ����ֵ(V)
	{&_SC_TSCRON,	25,			10,         1000,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//SCR��ͨʱ�䣨ms��
	{&_SC_MSPEED1,	950, 		800,	    1500,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//ת�ٱ��� Ƿ�ٱ���ֵ
	{&_SC_MSPEED2,	2030, 		1800,	    2200,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//ת�ٱ��� ���ٱ���ֵ

	{&_SC_UDSTAC,	50,		    0,			150,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//���ӽӴ�����բ��ѹ����ֵ
	{&_SC_SKPTOV,	100,		20,			110,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//IGBT����
	{&_SC_FREQ1,	515,		500,		550,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT1},	//Ƶ�ʷ�Χ����
	{&_SC_FREQ2,	475,		450,		500,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT1},	//Ƶ�ʷ�Χ����	
	{&_SCIB_BAUD,	19200,		1200,		57600,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//485������

	{&_SCIB_ADDR,	1,			1,			31,			&_NULL_VAL,	&_NULL_VAL,	WR1},		//485�ڵ��ַ	
	{&_PROSTDBY1,	500,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//����
	{&_PROSTDBY2,	665,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//����

//201005atcpc
	{&_SC_Rs,		7747,		0,			15000,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT3},	//���ӵ���(mohm)
	{&_SC_Lls,		284,		1,			1000,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT3},	//����©��(mH)
	{&_SC_Rr,		4175,		0,			15000,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT3},	//ת�ӵ���(mohm)	
	{&_SC_Llr,		456,		1,			1000,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT3},	//ת��©��(mH)
	{&_SC_Lm,		17870,		1,			50000,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT3},	//���ŵ翹(mH)

	{&_SC_POLES,	2,		    1,			8,			&_NULL_VAL,	&_NULL_VAL,	WR1},		//������
	{&_SC_Usn,		690,		380,		690,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//���Ӷ��ѹ(V)
	{&_SC_Uro,		2018,		380,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//ת�ӿ�·��ѹ(V)	
	{&_SC_PLSPRVL,	2048,		1,			10000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//������ÿȦ��դ����
	{&_SC_MSTDBY,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//����������� MagnetCurve2013-12-13--ZZJ

	{&_SC_EXISTY1,	890,		100,		3000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},		//������ŵ��ڱ���1 MagnetCurve2013-12-13--ZZJ
	{&_SC_EXISTY2,	930,		100,		3000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},		//������ŵ��ڱ���2 MagnetCurve2013-12-13--ZZJ
	{&_SC_EXISTY3,	1000,		100,		3000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},		//������ŵ��ڱ���3	MagnetCurve2013-12-13--ZZJ
	{&_SC_EXISTY4,	1100,		100,		3000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},		//������ŵ��ڱ���4 MagnetCurve2013-12-13--ZZJ
	{&_SC_EXISTY5,	1290,		100,		3000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},		//������ŵ��ڱ���5 MagnetCurve2013-12-13--ZZJ

//-------------------------------'ģ������������'-----------------------------------------------------
//--14/114-127
	{&_COMMAND1,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR3},		//����ָ���1��
	{&_COMMAND2,	0,			0,		    65535,		&_NULL_VAL,	&_NULL_VAL,	WR3},	    //����ָ���2�� 2048--0x0800 2013-12-9 ZZJ
	{&_STDBY9,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR3},		//����9	20141004 LJD
	{&_PCEROTRIG,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//PC����ʾ������������ѡ�� 20100308
	{&_STATE1, 		0,	  	    0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//����״̬1

	{&_STATE2,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},	    //����״̬2
	{&_DISPLAY,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//��ʾ
	{&_INPUT,		0,		    0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},   	//IO����״̬
	{&_OUTPUT,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//IO���״̬
	{&_LVRT,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//LVRT״̬���λ

	{&_STDBY11,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//����11
	{&_STDBY12,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//����12
	{&_STDBY13,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//����13
	{&_STDBY14,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//����14

//-------------------------------ʱ���趨-----------------------------------------------------------
//--6/128-133
	{&RTIMER.time[0],	0,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//���¹���ʱ��(��)
	{&RTIMER.time[1],	0,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//���¹���ʱ��(��)
	{&RTIMER.time[2],	0,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//���¹���ʱ��(ʱ)
	{&RTIMER.time[3],	1,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//���¹���ʱ��(��)
	{&RTIMER.time[4],	1,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//���¹���ʱ��(��)
	{&RTIMER.time[5],	10,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//���¹���ʱ��(��)


//-------------------------------���м��----------------------------------------------------------
//--53/134-186
	{&_BA_MIDRF,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//d�����ָ��(A)
	{&_BA_NIQRF,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//q�����ָ��(A)
	{&_BA_MIQRF,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//q�����ָ��(A)
	{&_BA_TOQRF,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},	//ת��ָ��(NM)
	{&_BA_AGLRF,	0,			-90,		90,	    	&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},	//�޹��Ƕ�ָ��()

	{&_BA_URF,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//�м�ֱ����ѹ����ֵ(V)
	{&_BA_UDC,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//�м�ֱ����ѹ(V)	
	{&_BA_IA1,		0,			0,    		65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},	//���������A�ಢ��������Чֵ(A)
	{&_BA_IB1,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},	//���������B�ಢ�������Ч�?A)
	{&_BA_IC1,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},	//���������C�ಢ��������Чֵ(A)

	{&_BA_IA2,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},	//����������Aಢ��������Чֵ(A)
	{&_BA_IB2,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},	//����������B�ಢ�������Ч�?A)	
	{&_BA_IC2,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},	//����������C�ಢ��������Чֵ(A)
	{&_BA_UAB1,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//����AB�ߵ�ѹ��Чֵ(V)
	{&_BA_UBC1,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//����BC�ߵ�ѹ��Чֵ(V)

	{&_BA_UAB2,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//20091027atzyŲ������ѹ������ �����AB�ߵ�ѹ��Чֵ(V)
	{&_BA_UBC2,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//20091027atzyŲ������ѹ������ �����BC�ߵ�ѹ��Чֵ(V)
	{&_BA_FREQ,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//����Ƶ��(hz)
	{&_BA_UAB0,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//����ǰAB�ߵ�ѹ��Чֵ(V)
	{&_BA_UBC0,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//����ǰBC�ߵ�ѹ��Чֵ(V)

	{&_BA_TLAC,		0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},	//�������¶�(���϶�)20091019atzy
	{&_BA_TLDUDT,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},	//�������¶�(���϶�)20091019atzy
	{&_BA_TNSKIIP,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//����SKIIP�¶�(���϶�) CPC
	{&_BA_TMSKIIP,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//����SKIIP�¶�(���϶�) cpc
	{&_BA_SPEED,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//���ת��(rpm)

	{&_BA_PIONU,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//�����ѹ�����
	{&_BA_PIONID,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//����d����������
	{&_BA_PIONIQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//����q����������
	{&_BA_MEXI,	    0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//
	{&_BA_PIOMID,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//����d����������

	{&_BA_PIOMIQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//����q����������
	{&_BA_GRDUD,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//d�����Ϻ���ѹ���� �˲���
	{&_BA_MC_IN,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT2},		//����У������MagnetCurve2013-12-13--ZZJ
//	{&_BA_STAUD,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//���ӵ�ѹd
	{&_BA_GRDUQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//q�����Ϻ���ѹ���� �˲���
	{&_BA_MC_K,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT3},		//����У�����MagnetCurve2013-12-13--ZZJ
//	{&_BA_STAUQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//���ӵ�ѹq


	{&_BA_NPRID,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//d��������� �˲��� 201005atcpc
	{&_BA_NPRIQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//q��������� �˲��� 201005atcpc
//	{&_BA_EXCID,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//�������ŵ���d��
//	{&_BA_EXCIQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	 //�������ŵ���q��
	{&_BA_SCRIA,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//SCR-A	20110906
	{&_BA_SCRIB,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//SCR-B	20110906
//	{&_BA_STAUABD,	0,			0,    		65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},		//20091027atzyŲ������ѹ������ ����ͬ������ǰ����ǰ���ѹ��
	{&_BA_STAUABD,	0,			-10000,		10000,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//�������ŵ�ѹ��--����У����������MagnetCurve2013-12-13--ZZJ

	{&_BA_STAUBCD,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},	    //20091027atzyŲ������ѹ������ ����ͬ������ǰ����ǰ���ѹ��
	{&_BA_STAIAC,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},		//�����ߵ�����Чֵ��ʾ cpc
	{&_BA_STAIBA,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},		//�����ߵ�����Чֵ��ʾcpc
	{&_BA_TOQFB,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//�ط���(NM)
	{&_BA_PSTA,		0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���Ӳ��й�������ʾkW

	{&_BA_PNPR,		0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//�����й�������ʾ
	{&_BA_PGRID,	0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//�����ܵ��й�������ʾ 
	{&_BA_QSTA,		0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���Ӳ��޹�������ʾkVAR
	{&_BA_QNPR,		0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//�����޹�������ʾ
	{&_BA_QGRID,	0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//�����ܵ��޹�������ʾ

	{&_BA_SSTA,		0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���Ӳ����ڹ�����ʾ
	{&_BA_SNPR,		0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//�������ڹ�����ʾ
	{&_BA_SGRID,	0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//�����ܵ��й�������ʾ 


//-------------------------------'������Ϣ'--------------------------------------------------------
//--42/187-228
	{&_BA_EMIDRF,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� d����ָ��(A)
	{&_BA_ENIQRF,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� q�����(A)
	{&_BA_EMIQRF,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� q�����ָ��(A)
	{&_BA_ETOQRF,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//���¹���ʱ�� ת��ָ��(A)
	{&_BA_EURF,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//���¹���ʱ�� �м�ֱ����ѹ����ֵ(V)

	{&_BA_EAGLRF,	0,			-90,		90,	    	&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//���¹���ʱ�� �޹��Ƕ�ָ��()   07.27
	{&_BA_EUDC,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//���¹���ʱ�� �м�ֱ����(V)	
	{&_BA_EIA1,		0,			-32767,     32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� ��������A�ಢ��������Чֵ(A)
	{&_BA_EIB1,		0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� ���������B�ಢ��������Чֵ(A)
	{&_BA_EIC1,		0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� ���������C�ಢ��������Чֵ(A)

	{&_BA_EIA2,		0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� ���������A�ಢ��������Чֵ(A)
	{&_BA_EIB2,		0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� ����������B�ಢ��������Чֵ(A)	
	{&_BA_EIC2,		0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� ����������C�ಢ��������Чֵ(A)
	{&_BA_EUAB1,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//���¹���ʱ�� ����AB�ߵ�ѹ��ֵ(V)
	{&_BA_EUBC1,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//���¹��ϱ�� ����BC�ߵ�ѹ�Чֵ(V)

	{&_BA_EUAB2,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//���¹���ʱ�� �����AB�ߵ�ѹ��Чֵ(V)
	{&_BA_EUBC2,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//���¹���ʱ�� �����BC�ߵ�ѹ��Чֵ(V)	
    {&_BA_EFREQ,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//���¹���ʱ�� ����Ƶ��(hz)
	{&_BA_EUAB0,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//���¹���ʱ�� ����ǰAB�ߵ�ѹ��Чֵ(V)
	{&_BA_EUBC0,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//���¹���ʱ�� ����ǰBC�ߵ�ѹ��Чֵ(V)

	{&_BA_ETLAC,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//���¹���ʱ�� �������¶�(���϶�) 20091019atzy
	{&_BA_ETLDUDT,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//���¹���ʱ�� �������¶�(���϶�)	20091019atzy
	{&_BA_ETSKIIP,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//���¹���ʱ�� SKIPP�¶�(���϶�)
	{&_BA_ESPEED,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//���¹���ʱ�� ���ת��(rpm)
	{&_BA_ENPRUD,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//���¹���ʱ�� d�����Ϻ���ѹ����	20091019atzy

	{&_BA_ENPRUQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//���¹���ʱ�� q�����Ϻ���ѹ����	20091019atzy
	{&_BA_ENPRUD2,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//���¹���ʱ�� d�����Ϻ���ѹ���� �˲���	20091019atzy
	{&_BA_ENPRUQ2,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//���¹���ʱ�� q�����Ϻ���ѹ���� �˲���	20091019atzy
	{&_BA_ENUDOUT,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//���¹���ʱ�� ����d���ѹ���	20091019atzy
	{&_BA_ENUQOUT,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//���¹���ʱ�� ����q���ѹ���	20091019atzy

	{&_BA_EPIONU,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� �����ѹ�����
	{&_BA_EPIONID,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� ����d���������
	{&_BA_EPIONIQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� ����q����������
	{&_BA_EMEXI,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� �����������ۼ���ֵ��ʾ 07.27
	{&_BA_EPIOMID,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� ����d����������

	{&_BA_EPIOMIQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� ����q����������
	{&_BA_ESTAIAC,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},		//���¹���ʱ�� �����ߵ�����Чֵ��ʾ cpc 07.27
	{&_BA_ESTAIBA,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},		//���¹���ʱ�� �����ߵ�����Чֵ��ʾcpc 07.27
	{&_BA_ETOQFB,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//���¹���ʱ�� ת�ط���(NM) 07.27
	{&_BA_EPSTA,	0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� ���Ӳ��й�������ʾkW 07.27

	{&_BA_EPNPR,	0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� �����й�������ʾ 07.27
	{&_BA_EPGRID,	0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//���¹���ʱ�� �����ܵ��й�������ʾ  07.27

	

//-------------------------------'����ԭ��/ʱ��'--------------------------------------------------------
//--28/229-256	
	{&_BA_ERR4,		0,			0,			100,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//���¹�����Ϣ
	{&_BA_TIME4_0,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//���¹���ʱ��(��)
	{&_BA_TIME4_1,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//�¹���ʱ��(��)
	{&_BA_TIME4_2,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//���¹���ʱ��(ʱ)
	{&_BA_TIME4_3,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//���¹���ʱ��(��)
	{&_BA_TIME4_4,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//���¹���ʱ��(��)	
	{&_BA_TIME4_5,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//���¹���ʱ��(��)

	{&_BA_ERR3,		0,			0,			100,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//�����ι�����Ϣ 
	{&_BA_TIME3_0,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//�����ι���ʱ��(��)
	{&_BA_TIME3_1,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//�����ι���ʱ��(��)
	{&_BA_TIME3_2,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//�����ι���ʱ��(ʱ)
	{&_BA_TIME3_3,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//�����ι���ʱ��(��)	
	{&_BA_TIME3_4,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//�����ι���ʱ��(��)
	{&_BA_TIME3_5,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//�����ι���ʱ��(��)

	{&_BA_ERR2,		0,			0,			100,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//�ڶ��ι�����Ϣ 
	{&_BA_TIME2_0,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//�ڶ��ι���ʱ��(��)
	{&_BA_TIME2_1,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//�ڶ��ι���ʱ��(��)
	{&_BA_TIME2_2,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//�ڶ��ι���ʱ��(ʱ)	
	{&_BA_TIME2_3,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//�ڶ��ι���ʱ��(��)
	{&_BA_TIME2_4,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//�ڶ��ι���ʱ��(��)
	{&_BA_TIME2_5,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//�ڶ�ι���ʱ�?��)

	{&_BA_ERR1,		0,			0,			100,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//��һ�ι�����Ϣ 
	{&_BA_TIME1_0,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//��һ�ι���ʱ��(��)
	{&_BA_TIME1_1,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//��һ�ι���ʱ��(��)
	{&_BA_TIME1_2,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//��һ�ι���ʱ��(ʱ)
	{&_BA_TIME1_3,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//��һ�ι���ʱ��(��)
	{&_BA_TIME1_4,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//ڴ���(��)
	{&_BA_TIME1_5,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//��һ�ι���ʱ��(��)	                                    	

	{&_BA_ERR5,		0,			0,			100,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//ʾ����������Ϣ 201005atcpc
	{&_BA_TIME5_0,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//ʾ��������ʱ��(��)
	{&_BA_TIME5_1,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//ʾ��������ʱ��(��)
	{&_BA_TIME5_2,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//ʾ��������ʱ��(ʱ)
	{&_BA_TIME5_3,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//ʾ��������ʱ��(��)
	{&_BA_TIME5_4,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//ʾ��������ʱ��(��)
	{&_BA_TIME5_5,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//ʾ��������ʱ��(��)	                                    	

//-------------------------------------------------------------------------------------------------
};

//--------------------------------------//������Ϣ�ӱ�(������ݵ����--70)20091026atzy
const Uint16 TAB_ERR[ERRO_NUM]={
	TAB_ERR_FIRST   ,TAB_ERR_FIRST+ 1,TAB_ERR_FIRST+ 2,TAB_ERR_FIRST+ 3,
	TAB_ERR_FIRST+ 4,TAB_ERR_FIRST+ 5,TAB_ERR_FIRST+ 6,TAB_ERR_FIRST+ 7,
	TAB_ERR_FIRST+ 8,TAB_ERR_FIRST+ 9,TAB_ERR_FIRST+10,TAB_ERR_FIRST+11,
	TAB_ERR_FIRST+12,TAB_ERR_FIRST+13,TAB_ERR_FIRST+14,TAB_ERR_FIRST+15,
	TAB_ERR_FIRST+16,TAB_ERR_FIRST+17,TAB_ERR_FIRST+18,TAB_ERR_FIRST+19,
	TAB_ERR_FIRST+20,TAB_ERR_FIRST+21,TAB_ERR_FIRST+22,TAB_ERR_FIRST+23,
	TAB_ERR_FIRST+24,TAB_ERR_FIRST+25,TAB_ERR_FIRST+26,TAB_ERR_FIRST+27,
	TAB_ERR_FIRST+28,TAB_ERR_FIRST+29,TAB_ERR_FIRST+30,TAB_ERR_FIRST+31,
	TAB_ERR_FIRST+32,TAB_ERR_FIRST+33,TAB_ERR_FIRST+34,TAB_ERR_FIRST+35,
	TAB_ERR_FIRST+36,TAB_ERR_FIRST+37,TAB_ERR_FIRST+38,TAB_ERR_FIRST+39,
	TAB_ERR_FIRST+40,TAB_ERR_FIRST+41,TAB_ERR_FIRST+42,TAB_ERR_FIRST+43,
	TAB_ERR_FIRST+44,TAB_ERR_FIRST+45,TAB_ERR_FIRST+46,TAB_ERR_FIRST+47,
	TAB_ERR_FIRST+48,TAB_ERR_FIRST+49,TAB_ERR_FIRST+50,TAB_ERR_FIRST+51,
	TAB_ERR_FIRST+52,TAB_ERR_FIRST+53,TAB_ERR_FIRST+54,TAB_ERR_FIRST+55,
	TAB_ERR_FIRST+56,TAB_ERR_FIRST+57,TAB_ERR_FIRST+58,TAB_ERR_FIRST+59,
    TAB_ERR_FIRST+60,TAB_ERR_FIRST+61,TAB_ERR_FIRST+62,TAB_ERR_FIRST+63,
    TAB_ERR_FIRST+64,TAB_ERR_FIRST+65,TAB_ERR_FIRST+66,TAB_ERR_FIRST+67,
    TAB_ERR_FIRST+68,TAB_ERR_FIRST+69,TAB_ERR_FIRST+70,TAB_ERR_FIRST+71,
    TAB_ERR_FIRST+72,TAB_ERR_FIRST+73,TAB_ERR_FIRST+74,TAB_ERR_FIRST+75,
    TAB_ERR_FIRST+76
};

//--------------------------------------//�������ϵ����Ϣ�ӱ�(80%��115%��ѹʱ(540V--800V)��ʵ�ird*/���ۼ���ird*)
const float TAB_EXI[128]={
 0.9743,    0.9750,    0.9758,    0.9766,    0.9774,    0.9783,    0.9792,
 0.9801,    0.9811,    0.9820,    0.9830,    0.9841,    0.9851,    0.9862,
 0.9873,    0.9884,    0.9896,    0.9907,    0.9919,    0.9932,    0.9944,
 0.9957,    0.9970,    0.9983,    0.9997,    1.0010,    1.0024,    1.0039,
 1.0053,    1.0068,    1.0083,    1.0098,    1.0114,    1.0129,    1.0146,

 1.0162,    1.0178,    1.0195,    1.0213,    1.0230,    1.0248,    1.0266,
 1.0284,    1.0303,    1.0322,    1.0342,    1.0361,    1.0381,    1.0402,
 1.0423,    1.0444,    1.0465,    1.0487,    1.0510,    1.0532,    1.0556,
 1.0579,    1.0603,    1.0628,    1.0653,    1.0678,    1.0704,    1.0731,
 1.0758,    1.0785,    1.0813,    1.0842,    1.0871,    1.0901,    1.0931,

 1.0962,    1.0994,    1.1026,    1.1059,    1.1092,    1.1127,    1.1162,
 1.1197,    1.1234,    1.1271,    1.1309,    1.1348,    1.1388,    1.1428,
 1.1470,    1.1512,    1.1555,    1.1599,    1.1644,    1.1690,    1.1737,
 1.1785,    1.1834,    1.1884,    1.1935,    1.1987,    1.2040,    1.2095,
 1.2150,    1.2207,    1.2265,    1.2324,    1.2384,    1.2446,    1.2509,

 1.2574,    1.2639,    1.2707,    1.2775,    1.2845,    1.2917,    1.2990,
 1.3064,    1.3140,    1.3218,    1.3297,    1.3378,    1.3460,    1.3545,
 1.3631,    1.3718,    1.3808,    1.3899,    1.3992,    1.4088,    1.4185,
 1.4284,    1.4384 
}; 

//--------------------------------------//�������ϵ���ӱ�(�������ص���)
const float TAB_IQEXI[128]={
 1.0000,   1.0239,    1.0319,    1.0398,    1.0476,    1.0554,    1.0631,    1.0707,    1.0783,    1.0858,    1.0932,    1.1006,
 1.1079,   1.1151,    1.1222,    1.1293,    1.1363,    1.1432,    1.1501,    1.1569,    1.1636,    1.1702,    1.1768,    1.1833,
 1.1898,   1.1961,    1.2024,    1.2087,    1.2148,    1.2209,    1.2269,    1.2329,    1.2387,    1.2445,    1.2503,    1.2559,
 1.2615,   1.2671,    1.2725,    1.2779,    1.2832,    1.2884,    1.2936,    1.2987,    1.3037,    1.3087,    1.3136,    1.3184,
 1.3232,   1.3278,    1.3324,    1.3370,    1.3414,    1.3458,    1.3502,    1.3544,    1.3586,    1.3627,    1.3668,    1.3707,
 1.3746,   1.3785,    1.3822,    1.3859,    1.3896,    1.3931,    1.3966,    1.4000,    1.4033,    1.4066,    1.4098,    1.4129,
 1.4160,   1.4190,    1.4219,    1.4248,    1.4275,    1.4302,    1.4329,    1.4354,    1.4379,    1.4404,    1.4427,    1.4450,
 1.4472,   1.4494,    1.4514,    1.4534,    1.4554,    1.4572,    1.4590,    1.4607,    1.4624,    1.4640,    1.4655,    1.4669,
 1.4683,   1.4696,    1.4708,    1.4720,    1.4731,    1.4741,    1.4750,    1.4759,    1.4767,    1.4774,    1.4781,    1.4787,
 1.4792,   1.4797,    1.4801,    1.4804,    1.4806,    1.4808,    1.4809,    1.4809,    1.4809,    1.4808,    1.4806,    1.4804,
 1.4800,   1.4796,    1.4792,    1.4787,    1.4780,    1.4774,    1.4766,    1.4758                                           
}; 


//�����¶ȱ��
//Ҫ�󣺹���������               105��/1h��110��/3min��150��/10s��200%/0s

//tab1//1s t1(j)=10^(0.36095/(i^2-0.97538) +0.7169);
const Uint16 TAB_ET2[256]={										//1s
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	
	0,	0,	0,	0,	0,	0,	1,	1,	2,	2,	
	3,	4,	6,	8,	10,	12,	15,	18,	22,	27,	
	31,	37,	43,	49,	56,	64,	73,	82,	91,	102,	
	113,	124,	136,	149,	163,	177,	192,	207,	223,	239,	
	257,	274,	292,	311,	330,	350,	370,	391,	412,	434,	
	456,	478,	501,	524,	548,	572,	596,	620,	645,	670,	
	696,	722,	748,	774,	800,	827,	854,	881,	908,	936,	
	963,	991,	1019,	1047,	1075,	1103,	1131,	1160,	1188,	1217,	
	1246,	1274,	1303,	1332,	1361,	1390,	1419,	1448,	1477,	1506,	
	1534,	1563,	1592,	1621,	1650,	1679,	1708,	1737,	1766,	1794,	
	1823,	1852,	1881,	1909,	1938,	1966,	1995,	2023,	2051,	2079,	
	2107,	2136,	2164,	2191,	2219,	2247,	2275,	2302,	2330,	2357,	
	2384,	2412,	2439,	2466,	2493,	2519,	2546,	2573,	2599,	2626,	
	2652,	2678,	2704,	2730,	2756,	2782,	2807,	2833,	2858,	2884,	
	2909,	2934,	2959,	2984,	3009,	3034,	3058,	3083,	3107,	3131,	
	3155,	3179,	3203,	3227,	3251,	3274,	3298,	3321,	3344,	3368,	
	3391,	3414,	3436,	3459,	3482,	3504,	3526,	3549,	3571,	3593,	
	3615,	3637,	3658,	3680,	3702,	3723,	3744,	3765,	3787,	3808,	
	3828,	3849,	3870,	3890,	3911,	3931,	3952,	3972,	3992,	4012,	
	4032,	4051,	4071,	4091,	4110,	4129,	4149,	4168,	4187,	4206,	
	4225,	4244,	4262,	4281,	4299,	4318,	4336,	4354,	4373,	4391,	
	4409,	4426,	4444,	4462,	4480,	4497,	4515,	4532,	4549,	4566,	
	4583,	4600,	4617,	4634,	4651,	4668,	4684,	4701,	4717,	4733,	
	4750,	4766,	4782,	4798,	4814,	4830,	4845,	4861,	4877,	4892,	
	4908,	4923,	4939,	4954,	4969,	4984 
};

//tab2//200us   t2(j)=10^(24.1717/(i^2) -9.7430);
const Uint16 TAB_ET1[256]={										//200us
	1,	1,	1,	1,	1,	1,	1,	2,	2,	2,	
	2,	2,	2,	2,	2,	3,	3,	3,	3,	3,	
	3,	4,	4,	4,	4,	5,	5,	5,	6,	6,	
	6,	7,	7,	7,	8,	8,	9,	9,	10,	10,	
	11,	12,	12,	13,	14,	14,	15,	16,	17,	18,	
	19,	20,	21,	22,	23,	24,	26,	27,	29,	30,	
	32,	33,	35,	37,	39,	41,	43,	45,	47,	50,	
	52,	55,	58,	61,	64,	67,	70,	74,	78,	81,	
	85,	90,	94,	98,	103,	108,	113,	119,	125,	130,	
	137,	143,	150,	157,	164,	172,	180,	188,	197,	206,	
	215,	225,	235,	246,	257,	268,	280,	293,	306,	320,	
	334,	348,	364,	379,	396,	413,	431,	450,	469,	489,	
	510,	531,	554,	577,	602,	627,	653,	680,	708,	738,	
	768,	799,	832,	866,	901,	938,	976,	1015,	1056,	1098,	
	1141,	1187,	1234,	1282,	1333,	1385,	1439,	1495,	1553,	1613,	
	1675,	1739,	1806,	1874,	1946,	2019,	2096,	2174,	2256,	2340,	
	2427,	2518,	2611,	2707,	2807,	2909,	3016,	3125,	3239,	3356,	
	3477,	3602,	3731,	3864,	4002,	4143,	4290,	4441,	4597,	4758,	
	4924,	5095,	5272,	5454,	5642,	5836,	6036,	6242,	6454,	6673,	
	6899,	7131,	7370,	7617,	7871,	8133,	8403,	8681,	8967,	9261,	
	9565,	9877,	10198,	10529,	10870,	11220,	11580,	11951,	12333,	12726,	
	13130,	13545,	13972,	14411,	14863,	15328,	15805,	16296,	16800,	17319,	
	17852,	18399,	18962,	19540,	20134,	20744,	21370,	22014,	22674,	23353,	
	24050,	24765,	25500,	26254,	27027,	27822,	28637,	29473,	30331,	31212,	
	32115,	33042,	33993,	34968,	35968,	36993,	38045,	39123,	40229,	41362,	
	42524,	43715,	44935,	46186,	47468,	48781 
};
 
//--------------------------------------//������Ϣ�ӱ�(�������ݵ����)

//--------------------------------------//��ʾ��Ϣ�ӱ�Systest 2014-05-06LJD���ջ��������޸Ĺ�������
const struct MESSAGE TAB_MSG[MSG_NUM]={
	{{'-',	'-',	'-'},	0, 			NONE},						//0=MSG_NONE

	{{'D',	'-',	'-'},	20, 		SHUT+D_RCVR+SAVE},			//1=MSG_CODEOK
	{{'E',	'E',	'E'},	20, 		SHUT+D_RCVR+SAVE},			//2=MSG_EE_FAIL
	{{'E',	'S',	'P'},	19, 		SHUT+I_RCVR+SAVE},			//3=MSG_ESTOP
	{{'C',	'B',	'F'},	18, 		OFFCB+D_RCVR+SAVE},			//4=MSG_CBTRIP
	{{'E',	'F',	'T'},	17, 		OFFCB+CNT+SAVE},			//5=MSG_EXFAULT 
	{{'C',	'A',	'N'},	16, 		SHUT+I_RCVR+SAVE},			//6=MSG_CAN_FAIL
	{{'E',	'C',	'F'},	15, 		SHUT+D_RCVR+CNT+SAVE}, 		//7=MSG_ENCODFAULT 
	{{'C',	'B',	'F'},	14, 		OFFCB+D_RCVR+CNT+SAVE},		//8=MSG_CBERROR 
	{{'P',	'R',	'F'},	14, 		OFFCB+D_RCVR+CNT+SAVE},		//9=MSG_PREERROR 
	{{'M',	'K',	'F'},	14, 		OFFCB+D_RCVR+CNT+SAVE},		//10=MSG_MAINKERROR 
	{{'M',	'F',	'F'},	14, 		OFFCB+D_RCVR+CNT+SAVE},		//11=MSG_MAINFERROR 2014-05-06û�в��ջ�����������CNT����
	{{'S',	'C',	'F'},	14, 		OFFCB+D_RCVR+CNT+SAVE},		//12=MSG_STACERROR
	{{'O',	'C',	'1'},	13, 		OFFCB+D_RCVR+CNT+SAVE},     //13=MSG_HIA1
	{{'O',	'C',	'2'},	13, 		OFFCB+D_RCVR+CNT+SAVE},		//14=MSG_HIB1
	{{'O',	'C',	'3'},	13, 		OFFCB+D_RCVR+CNT+SAVE},		//15=MSG_HIC1
	{{'O',	'C',	'4'},	13, 		OFFCB+D_RCVR+CNT+SAVE},		//16=MSG_HIA2
	{{'O',	'C',	'5'},	13, 		OFFCB+D_RCVR+CNT+SAVE},		//17=MSG_HIB2	
	{{'O',	'C',	'6'},	13, 		OFFCB+D_RCVR+CNT+SAVE},		//18=MSG_HIC2
    {{'U',	'F',	'1'},	12, 		OFFCB+D_RCVR+CNT+SAVE},		//19=MSG_UDCOV	
	{{'O',	'C',	'7'},	12, 		OFFCB+D_RCVR+CNT+SAVE},		//20=MSG_HIAC1 
 	{{'O',	'C',	'8'},	12, 		OFFCB+D_RCVR+CNT+SAVE},		//21=MSG_HIAC2 
	{{'P',	'D',	'A'},	11, 		OFFCB+D_RCVR+CNT+SAVE},		//22=MSG_PDPASERIES 	
 	{{'P',	'D',	'B'},	11, 		OFFCB+D_RCVR+CNT+SAVE},		//23=MSG_PDPBSERIES 
	{{'S',	'P',	'O'},	10, 		SHUT+D_RCVR+SAVE},			//24=MSG_MSPEEDOUT
	{{'O',	'C',	'9'},	9, 			SHUT+D_RCVR+CNT+SAVE},		//25=MSG_SIAC1 
	{{'O',	'C',	'A'},	9, 			SHUT+D_RCVR+CNT+SAVE},		//26=MSG_SIAC2 	
	{{'F',	'E',	'1'},	8, 			SHUT+I_RCVR+SAVE},			//27=MSG_FE1 201005atcpc	
	{{'U',	'F',	'2'},	7, 			SHUT+D_RCVR+CNT+SAVE},		//28=MSG_SUDCOV
	{{'U',	'F',	'3'},	7, 			SHUT+I_RCVR+CNT+SAVE},  	//29=MSG_SUACOV1
	{{'U',	'F',	'5'},	6, 			SHUT+I_RCVR+CNT+SAVE},		//30=MSG_SUDCLV
	{{'U',	'F',	'6'},	6, 			SHUT+I_RCVR+CNT+SAVE},		//31=MSG_SUACLV1 201005atcpc
	{{'D',	'C',	'E'},	5, 			SHUT+D_RCVR+CNT+SAVE},		//32=MSG_UDCSAVE �м��ѹ��������
	{{'O',	'C',	'S'},	5, 			SHUT+D_RCVR+CNT+SAVE},		//33=MSG_SIOVST ������ӵ�������
	{{'G',	'F',	'1'},	4, 			SHUT+I_RCVR+CNT+SAVE},		//34=MSG_GFAULT1 ����ӵع���
	{{'G',	'F',	'2'},	4, 			SHUT+I_RCVR+CNT+SAVE},		//35=MSG_GFAULT2 ����ӵع���
	{{'O',	'H',	'1'},	3, 			SHUT+I_RCVR+CNT+SAVE},		//36=MSG_TOV1����
	{{'O',	'H',	'2'},	3, 			SHUT+I_RCVR+CNT+SAVE},		//37=MSG_TOV2����
	{{'O',	'S',	'K'},	3, 			SHUT+I_RCVR+CNT+SAVE},		//38=MSG_SKTOV����
	{{'O',	'L',	'1'},	2, 			SHUT+I_RCVR+CNT+SAVE},		//39=MSG_TLOV1�����г���
	{{'O',	'L',	'2'},	2, 			SHUT+I_RCVR+CNT+SAVE},		//40=MSG_TLOV2�����г���
	{{'P',	'O',	'V'},	1, 			SHUT+CNT+SAVE},				//41=MSG_PGOV�������й����ʹ���
	{{'Q',	'O',	'V'},	1, 			SHUT+CNT+SAVE},				//42=MSG_QGOV�������޹����ʹ���
	{{'S',	'Y',	'F'},	1, 			SHUT+SAVE},					//43=MSG_STASYNFAIL����ͬ������ʧ��
	{{'P',	'O',	'E'},	1, 			SHUT+I_RCVR+CNT+SAVE},		//44=MSG_PHORDE 
};


#ifdef __cplusplus
}
#endif /* extern "C" */


#endif  // end

//===========================================================================
// No more.
//===========================================================================
