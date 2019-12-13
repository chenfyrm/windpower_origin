/****************************************Copyright (c)**************************************************
**                       		     ��	��	��	ͨ	��	ѧ
**                                         ��������ѧԺ
**                                         604ʵ����
**
**                              
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: user_main.c
**��   ��   ��: 
**����޸�����: 
**��        ��: ����1.5MW˫�������鲢���������������������----201005atcpc-201007atbjtu
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: 
** ��  ��: 
** �ա���: 
** �衡��: 
**
**--------------��ǰ�汾�޶�------------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
/*---------------------�Դ�ͷ�ļ�-----------------------------*/
#include "C28x_FPU_FastRTS.h"
#include "DSP2833x_Device.h"     				// DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   				// DSP2833x Examples Include File
/*--------------------�Զ���ͷ�ļ�----------------------------*/
#include "user_header.h"  						//������������
#include "user_macro.h"							//�꺯��
#include "user_database.h"						//���ݿ�   
#include "user_interface.c"						//�ӿڲ�
#include "user_work.c"							//��������
#include "math.h"

/*-----------------------�ж�����-----------------------------*/
#pragma CODE_SECTION(CpuTimer0Isr, "ramfuncs");
interrupt void CpuTimer0Isr(void);				//����ʱ���ж�			
interrupt void EPWM1_TZ1_Isr(void);				//TZ1�ж�
interrupt void EPWM2_TZ2_Isr(void);				//TZ2�ж�
/*-----------------------��������-----------------------------*/

void Protect(void);

#pragma CODE_SECTION(Scout, "ramfuncs");
void Scout(void);
void et_relay_N(void);
void et_relay_M(void);
void CntCtrl(void);
void SysCtrl(void);
void Bank(void);
void Display(void);
void BANK_Datasave(void);					
void ERROR_Datasave(void);
#pragma CODE_SECTION(ACrowbar, "ramfuncs");					
void ACrowbar(void);					

// These are defined by the linker (see F28335_flash.cmd)
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
/*********************************************************************************************************
** ��������: main
** ��������: ϵͳ��ʼ��,��ѭ��
** �䡡��: 
** �䡡��:        
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void main(void)
{
//--------------------------------ϵͳ��ʼ��--------------------------------
	InitSysCtrl();
//---------------------------------IO��ʼ��---------------------------------
	InitGpio();	

	DINT;
   	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();
//--------------------------------�����ʼ��--------------------------------
	InitPeripherals(); 	
//---------------------------------дFLASH����------------------------------
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	InitFlash();
//--------------------------------������ʼ��--------------------------------
	InitWork();	
//---------------------------�û��Զ������ĳ�ʼ��-------------------------
	InitEeprom();

//-----MagnetCurve2013-12-13-ZZJ------------------
//-----��ȡEEROM�ڲ��洢�����Ų���----------------------

   	MC_DATA.y[0] = _SC_EXISTY1 * 0.001;
   	MC_DATA.y[1] = _SC_EXISTY1 * 0.001;
	MC_DATA.y[2] = _SC_EXISTY2 * 0.001;
	MC_DATA.y[3] = _SC_EXISTY3 * 0.001;
	MC_DATA.y[4] = _SC_EXISTY4 * 0.001;
	MC_DATA.y[5] = _SC_EXISTY5 * 0.001;
	MC_DATA.y[6] = 1.4088;
	MC_DATA.y[7] = 1.5904;
	MC_DATA.y[8] = 1.8233;
	MC_DATA.y[9] = 2.1123;
	MC_DATA.y[10] = 2.4714;

	MC_DATA.x[0] = 0;
	MC_DATA.x[1] = 50;
	MC_DATA.x[2] = 75;
	MC_DATA.x[3] = 90;
	MC_DATA.x[4] = 100;
	MC_DATA.x[5] = 110;
	MC_DATA.x[6] = 115;
	MC_DATA.x[7] = 120;
	MC_DATA.x[8] = 125;
	MC_DATA.x[9] = 130;
	MC_DATA.x[10] = 135;

	_MC_OK = 1;
//--------2013-12-13--------

//	InitRtimer();										//ʵʱʱ�ӳ�ʼ��ֻ��оƬ��Ҫ��ʼ��ʱ�Ž���
//-------------------------------�жϵ�ַ��ʼ��-----------------------------
	EALLOW;  
	PieVectTable.TINT0       = &CpuTimer0Isr;		  	//��ʱ��T0�����ж�
	PieVectTable.EPWM1_TZINT = &EPWM1_TZ1_Isr;      	//TZ1 ���ʱ����ж� 
	PieVectTable.EPWM2_TZINT = &EPWM2_TZ2_Isr;      	//TZ2 ���ʱ����ж� 

	EDIS;    
	
//------------------------------�����ж�ʹ�ܼĴ���--------------------------
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;		//CPUTIMER0�����ж�
    										//INT1(CPU)/INT1.7(PIE)
	PieCtrlRegs.PIEIER2.bit.INTx1 = 1;		//epwm1.tz
											//INT2(CPU)/INT2.1(PIE)
	PieCtrlRegs.PIEIER2.bit.INTx2 = 1;		//epwm2.tz 																	//INT1(CPU)/INT1.1(PIE)
												
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;  	//ʹ��PIE   
    
   	IER |= (M_INT1|M_INT2);					//�������ж����μĴ���
    
    EINT;   								//�����ж�
    
//-------------------------------��ʱ����ʼ����-------------------------------
	CpuTimer0Regs.TCR.all = 0x4001; // Use write-only instruction to set TSS bit = 0//	M_StartCpuTimer0();												//����ʱ������

//----------------------------------��ѭ��---------------------------------------------------------
	for(;;) 
	{				

//----------------------------------ÿ������----------------------------------
		EeCtrl();							//eeprom����
		Sci485Ctrl(); 
//		Sci_canopenrx();					//ϵͳ�����ȡ	

//-----------------------------һ��ѭ��5ms����һ��----------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt1,DELAY_MAIN1)>=0)		
		{
			MAIN_LOOP.cnt1=0;				//��ռ�����
			Input();						//ϵͳ�����ȡ
	        M_NotFlag(SL_WATCHDOG);		    //DSP������Ź� 20100401at27
			Output();						//ϵͳ���ָʾ
		}

//-----------------------------����ѭ��10ms����һ��---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt2,DELAY_MAIN2)>=0)
		{
			MAIN_LOOP.cnt2=0;
			if(M_ChkFlag(SL_CODEOK)!=0)		//��eeprom�����������
			{
			     Give();					//����ָ�����
			}
		}

//-----------------------------����ѭ��20ms����һ��---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt3,DELAY_MAIN3)>=0)
		{
			MAIN_LOOP.cnt3=0;
			SysCtrl();						//���س���
		}

//-----------------------------�ļ�ѭ��50ms����һ��---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt4,DELAY_MAIN4)>=0)
		{
			MAIN_LOOP.cnt4=0;
		    Bank();							//��ʾ����
		}

//----------------------------�弶ѭ��100ms����һ��---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt5,DELAY_MAIN5)>=0)
		{					
			MAIN_LOOP.cnt5=0;
			if((M_ChkFlag(SL_POWERON)!=0)&&(M_ChkCounter(MAIN_LOOP.cnt_poweron,DELAY_POWERON)>0))
			{
				M_ClrFlag(SL_POWERON);
				ClrPdpint();						//PDPINT�ж����	
				EnPdpint();							//PDPINTʹ���ж�
			}			
//20130801
			if(M_ChkFlag(SL_ENCODEPOS_OK)==0 && M_ChkFlag(SL_ECPOSSEARCH)!=0 && M_ChkFlag(SL_MPR_START)!=0)	//20130604
			{
				*FUNC[20].para_add = - QEPDATA.encodpos * 1000;	//�޸�RAM��EEROM

				EEPROM.mcode=20;			//д��2�ֽ�����
				M_SetFlag(SL_EEASK_MCODE);			//��EEPROM�޸Ĺ����������־
			}
		}
//-----------------------------����ѭ��1000ms����һ��---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt6,DELAY_MAIN6)>=0)
		{
			MAIN_LOOP.cnt6=0;

			if(M_ChkFlag(SL_CODEOK)!=0)		
				RtRead();							//��eeprom�����������//��ȡʵʱʱ��,�ܺ�ʱ��Ҫ13ms.20090801,CPC				
//20121103										
//			if(M_ChkCounter(MAIN_LOOP.cnt_senszfstdy,DELAY_SENSZFSTDY)>=0)
//				M_SetFlag(SL_SENSZFSTDY);   		//�ӳ�ʱ�䵽������Ư�˲��ȶ���־λ
			if(MAIN_LOOP.cnt_gridok_last!=65535)		MAIN_LOOP.cnt_gridok_last++;	
			if(MAIN_LOOP.cnt_hvlv_detect3!=65535)		MAIN_LOOP.cnt_hvlv_detect3++;	
			if(MAIN_LOOP.cnt_uacover!=65535)			MAIN_LOOP.cnt_uacover++;	

		}
	}  					
} 


/*********************************************************************************************************
** ��������: CpuTimer0Isr
** ��������: ����ʱ������ж?(0.04ms)
** �䡡��:
** �䡡��:        
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
interrupt void CpuTimer0Isr(void)
{
    Uint16 i,k;
    
	M_SetFlag(SL_PHASEA);            							//����CPUռ����,����DSP����T1����
	*OUT3_ADDR = _OUT3_DATA;
		
	if(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(SL_POWERON)==0)   //�ں�����ǰ���ϵ��ɺ���ѹ�������Ӻ��? cap5-ubc   cap6-uab
	{
	  PhaseOrderChk();        
	} 
//	QepEncodPos(); 					
    CapQepCtrl();  		  										//����ѹ��λ����ѹƵ�ʣ������������任�ĽǶ�
   	Ad8364Ctrl();             									//�д������ͻ�������AD������ת��
	ACrowbar();													//LVRT20100112
    PLLCtrl();		//201205LVRTatZB�����˽ǶȻ��ֵ�λ��20120527												//201011PLL
	QepEncodPos();		//����zb���������λ��20121103

    NPR_CONTROL();            									//��������������㷨
    MPR_CONTROL();            									//��������������㷨

	et_relay_N();												//���������ʱ�޼���
	et_relay_M();												//���������ʱ�޼���

	Da5344Manage();          	//DA output

//	testtheta = testtheta + 1;									//���ǲ������ź�
//	if(testtheta>=3721)	testtheta= -1879;

//---------------------------------��ʱ�������--------------------------------
	MAIN_LOOP.pulse++;
	if(MAIN_LOOP.pulse>=10)		MAIN_LOOP.pulse=0;
	
//--20����ʱ����
	switch(MAIN_LOOP.pulse)
	{

//--��ʱ���ۼ�,����AD,����ֵ����,����
		case 0:
		{
			Protect();			   //protect calculation		
			Scout();			   //���ϱ���
			BANK_Datasave();		//0.4msһ��
			break;
		}
		case 1:
		{	
			PwmDrive();	 		   //����������  				
		    CntCtrl(); 				//����1ms��һ��������ʱ���ж�����Ϊ200us
			ERROR_Datasave();		//0.4msһ��
			Sci_canopenrx();		//ϵͳ�����ȡ120507
			break;
		}
		case 2:
		{
			Protect();			   //protect calculation		
			Scout();			   //���ϱ���
			BANK_Datasave();		//0.4msһ��
			break;
		}
		case 3:
		{	
			PwmDrive();			   //����������				
			RunCtrl();			   //��������
			ERROR_Datasave();		//0.4msһ��
			break;
		}
		case 4:
		{	
			Protect();			   //protect calculation	
			Scout();			   //���ϱ���
			BANK_Datasave();		//0.4msһ��
			break;
		}
		case 5:
		{	
			PwmDrive();			   //����������				
			Display();
			ERROR_Datasave();		//0.4msһ��
			break;
		}
		case 6:
		{	
			CntCtrl(); 	           //������1ms��һ��������ʱ���ж�����Ϊ200us
			Protect();			   //protect calculation	
			Scout();			   //���ϱ���
			BANK_Datasave();		//0.4msһ��
			Sci_canopenrx();		//ϵͳ�����ȡ120507
			break;
		}
		case 7:
		{
			PwmDrive();			   //����������				
			ERROR_Datasave();	   //0.4msһ��
			break;
		}
		case 8:
		{	
			Protect();												//slow AD, protect calculation	
			Scout();												//���ϱ���
			BANK_Datasave();		//0.4msһ��
			break;
		}
		case 9:
		{	
			PwmDrive();			   //����������				
			RunCtrl();			   //��������
			ERROR_Datasave();		//0.4msһ��
			break;
		}
//--------------------------------------------------------------------------------------------------
		default:
			break;
	}

//--Ӧ���ж�20091109atzy
//	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	

//canopen
//-----------------------------canopen����һ��--120507----------------------------
	if(M_ChkCounter(MAIN_LOOP.canopen_tx,DELAY_CANOPENTX)>=0)    //250  6ms		
	{
		MAIN_LOOP.canopen_tx=0;				//��ռ�����
		if(M_ChkFlag(SL_CANOPENTX_FLAG)!=0)
		{
			if(TX_SPEED==0)	
			{	
		SCI_canopen.tx_speed = (int16)(PRO.speedflt * 16383.0 / CAN_SPEED);
//		SCI_canopen.tx_speed = (int16)(100 * 16383.0 / CAN_SPEED);
				TX_SPEED=1;
			}
			else
			{
		SCI_canopen.tx_speed = (int16)(PRO.speedflt * 16383.0 / CAN_SPEED) + 1;
//		SCI_canopen.tx_speed = (int16)(100 * 16383.0 / CAN_SPEED) + 1;
				TX_SPEED=0;
			}

			Sci_canopentx();				//12msִ��һ��
			M_SetFlag(SL_CANOPENTX);
			M_ClrFlag(SL_CANOPENTX_FLAG);		
		}
		else
		{
			Sci_canopentx_read();			//12msִ��һ��
			M_SetFlag(SL_CANOPENTX_FLAG);	
		}	
	} 

	if(M_ChkFlag(SL_CANOPENTX)!=0)
	{		
	 	if(SciaRegs.SCIFFTX.bit.TXFFST <= 8)	//12-->8 2011XGate					
		{
			for(i=SCICANOPENTXNUM-6;i<SCICANOPENTXNUM;i++)
				SciaRegs.SCITXBUF=SCI_canopen.txb[i];
			M_ClrFlag(SL_CANOPENTX);
		}
	}
	else
	{
		if(SciaRegs.SCIFFTX.bit.TXFFST == 0)						//�������?
		{
			if(SciaRegs.SCICTL2.bit.TXEMPTY==1)	
			{
				SciaRegs.SCIFFTX.bit.TXFIFOXRESET=1;	// Re-enable TX FIFO operation
			}
		}
	}


//RS485TX
//----------------------------------------------------------------------
/*	if(M_ChkFlag(SL_TXLONGDATA)!=0)
	{                              
	 	if(ScibRegs.SCIFFTX.bit.TXFFST <= 8)	
	 	{
	 		k=0;
			for(k=0;k<8;k++)
			{
				ScibRegs.SCITXBUF=SCI.txb[longtx_num];
				longtx_num++;
				
				if(longtx_num==length485)	
				{
					M_ClrFlag(SL_TXLONGDATA);
					longtx_num=15;
					k=8;
				}			
			}	 	
	 	}							
	}
	else	longtx_num=15;
*/
//----------------------------------------------//121014byljd ����ʩ�����飬��T0�жϼ��뼰ʱ�ͷ����߳���
	if(M_ChkFlag(SL_TX)!=0)						//Sci.txb�Ĵ������ݷ�����ɺ�������ѭ������������æ��
 	{											//�޷�����Sci485Ctrl�ӳ����ͷ�ͨѶ���ߣ�����ͨѶ�жϣ�
		if(M_ChkFlag(SL_TXLONGDATA)!=0)			//�������ͷ��ӳ�������T0�ж��м�ʱ�ͷ�	120625
		{                              
		 	if(ScibRegs.SCIFFTX.bit.TXFFST <= 8)	
		 	{
		 		k=0;
				for(k=0;k<8;k++)
				{
					ScibRegs.SCITXBUF=SCI.txb[longtx_num];
					longtx_num++;
					
					if(longtx_num==length485)	
					{
						M_ClrFlag(SL_TXLONGDATA);
						longtx_num=15;
						k=8;
					}
				}
		 	}
		}
		else									//�����������?
		{
			longtx_num = 15;

			if(ScibRegs.SCIFFTX.bit.TXFFST == 0)						//�������?Transmit FIFO is empty.
			{
				SCI.cnt_sciover=0;										//�������/���ճ�ʱ��ʱ��
				if(ScibRegs.SCICTL2.bit.TXEMPTY==1)						//���ͼĴ���Ϊ��?Transmitter buffer and shift registers are both empty
		 		{
					M_ClrFlag(SL_TX);
					Sci485_RxInit();									//485���ճ�ʼ��
				}
			}

		}
 	}


//------------------------------------------------------------------------

    M_ClrFlag(SL_PHASEA);								//����CPUռ����,����DSP����T1����
    *OUT3_ADDR = _OUT3_DATA;

//--Ӧ���ж�20091109atzy
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	
}

/*********************************************************************************************************
** ��������: EPWM1_TZ1_Isr
** ��������: ���ʱ����ж�
** �䡡��:
** �䡡��:   
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
interrupt void EPWM1_TZ1_Isr (void)
{
//----------------------------------------------//�������

	Disepwmio_NPR();
	Disepwmio_MPR();
    DisPdpint();                
	M_SetFlag(SL_PDPINTA);
//----------------------------------------------//Ӧ���ж�
	ClrPdpint();
	M_SetFlag(SL_DISPLAY3);    //LED3�����
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}
/*********************************************************************************************************
** ��������: EPWM2_TZ2_Isr
** ��������: ���ʱ����ж�
** �䡡��:
** �䡡��:   
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
interrupt void EPWM2_TZ2_Isr (void)
{
//----------------------------------------------//�������
	Disepwmio_NPR();
	Disepwmio_MPR();
    DisPdpint();     
	M_SetFlag(SL_PDPINTB);
//----------------------------------------------//Ӧ���ж�
	ClrPdpint();
	M_SetFlag(SL_DISPLAY3);    //LED3�����
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}


/*********************************************************************************************************
** ��������: ACrowbar
** ������: ActiveCrowbar����
** �䡡��: 	 
** ��  ��:   
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void ACrowbar(void)
{
    float NGS_U_D,NGS_U_Q;
//-------dc-chopper----------------����������7 
//----2016-1-13----����ն����������------
	if((_COMMAND2&0x0080)==0 && M_ChkFlag(SL_ERRSTOP)==0 && M_ChkFlag(SL_SERIESTOP)==0)
	{
		if(M_ChkFlag(SL_LV_STATE)!=0)
		   M_SetFlag(CL_CHOPEN);//DC-CHOPPER����ʹ���źţ�����ʹ�ܣ���ͨ�ضϻ���װ��ͻ�VDC2����й�
		else if(M_ChkFlag(SL_HV_STATE)!=0)
		{
		   if(PRO.udc >1300)
		   M_SetFlag(CL_CHOPEN);
		   else if(PRO.udc <1260)
		   {
			 M_ClrFlag(CL_CHOPEN);
		   }		   
		}
		else
		{
			M_ClrFlag(CL_CHOPEN);
		}		
	}	
	else
		M_ClrFlag(CL_CHOPEN);

	if((_COMMAND2&0x0800)!=0)	//115.11����11-ն������ 2013-12-6ZZJ
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_chopper,DELAY_CHOPPER)<=0) M_SetFlag(CL_CHOPTEST);
		else 													 M_ClrFlag(CL_CHOPTEST);
	}
	else
	{
		M_ClrFlag(CL_CHOPTEST);		//DC-CHOPPER����ʹ���źţ����ͽ�ֹ
		MAIN_LOOP.cnt_chopper=0;
	}

	*OUT2_ADDR = _OUT2_DATA; //Ѹ�����

//-------------------LV ����������-----------------
	
//---2015-11-2---�ʹ�������ӹ��ϡ����е�ǰ������----
	if(M_ChkFlag(SL_IN1_MIANK)!=0 && M_ChkFlag(SL_SERIESTOPING)==0 && M_ChkFlag(SL_SERIESTOP)==0 && M_ChkFlag(SL_ERRSTOPING)==0 && M_ChkFlag(SL_ERRSTOP)==0 && M_ChkFlag(SL_MPR_SYNOK)!=0)	
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_hvlv_detect,DELAY_HVLV_DETEC)>=0)			//1s
			M_SetFlag(SL_HVLV_DETEC);
	}
	else	
	{
		MAIN_LOOP.cnt_hvlv_detect=0;
		M_ClrFlag(SL_HVLV_DETEC);
	}

//	NGS_Udq    = sqrt(TRS_NGS_U.dpll * TRS_NGS_U.dpll + TRS_NGS_U.qpll * TRS_NGS_U.qpll); //������ѹ��ֵ-������������
//	NGS_Udq_lv = sqrt(TRS_NGS_U.dnpr * TRS_NGS_U.dnpr + TRS_NGS_U.qnpr * TRS_NGS_U.qnpr);  //������ѹ��ֵ-������������-����

	NGS_Udq = sqrt(TRS_NGS_U.dflt * TRS_NGS_U.dflt + TRS_NGS_U.qflt * TRS_NGS_U.qflt);  //20110326������ѹ��ֵ-������������-����

//  ��PLL�����NGS_Udq_p���BJTULVRT201204

	NGS_U_D    = 0.3333333 * (2 * AD_OUT_NGS_U.a - AD_OUT_NGS_U.b - AD_OUT_NGS_U.c);	//�þ�ֹ�������������ѹ��ֵ
	NGS_U_Q    = SQRT3_3 * (AD_OUT_NGS_U.b - AD_OUT_NGS_U.c);
	TRS_NPR_U.amp = sqrt(NGS_U_D * NGS_U_D + NGS_U_Q * NGS_U_Q);   //20110328
	DataFilter(0.443,&TRS_NPR_U.ampflt,TRS_NPR_U.amp); 			//c=0.4,Ts=200us,fh=1.2kHz,�˵�����Ƶ�ʴ�

	if(M_ChkFlag(SL_LV_QWORKING)==0||M_ChkFlag(SL_HV_QWORKING)==0)	DataFilter(0.998,&NGS_Udq_pflt,NGS_Udq_p);		//100ms	20130303

//-----�����޹�����ϵ��kq-----BJTULVRT201204
	if(M_ChkFlag(SL_UNBALANCE)==0)
//		GRD_Ut = NGS_Udq / GRD_UN;
		GRD_Ut = NGS_Udq / NGS_Udq_p_ex;	//20130301
	else
//		GRD_Ut = NGS_Udq_p / GRD_UN;
		GRD_Ut = NGS_Udq_p / NGS_Udq_p_ex;	//20130301
//	if(GRD_Ut <= 0.9)	kq = 1.5 * (1 - NGS_Udq_p / GRD_UN);	//201205LVRTatZB
//	else				kq = 0;		//20120531 201205LVRTatZB
//--------20130225--------
	if(GRD_Ut <= 0.94 )							//20130124	20130303
	{
		kq = 1.5 * (1.05 - NGS_Udq_p / NGS_Udq_p_ex);	//20130301
		MAIN_LOOP.cnt_hvlv_kq = 0;				//20130124
	}
	else if(GRD_Ut >= 1.05 )
	{
		kq = 1.5 * (NGS_Udq_p / NGS_Udq_p_ex-1.05);	
		MAIN_LOOP.cnt_hvlv_kq = 0;				
	}
	else if(M_ChkCounter(MAIN_LOOP.cnt_hvlv_kq,30)>0)
		kq = 0;		//��ѹ�ָ��������޹�������ʱ30ms���

	if(kq<0) kq=0;
//------------------------
	if(M_ChkFlag(SL_HVLV_DETEC)!=0)	//20120601night
	{
		if(M_ChkFlag(SL_LV_STATE)!=0||(M_ChkFlag(SL_HV_STATE)!=0))	MAIN_LOOP.cnt_gridok_last=0;//�ָ����ʱ
		else 		MAIN_LOOP.cnt_gridfault_last = 0; //���Ϻ��ʱ

		if(M_ChkCounter(MAIN_LOOP.cnt_gridok_last,DELAY_EQUIP_CD)>0)	//����ʹ�״̬���жϣ��ʹ�״̬�ָ��������5min���ܵڶ��ν���ʹ�״̬
		{
//			if(M_ChkFlag(SL_LV_STATE)==0 && (TRS_NPR_U.ampflt<(0.91 * GRD_UN)))
//			if(M_ChkFlag(SL_LV_STATE)==0 && (TRS_NPR_U.ampflt<(0.91 * NGS_Udq_p_ex)))		//20130301
//			if(M_ChkFlag(SL_LV_STATE)==0 && (TRS_NPR_U.ampflt<(_stdby01 * NGS_Udq_p_ex)))		//20130309
			//���¸��ݷ��ȼ�⣬�������������������10ms���ɱ�֤10ms�ڼ�⵽����
			if(M_ChkFlag(SL_LV_STATE)==0 && (TRS_NPR_U.ampflt<(0.6 * NGS_Udq_p_ex)))//2013-12-6С��0.6������λ
			{
				gridfault_flag_flt1++;
				if(gridfault_flag_flt1>=5)
				{
					M_SetFlag(SL_LV_STATE);
					if((_STDBY9&0x0002)==0)		M_SetFlag(CL_ZKHVLVRT);	
					gridfault_flag_flt1=0;
				}
			}
			else
			{
				gridfault_flag_flt1=0;
			}
			
			if(M_ChkFlag(SL_HV_STATE)==0 && (TRS_NPR_U.ampflt>(1.2 * NGS_Udq_p_ex)))
			{
				gridfault_flag_flt2++;
				if(gridfault_flag_flt2>=5)
				{
					M_SetFlag(SL_HV_STATE);
					gridfault_flag_flt2=0;
				}
			}
			else
			{
				gridfault_flag_flt2=0;
			}
			
			if(M_ChkFlag(SL_LV_STATE)==0 && (TRS_NPR_U.ampflt<(0.9 * NGS_Udq_p_ex)))//2013-12-6С��0.91��ʱ1ms��λ
			{
				gridfault_flag_flt3++;
				if(gridfault_flag_flt3>=10)
				{
					M_SetFlag(SL_LV_STATE);
					if((_STDBY9&0x0002)==0)		M_SetFlag(CL_ZKHVLVRT);	
					gridfault_flag_flt3=0;
				}
			}
			else
			{
				gridfault_flag_flt3=0;
			}
			
			if(M_ChkFlag(SL_HV_STATE)==0 && (TRS_NPR_U.ampflt>(1.10* NGS_Udq_p_ex)))//2013-12-6С��0.91��ʱ1ms��λ
			{
				gridfault_flag_flt4++;
				if(gridfault_flag_flt4>=10)
				{
					M_SetFlag(SL_HV_STATE);
					gridfault_flag_flt4=0;
				}
			}
			else gridfault_flag_flt4=0;
		}
		else 
		{
			gridfault_flag_flt1=0;
			gridfault_flag_flt2=0;
			gridfault_flag_flt3=0;
			gridfault_flag_flt4=0;	
		}
//		if(M_ChkFlag(SL_LV_STATE)!=0 && (NGS_Udq_p > (0.92 * GRD_UN)))
		if((M_ChkFlag(SL_LV_STATE)!=0)&& (NGS_Udq_p > (0.92 * NGS_Udq_p_ex)))		//�ָ�����̬���ж�
		{		
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_rcv,DELAY_HVLVRCV)>0)			//10ms ����������
			{
				M_ClrFlag(SL_LV_STATE);	

				if((_STDBY9&0x0002)==0)		M_ClrFlag(CL_ZKHVLVRT);		//20121107

				if(M_ChkCounter(MAIN_LOOP.cnt_gridfault_last,50)<=0)		//�󴥷�������ʱ������������λ20130303
				{
					if(NGS_Udq_n2p < 2.5)	MAIN_LOOP.cnt_gridok_last = 20000;						//Ӧ����DELAY_EQUIP_CD
					M_SetFlag(SL_LV_CLRERRAM);		//���RAM�ڲ���		20130306		
					M_ClrFlag(SL_LV_QWORKING);
					M_ClrFlag(SL_LV_STRICTLV);
				}
			}
		}
		else	MAIN_LOOP.cnt_lv_rcv = 0;

		if((M_ChkFlag(SL_HV_STATE)!=0)&& (NGS_Udq_p < (1.08 * NGS_Udq_p_ex)))		//�ָ�����̬���ж�
		{		
			if(M_ChkCounter(MAIN_LOOP.cnt_hv_rcv,DELAY_HVLVRCV)>0)			//10ms ����������
			{
				M_ClrFlag(SL_HV_STATE);	

				if(M_ChkCounter(MAIN_LOOP.cnt_gridfault_last,50)<=0)		//�󴥷�������ʱ������������λ20130303
				{
					if(NGS_Udq_n2p < 2.5)	MAIN_LOOP.cnt_gridok_last = 20000;						//Ӧ����DELAY_EQUIP_CD
					M_SetFlag(SL_LV_CLRERRAM);		//���RAM�ڲ���		20130306
					M_ClrFlag(SL_HV_QWORKING);			
				}
			}
		}
		else	MAIN_LOOP.cnt_hv_rcv = 0;		
	}
	else	
	{
		M_ClrFlag(SL_LV_STATE);	
		M_ClrFlag(SL_HV_STATE);	
		if((_STDBY9&0x0002)==0)		M_ClrFlag(CL_ZKHVLVRT);		//20121107
		MAIN_LOOP.cnt_lv_rcv = 0;
		MAIN_LOOP.cnt_hv_rcv = 0;
		MAIN_LOOP.cnt_gridok_last=0;
		gridfault_flag_flt1=0;
		gridfault_flag_flt2=0;
		gridfault_flag_flt3=0;
		gridfault_flag_flt4=0;
		MAIN_LOOP.cnt_gridfault_last = 0;
	} 
//--------20130225--------
    if(M_ChkFlag(SL_LV_STATE)!=0)		//20130128
	{
	  	if(M_ChkCounter(MAIN_LOOP.cnt_lv_dlrecmstop,3000)>0)	//3s ÿ��������ͣ����ļ��ʱ��Ϊ3s	20130221
	  	{
			if(M_ChkCounter(MAIN_LOOP.cnt_gridfault_last,500)>=0)	//������500ms�󣬱ܿ��˵���˲�䣬ֻ����ظ�ʱ
			{
//				if(((NGS_Udq_p - NGS_Udq_p_lv) > 55) && (NGS_Udq_p_lv < (0.85 * NGS_Udq_p_ex)))	//3ph-75,2ph-50 20130205
				if(((NGS_Udq_p - NGS_Udq_p_lv) > 55) && ((NGS_Udq_p_lv < (0.82 * NGS_Udq_p_ex))||(NGS_Udq_n2p_lv > 19)))	//3ph/2ph-50������ 20130301 xly��ǰ19Ϊ33%
				{
					M_SetFlag(SL_LV_RECMSTOP);					
//					M_SetFlag(SL_LV_RECNSTOP);		//2013-12-6ZZJȡ���ָ�˲����������������
				}
			}
		}
	}
	else
	{
		M_ClrFlag(SL_LV_RECMSTOP);
		M_ClrFlag(SL_LV_RECNSTOP);		//20130206
	}
//--------20130221--------
    if(M_ChkFlag(SL_LV_STATE)!=0)
	{
		if(NGS_Udq_p_lv < (0.88 * NGS_Udq_p_ex))
		{
			M_SetFlag(SL_LV_SCRKEEPON);
		}
		else	MAIN_LOOP.cnt_lv_scrkeepon = 0;

		if(M_ChkFlag(SL_LV_SCRKEEPON)!=0)
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scrkeepon,_stdby06)>=0)	
			M_ClrFlag(SL_LV_SCRKEEPON);//������г�������ϣ��ڲ��Գ���ȵ�����������SCR����Ͷ��ʱ��Ϊ604ms�����������
		}
	}
	else
	{
		M_ClrFlag(SL_LV_SCRKEEPON);
		MAIN_LOOP.cnt_lv_scrkeepon = 0;
	}
//------------------------


/*20120601night
 	if((_COMMAND2&0x0020)!=0)	//����������5  201011LVRT
	{
		LV_A1 = 0.9; 			//E.on2003��׼
		LV_A2 = 0.15;
		LV_T1 = 625;
		LV_T2 = 3000;
	}
	else						//Ĭ��Ϊ���Ժ�Ĺ��ұ�׼
	{
		LV_A1 = 0.9; //_stdby02  testing
		LV_A2 = 0.1; //�ŵ�Щ�������ױ��� 
		LV_T1 = 625;
		LV_T2 = 2000;	
//		LV_A2 = 0;			////�ſ�����,LVʵ�鲻ͣ�� 20110326
//		LV_T1 = 1500;
//		LV_T2 = 4000;	
	}

	if(M_ChkFlag(SL_HVLV_DETEC)!=0)
	{
//		if(M_ChkFlag(SL_HVLV_DETEC)!=0 && NGS_Udq_lv < (LV_A1 * GRD_UN))			//0.9 GRD_UN=��������ѹ��ֵ 690->563V; 380->310V
		if(M_ChkFlag(SL_HVLV_DETEC)!=0 && TRS_NPR_U.ampflt < (0.9 * GRD_UN))			//GRD_UN=��������ѹ��ֵ 690->563V; 380->310V LVRTNR201103
		{																//������:NGS_Udq_lv��ʱ1ms,NGS_UdqҪ10ms
				//--------------------LVRT�����ж�--------------------------
				if(M_ChkCounter(MAIN_LOOP.cnt_lv_time,(int32)LV_T1)<=0)			//T1=625ms
				{
					if(NGS_Udq>(LV_A2 * GRD_UN))	//0.2 or 0.15	
					{
						M_SetFlag(SL_LV_STATE);	
						MAIN_LOOP.cnt_lv_fault1=0;
					}
					else   								
					{
						if(M_ChkCounter(MAIN_LOOP.cnt_lv_fault1,DELAY_LVFAULT1)>0)	//200ms
						{
//							M_SetFlag(SL_UACLV2);							//���200ms��ʱ
						}
					}
					MAIN_LOOP.cnt_lv_fault2=0;				
				}
				else if(M_ChkCounter(MAIN_LOOP.cnt_lv_time,(int32)LV_T2)<=0)  //3000ms T2=3s 0.625<t<=3s
				{
					temp_t = (float)MAIN_LOOP.cnt_lv_time;
					Ud_lv = GRD_UN * ((LV_A1-LV_A2)/(LV_T2-LV_T1) * (temp_t - LV_T1) + LV_A2); 	//Ud_lv=[((0.9-0.2)/(2000-625))*(t-625)+0.2]*GRD_UN
		    		if(NGS_Udq > Ud_lv)	
		    		{
		    			M_SetFlag(SL_LV_STATE);	
		    			MAIN_LOOP.cnt_lv_fault2=0;						
					}
					else   								
					{
						if(M_ChkCounter(MAIN_LOOP.cnt_lv_fault2,DELAY_LVFAULT2)>0)	//100ms
						{
//							M_SetFlag(SL_UACLV2);							//���100ms��ʱ
						}
					}
					MAIN_LOOP.cnt_lv_fault1=0;			
				}
				else	
				{
					M_SetFlag(SL_UACLV2);
					M_ClrFlag(SL_LV_STATE);
					MAIN_LOOP.cnt_lv_fault1=0;
					MAIN_LOOP.cnt_lv_fault2=0;	
				}
				//--------------------LVRT�����ж�--------------------------
			MAIN_LOOP.cnt_hvlv_rcv=0;	
		}
		else if(NGS_Udq_p > (0.9 * GRD_UN))									//������������
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_hvlv_rcv,DELAY_HVLVRCV)>0)			//20ms
			{
			 	M_ClrFlag(SL_LV_STATE);										//�ָ���
			 	M_ClrFlag(SL_UACLV2);		
			}
			MAIN_LOOP.cnt_lv_time=0;
			MAIN_LOOP.cnt_lv_fault1=0;	
			MAIN_LOOP.cnt_lv_fault2=0;		
		}		
		else	
		{
			MAIN_LOOP.cnt_hvlv_rcv=0;	
			MAIN_LOOP.cnt_lv_time=0;
			MAIN_LOOP.cnt_lv_fault1=0;	
			MAIN_LOOP.cnt_lv_fault2=0;	
		} 
	}
	else
	{
		M_ClrFlag(SL_LV_STATE);										
		M_ClrFlag(SL_UACLV2);	
		MAIN_LOOP.cnt_hvlv_rcv=0;	
		MAIN_LOOP.cnt_lv_time=0;
		MAIN_LOOP.cnt_lv_fault1=0;	
		MAIN_LOOP.cnt_lv_fault2=0;	
	}
*/
//------------------�жϵ���ʱ��ѹ�Ĳ�ƽ���--20110326---------------------------------------------------------------
	if(M_ChkFlag(SL_HVLV_DETEC)!=0 && M_ChkFlag(SL_LV_STATE)!=0)
	{
		if(NGS_Udq_n2p > 10)		//10��ʾ���ԳƵ���ﵽԼ20%		
//		if(NGS_Udq_n2p > _stdby05)	//�ⲿ����201011LVRT LVRTNR201103
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_unbalance,DELAY_LVUBALNCE)>0)	
			M_SetFlag(SL_UNBALANCE);//30ms <50ms ҪС��MPR��NPR��ʱ������ʱ��
		}
		else if(NGS_Udq_n2p < 6)	
//		else if(NGS_Udq_n2p < (_stdby05-5))	
		{
			M_ClrFlag(SL_UNBALANCE);
			MAIN_LOOP.cnt_unbalance=0;
		}
		else	MAIN_LOOP.cnt_unbalance=0;
	}
	else if	(M_ChkFlag(SL_HVLV_DETEC)!=0 && M_ChkFlag(SL_HV_STATE)!=0)
	{
		if(NGS_Udq_n2pex > 7)		//10��ʾ���ԳƵ���ﵽԼ14%	
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_unbalance,3)>0)
			M_SetFlag(SL_UNBALANCE);	
		}
		else if(NGS_Udq_n2pex < 5)	
		{
			M_ClrFlag(SL_UNBALANCE);
			MAIN_LOOP.cnt_unbalance=0;
		}
		else	MAIN_LOOP.cnt_unbalance=0;
	}
	else 	
	{
		M_ClrFlag(SL_UNBALANCE);
		MAIN_LOOP.cnt_unbalance=0;
	}

//����������6
/*	if((_COMMAND2&0x0040)!=0)	
	{
		if(M_ChkFlag(SL_LV_STATE)!=0)	M_SetFlag(SL_UNBALANCE);//=1����Ϊ�ǲ�ƽ�����,LV���������ֹ������201011LVRT
		else							M_ClrFlag(SL_UNBALANCE);	//20110327
	}
*/
//----------20130225���õ�ѹ�����жϴ���CROWBAR���ӿ�Ͷ���ٶ�-------------
	if(M_ChkFlag(SL_LV_STATE)!=0)
	{
	 if(M_ChkCounter(MAIN_LOOP.cnt_gridfault_last,30)<=0)		//����30ms����ִ���ж�		20130205
	 {
		if(M_ChkCounter(MAIN_LOOP.cnt_lv_start_scron,DELAY_LV_START_SCRON)<=0)
		{
//			if(NGS_Udq<300)		M_SetFlag(SL_LV_START_SCRON);
//			if(NGS_Udq<340)		//20130202 20%,35%
//			if(NGS_Udq<(NGS_Udq_p_ex * 0.5))		//20130221 20%(2ph/3ph),35%(3ph)
//			if(NGS_Udq<(NGS_Udq_p_ex * 0.41))		//20130223 20%(2ph/3ph),35%(3ph)
//			if((NGS_Udq<(NGS_Udq_p_ex * 0.41)) && (PRO.Pgactive_lv>600000))		//20130228 20%(2ph/3ph),35%(3ph),fullpower
//2013-12-6ZZJȡ��Ͷ��crowbar����������������
/*			if(NGS_Udq<(NGS_Udq_p_ex * 0.65))		//20130228 20%(2ph/3ph),35%(3ph),����Ͷ	20130308
			{
//				M_SetFlag(SL_LV_START_SCRON);
				if((_STDBY9&0x0100)==0)		M_SetFlag(SL_LV_START_SCRON);		//116.8	20130223
			}
//			if(NGS_Udq<(NGS_Udq_p_ex * 0.95))		//20130206 20%,35%,50%
			if((NGS_Udq<(NGS_Udq_p_ex * 0.95))||(PRO.NPR_iac>150.0))		//20130228 20%,35%,50%
			{
				if(M_ChkFlag(SL_NSTOP)==0)	M_SetFlag(SL_LV_ZDNSTOP);		//����˲����������������20130202
			}
*/
//			if(NGS_Udq<(NGS_Udq_p_ex * 0.7))		//20130221 50%���������й���
			if((NGS_Udq<(NGS_Udq_p_ex * 0.8))||(PRO.MPR_iac>150.0))		//20130228 80%�����»�1023A������ͣ���������
//			if((NGS_Udq<(NGS_Udq_p_ex * 0.9))||(PRO.MPR_iac>1000.0))		//20130228 50%���������й���
			{
				if(M_ChkFlag(SL_MSTOP)==0)	
				M_SetFlag(SL_LV_ZDMSTOP);		//����˲���������������20130202
			}
			if(NGS_Udq>520)		
			MAIN_LOOP.cnt_lv_start_scron = 0;		
			//20130205 XLY?������ʱ�������⣬�˾仰�������ڵ��仺��ʱ����ʱ������ͣ����Ҳ���ܻ�������ȥ��ǰ30ms�����˶�		
			MAIN_LOOP.cnt_lv_start_scroff=0;
		}
		else if(M_ChkCounter(MAIN_LOOP.cnt_lv_start_scroff,DELAY_VDSCROFF)>=0) //scr���ٵ�ͨ3+7=10ms
		{
			M_ClrFlag(SL_LV_START_SCRON);
		}
	 }
	}
	else
	{
		M_ClrFlag(SL_LV_START_SCRON);
		MAIN_LOOP.cnt_lv_start_scron = 0;
	}

//-------------------������ʱ�汾--MPR��������� ��ͣ10ms ����Դ(����������ͣ���ָ���ͣ��SCR������ͣ��������ͣ����ѹ��ѹ��ͣ)--------------------------------
//	if(M_ChkFlag(SL_MSTOP)==0 && M_ChkFlag(SL_LV_STATE)!=0 &&((PRO.MPR_iac>_SC_LVIAC2) || (PRO.udc>_SC_LVUDC1) || (M_ChkFlag(SL_SCRRUNING)!=0) || (M_ChkFlag(SL_LV_START_SCRON)!=0)))   //20110326mpr�����������Vdc��ѹ�������ƽ�� 201012
	if(M_ChkFlag(SL_MSTOP)==0 && M_ChkFlag(SL_LV_STATE)!=0 &&((PRO.MPR_iac>_SC_LVIAC2) || (PRO.udc>_SC_LVUDC1) || (M_ChkFlag(SL_SCRRUNING)!=0) || (M_ChkFlag(SL_LV_START_SCRON)!=0) || (M_ChkFlag(SL_LV_RECMSTOP)!=0) || (M_ChkFlag(SL_LV_ZDMSTOP)!=0)))   //20110326mpr�����������Vdc��ѹ�������ƽ�� 20130225
	{
		M_SetFlag(SL_MSTOP);	
		M_ClrFlag(SL_LV_ZDMSTOP);		//20130221	20130225
		MAIN_LOOP.cnt_mpwmrestart=0;									//LVRT��������ֹͣ
		Disepwmio_MPR();       											//201011LVRT
		M_ClrFlag(SL_MPR_PWMOUT);
		M_ClrFlag(SL_HVLV_MPWMOUT);

		if(M_ChkFlag(SL_LV_RECMSTOP)!=0)	MAIN_LOOP.cnt_lv_dlrecmstop=0;	//20130201 �ָ�ʱֻ��һ��
		M_ClrFlag(SL_LV_RECMSTOP);		//20130130 ����ֻ��һ��
	}
	else if(M_ChkFlag(SL_MSTOP)==0 && M_ChkFlag(SL_HV_STATE)!=0 &&((PRO.MPR_iac>_SC_LVIAC2) || (M_ChkFlag(SL_SCRRUNING)!=0)))
	{
		M_SetFlag(SL_MSTOP);	
		MAIN_LOOP.cnt_mpwmrestart=0;									//LVRT��������ֹͣ
		Disepwmio_MPR();       											//201011LVRT
		M_ClrFlag(SL_MPR_PWMOUT);
		M_ClrFlag(SL_HVLV_MPWMOUT);
	}
	else if(M_ChkFlag(SL_MSTOP)!=0)									//20110829����ָ�����
//	else if(M_ChkFlag(SL_MSTOP)!=0 && M_ChkFlag(SL_UNBALANCE)==0)	//LVRTNR201103
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_mpwmrestart,_stdby07)>=0)					//��ǰ���� ��ͣ10ms
		{
			if((PRO.MPR_iac<(_SC_LVIAC2-20)) && (PRO.udc<(_SC_LVUDC1-5)) && (M_ChkFlag(SL_SCRRUNING)==0))		//20121210	С���ʿ�������20130228
//			if(PRO.MPR_iac<(_SC_LVIAC2-20) && PRO.udc<(_SC_LVUDC1-5) && M_ChkFlag(SL_SCRRUNING)==0 && M_ChkFlag(SL_UNBALANCE)==0)
//				M_ClrFlag(SL_MSTOP);				  					//����LVRT������ɽ����Ҳ�ƽ����ʧ������������������20110829
//--------���˴���2��20��35������������������20130228--------
			{
//				if(M_ChkFlag(SL_UNBALANCE)==0)	M_ClrFlag(SL_MSTOP);
				if((M_ChkFlag(SL_UNBALANCE)==0) && (NGS_Udq_n2p<23))	
				M_ClrFlag(SL_MSTOP);		//20130306�ԳƵ����������23��Ӧ���ԳƵ���40%
//				else if((PRO.Pgactive_lv<600000) || (NGS_Udq_n2p<27))
//				else if(((PRO.Pgactive_lv<600000) || (NGS_Udq_n2p<27)) && ((_STDBY9&0x0080)!=0))	//116.7		20130303
				else if((NGS_Udq_n2p<31) && (NGS_Udq_p>(NGS_Udq_p_ex * 0.70)))	//���ԳƵ�����Ȳ�������50%�������������
				M_ClrFlag(SL_MSTOP);
			}
//------------------------
		}
	}
	else	MAIN_LOOP.cnt_mpwmrestart=0;

//------------------�޹�����--�޸ģ�δ��֤����ת�������й�20110327-------------------------
	if(M_ChkFlag(SL_LV_STATE)!=0)//20110829HC
//	if(M_ChkFlag(SL_HVLV_DETEC)!=0 && (NGS_Udq < (0.9 * GRD_UN)))//20111201bjtu�������а汾
//	if(M_ChkFlag(SL_HVLV_DETEC)!=0 && M_ChkFlag(SL_LV_STATE)!=0 && (NGS_Udq < (0.7 * GRD_UN)))//20110327
//	if(M_ChkFlag(SL_HVLV_DETEC)!=0 && M_ChkFlag(SL_LV_STATE)!=0 &&(NGS_Udq < (0.8 * GRD_UN)) && M_ChkFlag(SL_MSTOP)==0 && M_ChkFlag(SL_NSTOP)==0)//20110327
//	if(M_ChkFlag(SL_HVLV_DETEC)!=0 && M_ChkFlag(SL_LV_STATE)!=0 &&(NGS_Udq < (0.8 * GRD_UN)) && M_ChkFlag(SL_MSTOP)==0 && M_ChkFlag(SL_NSTOP)==0 && M_ChkFlag(SL_UNBALANCE)==0)//LVRTNR201103��ƽ������
	{
		M_SetFlag(SL_TRIG_ERRDSAVE);	 	//LVRT������������ʾ����201205LVRTatZB
		M_SetFlag(SL_LV_QWORKING);												//���෢�޹�
		M_ClrFlag(SL_HV_QWORKING);
		M_SetFlag(SL_LV_STRICTLV);			//20130222
		M_ClrFlag(SL_HV_STRICTLV);
		GIVE.lvwtiqrf = kq * 1775.0;	//BJTULVRT201204
		if(GIVE.lvwtiqrf > 1775)	GIVE.lvwtiqrf = 1775; //�޹������޷��޷�
		MAIN_LOOP.cnt_qworking = 0;	

//--------20130222	20130225--------
		if(M_ChkCounter(MAIN_LOOP.cnt_gridfault_last,500)>=0)	//500ms
		{
			if(((NGS_Udq_p - NGS_Udq_p_lv) > 55) || (NGS_Udq_p > (0.95 * NGS_Udq_p_ex)))
				M_ClrFlag(SL_LV_STRICTLV);
		}
//-------------------------
	}
	else if(M_ChkFlag(SL_HV_STATE)!=0)
	{
		M_SetFlag(SL_TRIG_ERRDSAVE);	 			
		M_SetFlag(SL_HV_QWORKING);			//���෢�޹�
		M_ClrFlag(SL_LV_QWORKING);
		M_SetFlag(SL_HV_STRICTLV);
		M_ClrFlag(SL_LV_STRICTLV);			
		GIVE.hvwtiqrf = kq * 1775.0;	   
		if(GIVE.hvwtiqrf > 1775)	GIVE.hvwtiqrf = 1775; //�޹������޷��޷�
		MAIN_LOOP.cnt_qworking = 0;
		if(M_ChkCounter(MAIN_LOOP.cnt_gridfault_last,500)>=0)	//hVRT��־λ�趨��500ms����
		{
			if(NGS_Udq_p < (1.05 * NGS_Udq_p_ex))    //��ѹ�ָ������
			M_ClrFlag(SL_HV_STRICTLV);
		}
//-------------------------
	}
//	else if(NGS_Udq > (_stdby01 * GRD_UN))//!!!!!!!!!!!
//	else if(NGS_Udq > (0.9 * GRD_UN))//!!!!!!!!!!!
	else	//20110829HC
	{
//		M_ClrFlag(SL_LV_QWORKING);												//����ֹͣ���޹�,�ָ��й�
		GIVE.lvwtiqrf = 0;
		GIVE.hvwtiqrf = 0;
		if(M_ChkCounter(MAIN_LOOP.cnt_qworking,DELAY_QWORKING)>=0)			//100ms 201205LVRTatZB
		{
			M_ClrFlag(SL_LV_QWORKING);											//LV��־λ�ָ�100ms������ֹͣ���޹�,�ָ��й�
			M_ClrFlag(SL_HV_QWORKING);
		}
		M_ClrFlag(SL_LV_STRICTLV);
		M_ClrFlag(SL_HV_STRICTLV);
	}

//--------LV NPR���������LV NPR��������ƣ�����������ʱ���������⡢�ָ�ʱ���������⣬�����ͣ��--------
//--------2013-12-6����_SC_LVIAC2��195��Ϊ190������_SC_LVIAC2-35��Ϊ_SC_LVIAC2-40	
	if(M_ChkFlag(SL_NSTOP)==0 && M_ChkFlag(SL_LV_STATE)!=0 && ((PRO.NPR_iac>(_SC_LVIAC2-45)) || M_ChkFlag(SL_LV_ZDNSTOP)!=0 || M_ChkFlag(SL_LV_RECNSTOP)!=0)) 
	{
		M_SetFlag(SL_NSTOP);
		M_ClrFlag(SL_LV_ZDNSTOP);		//20130202
		MAIN_LOOP.cnt_npwmrestart=0;									//LVRT��������ֹͣ	

//		if(M_ChkFlag(SL_LV_RECNSTOP)!=0) //2013-12-6ZZJȡ����ѹ�ָ�ʱ����������
//		M_ClrFlag(SL_LV_RECNSTOP);		//20130206 ����ֻ��һ��
	}
//	else if(M_ChkFlag(SL_NSTOP)!=0 && (PRO.NPR_iac<(_SC_LVIAC2-20)) && M_ChkFlag(SL_UNBALANCE)==0) //LVRTNR201103
	else if(M_ChkFlag(SL_NSTOP)==0 && M_ChkFlag(SL_HV_STATE)!=0 && (PRO.NPR_iac>(_SC_LVIAC2)))	
	{
		M_SetFlag(SL_NSTOP);
		MAIN_LOOP.cnt_npwmrestart=0;									//LVRT��������ֹͣ	
	}
	else if(M_ChkFlag(SL_NSTOP)!=0)			//20110829
	{
//		if(M_ChkCounter(MAIN_LOOP.cnt_npwmrestart,DELAY_NPWMRESTART)>=0)		//50ms
//		if(M_ChkCounter(MAIN_LOOP.cnt_npwmrestart,_stdby04)>=0)					//BJTULVRT201204
		if(M_ChkCounter(MAIN_LOOP.cnt_npwmrestart,5)>=0)					//20130309atsp��ͣ5ms
		{
//			 if(PRO.NPR_iac<(_SC_LVIAC2-20) && M_ChkFlag(SL_UNBALANCE)==0)
			 if(PRO.NPR_iac<(_SC_LVIAC2-45))				//20121226
				M_ClrFlag(SL_NSTOP);				  							//����LVRT������Ҳ�ƽ����ʧ�����������������?20110829
		}
	}
	else MAIN_LOOP.cnt_npwmrestart=0;

//-------------------LV SCR���������жϣ���ζ��SCR�Ѿ��ضϣ����˲ʱֵ��-20110829HC----------------
	if((abs(AD_OUT_SCR_I.a)>35) || (abs(AD_OUT_SCR_I.b)>35))	//SCR�����ӽ���   						
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_scrizero2,DELAY_SCRIZERO2)>=0)	M_ClrFlag(SL_SCRIZERO);	//1ms
		MAIN_LOOP.cnt_scrizero=0;	
	}
	else if((abs(AD_OUT_SCR_I.a)<25) && (abs(AD_OUT_SCR_I.b)<25))	//201007BJTULVRT?????I-LEM��Ư�����?
	{	
//		if(M_ChkCounter(MAIN_LOOP.cnt_scrizero,DELAY_SCRIZERO)>=0)	M_SetFlag(SL_LV_SCRIZERO);	  //15ms ͨ������ж�SCR�Ƿ񶼹ض�,DELAY_SCRIZERO�ӿ�ʵ��ȷ�� 
//		if(M_ChkCounter(MAIN_LOOP.cnt_scrizero,_stdby06)>=0)	M_SetFlag(SL_LV_SCRIZERO);	  //201007BJTULVRT ͨ������ж�SCR�Ƿ񶼹ض�,DELAY_SCRIZERO��ʱ��ʵ��ȷ�� 
		if(M_ChkCounter(MAIN_LOOP.cnt_scrizero,1)>=0)	M_SetFlag(SL_SCRIZERO);	  //201007BJTULVRT ͨ������ж�SCR�Ƿ񶼹ض�,DELAY_SCRIZERO��ʱ��ʵ��ȷ�� 
//		else	M_ClrFlag(SL_LV_SCRIZERO);								 
		MAIN_LOOP.cnt_scrizero2=0;							 
    }
	else	
	{
		MAIN_LOOP.cnt_scrizero=0;
		MAIN_LOOP.cnt_scrizero2=0;							 
	}

//---------------Crowbar���ƸĽ�20110829HC----------------------------------
/*	if(M_ChkFlag(SL_LV_STATE)!=0)	//���ԳƵ���crowbar��ͨ��3��֮��һֱ��ͨ��LV����20120602
	{
		if(M_ChkFlag(SL_UNBALANCE)!=0 && (CNT_SCRON > 2))
			M_SetFlag(SL_LV_SCRKEEPON);
		else
			M_ClrFlag(SL_LV_SCRKEEPON);
	}
	else
	{
		CNT_SCRON = 0;
		M_ClrFlag(SL_LV_SCRKEEPON);
	}
*///20130225
//-------------------LV SCR����Vdc�����ж�-----------------
//	if(((PRO.udc>1400) || (PRO.udc>_SC_LVUDC2 && M_ChkFlag(SL_LV_STATE)!=0))&& M_ChkFlag(SL_SCRON)==0)
/*	if(((PRO.udc>1400) || ((PRO.udc>_SC_LVUDC2 || PRO.MPR_iac>(_SC_LVIAC2+15)) && M_ChkFlag(SL_LV_STATE)!=0) || M_ChkFlag(SL_LV_START_SCRON)!=0)&& M_ChkFlag(SL_SCRON)==0) 
												//20120603night�����ܹ���Ҳ����crowbar	20121203
	{
		M_SetFlag(SL_SCRON);							//����SCR����ָ��
		MAIN_LOOP.cnt_lv_scron=0;						//��Ҫ����֤�´�SCR��ͨ��Ȼ��ʱ_SC_SCRON2010115�޸�
		CNT_SCRON++;		//20120602					
	}
	else if(PRO.udc< (_SC_LVUDC2-30))
	{
		M_ClrFlag(SL_SCRON);							//�ض�SCRָ��	
	}
*/
//--------20130225--------------
	if(M_ChkFlag(SL_SCRDONE)==0)		//�״�Ͷ���δͶ��
	{//SCRδ�����ң�ֱ����ѹ1400V���ʹ���ֱ����ѹ������ֵ����ǰ1180V����������������˲����������scr��
		if(((PRO.udc>1400) || ((PRO.udc>_SC_LVUDC2 || PRO.MPR_iac>(_SC_LVIAC2)) && M_ChkFlag(SL_LV_STATE)!=0) || M_ChkFlag(SL_LV_START_SCRON)!=0)&& M_ChkFlag(SL_SCRON)==0)  //2013-12-6 ZZJ�״�Ͷ��1300A
		{
			M_SetFlag(SL_SCRON);							//����SCR����ָ��
			MAIN_LOOP.cnt_lv_scron=0;						//��Ҫ����֤�´�SCR��ͨ��Ȼ��ʱ_SC_SCRON2010115�޸�
		}
		else if(PRO.udc< (_SC_LVUDC2-30))
		{
			M_ClrFlag(SL_SCRON);
		}
	}
	else  //�Ѿ�Ͷ����һ�������г�
	{//SCRδ�����ң�ֱ����ѹ1400V���ʹ���ֱ����ѹ������ֵ����ǰ1180V������������
		if(((PRO.udc>1400) || ((PRO.udc>1250 || PRO.MPR_iac>(_SC_LVIAC2+15)) && M_ChkFlag(SL_LV_STATE)!=0))&& M_ChkFlag(SL_SCRON)==0) 
		{
			M_SetFlag(SL_SCRON);							//����SCR����ָ��
			MAIN_LOOP.cnt_lv_scron=0;						//��Ҫ����֤�´�SCR��ͨ��Ȼ��ʱ_SC_SCRON2010115�޸�
		}
		else if(PRO.udc< (_SC_LVUDC2-30))
		{
			M_ClrFlag(SL_SCRON);
		}
	}

//-------------------LV SCR����-���ú��뽫������ʱ�������SL_SCRON����SL_SCRRUNING--------------------------------------
//	if(M_ChkFlag(SL_LV_SCRON)!=0 || M_ChkFlag(SL_LV_SCRKEEPON)!=0) //20120602											//Ҫ��Crowbar��ͨ
	if(M_ChkFlag(SL_SCRON)!=0)											//Ҫ��Crowbar��ͨ
	{		
		M_ACROWBARON();								 						//SCR��ͨ
		M_SetFlag(SL_SCRRUNING);											//SCR��ͨ��־λ
		M_SetFlag(SL_DISPLAY6);													
	}
	else if(M_ChkFlag(SL_SCRRUNING)!=0 && M_ChkFlag(SL_LV_STATE)!=0)//---------�ֵ�����Ⱥ������г�Crowbar20121210---------20130225
	{
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scroff3,_stdby04)>0)			//Ͷ��scr��5ms�ж�	20130309
			{
//				if(M_ChkFlag(SL_SCRDONE)!=0)
//				if((scroff1==1) || (M_ChkFlag(SL_SCRDONE)!=0) || ((NGS_Udq_epsilon > 18) && (PRO.Pgactive_lv<600000) && (NGS_Udq_p>(0.66 * NGS_Udq_p_ex))))	//С����35%-2ph������ֻͶ2-5ms	20130303
/*				if((scroff1==1) || (M_ChkFlag(SL_SCRDONE)!=0) || ((NGS_Udq_epsilon > 18) && (PRO.Pgactive_lv<600000) && (NGS_Udq_p>(0.55 * NGS_Udq_p_ex))))	//С����20%-2ph������ֻͶ2-5ms	20130308
				{
					scroff1 = 1;
					M_ACROWBAROFF();
					if(M_ChkFlag(SL_SCRIZERO)!=0)							//��⵽SCR����ӽ�Ϊ�?
					{
						M_ClrFlag(SL_SCRRUNING);								//����SCR��������
						MAIN_LOOP.cnt_lv_scroff3=0;								//��Ҫ����֤�´�SCR��ͨ��Ȼ��ʱ_SC_SCRON2010115�޸�	
						scroff1 = 0;
					}
				}
*///--------20130309С����20%�ٴ�Ͷ��--------
				if((M_ChkFlag(SL_SCRDONE)!=0) || (PRO.Pgactive_lv<600000))	//С����20%-2ph������ֻͶ2-5ms	20130308
				{
					M_ACROWBAROFF();
					if(M_ChkFlag(SL_SCRIZERO)!=0)							//��⵽SCR����ӽ�Ϊ�?
					{
						if((NGS_Udq_p<=(0.327 * NGS_Udq_p_ex)) && (scroff1==0) && (M_ChkFlag(SL_SCRDONE)==0))
						{//116.2	20-2phС�磬�״�Ͷ�룬��ȵ��䣬���ٴ�Ͷ��	20130314
							MAIN_LOOP.cnt_lv_scroff3=0;
//							M_ACROWBARON();
							if((_STDBY9&0x0004)!=0)		M_ACROWBARON();		
							scroff1 = 1;
						}
						else
						{
							M_ClrFlag(SL_SCRRUNING);								//����SCR��������
							MAIN_LOOP.cnt_lv_scroff3=0;								//��Ҫ����֤�´�SCR��ͨ��Ȼ��ʱ_SC_SCRON2010115�޸�	
							scroff1 = 0;
						}
					}
				}
			}
//-------------------------------------------
//------------------------
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scroff2,10)>0)			//Ͷ��scr��10ms�ж�
			{
//				if(((NGS_Udq_p>280) && (NGS_Udq_epsilon < 15)) || (M_ChkFlag(SL_LV_SCROFF2)!=0))				//������ȵ���50%	20130116
				if(((NGS_Udq_p>190) && (NGS_Udq_n2p < 15)) || (M_ChkFlag(SL_LV_SCROFF2)!=0))				//35%�������������35%��磩	20130221
				{
					if((NGS_Udq_p<250) && (PRO.Pgactive_lv>600000))	{}		//�޳�35%���	20130221
					else
					{
						M_ACROWBAROFF();											//SCR�ض�
						M_SetFlag(SL_LV_SCROFF2);					
			
						if(M_ChkFlag(SL_SCRIZERO)!=0)							//��⵽SCR����ӽ�Ϊ�?
						{				
							M_ClrFlag(SL_SCRRUNING);								//����SCR��������
							MAIN_LOOP.cnt_lv_scroff2=0;								//��Ҫ����֤�´�SCR��ͨ��Ȼ��ʱ_SC_SCRON2010115�޸�	
							M_ClrFlag(SL_LV_SCROFF2);
						}
					}
				}
			}
/*			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scroff1,lv_ctrl3)>0)			//Ͷ��scr��22ms�ж�
			{
//				if((NGS_Udq_p<190 && PRO.Pgactive_lv<600000) && (NGS_Udq_epsilon < 15) || (M_ChkFlag(SL_LV_SCROFF1)!=0))				//��20%��С�� 20130221
				if((PRO.Pgactive_lv<600000) || (M_ChkFlag(SL_LV_SCROFF1)!=0))				//����С�繤����������ʱ�� 20130303
				{
					M_ACROWBAROFF();											//SCR�ض�
					M_SetFlag(SL_LV_SCROFF1);

					if(M_ChkFlag(SL_SCRIZERO)!=0)							//��⵽SCR����ӽ�Ϊ�?
					{				
						M_ClrFlag(SL_SCRRUNING);								//����SCR��������
						MAIN_LOOP.cnt_lv_scroff1=0;								//��Ҫ����֤�´�SCR��ͨ��Ȼ��ʱ_SC_SCRON2010115�޸�				
						M_ClrFlag(SL_LV_SCROFF1);
					}
				}
			}
*///20130309ȡ��,�ָ�ʱע��ָ���ʱ��
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scron,_SC_TSCRON)>0)			//SCR��ͨһ��_SC_SCRON�̶����25ms
			{								
				if(((NGS_Udq_n2p > 40) && (PRO.Pgactive_lv>600000) && (NGS_Udq_p<(NGS_Udq_p_ex * 0.56))) || (scrhold==1))		//2013022 ���ʲ��ԳƵ���
				{
					 scrhold = 1;		//20130223
					 if(M_ChkFlag(SL_LV_SCRKEEPON)==0)
					 {
					 	M_ACROWBAROFF();		//20130223
						scrhold = 0;
					 }
				}
				else if(NGS_Udq_n2p > 26)		//����20%С���ʼ�35%��С����20130222
				{
//					if(M_ChkCounter(MAIN_LOOP.cnt_lv_unblcscroff2,50)>0)	M_ACROWBAROFF();		//SCR�ض�	50ms-20130221
					if(M_ChkCounter(MAIN_LOOP.cnt_lv_unblcscroff2,80)>0)	M_ACROWBAROFF();		//SCR�ض�	50ms-20130221
				}
				else	M_ACROWBAROFF();				//SCR�ض�

				if(M_ChkFlag(SL_SCRIZERO)!=0)							//��⵽SCR����ӽ�Ϊ�?
				{
					M_ClrFlag(SL_SCRRUNING);								//����SCR��������
					MAIN_LOOP.cnt_lv_scron=0;								//��Ҫ����֤�´�SCR��ͨ��Ȼ��ʱ_SC_SCRON2010115�޸�				
					MAIN_LOOP.cnt_lv_unblcscroff1 = 0;
					MAIN_LOOP.cnt_lv_unblcscroff2 = 0;
					MAIN_LOOP.cnt_lv_unblcscroff3 = 0;			//20130124
					scrhold = 0;								//20130223
				}
			}
	}
	else if(M_ChkFlag(SL_SCRRUNING)!=0) //�����г��߼��������ߵ�ѹ�г�
	{
			M_ACROWBAROFF();
			if(M_ChkFlag(SL_SCRIZERO)!=0)							//��⵽SCR����ӽ�Ϊ�?
			{
				M_ClrFlag(SL_SCRRUNING);								//����SCR��������

			}
	}
	else
	{
		M_ACROWBAROFF();
		M_ClrFlag(SL_LV_SCROFF1);
		M_ClrFlag(SL_LV_SCROFF2);
		MAIN_LOOP.cnt_lv_scron=0;
		MAIN_LOOP.cnt_lv_scroff1=0;		//20121210
		MAIN_LOOP.cnt_lv_scroff2=0;		//20121210
		MAIN_LOOP.cnt_lv_scroff3=0;		//20130221

		MAIN_LOOP.cnt_lv_unblcscroff1 = 0;		//20121210
		MAIN_LOOP.cnt_lv_unblcscroff2 = 0;		//20121210
		MAIN_LOOP.cnt_lv_unblcscroff3 = 0;		//20130124
		scrhold = 0;							//20130223
		scroff1 = 0;							//20130303
	}
//---------------------
//----------��һ��Ͷ��scr�����ж�20130222-----------20130225
	if((M_ChkFlag(SL_LV_STATE)!=0 && M_ChkFlag(SL_SCRON)!=0) || (scrdone == 1))
	{
		scrdone = 1;
		if(M_ChkFlag(SL_SCRRUNING)==0)	M_SetFlag(SL_SCRDONE);
	}
	if(M_ChkFlag(SL_LV_STATE)==0)
	{
		scrdone = 0;
		M_ClrFlag(SL_SCRDONE);
	}
//----------------------------------------------

//----------LV��̬���Ӵ������� ����ͻָ���100-200ms��ʵʩ BJTULVRT201204
/*	if(M_ChkFlag(SL_LV_STATE)!=0)	
	{
		M_SetFlag(SL_LV_PHICON);
		MAIN_LOOP.cnt_lv_phicontrol=0;
	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_lv_phicontrol,DELAY_PHICON)>=0)	//�ָ��������������ʱ��50ms 201205LVRTatZB
//		if(M_ChkCounter(MAIN_LOOP.cnt_lv_phicontrol,_stdby03)>=0)	//�ָ�������������ʱ�?
			 M_ClrFlag(SL_LV_PHICON);
	}		
*/			

//----------LV��̬PI���� ����������20ms��ʵʩ BJTULVRT201204
	if(M_ChkFlag(SL_MSTOP)!=0)
	{
		M_SetFlag(SL_LV_SYNCON);
		MAIN_LOOP.cnt_lv_lowpi = 0;
	}
	else
	{
//		if(M_ChkCounter(MAIN_LOOP.cnt_lv_lowpi,DELAY_LOWPI)>0) //20120323
//		if(M_ChkCounter(MAIN_LOOP.cnt_lv_lowpi,_stdby01)>0) 	// 20ms 20120601night
			M_ClrFlag(SL_LV_SYNCON);
	}

}


/*********************************************************************************************************
** ������?: Protect
** ��������: ����ֵ����
** �䡡��: 	 
** ��  ��:   
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:20121103
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Protect(void)
{
/*	float temp;

//----------------------------��������˲ʱֵ����---------------------------
	if(abs(AD_OUT_NPR_I.a)>abs( AD_OUT_NPR_I.b))
	{
		if(abs( AD_OUT_NPR_I.a)>abs( AD_OUT_NPR_I.c))	temp=abs(AD_OUT_NPR_I.a);
		else										    temp=abs(AD_OUT_NPR_I.c);
	}
	else
	{
		if(abs( AD_OUT_NPR_I.b)>abs( AD_OUT_NPR_I.c))	temp=abs(AD_OUT_NPR_I.b);
		else										    temp=abs(AD_OUT_NPR_I.c);
	}

	PRO.NPR_iac = temp * 100 / NPR_IACN;			//��������˲ʱ���ֵ����׼�����İٷ�ֵ

//----------------------------ת�ӵ���˲ʱֵ����---------------------------
	if(abs(AD_OUT_MPR_I.a)>abs(AD_OUT_MPR_I.b))
	{
		if(abs(AD_OUT_MPR_I.a)>abs(AD_OUT_MPR_I.c))		temp=abs(AD_OUT_MPR_I.a);
		else										temp=abs(AD_OUT_MPR_I.c);
	}
	else
	{
		if(abs(AD_OUT_MPR_I.b)>abs(AD_OUT_MPR_I.c))		temp=abs(AD_OUT_MPR_I.b);
		else										temp=abs(AD_OUT_MPR_I.c);
	}

	PRO.MPR_iac = temp * 100 / MPR_IACN;		//ת�ӵ����ֵ����׼����İٷ�ֵ

//------------------------�м�ֱ����ѹ����ֵ����-----------------------------

	PRO.udc  = AD_OUT_UDC;	                                 //��λV
*/
//--------------------------- �м�ն����������ֵ-201105atbjtuLVRT---------------------------
	PRO.CHOP_idc = AD_OUT_DC_I.b * 100 / CHOP_IDCN;

//---------------------------�����ߵ�ѹ��Чֵ����----------------------------
    PRO.NPR_uab= MEAN_DATA.uab1 * 1.110721;                      //����Uab��Чֵ��λV rms=mean*PAI/(2*sqrt(2)) 
	PRO.NPR_ubc= MEAN_DATA.ubc1 * 1.110721;                      //����Ubc��Чֵ��λV 1.110721=PAI * SQRT2 / 4
//	PRO.GID_uab = MEAN_DATA.uab * 1.110721;        //20091026atzy ������Uab��Чֵ��λV ��λV  rms=mean*PAI/(2*sqrt(2)) �� 1.110721=PAI * SQRT2 / 4
//	PRO.GID_ubc = MEAN_DATA.ubc * 1.110721;        //20091026atzy ������Uab��Чֵ��λV ��λV  rms=mean*PAI/(2*sqrt(2)) �� 1.110721=PAI * SQRT2 / 4
//	PRO.GID_uab = MEAN_DATA.uab1 * 1.110721;        //20091026atzy ������Uab��Чֵ��λV ��λV  rms=mean*PAI/(2*sqrt(2)) �� 1.110721=PAI * SQRT2 / 4
//	PRO.GID_ubc = MEAN_DATA.ubc1 * 1.110721;        //20091026atzy ������Uab��Чֵ��λV ��λV  rms=mean*PAI/(2*sqrt(2)) �� 1.110721=PAI * SQRT2 / 4

//---------------------------���Ӳ��ߵ�ѹ��Чֵ����----------------------------
    PRO.STA_uab= MEAN_DATA.uab2 * 1.110721;                    //���Uab��Чֵ��V
	PRO.STA_ubc= MEAN_DATA.ubc2 * 1.110721;                    //�����Ubc��Чֵ��λV

//------------------���ʱ�������ʾֵ����-----------------------------------------
//------------------���Ӳ�dq�任����Ƕ�ֵ--------------------------------------------------------------------
/*	temp = CAP4.nprtrstheta + 0.5236 - 1.57; //systest 201007BJTULVRT CAP4.nprtrstheta+30-90
//	temp = CAP4.nprtrstheta + (1.047 * 0.5); //systest 2010atcpc
	DIP_STA_I.sintheta = sin(temp);  //systest 2010atcpc
	DIP_STA_I.costheta = cos(temp);  //systest 2010atcpc
//------------------���Ӳ����dqֵ--------------------------------------------------------------------
   	DIP_STA_I.a     = AD_OUT_STA_I.a;
   	DIP_STA_I.b     = AD_OUT_STA_I.b;
   	DIP_STA_I.c     = AD_OUT_STA_I.c;
	Transform_3s_2s_2r(&DIP_STA_I);
	DataFilter(0.99,&DIP_STA_I.dflt,DIP_STA_I.d); 	//���Ӳ��������ֵ�˲�,Ts=200us,�˵�����Ƶ�ʴ�
	DataFilter(0.99,&DIP_STA_I.qflt,DIP_STA_I.q); 	//���Ӳ��������ֵ�˲�,Ts=200us,�˵�����Ƶ�ʴ�

//------------------���Ӳ��ѹdqֵ--------------------------------------------------------------------
	DIP_STA_U.dflt = 	DIP_NPR_U.dflt;			   //systest 2010atcpc
	DIP_STA_U.qflt = 	DIP_NPR_U.qflt;

//-----------------���㶨�Ӵ���--��Ҫת�Ӷ��ӹ��㣡201007BJTULVRT---------------------------------------------------------------------
	PHAI_d = DIP_STA_I.dflt * MPR_Ls + TRS_MPR_I.dflt * MPR_Lm / STAROTRTO;
	PHAI_q = DIP_STA_I.qflt * MPR_Ls + TRS_MPR_I.qflt * MPR_Lm / STAROTRTO;
	PHAI_dq = sqrt(PHAI_d * PHAI_d + PHAI_q * PHAI_q);

//------------------------�������ѹ��Чֵ��ʾֵ����(Ϊ���ʼ���)--------------------------------------------------
    PRO.sta_uar = MEAN_DATA.ua1 * 1.110721;         //��λV ������ѹ��Ϊ���ӵ�ѹ����Ϊ����V-LEM�����ѹ�����˴����˲?
	PRO.sta_ubr = MEAN_DATA.ub1 * 1.110721;
	PRO.sta_ucr = MEAN_DATA.uc1 * 1.110721;

//------------------------�����������Чֵ��ʾֵ����(Ϊ���ʼ���)--------------------------------------------------
    PRO.sta_iar = MEAN_DATA.ia3 * 1.110721;			//��λA
	PRO.sta_ibr = MEAN_DATA.ib3 * 1.110721;
	PRO.sta_icr = MEAN_DATA.ic3 * 1.110721;
*///����BJTULVRT201204

//------------------���Ӳ��й����޹�����ֵ--------------------------------------------------------------------
//    PRO.Psactive   = 1.5 * SQRT3 * (DIP_STA_U.dflt * DIP_STA_I.dflt + DIP_STA_U.qflt * DIP_STA_I.qflt); 
//    PRO.Psreactive = 1.5 * SQRT3 * (DIP_STA_U.qflt * DIP_STA_I.dflt - DIP_STA_U.dflt * DIP_STA_I.qflt); 
//	PRO.Ps		   = SQRT3 * (PRO.sta_uar * PRO.sta_iar + PRO.sta_ubr * PRO.sta_ibr + PRO.sta_ucr * PRO.sta_icr);//20091007

//------------------------���ಢ��������Чֵ��ʾֵ����----------------------------------------------
    PRO.npr_iar = MEAN_DATA.ia1 * 1.110721;     			 //1.110721=PAI * SQRT2 / 4  	//��λA
	PRO.npr_ibr = MEAN_DATA.ib1 * 1.110721;
	PRO.npr_icr = MEAN_DATA.ic1 * 1.110721;

//------------------------������ѹ��Чֵ��ʾֵ���?----------------------------------------------
    PRO.npr_uar = MEAN_DATA.ua1 * 1.110721;      			//1.110721=PAI * SQRT2 / 4  //��λA
	PRO.npr_ubr = MEAN_DATA.ub1 * 1.110721;
	PRO.npr_ucr = MEAN_DATA.uc1 * 1.110721;

//------------------����й����޹������?--------------------------------------------------------------------
// 	PRO.Pnactive   = Pnactive;
//	PRO.Pnreactive = Pnreactive;
//	PRO.Pn         = PRO.npr_iar * PRO.npr_uar + PRO.npr_ibr * PRO.npr_ubr + PRO.npr_icr * PRO.npr_ucr;

//------------------------------���Ӳ�������ܲ���������ʾ-----------------------------------------------------
//	PRO.Pgactive   = PRO.Psactive   + PRO.Pnactive;
//	PRO.Pgreactive = PRO.Psreactive + PRO.Pnreactive;

//	if(CAP4.omigaslp >= 0)	 PRO.Pg = PRO.Ps - PRO.Pn;
//	else 					 PRO.Pg = PRO.Ps + PRO.Pn;
//---------------------------����ͻ����¶��?----------------------------
    PRO.NPR_TLOV= MEAN_DATA.Lac_temp;                    //�������¶�
	PRO.MPR_TLOV= MEAN_DATA.Ldudt_temp;                  //�������¶�

//--------------------------- ���Ӳ��ߵ�������ֵ----------------------------
	PRO.STA_iac = MEAN_DATA.iac3 * 1.110721  * 100/ STA_IACN;
	PRO.STA_icb = MEAN_DATA.icb3 * 1.110721  * 100/ STA_IACN;

//���ʼ���201205LVRTatZB
//------------------�����й����޹�����ֵ--------------------------------------------------------------------
	if((M_ChkFlag(SL_IN1_CBSTS)!=0) && (M_ChkFlag(SL_IN1_MIANFILTER)!=0))	
		Pcreactive = DIP_NPR_U.dflt * DIP_NPR_U.dflt * 0.236;	//Pc=1.5*ed*ed*ws*Cn 201205LVRTatZB
	else	Pcreactive = 0;

//--------˲ʱ����201205LVRTatZB
 	PRO.Pnactive   = AD_OUT_NGS_U.a * AD_OUT_NPR_I.a + AD_OUT_NGS_U.b * AD_OUT_NPR_I.b + AD_OUT_NGS_U.c * AD_OUT_NPR_I.c;
	PRO.Pnreactive = SQRT3_3 *  (AD_OUT_NGS_U.bc * AD_OUT_NPR_I.a + (AD_OUT_NGS_U.c-AD_OUT_NGS_U.a) * AD_OUT_NPR_I.b + AD_OUT_NGS_U.ab * AD_OUT_NPR_I.c);
	PRO.Pnreactive = PRO.Pnreactive + Pcreactive;

    PRO.Psactive   = AD_OUT_NGS_U.a * AD_OUT_STA_I.ac + AD_OUT_NGS_U.b * AD_OUT_STA_I.ba + AD_OUT_NGS_U.c * AD_OUT_STA_I.cb; 
    PRO.Psreactive = SQRT3_3 *  (AD_OUT_NGS_U.bc * AD_OUT_STA_I.ac + (AD_OUT_NGS_U.c-AD_OUT_NGS_U.a) * AD_OUT_STA_I.ba + AD_OUT_NGS_U.ab * AD_OUT_STA_I.cb); 

//---------------------------ת�ٱ���ֵ����----------------------------------
	PRO.speed   = 9.5492966 * QEPDATA.omigamec;		//��λ��ת/�֣�n=60*w/2pai=*w
//	PRO.speed   = 1200;		//controltest

	DataFilter(0.45,&PRO.speedflt,PRO.speed); 				//����ת�ٷ���ֵ�˲��� Ts=200us,Tr=5ms 20091021atzy֮ǰ123ms�˲�̫����
	
	if(PRO.speedflt>700)		M_SetFlag(SL_SPEED_HIGH);	//�����ת����   20091021atzy 650��Ϊ700���Ӵ��ͻ�
    else if(PRO.speedflt<600)	M_ClrFlag(SL_SPEED_HIGH);   //650r/min���ܷ�PRD=845.��Ƶ��POSLAT=887.

    if((PRO.speedflt > _SC_MSPEED1) && (PRO.speedflt < _SC_MSPEED2)) 	 
    		M_SetFlag(SL_SPEED_IN_RANGE);  
    else if((PRO.speedflt < (_SC_MSPEED1-SC_MSPD_HW)) || (PRO.speedflt > (_SC_MSPEED2 + SC_MSPD_HW))) 	 
			M_ClrFlag(SL_SPEED_IN_RANGE);					//�жϼӻز�201005atcpc

//--------------20120321---------------
	if((PRO.speedflt < 1050) && (M_ChkFlag(SL_IN1_STATORK)!=0))
	{
		M_SetFlag(SL_SYSOFF);
		MAIN_LOOP.cnt_sysoff = 0;
	}
//	else if((PRO.speedflt > 1060) || (M_ChkFlag(SL_IN1_STATORK)==0))
	else if((PRO.speedflt > 1100) && (M_ChkFlag(SL_IN1_STATORK)==0))	//20130308
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_sysoff,DELAY_SYSOFF)>=0)		M_ClrFlag(SL_SYSOFF);
	}
//-------------------------------------

//----------------------------�������㲢���ù�����,��ʾ---------------------------------------
//------------------------------���Ӳ�������ܲ���������ʾ-----------------------------------------------------
//	PRO.Pgactive = Te_feedback * PRO.speedflt * PAI * 0.03333333;	//P=T*2PIE*n/60 201005atcpc
	PRO.Pgactive2   = PRO.Psactive   + PRO.Pnactive;					
	PRO.Pgreactive2 = PRO.Psreactive + PRO.Pnreactive;
	DataFilter(0.99,&PRO.Pgactive,PRO.Pgactive2); 		//ע�����˲�?400us/2.5kHzִ��һ�Ρ����Ӳ��������ֵ�˲�,Ts=200us,�˵�����Ƶ�ʴ�0.97
	DataFilter(0.99,&PRO.Pgreactive,PRO.Pgreactive2); 	//c=0.99->8Hz; c=0.9->88Hz

	if(M_ChkFlag(SL_LV_QWORKING)==0) 	PRO.Pgactive_lv = PRO.Pgactive;		//�ж�LVRT֮ǰ����20121210

	if((_STDBY9&0x0008)!=0)	//116.3 201205LVRTatZB
    	PRO.Pgactive   = Te_feedback * PRO.speedflt * PAI * 0.03333333;	//P=T*2PIE*n/60 201005atcpc

//----------------------------����ӵع���-���������ƽ�Ᵽ���ж�---------------------------20100302
	if((AD_OUT_NPR_I.a+AD_OUT_NPR_I.b+AD_OUT_NPR_I.c)>120)
			M_SetFlag(SL_IUBLNCE1);
	else	M_ClrFlag(SL_IUBLNCE1);

//----------------------------����ӵع���-���������ƽ�Ᵽ���ж�---------------------------20100302
	if((AD_OUT_MPR_I.a+AD_OUT_MPR_I.b+AD_OUT_MPR_I.c)>200)
			M_SetFlag(SL_IUBLNCE2);
	else	M_ClrFlag(SL_IUBLNCE2);

}
/*********************************************************************************************************
** ��������: Scout
** ��������: ϵͳ���ϼ�⼰�������
** ���룺
** ���:        
**  ��: 
**-------------------------------------------------------------------------------------------------------
** ����: 
** ����: 
**-------------------------------------------------------------------------------------------------------
** �޸��ߣ�
** ����:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Scout(void)
{

//  float temp_pgactive,temp_pgreactive;
  Uint16 tempb,tempc;
//------------------�����ӳ����ϱ����Ϣ���ж���TAB_MSG����һ��-------------------------------------
	if(M_ChkFlag(SL_POWERON)==0)										//������ɹ����������
	{	
//---------------------------------TZ1�жϱ���------------------------------------------------------
		if(M_ChkFlag(SL_PDPINTA)!=0)	
		{	
			tempb = *IN2_ADDR;
			tempc = *IN3_ADDR;
			_IN12_DATA = (_IN12_DATA & 0x00FF) | ((tempb<<8) & 0xFF00);
			_IN34_DATA = (tempc & 0x00FF);

			if(M_ChkFlag(SL_IN2_IOVA1)!=0)			M_SetFlag(SL_HIA1);		//����A��SKiiP����
			else if(M_ChkFlag(SL_IN2_IOVB1)!=0)	   	M_SetFlag(SL_HIB1);		//����B��SKiiP����
		    else if(M_ChkFlag(SL_IN2_IOVC1)!=0)	   	M_SetFlag(SL_HIC1);		//����C��SKiiP����
			else if(M_ChkFlag(SL_IN3_VDCOV)!=0)   	M_SetFlag(SL_HUDCOV);	//ֱ�����ѹ����
			else if(M_ChkFlag(SL_IN3_NPRIOV)!=0)  	M_SetFlag(SL_HIACOV1);	//����Ӳ����������
//2015-12-7
			else if(M_ChkFlag(SL_IN2_IOVA2)!=0)		M_SetFlag(SL_HIA2);			//�����A��SKiiP����
			else if(M_ChkFlag(SL_IN2_IOVB2)!=0)	    M_SetFlag(SL_HIB2);			//�����B��SKiiP����
		    else if(M_ChkFlag(SL_IN2_IOVC2)!=0)	    M_SetFlag(SL_HIC2);			//�����C��SKiiP����
		    else if(M_ChkFlag(SL_IN3_MPRIOV)!=0)    M_SetFlag(SL_HIACOV2);		//�����Ӳ����������
			else if(M_ChkFlag(SL_IN3_IOVDC)!=0)  	M_SetFlag(SL_HIDCOV);		//�м�ֱ��ն��IGBTӲ������20120228atbjtu_chopper	BJTULVRT201204

			else if(M_ChkFlag(SL_PDPASERIES)!=0)	M_ClrFlag(SL_PDPINTA);	//����������������	20091107atzy						
			else									M_SetFlag(SL_PDPASERIES);//����Ӳ������,CPLDû�д浽����
		}
		else
		{
			M_ClrFlag(SL_HIA1);   	    									//����ϱ�־
			M_ClrFlag(SL_HIB1);
			M_ClrFlag(SL_HIC1);
			M_ClrFlag(SL_HUDCOV);
			M_ClrFlag(SL_HIACOV1);  			

			M_ClrFlag(SL_HIA2);    											//����ϱ�־
			M_ClrFlag(SL_HIB2);	   		
			M_ClrFlag(SL_HIC2);	    
			M_ClrFlag(SL_HIACOV2);
			M_ClrFlag(SL_HIDCOV);  						//20120228	BJTULVRT201204

			M_ClrFlag(SL_PDPASERIES);  			
		}

//---------------------------------TZ2�жϱ���-------------------------------------------------------
		if(M_ChkFlag(SL_PDPINTB)!=0)	
		{	
			tempb = *IN2_ADDR;
			tempc = *IN3_ADDR;
			_IN12_DATA = (_IN12_DATA & 0x00FF) | ((tempb<<8) & 0xFF00);
			_IN34_DATA = (tempc & 0x00FF);

			if(M_ChkFlag(SL_PDPBSERIES)!=0)	M_ClrFlag(SL_PDPINTB);		//��������������	20091107atzy						
			else									M_SetFlag(SL_PDPBSERIES);	//����Ӳ������,CPLDû�д浽����
		}
		else
		{
			M_ClrFlag(SL_PDPBSERIES);	
		}

//---------------------------------E-STOP����-------------------------------------------------------
		if(M_ChkFlag(SL_IN1_EXESTOP)!=0)								//�ⲿ��ͣ����  �������źŬ�糡û�?								
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_estop,DELAY_ESTOP)>=0)   M_SetFlag(SL_ESTOP);		//����ֹͣ�ӳ�ʱ�䵽��
			else M_ClrFlag(SL_ESTOP);									//���־�?
		}
		else 
		{
			MAIN_LOOP.cnt_estop=0;										//�嶨ʱ��
			M_ClrFlag(SL_ESTOP);
		}  

//---------------------------------�������ⲿӲ�����ϱ���-------------------------------------------------
		if((M_ChkFlag(SL_IN1_EXFAULTOK)==0)||(M_ChkFlag(SL_IN1_MIANFAN)==0))	//�ⲿ���϶������߹��������? new							
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_exfault,DELAY_EXFAULT)>=0)   M_SetFlag(SL_EXFAIL);	//�ⲿӲ�������ӳ�ʱ�䵽��
			else M_ClrFlag(SL_EXFAIL);							     //���־λ
		}
		else 
		{
			MAIN_LOOP.cnt_exfault=0;                               	 //�嶨ʱ��
			M_ClrFlag(SL_EXFAIL);
		}	

//-----------------------CANOPENOVER CANͨѶ����-----------------------------------------------------------
	    if(M_ChkFlag(SL_ENPCOPER)==0) 											//�ϵ���ѡ��CAN���ƲŽ���ͨѶ����
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_canfault,DELAY_CANFAULT)>=0)		//10s  20090816
		  	{
				if(M_ChkCounter(SCI_canopen.cnt_heartbeat,DELAY_CANOPENOVER)>0)
				{                                   						
			   		 M_SetFlag(SL_CANOPENOVER);
//			   		 SCI_canopen.rx_controlword=0;
//					 SCI_canopen.rx_torque=0;
//					 SCI_canopen.rx_angle=0;
				}
				else M_ClrFlag(SL_CANOPENOVER); 
		  	}   
        }
		else  
		{
			MAIN_LOOP.cnt_canfault=0;
			M_ClrFlag(SL_CANOPENOVER);
		}

//-----------------------CBTRIP����(���Ϲ����ѿ�)---------------------------------------

	    if(M_ChkFlag(SL_IN1_CBRESET)==0)							   	//1=���ط���Ҫ�����ϸ�λ�ź�
		{		
			if(M_ChkFlag(SL_IN3_CBTRIP)!=0)								//���Ϲ����ѿ۹���	090816	
	    	{
	    		if(M_ChkCounter(MAIN_LOOP.cnt_cbtp,DELAY_CBTP)>=0)	   	//�����ѿ���Сʱ�䵽?
					M_SetFlag(SL_CBTRIP);	
				else 
					M_ClrFlag(SL_CBTRIP);								//���־λ	    
	    	}
			else   	MAIN_LOOP.cnt_cbtp=0;		
		}
		else if(M_ChkFlag(SL_IN3_CBTRIP)==0)
		{
			M_ClrFlag(SL_CBTRIP);								 		//���־λ
			MAIN_LOOP.cnt_cbtp=0;
		}
		else	MAIN_LOOP.cnt_cbtp=0;
	    	    

//-----------------------�����ض�������110818---------------------------------------
//	if(M_ChkFlag(SL_SWITCHDETECT)==0)
//	{
//----���ݽ��Ϸ糡��bug��ʱȡ���ü��----20121024----
//-----------------------���Ϻ�բ/��բ/����Ͽ�����---------------------------------------
	    if((M_ChkFlag(CL_CBON)!=0 && M_ChkFlag(SL_IN1_CBSTS)==0)||(M_ChkFlag(CL_CBLVC)==0 && M_ChkFlag(SL_IN1_CBSTS)!=0)||(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(CL_CBLVC)!=0 && M_ChkFlag(SL_CBCLOSED)!=0 && M_ChkFlag(SL_IN3_CBTRIP)==0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_cberror,DELAY_CBERROR)>=0)	M_SetFlag(SL_ERROR_CB);	  //2s  
			else M_ClrFlag(SL_ERROR_CB);								 //���־λ
		}
		else 
		{
			MAIN_LOOP.cnt_cberror=0;                                    //��ʱ��
			M_ClrFlag(SL_ERROR_CB);
		} 
		
//-----------------------���Ӵ����պ�/�Ͽ�����---------------------------------------
	    if((M_ChkFlag(CL_MAINK)!=0 && M_ChkFlag(SL_IN1_MIANK)==0)||(M_ChkFlag(CL_MAINK)==0 && M_ChkFlag(SL_IN1_MIANK)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_mainkerror,DELAY_MAINKERROR)>=0)	M_SetFlag(SL_ERROR_MAINK);	
			else M_ClrFlag(SL_ERROR_MAINK);								 //���־λ
		}
		else 
		{
			MAIN_LOOP.cnt_mainkerror=0;                                    //�嶨ʱ��
			M_ClrFlag(SL_ERROR_MAINK);
		}  
		
//-----------------------���˲����պ�/�Ͽ�����---------------------------------------
	    if((M_ChkFlag(CL_MAINK)!=0 && M_ChkFlag(SL_IN1_MIANFILTER)==0)||(M_ChkFlag(CL_MAINK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_mainferror,DELAY_MAINFERROR)>=0)	M_SetFlag(SL_ERROR_MAINF);	
			else M_ClrFlag(SL_ERROR_MAINF);								 //���־λ
		}
		else 
		{
			MAIN_LOOP.cnt_mainferror=0;                                    //�嶨ʱ��
			M_ClrFlag(SL_ERROR_MAINF);
		}   

//-----------------------���ӽӴ����պ�/�Ͽ�����---------------------------------------
	    if((M_ChkFlag(CL_STATORK)!=0 && M_ChkFlag(SL_IN1_STATORK)==0)||(M_ChkFlag(CL_STATORK)==0 && M_ChkFlag(SL_IN1_STATORK)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_stacerror,DELAY_STACERROR)>=0)	M_SetFlag(SL_ERROR_STAC);	
			else M_ClrFlag(SL_ERROR_STAC);								 //���־λ
		}
		else 
		{
			MAIN_LOOP.cnt_stacerror=0;                                    //�嶨ʱ��
			M_ClrFlag(SL_ERROR_STAC);
		}   
//	}

//-----------------------���������⶯������110818---------------------------------------
/*	else if(M_ChkFlag(SL_SWITCHDETECT)!=0)
	{
//-----------------------���Ϻ�բ/��բ����---------------------------------------
	    if((M_ChkFlag(CL_CBON)!=0 && M_ChkFlag(SL_IN1_CBSTS)==0) || (M_ChkFlag(CL_CBFCON)==0 && M_ChkFlag(SL_IN1_CBSTS)!=0))	     //110723��CL_CBFCON�滻CL_CBLVC							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_cberror,DELAY_SWITCHDETECT)>=0)	M_SetFlag(SL_ERROR_CB);	   
			else M_ClrFlag(SL_ERROR_CB);								 //���־λ
		}
		else 
		{
			MAIN_LOOP.cnt_cberror=0;                                    //�嶨ʱ��
			M_ClrFlag(SL_ERROR_CB);
		} 
		
//-----------------------���Ӵ����պ�/�Ͽ�����---------------------------------------
	    if((M_ChkFlag(CL_MAINK)!=0 && M_ChkFlag(SL_IN1_MIANK)==0)||(M_ChkFlag(CL_MAINK)==0 && M_ChkFlag(SL_IN1_MIANK)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_mainkerror,DELAY_SWITCHDETECT)>=0)	M_SetFlag(SL_ERROR_MAINK);	
			else M_ClrFlag(SL_ERROR_MAINK);								 //���־λ
		}
		else 
		{
			MAIN_LOOP.cnt_mainkerror=0;                                    //�嶨ʱ��
			M_ClrFlag(SL_ERROR_MAINK);
		}  
		
//-----------------------���˲����պ�/�Ͽ�����---------------------------------------
	    if((M_ChkFlag(CL_MAINK)!=0 && M_ChkFlag(SL_IN1_MIANFILTER)==0)||(M_ChkFlag(CL_MAINK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_mainferror,DELAY_SWITCHDETECT)>=0)	M_SetFlag(SL_ERROR_MAINF);	
			else M_ClrFlag(SL_ERROR_MAINF);								 //���־λ
		}
		else 
		{
			MAIN_LOOP.cnt_mainferror=0;                                    //�嶨ʱ��
			M_ClrFlag(SL_ERROR_MAINF);
		}   

//-----------------------���ӽӴ����պ�/�Ͽ�����---------------------------------------
	    if((M_ChkFlag(CL_STATORK)!=0 && M_ChkFlag(SL_IN1_STATORK)==0)||(M_ChkFlag(CL_STATORK)==0 && M_ChkFlag(SL_IN1_STATORK)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_stacerror,DELAY_SWITCHDETECT)>=0)	M_SetFlag(SL_ERROR_STAC);	
			else M_ClrFlag(SL_ERROR_STAC);								 //���־λ
		}
		else 
		{
			MAIN_LOOP.cnt_stacerror=0;                                    //�嶨ʱ��
			M_ClrFlag(SL_ERROR_STAC);
		}
	}
*/
//----------------------Ԥ������---------------------------------------
	    if((M_ChkFlag(CL_PRE)!=0)&&(M_ChkFlag(CL_MAINK)==0))	     	//							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_preerror,DELAY_PREERROR)>=0)	M_SetFlag(SL_ERROR_PRE);  //15s	
			else M_ClrFlag(SL_ERROR_PRE);								 //���־λ
		}
		else 
		{
			MAIN_LOOP.cnt_preerror=0;                                    //�嶨ʱ��
			M_ClrFlag(SL_ERROR_PRE);
		}  
 
//-----------------------ENCODFAULT����(Qep���,��������?)-----------------------------------------
//	    if(((M_ChkFlag(SL_QEPPCO)!=0)||(M_ChkFlag(SL_QCAPDISTURB)!=0)||(M_ChkFlag(SL_QEPZDISTRUB)!=0))&&(M_ChkFlag(SL_OCS_EIN)!=0))	//QEP�������QCAP��QEP Z�ź��ܸ��Ź���  20090804��cpc							
	    if((M_ChkFlag(SL_QCAPSPDIN)!=0)&&((M_ChkFlag(SL_QEPPCO)!=0)||(M_ChkFlag(SL_QCAPDISTURB)!=0)||(M_ChkFlag(SL_QEPZDISTRUB)!=0)||(M_ChkFlag(SL_QEPPCDE)!=0)))	//QEP�������QCAP��QEP Z/A/B�ź��ܸ��Ź���  201007BJTULVRT						
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_encofault,DELAY_ENCOFAULT)>=0)	//150ms 2010atcpc									//�ì��ת�ٷ����־λ
				M_SetFlag(SL_ENCODFAULT);				     			    //�ñ�־λ
		}
		else 
		{
			M_ClrFlag(SL_ENCODFAULT);									//���־λ
			MAIN_LOOP.cnt_encofault=0;
		} 

//-----------------------�������ת�ٷ�Χ�ж�-----------------------------------------------------
//	    if(M_ChkFlag(SL_OCS_EIN)!=0)									//bit0�Ž���ת�٬����Χ�ж? 201005atcpc
	    if((M_ChkFlag(SL_OCS_EIN)!=0)&&(M_ChkFlag(SL_IN1_CBSTS)!=0))	//bit0�Ž���ת�ٳ�����Χ�ж� 20120310
		{
//			if((M_ChkFlag(SL_MSPOUT)==0)&& ((PRO.speedflt<_SC_MSPEED1)||(PRO.speedflt>_SC_MSPEED2)))  //�Ӹ���ʱ�ж�20090817
			if((M_ChkFlag(SL_MSPOUT)==0) && (M_ChkFlag(SL_MPR_PWMOUT)!=0) && ((PRO.speedflt<_SC_MSPEED1)||(PRO.speedflt>_SC_MSPEED2)))  //�Ӹ��ӱ�ж?220120310
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_speedout,DELAY_SPEEDOUT)>=0)	M_SetFlag(SL_MSPOUT);	//50ms 20091022atzy									//�ì��ת�ٷ����־λ
			}
			else if((PRO.speedflt>(_SC_MSPEED1 + SC_MSPD_HW))&&(PRO.speedflt<(_SC_MSPEED2 - SC_MSPD_HW)))   //�жϼӻز�201005atcpc
			{
				MAIN_LOOP.cnt_speedout=0;	
				M_ClrFlag(SL_MSPOUT);
			}			
		}
		else 
		{
			M_ClrFlag(SL_MSPOUT);
			MAIN_LOOP.cnt_speedout=0;
		}
/*
//-----------------------�����������������ж�-----------------------------------------------------
		if((M_ChkFlag(SL_SIAC1)==0)&&(PRO.NPR_iac>_SC_IACOV1))
		{
			M_SetFlag(SL_SIAC1);									//������������?
		}
		else if((M_ChkFlag(SL_SIAC1)!=0)&&(PRO.NPR_iac<(_SC_IACOV1-SC_IAC_HW))) 
		{
			M_ClrFlag(SL_SIAC1);									//�����������־λ
		}
 

//------------------------������������������ж�--------------------------------------------------
		if((M_ChkFlag(SL_SIAC2)==0)&&(PRO.MPR_iac>_SC_IACOV2))
		{
			M_SetFlag(SL_SIAC2);									//�����������־λ
		}
		else if((M_ChkFlag(SL_SIAC2)!=0)&&(PRO.MPR_iac<(_SC_IACOV2-SC_IAC_HW))) 
		{
			M_ClrFlag(SL_SIAC2);									//�����������־λ
		}
*/
/*
//-------------------------�м�ֱ����ѹ���Ƿѹ�ж�----�����⣡�ù�������---------------------------------------------
	    if((M_ChkFlag(SL_CHARGEOK)!=0)&&(M_ChkFlag(SL_NPR_PWMOUT)!=0))//Ԥ�����ɺ�Ž��Ƿѹ�ж?
		{
			if((M_ChkFlag(SL_SUDCLV)==0)&&(PRO.udc<_SC_UDCLV))	
			{
				M_SetFlag(SL_SUDCLV);									//֮ǰû��Ƿѹ���˿̼���Ƿ����Ƿѹ��־
			}	
			else if((M_ChkFlag(SL_SUDCLV)!=0)&&(PRO.udc>=(_SC_UDCLV+SC_UDC_HW)))	
			{
				M_ClrFlag(SL_SUDCLV);									//֮ǰǷѹ���˿̼�������(Ƿѹֵ�ӻز�)����Ƿѹ��־
			}		
		}
		else	M_ClrFlag(SL_SUDCLV);				
*/
//-----------------------�м�ֱ��ն�����������ж�-BJTULVRT201204----------------------------------------------------------
		//chooper������
		if(((_COMMAND2&0x0002)==0)&&(PRO.CHOP_idc>10)&&((M_ChkFlag(SL_LV_STATE)!=0)||(M_ChkFlag(SL_HV_STATE)!=0)))
		{
			if(MAIN_LOOP.cnt_idcov!=65535)		MAIN_LOOP.cnt_idcov++;	
			if (M_ChkCounter(MAIN_LOOP.cnt_idcov,1294)>=0) //1294  ��ӦI2RT=350kJ
			{
				M_SetFlag(SL_IDCOV);
			}		
		}
		else
		{
			MAIN_LOOP.cnt_idcov = 0;
			M_ClrFlag(SL_IDCOV);
		}
		
//------------------------�м�ֱ����ѹ�����ѹ�ж�--------------------------------------------------
		if((M_ChkFlag(SL_SUDCOV)==0)&&(PRO.udc>_SC_UDCOV))	                   
		{
			M_SetFlag(SL_SUDCOV);	 	//
		}
		else if((M_ChkFlag(SL_SUDCOV)!=0)&&(PRO.udc<=(_SC_UDCOV-SC_UDC_HW)))   M_ClrFlag(SL_SUDCOV);  //֮ǰ��ѹ���˿̼����ڹ�ѹֵ���ز�	


//-----------------------��ѹǷѹ����ж�(��ѹ��Чֵ)-----------------------------------------------
	   if(M_ChkCounter(MAIN_LOOP.cnt_uaclv1,DELAY_UACLV1)>0)	//10s �ϵ���ʱ�ж�������Ƿѹ���� �˴����Ƶʹ�ʱ�͵�ѹ�����˵��ˣ����Բ��ᱨ��
	   {
		if(M_ChkFlag(SL_UACLV1)==0)
		{
			if((PRO.NPR_uab<_SC_UACLV1)||(PRO.NPR_ubc<_SC_UACLV1))	M_SetFlag(SL_UACLV1);  //20091026atzy ���ߵ�ѹ����һ������Ҫ��,��Ƿ��?
		}
		else 
		{
			if((PRO.NPR_uab>=(_SC_UACLV1+SC_UAC_HW))&&(PRO.NPR_ubc>=(_SC_UACLV1+SC_UAC_HW)))	M_ClrFlag(SL_UACLV1); //20091026atzy �����ߵ�ѹ��ظ�����������Ƿ���־	
		}
	   }					
//----------------------��ѹ��ѹ����ж�(��ѹ��Чֵ)------------------------------------------------
		if(M_ChkFlag(SL_HV_STATE)==0)
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_uacover,10)>=0) //�ߵ�ѹ�ָ�10s���ж�
			{
			if((PRO.NPR_uab>_SC_UACOV1)||(PRO.NPR_ubc>_SC_UACOV1))	 M_SetFlag(SL_UACOV1);
			else if((PRO.NPR_uab<=(_SC_UACOV1-SC_UAC_HW))&&(PRO.NPR_ubc<=(_SC_UACOV1-SC_UAC_HW)))	M_ClrFlag(SL_UACOV1);
			}
		}
		else
			MAIN_LOOP.cnt_uacover = 0;
		


//------------------------����Ƶ�ʹ���--------------------------------------------------------------
/*		if(M_ChkFlag(SL_IN1_CBSTS)!=0)     									//���ϱպ����ж���Ƶ���� 20090816
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_cbfreq,DELAY_CBFREQ)>=0)      	//1s
			{
*/				if(M_ChkFlag(SL_GRDFQE)!=0)									//10����ѹ����(200ms),����Ƶ�ʴ����־
				{
					if(M_ChkCounter(MAIN_LOOP.cnt_freq,DELAY_FREQ)>=0)	 M_SetFlag(SL_FE1);	
				}	
				else
				{
					M_ClrFlag(SL_FE1);									   	//������Ƶ�ʴ����־
					MAIN_LOOP.cnt_freq=0;                                  	//��ʱ��ÿ1ms��1
				}
/*			}
		}
		else   	
		{
			MAIN_LOOP.cnt_cbfreq=0;	
			M_ClrFlag(SL_FE1);
		}
*/	
//-----------------------���ӵ��������ж�-----------------------------------------------------------
		if((M_ChkFlag(SL_SIOVST)==0)&&((PRO.STA_iac>_SC_IACOVST)||(PRO.STA_icb>_SC_IACOVST)))
		{
			M_SetFlag(SL_SIOVST);									//�ö��ӵ���������־λ
		}
		else if((M_ChkFlag(SL_SIOVST)!=0)&&(PRO.STA_iac<(_SC_IACOVST-SC_IAC_HW))&&(PRO.STA_icb<(_SC_IACOVST-SC_IAC_HW))) 
		{
			M_ClrFlag(SL_SIOVST);									//�嶨�ӵ���������־λ
		}
/*
//-----------------------�м��ѹ���ȶ�---��ʱ����201005atcpc----------------------------------------------------------
        if(M_ChkFlag(SL_NPR_PWMOUT)!=0 && M_ChkFlag(SL_MPR_PWMOUT)!=0)
		{
		   if((AD_OUT_UDC<(_URF - 70))||(AD_OUT_UDC>(_URF + 70)))  //�м��ѹ������Χ���ù��ϱ�־λ new
		   {
		       if(M_ChkCounter(MAIN_LOOP.cnt_steadyfb,DELAY_UDCWAVE)>0)
			   {                                   //DELAY_STEADYFB��cnt_steadyfb��sysctrl�������ͬһ��
			   	   M_SetFlag(SL_UDCWAVE);  
			   }	   
		   }
		   else
	   	   {
	      		   MAIN_LOOP.cnt_steadyfb=0;
		  		   M_ClrFlag(SL_UDCWAVE);
	   	   }
		}
		else   M_ClrFlag(SL_UDCWAVE);
*/
//-----------------------SCR �ϲ�������LVRT---201007BJTULVRT----------------------------------------------------------
 //       if((M_ChkFlag(SL_SCRON)!=0)&&(M_ChkFlag(SL_LV_SCRIZERO)==0))
        if(M_ChkFlag(SL_SCRIZERO)==0)	//20110829HC
		{
		    if(M_ChkCounter(MAIN_LOOP.cnt_scrondelay,DELAY_SCRONDELAY)>0)
			{                                   
				M_SetFlag(SL_UDCWAVE);  
			}	   
		    else	M_ClrFlag(SL_UDCWAVE);
		}
		else   
		{
		 	M_ClrFlag(SL_UDCWAVE);
			MAIN_LOOP.cnt_scrondelay=0;
		}

//------------------------����ӵع���-���������ƽ��-----20100302---------------------------------------------------------
		if(M_ChkFlag(SL_IUBLNCE1)!=0)
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_gfault1,DELAY_GFAULT)>=0)	 M_SetFlag(SL_GFAULT1);	
		}	
		else
		{
			M_ClrFlag(SL_GFAULT1);									  
			MAIN_LOOP.cnt_gfault1=0;                               
		}		

//------------------------����ӵع���-���������ƽ��-----20100302---------------------------------------------------------
		if(M_ChkFlag(SL_IUBLNCE2)!=0)
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_gfault2,DELAY_GFAULT)>=0)	 M_SetFlag(SL_GFAULT2);	
		}	
		else
		{
			M_ClrFlag(SL_GFAULT2);									  
			MAIN_LOOP.cnt_gfault2=0;                               
		}		

//-----------------------����¶ȷ���SKIIP���¹����ж�----------------------------------------------
		if(AMUX.NPR_tempa > AMUX.NPR_tempb)	
		{	if(AMUX.NPR_tempa > AMUX.NPR_tempc)	    AMUX.NPR_skiiptemp = AMUX.NPR_tempa;
			else									AMUX.NPR_skiiptemp = AMUX.NPR_tempc;
		}	
		else
		{   if(AMUX.NPR_tempb > AMUX.NPR_tempc)	    AMUX.NPR_skiiptemp = AMUX.NPR_tempb;
			else									AMUX.NPR_skiiptemp = AMUX.NPR_tempc;
		}
		if(AMUX.MPR_tempa > AMUX.MPR_tempb)
		{	if(AMUX.MPR_tempa > AMUX.MPR_tempc)	    AMUX.MPR_skiiptemp = AMUX.MPR_tempa;
			else								    AMUX.MPR_skiiptemp = AMUX.MPR_tempc;
		}
		else
		{	if(AMUX.MPR_tempb > AMUX.MPR_tempc)	    AMUX.MPR_skiiptemp = AMUX.MPR_tempb;
			else									AMUX.MPR_skiiptemp = AMUX.MPR_tempc;
		}   
   	 	if(AMUX.NPR_skiiptemp > AMUX.MPR_skiiptemp)	AMUX.skiiptempmax = AMUX.NPR_skiiptemp;    
		else                          				AMUX.skiiptempmax = AMUX.MPR_skiiptemp; 

		if((M_ChkFlag(SL_SKTOV)==0)&&(AMUX.skiiptempmax > _SC_SKPTOV))//201005atcpc
		{
			M_SetFlag(SL_SKTOV);									//��SKIIP���¹��ϱ�־λ
		}
		else if((M_ChkFlag(SL_SKTOV)!=0)&&(AMUX.skiiptempmax < (_SC_SKPTOV-10))) 
		{
			M_ClrFlag(SL_SKTOV);									//��SKIIP���¹��ϱ�־λ
		}
        
//----------------------����SKIIP���¹���-----201005atcpc-----------------------------------------------------			
		if(M_ChkFlag(SL_IN2_TAOV)!=0)
		{	
			if(M_ChkCounter(MAIN_LOOP.cnt_nskiptov,DELAY_NSKIPTOV)>=0)	M_SetFlag(SL_TAOV); 	//ó�±�־λ
		}
       	else
		{
			MAIN_LOOP.cnt_nskiptov=0; 								//����ʱ����
			M_ClrFlag(SL_TAOV);										//�峬�±�־λ
		}

//--------------------�����SKIIP���¹���-----201005atcpc------------------------------------------------------	
		if(M_ChkFlag(SL_IN2_TBOV)!=0)
		{	
			if(M_ChkCounter(MAIN_LOOP.cnt_mskiptov,DELAY_MSKIPTOV)>=0)	M_SetFlag(SL_TBOV); 	//�ó��±�־λ
		}
       	else
		{
			MAIN_LOOP.cnt_mskiptov=0; 								//����ʱ����
			M_ClrFlag(SL_TBOV);										//�峬�±�־λ
		}

//----------------------�����г��¹���----------------------------------------------------------			
		if((M_ChkFlag(SL_NPR_TLOV)==0)&&(PRO.NPR_TLOV>_SC_NPR_TLOV))
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_nprtlov,DELAY_NPRTLOV)>=0)	M_SetFlag(SL_NPR_TLOV);	//�������г��±�־λ
		}
		else if((M_ChkFlag(SL_NPR_TLOV)!=0)&&(PRO.NPR_TLOV<(_SC_NPR_TLOV - 5))) 
		{
			M_ClrFlag(SL_NPR_TLOV);									//������г��±�־�?
			MAIN_LOOP.cnt_nprtlov=0;
		} 


//--------------------������г��¹���-----------------------------------------------------------	
		if((M_ChkFlag(SL_MPR_TLOV)==0)&&(PRO.MPR_TLOV>_SC_MPR_TLOV))
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_mprtlov,DELAY_MPRTLOV)>=0)	M_SetFlag(SL_MPR_TLOV);	//�û����г��±�־λ
		}
		else if((M_ChkFlag(SL_MPR_TLOV)!=0)&&(PRO.MPR_TLOV<(_SC_MPR_TLOV - 5))) 
		{
			M_ClrFlag(SL_MPR_TLOV);									//������г��±�־λ
			MAIN_LOOP.cnt_mprtlov=0;
		} 


//-----------------------�������й����ʹ����ж�-----------------------------------------------------------
/*		temp_pgactive = abs(PRO.Pgactive);							//20090816
		temp_pgactive = temp_pgactive * 0.001;
		if((M_ChkFlag(SL_PGOV)==0)&&(temp_pgactive>_SC_PGOV))
		{
			M_SetFlag(SL_PGOV_COM);
			if(M_ChkCounter(MAIN_LOOP.cnt_pgovload,DELAY_PGOVLOAD)>0)  	//10s
				M_SetFlag(SL_PGOV);										//�ñ������й���ʹ��ر�־�?
		}
		else if((M_ChkFlag(SL_PGOV)!=0)&&(temp_pgactive<(_SC_PGOV-SC_POWOROV_HW))) 
		{
			M_ClrFlag(SL_PGOV);
			M_ClrFlag(SL_PGOV_COM);										//��������й����ʹ��ر�־λ
			MAIN_LOOP.cnt_pgovload=0;
		}
		else  MAIN_LOOP.cnt_pgovload=0;

//-----------------------�������޹����ʹ����ж�-----------------------------------------------------------
        temp_pgreactive = abs(PRO.Pgreactive);							//20090816
        temp_pgreactive = temp_pgreactive * 0.001;
		if((M_ChkFlag(SL_QGOV)==0)&&(temp_pgreactive>_SC_QGOV))
		{
			M_SetFlag(SL_QGOV_COM);										//20091007
			if(M_ChkCounter(MAIN_LOOP.cnt_qgovload,DELAY_QGOVLOAD)>0)  	//10s
				M_SetFlag(SL_QGOV);										//�ñ������޹����ʹ��ر�־λ
		}
		else if((M_ChkFlag(SL_QGOV)!=0)&&(temp_pgreactive<(_SC_QGOV-SC_POWOROV_HW))) 
		{
			M_ClrFlag(SL_QGOV);	
			M_ClrFlag(SL_QGOV_COM);										//��������޹����ʹ��ر�־λ
			MAIN_LOOP.cnt_qgovload=0;
		} 
		else MAIN_LOOP.cnt_qgovload=0;
*/
//-------���ӵʹ���ʱ����---------���޹����ع���λ--------2016-2-17-------------
	if(M_ChkFlag(SL_LV_STATE)!=0)
	{
		if (M_ChkCounter(MAIN_LOOP.cnt_hvlv_detect3,DELAY_LVTEST3)>=0)
		M_SetFlag(SL_QGOV);			
	}
	else if(M_ChkFlag(SL_HV_STATE)!=0)
	{
		if (M_ChkCounter(MAIN_LOOP.cnt_hvlv_detect3,12)>=0)
		M_SetFlag(SL_QGOV);			
	}
	else
	{
		MAIN_LOOP.cnt_hvlv_detect3=0;
		M_ClrFlag(SL_QGOV);
	}

//-----------------------����ͬ������ʧ�ܹ���---------------------------------------
/*	    if(M_ChkFlag(SL_OCS_SYSRUN)!=0 && M_ChkFlag(SL_MPR_SYNOK)==0 && M_ChkFlag(SL_ENPCOPER)==0 )	//PC���Բ����иù����ж�							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_synfail,120000)>=0)	M_SetFlag(SL_SYNFAIL);	  //2min  201005atcpc
			else M_ClrFlag(SL_SYNFAIL);								 	//���־λ
		}
		else 
		{
			MAIN_LOOP.cnt_synfail=0;                                    //�嶨ʱ��
			M_ClrFlag(SL_SYNFAIL);
		} 
*/
//--------20130801--------
	    if(M_ChkFlag(SL_OCS_SYSRUN)!=0 && M_ChkFlag(SL_MPR_SYNOK)==0 && M_ChkFlag(SL_ENPCOPER)==0 )	//PC���Բ����иù����ж�							
//		if(M_ChkFlag(SL_MPR_START)!=0 && M_ChkFlag(SL_MPR_SYNOK)==0 && M_ChkFlag(SL_ENPCOPER)==0 )	//PC���Բ����иù����ж�							
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_synfail,60000)>=0)
			{
				M_SetFlag(SL_SYNFAIL);	  //2min  201005atcpc
				ecpostep1 = 0;
				ecpostep2 = 0;
				ecpostep3 = 0;
				MAIN_LOOP.cnt_synfail = 0;

			}
			else M_ClrFlag(SL_SYNFAIL);								 	//���־λ
		}
		else 
		{
//			if(M_ChkFlag(SL_ENCODEPOS_OK)!=0 && M_ChkFlag(SL_ECPOSSEARCH)==0)	MAIN_LOOP.cnt_synfail=0;		//�嶨ʱ��
			if(M_ChkFlag(SL_ECPOSSEARCH)==0)	MAIN_LOOP.cnt_synfail=0;		//�嶨ʱ��
			M_ClrFlag(SL_SYNFAIL);
		}

//--------------------------------�õ�TAB_MSG�еĹ����?-systest------------------------------------------
		_MSG_SCOUT2 = MSG_NONE;												//�Ƚ�MSG����
		
		if(M_ChkFlag(SL_CODEOK)==0)				_MSG_SCOUT2=MSG_CODEOK;     //1=������δУ�����
		
		else if(M_ChkFlag(SL_EE_FAIL)!=0) 		_MSG_SCOUT2=MSG_EE_FAIL;    //2=EEPROM����

		else if(M_ChkFlag(SL_ESTOP)!=0) 	    _MSG_SCOUT2=MSG_ESTOP;      //3=����ͣ������

		else if(M_ChkFlag(SL_CBTRIP)!=0) 	    _MSG_SCOUT2=MSG_CBTRIP;     //4=�����ѿ۹���

		else if(M_ChkFlag(SL_EXFAIL)!=0) 	 	_MSG_SCOUT2=MSG_EXFAULT;    //5=�������ⲿӲ������

		else if(M_ChkFlag(SL_CANOPENOVER)!=0) 	_MSG_SCOUT2=MSG_CAN_FAIL;   //6=CANͨѶ����//controltest

		else if(M_ChkFlag(SL_ENCODFAULT)!=0) 	_MSG_SCOUT2=MSG_ENCODFAULT; //7=����������//controltest

		else if(M_ChkFlag(SL_ERROR_CB)!=0) 	 	_MSG_SCOUT2=MSG_CBERROR;    //8=���Ϻ�բ����

		else if(M_ChkFlag(SL_ERROR_PRE)!=0) 	_MSG_SCOUT2=MSG_PREERROR;   //9=Ԥ������

		else if(M_ChkFlag(SL_ERROR_MAINK)!=0) 	_MSG_SCOUT2=MSG_MAINKERROR; //10=���Ӵ����պϹ���

		else if(M_ChkFlag(SL_ERROR_MAINF)!=0) 	_MSG_SCOUT2=MSG_MAINFERROR; //11=���˲����պϹ���

		else if(M_ChkFlag(SL_ERROR_STAC)!=0) 	_MSG_SCOUT2=MSG_STACERROR;  //12=����Ӵ����պϹ��?

		else if(M_ChkFlag(SL_HIA1)!=0)			_MSG_SCOUT2=MSG_HIA1;     	//13=���������A��SKIIP����

		else if(M_ChkFlag(SL_HIB1)!=0)			_MSG_SCOUT2=MSG_HIB1;     	//14=���������B��SKIIP����

		else if(M_ChkFlag(SL_HIC1)!=0)			_MSG_SCOUT2=MSG_HIC1;     	//15=���������C��SKIIP����
		
		else if(M_ChkFlag(SL_HIA2)!=0)			_MSG_SCOUT2=MSG_HIA2;     	//16=����������A��SKIIP��?

		else if(M_ChkFlag(SL_HIB2)!=0)			_MSG_SCOUT2=MSG_HIB2;     	//17=����������B��SKIIP����

		else if(M_ChkFlag(SL_HIC2)!=0)			_MSG_SCOUT2=MSG_HIC2;     	//18=����������C��SKIIP����

		else if(M_ChkFlag(SL_HUDCOV)!=0)		_MSG_SCOUT2=MSG_UDCOV;    	//19=������ֱ��ĸ��Ӳ����ѹ����

        else if(M_ChkFlag(SL_HIACOV1)!=0)		_MSG_SCOUT2=MSG_HIAC1;    	//20=����Ӳ����������

		else if(M_ChkFlag(SL_HIACOV2)!=0)		_MSG_SCOUT2=MSG_HIAC2;    	//21=�����Ӳ����������

        else if(M_ChkFlag(SL_PDPASERIES)!=0)	_MSG_SCOUT2=MSG_PDPASERIES;	//22=�������ع���

		else if(M_ChkFlag(SL_PDPBSERIES)!=0)	_MSG_SCOUT2=MSG_PDPBSERIES;	//23=�������ع���

		else if(M_ChkFlag(SL_MSPOUT)!=0) 	    _MSG_SCOUT2=MSG_MSPEEDOUT;  //24=ת�ٳ�����Χ����

		else if(M_ChkFlag(SL_SIAC1)!=0)			_MSG_SCOUT2=MSG_SIAC1;    	//25=���������������

		else if(M_ChkFlag(SL_SIAC2)!=0)			_MSG_SCOUT2=MSG_SIAC2;    	//26=����������������

		else if(M_ChkFlag(SL_FE1)!=0)			_MSG_SCOUT2=MSG_FE1;      	//27=����Ƶ�ʲ�������//controltest

		else if(M_ChkFlag(SL_SUDCOV)!=0)		_MSG_SCOUT2=MSG_SUDCOV;   	//28=����м�ֱ����ѹ��ѹ
		
		else if(M_ChkFlag(SL_UACOV1)!=0)		_MSG_SCOUT2=MSG_SUACOV1;  	//29=�����ѹ������ѹ

//		else if(M_ChkFlag(SL_SUDCLV)!=0)		_MSG_SCOUT2=MSG_SUDCLV;   	//30=����м�����ѹǷ�?
		else if((M_ChkFlag(SL_IDCOV)!=0)||(M_ChkFlag(SL_HIDCOV)!=0))	_MSG_SCOUT2=MSG_SIDCOV;   	//30=��Ϊdc-chopper��������BJTULVRT201204

//		else if(M_ChkFlag(SL_UACLV1)!=0 || M_ChkFlag(SL_UACLV2)!=0)		_MSG_SCOUT2=MSG_SUACLV1;  	//31=�����ѹ����Ƿѹ 201007BJTULVRT				
		else if(M_ChkFlag(SL_UACLV1)!=0)		_MSG_SCOUT2=MSG_SUACLV1;  	//31=����ѹ����Ƿ�?
		
		else if(M_ChkFlag(SL_UDCWAVE)!=0)		_MSG_SCOUT2=MSG_UDCWAVE;  	//32=�м��ѹ��������

        else if(M_ChkFlag(SL_SIOVST)!=0)		_MSG_SCOUT2=MSG_SIOVST;   	//33=�����ⶨ�ӹ�������
		
		else if(M_ChkFlag(SL_GFAULT1)!=0)		_MSG_SCOUT2=MSG_GFAULT1;   	//34=����ӵع���
		
		else if(M_ChkFlag(SL_GFAULT2)!=0)		_MSG_SCOUT2=MSG_GFAULT2;  	//35=����ӵع���

		else if(M_ChkFlag(SL_TAOV)!=0)			_MSG_SCOUT2=MSG_TOV1;	  	//36=����SKIIP����//controltest
		
		else if(M_ChkFlag(SL_TBOV)!=0)			_MSG_SCOUT2=MSG_TOV2;  	  	//37=�����SKIIP����//controltest

		else if(M_ChkFlag(SL_SKTOV)!=0)			_MSG_SCOUT2=MSG_SKTOV;     	//38=����ж�SKIIP����

        else if(M_ChkFlag(SL_NPR_TLOV)!=0)		_MSG_SCOUT2=MSG_TLOV1;     	//39=�������г���

		else if(M_ChkFlag(SL_MPR_TLOV)!=0)		_MSG_SCOUT2=MSG_TLOV2;     	//40=������г���

//		else if(M_ChkFlag(SL_PGOV)!=0)		    _MSG_SCOUT2=MSG_PGOV;      	//41=�������й����ʹ���

		else if(M_ChkFlag(SL_QGOV)!=0)	    	_MSG_SCOUT2=MSG_QGOV;      	//42=�������޹����ʹ���

		else if(M_ChkFlag(SL_SYNFAIL)!=0)	    _MSG_SCOUT2=MSG_STASYNFAIL; //43=����ͬ������ʧ�ܹ���

		else if(M_ChkFlag(SL_PHORDE)!=0) 		_MSG_SCOUT2=MSG_PHORDE;     //44=����������� //controltest

//------------------------PCʾ�������ϲ��δ洢����--20100308-------------------------------------------------------

//		if(_MSG_SCOUT2!=MSG_NONE)												//�����й��Ϸ���
		if((_MSG_SCOUT2!=MSG_NONE)&&(_MSG_SCOUT2!=6))		//�����й��Ϸ��� ���洢canopen����20121129
		{
			if(_PCEROTRIG==0)					M_SetFlag(SL_TRIG_ERRDSAVE);	//���й��ϴ������� 201005atcpc
			else if(_MSG_SCOUT2==_PCEROTRIG)	M_SetFlag(SL_TRIG_ERRDSAVE);	//ĳ��ϴ������?
			else if((_PCEROTRIG==50)&&(_MSG_SCOUT2==14||_MSG_SCOUT2==15||_MSG_SCOUT2==16||_MSG_SCOUT2==21||_MSG_SCOUT2==23))
				M_SetFlag(SL_TRIG_ERRDSAVE);	 								//��������Ӳ�����ϴ�������
			else if((_PCEROTRIG==51)&&(_MSG_SCOUT2==17||_MSG_SCOUT2==18||_MSG_SCOUT2==19||_MSG_SCOUT2==22||_MSG_SCOUT2==24))
				M_SetFlag(SL_TRIG_ERRDSAVE);	 								//���л���Ӳ�����ϴ�������
		}
		
		if((_PCEROTRIG==99)&&(M_ChkFlag(SL_PC_ERRDATASAVE)!=0))
				M_SetFlag(SL_TRIG_ERRDSAVE);	 								//PC�����ⲿRAM�������� 201005atcpc
		if((_PCEROTRIG==0)&&(M_ChkFlag(CL_CHOPTEST)!=0))	M_SetFlag(SL_TRIG_ERRDSAVE);	//ն������2013-12-3

//------------------------���ϴ���--systest-------------------------------------------------------

//NO1-------ԭ��û�й��ϱ����й���(��)ԭ���й��ϵ����μ������-------------------������������------
		if(((_MSG_SCOUT2!=MSG_NONE)&&(M_ChkFlag(SL_ERROR)==0))||((_MSG_SCOUT2!=MSG_NONE)&&(M_ChkFlag(SL_ERROR)!=0)&&(TAB_MSG[_MSG_SCOUT2].rank > TAB_MSG[_MSG_SCOUT1].rank)))
		{													
			M_SetFlag(SL_ERROR);							//�ù��ϱ�־λ
			M_SetFlag(SL_DISPLAY5);                         //��ϵͳ����ָʾ

//----------------------------¹������Ա��?-------------------------------------------------------												
			if((TAB_MSG[_MSG_SCOUT2].attr & OFFCB)==OFFCB)		M_SetFlag(SL_OFFCB);	//���ؼ�ͣ����
			else												M_ClrFlag(SL_OFFCB);

			if((TAB_MSG[_MSG_SCOUT2].attr & SHUT)==SHUT)		M_SetFlag(SL_SHUT);		//һ��ͣ������
			else												M_ClrFlag(SL_SHUT);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & N_RCVR)==N_RCVR)	M_SetFlag(SL_NRCVR);	//���ָܻ�����
			else												M_ClrFlag(SL_NRCVR);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & I_RCVR)==I_RCVR)	M_SetFlag(SL_IRCVR);	//�����ָ�����
			else												M_ClrFlag(SL_IRCVR);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & D_RCVR)==D_RCVR)	M_SetFlag(SL_DRCVR);	//��ʱ�ָ�����
			else												M_ClrFlag(SL_DRCVR);
						
			if((TAB_MSG[_MSG_SCOUT2].attr & CNT)==CNT)			M_SetFlag(SL_CNT);		//�ƴ�������
			else												M_ClrFlag(SL_CNT);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & OT_SER)==OT_SER)	M_SetFlag(SL_OTSER);	//��ʱ��������
			else												M_ClrFlag(SL_OTSER);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & SAVE)==SAVE)		M_SetFlag(SL_SAVE);		//��Ҫ���������Ϣ����
			else												M_ClrFlag(SL_SAVE);		

			if((TAB_MSG[_MSG_SCOUT2].attr & WARNING)==WARNING)	M_SetFlag(SL_WARNING);	//��������
			else												M_ClrFlag(SL_WARNING);	

//----------------------------------���ݹ��������ж�ϵͳ����---------------------------------------
						
			if(M_ChkFlag(SL_NRCVR)!=0)		//���ɸ�λ����
			{
				M_SetFlag(SL_NO_RESET);		//���ɸ�λ����,�� ��ֹ��λ ��־
				M_ClrFlag(SL_NRCVR);		//�岻�ɸ�λ����
			}

			if(M_ChkFlag(SL_DRCVR)!=0)		//��ʱ��λ����
			{
				M_SetFlag(SL_DL_RESET);		//������ ��ֹ��λ ��־
			}											

/*			if(M_ChkFlag(SL_IRCVR)!=0)		//����������λ����,û�в����ȴ���λ
			{
//				M_ClrFlag(SL_NO_RESET);		//�� ��ֹ��λ ��־
				M_ClrFlag(SL_IRCVR);							
			}											
*/
            if(M_ChkFlag(SL_OFFCB)!=0)		//��ع��?
			{
				M_SetFlag(SL_SERIESTOP);	//�� ���ع���ͣ�� ��־											
				M_ClrFlag(SL_OFFCB);
			}

            if(M_ChkFlag(SL_SHUT)!=0)		//һ�����
			{
				M_SetFlag(SL_ERRSTOP);		//�� һ�����ͣ�� ��־												
				M_ClrFlag(SL_SHUT);
			}

    		if(M_ChkFlag(SL_CNT)!=0)
			{
				if(R_SCOUT == _MSG_SCOUT2)	_SY_RTRN++;				//�ظ�����ͬһ����
				else
				{
					R_SCOUT = _MSG_SCOUT2;
					_SY_RTRN=0;
				}
				if(_SY_RTRN>=_SC_RTRN)		M_SetFlag(SL_NO_RESET);	//�� ��ֹ��λ ��־	
				M_ClrFlag(SL_CNT);				
			}

        	if(M_ChkFlag(SL_WARNING)!=0)   //��ʱû�н��кβ���,��ͨ��ͨѶ��������
			{
				M_ClrFlag(SL_WARNING);
			} 

			MAIN_LOOP.cnt_rcvr=0;			//����ʱ�ָ�������	
			_MSG_SCOUT1 = _MSG_SCOUT2;		//���ι�����Ϣת��
								
//----------------------------���¹�ϼ�¼������λ�������?------------------------------------------						
			if((M_ChkFlag(SL_SAVE)!=0)&&(M_ChkFlag(SL_EEBUSY_ERRSAVE)==0))			
			{
				MAIN_LOOP.cnt_rcvr=0;									//���ʱ�ָ����?	

				M_SetFlag(SL_EEASK_ERRSAVE);							//EEPROM������?
				M_ClrFlag(SL_SAVE);
				_BA_ERR1 = _BA_ERR2;									//������Ϣ����
				_BA_ERR2 = _BA_ERR3;
				_BA_ERR3 = _BA_ERR4;
				_BA_ERR4 = _MSG_SCOUT2;
       			
				_BA_EURF   = (int16)RUN.urf;							//�м�ֱ����ѹ����
				_BA_EUDC   = (int16)AD_OUT_UDC;							//�м�ֱ����ѹ
				_BA_EMIDRF = (int16)(RUN.mpridrf * 10);					//d���������
				_BA_ENIQRF = (int16)(RUN.npriqrf * 10);					//q���������
				_BA_EMIQRF = (int16)(RUN.mpriqrf * 10);					//q���������
				_BA_ETOQRF = (int16)RUN.toqrf;							//ת�ص�������
                _BA_EAGLRF = (int16)RUN.aglrf;            			 //�޹��Ƕ�ָ��

				_BA_EIA1  = (int16)(AD_OUT_NPR_I.a * 10);				//���������,A�����˲ʱֵ
				_BA_EIB1  = (int16)(AD_OUT_NPR_I.b * 10);				//���������,B�����˲ʱֵ
				_BA_EIC1  = (int16)(AD_OUT_NPR_I.c * 10);				//���������,C�����˲ʱ�?
				_BA_EIA2  = (int16)(AD_OUT_MPR_I.a * 10);				//����������,A�����˲ʱֵ
				_BA_EIB2  = (int16)(AD_OUT_MPR_I.b * 10);	            //��������?,B�����˲ʱֵ
				_BA_EIC2  = (int16)(AD_OUT_MPR_I.c * 10);	            //����������,C�����˲ʱֵ
				
				_BA_EUAB1  = (int16)PRO.NPR_uab;						//����ab�ߵ�ѹ
				_BA_EUBC1  = (int16)PRO.NPR_ubc;						//����bc�ߵ�ѹ
				_BA_EUAB2  = (int16)PRO.STA_uab;						//���Ӳ�ab�ߵ�ѹ
				_BA_EUBC2  = (int16)PRO.STA_ubc;						//���Ӳ�bc�ߵ�ѹ

//				_BA_EUAB0    = (int16)DISP.grd_uab;			       		//����ǰ����ab��ѹ
//            	_BA_EUBC0    = (int16)DISP.grd_ubc;			       		//����ǰ����bc��ѹ
				_BA_EUAB0    = (int16)(AD_OUT_DC_I.b);			       	//����ǰ����ab��ѹBJTULVRT201204
            	_BA_EUBC0    = (int16)GRD_Ut;			       			//����ǰ����bc��ѹ

				_BA_ETLAC  = (int16)PRO.NPR_TLOV;                           //�������¶�
//				_BA_ETLAC  = (int16)SCI_canopen.cnt_heartbeat;             //�������¶? 2014-05-06LJD��ʱ�޸�CANopen
				
				_BA_ETLDUDT= (int16)PRO.MPR_TLOV;                           //�������¶�
				_BA_ETSKIIP= (int16)AMUX.skiiptempmax;                      //SKIIP�¶�	

				_BA_EFREQ  = (int16)(CAP4.freq * 10);						//����Ƶ��
				_BA_ESPEED = (int16)(PRO.speed);							//���ת�� 

	            _BA_ENPRUD = (int16)TRS_NGS_U.d;              			//d�����Ϻ���ѹ����20091026atzy
				_BA_ENPRUQ = (int16)TRS_NGS_U.q;          				//q�����Ϻ���ѹ����20091026atzy
	            _BA_ENPRUD2 = (int16)TRS_NGS_U.dflt;          			//d�����Ϻ���ѹ���� �˲���20091026atzy
				_BA_ENPRUQ2 = (int16)TRS_NGS_U.qflt;          			//q�����Ϻ���ѹ���� ˲��?20091026atzy
	            _BA_ENUDOUT = (int16)TRS_NPR_U.d;          				//����d���ѹ���20091026atzy
				_BA_ENUQOUT = (int16)TRS_NPR_U.q;          				//����q���ѹ���20091026atzy

				_BA_EPIONU  = (int16)(PI_NPR_U.out * 10);					//���A
				_BA_EPIONID = (int16)(PI_NPR_Id.out* 10);					//��λV
				_BA_EPIONIQ	= (int16)(PI_NPR_Iq.out* 10);					//��λV

				_BA_EMEXI   = (int16)(RUN_mpridrf_exi * 10);				//��λA  �����������ۼ���ֵ��ʾ cpc
				_BA_EPIOMID = (int16)(PI_MPR_Id.out* 10);					//���V
				_BA_EPIOMIQ	= (int16)(PI_MPR_Iq.out* 10);					//��λV

//				_BA_ESTAIAC = (int16)(DISP.sta_iac * 10);
//				_BA_ESTAIBA = (int16)(DISP.sta_iba * 10);
//				_BA_ETOQFB  = (int16)DISP.toqfb;				        	//ת�ط���
//				_BA_EPSTA   = (int16)(DISP.Psactive * 0.01);          		//���Ӳ��й�������ʾkW,DOT1 *10/1000=100
//				_BA_EPNPR   = (int16)(DISP.Pnactive * 0.01);         		//�����й�������ʾ
//				_BA_EPGRID  = (int16)(DISP.Pgactive * 0.01);          		//�����ܵ��й�������ʾ zuoyuntest
				_BA_ESTAIAC = (int16)(PRO.STA_iac * 10);				//BJTULVRT201204
				_BA_ESTAIBA = (int16)(PRO.STA_icb * 10);				//BJTULVRT201204
				_BA_ETOQFB  = (int16)Te_feedback;				        	//ת�ط���//BJTULVRT201204
				_BA_EPSTA   = (int16)(PRO.Psactive * 0.01);          		//���Ӳ��й�������ʾkW,DOT1 *10/1000=100
				_BA_EPNPR   = (int16)(PRO.Pnactive * 0.01);         		//�����й�������ʾBJTULVRT201204
				_BA_EPGRID  = (int16)(PRO.Pgactive * 0.01);          		//�����ܵ��й�������ʾ zuoyuntest

				_BA_TIME1_0=_BA_TIME2_0;									//����ʱ��1
				_BA_TIME1_1=_BA_TIME2_1;
				_BA_TIME1_2=_BA_TIME2_2;
				_BA_TIME1_3=_BA_TIME2_3;
				_BA_TIME1_4=_BA_TIME2_4;
				_BA_TIME1_5=_BA_TIME2_5;
				
				_BA_TIME2_0=_BA_TIME3_0;									//����ʱ��2
				_BA_TIME2_1=_BA_TIME3_1;
				_BA_TIME2_2=_BA_TIME3_2;
				_BA_TIME2_3=_BA_TIME3_3;
				_BA_TIME2_4=_BA_TIME3_4;
				_BA_TIME2_5=_BA_TIME3_5;
					
				_BA_TIME3_0=_BA_TIME4_0;									//����ʱ��3
				_BA_TIME3_1=_BA_TIME4_1;
				_BA_TIME3_2=_BA_TIME4_2;
				_BA_TIME3_3=_BA_TIME4_3;
				_BA_TIME3_4=_BA_TIME4_4;
				_BA_TIME3_5=_BA_TIME4_5;
					
				_BA_TIME4_0=RTIMER.time[0];									//����ʱ��4
				_BA_TIME4_1=RTIMER.time[1];                         		//���¹���ʱ��
				_BA_TIME4_2=RTIMER.time[2];
				_BA_TIME4_3=RTIMER.time[3];
				_BA_TIME4_4=RTIMER.time[4];
				_BA_TIME4_5=RTIMER.time[5];
			}
		}

//NO2------ԭ�й�������Ҳ�У��Ҽ�����ͬ���ߵ���֮ǰ����Ĺ���------������ƽ�����㡢�������㣩----
		else if((_MSG_SCOUT2!=MSG_NONE)&&(M_ChkFlag(SL_ERROR)!=0)&&((TAB_MSG[_MSG_SCOUT2].rank <= TAB_MSG[_MSG_SCOUT1].rank)))					
		{
			MAIN_LOOP.cnt_rcvr=0;
			_MSG_SCOUT1 = _MSG_SCOUT2;									//���ι�����Ϣת��

			if((TAB_MSG[_MSG_SCOUT2].attr & D_RCVR)==D_RCVR)	M_SetFlag(SL_DRCVR);	//�߼�������D_RCVR���ͼ�������D_RCVR��ǰ�����ȷ���ʱ��Ҫ
			else												M_ClrFlag(SL_DRCVR);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & OT_SER)==OT_SER)	M_SetFlag(SL_OTSER);	//��ʱ��������,�߼�������D_RCVR���ͼ�������D_RCVR��ǰ�����ȷ���ʱ��Ҫ
			else												M_ClrFlag(SL_OTSER);

			if(M_ChkFlag(SL_OTSER)!=0)
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_otser,DELAY_OTSER)>=0)	//��ʱʱ�䵽
				{
					M_SetFlag(SL_NO_RESET);								//�� ��ֹ��λ ��־	
					M_ClrFlag(SL_OTSER);								//�峬ʱ���ر�־
				}	
			}
			else	MAIN_LOOP.cnt_otser=0;		
			
			if(M_ChkFlag(SL_DRCVR)!=0)									//��ʱ��λ����
			{
				M_SetFlag(SL_DL_RESET);									//������ ��ֹ��λ ��־
			}											
		
		}

//NO3---------ԭ���й��ϣ�����û�й���----------------------------------------�����Ͻ���Ϊ�㣩------			
		else if((_MSG_SCOUT2==MSG_NONE)&&(M_ChkFlag(SL_ERROR)!=0))					
		{				
			MAIN_LOOP.cnt_otser=0;  								//�峬ʱ������ʱ��
			if(M_ChkFlag(SL_IRCVR)!=0)								//���������ָ�����
			{
				M_ClrFlag(SL_IRCVR);								//�������ָ���־
				M_ClrFlag(SL_ERROR);								//ɾ�������ź�
				M_ClrFlag(SL_ERRSTOP);
				M_ClrFlag(SL_SERIESTOP);							//20110829
//-------20130314--------
					M_ClrFlag(SL_OCS_NPRSTART);	          	//����������	 
					M_ClrFlag(SL_OCS_MPRSTART);				//����������								
					M_ClrFlag(SL_QEPPCO);

					M_ClrFlag(SL_EE_FAIL);					//���EEROM���ϱ�־

					M_ClrFlag(SL_PDPINTA);					//�������TZ1_PDP���ϱ�־
					M_ClrFlag(SL_PDPINTB);					//�������TZ2_PDP���ϱ�־
					M_ClrFlag(SL_DISPLAY3);             	//��PDP����ָʾ��
					ClrPdpint();							//PDPINT�ж����	
					EnPdpint();								//PDPINT�ж�ʹ��

					MAIN_LOOP.cnt_rcvr=0;                   //������ʱ�ָ���ʱ
					MAIN_LOOP.cnt_otser=0;                  //��ʱ������ʱ	
//----------------------
			}
			
//			if(M_ChkFlag(SL_DL_RESET)!=0)							//������ʱ�ָ��ұ���û�й���
			if((M_ChkFlag(SL_DL_RESET)!=0)&&(M_ChkFlag(SL_NO_RESET)==0))		//������ʱ�ָ��ұ���û�й���20121023
			{
				PRO.rcvr = _SC_RTRT * 1000;							//��λ�任:s->ms
				if(M_ChkCounter(MAIN_LOOP.cnt_rcvr,PRO.rcvr)>=0)	//��ʱʱ�䵽
				{
					M_ClrFlag(SL_DRCVR);							//����ʱ�ָ���־
					M_ClrFlag(SL_DL_RESET);							//�� ��ֹ��λ ��?
					M_ClrFlag(SL_ERROR);							//ɾ�������ź�20120310
					M_ClrFlag(SL_ERRSTOP);							//20120310
					M_ClrFlag(SL_SERIESTOP);						//20120310
//-------20121206--------
					M_ClrFlag(SL_OCS_NPRSTART);	          	//����������	 
					M_ClrFlag(SL_OCS_MPRSTART);				//����������								
					M_ClrFlag(SL_QEPPCO);

					M_ClrFlag(SL_EE_FAIL);					//���EEROM���ϱ�־

					M_ClrFlag(SL_PDPINTA);					//�������TZ1_PDP���ϱ�־
					M_ClrFlag(SL_PDPINTB);					//�������TZ2_PDP���ϱ�־
					M_ClrFlag(SL_DISPLAY3);             	//��PDP����ָʾ��
					ClrPdpint();							//PDPINT�ж����	
					EnPdpint();								//PDPINT�ж�ʹ��

					MAIN_LOOP.cnt_rcvr=0;                   //������ʱ�ָ���ʱ
					MAIN_LOOP.cnt_otser=0;                  //��ʱ������ʱ	
//----------------------
				}
			}
			else   MAIN_LOOP.cnt_rcvr=0;						
		}
//NO4---------------------------------ԭ��û�й��ϱ���Ҳû�й���--------------������ƽ��Ϊ�㣩------
		else if((_MSG_SCOUT2==MSG_NONE)&&(M_ChkFlag(SL_ERROR)==0))	
		{
			M_ClrFlag(SL_OFFCB);
			M_ClrFlag(SL_SHUT);
			M_ClrFlag(SL_WARNING);
			M_ClrFlag(SL_OTSER);
			M_ClrFlag(SL_IRCVR);
			M_ClrFlag(SL_NRCVR);			
			M_ClrFlag(SL_DRCVR);
			MAIN_LOOP.cnt_rcvr=0;
			MAIN_LOOP.cnt_otser=0;
			_MSG_SCOUT1=0;
			M_ClrFlag(SL_DISPLAY5);                         //��ϵͳ����ָʾ
		}
	}

//-------------------------����ͣ����λ����-------------------------------------------------------
	if((M_ChkFlag(SL_ERRSTOP)!=0 || M_ChkFlag(SL_SERIESTOP)!=0) &&(M_ChkFlag(SL_OCS_RESET)!=0)&&(M_ChkFlag(SL_NO_RESET)==0)&&(M_ChkFlag(SL_DL_RESET)==0)&&(_MSG_SCOUT2==0))   //û�й��ϲ�ܸ��?
	{	
			M_ClrFlag(SL_SERIESTOP);				//�������ͣ�����ϱ�־
			M_ClrFlag(SL_ERRSTOP);					//���ͣ�����ϱ�־
			M_ClrFlag(SL_ERROR);					//������ϱ�־
			
			M_ClrFlag(SL_OCS_NPRSTART);	          	//����������	 
			M_ClrFlag(SL_OCS_MPRSTART);				//����������								
			M_ClrFlag(SL_QEPPCO);

			M_ClrFlag(SL_EE_FAIL);					//���EEROM���ϱ�־

			M_ClrFlag(SL_PDPINTA);					//�������TZ1_PDP���ϱ�־
			M_ClrFlag(SL_PDPINTB);					//�������TZ2_PDP���ϱ�־
			M_ClrFlag(SL_DISPLAY3);             	//��PDP����ָʾ��
			ClrPdpint();							//PDPINT�ж����	
			EnPdpint();								//PDPINT�ж�ʹ��
	
			_MSG_SCOUT1=0;							//�������Ϣλ
			MAIN_LOOP.cnt_rcvr=0;                   //������ʱ�ָ���ʱ
			MAIN_LOOP.cnt_otser=0;                  //��ʱ������ʱ	
//			_SY_RTRN=0;								//������������?

//			M_ClrFlag(SL_QEPPHE);
//			M_ClrFlag(SL_QEPPCDE);
//			M_ClrFlag(SL_CANOPENOVER);               //CANͨѶ
	}

	if(_SY_RTRN!=0)
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_resetrn,7200000)>=0)	//1 day
//		if(M_ChkCounter(MAIN_LOOP.cnt_resetrn,300000)>=0)	//5 min test
		{
			_SY_RTRN=0;	
			MAIN_LOOP.cnt_resetrn=0;
		}
	}
	else	MAIN_LOOP.cnt_resetrn=0;

//	if(_STDBY2!=0)	M_ClrFlag(SL_NO_RESET);	//systest201005atcpc

} 
/*********************************************************************************************************
** ������?: et_relay
** ��������: ���ر�������-NPR
** �䡡��: 	 
** ��:   
** ?  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void et_relay_N(void)
{
	int32 et1,et2,ev1,ev2,ev,et;
//����	
	ev = (int32)PRO.NPR_iac;
	et = ((ev-_SC_IACOV1) * 512) /_SC_IACOV1;

//200us
	if(abs(et)>255)
	{
		if(et>=0)
		{
			et1=et-256;
			if(et1>255) et1=255;   //����200%�Ļ�������200%
		
			ev1=TAB_ET1[et1];
		}
		else 
		{
			et1=et+256;
			if(et1<-255) et1=-255;
	
			et1=-et1;
			ev1=TAB_ET1[et1];
			ev1=-ev1;
		}
	}
	else ev1=0; 
//1s
	if(M_ChkCounter(MAIN_LOOP.et_relay1,1000)>=0)
	{
		MAIN_LOOP.et_relay1=0;
		et2=et;
		
		if(et2>=0)
		{
			if(et2>255) ev2=0;
			else ev2=TAB_ET2[et2];
		}
		else
		{
			if(et2<-255) ev2=0;
			else
			{
			 et2=-et2;
			 ev2=TAB_ET2[et2];
			 ev2=-ev2;
			}
		}

	}
	else ev2=0;

	ET_SUM1=ET_SUM1+ev1+ev2;
	if(ET_SUM1>65535) ET_SUM1=65535;

	if(ET_SUM1<=0)					//С�ڶֵ������
	{
		M_ClrFlag(SL_SIAC1);
		ET_SUM1=0;
	}
	else
	{
		if(ET_SUM1>et_gate)
		{
			M_SetFlag(SL_SIAC1);
			ET_SUM1=et_gate;
//			M_SetFlag(SL_ERRDATASAVE);	//�����ⲿRAM����ת?20091109atzy
		}
		else M_ClrFlag(SL_SIAC1);
	}
}  
/*********************************************************************************************************
** ��������: et_relay
** ������: ���ر�������--MPR
** �䡡��: 	 
** ��:   
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** ����:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void et_relay_M(void)
{
	int32 et1,et2,ev1,ev2,ev,et;
//����	
	ev = (int32)PRO.MPR_iac;
	et = ((ev-_SC_IACOV2) * 512) /_SC_IACOV2;

//200us
	if(abs(et)>255)
	{
		if(et>=0)
		{
			et1=et-256;
			if(et1>255) et1=255;
		
			ev1=TAB_ET1[et1];
		}
		else 
		{
			et1=et+256;
			if(et1<-255) et1=-255;
	
			et1=-et1;
			ev1=TAB_ET1[et1];
			ev1=-ev1;
		}
	}
	else ev1=0; 
//1s
	if(M_ChkCounter(MAIN_LOOP.et_relay2,1000)>=0)
	{
		MAIN_LOOP.et_relay2=0;
		et2=et;
		
		if(et2>=0)
		{
			if(et2>255) ev2=0;
			else ev2=TAB_ET2[et2];
		}
		else
		{
			if(et2<-255) ev2=0;
			else
			{
			 et2=-et2;
			 ev2=TAB_ET2[et2];
			 ev2=-ev2;
			}
		}

	}
	else ev2=0;

	ET_SUM2=ET_SUM2+ev1+ev2;
	if(ET_SUM2>65535) ET_SUM2=65535;

	if(ET_SUM2<=0)
	{
		M_ClrFlag(SL_SIAC2);
		ET_SUM2=0;
	}
	else
	{
		if(ET_SUM2>et_gate)
		{
			M_SetFlag(SL_SIAC2);
			ET_SUM2=et_gate;
//			M_SetFlag(SL_ERRDATASAVE);	//�����ⲿRAM����ת��20091109atzy
		}
		else M_ClrFlag(SL_SIAC2);
	}
}  

/*********************************************************************************************************
** ������: CntCtrl
** ��������: ��������
** �䡡��:
** �䡡��       
** ע  ��: systest
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** ����:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void CntCtrl(void)
{
	if(MAIN_LOOP.cnt1!=65535)			MAIN_LOOP.cnt1++;
	if(MAIN_LOOP.cnt2!=65535)			MAIN_LOOP.cnt2++;
	if(MAIN_LOOP.cnt3!=65535)			MAIN_LOOP.cnt3++;
	if(MAIN_LOOP.cnt4!=65535)			MAIN_LOOP.cnt4++;
	if(MAIN_LOOP.cnt5!=65535)			MAIN_LOOP.cnt5++;
	if(MAIN_LOOP.cnt6!=65535)			MAIN_LOOP.cnt6++;

	if(MAIN_LOOP.cnt_poweron!=65535)	MAIN_LOOP.cnt_poweron++;			
	if(MAIN_LOOP.cnt_pwmout!=65535)		MAIN_LOOP.cnt_pwmout++;
	if(MAIN_LOOP.cnt_freq!=65535)		MAIN_LOOP.cnt_freq++;
	if(MAIN_LOOP.cnt_cbfreq!=65535)		MAIN_LOOP.cnt_cbfreq++;							
					
	if(MAIN_LOOP.cnt_nprlamp!=65535)	MAIN_LOOP.cnt_nprlamp++;  //ָʾ���������������
	if(MAIN_LOOP.cnt_mprlamp!=65535)	MAIN_LOOP.cnt_mprlamp++;  //ָʾ���������������
				
	if(MAIN_LOOP.cnt_rcvr!=65535)		MAIN_LOOP.cnt_rcvr++;
	if(MAIN_LOOP.cnt_otser!=65535)		MAIN_LOOP.cnt_otser++;

	if(MAIN_LOOP.cnt_estop!=65535)		MAIN_LOOP.cnt_estop++;
	if(MAIN_LOOP.cnt_exfault!=65535)	MAIN_LOOP.cnt_exfault++;
	if(MAIN_LOOP.cnt_qgovload!=65535)	MAIN_LOOP.cnt_qgovload++;
	if(MAIN_LOOP.cnt_pgovload!=65535)	MAIN_LOOP.cnt_pgovload++;
	if(MAIN_LOOP.cnt_canfault!=65535)	MAIN_LOOP.cnt_canfault++;

	if(MAIN_LOOP.cnt_detectdelay!=65535)	MAIN_LOOP.cnt_detectdelay++;	//110818
	if(MAIN_LOOP.cnt_cbtp!=65535)		MAIN_LOOP.cnt_cbtp++;
	if(MAIN_LOOP.cnt_cberror!=65535)	MAIN_LOOP.cnt_cberror++;
	if(MAIN_LOOP.cnt_mainkerror!=65535)	MAIN_LOOP.cnt_mainkerror++;
	if(MAIN_LOOP.cnt_mainferror!=65535)	MAIN_LOOP.cnt_mainferror++;
	if(MAIN_LOOP.cnt_stacerror!=65535)	MAIN_LOOP.cnt_stacerror++;
	if(MAIN_LOOP.cnt_preerror!=65535)	MAIN_LOOP.cnt_preerror++;
	if(MAIN_LOOP.cnt_gfault1!=65535)	MAIN_LOOP.cnt_gfault1++;
	if(MAIN_LOOP.cnt_gfault2!=65535)	MAIN_LOOP.cnt_gfault2++;					
//	if(MAIN_LOOP.cnt_synfail!=65535)	MAIN_LOOP.cnt_synfail++;   
	if(MAIN_LOOP.cnt_nprtlov!=65535)	MAIN_LOOP.cnt_nprtlov++;
	if(MAIN_LOOP.cnt_mprtlov!=65535)	MAIN_LOOP.cnt_mprtlov++;
	if(MAIN_LOOP.cnt_nskiptov!=65535)	MAIN_LOOP.cnt_nskiptov++;
	if(MAIN_LOOP.cnt_mskiptov!=65535)	MAIN_LOOP.cnt_mskiptov++;

	if(SCI.cnt_sciover!=65535)			SCI.cnt_sciover++;
	if(SCI.cnt_scispace!=65535)			SCI.cnt_scispace++;

	if(MAIN_LOOP.cnt_isteady0!=65535)	MAIN_LOOP.cnt_isteady0++;     //RunCtrl����ͣ��������ʱ
	if(MAIN_LOOP.cnt_mprsteady0!=65535)	MAIN_LOOP.cnt_mprsteady0++;   //RunCtrl����ͣ��������ʱ

//	if(MAIN_LOOP.cnt_senszfstdy!=60001)	MAIN_LOOP.cnt_senszfstdy++;		//��������Ư�˲�������
	if(MAIN_LOOP.cnt_speedout!=65535)	MAIN_LOOP.cnt_speedout++;		//20090817
	if(MAIN_LOOP.cnt_uaclv1!=65535)		MAIN_LOOP.cnt_uaclv1++;		//200901027atzy
	if(MAIN_LOOP.cnt_encofault!=65535)	MAIN_LOOP.cnt_encofault++;		//200905atcpc

//sysctl_zl_start
	if(MAIN_LOOP.cnt_cbon!=65535)			MAIN_LOOP.cnt_cbon++;    		//���ϱպ��ӳ���	
	if(MAIN_LOOP.cnt_errstop!=65535)	    MAIN_LOOP.cnt_errstop++;    	//һ�����ͣ��
	if(MAIN_LOOP.cnt_serstop!=65535)	    MAIN_LOOP.cnt_serstop++;        //���ع���ͣ��     
	if(MAIN_LOOP.cnt_norstop!=65535)		MAIN_LOOP.cnt_norstop++;        //����ͣ��
	if(MAIN_LOOP.cnt_synrun!=65535)      	MAIN_LOOP.cnt_synrun++;         //����
	if(MAIN_LOOP.cnt_epwmstop!=65535)      	MAIN_LOOP.cnt_epwmstop++;        //һ����Ϸ��������ʱ������20110829
	if(MAIN_LOOP.cnt_sysoff!=65535)			MAIN_LOOP.cnt_sysoff++;			//����ͣ���ӳ٣�20120321
//sysctl_zl_end	

//ADctl_zl_start
    if(MAIN_LOOP.cnt_AMUX!=65535)      	    MAIN_LOOP.cnt_AMUX++;           //����AD������ʱ��ʱ
//ADctl_zl_end
    if(MAIN_LOOP.cnt_reset!=65535)      	MAIN_LOOP.cnt_reset++;           //��λ��I/O��λ����ʱ2s����ֹ����?
    if(MAIN_LOOP.cnt_clostacmd!=65535)      MAIN_LOOP.cnt_clostacmd++;       //������϶��ӽӴ�������ʱ1s����ֹ���
    if(MAIN_LOOP.cnt_nprcmd!=65535)       	MAIN_LOOP.cnt_nprcmd++;         //��λ������NPR���������?1s����ֹ�����
    if(MAIN_LOOP.cnt_mprcmd!=65535)         MAIN_LOOP.cnt_mprcmd++;         //��λ������MPR�����壬��ʱ1s���������?
	if(MAIN_LOOP.cnt_qcapspdin!=65535)		MAIN_LOOP.cnt_qcapspdin++;   
	if(MAIN_LOOP.cnt_qcapdisturb!=65535)	MAIN_LOOP.cnt_qcapdisturb++;		//QEP����?   
	if(MAIN_LOOP.cnt_qepcntok!=65535)		MAIN_LOOP.cnt_qepcntok++;   		//QEP������   
	if(MAIN_LOOP.cnt_qepzdisturb!=65535)	MAIN_LOOP.cnt_qepzdisturb++;   		//QEP������   
    if(MAIN_LOOP.cnt_ein!=65535)       		MAIN_LOOP.cnt_ein++;        	 //�ϻ������ϣ���ʱ1s����ֹ�����
    if(MAIN_LOOP.cnt_sysrun!=65535)       	MAIN_LOOP.cnt_sysrun++;        	 //��λ����?/������飬��ʱ1s����ֹ�����
    if(MAIN_LOOP.cnt_prec!=65535)       	MAIN_LOOP.cnt_prec++;         	 //��λ��Ԥ��磬��ʱ1s����ֹ�����
	   
//canopen
	if(MAIN_LOOP.canopen_tx!=65535)			MAIN_LOOP.canopen_tx++;   
	if(MAIN_LOOP.canopen_rx!=65535)			MAIN_LOOP.canopen_rx++;      

//	if(SCI_canopen.cnt_toggle!=65535)		SCI_canopen.cnt_toggle++;		//20130801
	if(SCI_canopen.cnt_heartbeat!=65535)	SCI_canopen.cnt_heartbeat++;      
	if(SCI_canopen.cnt_sciover!=65535)		SCI_canopen.cnt_sciover++;   
	if(MAIN_LOOP.cnt_cbreset!=65535)		MAIN_LOOP.cnt_cbreset++;  

	if(MAIN_LOOP.et_relay1!=65535)		    MAIN_LOOP.et_relay1++;      
	if(MAIN_LOOP.et_relay2!=65535)		    MAIN_LOOP.et_relay2++;   

//	if(MAIN_LOOP.cnt_ocsnprrun!=65535)		MAIN_LOOP.cnt_ocsnprrun++; //120507 
	if(MAIN_LOOP.cnt_ocsein1!=65535)		MAIN_LOOP.cnt_ocsein1++;   
	if(MAIN_LOOP.cnt_ocsein2!=65535)		MAIN_LOOP.cnt_ocsein2++;  
	if(MAIN_LOOP.cnt_ocssysrun1!=65535)		MAIN_LOOP.cnt_ocssysrun1++;      
	if(MAIN_LOOP.cnt_ocssysrun2!=65535)		MAIN_LOOP.cnt_ocssysrun2++;   

	if(MAIN_LOOP.cnt_datasave!=65535)		MAIN_LOOP.cnt_datasave++;   
	if(MAIN_LOOP.cnt_datasave2!=65535)		MAIN_LOOP.cnt_datasave2++;   
//LVRT
    if(MAIN_LOOP.cnt_lv_mpwmstop!=65535)    MAIN_LOOP.cnt_lv_mpwmstop++;  
    if(MAIN_LOOP.cnt_scrizero!=65535)       MAIN_LOOP.cnt_scrizero++;      
    if(MAIN_LOOP.cnt_scrizero2!=65535)      MAIN_LOOP.cnt_scrizero2++;      
    if(MAIN_LOOP.cnt_lv_scron!=65535)       MAIN_LOOP.cnt_lv_scron++;        
    if(MAIN_LOOP.cnt_lv_mpwmstart!=65535)   MAIN_LOOP.cnt_lv_mpwmstart++;       
    if(MAIN_LOOP.cnt_mpwmrestart!=65535)    MAIN_LOOP.cnt_mpwmrestart++;       

    if(MAIN_LOOP.cnt_npwmrestart!=65535)    MAIN_LOOP.cnt_npwmrestart++;  //201007BJTULVRT     
    if(MAIN_LOOP.cnt_lv_state!=65535)   	MAIN_LOOP.cnt_lv_state++;       
    if(MAIN_LOOP.cnt_hv_rcv!=65535)    		MAIN_LOOP.cnt_hv_rcv++; 
	if(MAIN_LOOP.cnt_lv_rcv!=65535)    		MAIN_LOOP.cnt_lv_rcv++;           
    if(MAIN_LOOP.cnt_lv_time!=65535)    	MAIN_LOOP.cnt_lv_time++;       
    if(MAIN_LOOP.cnt_hvlv_detect!=65535)    	MAIN_LOOP.cnt_hvlv_detect++;       
    if(MAIN_LOOP.cnt_scrondelay!=65535)    	MAIN_LOOP.cnt_scrondelay++;       
    if(MAIN_LOOP.cnt_lv_fault1!=65535)    	MAIN_LOOP.cnt_lv_fault1++;       
    if(MAIN_LOOP.cnt_lv_fault2!=65535)    	MAIN_LOOP.cnt_lv_fault2++;       

    if(MAIN_LOOP.cnt_resetrn!=100000000)    MAIN_LOOP.cnt_resetrn++;   
	if(MAIN_LOOP.cnt_phorde!=65535)		    MAIN_LOOP.cnt_phorde++;				//201005atcpc

	if(MAIN_LOOP.cnt_freqrecv!=65535)		MAIN_LOOP.cnt_freqrecv++;			//201011PLL
//	if(MAIN_LOOP.cnt_lv_gving!=65535)		MAIN_LOOP.cnt_lv_gving++;			//201011PLL
	if(MAIN_LOOP.cnt_unbalance!=65535)	MAIN_LOOP.cnt_unbalance++;		//201011LVRT

//---------20121103atsp---------------
//---------BJTULVRT201204
	if(MAIN_LOOP.cnt_qworking!=65535)	MAIN_LOOP.cnt_qworking++;		
	if(MAIN_LOOP.cnt_lv_phicontrol!=65535)	MAIN_LOOP.cnt_lv_phicontrol++;		
	if(MAIN_LOOP.cnt_lv_lowpi!=65535)		MAIN_LOOP.cnt_lv_lowpi++;	
	if(MAIN_LOOP.cnt_lv_mpridvar!=65535)	MAIN_LOOP.cnt_lv_mpridvar++;	
	if(MAIN_LOOP.cnt_lv_utlv!=65535)		MAIN_LOOP.cnt_lv_utlv++;	
//--------20130225--------
	if(MAIN_LOOP.cnt_lv_irqdelay!=65535)	MAIN_LOOP.cnt_lv_irqdelay++;	//20121210
	if(MAIN_LOOP.cnt_lv_start_scron!=65535)		MAIN_LOOP.cnt_lv_start_scron++;	//20121210
	if(MAIN_LOOP.cnt_lv_start_scroff!=65535)		MAIN_LOOP.cnt_lv_start_scroff++;	//20121210
//	if(MAIN_LOOP.cnt_lv_scroff1!=65535)		MAIN_LOOP.cnt_lv_scroff1++;	//20121210
	if(MAIN_LOOP.cnt_lv_scroff2!=65535)		MAIN_LOOP.cnt_lv_scroff2++;	//20121210
	if(MAIN_LOOP.cnt_lv_scroff3!=65535)		MAIN_LOOP.cnt_lv_scroff3++;	//20130221
	if(MAIN_LOOP.cnt_lv_unblcscroff1!=65535)		MAIN_LOOP.cnt_lv_unblcscroff1++;	//20121210
	if(MAIN_LOOP.cnt_lv_unblcscroff2!=65535)		MAIN_LOOP.cnt_lv_unblcscroff2++;	//20121210

	if(MAIN_LOOP.cnt_lv_unblcscroff3!=65535)	MAIN_LOOP.cnt_lv_unblcscroff3++;	//20130124
	if(MAIN_LOOP.cnt_lv_npriqrf!=65535)			MAIN_LOOP.cnt_lv_npriqrf++;		//20130124
	if(MAIN_LOOP.cnt_lv_mpridrf!=65535)			MAIN_LOOP.cnt_lv_mpridrf++;		//20130130
	if(MAIN_LOOP.cnt_hvlv_kq!=65535)			MAIN_LOOP.cnt_hvlv_kq++;			//20130124
	if(MAIN_LOOP.cnt_gridfault_last!=65535)		MAIN_LOOP.cnt_gridfault_last++;	//20130124
	if(MAIN_LOOP.cnt_lv_dlrecmstop!=65535)		MAIN_LOOP.cnt_lv_dlrecmstop++;	//20130130
//	if(MAIN_LOOP.cnt_lv_xnzkkz!=65535)			MAIN_LOOP.cnt_lv_xnzkkz++;		//20130202
	if(MAIN_LOOP.cnt_jgvdscron!=65535)			MAIN_LOOP.cnt_jgvdscron++;		//20130205
	if(MAIN_LOOP.cnt_lv_reciq!=65535)			MAIN_LOOP.cnt_lv_reciq++;		//20130205
	if(MAIN_LOOP.cnt_lv_holdpow!=65535)			MAIN_LOOP.cnt_lv_holdpow++;		//20130205
	if(MAIN_LOOP.cnt_lv_scrkeepon!=65535)		MAIN_LOOP.cnt_lv_scrkeepon++;		//20130221
	
	if(MAIN_LOOP.cnt_mc_ack!=65535)		    MAIN_LOOP.cnt_mc_ack++;				//MagnetCurve2013-12-13
		
//--------20130801--------
	if(M_ChkFlag(SL_ECPOSSEARCH)!=0 && M_ChkFlag(SL_MPR_START)!=0)
	{
		if(MAIN_LOOP.cnt_synfail!=200000)	MAIN_LOOP.cnt_synfail++;   	//20130525
	}
	if(MAIN_LOOP.cnt_ecposcheck!=65535)		MAIN_LOOP.cnt_ecposcheck++;			//20130525
	if(MAIN_LOOP.cnt_ecpostrace!=65535)		MAIN_LOOP.cnt_ecpostrace++;			//20130525

	if(MAIN_LOOP.cnt_chopper!=65535)		MAIN_LOOP.cnt_chopper++;		//201101PLL

}

/*********************************************************************************************************
** ��������: Display
** ������: ��ʾֵ��?
** �䡡��: 	 
** ���:   
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Display(void)
{
	float temp1,temp2,temp3;

//------------------------�м�ֱ����ѹ��ָ��ֵ��ʾֵ����-------------------------------------------
	DISP.udc  = PRO.udc;							//��λV
	DISP.urf  = RUN.urf;							//��λV												//��λV
	DISP.mpridrf = RUN.mpridrf;							//��λA
	DISP.npriqrf = RUN.npriqrf;							//��λA
	DISP.mpriqrf = RUN.mpriqrf;							//��λA
	DISP.toqrf   = RUN.toqrf;							//��λNM
	DISP.aglrf   = RUN.aglrf;							//��λ �Ƕ�
    DISP.toqfb   = Te_feedback;    
//---------------------------�ߵ�ѹ��Чֵ��ʾֵ����------------------------------------------------
	DISP.npr_uab = PRO.NPR_uab;						//��λV
	DISP.npr_ubc = PRO.NPR_ubc;						//��λV
	DISP.sta_uab = PRO.STA_uab;						//��λV
	DISP.sta_ubc = PRO.STA_ubc;						//��λV

//---------------------------����ǰ�����ߵ�ѹ��Чֵ����-��ʾ---------------------------
//	DISP.grd_uab = MEAN_DATA.uab * 1.110721;        //��λV  rms=mean*PAI/(2*sqrt(2)) �� 1.110721=PAI * SQRT2 / 4
//	DISP.grd_ubc = MEAN_DATA.ubc * 1.110721;        //��λV  rms=mean*PAI/(2*sqrt(2)) �� 1.110721=PAI * SQRT2 / 4
	DISP.grd_uab = MEAN_DATA.uab1 * 1.110721;        //��λV  rms=mean*PAI/(2*sqrt(2)) �� 1.110721=PAI * SQRT2 / 4
	DISP.grd_ubc = MEAN_DATA.ubc1 * 1.110721;        //��λV  rms=mean*PAI/(2*sqrt(2)) �� 1.110721=PAI * SQRT2 / 4

//---------------------------����ͻ������¶���ʾ----------------------------
    DISP.Lac_temp      = MEAN_DATA.Lac_temp;                  //�������¶�
	DISP.Ldudt_temp    = MEAN_DATA.Ldudt_temp;                //�������¶� 
	DISP.NPR_skiptemp  = AMUX.NPR_skiiptemp;              //����SKIIP�¶�
	DISP.MPR_skiptemp  = AMUX.MPR_skiiptemp;              //����SKIIP��


//---------------------------˲ʱ������ʾֵ����----------------------------------------------------
	DISP.npr_iai = AD_OUT_NPR_I.a;					//��λA
	DISP.npr_ibi = AD_OUT_NPR_I.b;					//��λA
	DISP.npr_ici = AD_OUT_NPR_I.c;					//��λA
	DISP.mpr_iai = AD_OUT_MPR_I.a;					//��λA
	DISP.mpr_ibi = AD_OUT_MPR_I.b;					//��λA
	DISP.mpr_ici = AD_OUT_MPR_I.c;					//��λA

//------------------------���ಢ��������Чֵ��ʾֵ��?----------------------------------------------
	DISP.npr_iar = PRO.npr_iar;							//��λA
	DISP.npr_ibr = PRO.npr_ibr;							//��λA
	DISP.npr_icr = PRO.npr_icr;							//��λA

//------------------------���������Чֵ��ʾֵ����--------------------------------------------------
    temp1 = MEAN_DATA.ia2 * 1.110721;
	temp2 = MEAN_DATA.ib2 * 1.110721;
	temp3 = MEAN_DATA.ic2 * 1.110721;
	DISP.mpr_iar = temp1;							//��λA
	DISP.mpr_ibr = temp2;							//��λA
	DISP.mpr_icr = temp3;							//��λA

//------------------------�����ߵ�����Чֵ��ʾֵ����--------------------------------------------------
    temp1 = MEAN_DATA.iac3 * 1.110721;
	temp2 = MEAN_DATA.icb3 * 1.110721;
	DISP.sta_iac = temp1;							//��λA
	DISP.sta_iba = temp2;							//��λA

//------------------------------���Ӳ��������ܲ����й�������ʾ-----------------------------------------------------
	DISP.Psactive   = PRO.Psactive;
	DISP.Psreactive = PRO.Psreactive;
	DISP.Ps		    = PRO.Ps;

	DISP.Pnactive   = PRO.Pnactive;
	DISP.Pnreactive = PRO.Pnreactive;
    DISP.Pn         = PRO.Pn;

	DISP.Pgactive   = PRO.Pgactive;
	DISP.Pgreactive = PRO.Pgreactive;
	DISP.Pg      	= PRO.Pg;

//------------------------------Ƶ�ʺ�ת����ʾ-----------------------------------------------------	
	if(M_ChkFlag(SL_IN1_CBSTS)==0) 	DISP.freq=0;            //20090815
	else 							DISP.freq = CAP4.freqtmp;

	DISP.speed= PRO.speedflt;		//20090815

//------------------------------����ͬ������ǰ����ǰ���ѹ��ֵ��ʾ-----------------------------------------------------
	DISP.uab23 = MEAN_DATA.uab_d;
	DISP.ubc23 = MEAN_DATA.ubc_d;

//------------------------------PI�������ʾ-------------------------------------------------------
	DISP.pionu  = PI_NPR_U.out;						//��λA
	DISP.pionid = PI_NPR_Id.out;					//��λV
	DISP.pioniq	= PI_NPR_Iq.out;					//��λV
	DISP.mexi  = RUN_mpridrf_exi;					//��λA  �����������ۼ���ֵ��ʾ
	DISP.piomid = PI_MPR_Id.out;				    //��λV
	DISP.piomiq	= PI_MPR_Iq.out;					//��λV
} 
/*********************************************************************************************************
** ��������: Bank()
** ��������: ��ʾ����
** �䡡��: 	 
** ���:   
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Bank(void)
{
	
//------------------------------------���๤��ָʾ����ʾ--------------------------------------------
	if(M_ChkFlag(SL_NPR_PWMOUT)!=0)
	{
		M_SetFlag(SL_DISPLAY0);
/*		if(M_ChkCounter(MAIN_LOOP.cnt_nprlamp,DELAY_NPRRUN)>=0)	//0�ƿ�����?,ָʾ�������ڷ���������
		{
			M_NotFlag(SL_DISPLAY0);
			MAIN_LOOP.cnt_nprlamp=0;
		}
*/	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_nprlamp,DELAY_NPRSTDBY)>=0)	//0��������?
		{
		   M_NotFlag(SL_DISPLAY0);
		   MAIN_LOOP.cnt_nprlamp=0;
		} 
	}
			
//------------------------------------���๤���ʾ�����?--------------------------------------------
	if(M_ChkFlag(SL_MPR_PWMOUT)!=0)
	{
		M_SetFlag(SL_DISPLAY1);
/*		if(M_ChkCounter(MAIN_LOOP.cnt_mprlamp,DELAY_MPRRUN)>=0)	//1�ƿ�����?,ָʾ�������ڷ���������
		{
			M_NotFlag(SL_DISPLAY1);
			MAIN_LOOP.cnt_mprlamp=0;
		}
*/	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_mprlamp,DELAY_MPRSTDBY)>=0)	//1��������˸
		{
			M_NotFlag(SL_DISPLAY1);
			MAIN_LOOP.cnt_mprlamp=0;
		}
	}


	if(M_ChkFlag(SL_STEADYFB)!=0)	M_SetFlag(SL_DISPLAY2);			//Vdc�ȶ�ָʾ
	else							M_ClrFlag(SL_DISPLAY2);

	if(M_ChkFlag(SL_UDSTACOK)!=0 && M_ChkFlag(SL_MPR_SYNOK)!=0)	M_SetFlag(SL_DISPLAY4);			//���Ӵﵽ��������ָʾ
	else if(M_ChkFlag(SL_UDSTACOK)!=0)							M_NotFlag(SL_DISPLAY4);
	else														M_ClrFlag(SL_DISPLAY4);

	if(M_ChkFlag(SL_CBCLOSED)!=0 && M_ChkFlag(SL_OCS_SYSRUN)==0 && M_ChkFlag(SL_SERIESTOPING)==0 && M_ChkFlag(SL_SERIESTOP)==0 && M_ChkFlag(SL_ERRSTOPING)==0 && M_ChkFlag(SL_ERRSTOP)==0 && M_ChkFlag(SL_OCS_EIN)!=0)	
			M_SetFlag(SL_DISPLAY7);									//������պ?,Ԥ������ָʾ
	else	M_ClrFlag(SL_DISPLAY7);

//--------20130801--------
	if((M_ChkFlag(SL_POWERON)==0) && (M_ChkFlag(SL_MPR_START)==0))
		QEPDATA.encodpos =  - _encodpos;

//----------------------------------------���м��--------------------------------------------------
	_BA_URF   = (int16)DISP.urf;				//�м��ѹ����ֵ
	_BA_UDC   = (int16)DISP.udc;				//�м�ֱ����?
	_BA_MIDRF  = (int16)(DISP.mpridrf * 10);		//d�����ָ��
	_BA_NIQRF  = (int16)(DISP.npriqrf * 10);		//q�����ָ��
	_BA_MIQRF  = (int16)(DISP.mpriqrf * 10);		//q�����ָ��
	_BA_TOQRF  = (int16)(DISP.toqrf);				//ת��ָ��
    _BA_AGLRF  = (int16)(DISP.aglrf);             //�޹��Ƕȸ�? 20091027atzy

	_BA_IA1   = (int16)(DISP.npr_iar * 10);		//����,a�ಢ����������Ϊ1λ��?
	_BA_IB1   = (int16)(DISP.npr_ibr * 10);		//����,b�ಢ������
	_BA_IC1   = (int16)(DISP.npr_icr * 10);		//��?,c�ಢ������
	_BA_IA2   = (int16)(DISP.mpr_iar * 10);		//�����,a���
	_BA_IB2   = (int16)(DISP.mpr_ibr * 10);		//�����,b�����
	_BA_IC2   = (int16)(DISP.mpr_icr * 10);		//�����,c�����

	_BA_UAB0   = (int16)DISP.grd_uab;			//����ǰ����ab��ѹ
	_BA_UBC0   = (int16)DISP.grd_ubc;			//����ǰ����bc��ѹ				
	_BA_UAB1  = (int16)DISP.npr_uab;			//����ab��ѹ
	_BA_UBC1  = (int16)DISP.npr_ubc;			//����bc��ѹ
	_BA_UAB2  = (int16)DISP.sta_uab;			//���Ӳ�ab��ѹ,���Ӳ� 20091027atzy
	_BA_UBC2  = (int16)DISP.sta_ubc;			//���Ӳ�bc��ѹ,���Ӳ� 20091027atzy
	_BA_GRDUD = (int16)TRS_NGS_U.dflt;          //d�����Ϻ���ѹ���� �˲���
	_BA_GRDUQ = (int16)TRS_NGS_U.qflt;          //q�����Ϻ���ѹ���� �˲���
	_BA_NPRID = (int16)TRS_NPR_I.dflt;          //d������������� �˲���
	_BA_NPRIQ = (int16)TRS_NPR_I.qflt;          //q������������� �˲���
//	_BA_EXCID = (int16)(TRS_MPR_I.d * 10);             //d����������? �˲�ǰ
//	_BA_EXCIQ = (int16)(TRS_MPR_I.q * 10);             //q������������ �˲�ǰ
    _BA_SCRIA = (int16)(AD_OUT_SCR_I.a * 10);             //SCR����a	20110906
    _BA_SCRIB = (int16)(AD_OUT_SCR_I.b * 10);             //SCR����b	20110906

//	_BA_STAUD = (int16)TRS_MPR_U.d;		//2013-12-13
//	_BA_STAUQ = (int16)TRS_MPR_U.q;		//2013-12-13

	_BA_MC_IN = (int16)(MC_DATA.in * 100);		//MagnetCurve2013-12-13ZZJ
	_BA_MC_K  = (int16)(MC_DATA.k * 1000);		//MagnetCurve2013-12-13ZZJ

	_BA_TLAC  = (int16)DISP.Lac_temp;		    //�������¶�
//	_BA_TLAC  = (int16)SCI_canopen.cnt_heartbeat;	    //�������¶� 2014-05-06LJD��ʱ�޸�ΪCANopen

	_BA_TLDUDT= (int16)DISP.Ldudt_temp;	        //�������¶�
	_BA_TNSKIIP= (int16)DISP.NPR_skiptemp;      //����SKIIP�¶� ���϶�
	_BA_TMSKIIP= (int16)DISP.MPR_skiptemp;      //����SKIIP�¶� ���϶�

	_BA_FREQ  = (int16)(DISP.freq * 10);		//����Ƶ��
	_BA_SPEED = (int16)DISP.speed;				//���ת�� 

	_BA_PIONU  = (int16)(DISP.pionu  * 10);			    //NPR��ѹ����� 6.23change_zl��Ϊ1λС?
	_BA_PIONID = (int16)(DISP.pionid * 100);			//NPR������d���
	_BA_PIONIQ = (int16)(DISP.pioniq * 100);			//NPR������q���
	_BA_MEXI   = (int16)(DISP.mexi  * 10);			    // 6.23change_zl��Ϊ1С�� ��Ϊ���ŵ�������ֵ��ʾcpc
	_BA_PIOMID = (int16)(DISP.piomid * 100);			//MPR������d���
	_BA_PIOMIQ = (int16)(DISP.piomiq * 100);			//MPR������q���

//	_BA_STAUABD = (int16)(DISP.uab23 * 10);             //����ͬ������ǰ���ǰ���ѹ�? 20091027atzy//2013-12-13
	_BA_STAUABD = (int16)(MC_U_test * 10);             	//�������ŵ�ѹ��--����У����������MagnetCurve2013-12-13ZZJ
	_BA_STAUBCD = (int16)(DISP.ubc23 * 10);				//����ͬ������ǰ����ǰ���ѹ�� 20091027atzy
	_BA_STAIAC = (int16)(DISP.sta_iac * 10);            //�����ߵ�����Чֵ��ʾ
	_BA_STAIBA = (int16)(DISP.sta_iba * 10);			//�����ߵ�����Чֵ��ʾ

	_BA_TOQFB  = (int16)DISP.toqfb;				        //ת�ط���

	_BA_PSTA  = (int16)(DISP.Psactive * 0.01);          //���Ӳ��й�������ʾkW,DOT1 *10/1000=100
	_BA_PNPR  = (int16)(DISP.Pnactive * 0.01);          //�����й�������ʾ
	_BA_PGRID = (int16)(DISP.Pgactive * 0.01);          //�����ܵ��й�������ʾ

	_BA_QSTA  = (int16)(DISP.Psreactive * 0.01);        //���Ӳ��޹�������ʾkVAR,DOT1 *10/1000=100
	_BA_QNPR  = (int16)(DISP.Pnreactive * 0.01);        //�����޹�������ʾ
	_BA_QGRID = (int16)(DISP.Pgreactive * 0.01);        //�����ܵ��޹�������ʾ

	_BA_SSTA  = (int16)(DISP.Ps * 0.01);              	//���Ӳ����ڹ�����ʾkVA,DOT1 *10/1000=100
	_BA_SNPR  = (int16)(DISP.Pn * 0.01);              	//�������ڹ�����ʾ
	_BA_SGRID = (int16)(DISP.Pg * 0.01);              	//�����ܵ����ڹ�����ʾ

//ͨ��CANOPENͨѶ���������صļ�ر���	

//	DISP.toqfb = 8000;  //cantest
//    DISP.speed = 1800;  //cantest
//	AMUX.skiiptempmax = 65; //cantest

	SCI_canopen.tx_watertempin  = 0;
	SCI_canopen.tx_watertempout = 0;
	SCI_canopen.tx_demand = 0;

}


/*********************************************************************************************************
** ��������: ERROR_Datasave
** ��  ��: ������д���ⲿRAM
** �䡡��:
** �䡡��:        
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 20100203atbjtu PCmodbus
**-------------------------------------------------------------------------------------------------------
** �޸�?:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void ERROR_Datasave(void)
{
//--------------------------------External RAM Data Save-----------------------------------------
	//20121103
//--------------------------------External RAM Data Save-----------------------------------------

	if(M_ChkFlag(SL_PC_ERRDATACLR)!=0)						//PCָ���ⲿRAM�����ͷ�
	{
		M_ClrFlag(SL_RAMERRORSAVE);						
		*(ERROR_RAMEND + 1) = 0x0000;
		M_ClrFlag(SL_TRIG_ERRDSAVE);
		M_ClrFlag(SL_PC_ERRDATACLR);						//�ͷ�����/��λ���,�Զ������λָ��
		M_ClrFlag(SL_PC_ERRDATASAVE);						//�ͷ�����/��λ���,�Զ�����ֶ������洢ָ�� 201005atcpc
		M_ClrFlag(SL_DISPLAY6);								//201007BJTULVRT
		*(BANK_RAMEND + 1) = 0x0000;
		M_ClrFlag(SL_RAMBANKSAVE);							//
	} 

  	if(*(ERROR_RAMEND + 1) != 0x55AA)
  	{
		if(M_ChkFlag(SL_TRIG_ERRDSAVE)==0)  								//û�й���	
			MAIN_LOOP.cnt_datasave=0;
		else if(M_ChkCounter(MAIN_LOOP.cnt_datasave,DELAY_DATASAVE)>=0)	//���Ϸ�������ʱ1.8s��ֹ��ͼ201205LVRTatZB
		{
			M_SetFlag(SL_RAMERRORSAVE);
//			M_ClrFlag(SL_TRIG_ERRDSAVE);
			*(ERROR_RAMEND + 1) = 0x55AA;			
			*(ERROR_RAMEND + 2) = RTIMER.time[0];				//���¹���ʱ��systest 201005atcpc
			*(ERROR_RAMEND + 3) = RTIMER.time[1];
			*(ERROR_RAMEND + 4) = RTIMER.time[2];
			*(ERROR_RAMEND + 5) = RTIMER.time[3];
			*(ERROR_RAMEND + 6) = RTIMER.time[4];
			*(ERROR_RAMEND + 7) = RTIMER.time[5];				
			*(ERROR_RAMEND + 8) = _MSG_SCOUT2;				

			_BA_TIME5_0 = RTIMER.time[0];						//���¹���ʱ��systest 201005atcpc
			_BA_TIME5_1 = RTIMER.time[1];
			_BA_TIME5_2 = RTIMER.time[2];
			_BA_TIME5_3 = RTIMER.time[3];
			_BA_TIME5_4 = RTIMER.time[4];
			_BA_TIME5_5 = RTIMER.time[5];				
			_BA_ERR5	= _MSG_SCOUT2;				
		}

		if(M_ChkFlag(SL_RAMERRORSAVE)==0)  										//�й���֮����ʱ2s��ֹͣ���ݴ洢
		{
//----------LVRT--201205LVRTatZB
			*(ERROR_RAMSTART+ ERROR_RAMDATA_POS) = (int16)AD_OUT_UDC;										//0=�м�ֱ����ѹ
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 1 + ERROR_RAMDATA_POS)) = (int16)(CAP4.nprtrstheta*1000);	//1=NPR����Ƕ�
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 2 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NPR_I.a*10);		//2=NPR-A����
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 3 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NPR_I.b*10);		//3=NPR-B����
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 4 + ERROR_RAMDATA_POS)) = (int16)(GRD_Ut*10);				//4=**********������ѹ**********
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 5 + ERROR_RAMDATA_POS)) = (int16)(NGS_Udq*10);				//5=*****������ѹ******
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 6 + ERROR_RAMDATA_POS)) = (int16)(NGS_Udq_p*10);			//6=********������ѹ����������PLL*******
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 7 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_SCR_I.a);		//7=SCR����BJTULVRT201204
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 8 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_SCR_I.b);		//8=SCR����BJTULVRT201204
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 9 + ERROR_RAMDATA_POS)) = (int16)(GIVE.toqrf);			//9=**********���ظ���ת��*ʾ������ʾ��С��10��*************
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 10 + ERROR_RAMDATA_POS)) = (int16)(GIVE.toqrfrun);		//10=*******���ظ���ת�ش��˲�*ʾ������ʾ��С��10��*********
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 11 + ERROR_RAMDATA_POS)) = (int16)(PI_NPR_Id.reference*10);//11=����Idָ��
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 12 + ERROR_RAMDATA_POS)) = (int16)(TRS_NPR_I.dflt*10);		//12=����Id����
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 13 + ERROR_RAMDATA_POS)) = (int16)(RUN.toqrf);			//13=*******���и���ת��*ʾ������ʾ��С��10��*********
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 14 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_DC_I.b*10);		//14=�м�ֱ��ն������BJTULVRT201204
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 15 + ERROR_RAMDATA_POS)) = (int16)(RUN.mpridrf*10);		//15=�������ŵ���ָ��
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 16 + ERROR_RAMDATA_POS)) = (int16)(TRS_MPR_I.dflt*10);		//16=�������ŵ�������
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 17 + ERROR_RAMDATA_POS)) = (int16)(RUN.mpriqrf*10);		//17=����ת�ص���ָ��
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 18 + ERROR_RAMDATA_POS)) = (int16)(TRS_MPR_I.qflt*10);		//18=����ת�ص�������BJTULVRT201204
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 19 + ERROR_RAMDATA_POS)) = (int16)(NGS_Udq_n2p* 10);	    //19=��ѹ��ƽ���
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 20 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_MPR_I.a*10);		//20=MPR-A����
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 21 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_MPR_I.b*10);		//21=MPR-B����
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 22 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_MPR_I.c*10);		//22=MPR-C����
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 23 + ERROR_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_STATE) * 10);//23=�͵�ѹ״̬
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 24 + ERROR_RAMDATA_POS)) = (int16)PRO.speed; 				//24=���ת��

			*(ERROR_RAMEND) = ERROR_RAMDATA_POS;															//

			ERROR_RAMDATA_POS++;
			if(ERROR_RAMDATA_POS >= RAM_BIAS)  ERROR_RAMDATA_POS=0;		
		}
  	}

//--------------------------------External RAM Data Read-----------------------------------------
} 

/*********************************************************************************************************
** ��������: BANK_Datasave
** ��  ��: ������д���ⲿRAM
** �䡡��:
** �䡡��:        
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 20100203atbjtu PCmodbus
**-------------------------------------------------------------------------------------------------------
** ����:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void BANK_Datasave(void)
{
//--------------------------------External RAM Data Save-----------------------------------------
	//20121103
  	if(*(BANK_RAMEND + 1) != 0x55AA)
  	{
		if(M_ChkFlag(SL_TRIG_ERRDSAVE)==0)  								//û�й���	
			MAIN_LOOP.cnt_datasave2=0;
		else if(M_ChkCounter(MAIN_LOOP.cnt_datasave2,DELAY_DATASAVE)>=0)	//���Ϸ�������ʱ1.8s��ֹ��ͼ201205LVRTatZB
		{
			M_SetFlag(SL_RAMBANKSAVE);
			*(BANK_RAMEND + 1) = 0x55AA;			
		}

		if(M_ChkFlag(SL_RAMBANKSAVE)==0)  																//PCָ��δҪ���������棬����ˢ��״̬
		{
			if(_NPR_ID_Kd==0)
			{
			*(BANK_RAMSTART+ BANK_RAMDATA_POS) = (int16)(DIP_STA_I.qflt);							//0=���ඨ��Ƕ�
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 1 + BANK_RAMDATA_POS)) = (int16)(CAP4.mprtrstheta*1000);	//1=���ඨ��Ƕ?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 2 + BANK_RAMDATA_POS)) = (int16)(GRD_Utlv*10);		//2=ת�ӽ�?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 3 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.ab* 10);		//3=�����˲�AB�ߵ�ѹ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 4 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.bc* 10);		//4=�����˲�BC�ߵ�ѹ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 5 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_U.ab* 10);		//5=�����˲�AB�ߵ�ѹ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 6 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_U.bc* 10);		//6=�����˲�BC�ߵ�ѹ

//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 3 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_STATE) * 10);		//3=�����˲�AB�ߵ�ѹ
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 4 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_QWORKING)* 10);		//4=�����˲�BC����?
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 5 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_SCRRUNING)* 10);		//5=�����˲�AB�ߵ�ѹ
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 6 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_SCRIZERO)* 10);		//6=�����˲�BC�ߵ�ѹ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 7 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_SYNCON)* 10);					//7=�м�ֱ����ѹ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 8 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_UNBALANCE)* 10);			//8=����d���ѹ�����˲�ǰ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 9 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_MSTOP)* 10);			//9=����q���ѹ�ֿ�˲��?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 10 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_NSTOP)* 10);		//10=����A�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 11 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_I.ac);		//11=����B�����
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_PHICON)* 10);		//11=����B�����
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(GIVE.npriqrf_cl);		//12=����C�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(NGS_Udq_pflt* 10);		//12=����C�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 13 + BANK_RAMDATA_POS)) = (int16)(RUN.npriqrf* 10);		//13=����A�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 14 + BANK_RAMDATA_POS)) = (int16)(TRS_NPR_I.qflt* 10);		//14=����B�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 15 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NPR_I.c* 10);		//15=����C���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 16 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_STRICTLV) * 10);			//16=����ʵʱƵ��
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 17 + BANK_RAMDATA_POS)) = (int16)(kq * 100);					//17=���ת��
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 18 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_I.cb);	//18=�����ѹ�����
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_SCRKEEPON)* 10);		//19=�������A��BJTULVRT201204
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_SCRDONE)* 10);		//19=�������A��BJTULVRT20130221
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_SCRKEEPON) * 10);		//20=���ӵ���C��
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(TRS_NGS_U.d * 10);		//20=���ӵ���C��
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(PHI_DATA_M.PHId * 10);		//21=�м�ֱ��ն�����BJTULVRT201204
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_START_SCRON) * 10);		//21=20121210
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(PHI_DATA_M.PHIq* 10);			//22=�������ŵ���ָ��
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_IMUNBALANCE) * 10);		//21=20130116
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(PRO.Pgactive2 * 0.01);		//21=20130116
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(PHI_DATA_M.Id);		//23=SCR֧·���� BJTULVRT201204
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(TRS_NPR_U.d * 10);		//23=SCR֧·���� BJTULVRT201204
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(PHI_DATA_M.Iq);		//24=SCR֧·���� BJTULVRT201204
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(TRS_NPR_U.q * 10);		//24=SCR֧·���� BJTULVRT201204
			}
			else if(_NPR_ID_Kd==1000)
			{
			*(BANK_RAMSTART+ BANK_RAMDATA_POS) = (int16)(NGS_Udq_p * 10);							        //0=��ѹ�������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 1 +  BANK_RAMDATA_POS)) = (int16)(CAP4.mprtrstheta*1000);	//1=���ඨ��Ƕ�
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 2 +  BANK_RAMDATA_POS)) = (int16)(NGS_Udq_n2pex * 10);		//2=��ѹ������������ǰ��ѹ�������֮��
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 3 +  BANK_RAMDATA_POS)) = (int16)(kq * 100);				//3=�޹�����ϵ��
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 4 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_HV_STATE)* 10);	//4=�ߵ�ѹ��Խ״̬
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 5 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_UNBALANCE)* 10);	//5=������ƽ��״̬
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 6 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_MSTOP)* 10);		//6=���������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 7 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_NSTOP)* 10);		//7=���������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 8 +  BANK_RAMDATA_POS)) = (int16)(DIP_STA_I.qflt * 10);			//8=���Ӳ�q���޹������˲���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 9 +  BANK_RAMDATA_POS)) = (int16)(PI_NPR_U.reference);			//9=ֱ����ѹ�ο�ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 10 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_U.feedback);			//10=ֱ����ѹ����ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 11 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.reference);		 	//11=�����޹������ο�ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.feedback);			//12=�����޹���������ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 13 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.reference);			//13=�����й������ο�ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 14 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.feedback);			//14=�����޹���������ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 15 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.out);			//15=����id��PI���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 16 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.out);			//16=����iq��PI���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 17 + BANK_RAMDATA_POS)) = (int16)(PHAI_d);					//17=���Ӵ���d�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 18 + BANK_RAMDATA_POS)) = (int16)(PHAI_q);				    //18=���Ӵ���q�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(PI_MPR_Id.out *10);			//19=����id���������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(PI_MPR_Iq.out *10);			//20=����iq���������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(TRS_MPR_U.d * 10);		//21=����d���ѹ���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(TRS_MPR_U.q * 10);		//22=����q���ѹ���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(TRS_NPR_U.d * 10);		//23=����d�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(TRS_NPR_U.q * 10);		//24=����q�����
			*(BANK_RAMEND) = BANK_RAMDATA_POS;																
			}
			else if(_NPR_ID_Kd==2000)
			{
			*(BANK_RAMSTART+ BANK_RAMDATA_POS) = (int16)(NGS_Udq_p * 10);							        //0=��ѹ�������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 1 +  BANK_RAMDATA_POS)) = (int16)(CAP4.mprtrstheta*1000);	//1=���ඨ��Ƕ�
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 2 +  BANK_RAMDATA_POS)) = (int16)(NGS_Udq_n2pex * 10);		//2=��ѹ������������ǰ��ѹ�������֮��
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 3 +  BANK_RAMDATA_POS)) = (int16)(kq * 100);				//3=�޹�����ϵ��
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 4 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_HV_STATE)* 10);	//4=�ߵ�ѹ��Խ״̬
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 5 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_UNBALANCE)* 10);	//5=������ƽ��״̬
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 6 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_MSTOP)* 10);		//6=���������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 7 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_NSTOP)* 10);		//7=���������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 8 +  BANK_RAMDATA_POS)) = (int16)(DIP_STA_I.qflt * 10);			//8=���Ӳ�q���޹������˲���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 9 +  BANK_RAMDATA_POS)) = (int16)(PI_NPR_U.reference);			//9=ֱ����ѹ�ο�ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 10 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_U.feedback);			//10=ֱ����ѹ����ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 11 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.reference);		 	//11=�����޹������ο�ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.feedback);			//12=�����޹���������ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 13 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.reference);			//13=�����й������ο�ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 14 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.feedback);			//14=�����޹���������ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 15 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.out);			//15=����id��PI���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 16 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.out);			//16=����iq��PI���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 17 + BANK_RAMDATA_POS)) = (int16)(PHAI_d);					//17=���Ӵ���d�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 18 + BANK_RAMDATA_POS)) = (int16)(PHAI_q);				    //18=���Ӵ���q�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(PHAI_d*10);			//19=����id���������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(PHAI_q*10);			//20=����iq���������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(urdc * 10);		//21=����d���ѹ���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(urqc * 10);		//22=����q���ѹ���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(urdc_dynamic * 10);		//23=����d�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(urqc_dynamic * 10);		//24=����q�����
			*(BANK_RAMEND) = BANK_RAMDATA_POS;		
			}
			else if(_NPR_ID_Kd==3000)
			{
			*(BANK_RAMSTART+ BANK_RAMDATA_POS) = (int16)(NGS_Udq_p * 10);							        //0=��ѹ�������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 1 +  BANK_RAMDATA_POS)) = (int16)(CAP4.mprtrstheta*1000);	//1=���ඨ��Ƕ�
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 2 +  BANK_RAMDATA_POS)) = (int16)(NGS_Udq_n2pex * 10);		//2=��ѹ������������ǰ��ѹ�������֮��
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 3 +  BANK_RAMDATA_POS)) = (int16)(kq * 100);				//3=�޹�����ϵ��
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 4 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_HV_STATE)* 10);	//4=�ߵ�ѹ��Խ״̬
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 5 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_UNBALANCE)* 10);	//5=������ƽ��״̬
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 6 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_MSTOP)* 10);		//6=���������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 7 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_NSTOP)* 10);		//7=���������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 8 +  BANK_RAMDATA_POS)) = (int16)(DIP_STA_I.qflt * 10);			//8=���Ӳ�q���޹������˲���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 9 +  BANK_RAMDATA_POS)) = (int16)(PI_NPR_U.reference);			//9=ֱ����ѹ�ο�ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 10 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_U.feedback);			//10=ֱ����ѹ����ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 11 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.reference);		 	//11=�����޹������ο�ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.feedback);			//12=�����޹���������ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 13 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.reference);			//13=�����й������ο�ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 14 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.feedback);			//14=�����޹���������ֵ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 15 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.out);			//15=����id��PI���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 16 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.out);			//16=����iq��PI���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 17 + BANK_RAMDATA_POS)) = (int16)(PHAI_d);					//17=���Ӵ���d�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 18 + BANK_RAMDATA_POS)) = (int16)(PHAI_q);				    //18=���Ӵ���q�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(TRS_STA_U.dflt*10);			//19=����id���������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(TRS_STA_U.qflt*10);			//20=����iq���������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(TRS_STA_I.dflt*10);		//21=����d���ѹ���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(TRS_STA_I.qflt*10);		//22=����q���ѹ���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(TRS_MPR_I.dflt*10);		//23=����d�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(TRS_MPR_I.qflt*10);		//24=����q�����
			*(BANK_RAMEND) = BANK_RAMDATA_POS;		
			}
			BANK_RAMDATA_POS++;
			if(BANK_RAMDATA_POS >= RAM_BIAS)  BANK_RAMDATA_POS=0;	
			
		}
	}


//--------------------------------External RAM Data Read-----------------------------------------
}  

/*********************************************************************************************************
** ��������: BANK_Datasave
** ��  ��: ������д���ⲿRAM
** �䡡��:
** �䡡��:        
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 20100203atbjtu 
**-------------------------------------------------------------------------------------------------------
** �޸�?:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
/*void BANK_Datasave(void)	//testPCmodbus
{
//--------------------------------External RAM Data Save-----------------------------------------

		if(M_ChkFlag(SL_PC_BANKDATASAVE)==0)  																//PCָ��δҪ���������棬����ˢ��״̬
		{
			*(BANK_RAMSTART+ BANK_RAMDATA_POS) = (int16)(testtheta* 1000);									//0=���ඨ��Ƕ�
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 1 + BANK_RAMDATA_POS)) = (int16)(testtheta_120* 1000);		//1=���ඨ��Ƕ�
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 2 + BANK_RAMDATA_POS)) = (int16)(testtheta_sta* 1000);		//2=��ӽǶ?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 3 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.ab* 10);		//3=�����˲�AB�ߵ�ѹ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 4 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.bc* 10);		//4=�����˲�BC�ߵ�ѹ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 5 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_U.ab* 10);		//5=�����˲�AB����?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 6 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_U.bc* 10);		//6=�����˲�BC�ߵ�ѹ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 7 + BANK_RAMDATA_POS)) = 1100;								//7=�м�ֱ����ѹ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 8 + BANK_RAMDATA_POS)) = (int16)(testtheta+400);			//8=����d���ѹ�����˲�ǰ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 9 + BANK_RAMDATA_POS)) = (int16)(testtheta+450);			//9=����q���ѹ�����˲�ǰ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 10 + BANK_RAMDATA_POS)) = (int16)(testtheta+500);			//10=����A�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 11 + BANK_RAMDATA_POS)) = (int16)(testtheta+550);			//11=����B���?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(testtheta+600);			//12=����C�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 13 + BANK_RAMDATA_POS)) = (int16)(testtheta+650);			//13=����A�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 14 + BANK_RAMDATA_POS)) = (int16)(testtheta+700);			//14=����B�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 15 + BANK_RAMDATA_POS)) = (int16)(testtheta+750);			//15=����C�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 16 + BANK_RAMDATA_POS)) = (int16)(testtheta+800);			//16=����ʵʱƵ��
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 17 + BANK_RAMDATA_POS)) = (int16)(testtheta+850);			//17=���ת��
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 18 + BANK_RAMDATA_POS)) = (int16)(testtheta+900);			//18=�����ѹ�����
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(testtheta+950);			//19=����d����������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(testtheta+1000);			//20=����q���������?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(testtheta+1050);			//21=����������ŵ���
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(testtheta+1100);			//22=�����������ָ�?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(testtheta+1150);			//23=����d����������
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(testtheta+1200);			//24=����q����������

			*(BANK_RAMEND) = BANK_RAMDATA_POS;																//��ǰ���ݴ洢λ��ת��

			BANK_RAMDATA_POS++;
			if(BANK_RAMDATA_POS >= RAM_BIAS)  BANK_RAMDATA_POS=0;		
			
			M_ClrFlag(SL_RAMBANKSAVE);												//��ʾ�������ݴ���ˢ�±�־
		}
		else	M_SetFlag(SL_RAMBANKSAVE);											//��ʾ�������ݴ��������־

//--------------------------------External RAM Data Read-----------------------------------------
}  

void ERROR_Datasave(void)	//testPCmodbus
{
//--------------------------------External RAM Data Save-----------------------------------------
	if(M_ChkFlag(SL_PC_ERRDATACLR)!=0)						//PCָ���ⲿRAM�����ͷ�
	{
		M_ClrFlag(SL_RAMERRORSAVE);						
		*(ERROR_RAMEND + 1) = 0x0000;
		M_ClrFlag(SL_TRIG_ERRDSAVE);
		M_ClrFlag(SL_PC_ERRDATACLR);						//�ͷ�����/��λ���,�Զ������λָ��
	} 

  	if(*(ERROR_RAMEND + 1) != 0x55AA)
  	{
		if(M_ChkFlag(SL_TRIG_ERRDSAVE)==0)  								//û�й���	
			MAIN_LOOP.cnt_datasave=0;
		else if(M_ChkCounter(MAIN_LOOP.cnt_datasave,DELAY_DATASAVE)>=0)	//���Ϸ�������ʱ0.5s��ֹ��ͼ
		{
			M_SetFlag(SL_RAMERRORSAVE);
			M_ClrFlag(SL_TRIG_ERRDSAVE);
			*(ERROR_RAMEND + 1) = 0x55AA;			
		}

		if(M_ChkFlag(SL_RAMERRORSAVE)==0)  										//�й���֮����ʱ2s��ֹͣ���ݴ洢
		{
			*(ERROR_RAMSTART+ ERROR_RAMDATA_POS) = 1100;														//0=test
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 1 + ERROR_RAMDATA_POS)) = (int16)(testtheta* 1000);			//1=NPR����Ƕ�
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 2 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.ab* 10);		//2=NPR-A����
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 3 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.bc* 10);		//3=NPR-B����
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 4 + ERROR_RAMDATA_POS)) = (int16)(testtheta+400);				//4=NPR-C��
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 5 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.ab* 10);		//5=������ѹVab
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 6 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.bc* 10);		//6=������ѹVbc
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 7 + ERROR_RAMDATA_POS)) = (int16)(testtheta+700);				//7=��ѹd�����ed�˲�ǰ
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 8 + ERROR_RAMDATA_POS)) = (int16)(testtheta+800);				//8=��ѹq�����eq�˲�ǰ
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 9 + ERROR_RAMDATA_POS)) = (int16)(testtheta+900);				//9=��ѹd�����ed�˲���
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 10 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1000);			//10=��ѹq�����eq�˲���
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 11 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1100);			//11=����Idָ��
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 12 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1200);			//12=����Id����
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 13 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1300);			//13=����IdPI���
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 14 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1400);			//14=����Ud���
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 15 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1500);			//15=����Iqָ��
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 16 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1600);			//16=����Iq����
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 17 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1700);			//17=����IqPI���
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 18 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1800);			//18=����Uq���
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 19 + ERROR_RAMDATA_POS)) = (int16)(testtheta_sta* 1000);		//19=MPR����Ƕ�
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 20 + ERROR_RAMDATA_POS)) = (int16)(testtheta+2000);			//20=MPR-A����
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 21 + ERROR_RAMDATA_POS)) = (int16)(testtheta+2100);			//21=MPR-B����
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 22 + ERROR_RAMDATA_POS)) = (int16)(testtheta+2200);			//22=MPR-C����
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 23 + ERROR_RAMDATA_POS)) = (int16)(testtheta+2300);			//23=ʵ��ʵʱ��Ƶ
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 24 + ERROR_RAMDATA_POS)) = (int16)(testtheta+2400);			//24=���ת��

			*(ERROR_RAMEND) = ERROR_RAMDATA_POS;																//��ǰ���ݴ洢λ��ת��

			ERROR_RAMDATA_POS++;
			if(ERROR_RAMDATA_POS >= RAM_BIAS)  ERROR_RAMDATA_POS=0;		
		}
  	}

//--------------------------------External RAM Data Read-----------------------------------------
}  

*/
/*********************************************************************************************************
** ��������: Draw
** ������?: ���ͼ�?
** �䡡��:
** �䡡��:        
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸�?:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
/*void Draw(void)
{
	if(M_ChkFlag(SL_DRAW)!=0)
	{
		if(draw<200)
		{
	//		data1[draw]=ADFINAL.ubc1;
	//		data2[draw]=UN.d;
	//		data3[draw]=ADFINAL.uab1;
	//		data4[draw]=UN.q;
	//		data1[draw]=UN.theta;
	//		data2[draw]=UN.theta2;
		//		data1[draw]=UN.d;
		//	data2[draw]=UN.q;
			data1[draw]=ADFINAL.ua1;
			data2[draw]=ADFINAL.ub1;
			data3[draw]=US1.a;
			data4[draw]=US1.b;

		//	data1[draw]=Iq2_LOOP.fb;
		//	data2[draw]=Iq2_LOOP.out;
		//	data3[draw]=EvbRegs.CMPR6;
		//	data4[draw]=EN.q;
			draw++;
		}
	}
	else	draw=0;
} 
*/

//===========================================================================
