/****************************************Copyright (c)**************************************************
**                       		     ��	��	��	ͨ	��	ѧ
**                                         ��������ѧԺ
**                                         614ʵ����
**
**                              
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: user_main.c
**��   ��   ��: 
**����޸�����: 
**��        ��: 1.5MW˫���������������������----���Ʒ糡
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
#include "IQmathLib.h"
#include "qmath.h"
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
/*--------------------�Զ���ͷ�ļ�----------------------------*/
#include "user_header.h"  						//������������
#include "user_macro.h"							//�꺯��
#include "user_database.h"						//���ݿ�   
#include "user_interface.c"						//�ӿڲ�
#include "user_work.c"							//��������
#include "math.h"

/*-----------------------�ж�����-----------------------------*/
interrupt void CpuTimer0Isr(void);				//����ʱ���ж�			
interrupt void EPWM1_TZ1_Isr(void);			//TZ1�ж�
interrupt void EPWM2_TZ2_Isr(void);			//TZ2�ж�
/*-----------------------��������-----------------------------*/

void Protect(void);
void Scout(void);
void et_relay_N(void);
void et_relay_M(void);
void CntCtrl(void);
void SysCtrl(void);
void Bank(void);
void Display(void);
void Datasave(void);	//20091109atzy

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

//	InitRtimer();										//ʵʱʱ�ӳ�ʼ��ֻ��оƬ��Ҫ��ʼ��ʱ�Ž���
//-------------------------------�жϵ�ַ��ʼ��-----------------------------
	EALLOW;  
	PieVectTable.TINT0       = &CpuTimer0Isr;		  //��ʱ��T0�����ж�
	PieVectTable.EPWM1_TZINT = &EPWM1_TZ1_Isr;      //TZ1 ���ʱ����ж� 
	PieVectTable.EPWM2_TZINT = &EPWM2_TZ2_Isr;      //TZ2 ���ʱ����ж� 

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
		Sci_canopenrx();					//ϵͳ�����ȡ


//-----------------------------һ��ѭ��5ms����һ��----------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt1,DELAY_MAIN1)>=0)		
		{
			MAIN_LOOP.cnt1=0;				//��ռ�����
			Input();						//ϵͳ�����ȡ
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
		}
//-----------------------------����ѭ��1000ms����һ��---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt6,DELAY_MAIN6)>=0)
		{
			MAIN_LOOP.cnt6=0;
//			M_NotFlag(SL_DISPLAY4);

			if(M_ChkFlag(SL_CODEOK)!=0)		
				RtRead();							//��eeprom�����������//��ȡʵʱʱ��,�ܺ�ʱ��Ҫ13ms.20090801,CPC				
										
			if(M_ChkCounter(MAIN_LOOP.cnt_senszfstdy,DELAY_SENSZFSTDY)>=0)
				M_SetFlag(SL_SENSZFSTDY);   		//�ӳ�ʱ�䵽������Ư�˲��ȶ���־λ


		}
	}  					
} 

//    	_OUT3_TEST |= 0x0001;   
//		*OUT3_ADDR = _OUT3_TEST;
//   	 _OUT3_TEST &= 0xFFFE;   
//		*OUT3_ADDR = _OUT3_TEST; 
//   	 _OUT3_TEST |= 0x0002;   
//		*OUT3_ADDR = _OUT3_TEST; 
//    	 _OUT3_TEST &= 0xFFFD;
//		*OUT3_ADDR = _OUT3_TEST; 
//		if((_OUT3_TEST & 0x0001)!=0)  _OUT3_TEST&=0xFFFE;
//		else      	       _OUT3_TEST|=0x0001; 
//		if((_OUT3_TEST & 0x0002)!=0)  _OUT3_TEST&=0xFFFD;
//     	else      	       _OUT3_TEST|=0x0002; 
/*********************************************************************************************************
** ��������: CpuTimer0Isr
** ��������: ����ʱ�������ж�(0.04ms)
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
    Uint16 i;
    
    M_SetFlag(SL_PHASEA);            //����CPUռ����,����DSP����T1����
    *OUT3_ADDR = _OUT3_DATA;
		
	if(M_ChkFlag(SL_IN1_CBSTS)==0)   //�ں�����ǰ��ѹ�������Ӻ��� cap5-ubc   cap6-uab
	{
	  PhaseOrderChk();        
	} 
 	QepEncodPos(); 					


    CapQepCtrl();  		  			//����ѹ��λ����ѹƵ�ʣ������������任�ĽǶ�
   	Ad8364Ctrl();             		//�д������ͻ�������AD������ת��

    NPR_CONTROL();            		//��������������㷨
    MPR_CONTROL();            		//��������������㷨

	et_relay_N();					//200usִ��һ��
	et_relay_M();					//200usִ��һ��

	testtheta = testtheta + 1;		//hoteltest20091109
	if(testtheta>=3721)	testtheta= -1879;//hoteltest20091109

//------------------������ѹ����20091027atzy-----------------------------------
  if(M_ChkFlag(SL_CHARGEOK)!=0)
  {
//70%
	if((TRS_NGS_U.dflt2<=394) && (_BA_UAB2<65535) && (M_ChkFlag(SL_GRIDLOSE70)==0))			//ed=563V; *0.7=394V
	{
		_BA_UAB2 = _BA_UAB2+1;
		M_SetFlag(SL_DISPLAY6);		//��ָʾ
	}
	else if(_BA_UAB2!=0)	M_SetFlag(SL_GRIDLOSE70);

//50%
	if((TRS_NGS_U.dflt2<=282) && (_BA_UBC2<65535) && (M_ChkFlag(SL_GRIDLOSE50)==0))			//ed=563V; *0.5=282V
	{
		_BA_UBC2 = _BA_UBC2+1;
		M_SetFlag(SL_DISPLAY7);		//��ָʾ
	}
	else if(_BA_UBC2!=0)	M_SetFlag(SL_GRIDLOSE50);

//30%
	if((TRS_NGS_U.dflt2<=169) && (_BA_STAUABD<65535) && (M_ChkFlag(SL_GRIDLOSE30)==0))			//ed=563V; *0.3=169V
	{
		_BA_STAUABD = _BA_STAUABD+1;
	}
	else if(_BA_STAUABD!=0)	M_SetFlag(SL_GRIDLOSE30);

//15%
	if((TRS_NGS_U.dflt2<=84.5) && (_BA_STAUBCD<65535) && (M_ChkFlag(SL_GRIDLOSE15)==0))			//ed=563V; *0.15=84.5V
	{
		_BA_STAUBCD = _BA_STAUBCD+1;
	}
	else if(_BA_STAUBCD!=0)	M_SetFlag(SL_GRIDLOSE15);
  }


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
			break;
		}
		case 1:
		{	
			PwmDrive();	 		   //����������  				
		    CntCtrl(); 				//����1ms��һ��������ʱ���ж�����Ϊ200us
			Datasave();				//0.4msһ��20091109
			break;
		}
		case 2:
		{
			Protect();			   //protect calculation		
			Scout();			   //���ϱ���
			break;
		}
		case 3:
		{	
			PwmDrive();			   //����������				
			RunCtrl();			   //��������
			Datasave();				//0.4msһ��20091109
			break;
		}
		case 4:
		{	
			Protect();			   //protect calculation	
			Scout();			   //���ϱ���
			break;
		}
		case 5:
		{	
			PwmDrive();			   //����������				
			Display();
			Datasave();				//0.4msһ��20091109
			break;
		}
		case 6:
		{	
			CntCtrl(); 	           //������1ms��һ��������ʱ���ж�����Ϊ200us
			Protect();			   //protect calculation	
			Scout();			   //���ϱ���
			break;
		}
		case 7:
		{
			PwmDrive();			   //����������				
			Datasave();				//0.4msһ��20091109
			break;
		}
		case 8:
		{	
			Protect();												//slow AD, protect calculation	
			Scout();												//���ϱ���
			break;
		}
		case 9:
		{	
			PwmDrive();			   //����������				
			RunCtrl();			   //��������
			Datasave();				//0.4msһ��20091109
			break;
		}
//--------------------------------------------------------------------------------------------------
		default:
			break;
	}

//--------------------DAת��------------------------------------------------------------------------
//--------NPR--------
//	zys1 = (int16)(PI_NPR_U.error 	    * 20.0)	+ 2048;
//	zys2 = (int16)(PI_NPR_U.out	        * 2.0) + 2048;
//	zys3 = (int16)(PI_NPR_Id.feedback   * 2.0) + 2048;
//	zys4 = (int16)(PI_NPR_Id.out        * 2.0) + 2048;

//	zys1 = (int16)(TRS_NPR_U.q 	        * 2.0)	+ 2048;
//	zys2 = (int16)(PI_NPR_Iq.reference	* 2.0) + 2048;
//	zys3 = (int16)(PI_NPR_Iq.feedback   * 2.0) + 2048;
//	zys4 = (int16)(PI_NPR_Iq.out        * 2.0) + 2048;

//--------MPR--------
//	zys1 = (int16)(TRS_MPR_U.d          * 5.0) + 2048;
//	zys2 = (int16)(TRS_MPR_I.dflt	    * 5.0) + 2048;
//	zys3 = (int16)(CAP4.omigaslp * MPR_Lr * DM_imrd * 5.0) + 2048;
//	zys4 = (int16)(TRS_MPR_U.q          * 5.0) + 2048;

//	zys1 = (int16)(TRS_MPR_U.d     	    * 2.0) + 2048;
//	zys2 = (int16)(PI_MPR_Id.reference	* 2.0) + 2048;
//	zys3 = (int16)(PI_MPR_Id.feedback   * 2.0) + 2048;
//	zys4 = (int16)(PI_MPR_Id.out        * 2.0) + 2048;

//	zys1 = (int16)(TRS_MPR_U.q     	    * 2.0) + 2048;
//	zys2 = (int16)(PI_MPR_Iq.reference	* 2.0) + 2048;
//	zys3 = (int16)(PI_MPR_Iq.feedback   * 2.0) + 2048;
//	zys4 = (int16)(PI_MPR_Iq.out        * 2.0) + 2048;

//--------Theta--------
//	zys1 = (int16)(CAP4.nprtrstheta     * 200.0) + 2048;   //NPR
//	zys2 = (int16)(QEPDATA.rotposdisp	* 200.0) + 2048;   //rotor
//	zys3 = (int16)(CAP4.mprtrstheta     * 200.0) + 2048;   //MPR
//	zys4 = (int16)(CAP4.mpratitheta     * 200.0) + 2048;   //MPR anti

//--------STAC_SYN TEST--------
	zys1 = (int16)(TRS_NGS_U.dflt2     * 2.0) + 2048;  //edС�˲��������ѹ����20091027atzy
	zys2 = (int16)(AD_OUT_STA_U.bc	   * 2.0) + 2048;   //
	zys3 = (int16)(AD_OUT_STAD_U.bc    * 20.0)+ 2048;   //
	zys4 = (int16)(CAP4.mprtrstheta    * 200.0) + 2048;    //
//--------��ʾ�˲���ת��-------
//    zys1 = (int16)(PRO.speedflt     * 1.0) + 2048;  //ת��

	if		(zys1>4095)  	zys1 = 4095;
	else if	(zys1<0)		zys1 = 0;

	if		(zys2>4095)  	zys2 = 4095;
	else if	(zys2<0)		zys2 = 0;

	if		(zys3>4095)  	zys3 = 4095;
	else if	(zys3<0)		zys3 = 0;

	if		(zys4>4095)  	zys4 = 4095;
	else if	(zys4<0)		zys4 = 0;
		
	*DA_ADD0 =	zys1;
	*DA_ADD1 =	zys2;
	*DA_ADD2 =	zys3;
	*DA_ADD3 =	zys4;


//--Ӧ���ж�20091109atzy
//	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	

//canopen
//-----------------------------canopen����һ��----------------------------
	if(M_ChkCounter(MAIN_LOOP.canopen_tx,DELAY_CANOPENTX)>=0)    //13ms		
	{
		MAIN_LOOP.canopen_tx=0;				//��ռ�����
		Sci_canopentx();
		M_SetFlag(SL_CANOPENTX);
//			if((_OUT3_TEST & 0x0001)!=0)  	_OUT3_TEST&=0xFFFE;
//			else      	       				_OUT3_TEST|=0x0001;	
//			*OUT3_ADDR = _OUT3_TEST;
	} 

	if(M_ChkFlag(SL_CANOPENTX)!=0)
	{		
	 	if(ScibRegs.SCIFFTX.bit.TXFFST <= 12)						
		{
			for(i=SCICANOPENTXNUM-2;i<SCICANOPENTXNUM;i++)
				ScibRegs.SCITXBUF=SCI_canopen.txb[i];
			M_ClrFlag(SL_CANOPENTX);
		}
	}
	else
	{
		if(ScibRegs.SCIFFTX.bit.TXFFST == 0)						//�������?
		{
			if(ScibRegs.SCICTL2.bit.TXEMPTY==1)	
			{
				ScibRegs.SCIFFTX.bit.TXFIFOXRESET=1;	// Re-enable TX FIFO operation
			}
		}
	}

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
	_BA_EPGRID++;				//20100506��ʱ�޸� ��¼Ӳ���ϴ���			
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
** ��������: Protect
** ��������: ����ֵ����
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
void Protect(void)
{
	float temp;
	
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

//---------------------------�����ߵ�ѹ��Чֵ����----------------------------
    PRO.NPR_uab= MEAN_DATA.uab1 * 1.110721;                      //����Uab��Чֵ��λV rms=mean*PAI/(2*sqrt(2)) 
	PRO.NPR_ubc= MEAN_DATA.ubc1 * 1.110721;                      //����Ubc��Чֵ��λV 1.110721=PAI * SQRT2 / 4
	PRO.GID_uab = MEAN_DATA.uab * 1.110721;        //20091026atzy ������Uab��Чֵ��λV ��λV  rms=mean*PAI/(2*sqrt(2)) �� 1.110721=PAI * SQRT2 / 4
    PRO.GID_ubc = MEAN_DATA.ubc * 1.110721;        //20091026atzy ������Uab��Чֵ��λV ��λV  rms=mean*PAI/(2*sqrt(2)) �� 1.110721=PAI * SQRT2 / 4

//---------------------------���Ӳ��ߵ�ѹ��Чֵ����----------------------------
    PRO.STA_uab= MEAN_DATA.uab2 * 1.110721;                    //���Uab��Чֵ��λV
	PRO.STA_ubc= MEAN_DATA.ubc2 * 1.110721;                    //�����Ubc��Чֵ��λV

//------------------���ʱ�������ʾֵ����-----------------------------------------
//------------------���Ӳ�dq�任����Ƕ�ֵ--------------------------------------------------------------------
	DIP_STA_I.sintheta = sin(CAP4.nprtrstheta);		//���㶨�Ӳ๦�� dq�任�Ƕ�
	DIP_STA_I.costheta = cos(CAP4.nprtrstheta);		//���㶨�Ӳ๦�� dq�任�Ƕ�
	DIP_STA_U.sintheta = DIP_STA_I.sintheta;
	DIP_STA_U.costheta = DIP_STA_I.costheta; 

//------------------���Ӳ����dqֵ--------------------------------------------------------------------
   	DIP_STA_I.a     = AD_OUT_STA_I.a;
   	DIP_STA_I.b     = AD_OUT_STA_I.b;
   	DIP_STA_I.c     = AD_OUT_STA_I.c;
	Transform_3s_2s_2r(&DIP_STA_I);
	DataFilter(0.99,&DIP_STA_I.dflt,DIP_STA_I.d); 	//���Ӳ��������ֵ�˲�,Ts=200us,fh=88Hz,�˵�����Ƶ�ʴ�
	DataFilter(0.99,&DIP_STA_I.qflt,DIP_STA_I.q); 	//���Ӳ��������ֵ�˲��� Ts=200us,fh=88Hz,�˵�����Ƶ�ʴ�

//------------------���Ӳ��ѹdqֵ--------------------------------------------------------------------
   	DIP_STA_U.a     = AD_OUT_NGS_U.a;    //������ѹ��Ϊ���ӵ�ѹ����Ϊ����V-LEM������ѹ�����˴����˲�
   	DIP_STA_U.b     = AD_OUT_NGS_U.b;
   	DIP_STA_U.c     = AD_OUT_NGS_U.c;
	Transform_3s_2s_2r(&DIP_STA_U);
	DataFilter(0.99,&DIP_STA_U.dflt,DIP_STA_U.d); 	//���Ӳ��ѹ����ֵ�˲��� Ts=200us,fh=88Hz,�˵�����Ƶ�ʴ�
	DataFilter(0.99,&DIP_STA_U.qflt,DIP_STA_U.q); 	//���Ӳ��ѹ����ֵ�˲��� Ts=200us,fh=88Hz,�˵�����Ƶ�ʴ�

//------------------------�������ѹ��Чֵ��ʾֵ����(Ϊ���ʼ���)--------------------------------------------------
    PRO.sta_uar = MEAN_DATA.ua1 * 1.110721;         //��λV ������ѹ��Ϊ���ӵ�ѹ����Ϊ����V-LEM������ѹ�����˴����˲�
	PRO.sta_ubr = MEAN_DATA.ub1 * 1.110721;
	PRO.sta_ucr = MEAN_DATA.uc1 * 1.110721;

//------------------------�����������Чֵ��ʾֵ����(Ϊ���ʼ���)--------------------------------------------------
    PRO.sta_iar = MEAN_DATA.ia3 * 1.110721;			//��λA
	PRO.sta_ibr = MEAN_DATA.ib3 * 1.110721;
	PRO.sta_icr = MEAN_DATA.ic3 * 1.110721;

//------------------���Ӳ��й����޹�����ֵ--------------------------------------------------------------------
    PRO.Psactive   = 1.5 * SQRT3 * (DIP_STA_U.dflt * DIP_STA_I.dflt + DIP_STA_U.qflt * DIP_STA_I.qflt); 
    PRO.Psreactive = 1.5 * SQRT3 * (DIP_STA_U.qflt * DIP_STA_I.dflt - DIP_STA_U.dflt * DIP_STA_I.qflt); 
	PRO.Ps		   = SQRT3 * (PRO.sta_uar * PRO.sta_iar + PRO.sta_ubr * PRO.sta_ibr + PRO.sta_ucr * PRO.sta_icr);//20091007

//------------------------���ಢ��������Чֵ��ʾֵ����----------------------------------------------
    PRO.npr_iar = MEAN_DATA.ia1 * 1.110721;     			 //1.110721=PAI * SQRT2 / 4  	//��λA
	PRO.npr_ibr = MEAN_DATA.ib1 * 1.110721;
	PRO.npr_icr = MEAN_DATA.ic1 * 1.110721;

//------------------------�������ѹ��Чֵ��ʾֵ����----------------------------------------------
    PRO.npr_uar = MEAN_DATA.ua1 * 1.110721;      			//1.110721=PAI * SQRT2 / 4  //��λA
	PRO.npr_ubr = MEAN_DATA.ub1 * 1.110721;
	PRO.npr_ucr = MEAN_DATA.uc1 * 1.110721;

//------------------�����й����޹�����ֵ--------------------------------------------------------------------
 	PRO.Pnactive   = Pnactive;
	PRO.Pnreactive = Pnreactive;
    PRO.Pn         = PRO.npr_iar * PRO.npr_uar + PRO.npr_ibr * PRO.npr_ubr + PRO.npr_icr * PRO.npr_ucr;

//------------------------------���Ӳ�������ܲ���������ʾ-----------------------------------------------------
	PRO.Pgactive   = PRO.Psactive   + PRO.Pnactive;
	PRO.Pgreactive = PRO.Psreactive + PRO.Pnreactive;

    if(CAP4.omigaslp >= 0)	 PRO.Pg = PRO.Ps - PRO.Pn;
	else 					 PRO.Pg = PRO.Ps + PRO.Pn;

//---------------------------����ͻ������¶�ֵ----------------------------
    PRO.NPR_TLOV= AMUX.Lac_temp;                    //�������¶�
	PRO.MPR_TLOV= AMUX.Ldudt_temp;                  //�������¶�

//--------------------------- ���Ӳ��ߵ�������ֵ----------------------------
	PRO.STA_iac = MEAN_DATA.iac3 * 1.110721  * 100/ STA_IACN;
	PRO.STA_iba = MEAN_DATA.iba3 * 1.110721  * 100/ STA_IACN;

//---------------------------ת�ٱ���ֵ����----------------------------------
	PRO.speed   = 9.5492966 * QEPDATA.omigamec;		//��λ��ת/�֣�n=60*w/2pai=*w
//	PRO.speed   = 1200;		//��λ��ת/�֣�n=60*w/2pai=*w

//	if(_stdby05!=0)   	DataFilter(_stdby01,&PRO.speedflt,PRO.speed); 	//����ת�ٷ���ֵ�˲��� Ts=200us,Tr=248ms 20090815
//	else   				PRO.speedflt= PRO.speed;         //20090816test

//	DataFilter(0.8,&PRO.speedflt,PRO.speed); 				//����ת�ٷ���ֵ�˲��� Ts=200us,Tr=5ms 20091021atzy֮ǰ123ms�˲�̫����
	DataFilter(0.45,&PRO.speedflt,PRO.speed); 				//����ת�ٷ���ֵ�˲��� Ts=200us,Tr=1ms 20111116
	
	if(PRO.speedflt>700)		M_SetFlag(SL_SPEED_HIGH);	//�����ת����   20091021atzy 650��Ϊ700���Ӵ��ͻ�
    else if(PRO.speedflt<600)	M_ClrFlag(SL_SPEED_HIGH);   //650r/min���ܷ�PRD=845.��Ƶ��POSLAT=887.

    if((PRO.speedflt > _SC_MSPEED1) && (PRO.speedflt < _SC_MSPEED2)) 	 M_SetFlag(SL_SPEED_IN_RANGE);  //20091021atzy	
	else													 M_ClrFlag(SL_SPEED_IN_RANGE);

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

  float temp_pgactive,temp_pgreactive;
//------------------�����ӳ����ϱ�����Ϣ���ж���TAB_MSG����һ��-------------------------------------
	if(M_ChkFlag(SL_POWERON)==0)										//������ɹ����������
	{	
//---------------------------------TZ1�жϱ���------------------------------------------------------
		if(M_ChkFlag(SL_PDPINTA)!=0)	
		{	
			if(M_ChkFlag(SL_IN2_IOVA1)!=0)			M_SetFlag(SL_HIA1);		//����A��SKiiP����
			else if(M_ChkFlag(SL_IN2_IOVB1)!=0)	   	M_SetFlag(SL_HIB1);		//����B��SKiiP����
		    else if(M_ChkFlag(SL_IN2_IOVC1)!=0)	   	M_SetFlag(SL_HIC1);		//����C��SKiiP����
			else if(M_ChkFlag(SL_IN3_VDCOV)!=0)   	M_SetFlag(SL_HUDCOV);	//ֱ�����ѹ����
			else if(M_ChkFlag(SL_IN3_NPRIOV)!=0)  	M_SetFlag(SL_HIACOV1);	//����Ӳ����������
			else if(M_ChkFlag(SL_PDPASERIES)!=0)	M_ClrFlag(SL_PDPINTA);	//����������������	20091107atzy						
			else									M_SetFlag(SL_PDPASERIES);//����Ӳ������,CPLDû�д浽����
			
			M_SetFlag(SL_ERRDATASAVE);	//�����ⲿRAM����ת��20091109atzy
		}
		else
		{
			M_ClrFlag(SL_HIA1);   	    									//����ϱ�־
			M_ClrFlag(SL_HIB1);
			M_ClrFlag(SL_HIC1);
			M_ClrFlag(SL_HUDCOV);
			M_ClrFlag(SL_HIACOV1);  			
			M_ClrFlag(SL_PDPASERIES);  			
		}

//---------------------------------TZ2�жϱ���-------------------------------------------------------
		if(M_ChkFlag(SL_PDPINTB)!=0)	
		{	
			if(M_ChkFlag(SL_IN2_IOVA2)!=0)			M_SetFlag(SL_HIA2);			//�����A��SKiiP����
			else if(M_ChkFlag(SL_IN2_IOVB2)!=0)	    M_SetFlag(SL_HIB2);			//�����B��SKiiP����
		    else if(M_ChkFlag(SL_IN2_IOVC2)!=0)	    M_SetFlag(SL_HIC2);			//�����C��SKiiP����
		    else if(M_ChkFlag(SL_IN3_MPRIOV)!=0)    M_SetFlag(SL_HIACOV2);		//�����Ӳ����������
			else if(M_ChkFlag(SL_PDPBSERIES)!=0)	M_ClrFlag(SL_PDPINTB);		//��������������	20091107atzy						
			else									M_SetFlag(SL_PDPBSERIES);	//����Ӳ������,CPLDû�д浽����

//			M_SetFlag(SL_ERRDATASAVE);	//�����ⲿRAM����ת��20091109atzy
		}
		else
		{
			M_ClrFlag(SL_HIA2);    											//����ϱ�־
			M_ClrFlag(SL_HIB2);	   		
			M_ClrFlag(SL_HIC2);	    
			M_ClrFlag(SL_HIACOV2);
			M_ClrFlag(SL_PDPBSERIES);	
		}

//---------------------------------E-STOP����-------------------------------------------------------
		if(M_ChkFlag(SL_IN1_EXESTOP)!=0)								//�ⲿ��ͣ����  �������źţ��糡û��								
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_estop,DELAY_ESTOP)>=0)   M_SetFlag(SL_ESTOP);		//����ֹͣ�ӳ�ʱ�䵽��
			else M_ClrFlag(SL_ESTOP);									//���־λ
		}
		else 
		{
			MAIN_LOOP.cnt_estop=0;										//�嶨ʱ��
			M_ClrFlag(SL_ESTOP);
		}  

//---------------------------------�ⲿӲ�����ϱ���-------------------------------------------------
		if((M_ChkFlag(SL_IN1_EXFAULTOK)==0)||(M_ChkFlag(SL_IN1_MIANFAN)==0))	//�ⲿ���϶������߹��������� new							
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
	    if(_CANOPER!=0) 											//�ϵ���ѡ��CAN���ƲŽ���ͨѶ����
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_canfault,DELAY_CANFAULT)>=0)		//10s  20090816
		  	{
				if(M_ChkCounter(SCI_canopen.cnt_heartbeat,DELAY_CANOPENOVER)>0)
				{                                   						
			   		 M_SetFlag(SL_CANOPENOVER);
			   		 SCI_canopen.rx_controlword=0;
					 SCI_canopen.rx_torque=0;
					 SCI_canopen.rx_angle=0;
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
	    	    
//-----------------------���Ϻ�բ/��բ/����Ͽ�����---------------------------------------
	    if((M_ChkFlag(CL_CB)!=0 && M_ChkFlag(SL_IN1_CBSTS)==0)||(M_ChkFlag(CL_CBENGSTRG)==0 && M_ChkFlag(SL_IN1_CBSTS)!=0)||(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(CL_CBENGSTRG)!=0 && M_ChkFlag(SL_CBCLOSED)!=0 && M_ChkFlag(SL_IN3_CBTRIP)==0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_cberror,DELAY_CBERROR)>=0)	M_SetFlag(SL_ERROR_CB);	  //2s  
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

//----------------------Ԥ������--------------------------------------- 
	    if((M_ChkFlag(CL_PRE)!=0)&&(M_ChkFlag(CL_MAINK)==0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_preerror,DELAY_PREERROR)>=0)	M_SetFlag(SL_ERROR_PRE);  //15s	
			else M_ClrFlag(SL_ERROR_PRE);								 //���־λ
		}
		else 
		{
			MAIN_LOOP.cnt_preerror=0;                                    //�嶨ʱ��
			M_ClrFlag(SL_ERROR_PRE);
		}  
 
//-----------------------ENCODFAULT����(Qep���,����������)-----------------------------------------
	    if(((M_ChkFlag(SL_QEPPCO)!=0)||(M_ChkFlag(SL_QCAPDISTURB)!=0)||(M_ChkFlag(SL_QEPZDISTRUB)!=0))&&(M_ChkFlag(SL_OCS_EIN)!=0)&&(M_ChkFlag(SL_QCAPSPDIN)!=0))	//QEP�������QCAP��QEP Z�ź��ܸ��Ź���  20090804��cpc							
//	    if((M_ChkFlag(SL_QEPPCO)!=0)||(M_ChkFlag(SL_QCAPDISTURB)!=0)||(M_ChkFlag(SL_QEPZDISTRUB)!=0))	//QEP�������QCAP��QEP Z�ź��ܸ��Ź���  20090804��cpc	//201105atzuoyun						
     	{
			M_SetFlag(SL_ENCODFAULT);				     			    //�ñ�־λ
		}
		else 
		{
			M_ClrFlag(SL_ENCODFAULT);									//���־λ
		} 

//-----------------------�������ת�ٷ�Χ�ж�-----------------------------------------------------
	    if(M_ChkFlag(SL_CHARGEOK)!=0)									//Ԥ���OK��Ž���ת�ٳ�����Χ�ж� 20090815
		{
			if((M_ChkFlag(SL_MSPOUT)==0)&& ((PRO.speedflt<_SC_MSPEED1)||(PRO.speedflt>_SC_MSPEED2)))  //�Ӹ���ʱ�ж�20090817
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_speedout,DELAY_SPEEDOUT)>=0)	M_SetFlag(SL_MSPOUT);	//50ms 20091022atzy									//�ó���ת�ٷ����־λ
			}
			else  MAIN_LOOP.cnt_speedout=0;
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
			M_SetFlag(SL_SIAC1);									//�����������־λ
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
//-------------------------�м�ֱ����ѹ���Ƿѹ�ж�------�����⣡-------------------------------------------
	    if((M_ChkFlag(SL_CHARGEOK)!=0)&&(M_ChkFlag(SL_NPR_PWMOUT)!=0))//Ԥ�����ɺ�Ž��Ƿѹ�ж�
		{
			if((M_ChkFlag(SL_SUDCLV)==0)&&(PRO.udc<_SC_UDCLV))	//�ֳ���ʱ��Ϊ0��
			{
				M_SetFlag(SL_SUDCLV);									//֮ǰû��Ƿѹ���˿̼���Ƿ����Ƿѹ��־
			}	
			else if((M_ChkFlag(SL_SUDCLV)!=0)&&(PRO.udc>=(_SC_UDCLV+SC_UDC_HW)))	
			{
				M_ClrFlag(SL_SUDCLV);									//֮ǰǷѹ���˿̼�������(Ƿѹֵ�ӻز�)����Ƿѹ��־
			}		
		}
		else	M_ClrFlag(SL_SUDCLV);	//20100511		

//------------------------�м�ֱ����ѹ�����ѹ�ж�--------------------------------------------------
		if((M_ChkFlag(SL_SUDCOV)==0)&&(PRO.udc>_SC_UDCOV))	                   
		{
			M_SetFlag(SL_SUDCOV);	 	//
//			M_SetFlag(SL_ERRDATASAVE);	//�����ⲿRAM����ת��20091109atzy
		}
		else if((M_ChkFlag(SL_SUDCOV)!=0)&&(PRO.udc<=(_SC_UDCOV-SC_UDC_HW)))   M_ClrFlag(SL_SUDCOV);  //֮ǰ��ѹ���˿̼����ڹ�ѹֵ���ز�	

//------------------------�м�ֱ����ѹ�����ѹ�ж�(������ѹ����) --------------------------------------------------
//		if((M_ChkFlag(SL_SUDCOVH)==0)&&(PRO.udc>_SC_UDCOVH))	                   M_SetFlag(SL_SUDCOVH);	 //֮ǰû�й�ѹ���˿̼�����ѹ����Ƿѹ��־

//		else if((M_ChkFlag(SL_SUDCOVH)!=0)&&(PRO.udc<=(_SC_UDCOVH-SC_UDC_HW)))     M_ClrFlag(SL_SUDCOVH);  //֮ǰ��ѹ���˿̼����ڹ�ѹֵ���ز�	

 
//-----------------------��ѹǷѹ����ж�(��ѹ��Чֵ)-----------------------------------------------
	   if(M_ChkCounter(MAIN_LOOP.cnt_uaclv1,DELAY_UACLV1)>0)	//10s �ϵ���ʱ�ж�������Ƿѹ����
	   {
		if(M_ChkFlag(SL_UACLV1)==0)
		{
			if((PRO.GID_uab<_SC_UACLV1)||(PRO.GID_ubc<_SC_UACLV1))	M_SetFlag(SL_UACLV1);  //20091026atzy �����ߵ�ѹ����һ������Ҫ��,��Ƿ��־
		}
		else 
		{
			if((PRO.GID_uab>=(_SC_UACLV1+SC_UAC_HW))&&(PRO.GID_ubc>=(_SC_UACLV1+SC_UAC_HW)))	M_ClrFlag(SL_UACLV1); //20091026atzy �����ߵ�ѹ���ظ�����������Ƿ���־	
		}
	   }					
//----------------------��ѹ��ѹ����ж�(��ѹ��Чֵ)------------------------------------------------
		if(M_ChkFlag(SL_UACOV1)==0)
		{
			if((PRO.NPR_uab>_SC_UACOV1)||(PRO.NPR_ubc>_SC_UACOV1))	 M_SetFlag(SL_UACOV1);

		}
		else
 		{
   			if((PRO.NPR_uab<=(_SC_UACOV1-SC_UAC_HW))&&(PRO.NPR_ubc<=(_SC_UACOV1-SC_UAC_HW)))	M_ClrFlag(SL_UACOV1);

		}

/*
//-----------------------���Ӳ���ѹǷѹ����ж�(��ѹ��Чֵ)-----------------------------------------------
		if(M_ChkFlag(SL_UACLV2)==0)
		{
			if((PRO.STA_uab<_SC_UACLV2)||(PRO.STA_ubc<_SC_UACLV2))	 M_SetFlag(SL_UACLV2);	  //�����ߵ�ѹ����һ������Ҫ��,��Ƿѹ��־

		}
		else 
		{
			if((PRO.STA_uab>=(_SC_UACLV2+SC_UAC_HW))&&(PRO.STA_ubc>=(_SC_UACLV2+SC_UAC_HW)))  M_ClrFlag(SL_UACLV2);		//�����ߵ�ѹ���ظ�����������Ƿѹ��־

		}
						
//----------------------���Ӳ���ѹ��ѹ����ж�(��ѹ��Чֵ)------------------------------------------------
		if(M_ChkFlag(SL_UACOV2)==0)
		{
			if((PRO.STA_uab>_SC_UACOV2)||(PRO.STA_ubc>_SC_UACOV2))	 M_SetFlag(SL_UACOV2);

		}
		else
 		{
   			if((PRO.STA_uab<=(_SC_UACOV2-SC_UAC_HW))&&(PRO.STA_ubc<=(_SC_UACOV2-SC_UAC_HW)))   M_ClrFlag(SL_UACOV2);

		}
*/ 
//------------------------����Ƶ�ʹ���--------------------------------------------------------------
		if(M_ChkFlag(SL_IN1_CBSTS)!=0)     									//���ϱպ����ж���Ƶ���� 20090816
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_cbfreq,DELAY_CBFREQ)>=0)      	//1s
			{
				if(M_ChkFlag(SL_GRDFQE)!=0)									//10����ѹ����(200ms),����Ƶ�ʴ����־
				{
					if(M_ChkCounter(MAIN_LOOP.cnt_freq,DELAY_FREQ)>=0)	 M_SetFlag(SL_FE1);	
				}	
				else
				{
					M_ClrFlag(SL_FE1);									   	//������Ƶ�ʴ����־
					MAIN_LOOP.cnt_freq=0;                                  	//��ʱ��ÿ1ms��1
				}
			}
		}
		else   	
		{
			MAIN_LOOP.cnt_cbfreq=0;	
			M_ClrFlag(SL_FE1);
		}
	
//-----------------------���ӵ��������ж�-----------------------------------------------------------
		if((M_ChkFlag(SL_SIOVST)==0)&&((PRO.STA_iac>_SC_IACOVST)||(PRO.STA_iba>_SC_IACOVST)))
		{
			M_SetFlag(SL_SIOVST);									//�ö��ӵ���������־λ
		}
		else if((M_ChkFlag(SL_SIOVST)!=0)&&(PRO.STA_iac<(_SC_IACOVST-SC_IAC_HW))&&(PRO.STA_iba<(_SC_IACOVST-SC_IAC_HW))) 
		{
			M_ClrFlag(SL_SIOVST);									//�嶨�ӵ���������־λ
		}

//-----------------------�м��ѹ���ȶ�-------------------------------------------------------------
        if((M_ChkFlag(SL_STEADYFB)!=0)&&(M_ChkFlag(SL_NPR_PWMOUT)!=0))
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

		if((M_ChkFlag(SL_SKTOV)==0)&&(AMUX.skiiptempmax > 100.0))
		{
			M_SetFlag(SL_SKTOV);									//��SKIIP���¹��ϱ�־λ
		}
		else if((M_ChkFlag(SL_SKTOV)!=0)&&(AMUX.skiiptempmax < 90.0)) 
		{
			M_ClrFlag(SL_SKTOV);									//��SKIIP���¹��ϱ�־λ
		}
        
//----------------------����SKIIP���¹���----------------------------------------------------------			
		if((M_ChkFlag(SL_IN2_TAOV)!=0)&&(M_ChkFlag(SL_TAOVONCE)==0))
		{	
			M_SetFlag(SL_TAOVONCE);           		//���¶ȼ�⿹���ű�־λ
		}
		else if((M_ChkFlag(SL_IN2_TAOV)!=0)&&(M_ChkFlag(SL_TAOVONCE)!=0))
		{
			M_SetFlag(SL_TAOV); 					//�ó��±�־λ
		}		
       	else
		{
			M_ClrFlag(SL_TAOVONCE); 				//�峬�¼�⿹���ű�־λ
			M_ClrFlag(SL_TAOV);						//�峬�±�־λ
		}

//--------------------�����SKIIP���¹���-----------------------------------------------------------	
		if((M_ChkFlag(SL_IN2_TBOV)!=0)&&(M_ChkFlag(SL_TBOVONCE)==0))
		{	
			M_SetFlag(SL_TBOVONCE);            		//���¶ȼ�⿹���ű�־λ
		}
		else if((M_ChkFlag(SL_IN2_TBOV)!=0)&&(M_ChkFlag(SL_TBOVONCE)!=0))
		{
		   	M_SetFlag(SL_TBOV); 					//�ó��±�־λ
		}		
       	else
		{
			M_ClrFlag(SL_TBOVONCE); 				//�峬�¼�⿹���ű�־λ
			M_ClrFlag(SL_TBOV);						//�峬�±�־λ
		}


//----------------------�����г��¹���----------------------------------------------------------			
		if((M_ChkFlag(SL_NPR_TLOV)==0)&&(PRO.NPR_TLOV>_SC_NPR_TLOV))
		{
			M_SetFlag(SL_NPR_TLOV);									//�������г��±�־λ
		}
		else if((M_ChkFlag(SL_NPR_TLOV)!=0)&&(PRO.NPR_TLOV<(_SC_NPR_TLOV - 10))) 
		{
			M_ClrFlag(SL_NPR_TLOV);									//�������г��±�־λ
		} 


//--------------------������г��¹���-----------------------------------------------------------	
		if((M_ChkFlag(SL_MPR_TLOV)==0)&&(PRO.MPR_TLOV>_SC_MPR_TLOV))
		{
			M_SetFlag(SL_MPR_TLOV);									//�û����г��±�־λ
		}
		else if((M_ChkFlag(SL_MPR_TLOV)!=0)&&(PRO.MPR_TLOV<(_SC_MPR_TLOV - 10))) 
		{
			M_ClrFlag(SL_MPR_TLOV);									//������г��±�־λ
		} 

//-----------------------�������й����ʹ����ж�-----------------------------------------------------------
		temp_pgactive = abs(PRO.Pgactive);							//20090816
		temp_pgactive = temp_pgactive * 0.001;
		if((M_ChkFlag(SL_PGOV)==0)&&(temp_pgactive>_SC_PGOV))
		{
			M_SetFlag(SL_PGOV_COM);
			if(M_ChkCounter(MAIN_LOOP.cnt_pgovload,DELAY_PGOVLOAD)>0)  	//10s
				M_SetFlag(SL_PGOV);										//�ñ������й����ʹ��ر�־λ
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
 
//--------------------------------�õ�TAB_MSG�еĹ������-------------------------------------------
		_MSG_SCOUT2 = MSG_NONE;												//�Ƚ�MSG����
		
		if(M_ChkFlag(SL_CODEOK)==0)				_MSG_SCOUT2=MSG_CODEOK;      //1=������δУ�����
		
		else if(M_ChkFlag(SL_EE_FAIL)!=0) 		_MSG_SCOUT2=MSG_EE_FAIL;     //2=EEPROM����

		else if(M_ChkFlag(SL_CANOPENOVER)!=0) 	_MSG_SCOUT2=MSG_CAN_FAIL;    //3=CANͨѶ����

		else if(M_ChkFlag(SL_PHORDE)!=0) 		_MSG_SCOUT2=MSG_PHORDE;      //4=�������

		else if(M_ChkFlag(SL_ENCODFAULT)!=0) 	_MSG_SCOUT2=MSG_ENCODFAULT;  //5=����������

		else if(M_ChkFlag(SL_ESTOP)!=0) 	    _MSG_SCOUT2=MSG_ESTOP;       //6=����ͣ������

		else if(M_ChkFlag(SL_CBTRIP)!=0) 	    _MSG_SCOUT2=MSG_CBTRIP;      //7=�����ѿ۹���

		else if(M_ChkFlag(SL_EXFAIL)!=0) 	 	_MSG_SCOUT2=MSG_EXFAULT;     //8=�ⲿӲ������

		else if(M_ChkFlag(SL_ERROR_CB)!=0) 	 	_MSG_SCOUT2=MSG_CBERROR;     //9=��Ϻ�բ����

		else if(M_ChkFlag(SL_ERROR_PRE)!=0) 	_MSG_SCOUT2=MSG_PREERROR;    //10=Ԥ������

		else if(M_ChkFlag(SL_ERROR_MAINK)!=0) 	_MSG_SCOUT2=MSG_MAINKERROR;  //11=���Ӵ����պϹ���

		else if(M_ChkFlag(SL_ERROR_MAINF)!=0) 	_MSG_SCOUT2=MSG_MAINFERROR;  //12=���˲����պϹ���

		else if(M_ChkFlag(SL_ERROR_STAC)!=0) 	_MSG_SCOUT2=MSG_STACERROR;     //13=����Ӵ����պϹ���

		else if(M_ChkFlag(SL_MSPOUT)!=0) 	    _MSG_SCOUT2=MSG_MSPEEDOUT;     //14=ת�ٳ�����Χ����
					
		else if(M_ChkFlag(SL_HIA1)!=0)			_MSG_SCOUT2=MSG_HIA1;     //15=���������A��SKIIP����

		else if(M_ChkFlag(SL_HIB1)!=0)			_MSG_SCOUT2=MSG_HIB1;     //16=���������B��SKIIP����

		else if(M_ChkFlag(SL_HIC1)!=0)			_MSG_SCOUT2=MSG_HIC1;     //17=���������C��SKIIP����
		
		else if(M_ChkFlag(SL_HIA2)!=0)			_MSG_SCOUT2=MSG_HIA2;     //18=����������A��SKIIP���

		else if(M_ChkFlag(SL_HIB2)!=0)			_MSG_SCOUT2=MSG_HIB2;     //19=����������B��SKIIP����

		else if(M_ChkFlag(SL_HIC2)!=0)			_MSG_SCOUT2=MSG_HIC2;     //20=����������C��SKIIP����

		else if(M_ChkFlag(SL_HUDCOV)!=0)		_MSG_SCOUT2=MSG_UDCOV;    //21=������ֱ��ĸ��Ӳ����ѹ����

        else if(M_ChkFlag(SL_HIACOV1)!=0)		_MSG_SCOUT2=MSG_HIAC1;    //22=����Ӳ����������

		else if(M_ChkFlag(SL_HIACOV2)!=0)		_MSG_SCOUT2=MSG_HIAC2;    //23=�����Ӳ����������

        else if(M_ChkFlag(SL_PDPASERIES)!=0)	_MSG_SCOUT2=MSG_PDPASERIES;//24=�������ع���

		else if(M_ChkFlag(SL_PDPBSERIES)!=0)	_MSG_SCOUT2=MSG_PDPBSERIES;//25=�������ع���

		else if(M_ChkFlag(SL_SIAC1)!=0)			_MSG_SCOUT2=MSG_SIAC1;    //26=���������������

		else if(M_ChkFlag(SL_SIAC2)!=0)			_MSG_SCOUT2=MSG_SIAC2;    //27=����������������

		else if(M_ChkFlag(SL_FE1)!=0)			_MSG_SCOUT2=MSG_FE1;      //28=����Ƶ�ʲ�������

		else if(M_ChkFlag(SL_SUDCOV)!=0)		_MSG_SCOUT2=MSG_SUDCOV;   //29=����м�ֱ����ѹ��ѹ
		
		else if(M_ChkFlag(SL_UACOV1)!=0)		_MSG_SCOUT2=MSG_SUACOV1;  //30=�����ѹ������ѹ

//		else if(M_ChkFlag(SL_SUDCLV)!=0)		_MSG_SCOUT2=MSG_SUDCLV;   //31=����м�ֱ����ѹǷѹ 20100507atzuoyun
				
		else if(M_ChkFlag(SL_UACLV1)!=0)		_MSG_SCOUT2=MSG_SUACLV1;  //32=�����ѹ����Ƿѹ
		
//		else if(M_ChkFlag(SL_UDCWAVE)!=0)		_MSG_SCOUT2=MSG_UDCWAVE;  //33=�м��ѹ��������

        else if(M_ChkFlag(SL_SIOVST)!=0)		_MSG_SCOUT2=MSG_SIOVST;   //34=�����ⶨ�ӹ�������
		
		else if(M_ChkFlag(SL_TAOV)!=0)			_MSG_SCOUT2=MSG_TOV1;	  //35=����SKIIP����
		
		else if(M_ChkFlag(SL_TBOV)!=0)			_MSG_SCOUT2=MSG_TOV2;  	  //36=�����SKIIP����

		else if(M_ChkFlag(SL_SKTOV)!=0)			_MSG_SCOUT2=MSG_SKTOV;     //37=����ж�SKIIP����

        else if(M_ChkFlag(SL_NPR_TLOV)!=0)		_MSG_SCOUT2=MSG_TLOV1;     //38=�������г���

		else if(M_ChkFlag(SL_MPR_TLOV)!=0)		_MSG_SCOUT2=MSG_TLOV2;     //39=������г���

        else if(M_ChkFlag(SL_PGOV)!=0)		    _MSG_SCOUT2=MSG_PGOV;      //40=�������й����ʹ���

		else if(M_ChkFlag(SL_QGOV)!=0)	    	_MSG_SCOUT2=MSG_QGOV;      //41=�������޹����ʹ���

//------------------------���ϴ���---------------------------------------------------------

//NO1-------ԭ��û�й��ϱ����й���(��)ԭ���й��ϵ����μ������-------------------������������------
		if(((_MSG_SCOUT2!=MSG_NONE)&&(M_ChkFlag(SL_ERROR)==0))||((_MSG_SCOUT2!=MSG_NONE)&&(M_ChkFlag(SL_ERROR)!=0)&&(TAB_MSG[_MSG_SCOUT2].rank > TAB_MSG[_MSG_SCOUT1].rank)))
		{													
			M_SetFlag(SL_ERROR);							//�ù��ϱ�־λ
			M_SetFlag(SL_DISPLAY5);                         //��ϵͳ����ָʾ

//----------------------------�¹������Ա�ʾ-------------------------------------------------------												
			if((TAB_MSG[_MSG_SCOUT2].attr & SHUT)==SHUT)		M_SetFlag(SL_SHUT);		//ͣ������
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
			
			if((TAB_MSG[_MSG_SCOUT2].attr & SAVE)==SAVE)		M_SetFlag(SL_SAVE);		//��Ҫ�洢
			else												M_ClrFlag(SL_SAVE);		

			if((TAB_MSG[_MSG_SCOUT2].attr & WARNING)==WARNING)	M_SetFlag(SL_WARNING);	//��������
			else												M_ClrFlag(SL_WARNING);	
	
			if((TAB_MSG[_MSG_SCOUT2].attr & OFFCB)==OFFCB)		M_SetFlag(SL_OFFCB);	//�ȶ���������
			else												M_ClrFlag(SL_OFFCB);	

			if((TAB_MSG[_MSG_SCOUT2].attr & REPORT)==REPORT)	M_SetFlag(SL_REPORT);	//�ϱ���������
			else												M_ClrFlag(SL_REPORT);	

//----------------------------------���ݹ��������ж�ϵͳ����---------------------------------------
						
			if(M_ChkFlag(SL_NRCVR)!=0)		//�������Թ���
			{
				M_SetFlag(SL_FORBIDRESET);	//���ϴ�����������,��ֹ���ظ�λ 201105atzuoyun	
				M_SetFlag(SL_ERRSTOP);		
				M_SetFlag(SL_SERIESTOP);    //���ع��ϣ����ֶ���λ
				M_ClrFlag(SL_NRCVR);		
			}

			if(M_ChkFlag(SL_OFFCB)!=0)		//���ع���,ֱ�Ӷ����� 201105atzuoyun
			{
				M_SetFlag(SL_ERRSTOP);		
				M_SetFlag(SL_SERIESTOP);    //���ع��ϣ��������ظ�λ
				M_ClrFlag(SL_OFFCB);	
			}


        	if(M_ChkFlag(SL_REPORT)!=0)   //�ù�����Ҫ�ϱ�������201105atzuoyun
			{
				M_SetFlag(SL_REPORT_OCS);  //�ϱ�������			
				M_ClrFlag(SL_REPORT);												
			} 
			
            if(M_ChkFlag(SL_SHUT)!=0)
			{
				M_SetFlag(SL_ERRSTOP);												
				M_ClrFlag(SL_SHUT);
			}

/*			if(M_ChkFlag(SL_IRCVR)!=0)		//���������ָ����� ������,������������ָ�201105atzuoyun
			{
				M_ClrFlag(SL_IRCVR);							
				M_SetFlag(SL_ERRSTOP);
			}											
*/					

			if(M_ChkFlag(SL_CNT)!=0)
			{
				_SY_RTRN++;
//				if(_SY_RTRN>_SC_RTRN)	M_SetFlag(SL_SERIESTOP);		//��������,���ع���	
				if(_SY_RTRN>_SC_RTRN)	M_SetFlag(SL_FORBIDRESET);		//���ϴ�����������,��ֹ���ظ�λ 201105atzuoyun	

				M_ClrFlag(SL_CNT);				
			}

			if(M_ChkFlag(SL_OTSER)!=0)
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_otser,DELAY_OTSER)>=0)	//��ʱʱ�䵽
				{
					M_SetFlag(SL_SERIESTOP);							//���ع���
					M_ClrFlag(SL_OTSER);								//�峬ʱ���ر�־
				}	
			}	

			MAIN_LOOP.cnt_rcvr=0;			//����ʱ�ָ�������	
			_MSG_SCOUT1 = _MSG_SCOUT2;		//���ι�����Ϣת��
								
//----------------------------���¹��ϼ�¼������λ��������------------------------------------------						
			if((M_ChkFlag(SL_SAVE)!=0)&&(M_ChkFlag(SL_EEBUSY_ERRSAVE)==0))			
			{
				MAIN_LOOP.cnt_rcvr=0;			//����ʱ�ָ�������	

				M_SetFlag(SL_EEASK_ERRSAVE);							//EEPROM��������
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
                _BA_EAGLRF = (int16)(DISP.aglrf);            			 //�޹��Ƕ�ָ��

				_BA_EIA1  = (int16)(AD_OUT_NPR_I.a * 10);				//���������,A�����˲ʱֵ
				_BA_EIB1  = (int16)(AD_OUT_NPR_I.b * 10);				//���������,B�����˲ʱֵ
				_BA_EIC1  = (int16)(AD_OUT_NPR_I.c * 10);				//���������,C�����˲ʱֵ
				_BA_EIA2  = (int16)(AD_OUT_MPR_I.a * 10);				//����������,A�����˲ʱֵ
				_BA_EIB2  = (int16)(AD_OUT_MPR_I.b * 10);	            //����������,B�����˲ʱֵ
				_BA_EIC2  = (int16)(AD_OUT_MPR_I.c * 10);	            //����������,C�����˲ʱֵ
				
				_BA_EUAB1  = (int16)PRO.NPR_uab;						//����ab�ߵ�ѹ
				_BA_EUBC1  = (int16)PRO.NPR_ubc;						//����bc�ߵ�ѹ
				_BA_EUAB2  = (int16)PRO.STA_uab;						//���Ӳ�ab�ߵ�ѹ
				_BA_EUBC2  = (int16)PRO.STA_ubc;						//���Ӳ�bc�ߵ�ѹ

				_BA_EUAB0    = (int16)DISP.grd_uab;			       		//����ǰ����ab��ѹ
            	_BA_EUBC0    = (int16)DISP.grd_ubc;			       		//����ǰ����bc��ѹ

				_BA_ETLAC  = (int16)PRO.NPR_TLOV;                           //�������¶�
				_BA_ETLDUDT= (int16)PRO.MPR_TLOV;                           //�������¶�
				_BA_ETSKIIP= (int16)AMUX.skiiptempmax;                      //SKIIP�¶�	

				_BA_EFREQ  = (int16)(CAP4.freq * 10);						//����Ƶ��
				_BA_ESPEED = (int16)(PRO.speed);							//���ת�� 

	            _BA_ENPRUD = (int16)TRS_NGS_U.d;              			//d�����Ϻ���ѹ����20091026atzy
				_BA_ENPRUQ = (int16)TRS_NGS_U.q;          				//q�����Ϻ���ѹ����20091026atzy
	            _BA_ENPRUD2 = (int16)TRS_NGS_U.dflt;          			//d�����Ϻ���ѹ���� �˲���20091026atzy
				_BA_ENPRUQ2 = (int16)TRS_NGS_U.qflt;          			//q�����Ϻ���ѹ���� �˲���20091026atzy
	            _BA_ENUDOUT = (int16)TRS_NPR_U.d;          				//����d���ѹ���20091026atzy
				_BA_ENUQOUT = (int16)TRS_NPR_U.q;          				//����q���ѹ���20091026atzy

				_BA_EPIONU  = (int16)(PI_NPR_U.out * 10);					//���A
				_BA_EPIONID = (int16)(PI_NPR_Id.out* 10);					//��λV
				_BA_EPIONIQ	= (int16)(PI_NPR_Iq.out* 10);					//��λV

				_BA_EMEXI   = (int16)(RUN_mpridrf_exi * 10);				//��λA  �����������ۼ���ֵ��ʾ cpc
				_BA_EPIOMID = (int16)(PI_MPR_Id.out* 10);					//���V
				_BA_EPIOMIQ	= (int16)(PI_MPR_Iq.out* 10);					//��λV

				_BA_ESTAIAC = (int16)(DISP.sta_iac * 10);
				_BA_ESTAIBA = (int16)(DISP.sta_iba * 10);
				_BA_ETOQFB  = (int16)DISP.toqfb;				        	//ת�ط���
				_BA_EPSTA   = (int16)(DISP.Psactive * 0.01);          		//���Ӳ��й�������ʾkW,DOT1 *10/1000=100
				_BA_EPNPR   = (int16)(DISP.Pnactive * 0.01);         		//�����й�������ʾ
//				_BA_EPGRID  = (int16)(DISP.Pgactive * 0.01);          		//�����ܵ��й�������ʾ
				_BA_EPGRID  = (int16)(_BA_EPGRID);          				//��¼PDP���ϴ��� 20100507atzuoyun

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
			_MSG_SCOUT1 = _MSG_SCOUT2;		//���ι�����Ϣת��

			if(M_ChkFlag(SL_OTSER)!=0)
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_otser,DELAY_OTSER)>=0)	//��ʱʱ�䵽
				{
					M_SetFlag(SL_ERRSTOP);								//����
					M_SetFlag(SL_SERIESTOP);							//���ع���
					M_ClrFlag(SL_OTSER);								//�峬ʱ���ر�־
				}	
			}
			else	MAIN_LOOP.cnt_otser=0;				
		}

//NO3---------ԭ���й��ϣ�����û�й���----------------------------------------�����Ͻ���Ϊ�㣩------			
		else if((_MSG_SCOUT2==MSG_NONE)&&(M_ChkFlag(SL_ERROR)!=0))					
		{				
		
			if(M_ChkFlag(SL_OTSER)!=0)		
			{
				M_ClrFlag(SL_OTSER);								//��ʱ����,����û�й���
				MAIN_LOOP.cnt_otser=0;  							//�峬ʱ������ʱ������
			}

			if(M_ChkFlag(SL_IRCVR)!=0)								//���������ָ�����
			{
				M_ClrFlag(SL_IRCVR);								//�������ָ���־
				if(M_ChkFlag(SL_FORBIDRESET)==0)
				{
					M_ClrFlag(SL_ERROR);							//ɾ�������ź�
					M_ClrFlag(SL_ERRSTOP);							//ɾ�������ź�  new
					M_ClrFlag(SL_SERIESTOP);						//��ʱ�ָ����������ع��ϱ�־201105atzuoyun
					M_ClrFlag(SL_REPORT_OCS); 						//���ٳ��������ϱ�����	201105atzuoyun
				} 
			}
			
			if(M_ChkFlag(SL_DRCVR)!=0)								//������ʱ�ָ��ұ���û�й���
			{
				PRO.rcvr = _SC_RTRT * 1000;							//��λ�任:s->ms
				if(M_ChkCounter(MAIN_LOOP.cnt_rcvr,PRO.rcvr)>=0)	//��ʱʱ�䵽
				{
					M_ClrFlag(SL_DRCVR);							//����ʱ�ָ���־
					if(M_ChkFlag(SL_FORBIDRESET)==0)
					{
						M_ClrFlag(SL_ERROR);							//ɾ�������ź�
						M_ClrFlag(SL_ERRSTOP);							//ɾ�������ź�  new
						M_ClrFlag(SL_SERIESTOP);						//��ʱ�ָ����������ع��ϱ�־201105atzuoyun
						M_ClrFlag(SL_REPORT_OCS); 						//���ٳ��������ϱ�����	201105atzuoyun
					}
				}
			}
			else	
				MAIN_LOOP.cnt_rcvr=0;		
		}
//NO4---------------------------------ԭ��û�й��ϱ���Ҳû�й���--------------������ƽ��Ϊ�㣩------
		else if((_MSG_SCOUT2==MSG_NONE)&&(M_ChkFlag(SL_ERROR)==0))	
		{
			M_ClrFlag(SL_WARNING);
			M_ClrFlag(SL_SHUT);
			M_ClrFlag(SL_OTSER);
			M_ClrFlag(SL_IRCVR);
			M_ClrFlag(SL_NRCVR);			
			M_ClrFlag(SL_DRCVR);
			M_ClrFlag(SL_OFFCB);	//201105atzuoyun
			M_ClrFlag(SL_REPORT);  	//201105atzuoyun
			MAIN_LOOP.cnt_rcvr=0;
			MAIN_LOOP.cnt_otser=0;
			_MSG_SCOUT1=0;
			_MSG_SCOUT2=0;
			M_ClrFlag(SL_DISPLAY5);                         //��ϵͳ����ָʾ
		}
	}//if((M_ChkFlag(SL_POWERON)==0)&&(M_ChkFlag(SL_ERRSTOP)==0))

//-------------------------����ͣ����λ����-------------------------------------------------------
	if(((M_ChkFlag(SL_ERRSTOP)!=0)||(M_ChkFlag(SL_SERIESTOP)!=0)) &&(M_ChkFlag(SL_RESET)!=0)&&(_MSG_SCOUT2==0)&&(M_ChkFlag(SL_FORBIDRESET)==0))   //û�й���ʱ���ܸ�λ
	{																											//һ��ʱ���ڹ��ϳ����������ֹ���ظ�λ 201105atzuoyun
			M_ClrFlag(SL_SERIESTOP);				//�������ͣ�����ϱ�־
			M_ClrFlag(SL_ERRSTOP);					//���ͣ�����ϱ�־
			M_ClrFlag(SL_ERROR);					//������ϱ�־
//			_SY_RTRN=0;								//���ϼ���������		201105atzuoyun
			M_ClrFlag(SL_REPORT_OCS); 				//���ٳ��������ϱ�����	201105atzuoyun
			
			M_ClrFlag(SL_OCS_NPRSTART);	          	//����������	 
			M_ClrFlag(SL_OCS_MPRSTART);				//����������								

			M_ClrFlag(SL_EE_FAIL);					//���EEROM���ϱ�־

			M_ClrFlag(SL_PDPINTA);					//�������TZ1_PDP���ϱ�־
			M_ClrFlag(SL_PDPINTB);					//�������TZ2_PDP���ϱ�־
			M_ClrFlag(SL_QEPPCO);
//			M_ClrFlag(SL_QEPPHE);
//			M_ClrFlag(SL_QEPPCDE);

			M_ClrFlag(SL_DISPLAY3);             	//��PDP����ָʾ��
			ClrPdpint();							//PDPINT�ж����	
			EnPdpint();								//PDPINT�ж�ʹ��
	
			_MSG_SCOUT1=0;											//�������Ϣλ
			_MSG_SCOUT2=0;
			MAIN_LOOP.cnt_rcvr=0;                                   //������ʱ�ָ���ʱ
			MAIN_LOOP.cnt_otser=0;                                  //��ʱ������ʱ	

//			M_ClrFlag(SL_CANOPENOVER);               //CANͨѶ
	}

	if(_SY_RTRN!=0)
	{
		PRO.reset = _stdby05 * 1000;							//��λ�任:s->ms

		if(M_ChkCounter(MAIN_LOOP.cnt_resetrn,PRO.reset)>=0)	//�ɱ���_STDBY5�趨0-32767,��λs
//		if(M_ChkCounter(MAIN_LOOP.cnt_resetrn,86400000)>=0)	//1 day
//		if(M_ChkCounter(MAIN_LOOP.cnt_resetrn,300000)>=0)	//5 min test 
		{
			_SY_RTRN=0;	
			MAIN_LOOP.cnt_resetrn=0;
		}
	}
	else	MAIN_LOOP.cnt_resetrn=0;


}
/*********************************************************************************************************
** ��������: et_relay
** ��������: ���ر�������-NPR
** �䡡��: 	 
** ��:   
** ע  ��: 	 
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
//			M_SetFlag(SL_ERRDATASAVE);	//�����ⲿRAM����ת��20091109atzy
		}
		else M_ClrFlag(SL_SIAC1);
	}
}  
/*********************************************************************************************************
** ��������: et_relay
** ��������: ���ر�������--MPR
** �䡡��: 	 
** ��:   
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
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
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
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
	if(MAIN_LOOP.cnt_freq2!=65535)		MAIN_LOOP.cnt_freq2++;							
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

	if(MAIN_LOOP.cnt_offcb!=65535)		MAIN_LOOP.cnt_offcb++;
	if(MAIN_LOOP.cnt_cbtp!=65535)		MAIN_LOOP.cnt_cbtp++;
	if(MAIN_LOOP.cnt_cberror!=65535)		MAIN_LOOP.cnt_cberror++;
	if(MAIN_LOOP.cnt_mainkerror!=65535)		MAIN_LOOP.cnt_mainkerror++;
	if(MAIN_LOOP.cnt_mainferror!=65535)		MAIN_LOOP.cnt_mainferror++;
	if(MAIN_LOOP.cnt_stacerror!=65535)		MAIN_LOOP.cnt_stacerror++;
	if(MAIN_LOOP.cnt_preerror!=65535)		MAIN_LOOP.cnt_preerror++;

	if(SCI.cnt_sciover!=65535)			SCI.cnt_sciover++;
	if(SCI.cnt_scispace!=65535)			SCI.cnt_scispace++;

	if(MAIN_LOOP.cnt_isteady0!=65535)		MAIN_LOOP.cnt_isteady0++;     //RunCtrl����ͣ��������ʱ
	if(MAIN_LOOP.cnt_mprsteady0!=65535)		MAIN_LOOP.cnt_mprsteady0++;   //RunCtrl����ͣ��������ʱ

	if(MAIN_LOOP.cnt_senszfstdy!=60001)		MAIN_LOOP.cnt_senszfstdy++;		//��������Ư�˲�������
	if(MAIN_LOOP.cnt_speedout!=65535)		MAIN_LOOP.cnt_speedout++;		//20090817
	if(MAIN_LOOP.cnt_uaclv1!=65535)		MAIN_LOOP.cnt_uaclv1++;		//200901027atzy

//sysctl_zl_start
	if(MAIN_LOOP.cnt_opencontac!=65535)		MAIN_LOOP.cnt_opencontac++;    //���ز��ɻָ������£�����������ָ��󵽷����϶��ӽӴ��������Ӵ�������ʱ
	
	if(MAIN_LOOP.cnt_mkoff!=65535)	    	MAIN_LOOP.cnt_mkoff++;         //���յ�ϵͳͣ��ָ�����ʱ�����Ӵ�������
	if(MAIN_LOOP.cnt_cboff!=65535)	    	MAIN_LOOP.cnt_cboff++;         //���յ�ϵͳͣ��ָ�����ʱ�����ϼ���
      
	if(MAIN_LOOP.cnt_closecb!=65535)		MAIN_LOOP.cnt_closecb++;        //�������������ϴ��ܵ����Ϻ�բ����ʱ����
	if(MAIN_LOOP.cnt_precok!=65535)      	MAIN_LOOP.cnt_precok++;         //����������Ԥ���ʱ��

	if(MAIN_LOOP.cnt_steadyfb!=65535)      	MAIN_LOOP.cnt_steadyfb++;       //��������/ֹͣ����⵽Vdc�ȶ���1100V����������������ʱ����
	if(MAIN_LOOP.cnt_mprstart!=65535)      	MAIN_LOOP.cnt_mprstart++;       //����������Vdc�ȶ���MPR��������ʱ���� 
//sysctl_zl_end	

//ADctl_zl_start
    if(MAIN_LOOP.cnt_AMUX!=65535)      	    MAIN_LOOP.cnt_AMUX++;           //����AD������ʱ��ʱ
//ADctl_zl_end
    if(MAIN_LOOP.cnt_reset!=65535)      	MAIN_LOOP.cnt_reset++;           //��λ��I/O��λ����ʱ2s����ֹ�����
    if(MAIN_LOOP.cnt_clostacmd!=65535)      MAIN_LOOP.cnt_clostacmd++;       //�λ�����϶��ӽӴ�������ʱ1s��ֹ���
    if(MAIN_LOOP.cnt_nprcmd!=65535)         MAIN_LOOP.cnt_nprcmd++;         //��λ������NPR��������ʱ1s����ֹ�����
    if(MAIN_LOOP.cnt_mprcmd!=65535)         MAIN_LOOP.cnt_mprcmd++;         //��λ������MPR�����壬��ʱ1s����������
    if(MAIN_LOOP.cnt_fanstar!=65535)        MAIN_LOOP.cnt_fanstar++;          //��������Ǳ任��ʱ����
    if(MAIN_LOOP.cnt_fantriangle!=65535)    MAIN_LOOP.cnt_fantriangle++;      //��������Ǳ任��ʱ���� 
//    if(MAIN_LOOP.cnt_fanstop!=65535)        MAIN_LOOP.cnt_fanstop++;          //ϵͳͣ���󣬿��Ʒ�����ǽӹ�����ʱ���� 
	if(MAIN_LOOP.cnt_qcapspdin!=65535)		MAIN_LOOP.cnt_qcapspdin++;   
	if(MAIN_LOOP.cnt_async!=65535)		    MAIN_LOOP.cnt_async++;   
	if(MAIN_LOOP.cnt_qcapdisturb!=65535)	MAIN_LOOP.cnt_qcapdisturb++;		//QEP�����   
	if(MAIN_LOOP.cnt_qepcntok!=65535)		MAIN_LOOP.cnt_qepcntok++;   		//QEP������   
	if(MAIN_LOOP.cnt_qepzdisturb!=65535)	MAIN_LOOP.cnt_qepzdisturb++;   		//QEP������   
	   
//canopen
	if(MAIN_LOOP.canopen_tx!=65535)			MAIN_LOOP.canopen_tx++;   
	if(MAIN_LOOP.canopen_rx!=65535)			MAIN_LOOP.canopen_rx++;      

	if(SCI_canopen.cnt_heartbeat!=65535)	SCI_canopen.cnt_heartbeat++;      
	if(SCI_canopen.cnt_sciover!=65535)		SCI_canopen.cnt_sciover++;   
	if(MAIN_LOOP.cnt_cbreset!=65535)		MAIN_LOOP.cnt_cbreset++;  

	if(MAIN_LOOP.et_relay1!=65535)		    MAIN_LOOP.et_relay1++;      
	if(MAIN_LOOP.et_relay2!=65535)		    MAIN_LOOP.et_relay2++;   

	if(MAIN_LOOP.cnt_ocsein1!=65535)		MAIN_LOOP.cnt_ocsein1++;   
	if(MAIN_LOOP.cnt_ocsein2!=65535)		MAIN_LOOP.cnt_ocsein2++;  
	if(MAIN_LOOP.cnt_ocssysrun1!=65535)		MAIN_LOOP.cnt_ocssysrun1++;      
	if(MAIN_LOOP.cnt_ocssysrun2!=65535)		MAIN_LOOP.cnt_ocssysrun2++;   

	if(MAIN_LOOP.cnt_datasave!=65535)		MAIN_LOOP.cnt_datasave++;   
    if(MAIN_LOOP.cnt_resetrn!=65535)    	MAIN_LOOP.cnt_resetrn++;    //201105atzuoyun

//test----------------------------------------------------------------------------------------------
/*
if((M_ChkFlag(SL_NPR_RUNING)!=0))//&&(i_cnt1<=99)
{
	i_cnt2++;
	if(i_cnt2>=1)	 i_cnt2=0;
	if(i_cnt2==0)
	{
		draw1[i_cnt1] = CAP4.freq;      //
		draw2[i_cnt1] = TRS_NPR_U.d;        //

		draw3[i_cnt1] = TRS_NGS_U.qflt;       //
		draw4[i_cnt1] = TRS_NPR_U.q;            //
	
		draw5[i_cnt1] = TRS_NPR_U.alfa;//
		draw6[i_cnt1] = TRS_NPR_U.beta;//

		draw5[i_cnt1] = PI_NPR_Id.out;//
		draw6[i_cnt1] = PI_NPR_Iq.out;//

		i_cnt1++;
    	if(i_cnt1>=149)	i_cnt1=149;
	}

}
else 
{

	i_cnt2++;
	if(i_cnt2>=1)	 i_cnt2=0;
	if(i_cnt2==0)
	{
		draw1[i_cnt1] = CAP4.freqtmp;//PI_NPR_U.reference;//PI_NPR_Id.reference;//;//;
		draw2[i_cnt1] = CAP4.prd;//PI_NPR_U.feedback;//PI_NPR_Id.feedback;//PI_NPR_Id.feedback;//AD_OUT_NPR_I.c;

		draw3[i_cnt1] = ECap4Regs.TSCTR;//PI_NPR_Id.errortrue;//AD_OUT_NPR_I.c;
		draw4[i_cnt1] = CAP4.freq;//EPwm2Regs.CMPA.half.CMPA*1.0 - EPwm3Regs.CMPA.half.CMPA*1.0;//PI_NPR_Id.error;//AD_OUT_NPR_I.b;
	
		draw5[i_cnt1] = CAP4.nprtrstheta;//PI_NPR_Id.errortrue;//AD_OUT_NPR_I.c;
		draw6[i_cnt1] = GpioDataRegs.GPADAT.bit.GPIO5;//EPwm2Regs.CMPA.half.CMPA*1.0 - EPwm3Regs.CMPA.half.CMPA*1.0;//PI_NPR_Id.error;//AD_OUT_NPR_I.b;
	
		i_cnt1++;
    	if(i_cnt1>=149)	i_cnt1=0;
		i_cnt3=i_cnt1;
	}

}
//test---------------------------------------------------------------------------------------------- 
*/ 
}

/*********************************************************************************************************
** ��������: Display
** ��������: ��ʾֵ����
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
	DISP.grd_uab = MEAN_DATA.uab * 1.110721;        //��λV  rms=mean*PAI/(2*sqrt(2)) �� 1.110721=PAI * SQRT2 / 4
    DISP.grd_ubc = MEAN_DATA.ubc * 1.110721;        //��λV  rms=mean*PAI/(2*sqrt(2)) �� 1.110721=PAI * SQRT2 / 4

//---------------------------����ͻ������¶���ʾ----------------------------
    DISP.Lac_temp      = AMUX.Lac_temp;                  //�������¶�
	DISP.Ldudt_temp    = AMUX.Ldudt_temp;                //�������¶� 
	DISP.NPR_skiptemp  = AMUX.NPR_skiiptemp;              //����SKIIP�¶�
	DISP.MPR_skiptemp  = AMUX.MPR_skiiptemp;              //����SKIIP�¶�


//---------------------------˲ʱ������ʾֵ����----------------------------------------------------
	DISP.npr_iai = AD_OUT_NPR_I.a;					//��λA
	DISP.npr_ibi = AD_OUT_NPR_I.b;					//��λA
	DISP.npr_ici = AD_OUT_NPR_I.c;					//��λA
	DISP.mpr_iai = AD_OUT_MPR_I.a;					//��λA
	DISP.mpr_ibi = AD_OUT_MPR_I.b;					//��λA
	DISP.mpr_ici = AD_OUT_MPR_I.c;					//��λA

//------------------------���ಢ��������Чֵ��ʾֵ����----------------------------------------------
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
	temp2 = MEAN_DATA.iba3 * 1.110721;
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
		if(M_ChkCounter(MAIN_LOOP.cnt_nprlamp,DELAY_NPRRUN)>=0)	//0�ƿ�����˸,ָʾ�������ڷ���������
		{
			M_NotFlag(SL_DISPLAY0);
			MAIN_LOOP.cnt_nprlamp=0;
		}
	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_nprlamp,DELAY_NPRSTDBY)>=0)	//0�������˸
		{
		   M_NotFlag(SL_DISPLAY0);
		   MAIN_LOOP.cnt_nprlamp=0;
		} 
	}
			
//------------------------------------���๤��ָʾ����ʾ--------------------------------------------
	if(M_ChkFlag(SL_MPR_PWMOUT)!=0)
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_mprlamp,DELAY_MPRRUN)>=0)	//1�ƿ�����˸,�ʾ�������ڷ���������
		{
			M_NotFlag(SL_DISPLAY1);
			MAIN_LOOP.cnt_mprlamp=0;
		}
	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_mprlamp,DELAY_MPRSTDBY)>=0)	//1��������˸
		{
			M_NotFlag(SL_DISPLAY1);
			MAIN_LOOP.cnt_mprlamp=0;
		}
	}

//----------------------------------------���м��--------------------------------------------------
	_BA_URF   = (int16)DISP.urf;				//�м��ѹ����ֵ
	_BA_UDC   = (int16)DISP.udc;				//�м�ֱ����ѹ
	_BA_MIDRF  = (int16)(DISP.mpridrf * 10);		//d�����ָ��
	_BA_NIQRF  = (int16)(DISP.npriqrf * 10);		//q�����ָ��
	_BA_MIQRF  = (int16)(DISP.mpriqrf * 10);		//q�����ָ��
	_BA_TOQRF  = (int16)(DISP.toqrf);				//ת��ָ��
    _BA_AGLRF  = (int16)(DISP.aglrf);             //�޹��Ƕ�ָ�� 20091027atzy

	_BA_IA1   = (int16)(DISP.npr_iar * 10);		//����,a�ಢ����������Ϊ1λ���
	_BA_IB1   = (int16)(DISP.npr_ibr * 10);		//����,b�ಢ������
	_BA_IC1   = (int16)(DISP.npr_icr * 10);		//����,c�ಢ������
	_BA_IA2   = (int16)(DISP.mpr_iar * 10);		//�����,a�����
	_BA_IB2   = (int16)(DISP.mpr_ibr * 10);		//�����,b�����
	_BA_IC2   = (int16)(DISP.mpr_icr * 10);		//�����,c�����

	_BA_UAB0   = (int16)DISP.grd_uab;			//����ǰ����ab��ѹ
	_BA_UBC0   = (int16)DISP.grd_ubc;			//����ǰ����bc��ѹ				
	_BA_UAB1  = (int16)DISP.npr_uab;			//����ab��ѹ
	_BA_UBC1  = (int16)DISP.npr_ubc;			//����bc��ѹ
//	_BA_UAB2  = (int16)DISP.sta_uab;			//���Ӳ�ab��ѹ,���Ӳ� 20091027atzy
//	_BA_UBC2  = (int16)DISP.sta_ubc;			//���Ӳ�bc��ѹ,���Ӳ� 20091027atzy
	_BA_GRDUD = (int16)TRS_NGS_U.dflt;          //d�����Ϻ���ѹ���� �˲���
	_BA_GRDUQ = (int16)TRS_NGS_U.qflt;          //q�����Ϻ���ѹ���� �˲���
	_BA_STAID = (int16)TRS_NPR_I.dflt;          //d������������� �˲���
	_BA_STAIQ = (int16)TRS_NPR_I.qflt;          //q������������� �˲���
    _BA_EXCID = (int16)(TRS_MPR_I.d * 10);             //d������������ �˲�ǰ
    _BA_EXCIQ = (int16)(TRS_MPR_I.q * 10);             //q������������ �˲�ǰ
	_BA_STAUD = (int16)TRS_MPR_U.d;
	_BA_STAUQ = (int16)TRS_MPR_U.q;

	_BA_TLAC  = (int16)DISP.Lac_temp;		    //�������¶�
	_BA_TLDUDT= (int16)DISP.Ldudt_temp;	        //�������¶�
	_BA_TNSKIIP= (int16)DISP.NPR_skiptemp;      //����SKIIP�¶� ���϶�
	_BA_TMSKIIP= (int16)DISP.MPR_skiptemp;      //����SKIIP�¶� ���϶�

	_BA_FREQ  = (int16)(DISP.freq * 10);		//����Ƶ��
	_BA_SPEED = (int16)DISP.speed;				//���ת�� 

	_BA_PIONU  = (int16)(DISP.pionu  * 10);			    //NPR��ѹ����� 6.23change_zl��Ϊ1λС��
	_BA_PIONID = (int16)(DISP.pionid * 100);			//NPR������d���
	_BA_PIONIQ = (int16)(DISP.pioniq * 100);			//NPR������q���
	_BA_MEXI   = (int16)(DISP.mexi  * 10);			    // 6.23change_zl��Ϊ1λС�� ��Ϊ���ŵ�������ֵ��ʾcpc
	_BA_PIOMID = (int16)(DISP.piomid * 100);			//MPR������d���
	_BA_PIOMIQ = (int16)(DISP.piomiq * 100);			//MPR������q���

//	_BA_STAUABD = (int16)(DISP.uab23 * 10);             //����ͬ������ǰ����ǰ���ѹ�� 20091027atzy
//	_BA_STAUBCD = (int16)(DISP.ubc23 * 10);				//����ͬ������ǰ����ǰ���ѹ�� 20091027atzy
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
	_BA_SGRID = (int16)(DISP.Pg * 0.01);              	//�����ܵ����ڹ�����ʾ 20100506

//ͨ��CANOPENͨѶ���������صļ�ر���	

//	DISP.toqfb = 8000;  //cantest
//    DISP.speed = 1800;  //cantest
//	AMUX.skiiptempmax = 65; //cantest

	SCI_canopen.tx_torque= (int16)(DISP.toqfb * 16383.0 / CAN_TEN);
	SCI_canopen.tx_speed = (int16)(PRO.speedflt * 16383.0 / CAN_SPEED);

	SCI_canopen.tx_skiiptempmax = (int16)(AMUX.skiiptempmax * 16383.0 / CAN_TEMP);
	SCI_canopen.tx_watertempin  = 0;
	SCI_canopen.tx_watertempout = 0;
	SCI_canopen.tx_demand = 0;
	

} 



/*********************************************************************************************************
** ��������: Datasave
** ��  ��: ������д���ⲿRAM
** �䡡��:
** �䡡��:        
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 20091109atzy
**-------------------------------------------------------------------------------------------------------
** �޸��:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Datasave(void)
{
//--------------------------------External RAM Data Save-----------------------------------------
//	if(_STDBY2!=0)	M_SetFlag(SL_ERRDATASAVE);	//�����ⲿRAM����ת��20091109athotel

  	if(*(RAM_END - 1) != 0x55AA)
  	{
		if(M_ChkFlag(SL_ERRDATASAVE)==0)  								//û�й���	
			MAIN_LOOP.cnt_datasave=0;
		else if(M_ChkCounter(MAIN_LOOP.cnt_datasave,DELAY_DATASAVE)>=0)	//���Ϸ�������ʱ2s��ֹ��ͼ
		{
			M_SetFlag(SL_DRAW);
			M_ClrFlag(SL_ERRDATASAVE);
			*(RAM_END - 1) = 0x55AA;

			*(RAM_END - 2) = RTIMER.time[0];							 //���¹���ʱ��
			*(RAM_END - 3) = RTIMER.time[1];
			*(RAM_END - 4) = RTIMER.time[2];
			*(RAM_END - 5) = RTIMER.time[3];
			*(RAM_END - 6) = RTIMER.time[4];
			*(RAM_END - 7) = RTIMER.time[5];			
		}

		if(M_ChkFlag(SL_DRAW)==0)  										//�й���֮����ʱ2s��ֹͣ���ݴ洢
		{
			*(RAM_START+ RAMDATA_POS) = (int16)AD_OUT_UDC;							//0=�м�ֱ����ѹ
//			*(RAM_START+ RAMDATA_POS) = (int16)testtheta;	//1=test
			*(RAM_START+((Uint32)RAM_BIAS * 1 + RAMDATA_POS)) = (int16)CAP4.nprtrstheta;	//1=NPR����Ƕ�
			*(RAM_START+((Uint32)RAM_BIAS * 2 + RAMDATA_POS)) = (int16)AD_OUT_NPR_I.a;		//2=NPR-A����
			*(RAM_START+((Uint32)RAM_BIAS * 3 + RAMDATA_POS)) = (int16)AD_OUT_NPR_I.b;		//3=NPR-B����
			*(RAM_START+((Uint32)RAM_BIAS * 4 + RAMDATA_POS)) = (int16)AD_OUT_NPR_I.c;		//4=NPR-C����
			*(RAM_START+((Uint32)RAM_BIAS * 5 + RAMDATA_POS)) = (int16)AD_OUT_GRD_U.ab;		//5=������ѹVab
			*(RAM_START+((Uint32)RAM_BIAS * 6 + RAMDATA_POS)) = (int16)AD_OUT_GRD_U.bc;		//6=������ѹVbc
			*(RAM_START+((Uint32)RAM_BIAS * 7 + RAMDATA_POS)) = (int16)TRS_NGS_U.d;			//7=��ѹd�����ed�˲�ǰ
			*(RAM_START+((Uint32)RAM_BIAS * 8 + RAMDATA_POS)) = (int16)TRS_NGS_U.q;			//8=��ѹq�����eq�˲�ǰ
			*(RAM_START+((Uint32)RAM_BIAS * 9 + RAMDATA_POS)) = (int16)TRS_NGS_U.dflt;		//9=��ѹd�����ed�˲���
			*(RAM_START+((Uint32)RAM_BIAS * 10 + RAMDATA_POS)) = (int16)TRS_NGS_U.qflt;		//10=��ѹq�����eq�˲���
			*(RAM_START+((Uint32)RAM_BIAS * 11 + RAMDATA_POS)) = (int16)PI_NPR_Id.reference;//11=����Idָ��
			*(RAM_START+((Uint32)RAM_BIAS * 12 + RAMDATA_POS)) = (int16)TRS_NPR_I.dflt;		//12=����Id����
			*(RAM_START+((Uint32)RAM_BIAS * 13 + RAMDATA_POS)) = (int16)PI_NPR_Id.out;		//13=����IdPI���
			*(RAM_START+((Uint32)RAM_BIAS * 14 + RAMDATA_POS)) = (int16)TRS_NPR_U.d;		//14=����Ud���
			*(RAM_START+((Uint32)RAM_BIAS * 15 + RAMDATA_POS)) = (int16)PI_NPR_Iq.reference;//15=����Iqָ��
			*(RAM_START+((Uint32)RAM_BIAS * 16 + RAMDATA_POS)) = (int16)PI_NPR_Iq.feedback;	//16=����Iq����
			*(RAM_START+((Uint32)RAM_BIAS * 17 + RAMDATA_POS)) = (int16)PI_NPR_Iq.out;		//17=����IqPI���
			*(RAM_START+((Uint32)RAM_BIAS * 18 + RAMDATA_POS)) = (int16)TRS_NPR_U.q;		//18=����Uq���
			*(RAM_START+((Uint32)RAM_BIAS * 19 + RAMDATA_POS)) = (int16)CAP4.mprtrstheta;	//19=MPR����Ƕ�
			*(RAM_START+((Uint32)RAM_BIAS * 20 + RAMDATA_POS)) = (int16)AD_OUT_MPR_I.a;		//20=MPR-A����
			*(RAM_START+((Uint32)RAM_BIAS * 21 + RAMDATA_POS)) = (int16)AD_OUT_MPR_I.b;		//21=MPR-B����
			*(RAM_START+((Uint32)RAM_BIAS * 22 + RAMDATA_POS)) = (int16)AD_OUT_MPR_I.c;		//22=MPR-C����
			*(RAM_START+((Uint32)RAM_BIAS * 23 + RAMDATA_POS)) = (int16)RUN.mpriqrf;		//23=MPR-Iq�ο�ֵ
			*(RAM_START+((Uint32)RAM_BIAS * 24 + RAMDATA_POS)) = (int16)CAP4.freqtmp;		//24=ʵ��ʵʱ��Ƶ
			*(RAM_START+((Uint32)RAM_BIAS * 25 + RAMDATA_POS)) = (int16)PRO.speed;			//25=���ת��

			*(RAM_END) = RAMDATA_POS;												//��ǰ���ݴ洢λ��ת��

			RAMDATA_POS++;
			if(RAMDATA_POS >= RAM_BIAS)  RAMDATA_POS=0;		
		}
  	}
	
	if(_STDBY1!=0)		
	{
		M_ClrFlag(SL_DRAW);							//�ⲿ���ñ�־1�����û�ͼʹ�ܱ�־
		*(RAM_END - 1) = 0x0000;
		MAIN_LOOP.cnt_datasave=0;					//20100129atbjtu
		_BA_EPGRID = 0;								//��¼����PDPӲ�����Ϸ�������,����2010atzuoyun
	} 
//--------------------------------External RAM Data Read-----------------------------------------
} 


/*********************************************************************************************************
** ��������: Draw
** �������: ���ͼ��
** �䡡��:
** �䡡��:        
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸��:
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
// No more.
//=========================================================================== 
