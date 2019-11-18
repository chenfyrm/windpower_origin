/****************************************Copyright (c)**************************************************
**                       		     ±±	¾©	½»	Í¨	´ó	Ñ§
**                                         µçÆø¹¤³ÌÑ§Ôº
**                                         604ÊµÑéÊÒ
**
**                              
**
**--------------ÎÄ¼þÐÅÏ¢--------------------------------------------------------------------------------
**ÎÄ   ¼þ   Ãû: user_main.c
**´´   ½¨   ÈË: 
**×îºóÐÞ¸ÄÈÕÆÚ: 
**Ãè        Êö: ÓÒÓñ1.5MWË«À¡·çµç»ú×é²¢Íø±äÁ÷Æ÷¿ØÖÆÈí¼þÖ÷³ÌÐò----201005atcpc-201007atbjtu
**              
**--------------ÀúÊ·°æ±¾ÐÅÏ¢----------------------------------------------------------------------------
** ´´½¨ÈË: 
** °æ  ±¾: 
** ÈÕ¡¡ÆÚ: 
** Ãè¡¡Êö: 
**
**--------------µ±Ç°°æ±¾ÐÞ¶©------------------------------------------------------------------------------
** ÐÞ¸ÄÈË: 
** ÈÕ¡¡ÆÚ: 
** Ãè¡¡Êö: 
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
/*---------------------×Ô´øÍ·ÎÄ¼þ-----------------------------*/
#include "C28x_FPU_FastRTS.h"
#include "DSP2833x_Device.h"     				// DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   				// DSP2833x Examples Include File
/*--------------------×Ô¶¨ÒåÍ·ÎÄ¼þ----------------------------*/
#include "user_header.h"  						//±äÁ¿³£Á¿¶¨Òå
#include "user_macro.h"							//ºêº¯Êý
#include "user_database.h"						//Êý¾Ý¿â   
#include "user_interface.c"						//½Ó¿Ú²ã
#include "user_work.c"							//¹¤×÷¿ØÖÆ
#include "math.h"

/*-----------------------ÖÐ¶ÏÉùÃ÷-----------------------------*/
interrupt void CpuTimer0Isr(void);				//Ö÷¶¨Ê±Æ÷ÖÐ¶Ï			
interrupt void EPWM1_TZ1_Isr(void);				//TZ1ÖÐ¶Ï
interrupt void EPWM2_TZ2_Isr(void);				//TZ2ÖÐ¶Ï
/*-----------------------º¯ÊýÉùÃ÷-----------------------------*/

void Protect(void);
void Scout(void);
void et_relay_N(void);
void et_relay_M(void);
void CntCtrl(void);
void SysCtrl(void);
void Bank(void);
void Display(void);
void BANK_Datasave(void);					
void ERROR_Datasave(void);					
void ACrowbar(void);					

// These are defined by the linker (see F28335_flash.cmd)
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
/*********************************************************************************************************
** º¯ÊýÃû³Æ: main
** ¹¦ÄÜÃèÊö: ÏµÍ³³õÊ¼»¯,Ö÷Ñ­»·
** Êä¡¡Èë: 
** Êä¡¡³ö:        
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void main(void)
{
//--------------------------------ÏµÍ³³õÊ¼»¯--------------------------------
	InitSysCtrl();
//---------------------------------IO³õÊ¼»¯---------------------------------
	InitGpio();	

	DINT;
   	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();
//--------------------------------ÍâÉè³õÊ¼»¯--------------------------------
	InitPeripherals(); 	
//---------------------------------Ð´FLASH³ÌÐò------------------------------
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	InitFlash();
//--------------------------------¹¤×÷³õÊ¼»¯--------------------------------
	InitWork();	
//---------------------------ÓÃ»§×Ô¶¨Òå³ÌÐòµÄ³õÊ¼»¯-------------------------
	InitEeprom();

//	InitRtimer();										//ÊµÊ±Ê±ÖÓ³õÊ¼»¯Ö»ÔÚÐ¾Æ¬ÐèÒª³õÊ¼»¯Ê±²Å½øÐÐ
//-------------------------------ÖÐ¶ÏµØÖ·³õÊ¼»¯-----------------------------
	EALLOW;  
	PieVectTable.TINT0       = &CpuTimer0Isr;		  	//¶¨Ê±Æ÷T0ÖÜÆÚÖÐ¶Ï
	PieVectTable.EPWM1_TZINT = &EPWM1_TZ1_Isr;      	//TZ1 ¹¦ÂÊ±£»¤ÖÐ¶Ï 
	PieVectTable.EPWM2_TZINT = &EPWM2_TZ2_Isr;      	//TZ2 ¹¦ÂÊ±£»¤ÖÐ¶Ï 

	EDIS;    
	
//------------------------------¼ÓÔØÖÐ¶ÏÊ¹ÄÜ¼Ä´æÆ÷--------------------------
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;		//CPUTIMER0ÖÜÆÚÖÐ¶Ï
    										//INT1(CPU)/INT1.7(PIE)
	PieCtrlRegs.PIEIER2.bit.INTx1 = 1;		//epwm1.tz
											//INT2(CPU)/INT2.1(PIE)
	PieCtrlRegs.PIEIER2.bit.INTx2 = 1;		//epwm2.tz 																	//INT1(CPU)/INT1.1(PIE)
												
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;  	//Ê¹ÄÜPIE   
    
   	IER |= (M_INT1|M_INT2);					//¼ÓÔØ×ÜÖÐ¶ÏÆÁ±Î¼Ä´æÆ÷
    
    EINT;   								//¿ª×ÜÖÐ¶Ï
    
//-------------------------------¶¨Ê±Æ÷¿ªÊ¼¹¤×÷-------------------------------
	CpuTimer0Regs.TCR.all = 0x4001; // Use write-only instruction to set TSS bit = 0//	M_StartCpuTimer0();												//Ö÷¶¨Ê±Æ÷¹¤×÷

//----------------------------------Ö÷Ñ­»·---------------------------------------------------------
	for(;;) 
	{				

//----------------------------------Ã¿´ÎÔËÐÐ----------------------------------
		EeCtrl();							//eeprom¿ØÖÆ
		Sci485Ctrl(); 
//		Sci_canopenrx();					//ÏµÍ³ÊäÈë¶ÁÈ¡	


//-----------------------------Ò»¼¶Ñ­»·5msÔËÐÐÒ»´Î----------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt1,DELAY_MAIN1)>=0)		
		{
			MAIN_LOOP.cnt1=0;				//Çå¿Õ¼ÆÊýÆ÷
			Input();						//ÏµÍ³ÊäÈë¶ÁÈ¡
	        M_NotFlag(SL_WATCHDOG);		    //DSPÈí¼þ¿´ÃÅ¹· 20100401at27
			Output();						//ÏµÍ³Êä³öÖ¸Ê¾
		}

//-----------------------------¶þ¼¶Ñ­»·10msÔËÐÐÒ»´Î---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt2,DELAY_MAIN2)>=0)
		{
			MAIN_LOOP.cnt2=0;
			if(M_ChkFlag(SL_CODEOK)!=0)		//ÔÚeepromÕý³£µÄÇé¿öÏÂ
			{
			     Give();					//ÔËÐÐÖ¸Áî¸ø¶¨
			}
		}

//-----------------------------Èý¼¶Ñ­»·20msÔËÐÐÒ»´Î---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt3,DELAY_MAIN3)>=0)
		{
			MAIN_LOOP.cnt3=0;
			SysCtrl();						//Ö÷¿Ø³ÌÐò
		}

//-----------------------------ËÄ¼¶Ñ­»·50msÔËÐÐÒ»´Î---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt4,DELAY_MAIN4)>=0)
		{
			MAIN_LOOP.cnt4=0;
		    Bank();							//ÏÔÊ¾¿ØÖÆ
		}

//----------------------------Îå¼¶Ñ­»·100msÔËÐÐÒ»´Î---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt5,DELAY_MAIN5)>=0)
		{					
			MAIN_LOOP.cnt5=0;
			if((M_ChkFlag(SL_POWERON)!=0)&&(M_ChkCounter(MAIN_LOOP.cnt_poweron,DELAY_POWERON)>0))
			{
				M_ClrFlag(SL_POWERON);
				ClrPdpint();						//PDPINTÖÐ¶ÏÇå¿Õ	
				EnPdpint();							//PDPINTÊ¹ÄÜÖÐ¶Ï
			}			
//20130801
			if(M_ChkFlag(SL_ENCODEPOS_OK)==0 && M_ChkFlag(SL_ECPOSSEARCH)!=0 && M_ChkFlag(SL_MPR_START)!=0)	//20130604
			{
				*FUNC[20].para_add = - QEPDATA.encodpos * 1000;	//ÐÞ¸ÄRAMºÍEEROM

				EEPROM.mcode=20;			//Ð´Èë2×Ö½ÚÊý¾Ý
				M_SetFlag(SL_EEASK_MCODE);			//ÉèEEPROMÐÞ¸Ä¹¦ÄÜÂëÇëÇó±êÖ¾
			}
		}
//-----------------------------Áù¼¶Ñ­»·1000msÔËÐÐÒ»´Î---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt6,DELAY_MAIN6)>=0)
		{
			MAIN_LOOP.cnt6=0;

			if(M_ChkFlag(SL_CODEOK)!=0)		
				RtRead();							//ÔÚeepromÕý³£µÄÇé¿öÏÂ//¶ÁÈ¡ÊµÊ±Ê±ÖÓ,ºÜºÄÊ±£¬Òª13ms.20090801,CPC				
//20121103										
//			if(M_ChkCounter(MAIN_LOOP.cnt_senszfstdy,DELAY_SENSZFSTDY)>=0)
//				M_SetFlag(SL_SENSZFSTDY);   		//ÑÓ³ÙÊ±¼äµ½ºóÖÃÁãÆ¯ÂË²¨ÎÈ¶¨±êÖ¾Î»

		}
	}  					
} 


/*********************************************************************************************************
** º¯ÊýÃû³Æ: CpuTimer0Isr
** ¹¦ÄÜÃèÊö: Ö÷¶¨Ê±Æ÷ÖÜÆÚÖÐ¶Ï(0.04ms)
** Êä¡¡Èë:
** Êä¡¡³ö:        
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
interrupt void CpuTimer0Isr(void)
{
    Uint16 i,k;
    
	M_SetFlag(SL_PHASEA);            							//²âÁ¿CPUÕ¼ÓÐÂÊ,²âÁ¿DSP°åÉÏT1¶Ë×Ó
	*OUT3_ADDR = _OUT3_DATA;
		
	if(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(SL_POWERON)==0)   //ÔÚºÏÖ÷¶ÏÇ°ÇÒÉÏµçê³ÉºóÍøÑ¹ÏàÐò¼ì²â×Óº¯Êý cap5-ubc   cap6-uab
	{
	  PhaseOrderChk();        
	} 
//	QepEncodPos(); 					
    CapQepCtrl();  		  										//²âÍøÑ¹ÏàÎ»£¬ÍøÑ¹ÆµÂÊ£¬×ø±êÕý¡¢·´±ä»»µÄ½Ç¶È
   	Ad8364Ctrl();             									//ÓÐ´«¸ÐÆ÷ºÍ»¥¸ÐÆ÷øÐAD²ÉÑùºÍ×ª»»
	ACrowbar();													//LVRT20100112
    PLLCtrl();		//201205LVRTatZBµ÷»»ÁË½Ç¶È»ý·ÖµÄÎ»ÖÃ20120527												//201011PLL
	QepEncodPos();		//¸ù¾Ýzb¾­Ñéµ÷»»ÁËÎ»ÖÃ20121103

    NPR_CONTROL();            									//Íø²à±äÁ÷Æ÷¿ØÖÆËã·¨
    MPR_CONTROL();            									//»ú²à±äÁ÷Æ÷¿ØÖÆËã·¨

	et_relay_N();												//Íø²à¹ýÁ÷·´Ê±ÏÞ¼ÆËã
	et_relay_M();												//»ú²à¹ýÁ÷·´Ê±ÏÞ¼ÆËã

	Da5344Manage();          	//DA output

//	testtheta = testtheta + 1;									//Èý½Ç²¨²âÊÔÐÅºÅ
//	if(testtheta>=3721)	testtheta= -1879;

//---------------------------------·ÖÊ±Âö³å¼ÆËã--------------------------------
	MAIN_LOOP.pulse++;
	if(MAIN_LOOP.pulse>=10)		MAIN_LOOP.pulse=0;
	
//--20¼¶·ÖÊ±Âö³å
	switch(MAIN_LOOP.pulse)
	{

//--¶¨Ê±Æ÷ÀÛ¼Ó,ÂýËÙAD,±£»¤Öµ¼ÆËã,¹ÊÕÏ
		case 0:
		{
			Protect();			   //protect calculation		
			Scout();			   //¹ÊÕÏ±£»¤
			BANK_Datasave();		//0.4msÒ»´Î
			break;
		}
		case 1:
		{	
			PwmDrive();	 		   //Êä³öÂö³å¿ØÖÆ  				
		    CntCtrl(); 				//¼ÆÊý1ms¼ÆÒ»¸öÊý£¬¶¨Ê±Æ÷ÖÐ¶ÏÖÜÆÚÎª200us
			ERROR_Datasave();		//0.4msÒ»´Î
			Sci_canopenrx();		//ÏµÍ³ÊäÈë¶ÁÈ¡120507
			break;
		}
		case 2:
		{
			Protect();			   //protect calculation		
			Scout();			   //¹ÊÕÏ±£»¤
			BANK_Datasave();		//0.4msÒ»´Î
			break;
		}
		case 3:
		{	
			PwmDrive();			   //Êä³öÂö³å¿ØÖÆ				
			RunCtrl();			   //¸ø¶¨»ý·Ö
			ERROR_Datasave();		//0.4msÒ»´Î
			break;
		}
		case 4:
		{	
			Protect();			   //protect calculation	
			Scout();			   //¹ÊÕÏ±£»¤
			BANK_Datasave();		//0.4msÒ»´Î
			break;
		}
		case 5:
		{	
			PwmDrive();			   //Êä³öÂö³å¿ØÖÆ				
			Display();
			ERROR_Datasave();		//0.4msÒ»´Î
			break;
		}
		case 6:
		{	
			CntCtrl(); 	           //¼ÆÊý£¬1ms¼ÆÒ»¸öÊý£¬¶¨Ê±Æ÷ÖÐ¶ÏÖÜÆÚÎª200us
			Protect();			   //protect calculation	
			Scout();			   //¹ÊÕÏ±£»¤
			BANK_Datasave();		//0.4msÒ»´Î
			break;
		}
		case 7:
		{
			PwmDrive();			   //Êä³öÂö³å¿ØÖÆ				
			ERROR_Datasave();	   //0.4msÒ»´Î
			break;
		}
		case 8:
		{	
			Protect();												//slow AD, protect calculation	
			Scout();												//¹ÊÕÏ±£»¤
			BANK_Datasave();		//0.4msÒ»´Î
			break;
		}
		case 9:
		{	
			PwmDrive();			   //Êä³öÂö³å¿ØÖÆ				
			RunCtrl();			   //¸ø¶¨»ý·Ö
			ERROR_Datasave();		//0.4msÒ»´Î
			break;
		}
//--------------------------------------------------------------------------------------------------
		default:
			break;
	}

//--Ó¦´ðÖÐ¶Ï20091109atzy
//	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	

//canopen
//-----------------------------canopenÔËÐÐÒ»´Î--120507----------------------------
	if(M_ChkCounter(MAIN_LOOP.canopen_tx,DELAY_CANOPENTX)>=0)    //250  6ms		
	{
		MAIN_LOOP.canopen_tx=0;				//Çå¿Õ¼ÆÊýÆ÷
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

			Sci_canopentx();				//12msÖ´ÐÐÒ»´Î
			M_SetFlag(SL_CANOPENTX);
			M_ClrFlag(SL_CANOPENTX_FLAG);		
		}
		else
		{
			Sci_canopentx_read();			//12msÖ´ÐÐÒ»´Î
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
		if(SciaRegs.SCIFFTX.bit.TXFFST == 0)						//·¢ËÍÍê³É?
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
//----------------------------------------------//121014byljd ¸ù¾ÝÊ©ÔÏÊÔÑé£¬ÔÚT0ÖÐ¶Ï¼ÓÈë¼°Ê±ÊÍ·Å×ÜÏß³ÌÐò
	if(M_ChkFlag(SL_TX)!=0)						//Sci.txb¼Ä´æÆ÷Êý¾Ý·¢ËÍÍê³Éºó£¬ÓÉÓÚÖ÷Ñ­»·ÓÐÆäËû³ÌÐòÃ¦£¬
 	{											//ÎÞ·¨½øÈëSci485Ctrl×Ó³ÌÐòÊÍ·ÅÍ¨Ñ¶×ÜÏß£¬µ¼ÖÂÍ¨Ñ¶ÖÐ¶Ï£¬
		if(M_ChkFlag(SL_TXLONGDATA)!=0)			//½«×ÜÏßÊÍ·Å×Ó³ÌÐòÒÆÖÁT0ÖÐ¶ÏÖÐ¼°Ê±ÊÍ·Å	120625
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
		else									//·¢ËÍÊý¾ÝÍê³É?
		{
			longtx_num = 15;

			if(ScibRegs.SCIFFTX.bit.TXFFST == 0)						//·¢ËÍÍê³É?Transmit FIFO is empty.
			{
				SCI.cnt_sciover=0;										//Çå³ý·¢ËÍ/½ÓÊÕ³¬Ê±¶¨Ê±Æ÷
				if(ScibRegs.SCICTL2.bit.TXEMPTY==1)						//·¢ËÍ¼Ä´æÆ÷Îª¿Õ?Transmitter buffer and shift registers are both empty
		 		{
					M_ClrFlag(SL_TX);
					Sci485_RxInit();									//485½ÓÊÕ³õÊ¼»¯
				}
			}

		}
 	}


//------------------------------------------------------------------------

    M_ClrFlag(SL_PHASEA);								//²âÁ¿CPUÕ¼ÓÐÂÊ,²âÁ¿DSP°åÉÏT1¶Ë×Ó
    *OUT3_ADDR = _OUT3_DATA;

//--Ó¦´ðÖÐ¶Ï20091109atzy
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	
}

/*********************************************************************************************************
** º¯ÊýÃû³Æ: EPWM1_TZ1_Isr
** ¹¦ÄÜÃèÊö: ¹¦ÂÊ±£»¤ÖÐ¶Ï
** Êä¡¡Èë:
** Êä¡¡³ö:   
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
interrupt void EPWM1_TZ1_Isr (void)
{
//----------------------------------------------//´¦Àí³ÌÐò

	Disepwmio_NPR();
	Disepwmio_MPR();
    DisPdpint();                
	M_SetFlag(SL_PDPINTA);
//----------------------------------------------//Ó¦´ðÖÐ¶Ï
	ClrPdpint();
	M_SetFlag(SL_DISPLAY3);    //LED3ºìµÆÁÁ
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: EPWM2_TZ2_Isr
** ¹¦ÄÜÃèÊö: ¹¦ÂÊ±£»¤ÖÐ¶Ï
** Êä¡¡Èë:
** Êä¡¡³ö:   
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
interrupt void EPWM2_TZ2_Isr (void)
{
//----------------------------------------------//´¦Àí³ÌÐò
	Disepwmio_NPR();
	Disepwmio_MPR();
    DisPdpint();     
	M_SetFlag(SL_PDPINTB);
//----------------------------------------------//Ó¦´ðÖÐ¶Ï
	ClrPdpint();
	M_SetFlag(SL_DISPLAY3);    //LED3ºìµÆÁÁ
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}


/*********************************************************************************************************
** º¯ÊýÃû³Æ: ACrowbar
** ¹¦ÄÜÃè: ActiveCrowbar¿ØÖÆ
** Êä¡¡Èë: 	 
** Êä  ³ö:   
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void ACrowbar(void)
{
    float NGS_U_D,NGS_U_Q;
//-------dc-chopper----------------¿ª¹ØÁ¿±¸ÓÃ7 
//	if((_COMMAND2&0x0080)==0 && M_ChkFlag(SL_ERRSTOP)==0 && M_ChkFlag(SL_SERIESTOP)==0)	
	if((_COMMAND2&0x0080)==0)	//20130301
//		M_CHOPPER_EN();			//DC-CHOPPERÇý¶¯Ê¹ÄÜÐÅºÅ£¬À­¸ßÊ¹ÄÜ£¬¿ªÍ¨¹Ø¶Ï»¹Óëµ×°åÖÍ»·VDC2½á¹ûÓÐ¹Ø
		M_SetFlag(CL_CHOPEN);
	else
//		M_CHOPPER_DISEN();		//DC-CHOPPERÇý¶¯Ê¹ÄÜÐÅºÅ£¬À­µÍ½ûÖ¹
		M_ClrFlag(CL_CHOPEN);


//-------------------LV ¼ì²âµçÍøµøÂä-----------------
	
//	if(M_ChkFlag(SL_CBCLOSED)!=0)
	if(M_ChkFlag(SL_IN1_MIANK)!=0)			//20121124
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_lv_test,DELAY_LV_TEST)>=0)			//1s
			M_SetFlag(SL_LV_TEST);
	}
	else	
	{
		MAIN_LOOP.cnt_lv_test=0;
		M_ClrFlag(SL_LV_TEST);
	}

//	NGS_Udq    = sqrt(TRS_NGS_U.dpll * TRS_NGS_U.dpll + TRS_NGS_U.qpll * TRS_NGS_U.qpll); //¼ÆËãÍøÑ¹·ùÖµ-½öÊÊÓÃÓÚÕýÐò
//	NGS_Udq_lv = sqrt(TRS_NGS_U.dnpr * TRS_NGS_U.dnpr + TRS_NGS_U.qnpr * TRS_NGS_U.qnpr);  //¼ÆËãÍøÑ¹·ùÖµ-½öÊÊÓÃÓÚÕýÐò-µøÂä

	NGS_Udq = sqrt(TRS_NGS_U.dflt * TRS_NGS_U.dflt + TRS_NGS_U.qflt * TRS_NGS_U.qflt);  //20110326¼ÆËãÍøÑ¹·ùÖµ-½öÊÊÓÃÓÚÕýÐò-µøÂä
//  ÓÉPLLÊä³öµÄNGS_Udq_pÌæ´úBJTULVRT201204

	NGS_U_D    = 0.3333333 * (2 * AD_OUT_NGS_U.a - AD_OUT_NGS_U.b - AD_OUT_NGS_U.c);	//ÓÃ¾²Ö¹×ø±ê·ÖÁ¿¼ÆËãÍøÑ¹·ùÖµ
	NGS_U_Q    = SQRT3_3 * (AD_OUT_NGS_U.b - AD_OUT_NGS_U.c);
	TRS_NPR_U.amp = sqrt(NGS_U_D * NGS_U_D + NGS_U_Q * NGS_U_Q);   //20110328

	DataFilter(0.4,&TRS_NPR_U.ampflt,TRS_NPR_U.amp); 			//c=0.4,Ts=200us,fh=1.2kHz,ÂËµô¿ª¹ØÆµÂÊ´Î
	if(M_ChkFlag(SL_LV_QWORKING)==0)	DataFilter(0.998,&NGS_Udq_pflt,NGS_Udq_p);		//100ms	20130303

//-----¼ÆËãÎÞ¹¦²¹³¥ÏµÊýkq-----BJTULVRT201204
	if(M_ChkFlag(SL_UNBALANCE)==0)
//		GRD_Ut = NGS_Udq / GRD_UN;
		GRD_Ut = NGS_Udq / NGS_Udq_p_ex;	//20130301
	else
//		GRD_Ut = NGS_Udq_p / GRD_UN;
		GRD_Ut = NGS_Udq_p / NGS_Udq_p_ex;	//20130301
//	if(GRD_Ut <= 0.9)	kq = 1.5 * (1 - NGS_Udq_p / GRD_UN);	//201205LVRTatZB
//	else				kq = 0;		//20120531 201205LVRTatZB
//--------20130225--------
	if(GRD_Ut <= 0.94)							//20130124	20130303
//	if(NGS_Udq <= 0.94 * NGS_Udq_p_ex)							//20130301
	{
//		kq = 1.5 * (1 - NGS_Udq_p / GRD_UN);	//201205LVRTatZB	·ÀÖ¹ÍøÑ¹¸ßÊ±kqÌø±ä20130124
		kq = 1.5 * (1.05 - NGS_Udq_p / NGS_Udq_p_ex);	//20130301
		MAIN_LOOP.cnt_lv_kq = 0;				//20130124
	}
	else if(M_ChkCounter(MAIN_LOOP.cnt_lv_kq,30)>0)
			kq = 0;		//20120531 201205LVRTatZB
//------------------------

	if(M_ChkFlag(SL_LV_TEST)!=0)	//20120601night
	{
		if(M_ChkFlag(SL_LV_STATE)!=0)	MAIN_LOOP.cnt_lv_test2=0;	

	  	if(M_ChkCounter(MAIN_LOOP.cnt_lv_test2,DELAY_LVTEST2)>0)	//1s
	  	{
//			if(M_ChkFlag(SL_LV_STATE)==0 && (TRS_NPR_U.ampflt<(0.91 * GRD_UN)))
//			if(M_ChkFlag(SL_LV_STATE)==0 && (TRS_NPR_U.ampflt<(0.91 * NGS_Udq_p_ex)))		//20130301
//			if(M_ChkFlag(SL_LV_STATE)==0 && (TRS_NPR_U.ampflt<(_stdby01 * NGS_Udq_p_ex)))		//20130309
			if(M_ChkFlag(SL_LV_STATE)==0 && (TRS_NPR_U.ampflt<(0.8 * NGS_Udq_p_ex)))		//90%²»ÐèÒª·¢ÎÞ¹¦20130801
			{
				M_SetFlag(SL_LV_STATE);

				if((_STDBY9&0x0002)==0)		M_SetFlag(CL_ZKLVRT);		//20121107
			}
	  	}	

//		if(M_ChkFlag(SL_LV_STATE)!=0 && (NGS_Udq_p > (0.92 * GRD_UN)))
		if(M_ChkFlag(SL_LV_STATE)!=0 && (NGS_Udq_p > (0.92 * NGS_Udq_p_ex)))		//20130301
		{		
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_rcv,DELAY_LVRCV)>0)			//10ms
			{
				M_ClrFlag(SL_LV_STATE);			

				if((_STDBY9&0x0002)==0)		M_ClrFlag(CL_ZKLVRT);		//20121107

				if(M_ChkCounter(MAIN_LOOP.cnt_lv_recmstop,50)<=0)		//Îó´¥·¢ÎÞÐèÑÓÊ±¿ÉÁ¢¼´ÖØÐÂÖÃÎ»20130303
				{
					if(NGS_Udq_epsilon < 2.5)	MAIN_LOOP.cnt_lv_test2 = 20000;						//Ó¦´óÓÚDELAY_LVTEST2
					M_SetFlag(SL_LV_CLRERRAM);		//Çå³ýRAMÄÚ²¨ÐÎ		20130306
				}
			}
		}
		else	MAIN_LOOP.cnt_lv_rcv = 0;
		
	}
	else	
	{
		M_ClrFlag(SL_LV_STATE);	
		if((_STDBY9&0x0002)==0)		M_ClrFlag(CL_ZKLVRT);		//20121107
		MAIN_LOOP.cnt_lv_rcv = 0;
		MAIN_LOOP.cnt_lv_test2=0;
	} 
//--------20130225--------
    if(M_ChkFlag(SL_LV_STATE)!=0)		//20130128
	{

	  	if(M_ChkCounter(MAIN_LOOP.cnt_lv_dlrecmstop,3000)>0)	//3s	20130221
	  	{
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_recmstop,500)>=0)	//500ms
			{
//				if(((NGS_Udq_p - NGS_Udq_p_lv) > 55) && (NGS_Udq_p_lv < (0.85 * NGS_Udq_p_ex)))	//3ph-75,2ph-50 20130205
				if(((NGS_Udq_p - NGS_Udq_p_lv) > 55) && ((NGS_Udq_p_lv < (0.82 * NGS_Udq_p_ex))||(NGS_Udq_epsilon_lv > 19)))	//3ph/2ph-50¼°ÒÔÏÂ 20130301
				{
					M_SetFlag(SL_LV_RECMSTOP);					
					M_SetFlag(SL_LV_RECNSTOP);		//µøÂäË²¼äÖ÷¶¯·âÍø²àÂö³å20130202
				}
			}
		}
	}
	else
	{
		MAIN_LOOP.cnt_lv_recmstop = 0;
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
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scrkeepon,_stdby06)>=0)	M_ClrFlag(SL_LV_SCRKEEPON);
		}
	}
	else
	{
		M_ClrFlag(SL_LV_SCRKEEPON);
		MAIN_LOOP.cnt_lv_scrkeepon = 0;
	}
//------------------------


/*20120601night
 	if((_COMMAND2&0x0020)!=0)	//¿ª¹ØÁ¿±¸ÓÃ5  201011LVRT
	{
		LV_A1 = 0.9; 			//E.on2003±ê×¼
		LV_A2 = 0.15;
		LV_T1 = 625;
		LV_T2 = 3000;
	}
	else						//Ä¬ÈÏÎªµç¿ÆÔºµÄ¹ú¼Ò±ê×¼
	{
		LV_A1 = 0.9; //_stdby02  testing
		LV_A2 = 0.1; //·Åµ×Ð©£¬±ðÇáÒ×±£»¤ 
		LV_T1 = 625;
		LV_T2 = 2000;	
//		LV_A2 = 0;			////·Å¿íÌõ¼þ,LVÊµÑé²»Í£»ú 20110326
//		LV_T1 = 1500;
//		LV_T2 = 4000;	
	}

	if(M_ChkFlag(SL_LV_TEST)!=0)
	{
//		if(M_ChkFlag(SL_LV_TEST)!=0 && NGS_Udq_lv < (LV_A1 * GRD_UN))			//0.9 GRD_UN=¶î¶¨µçÍøÏàµçÑ¹·åÖµ 690->563V; 380->310V
		if(M_ChkFlag(SL_LV_TEST)!=0 && TRS_NPR_U.ampflt < (0.9 * GRD_UN))			//GRD_UN=¶î¶¨µçÍøÏàµçÑ¹·åÖµ 690->563V; 380->310V LVRTNR201103
		{																//µøÂä¼ì²â:NGS_Udq_lvÑÓÊ±1ms,NGS_UdqÒª10ms
				//--------------------LVRTÇøÓòÅÐ¶Ï--------------------------
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
//							M_SetFlag(SL_UACLV2);							//Ìí¼Ó200msÑÓÊ±
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
//							M_SetFlag(SL_UACLV2);							//Ìí¼Ó100msÑÓÊ±
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
				//--------------------LVRTÇøÓòÅÐ¶Ï--------------------------
			MAIN_LOOP.cnt_lv_rcv=0;	
		}
		else if(NGS_Udq_p > (0.9 * GRD_UN))									//µçÍøÕý³£»òÕß
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_rcv,DELAY_LVRCV)>0)			//20ms
			{
			 	M_ClrFlag(SL_LV_STATE);										//»Ö¸´£¡
			 	M_ClrFlag(SL_UACLV2);		
			}
			MAIN_LOOP.cnt_lv_time=0;
			MAIN_LOOP.cnt_lv_fault1=0;	
			MAIN_LOOP.cnt_lv_fault2=0;		
		}		
		else	
		{
			MAIN_LOOP.cnt_lv_rcv=0;	
			MAIN_LOOP.cnt_lv_time=0;
			MAIN_LOOP.cnt_lv_fault1=0;	
			MAIN_LOOP.cnt_lv_fault2=0;	
		} 
	}
	else
	{
		M_ClrFlag(SL_LV_STATE);										
		M_ClrFlag(SL_UACLV2);	
		MAIN_LOOP.cnt_lv_rcv=0;	
		MAIN_LOOP.cnt_lv_time=0;
		MAIN_LOOP.cnt_lv_fault1=0;	
		MAIN_LOOP.cnt_lv_fault2=0;	
	}
*/
//------------------ÅÐ¶ÏµøÂäÊ±ÍøÑ¹µÄ²»Æ½ºâ¶È--20110326---------------------------------------------------------------
	if(M_ChkFlag(SL_LV_TEST)!=0 && M_ChkFlag(SL_LV_STATE)!=0)
	{
		if(NGS_Udq_epsilon > 10)			
//		if(NGS_Udq_epsilon > _stdby05)	//Íâ²¿¸ø¶¨201011LVRT LVRTNR201103
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_unbalance,DELAY_LVUBALNCE)>0)	M_SetFlag(SL_UNBALANCE);	
														//30ms <50ms ÒªÐ¡ÓÚMPR»òNPRÁÙÊ±·âÂö³åÊ±¼ä
		}
		else if(NGS_Udq_epsilon < 6)	
//		else if(NGS_Udq_epsilon < (_stdby05-5))	
		{
			M_ClrFlag(SL_UNBALANCE);
			MAIN_LOOP.cnt_lv_unbalance=0;
		}
		else	MAIN_LOOP.cnt_lv_unbalance=0;
	}
	else	
	{
		M_ClrFlag(SL_UNBALANCE);
		MAIN_LOOP.cnt_lv_unbalance=0;
	}

//¿ª¹ØÁ¿±¸ÓÃ6
/*	if((_COMMAND2&0x0040)!=0)	
	{
		if(M_ChkFlag(SL_LV_STATE)!=0)	M_SetFlag(SL_UNBALANCE);//=1ÊÇÈÏÎªÊÇ²»Æ½ºâµøÂä,LV»ú²àÍø²à½ûÖ¹Æô¶¯£¡201011LVRT
		else							M_ClrFlag(SL_UNBALANCE);	//20110327
	}
*/
//----------20130225²ÉÓÃµçÑ¹µøÂäÅÐ¶Ï´¥·¢CROWBAR£¬¼Ó¿ìÍ¶ÈëËÙ¶È-------------
	if(M_ChkFlag(SL_LV_STATE)!=0)
	{
	 if(M_ChkCounter(MAIN_LOOP.cnt_jgvdscron,30)<=0)		//30msÅÐ¶Ï		20130205
	 {
		if(M_ChkCounter(MAIN_LOOP.cnt_vdscron,DELAY_VDSCRON)<=0)
		{
//			if(NGS_Udq<300)		M_SetFlag(SL_LV_VDSCRON);
//			if(NGS_Udq<340)		//20130202 20%,35%
//			if(NGS_Udq<(NGS_Udq_p_ex * 0.5))		//20130221 20%(2ph/3ph),35%(3ph)
//			if(NGS_Udq<(NGS_Udq_p_ex * 0.41))		//20130223 20%(2ph/3ph),35%(3ph)
//			if((NGS_Udq<(NGS_Udq_p_ex * 0.41)) && (PRO.Pgactive_lv>600000))		//20130228 20%(2ph/3ph),35%(3ph),fullpower
			if(NGS_Udq<(NGS_Udq_p_ex * 0.65))		//20130228 20%(2ph/3ph),35%(3ph),Ö÷¶¯Í¶	20130308
			{
//				M_SetFlag(SL_LV_VDSCRON);
				if((_STDBY9&0x0100)==0)		M_SetFlag(SL_LV_VDSCRON);		//116.8	20130223
			}
//			if(NGS_Udq<(NGS_Udq_p_ex * 0.95))		//20130206 20%,35%,50%
			if((NGS_Udq<(NGS_Udq_p_ex * 0.95))||(PRO.NPR_iac>150.0))		//20130228 20%,35%,50%
			{
				if(M_ChkFlag(SL_LV_NSTOP)==0)	M_SetFlag(SL_LV_ZDNSTOP);		//µøÂäË²¼äÖ÷¶¯·âÍø²àÂö³å20130202
			}
//			if(NGS_Udq<(NGS_Udq_p_ex * 0.7))		//20130221 50%¼°ÒÔÏÂËùÓÐ¹¤¿ö
			if((NGS_Udq<(NGS_Udq_p_ex * 0.8))||(PRO.MPR_iac>150.0))		//20130228 50%¼°ÒÔÏÂËùÓÐ¹¤¿ö
//			if((NGS_Udq<(NGS_Udq_p_ex * 0.9))||(PRO.MPR_iac>1000.0))		//20130228 50%¼°ÒÔÏÂËùÓÐ¹¤¿ö
			{
				if(M_ChkFlag(SL_LV_MSTOP)==0)	M_SetFlag(SL_LV_ZDMSTOP);		//µøÂäË²¼äÖ÷¶¯·âÍø²àÂö³å20130202
			}
			if(NGS_Udq>520)		MAIN_LOOP.cnt_vdscron = 0;		//20130205
			
			MAIN_LOOP.cnt_vdscroff=0;
		}
		else if(M_ChkCounter(MAIN_LOOP.cnt_vdscroff,DELAY_VDSCROFF)>=0)
		{
			M_ClrFlag(SL_LV_VDSCRON);
		}
	 }
	}
	else
	{
		M_ClrFlag(SL_LV_VDSCRON);
		MAIN_LOOP.cnt_vdscron = 0;
		MAIN_LOOP.cnt_jgvdscron = 0;	//20130205
	}

//-------------------¾ø¶ÔÑÓÊ±°æ±¾--LV MPR·âÂö³å¿ØÖÆ-----------------
//	if(M_ChkFlag(SL_LV_MSTOP)==0 && M_ChkFlag(SL_LV_STATE)!=0 &&((PRO.MPR_iac>_SC_LVIAC2) || (PRO.udc>_SC_LVUDC1) || (M_ChkFlag(SL_LV_SCRRUNING)!=0) || (M_ChkFlag(SL_LV_VDSCRON)!=0)))   //20110326mprÈí¼þ¹ýÁ÷»òÕßVdc¹ýÑ¹»òµçÍø²»Æ½ºâ 201012
	if(M_ChkFlag(SL_LV_MSTOP)==0 && M_ChkFlag(SL_LV_STATE)!=0 &&((PRO.MPR_iac>_SC_LVIAC2) || (PRO.udc>_SC_LVUDC1) || (M_ChkFlag(SL_LV_SCRRUNING)!=0) || (M_ChkFlag(SL_LV_VDSCRON)!=0) || (M_ChkFlag(SL_LV_RECMSTOP)!=0) || (M_ChkFlag(SL_LV_ZDMSTOP)!=0)))   //20110326mprÈí¼þ¹ýÁ÷»òÕßVdc¹ýÑ¹»òµçÍø²»Æ½ºâ 20130225
	{
		M_SetFlag(SL_LV_MSTOP);	
		M_ClrFlag(SL_LV_ZDMSTOP);		//20130221	20130225
		MAIN_LOOP.cnt_mpwmrestart=0;									//LVRT»ú²àÂö³åÍ£Ö¹
		Disepwmio_MPR();       											//201011LVRT
		M_ClrFlag(SL_MPR_PWMOUT);
		M_ClrFlag(SL_LV_MPWMOUT);

		if(M_ChkFlag(SL_LV_RECMSTOP)!=0)	MAIN_LOOP.cnt_lv_dlrecmstop=0;	//20130201 »Ö¸´Ê±Ö»·âÒ»´Î
		M_ClrFlag(SL_LV_RECMSTOP);		//20130130 Âö³åÖ»·âÒ»´Î
	}
	else if(M_ChkFlag(SL_LV_MSTOP)!=0)									//20110829¾¡¿ì»Ö¸´Àø´Å
//	else if(M_ChkFlag(SL_LV_MSTOP)!=0 && M_ChkFlag(SL_UNBALANCE)==0)	//LVRTNR201103
	{
//		if(M_ChkCounter(MAIN_LOOP.cnt_mpwmrestart,DELAY_MPWMRESTART)>=0)		//50ms
		if(M_ChkCounter(MAIN_LOOP.cnt_mpwmrestart,_stdby07)>=0)					//50ms
		{
			if((PRO.MPR_iac<(_SC_LVIAC2-20)) && (PRO.udc<(_SC_LVUDC1-5)) && (M_ChkFlag(SL_LV_SCRRUNING)==0))		//20121210	Ð¡¹¦ÂÊ¿ÉÒÔÖØÆô20130228
//			if(PRO.MPR_iac<(_SC_LVIAC2-20) && PRO.udc<(_SC_LVUDC1-5) && M_ChkFlag(SL_LV_SCRRUNING)==0 && M_ChkFlag(SL_UNBALANCE)==0)
//				M_ClrFlag(SL_LV_MSTOP);				  					//»ú²àLVRT¼°Æä¹ý¶É½áÊøÇÒ²»Æ½ºâÏûÊ§£¬Á¢¼´ÔÊÐíÖØÆôÂö³å20110829
//--------³ýÁË´ó¹¦ÂÊ2Ïà20¡¢35Çé¿ö£¬ÆäÓà¿ÉÖØÆôÂö³å20130228--------
			{
//				if(M_ChkFlag(SL_UNBALANCE)==0)	M_ClrFlag(SL_LV_MSTOP);
				if((M_ChkFlag(SL_UNBALANCE)==0) && (NGS_Udq_epsilon<23))	M_ClrFlag(SL_LV_MSTOP);		//20130306
//				else if((PRO.Pgactive_lv<600000) || (NGS_Udq_epsilon<27))
//				else if(((PRO.Pgactive_lv<600000) || (NGS_Udq_epsilon<27)) && ((_STDBY9&0x0080)!=0))	//116.7		20130303
				else if((NGS_Udq_epsilon<31) && (NGS_Udq_p>(NGS_Udq_p_ex * 0.70)))	//50%¿ÉÆôÂö³å		20130308
					M_ClrFlag(SL_LV_MSTOP);
			}
//------------------------
		}
	}
	else	MAIN_LOOP.cnt_mpwmrestart=0;

//------------------ÎÞ¹¦¿ØÖÆ--ÐÞ¸Ä£¬Î´ÑéÖ¤£¡Óë×ª¾ØÇåÁãÓÐ¹Ø20110327-------------------------
	if(M_ChkFlag(SL_LV_STATE)!=0)//20110829HC
//	if(M_ChkFlag(SL_LV_TEST)!=0 && (NGS_Udq < (0.9 * GRD_UN)))//20111201bjtu½»´óÏÖÐÐ°æ±¾
//	if(M_ChkFlag(SL_LV_TEST)!=0 && M_ChkFlag(SL_LV_STATE)!=0 && (NGS_Udq < (0.7 * GRD_UN)))//20110327
//	if(M_ChkFlag(SL_LV_TEST)!=0 && M_ChkFlag(SL_LV_STATE)!=0 &&(NGS_Udq < (0.8 * GRD_UN)) && M_ChkFlag(SL_LV_MSTOP)==0 && M_ChkFlag(SL_LV_NSTOP)==0)//20110327
//	if(M_ChkFlag(SL_LV_TEST)!=0 && M_ChkFlag(SL_LV_STATE)!=0 &&(NGS_Udq < (0.8 * GRD_UN)) && M_ChkFlag(SL_LV_MSTOP)==0 && M_ChkFlag(SL_LV_NSTOP)==0 && M_ChkFlag(SL_UNBALANCE)==0)//LVRTNR201103²»Æ½ºâÌõ¼þ
	{
		M_SetFlag(SL_TRIG_ERRDSAVE);	 	//LVRT´¥·¢¹ÊÕÏËø´æÊ¾²¨Æ÷201205LVRTatZB
		M_SetFlag(SL_LV_QWORKING);												//Íø²à·¢ÎÞ¹¦
		M_SetFlag(SL_LV_STRICTLV);			//20130222
		GIVE.lvwtiqrf = kq * 1775.0;	//BJTULVRT201204
		if(GIVE.lvwtiqrf > 1775)	GIVE.lvwtiqrf = 1775; //ÎÞ¹¦µçÁ÷ÏÞ·ùÏÞ·ù
		MAIN_LOOP.cnt_lv_qworking = 0;	

//--------20130222	20130225--------
		if(M_ChkCounter(MAIN_LOOP.cnt_lv_recmstop,500)>=0)	//500ms
		{
			if(((NGS_Udq_p - NGS_Udq_p_lv) > 55) || (NGS_Udq_p > (0.95 * NGS_Udq_p_ex)))
				M_ClrFlag(SL_LV_STRICTLV);
		}
//-------------------------
	}
//	else if(NGS_Udq > (_stdby01 * GRD_UN))//!!!!!!!!!!!
//	else if(NGS_Udq > (0.9 * GRD_UN))//!!!!!!!!!!!
	else	//20110829HC
	{
//		M_ClrFlag(SL_LV_QWORKING);												//Íø²àÍ£Ö¹·¢ÎÞ¹¦,»Ö¸´ÓÐ¹¦
		GIVE.lvwtiqrf = 0;
		if(M_ChkCounter(MAIN_LOOP.cnt_lv_qworking,DELAY_LVQWORKING)>=0)			//60ms 201205LVRTatZB
			M_ClrFlag(SL_LV_QWORKING);											//Íø²àÍ£Ö¹·¢ÎÞ¹¦,»Ö¸´ÓÐ¹¦
		M_ClrFlag(SL_LV_STRICTLV);									//20130301
	}

//-------------------LV NPR·âÂö³å¿ØÖÆ----------------------------------------------
	if(M_ChkFlag(SL_LV_NSTOP)==0 && M_ChkFlag(SL_LV_STATE)!=0 && ((PRO.NPR_iac>(_SC_LVIAC2-35)) || M_ChkFlag(SL_LV_ZDNSTOP)!=0 || M_ChkFlag(SL_LV_RECNSTOP)!=0)) //20130206atzb	
//	if(M_ChkFlag(SL_LV_NSTOP)==0 && M_ChkFlag(SL_LV_STATE)!=0 && (PRO.NPR_iac>(_SC_LVIAC2-15))) //20110326²»Æ½ºâ·âÂö³å LVRTNR201103
//	if(M_ChkFlag(SL_LV_NSTOP)==0 && M_ChkFlag(SL_LV_STATE)!=0 && (PRO.NPR_iac>_SC_LVIAC2 || M_ChkFlag(SL_UNBALANCE)!=0)) //²»Æ½ºâ·âÂö³å LVRTNR201103
//	if(M_ChkFlag(SL_LV_NSTOP)==0 && (PRO.NPR_iac>_SC_LVIAC2 || M_ChkFlag(SL_UNBALANCE)!=0)) //²»Æ½ºâ·âÂö³å LVRTNR201103
//	if(M_ChkFlag(SL_LV_NSTOP)==0 &&(PRO.NPR_iac>_SC_LVIAC2)) //ø²àÈí¼þ¹ýÁ÷ÇÒ»ú²àÒÑ·âÂö³åÇÒ´¦ÓÚLV
	{
		M_SetFlag(SL_LV_NSTOP);
		M_ClrFlag(SL_LV_ZDNSTOP);		//20130202
		MAIN_LOOP.cnt_npwmrestart=0;									//LVRTÍø²àÂö³åÍ£Ö¹	

		if(M_ChkFlag(SL_LV_RECNSTOP)!=0)	MAIN_LOOP.cnt_lv_dlrecmstop=0;	//20130206 »Ö¸´Ê±Ö»·âÒ»´Î
		M_ClrFlag(SL_LV_RECNSTOP);		//20130206 Âö³åÖ»·âÒ»´Î
	}
//	else if(M_ChkFlag(SL_LV_NSTOP)!=0 && (PRO.NPR_iac<(_SC_LVIAC2-20)) && M_ChkFlag(SL_UNBALANCE)==0) //LVRTNR201103
	else if(M_ChkFlag(SL_LV_NSTOP)!=0)			//20110829
	{
//		if(M_ChkCounter(MAIN_LOOP.cnt_npwmrestart,DELAY_NPWMRESTART)>=0)		//50ms
//		if(M_ChkCounter(MAIN_LOOP.cnt_npwmrestart,_stdby04)>=0)					//BJTULVRT201204
		if(M_ChkCounter(MAIN_LOOP.cnt_npwmrestart,5)>=0)					//20130309atsp
		{
//			 if(PRO.NPR_iac<(_SC_LVIAC2-20) && M_ChkFlag(SL_UNBALANCE)==0)
			 if(PRO.NPR_iac<(_SC_LVIAC2-45))				//20121226
				M_ClrFlag(SL_LV_NSTOP);				  							//»ú²àLVRT¼°ÆäøÇÒ²»Æ½ºâÏûÊ§£¬Á¢¼´ÔÊÐíÖØÆôÂö³å20110829
		}
	}
	else MAIN_LOOP.cnt_npwmrestart=0;

//-------------------LV SCRµçÁ÷µ½ÁãÅÐ¶Ï£¨¼ì²âË³Ê±Öµ£©-20110829HC----------------
	if((abs(AD_OUT_SCR_I.a)>35) || (abs(AD_OUT_SCR_I.b)>35))	//SCRµçÁ÷½Ó½üÁã   						
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_scrizero2,DELAY_SCRIZERO2)>=0)	M_ClrFlag(SL_LV_SCRIZERO);	//1ms
		MAIN_LOOP.cnt_scrizero=0;	
	}
	else if((abs(AD_OUT_SCR_I.a)<25) && (abs(AD_OUT_SCR_I.b)<25))	//201007BJTULVRT?????I-LEMÁãÆ¯Îó²î¶à´ó?
	{	
//		if(M_ChkCounter(MAIN_LOOP.cnt_scrizero,DELAY_SCRIZERO)>=0)	M_SetFlag(SL_LV_SCRIZERO);	  //15ms Í¨¹ýµç÷ÅÐ¶ÏSCRÊÇ·ñ¶¼¹Ø¶Ï,DELAY_SCRIZEROÑÓ¿¿ÊµÑéÈ·¶¨ 
//		if(M_ChkCounter(MAIN_LOOP.cnt_scrizero,_stdby06)>=0)	M_SetFlag(SL_LV_SCRIZERO);	  //201007BJTULVRT Í¨ýµçÁ÷ÅÐ¶ÏSCRÊÇ·ñ¶¼¹Ø¶Ï,DELAY_SCRIZEROÑÓÊ±¿¿ÊµÑéÈ·¶¨ 
		if(M_ChkCounter(MAIN_LOOP.cnt_scrizero,1)>=0)	M_SetFlag(SL_LV_SCRIZERO);	  //201007BJTULVRT Í¨ýµçÁ÷ÅÐ¶ÏSCRÊÇ·ñ¶¼¹Ø¶Ï,DELAY_SCRIZEROÑÓÊ±¿¿ÊµÑéÈ·¶¨ 
//		else	M_ClrFlag(SL_LV_SCRIZERO);								 
		MAIN_LOOP.cnt_scrizero2=0;							 
    }
	else	
	{
		MAIN_LOOP.cnt_scrizero=0;
		MAIN_LOOP.cnt_scrizero2=0;							 
	}

//---------------Crowbar¿ØÖÆ¸Ä½ø20110829HC----------------------------------
/*	if(M_ChkFlag(SL_LV_STATE)!=0)	//²»¶Ô³ÆµøÂäcrowbar¿ªÍ¨µÚ3´ÎÖ®ºóÒ»Ö±¿ªÍ¨µ½LV½áÊø20120602
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
//-------------------LV SCR¶¯×÷VdcÌõ¼þÅÐ¶Ï-----------------
//	if(((PRO.udc>1400) || (PRO.udc>_SC_LVUDC2 && M_ChkFlag(SL_LV_STATE)!=0))&& M_ChkFlag(SL_LV_SCRON)==0)
/*	if(((PRO.udc>1400) || ((PRO.udc>_SC_LVUDC2 || PRO.MPR_iac>(_SC_LVIAC2+15)) && M_ChkFlag(SL_LV_STATE)!=0) || M_ChkFlag(SL_LV_VDSCRON)!=0)&& M_ChkFlag(SL_LV_SCRON)==0) 
												//20120603night¶þ¼«¹Ü¹ýÁ÷Ò²´¥·¢crowbar	20121203
	{
		M_SetFlag(SL_LV_SCRON);							//¿ªÆôSCR¶¯×÷Ö¸Áî
		MAIN_LOOP.cnt_lv_scron=0;						//ÖØÒª£º±£Ö¤ÏÂ´ÎSCR¿ªÍ¨ÒÀÈ»ÑÓÊ±_SC_SCRON2010115ÐÞ¸Ä
		CNT_SCRON++;		//20120602					
	}
	else if(PRO.udc< (_SC_LVUDC2-30))
	{
		M_ClrFlag(SL_LV_SCRON);							//¹Ø¶ÏSCRÖ¸Áî	
	}
*/
//--------20130225--------------
	if(M_ChkFlag(SL_LV_SCRDONE)==0)		//20130221
	{
		if(((PRO.udc>1400) || ((PRO.udc>_SC_LVUDC2 || PRO.MPR_iac>(_SC_LVIAC2+15)) && M_ChkFlag(SL_LV_STATE)!=0) || M_ChkFlag(SL_LV_VDSCRON)!=0)&& M_ChkFlag(SL_LV_SCRON)==0) 
		{
			M_SetFlag(SL_LV_SCRON);							//¿ªÆôSCR¶¯×÷Ö¸Áî
			MAIN_LOOP.cnt_lv_scron=0;						//ÖØÒª£º±£Ö¤ÏÂ´ÎSCR¿ªÍ¨ÒÀÈ»ÑÓÊ±_SC_SCRON2010115ÐÞ¸Ä
		}
		else if(PRO.udc< (_SC_LVUDC2-30))
		{
			M_ClrFlag(SL_LV_SCRON);
		}
	}
	else
	{
		if(((PRO.udc>1400) || ((PRO.udc>1270 || PRO.MPR_iac>(_SC_LVIAC2+15)) && M_ChkFlag(SL_LV_STATE)!=0))&& M_ChkFlag(SL_LV_SCRON)==0) 
		{
			M_SetFlag(SL_LV_SCRON);							//¿ªÆôSCR¶¯×÷Ö¸Áî
			MAIN_LOOP.cnt_lv_scron=0;						//ÖØÒª£º±£Ö¤ÏÂ´ÎSCR¿ªÍ¨ÒÀÈ»ÑÓÊ±_SC_SCRON2010115ÐÞ¸Ä
		}
		else if(PRO.udc< (_SC_LVUDC2-30))
		{
			M_ClrFlag(SL_LV_SCRON);
		}
	}

//-------------------LV SCR¿ØÖÆ-²ÉÓÃºóÐë½«»ú²àÁÙÊ±·âÂö³åµÄSL_LV_SCRON»»³ÉSL_LV_SCRRUNING--------------------------------------
//	if(M_ChkFlag(SL_LV_SCRON)!=0 || M_ChkFlag(SL_LV_SCRKEEPON)!=0) //20120602											//ÒªÇóCrowbar¿ªÍ¨
	if(M_ChkFlag(SL_LV_SCRON)!=0)											//ÒªÇóCrowbar¿ªÍ¨
	{		
		M_ACROWBARON();								 						//SCR¿ªÍ¨
		M_SetFlag(SL_LV_SCRRUNING);											//SCR¿ªÍ¨±êÖ¾Î»
		M_SetFlag(SL_DISPLAY6);													
	}
	else
	{
//---------·ÖµøÂäÉî¶ÈºÍÀàÐÍÇÐ³öCrowbar20121210---------20130225

		if(M_ChkFlag(SL_LV_SCRRUNING)!=0)
		{
//--------20130221--------
//			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scroff3,2)>0)			//Í¶Èëscrºó2msÅÐ¶Ï
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scroff3,_stdby04)>0)			//Í¶Èëscrºó2msÅÐ¶Ï	20130309
			{
//				if(M_ChkFlag(SL_LV_SCRDONE)!=0)
//				if((scroff1==1) || (M_ChkFlag(SL_LV_SCRDONE)!=0) || ((NGS_Udq_epsilon > 18) && (PRO.Pgactive_lv<600000) && (NGS_Udq_p>(0.66 * NGS_Udq_p_ex))))	//Ð¡¹¦ÂÊ35%-2ph¼°ÒÔÉÏÖ»Í¶2-5ms	20130303
/*				if((scroff1==1) || (M_ChkFlag(SL_LV_SCRDONE)!=0) || ((NGS_Udq_epsilon > 18) && (PRO.Pgactive_lv<600000) && (NGS_Udq_p>(0.55 * NGS_Udq_p_ex))))	//Ð¡¹¦ÂÊ20%-2ph¼°ÒÔÉÏÖ»Í¶2-5ms	20130308
				{
					scroff1 = 1;
					M_ACROWBAROFF();
					if(M_ChkFlag(SL_LV_SCRIZERO)!=0)							//¼ì²âµ½SCRµç÷½Ó½üÎªÁã
					{
						M_ClrFlag(SL_LV_SCRRUNING);								//½áÊøSCR¶¯×÷½ø³Ì
						MAIN_LOOP.cnt_lv_scroff3=0;								//ÖØÒª£º±£Ö¤ÏÂ´ÎSCR¿ªÍ¨ÒÀÈ»ÑÓÊ±_SC_SCRON2010115ÐÞ¸Ä	
						scroff1 = 0;
					}
				}
*///--------20130309Ð¡¹¦ÂÊ20%ÔÙ´ÎÍ¶Èë--------
				if((M_ChkFlag(SL_LV_SCRDONE)!=0) || (PRO.Pgactive_lv<600000))	//Ð¡¹¦ÂÊ20%-2ph¼°ÒÔÉÏÖ»Í¶2-5ms	20130308
				{
					M_ACROWBAROFF();
					if(M_ChkFlag(SL_LV_SCRIZERO)!=0)							//¼ì²âµ½SCRµç÷½Ó½üÎªÁã
					{
						if((NGS_Udq_p<=(0.327 * NGS_Udq_p_ex)) && (scroff1==0) && (M_ChkFlag(SL_LV_SCRDONE)==0))
						{
							MAIN_LOOP.cnt_lv_scroff3=0;
//							M_ACROWBARON();
							if((_STDBY9&0x0004)!=0)		M_ACROWBARON();		//116.2	Ö»ÓÐ20-2phÐ¡·ç²ÅÄÜÔÙ´ÎÍ¶Èë	20130314
							scroff1 = 1;
						}
						else
						{
							M_ClrFlag(SL_LV_SCRRUNING);								//½áÊøSCR¶¯×÷½ø³Ì
							MAIN_LOOP.cnt_lv_scroff3=0;								//ÖØÒª£º±£Ö¤ÏÂ´ÎSCR¿ªÍ¨ÒÀÈ»ÑÓÊ±_SC_SCRON2010115ÐÞ¸Ä	
							scroff1 = 0;
						}
					}
				}
			}
//-------------------------------------------
//------------------------
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scroff2,10)>0)			//Í¶Èëscrºó10msÅÐ¶Ï
			{
//				if(((NGS_Udq_p>280) && (NGS_Udq_epsilon < 15)) || (M_ChkFlag(SL_LV_SCROFF2)!=0))				//µø·ùÉî¶ÈµÍÓÚ50%	20130116
				if(((NGS_Udq_p>190) && (NGS_Udq_epsilon < 15)) || (M_ChkFlag(SL_LV_SCROFF2)!=0))				//35%¼°ÒÔÉÏÇé¿ö£¨³ý35%´ó·ç£©	20130221
				{
					if((NGS_Udq_p<250) && (PRO.Pgactive_lv>600000))	{}		//ÌÞ³ý35%´ó·ç	20130221
					else
					{
						M_ACROWBAROFF();											//SCR¹Ø¶Ï
						M_SetFlag(SL_LV_SCROFF2);					
			
						if(M_ChkFlag(SL_LV_SCRIZERO)!=0)							//¼ì²âµ½SCRµç÷½Ó½üÎªÁã
						{				
							M_ClrFlag(SL_LV_SCRRUNING);								//½áÊøSCR¶¯×÷½ø³Ì
							MAIN_LOOP.cnt_lv_scroff2=0;								//ÖØÒª£º±£Ö¤ÏÂ´ÎSCR¿ªÍ¨ÒÀÈ»ÑÓÊ±_SC_SCRON2010115ÐÞ¸Ä	
							M_ClrFlag(SL_LV_SCROFF2);
						}
					}
				}
			}
/*			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scroff1,lv_ctrl3)>0)			//Í¶Èëscrºó22msÅÐ¶Ï
			{
//				if((NGS_Udq_p<190 && PRO.Pgactive_lv<600000) && (NGS_Udq_epsilon < 15) || (M_ChkFlag(SL_LV_SCROFF1)!=0))				//ÖÁ20%¼°Ð¡·ç 20130221
				if((PRO.Pgactive_lv<600000) || (M_ChkFlag(SL_LV_SCROFF1)!=0))				//ËùÓÐÐ¡·ç¹¤¿ö²»³¬¹ý¸ÃÊ±¼ä 20130303
				{
					M_ACROWBAROFF();											//SCR¹Ø¶Ï
					M_SetFlag(SL_LV_SCROFF1);

					if(M_ChkFlag(SL_LV_SCRIZERO)!=0)							//¼ì²âµ½SCRµç÷½Ó½üÎªÁã
					{				
						M_ClrFlag(SL_LV_SCRRUNING);								//½áÊøSCR¶¯×÷½ø³Ì
						MAIN_LOOP.cnt_lv_scroff1=0;								//ÖØÒª£º±£Ö¤ÏÂ´ÎSCR¿ªÍ¨ÒÀÈ»ÑÓÊ±_SC_SCRON2010115ÐÞ¸Ä				
						M_ClrFlag(SL_LV_SCROFF1);
					}
				}
			}
*///20130309È¡Ïû,»Ö¸´Ê±×¢Òâ»Ö¸´¶¨Ê±Æ÷
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scron,_SC_TSCRON)>0)			//SCR¿ªÍ¨Ò»¸ö_SC_SCRON¹Ì¶¨±¼ä£¬²Ù×÷Æ÷¿Éµ÷
			{								
				if(((NGS_Udq_epsilon > 40) && (PRO.Pgactive_lv>600000) && (NGS_Udq_p<(NGS_Udq_p_ex * 0.56))) || (scrhold==1))		//20130221	20130227
				{
					 scrhold = 1;		//20130223
					 if(M_ChkFlag(SL_LV_SCRKEEPON)==0)
					 {
					 	M_ACROWBAROFF();		//20130223
						scrhold = 0;
					 }
				}
				else if(NGS_Udq_epsilon > 26)		//º­¸Ç20%Ð¡¹¦ÂÊ¼°35%´óÐ¡¹¦ÂÊ20130222
				{
//					if(M_ChkCounter(MAIN_LOOP.cnt_lv_unblcscroff2,50)>0)	M_ACROWBAROFF();		//SCR¹Ø¶Ï	50ms-20130221
					if(M_ChkCounter(MAIN_LOOP.cnt_lv_unblcscroff2,80)>0)	M_ACROWBAROFF();		//SCR¹Ø¶Ï	50ms-20130221
				}
				else	M_ACROWBAROFF();				//SCR¹Ø¶Ï

				if(M_ChkFlag(SL_LV_SCRIZERO)!=0)							//¼ì²âµ½SCRµç÷½Ó½üÎªÁã
				{
					M_ClrFlag(SL_LV_SCRRUNING);								//½áÊøSCR¶¯×÷½ø³Ì
					MAIN_LOOP.cnt_lv_scron=0;								//ÖØÒª£º±£Ö¤ÏÂ´ÎSCR¿ªÍ¨ÒÀÈ»ÑÓÊ±_SC_SCRON2010115ÐÞ¸Ä				
					MAIN_LOOP.cnt_lv_unblcscroff1 = 0;
					MAIN_LOOP.cnt_lv_unblcscroff2 = 0;
					MAIN_LOOP.cnt_lv_unblcscroff3 = 0;			//20130124
					scrhold = 0;								//20130223
				}
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
	}
//---------------------
//----------µÚÒ»´ÎÍ¶Èëscr½áÊøÅÐ¶Ï20130222-----------20130225
	if((M_ChkFlag(SL_LV_STATE)!=0 && M_ChkFlag(SL_LV_SCRON)!=0) || (scrdone == 1))
	{
		scrdone = 1;
		if(M_ChkFlag(SL_LV_SCRRUNING)==0)	M_SetFlag(SL_LV_SCRDONE);
	}
	if(M_ChkFlag(SL_LV_STATE)==0)
	{
		scrdone = 0;
		M_ClrFlag(SL_LV_SCRDONE);
	}
//----------------------------------------------

//----------LV¶¯Ì¬¶¨×Ó´ÅÁ´¿ØÖÆ µøÂäºÍ»Ö¸´ºó100-200msÄÚÊµÊ© BJTULVRT201204
	if(M_ChkFlag(SL_LV_STATE)!=0)	
	{
		M_SetFlag(SL_LV_PHICON);
		MAIN_LOOP.cnt_lv_phicontrol=0;
	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_lv_phicontrol,DELAY_PHICON)>=0)	//»Ö¸´ºó´ÅÁ´µÖÏû¿ØÖÆÊ±¼ä50ms 201205LVRTatZB
//		if(M_ChkCounter(MAIN_LOOP.cnt_lv_phicontrol,_stdby03)>=0)	//»Ö¸´ºó´ÅÁ´µÖû¿ØÖÆÊ±¼ä
			 M_ClrFlag(SL_LV_PHICON);
	}		
			

//----------LV¶¯Ì¬PI¿ØÖÆ »ú²àÖØÆôºó20msÄÚÊµÊ© BJTULVRT201204
	if(M_ChkFlag(SL_LV_MSTOP)!=0)
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
** º¯ÊýÃûÆ: Protect
** ¹¦ÄÜÃèÊö: ±£»¤Öµ¼ÆËã
** Êä¡¡Èë: 	 
** Êä  ³ö:   
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:20121103
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Protect(void)
{
/*	float temp;

//----------------------------²¢ÍøµçÁ÷Ë²Ê±Öµ±£»¤---------------------------
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

	PRO.NPR_iac = temp * 100 / NPR_IACN;			//²¢ÍøµçÁ÷Ë²Ê±×î´óÖµ£¬»ù×¼µçÁ÷µÄ°Ù·ÖÖµ

//----------------------------×ª×ÓµçÁ÷Ë²Ê±Öµ±£»¤---------------------------
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

	PRO.MPR_iac = temp * 100 / MPR_IACN;		//×ª×Óµç÷´óÖµ£¬»ù×¼µç÷µÄ°Ù·ÖÖµ

//------------------------ÖÐ¼äÖ±Á÷µçÑ¹±£»¤Öµ¼ÆËã-----------------------------

	PRO.udc  = AD_OUT_UDC;	                                 //µ¥Î»V
*/
//--------------------------- ÖÐ¼äÕ¶²¨µçÁ÷±£»¤Öµ-201105atbjtuLVRT---------------------------
	PRO.CHOP_idc = AD_OUT_DC_I.b * 100 / CHOP_IDCN;

//---------------------------Íø²àÏßµçÑ¹ÓÐÐ§Öµ¼ÆËã----------------------------
    PRO.NPR_uab= MEAN_DATA.uab1 * 1.110721;                      //Íø²àUabÓÐÐ§Öµµ¥Î»V rms=mean*PAI/(2*sqrt(2)) 
	PRO.NPR_ubc= MEAN_DATA.ubc1 * 1.110721;                      //Íø²àUbcÓÐÐ§Öµµ¥Î»V 1.110721=PAI * SQRT2 / 4
//	PRO.GID_uab = MEAN_DATA.uab * 1.110721;        //20091026atzy µçÍø²àUabÓÐÐ§Öµµ¥Î»V µ¥Î»V  rms=mean*PAI/(2*sqrt(2)) £» 1.110721=PAI * SQRT2 / 4
//	PRO.GID_ubc = MEAN_DATA.ubc * 1.110721;        //20091026atzy µçÍø²àUabÓÐÐ§Öµµ¥Î»V µ¥Î»V  rms=mean*PAI/(2*sqrt(2)) £» 1.110721=PAI * SQRT2 / 4
//	PRO.GID_uab = MEAN_DATA.uab1 * 1.110721;        //20091026atzy µçÍø²àUabÓÐÐ§Öµµ¥Î»V µ¥Î»V  rms=mean*PAI/(2*sqrt(2)) £» 1.110721=PAI * SQRT2 / 4
//	PRO.GID_ubc = MEAN_DATA.ubc1 * 1.110721;        //20091026atzy µçÍø²àUabÓÐÐ§Öµµ¥Î»V µ¥Î»V  rms=mean*PAI/(2*sqrt(2)) £» 1.110721=PAI * SQRT2 / 4

//---------------------------¶¨×Ó²àÏßµçÑ¹ÓÐÐ§Öµ¼ÆËã----------------------------
    PRO.STA_uab= MEAN_DATA.uab2 * 1.110721;                    //µç²àUabÓÐÐ§Öµµ¥V
	PRO.STA_ubc= MEAN_DATA.ubc2 * 1.110721;                    //µç»ú²àUbcÓÐÐ§Öµµ¥Î»V

//------------------¹¦ÂÊ±£»¤¼°ÏÔÊ¾Öµ¼ÆËã-----------------------------------------
//------------------¶¨×Ó²àdq±ä»»¶¨Ïà½Ç¶ÈÖµ--------------------------------------------------------------------
/*	temp = CAP4.nprtrstheta + 0.5236 - 1.57; //systest 201007BJTULVRT CAP4.nprtrstheta+30-90
//	temp = CAP4.nprtrstheta + (1.047 * 0.5); //systest 2010atcpc
	DIP_STA_I.sintheta = sin(temp);  //systest 2010atcpc
	DIP_STA_I.costheta = cos(temp);  //systest 2010atcpc
//------------------¶¨×Ó²àµçÁ÷dqÖµ--------------------------------------------------------------------
   	DIP_STA_I.a     = AD_OUT_STA_I.a;
   	DIP_STA_I.b     = AD_OUT_STA_I.b;
   	DIP_STA_I.c     = AD_OUT_STA_I.c;
	Transform_3s_2s_2r(&DIP_STA_I);
	DataFilter(0.99,&DIP_STA_I.dflt,DIP_STA_I.d); 	//¶¨×Ó²àµçÁ÷·´À¡ÖµÂË²¨,Ts=200us,ÂËµô¿ª¹ØÆµÂÊ´Î
	DataFilter(0.99,&DIP_STA_I.qflt,DIP_STA_I.q); 	//¶¨×Ó²àµçÁ÷·´À¡ÖµÂË²¨,Ts=200us,ÂËµô¿ª¹ØÆµÂÊ´Î

//------------------¶¨×Ó²àµçÑ¹dqÖµ--------------------------------------------------------------------
	DIP_STA_U.dflt = 	DIP_NPR_U.dflt;			   //systest 2010atcpc
	DIP_STA_U.qflt = 	DIP_NPR_U.qflt;

//-----------------¼ÆËã¶¨×Ó´ÅÁ´--ÐèÒª×ª×Ó¶¨×Ó¹éËã£¡201007BJTULVRT---------------------------------------------------------------------
	PHAI_d = DIP_STA_I.dflt * MPR_Ls + TRS_MPR_I.dflt * MPR_Lm / STAROTRTO;
	PHAI_q = DIP_STA_I.qflt * MPR_Ls + TRS_MPR_I.qflt * MPR_Lm / STAROTRTO;
	PHAI_dq = sqrt(PHAI_d * PHAI_d + PHAI_q * PHAI_q);

//------------------------¶¨×ÓÏàµçÑ¹ÓÐÐ§ÖµÏÔÊ¾Öµ¼ÆËã(Îª¹¦ÂÊ¼ÆËã)--------------------------------------------------
    PRO.sta_uar = MEAN_DATA.ua1 * 1.110721;         //µ¥Î»V ²ÉÓÃÍøÑ¹×÷Îª¶¨×ÓµçÑ¹£¬ÒòÎª¶¨×ÓV-LEM·´¡µçÑ¹¾­¹ýÁË´ó³£ÊýÂË²¨
	PRO.sta_ubr = MEAN_DATA.ub1 * 1.110721;
	PRO.sta_ucr = MEAN_DATA.uc1 * 1.110721;

//------------------------¶¨×ÓÏàµçÁ÷ÓÐÐ§ÖµÏÔÊ¾Öµ¼ÆËã(Îª¹¦ÂÊ¼ÆËã)--------------------------------------------------
    PRO.sta_iar = MEAN_DATA.ia3 * 1.110721;			//µ¥Î»A
	PRO.sta_ibr = MEAN_DATA.ib3 * 1.110721;
	PRO.sta_icr = MEAN_DATA.ic3 * 1.110721;
*///ÆÁ±ÎBJTULVRT201204

//------------------¶¨×Ó²àÓÐ¹¦ºÍÎÞ¹¦¼ÆËãÖµ--------------------------------------------------------------------
//    PRO.Psactive   = 1.5 * SQRT3 * (DIP_STA_U.dflt * DIP_STA_I.dflt + DIP_STA_U.qflt * DIP_STA_I.qflt); 
//    PRO.Psreactive = 1.5 * SQRT3 * (DIP_STA_U.qflt * DIP_STA_I.dflt - DIP_STA_U.dflt * DIP_STA_I.qflt); 
//	PRO.Ps		   = SQRT3 * (PRO.sta_uar * PRO.sta_iar + PRO.sta_ubr * PRO.sta_ibr + PRO.sta_ucr * PRO.sta_icr);//20091007

//------------------------Íø²à²¢ÍøµçÁ÷ÓÐÐ§ÖµÏÔÊ¾Öµ¼ÆËã----------------------------------------------
    PRO.npr_iar = MEAN_DATA.ia1 * 1.110721;     			 //1.110721=PAI * SQRT2 / 4  	//µ¥Î»A
	PRO.npr_ibr = MEAN_DATA.ib1 * 1.110721;
	PRO.npr_icr = MEAN_DATA.ic1 * 1.110721;

//------------------------Íø²àÏàµçÑ¹ÓÐÐ§ÖµÏÔÊ¾Öµ¼ÆËã----------------------------------------------
    PRO.npr_uar = MEAN_DATA.ua1 * 1.110721;      			//1.110721=PAI * SQRT2 / 4  //µ¥Î»A
	PRO.npr_ubr = MEAN_DATA.ub1 * 1.110721;
	PRO.npr_ucr = MEAN_DATA.uc1 * 1.110721;

//------------------ÍøàÓÐ¹¦ºÍÎÞ¹¦¼ÆËãÖµ--------------------------------------------------------------------
// 	PRO.Pnactive   = Pnactive;
//	PRO.Pnreactive = Pnreactive;
//	PRO.Pn         = PRO.npr_iar * PRO.npr_uar + PRO.npr_ibr * PRO.npr_ubr + PRO.npr_icr * PRO.npr_ucr;

//------------------------------¶¨×Ó²àºÍÍø²à×Ü²¢Íø¹¦ÂÊÏÔÊ¾-----------------------------------------------------
//	PRO.Pgactive   = PRO.Psactive   + PRO.Pnactive;
//	PRO.Pgreactive = PRO.Psreactive + PRO.Pnreactive;

//	if(CAP4.omigaslp >= 0)	 PRO.Pg = PRO.Ps - PRO.Pn;
//	else 					 PRO.Pg = PRO.Ps + PRO.Pn;
//---------------------------Íø²àºÍ»úç¸ÐÎÂ¶ÈÖµ----------------------------
    PRO.NPR_TLOV= MEAN_DATA.Lac_temp;                    //Íø²àµç¸ÐÎÂ¶È
	PRO.MPR_TLOV= MEAN_DATA.Ldudt_temp;                  //»ú²àµç¸ÐÎÂ¶È

//--------------------------- ¶¨×Ó²àÏßµçÁ÷±£»¤Öµ----------------------------
	PRO.STA_iac = MEAN_DATA.iac3 * 1.110721  * 100/ STA_IACN;
	PRO.STA_icb = MEAN_DATA.icb3 * 1.110721  * 100/ STA_IACN;

//¹¦ÂÊ¼ÆËã201205LVRTatZB
//------------------Íø²àÓÐ¹¦ºÍÎÞ¹¦¼ÆËãÖµ--------------------------------------------------------------------
	if((M_ChkFlag(SL_IN1_CBSTS)!=0) && (M_ChkFlag(SL_IN1_MIANFILTER)!=0))	
		Pcreactive = DIP_NPR_U.dflt * DIP_NPR_U.dflt * 0.236;	//Pc=1.5*ed*ed*ws*Cn 201205LVRTatZB
	else	Pcreactive = 0;

//--------Ë²Ê±¹¦ÂÊ201205LVRTatZB
 	PRO.Pnactive   = AD_OUT_NGS_U.a * AD_OUT_NPR_I.a + AD_OUT_NGS_U.b * AD_OUT_NPR_I.b + AD_OUT_NGS_U.c * AD_OUT_NPR_I.c;
	PRO.Pnreactive = SQRT3_3 *  (AD_OUT_NGS_U.bc * AD_OUT_NPR_I.a + (AD_OUT_NGS_U.c-AD_OUT_NGS_U.a) * AD_OUT_NPR_I.b + AD_OUT_NGS_U.ab * AD_OUT_NPR_I.c);
	PRO.Pnreactive = PRO.Pnreactive + Pcreactive;

    PRO.Psactive   = AD_OUT_NGS_U.a * AD_OUT_STA_I.ac + AD_OUT_NGS_U.b * AD_OUT_STA_I.ba + AD_OUT_NGS_U.c * AD_OUT_STA_I.cb; 
    PRO.Psreactive = SQRT3_3 *  (AD_OUT_NGS_U.bc * AD_OUT_STA_I.ac + (AD_OUT_NGS_U.c-AD_OUT_NGS_U.a) * AD_OUT_STA_I.ba + AD_OUT_NGS_U.ab * AD_OUT_STA_I.cb); 

//---------------------------×ªËÙ±£»¤Öµ¼ÆËã----------------------------------
	PRO.speed   = 9.5492966 * QEPDATA.omigamec;		//µ¥Î»£º×ª/·Ö£»n=60*w/2pai=*w
//	PRO.speed   = 1200;		//controltest

	DataFilter(0.45,&PRO.speedflt,PRO.speed); 				//Ö÷¿Ø×ªËÙ·´À¡ÖµÂË²¨£¬ Ts=200us,Tr=5ms 20091021atzyÖ®Ç°123msÂË²¨Ì«´óÁË
	
	if(PRO.speedflt>700)		M_SetFlag(SL_SPEED_HIGH);	//½øÈë¸ß×ªËÙÇø   20091021atzy 650¸ÄÎª700£¬¼Ó´óÖÍ»·
    else if(PRO.speedflt<600)	M_ClrFlag(SL_SPEED_HIGH);   //650r/min²âÖÜ·¨PRD=845.²âÆµ·¨POSLAT=887.

    if((PRO.speedflt > _SC_MSPEED1) && (PRO.speedflt < _SC_MSPEED2)) 	 
    		M_SetFlag(SL_SPEED_IN_RANGE);  
    else if((PRO.speedflt < (_SC_MSPEED1-SC_MSPD_HW)) || (PRO.speedflt > (_SC_MSPEED2 + SC_MSPD_HW))) 	 
			M_ClrFlag(SL_SPEED_IN_RANGE);					//ÅÐ¶Ï¼Ó»Ø²î201005atcpc

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

//----------------------------¾ÀÕý¼ÆËã²¢ÍøÓÃ¹¦¹¦ÂÊ,ÏÔÊ¾---------------------------------------
//------------------------------¶¨×Ó²àºÍÍø²à×Ü²¢Íø¹¦ÂÊÏÔÊ¾-----------------------------------------------------
//	PRO.Pgactive = Te_feedback * PRO.speedflt * PAI * 0.03333333;	//P=T*2PIE*n/60 201005atcpc
	PRO.Pgactive2   = PRO.Psactive   + PRO.Pnactive;					
	PRO.Pgreactive2 = PRO.Psreactive + PRO.Pnreactive;
	DataFilter(0.99,&PRO.Pgactive,PRO.Pgactive2); 		//×¢£º¸ÄÂË²¨Ç400us/2.5kHzÖ´ÐÐÒ»´Î¡£¶¨×Ó²àµçÁ÷·´À¡ÖµÂË²¨,Ts=200us,ÂËµô¿ª¹ØÆµÂÊ´Î0.97
	DataFilter(0.99,&PRO.Pgreactive,PRO.Pgreactive2); 	//c=0.99->8Hz; c=0.9->88Hz

	if(M_ChkFlag(SL_LV_QWORKING)==0) 	PRO.Pgactive_lv = PRO.Pgactive;		//ÅÐ¶ÏLVRTÖ®Ç°¹¦ÂÊ20121210

	if((_STDBY9&0x0008)!=0)	//116.3 201205LVRTatZB
    	PRO.Pgactive   = Te_feedback * PRO.speedflt * PAI * 0.03333333;	//P=T*2PIE*n/60 201005atcpc

//----------------------------Íø²à½ÓµØ¹ÊÕÏ-ÈýÏàµçÁ÷²»Æ½ºâ±£»¤ÅÐ¶Ï---------------------------20100302
	if((AD_OUT_NPR_I.a+AD_OUT_NPR_I.b+AD_OUT_NPR_I.c)>120)
			M_SetFlag(SL_IUBLNCE1);
	else	M_ClrFlag(SL_IUBLNCE1);

//----------------------------»ú²à½ÓµØ¹ÊÕÏ-ÈýÏàµçÁ÷²»Æ½ºâ±£»¤ÅÐ¶Ï---------------------------20100302
	if((AD_OUT_MPR_I.a+AD_OUT_MPR_I.b+AD_OUT_MPR_I.c)>200)
			M_SetFlag(SL_IUBLNCE2);
	else	M_ClrFlag(SL_IUBLNCE2);

}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: Scout
** ¹¦ÄÜÃèÊö: ÏµÍ³¹ÊÕÏ¼ì²â¼°´¦Àí³ÌÐò
** ÊäÈë£º
** Êä³ö:        
**  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷Õß: 
** ÈÕÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÕß£º
** ÈÕÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Scout(void)
{

//  float temp_pgactive,temp_pgreactive;
  Uint16 tempb,tempc;
//------------------¸ù¾Ý×Ó³ÌÐòÉÏ±¨µÄÅÏ¢£¬ÅÐ¶ÏÊÇTAB_MSGÖÐÄÄÒ»¸ö-------------------------------------
	if(M_ChkFlag(SL_POWERON)==0)										//ÈôµçÍê³É¹ÊÕÏÔò¼ì²â¹ÊÕÏ
	{	
//---------------------------------TZ1ÖÐ¶Ï±£»¤------------------------------------------------------
		if(M_ChkFlag(SL_PDPINTA)!=0)	
		{	
			tempb = *IN2_ADDR;
			tempc = *IN3_ADDR;
			_IN12_DATA = (_IN12_DATA & 0x00FF) | ((tempb<<8) & 0xFF00);
			_IN34_DATA = (tempc & 0x00FF);

			if(M_ChkFlag(SL_IN2_IOVA1)!=0)			M_SetFlag(SL_HIA1);		//Íø²àAÏàSKiiP¹ÊÕÏ
			else if(M_ChkFlag(SL_IN2_IOVB1)!=0)	   	M_SetFlag(SL_HIB1);		//Íø²àBÏàSKiiP¹ÊÕÏ
		    else if(M_ChkFlag(SL_IN2_IOVC1)!=0)	   	M_SetFlag(SL_HIC1);		//Íø²àCÏàSKiiP¹ÊÕÏ
			else if(M_ChkFlag(SL_IN3_VDCOV)!=0)   	M_SetFlag(SL_HUDCOV);	//Ö±Á÷²à¹ýÑ¹¹ÊÕÏ
			else if(M_ChkFlag(SL_IN3_NPRIOV)!=0)  	M_SetFlag(SL_HIACOV1);	//Íø²àÓ²¼þ¹ýÁ÷¹ÊÕÏ
			else if(M_ChkFlag(SL_PDPASERIES)!=0)	M_ClrFlag(SL_PDPINTA);	//ÔÙÔÊÐíÆô¶¯±äÁ÷Æ÷	20091107atzy						
			else									M_SetFlag(SL_PDPASERIES);//·¢ÉúÓ²¼þ¹ÊÕÏ,CPLDÃ»ÓÐ´æµ½¹ÊÕÏ
		}
		else
		{
			M_ClrFlag(SL_HIA1);   	    									//Çå¹ÊÕÏ±êÖ¾
			M_ClrFlag(SL_HIB1);
			M_ClrFlag(SL_HIC1);
			M_ClrFlag(SL_HUDCOV);
			M_ClrFlag(SL_HIACOV1);  			
			M_ClrFlag(SL_PDPASERIES);  			
		}

//---------------------------------TZ2ÖÐ¶Ï±£»¤-------------------------------------------------------
		if(M_ChkFlag(SL_PDPINTB)!=0)	
		{	
			tempb = *IN2_ADDR;
			tempc = *IN3_ADDR;
			_IN12_DATA = (_IN12_DATA & 0x00FF) | ((tempb<<8) & 0xFF00);
			_IN34_DATA = (tempc & 0x00FF);

			if(M_ChkFlag(SL_IN2_IOVA2)!=0)			M_SetFlag(SL_HIA2);			//µç»ú²àAÏàSKiiP¹ÊÕÏ
			else if(M_ChkFlag(SL_IN2_IOVB2)!=0)	    M_SetFlag(SL_HIB2);			//µç»ú²àBÏàSKiiP¹ÊÕÏ
		    else if(M_ChkFlag(SL_IN2_IOVC2)!=0)	    M_SetFlag(SL_HIC2);			//µç»ú²àCÏàSKiiP¹ÊÕÏ
		    else if(M_ChkFlag(SL_IN3_MPRIOV)!=0)    M_SetFlag(SL_HIACOV2);		//µç»ú²àÓ²¼þ¹ýÁ÷¹ÊÕÏ
			else if(M_ChkFlag(SL_IN3_IOVDC)!=0)  	M_SetFlag(SL_HIDCOV);		//ÖÐ¼äÖ±Á÷Õ¶²¨IGBTÓ²¼þ¹ÊÕÏ20120228atbjtu_chopper	BJTULVRT201204
			else if(M_ChkFlag(SL_PDPBSERIES)!=0)	M_ClrFlag(SL_PDPINTB);		//ÔÊÐíÆô¶¯±äÁ÷Æ÷	20091107atzy						
			else									M_SetFlag(SL_PDPBSERIES);	//·¢ÉúÓ²¼þ¹ÊÕÏ,CPLDÃ»ÓÐ´æµ½¹ÊÕÏ
		}
		else
		{
			M_ClrFlag(SL_HIA2);    											//Çå¹ÊÕÏ±êÖ¾
			M_ClrFlag(SL_HIB2);	   		
			M_ClrFlag(SL_HIC2);	    
			M_ClrFlag(SL_HIACOV2);
			M_ClrFlag(SL_HIDCOV);  						//20120228	BJTULVRT201204
			M_ClrFlag(SL_PDPBSERIES);	
		}

//---------------------------------E-STOP±£»¤-------------------------------------------------------
		if(M_ChkFlag(SL_IN1_EXESTOP)!=0)								//Íâ²¿¼±Í£¹ÊÕÏ  ²Ù×÷°åÐÅºÅ£¬·ç³¡Ã»ÓÃ								
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_estop,DELAY_ESTOP)>=0)   M_SetFlag(SL_ESTOP);		//½ô¼±Í£Ö¹ÑÓ³ÙÊ±¼äµ½£¿
			else M_ClrFlag(SL_ESTOP);									//ÇåêÖ¾Î»
		}
		else 
		{
			MAIN_LOOP.cnt_estop=0;										//Çå¶¨Ê±Æ÷
			M_ClrFlag(SL_ESTOP);
		}  

//---------------------------------±äÁ÷Æ÷Íâ²¿Ó²¼þ¹ÊÕÏ±£»¤-------------------------------------------------
		if((M_ChkFlag(SL_IN1_EXFAULTOK)==0)||(M_ChkFlag(SL_IN1_MIANFAN)==0))	//Íâ²¿¹ÊÕÏ¶¯×÷»òÕß¹¦ÂÊç»ú¹ÊÕÏ new							
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_exfault,DELAY_EXFAULT)>=0)   M_SetFlag(SL_EXFAIL);	//Íâ²¿Ó²¼þ¹ÊÕÏÑÓ³ÙÊ±¼äµ½£¿
			else M_ClrFlag(SL_EXFAIL);							     //Çå±êÖ¾Î»
		}
		else 
		{
			MAIN_LOOP.cnt_exfault=0;                               	 //Çå¶¨Ê±Æ÷
			M_ClrFlag(SL_EXFAIL);
		}	

//-----------------------CANOPENOVER CANÍ¨Ñ¶¹ÊÕÏ-----------------------------------------------------------
	    if(M_ChkFlag(SL_ENPCOPER)==0) 											//ÉÏµçÇÒÑ¡ÔñCAN¿ØÖÆ²Å½øÐÐÍ¨Ñ¶¹ÊÕÏ
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

//-----------------------CBTRIP±£»¤(Ö÷¶Ï¹ýÁ÷ÍÑ¿Û)---------------------------------------

	    if(M_ChkFlag(SL_IN1_CBRESET)==0)							   	//1=Ö÷¿Ø·¢À´ÒªÇóÖ÷¶Ï¸´Î»ÐÅºÅ
		{		
			if(M_ChkFlag(SL_IN3_CBTRIP)!=0)								//Ö÷¶Ï¹ýÁ÷ÍÑ¿Û¹ÊÕÏ	090816	
	    	{
	    		if(M_ChkCounter(MAIN_LOOP.cnt_cbtp,DELAY_CBTP)>=0)	   	//Ö÷¶ÏÍÑ¿Û×îÐ¡Ê±¼äµ½?
					M_SetFlag(SL_CBTRIP);	
				else 
					M_ClrFlag(SL_CBTRIP);								//Çå±êÖ¾Î»	    
	    	}
			else   	MAIN_LOOP.cnt_cbtp=0;		
		}
		else if(M_ChkFlag(SL_IN3_CBTRIP)==0)
		{
			M_ClrFlag(SL_CBTRIP);								 		//Çå±êÖ¾Î»
			MAIN_LOOP.cnt_cbtp=0;
		}
		else	MAIN_LOOP.cnt_cbtp=0;
	    	    

//-----------------------¸÷¿ª¹Ø¶¯×÷¹ÊÕÏ110818---------------------------------------
//	if(M_ChkFlag(SL_SWITCHDETECT)==0)
//	{
//----¸ù¾Ý½ºÄÏ·ç³¡µÄbugÔÝÊ±È¡Ïû¸Ã¼ì²â----20121024----
//-----------------------Ö÷¶ÏºÏÕ¢/·ÖÕ¢/ÒâÍâ¶Ï¿ª¹ÊÕÏ---------------------------------------
	    if((M_ChkFlag(CL_CBON)!=0 && M_ChkFlag(SL_IN1_CBSTS)==0)||(M_ChkFlag(CL_CBLVC)==0 && M_ChkFlag(SL_IN1_CBSTS)!=0)||(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(CL_CBLVC)!=0 && M_ChkFlag(SL_CBCLOSED)!=0 && M_ChkFlag(SL_IN3_CBTRIP)==0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_cberror,DELAY_CBERROR)>=0)	M_SetFlag(SL_ERROR_CB);	  //2s  
			else M_ClrFlag(SL_ERROR_CB);								 //Çå±êÖ¾Î»
		}
		else 
		{
			MAIN_LOOP.cnt_cberror=0;                                    //ÇåÊ±Æ÷
			M_ClrFlag(SL_ERROR_CB);
		} 
		
//-----------------------Ö÷½Ó´¥Æ÷±ÕºÏ/¶Ï¿ª¹ÊÕÏ---------------------------------------
	    if((M_ChkFlag(CL_MAINK)!=0 && M_ChkFlag(SL_IN1_MIANK)==0)||(M_ChkFlag(CL_MAINK)==0 && M_ChkFlag(SL_IN1_MIANK)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_mainkerror,DELAY_MAINKERROR)>=0)	M_SetFlag(SL_ERROR_MAINK);	
			else M_ClrFlag(SL_ERROR_MAINK);								 //Çå±êÖ¾Î»
		}
		else 
		{
			MAIN_LOOP.cnt_mainkerror=0;                                    //Çå¶¨Ê±Æ÷
			M_ClrFlag(SL_ERROR_MAINK);
		}  
		
//-----------------------Ö÷ÂË²¨Æ÷±ÕºÏ/¶Ï¿ª¹ÊÕÏ---------------------------------------
	    if((M_ChkFlag(CL_MAINK)!=0 && M_ChkFlag(SL_IN1_MIANFILTER)==0)||(M_ChkFlag(CL_MAINK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_mainferror,DELAY_MAINFERROR)>=0)	M_SetFlag(SL_ERROR_MAINF);	
			else M_ClrFlag(SL_ERROR_MAINF);								 //Çå±êÖ¾Î»
		}
		else 
		{
			MAIN_LOOP.cnt_mainferror=0;                                    //Çå¶¨Ê±Æ÷
			M_ClrFlag(SL_ERROR_MAINF);
		}   

//-----------------------¶¨×Ó½Ó´¥Æ÷±ÕºÏ/¶Ï¿ª¹ÊÕÏ---------------------------------------
	    if((M_ChkFlag(CL_STATORK)!=0 && M_ChkFlag(SL_IN1_STATORK)==0)||(M_ChkFlag(CL_STATORK)==0 && M_ChkFlag(SL_IN1_STATORK)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_stacerror,DELAY_STACERROR)>=0)	M_SetFlag(SL_ERROR_STAC);	
			else M_ClrFlag(SL_ERROR_STAC);								 //Çå±êÖ¾Î»
		}
		else 
		{
			MAIN_LOOP.cnt_stacerror=0;                                    //Çå¶¨Ê±Æ÷
			M_ClrFlag(SL_ERROR_STAC);
		}   
//	}

//-----------------------¸÷¿ª¹ØÒâÍâ¶¯×÷¹ÊÕÏ110818---------------------------------------
/*	else if(M_ChkFlag(SL_SWITCHDETECT)!=0)
	{
//-----------------------Ö÷¶ÏºÏÕ¢/·ÖÕ¢¹ÊÕÏ---------------------------------------
	    if((M_ChkFlag(CL_CBON)!=0 && M_ChkFlag(SL_IN1_CBSTS)==0) || (M_ChkFlag(CL_CBFCON)==0 && M_ChkFlag(SL_IN1_CBSTS)!=0))	     //110723£¬CL_CBFCONÌæ»»CL_CBLVC							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_cberror,DELAY_SWITCHDETECT)>=0)	M_SetFlag(SL_ERROR_CB);	   
			else M_ClrFlag(SL_ERROR_CB);								 //Çå±êÖ¾Î»
		}
		else 
		{
			MAIN_LOOP.cnt_cberror=0;                                    //Çå¶¨Ê±Æ÷
			M_ClrFlag(SL_ERROR_CB);
		} 
		
//-----------------------Ö÷½Ó´¥Æ÷±ÕºÏ/¶Ï¿ª¹ÊÕÏ---------------------------------------
	    if((M_ChkFlag(CL_MAINK)!=0 && M_ChkFlag(SL_IN1_MIANK)==0)||(M_ChkFlag(CL_MAINK)==0 && M_ChkFlag(SL_IN1_MIANK)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_mainkerror,DELAY_SWITCHDETECT)>=0)	M_SetFlag(SL_ERROR_MAINK);	
			else M_ClrFlag(SL_ERROR_MAINK);								 //Çå±êÖ¾Î»
		}
		else 
		{
			MAIN_LOOP.cnt_mainkerror=0;                                    //Çå¶¨Ê±Æ÷
			M_ClrFlag(SL_ERROR_MAINK);
		}  
		
//-----------------------Ö÷ÂË²¨Æ÷±ÕºÏ/¶Ï¿ª¹ÊÕÏ---------------------------------------
	    if((M_ChkFlag(CL_MAINK)!=0 && M_ChkFlag(SL_IN1_MIANFILTER)==0)||(M_ChkFlag(CL_MAINK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_mainferror,DELAY_SWITCHDETECT)>=0)	M_SetFlag(SL_ERROR_MAINF);	
			else M_ClrFlag(SL_ERROR_MAINF);								 //Çå±êÖ¾Î»
		}
		else 
		{
			MAIN_LOOP.cnt_mainferror=0;                                    //Çå¶¨Ê±Æ÷
			M_ClrFlag(SL_ERROR_MAINF);
		}   

//-----------------------¶¨×Ó½Ó´¥Æ÷±ÕºÏ/¶Ï¿ª¹ÊÕÏ---------------------------------------
	    if((M_ChkFlag(CL_STATORK)!=0 && M_ChkFlag(SL_IN1_STATORK)==0)||(M_ChkFlag(CL_STATORK)==0 && M_ChkFlag(SL_IN1_STATORK)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_stacerror,DELAY_SWITCHDETECT)>=0)	M_SetFlag(SL_ERROR_STAC);	
			else M_ClrFlag(SL_ERROR_STAC);								 //Çå±êÖ¾Î»
		}
		else 
		{
			MAIN_LOOP.cnt_stacerror=0;                                    //Çå¶¨Ê±Æ÷
			M_ClrFlag(SL_ERROR_STAC);
		}
	}
*/
//----------------------Ô¤³äµç¹ÊÕÏ---------------------------------------
	    if((M_ChkFlag(CL_PRE)!=0)&&(M_ChkFlag(CL_MAINK)==0))	     	//							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_preerror,DELAY_PREERROR)>=0)	M_SetFlag(SL_ERROR_PRE);  //15s	
			else M_ClrFlag(SL_ERROR_PRE);								 //Çå±êÖ¾Î»
		}
		else 
		{
			MAIN_LOOP.cnt_preerror=0;                                    //Çå¶¨Ê±Æ÷
			M_ClrFlag(SL_ERROR_PRE);
		}  
 
//-----------------------ENCODFAULT±£»¤(Qep¼ì²â,±àÂëÆ÷¹ÊÕÏ)-----------------------------------------
//	    if(((M_ChkFlag(SL_QEPPCO)!=0)||(M_ChkFlag(SL_QCAPDISTURB)!=0)||(M_ChkFlag(SL_QEPZDISTRUB)!=0))&&(M_ChkFlag(SL_OCS_EIN)!=0))	//QEPÉÏÒç»òÕßQCAP¡¢QEP ZÐÅºÅÊÜ¸ÉÈÅ¹ÊÕÏ  20090804ÓÚcpc							
	    if((M_ChkFlag(SL_QCAPSPDIN)!=0)&&((M_ChkFlag(SL_QEPPCO)!=0)||(M_ChkFlag(SL_QCAPDISTURB)!=0)||(M_ChkFlag(SL_QEPZDISTRUB)!=0)||(M_ChkFlag(SL_QEPPCDE)!=0)))	//QEPÉÏÒç»òÕßQCAP¡¢QEP Z/A/BÐÅºÅÊÜ¸ÉÈÅ¹ÊÕÏ  201007BJTULVRT						
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_encofault,DELAY_ENCOFAULT)>=0)	//150ms 2010atcpc									//ÖÃ¬³ö×ªËÙ·¶§±êÖ¾Î»
				M_SetFlag(SL_ENCODFAULT);				     			    //ÖÃ±êÖ¾Î»
		}
		else 
		{
			M_ClrFlag(SL_ENCODFAULT);									//Çå±êÖ¾Î»
			MAIN_LOOP.cnt_encofault=0;
		} 

//-----------------------µç»ú¹¤×÷×ªËÙ·¶Î§ÅÐ¶Ï-----------------------------------------------------
//	    if(M_ChkFlag(SL_OCS_EIN)!=0)									//bit0²Å½øÐÐ×ªËÙ³¬³ö·¶Î§ÅÐ¶Ï 201005atcpc
	    if((M_ChkFlag(SL_OCS_EIN)!=0)&&(M_ChkFlag(SL_IN1_CBSTS)!=0))	//bit0²Å½øÐÐ×ªËÙ³¬³ö·¶Î§ÅÐ¶Ï 20120310
		{
//			if((M_ChkFlag(SL_MSPOUT)==0)&& ((PRO.speedflt<_SC_MSPEED1)||(PRO.speedflt>_SC_MSPEED2)))  //¼Ó¸öÑÓÊ±ÅÐ¶Ï20090817
			if((M_ChkFlag(SL_MSPOUT)==0) && (M_ChkFlag(SL_MPR_PWMOUT)!=0) && ((PRO.speedflt<_SC_MSPEED1)||(PRO.speedflt>_SC_MSPEED2)))  //¼Ó¸öÑÓ±ÅÐ¶Ï220120310
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_speedout,DELAY_SPEEDOUT)>=0)	M_SetFlag(SL_MSPOUT);	//50ms 20091022atzy									//ÖÃ¬³ö×ªËÙ·¶§±êÖ¾Î»
			}
			else if((PRO.speedflt>(_SC_MSPEED1 + SC_MSPD_HW))&&(PRO.speedflt<(_SC_MSPEED2 - SC_MSPD_HW)))   //ÅÐ¶Ï¼Ó»Ø²î201005atcpc
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
//-----------------------Íø²à±äÁ÷Æ÷Èí¼þ¹ýÁ÷ÅÐ¶Ï-----------------------------------------------------
		if((M_ChkFlag(SL_SIAC1)==0)&&(PRO.NPR_iac>_SC_IACOV1))
		{
			M_SetFlag(SL_SIAC1);									//ÖÃÈí¼þ¹ýÁ÷±ê¾Î»
		}
		else if((M_ChkFlag(SL_SIAC1)!=0)&&(PRO.NPR_iac<(_SC_IACOV1-SC_IAC_HW))) 
		{
			M_ClrFlag(SL_SIAC1);									//ÇåÈí¼þ¹ýÁ÷±êÖ¾Î»
		}
 

//------------------------µç»ú²à±äÁ÷Æ÷Èí¼þ¹ýÁ÷ÅÐ¶Ï--------------------------------------------------
		if((M_ChkFlag(SL_SIAC2)==0)&&(PRO.MPR_iac>_SC_IACOV2))
		{
			M_SetFlag(SL_SIAC2);									//ÖÃÈí¼þ¹ýÁ÷±êÖ¾Î»
		}
		else if((M_ChkFlag(SL_SIAC2)!=0)&&(PRO.MPR_iac<(_SC_IACOV2-SC_IAC_HW))) 
		{
			M_ClrFlag(SL_SIAC2);									//ÇåÈí¼þ¹ýÁ÷±êÖ¾Î»
		}
*/
/*
//-------------------------ÖÐ¼äÖ±Á÷µçÑ¹Èí¼þÇ·Ñ¹ÅÐ¶Ï----ÓÐÎÊÌâ£¡¸Ã¹ÊÕÏÆÁ±Î---------------------------------------------
	    if((M_ChkFlag(SL_CHARGEOK)!=0)&&(M_ChkFlag(SL_NPR_PWMOUT)!=0))//Ô¤³äµçÍê³Éºó²Å½øÐÇ·Ñ¹ÅÐ¶Ï
		{
			if((M_ChkFlag(SL_SUDCLV)==0)&&(PRO.udc<_SC_UDCLV))	
			{
				M_SetFlag(SL_SUDCLV);									//Ö®Ç°Ã»ÓÐÇ·Ñ¹£¬´Ë¿Ì¼ì²â³öÇ·£¬ÖÃÇ·Ñ¹±êÖ¾
			}	
			else if((M_ChkFlag(SL_SUDCLV)!=0)&&(PRO.udc>=(_SC_UDCLV+SC_UDC_HW)))	
			{
				M_ClrFlag(SL_SUDCLV);									//Ö®Ç°Ç·Ñ¹£¬´Ë¿Ì¼ì²â³ö³¬¹ý(Ç·Ñ¹Öµ¼Ó»Ø²î)£¬ÇåÇ·Ñ¹±êÖ¾
			}		
		}
		else	M_ClrFlag(SL_SUDCLV);				
*/
//-----------------------ÖÐ¼äÖ±Á÷Õ¶²¨µçÁ÷¹ýÁ÷ÅÐ¶Ï-BJTULVRT201204----------------------------------------------------------
		if(M_ChkFlag(SL_LV_STATE)==0)
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_idcovdelay,DELAY_IDCOVDELAY)>=0)
			{
				if((M_ChkFlag(SL_IDCOV)==0)&&((_COMMAND2&0x0002)==0)&&(PRO.CHOP_idc>10))
				{
					if(M_ChkCounter(MAIN_LOOP.cnt_idcov,DELAY_IDCOV)>=0)	M_SetFlag(SL_IDCOV);		//ÖÃ¶¨×ÓµçÁ÷¹ýÁ÷±êÖ¾Î»201205LVRTatZB20120531
				}
				else if((M_ChkFlag(SL_IDCOV)!=0)&&((M_ChkFlag(SL_LV_STATE)==0)||((_COMMAND2&0x0002)==0))&&(PRO.CHOP_idc<5)) 
				{
					M_ClrFlag(SL_IDCOV);									//ÇåchopperµçÁ÷¹ÊÕÏ±ê¾»
					MAIN_LOOP.cnt_idcov = 0;
				}
				else
					MAIN_LOOP.cnt_idcov = 0;  //201205LVRTatZB
			}
			else	
			{
				MAIN_LOOP.cnt_idcov = 0;
				M_ClrFlag(SL_IDCOV);									//ÇåchopperµçÁ÷¹ÊÕÏ±êÖ¾Î»
			}
		}
		else
		{
			MAIN_LOOP.cnt_idcovdelay = 0;
			MAIN_LOOP.cnt_idcov = 0;
			M_ClrFlag(SL_IDCOV);
		}
//------------------------ÖÐ¼äÖ±Á÷µçÑ¹Èí¼þ¹ýÑ¹ÅÐ¶Ï--------------------------------------------------
		if((M_ChkFlag(SL_SUDCOV)==0)&&(PRO.udc>_SC_UDCOV))	                   
		{
			M_SetFlag(SL_SUDCOV);	 	//
		}
		else if((M_ChkFlag(SL_SUDCOV)!=0)&&(PRO.udc<=(_SC_UDCOV-SC_UDC_HW)))   M_ClrFlag(SL_SUDCOV);  //Ö®Ç°¹ýÑ¹£¬´Ë¿Ì¼ì²âµÍÓÚ¹ýÑ¹Öµ¼õ»Ø²î	


//-----------------------ÍøÑ¹Ç·Ñ¹Èí¼þÅÐ¶Ï(ÏßÑ¹ÓÐÐ§Öµ)-----------------------------------------------
	   if(M_ChkCounter(MAIN_LOOP.cnt_uaclv1,DELAY_UACLV1)>0)	//10s ÉÏµçÑÓÊ±ÅÐ¶ÏÍø²àÈíÇ·Ñ¹¹ÊÕÏ
	   {
		if(M_ChkFlag(SL_UACLV1)==0)
		{
			if((PRO.NPR_uab<_SC_UACLV1)||(PRO.NPR_ubc<_SC_UACLV1))	M_SetFlag(SL_UACLV1);  //20091026atzy Á½ÏßµçÑ¹ÈÎÒâÒ»¸ö²»´ïÒªÇó,ÖÃÇ·êÖ¾
		}
		else 
		{
			if((PRO.NPR_uab>=(_SC_UACLV1+SC_UAC_HW))&&(PRO.NPR_ubc>=(_SC_UACLV1+SC_UAC_HW)))	M_ClrFlag(SL_UACLV1); //20091026atzy Á½¸öÏßµçÑ¹ù»Ø¸´µ½Õý³£ºóÇåÇ·¹±êÖ¾	
		}
	   }					
//----------------------ÍøÑ¹¹ýÑ¹Èí¼þÅÐ¶Ï(ÏßÑ¹ÓÐÐ§Öµ)------------------------------------------------
		if(M_ChkFlag(SL_UACOV1)==0)
		{
			if((PRO.NPR_uab>_SC_UACOV1)||(PRO.NPR_ubc>_SC_UACOV1))	 M_SetFlag(SL_UACOV1);

		}
		else
 		{
   			if((PRO.NPR_uab<=(_SC_UACOV1-SC_UAC_HW))&&(PRO.NPR_ubc<=(_SC_UACOV1-SC_UAC_HW)))	M_ClrFlag(SL_UACOV1);

		}


//------------------------µçÍøÆµÂÊ¹ÊÕÏ--------------------------------------------------------------
/*		if(M_ChkFlag(SL_IN1_CBSTS)!=0)     									//Ö÷¶Ï±ÕºÏÔÙÅÐ¶ÏÍøÆµ¹ÊÕÏ 20090816
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_cbfreq,DELAY_CBFREQ)>=0)      	//1s
			{
*/				if(M_ChkFlag(SL_GRDFQE)!=0)									//10¸öÍøÑ¹ÖÜÆÚ(200ms),ÔòÖÃÆµÂÊ´íÎó±êÖ¾
				{
					if(M_ChkCounter(MAIN_LOOP.cnt_freq,DELAY_FREQ)>=0)	 M_SetFlag(SL_FE1);	
				}	
				else
				{
					M_ClrFlag(SL_FE1);									   	//·ñÔòÇåÆµÂÊ´íÎó±êÖ¾
					MAIN_LOOP.cnt_freq=0;                                  	//¼ÆÊ±Æ÷Ã¿1ms¼Ó1
				}
/*			}
		}
		else   	
		{
			MAIN_LOOP.cnt_cbfreq=0;	
			M_ClrFlag(SL_FE1);
		}
*/	
//-----------------------¶¨×ÓµçÁ÷¹ýÁ÷ÅÐ¶Ï-----------------------------------------------------------
		if((M_ChkFlag(SL_SIOVST)==0)&&((PRO.STA_iac>_SC_IACOVST)||(PRO.STA_icb>_SC_IACOVST)))
		{
			M_SetFlag(SL_SIOVST);									//ÖÃ¶¨×ÓµçÁ÷¹ýÁ÷±êÖ¾Î»
		}
		else if((M_ChkFlag(SL_SIOVST)!=0)&&(PRO.STA_iac<(_SC_IACOVST-SC_IAC_HW))&&(PRO.STA_icb<(_SC_IACOVST-SC_IAC_HW))) 
		{
			M_ClrFlag(SL_SIOVST);									//Çå¶¨×ÓµçÁ÷¹ýÁ÷±êÖ¾Î»
		}
/*
//-----------------------ÖÐ¼äµçÑ¹²»ÎÈ¶¨---ÔÝÊ±²»ÓÃ201005atcpc----------------------------------------------------------
        if(M_ChkFlag(SL_NPR_PWMOUT)!=0 && M_ChkFlag(SL_MPR_PWMOUT)!=0)
		{
		   if((AD_OUT_UDC<(_URF - 70))||(AD_OUT_UDC>(_URF + 70)))  //ÖÐ¼äµçÑ¹³¬³ö·¶Î§ºóÖÃ¹ÊÕÏ±êÖ¾Î» new
		   {
		       if(M_ChkCounter(MAIN_LOOP.cnt_steadyfb,DELAY_UDCWAVE)>0)
			   {                                   //DELAY_STEADYFB£¬cnt_steadyfbÓësysctrlÀïÃæµÄÊÇÍ¬Ò»¸ö
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
//-----------------------SCR ¶Ï²»¿ª¹ÊÕÏLVRT---201007BJTULVRT----------------------------------------------------------
 //       if((M_ChkFlag(SL_LV_SCRON)!=0)&&(M_ChkFlag(SL_LV_SCRIZERO)==0))
        if(M_ChkFlag(SL_LV_SCRIZERO)==0)	//20110829HC
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

//------------------------Íø²à½ÓµØ¹ÊÕÏ-ÈýÏàµçÁ÷²»Æ½ºâ-----20100302---------------------------------------------------------
		if(M_ChkFlag(SL_IUBLNCE1)!=0)
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_gfault1,DELAY_GFAULT)>=0)	 M_SetFlag(SL_GFAULT1);	
		}	
		else
		{
			M_ClrFlag(SL_GFAULT1);									  
			MAIN_LOOP.cnt_gfault1=0;                               
		}		

//------------------------»ú²à½ÓµØ¹ÊÕÏ-ÈýÏàµçÁ÷²»Æ½ºâ-----20100302---------------------------------------------------------
		if(M_ChkFlag(SL_IUBLNCE2)!=0)
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_gfault2,DELAY_GFAULT)>=0)	 M_SetFlag(SL_GFAULT2);	
		}	
		else
		{
			M_ClrFlag(SL_GFAULT2);									  
			MAIN_LOOP.cnt_gfault2=0;                               
		}		

//-----------------------Èí¼þÎÂ¶È·´À¡SKIIP³¬ÎÂ¹ÊÕÏÅÐ¶Ï----------------------------------------------
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
			M_SetFlag(SL_SKTOV);									//ÖÃSKIIP³¬ÎÂ¹ÊÕÏ±êÖ¾Î»
		}
		else if((M_ChkFlag(SL_SKTOV)!=0)&&(AMUX.skiiptempmax < (_SC_SKPTOV-10))) 
		{
			M_ClrFlag(SL_SKTOV);									//ÇåSKIIP³¬ÎÂ¹ÊÕÏ±êÖ¾Î»
		}
        
//----------------------Íø²àSKIIP³¬ÎÂ¹ÊÕÏ-----201005atcpc-----------------------------------------------------			
		if(M_ChkFlag(SL_IN2_TAOV)!=0)
		{	
			if(M_ChkCounter(MAIN_LOOP.cnt_nskiptov,DELAY_NSKIPTOV)>=0)	M_SetFlag(SL_TAOV); 	//Ã³ÎÂ±êÖ¾Î»
		}
       	else
		{
			MAIN_LOOP.cnt_nskiptov=0; 								//ÇåÑÓÊ±¼ÆÊý
			M_ClrFlag(SL_TAOV);										//Çå³¬ÎÂ±êÖ¾Î»
		}

//--------------------µç»ú²àSKIIP³¬ÎÂ¹ÊÕÏ-----201005atcpc------------------------------------------------------	
		if(M_ChkFlag(SL_IN2_TBOV)!=0)
		{	
			if(M_ChkCounter(MAIN_LOOP.cnt_mskiptov,DELAY_MSKIPTOV)>=0)	M_SetFlag(SL_TBOV); 	//ÖÃ³¬ÎÂ±êÖ¾Î»
		}
       	else
		{
			MAIN_LOOP.cnt_mskiptov=0; 								//ÇåÑÓÊ±¼ÆÊý
			M_ClrFlag(SL_TBOV);										//Çå³¬ÎÂ±êÖ¾Î»
		}

//----------------------Íø²àµç¸Ð³¬ÎÂ¹ÊÕÏ----------------------------------------------------------			
		if((M_ChkFlag(SL_NPR_TLOV)==0)&&(PRO.NPR_TLOV>_SC_NPR_TLOV))
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_nprtlov,DELAY_NPRTLOV)>=0)	M_SetFlag(SL_NPR_TLOV);	//ÖÃÍø²àµç¸Ð³¬ÎÂ±êÖ¾Î»
		}
		else if((M_ChkFlag(SL_NPR_TLOV)!=0)&&(PRO.NPR_TLOV<(_SC_NPR_TLOV - 5))) 
		{
			M_ClrFlag(SL_NPR_TLOV);									//åÍø²àµç¸Ð³¬ÎÂ±êÖ¾Î»
			MAIN_LOOP.cnt_nprtlov=0;
		} 


//--------------------µçú²µç¸Ð³¬ÎÂ¹ÊÕÏ-----------------------------------------------------------	
		if((M_ChkFlag(SL_MPR_TLOV)==0)&&(PRO.MPR_TLOV>_SC_MPR_TLOV))
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_mprtlov,DELAY_MPRTLOV)>=0)	M_SetFlag(SL_MPR_TLOV);	//ÖÃ»ú²àµç¸Ð³¬ÎÂ±êÖ¾Î»
		}
		else if((M_ChkFlag(SL_MPR_TLOV)!=0)&&(PRO.MPR_TLOV<(_SC_MPR_TLOV - 5))) 
		{
			M_ClrFlag(SL_MPR_TLOV);									//Çå»ú²àµç¸Ð³¬ÎÂ±êÖ¾Î»
			MAIN_LOOP.cnt_mprtlov=0;
		} 


//-----------------------±äÁ÷Æ÷ÓÐ¹¦¹¦ÂÊ¹ýÔØÅÐ¶Ï-----------------------------------------------------------
/*		temp_pgactive = abs(PRO.Pgactive);							//20090816
		temp_pgactive = temp_pgactive * 0.001;
		if((M_ChkFlag(SL_PGOV)==0)&&(temp_pgactive>_SC_PGOV))
		{
			M_SetFlag(SL_PGOV_COM);
			if(M_ChkCounter(MAIN_LOOP.cnt_pgovload,DELAY_PGOVLOAD)>0)  	//10s
				M_SetFlag(SL_PGOV);										//ÖÃ±äÁ÷Æ÷ÓÐ¹¦¦ÂÊ¹ýÔØ±êÖ¾Î»
		}
		else if((M_ChkFlag(SL_PGOV)!=0)&&(temp_pgactive<(_SC_PGOV-SC_POWOROV_HW))) 
		{
			M_ClrFlag(SL_PGOV);
			M_ClrFlag(SL_PGOV_COM);										//Çå±äÁ÷Æ÷ÓÐ¹¦¹¦ÂÊ¹ýÔØ±êÖ¾Î»
			MAIN_LOOP.cnt_pgovload=0;
		}
		else  MAIN_LOOP.cnt_pgovload=0;

//-----------------------±äÁ÷Æ÷ÎÞ¹¦¹¦ÂÊ¹ýÔØÅÐ¶Ï-----------------------------------------------------------
        temp_pgreactive = abs(PRO.Pgreactive);							//20090816
        temp_pgreactive = temp_pgreactive * 0.001;
		if((M_ChkFlag(SL_QGOV)==0)&&(temp_pgreactive>_SC_QGOV))
		{
			M_SetFlag(SL_QGOV_COM);										//20091007
			if(M_ChkCounter(MAIN_LOOP.cnt_qgovload,DELAY_QGOVLOAD)>0)  	//10s
				M_SetFlag(SL_QGOV);										//ÖÃ±äÁ÷Æ÷ÎÞ¹¦¹¦ÂÊ¹ýÔØ±êÖ¾Î»
		}
		else if((M_ChkFlag(SL_QGOV)!=0)&&(temp_pgreactive<(_SC_QGOV-SC_POWOROV_HW))) 
		{
			M_ClrFlag(SL_QGOV);	
			M_ClrFlag(SL_QGOV_COM);										//Çå±äÁ÷Æ÷ÎÞ¹¦¹¦ÂÊ¹ýÔØ±êÖ¾Î»
			MAIN_LOOP.cnt_qgovload=0;
		} 
		else MAIN_LOOP.cnt_qgovload=0;
*/
//-----------------------¶¨×ÓÍ¬²½²¢ÍøÊ§°Ü¹ÊÕÏ---------------------------------------
/*	    if(M_ChkFlag(SL_OCS_SYSRUN)!=0 && M_ChkFlag(SL_MPR_SYNOK)==0 && M_ChkFlag(SL_ENPCOPER)==0 )	//PCµ÷ÊÔ²»½øÐÐ¸Ã¹ÊÕÏÅÐ¶Ï							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_synfail,120000)>=0)	M_SetFlag(SL_SYNFAIL);	  //2min  201005atcpc
			else M_ClrFlag(SL_SYNFAIL);								 	//Çå±êÖ¾Î»
		}
		else 
		{
			MAIN_LOOP.cnt_synfail=0;                                    //Çå¶¨Ê±Æ÷
			M_ClrFlag(SL_SYNFAIL);
		} 
*/
//--------20130801--------
	    if(M_ChkFlag(SL_OCS_SYSRUN)!=0 && M_ChkFlag(SL_MPR_SYNOK)==0 && M_ChkFlag(SL_ENPCOPER)==0 )	//PCµ÷ÊÔ²»½øÐÐ¸Ã¹ÊÕÏÅÐ¶Ï							
//		if(M_ChkFlag(SL_MPR_START)!=0 && M_ChkFlag(SL_MPR_SYNOK)==0 && M_ChkFlag(SL_ENPCOPER)==0 )	//PCµ÷ÊÔ²»½øÐÐ¸Ã¹ÊÕÏÅÐ¶Ï							
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_synfail,60000)>=0)
			{
				M_SetFlag(SL_SYNFAIL);	  //2min  201005atcpc
				ecpostep1 = 0;
				ecpostep2 = 0;
				ecpostep3 = 0;
				MAIN_LOOP.cnt_synfail = 0;

			}
			else M_ClrFlag(SL_SYNFAIL);								 	//Çå±êÖ¾Î»
		}
		else 
		{
//			if(M_ChkFlag(SL_ENCODEPOS_OK)!=0 && M_ChkFlag(SL_ECPOSSEARCH)==0)	MAIN_LOOP.cnt_synfail=0;		//Çå¶¨Ê±Æ÷
			if(M_ChkFlag(SL_ECPOSSEARCH)==0)	MAIN_LOOP.cnt_synfail=0;		//Çå¶¨Ê±Æ÷
			M_ClrFlag(SL_SYNFAIL);
		}

//--------------------------------µÃµ½TAB_MSGÖÐµÄ¹ÊÏÐòºÅ-systest------------------------------------------
		_MSG_SCOUT2 = MSG_NONE;												//ÏÈ½«MSGÇåÁã
		
		if(M_ChkFlag(SL_CODEOK)==0)				_MSG_SCOUT2=MSG_CODEOK;     //1=¹¦ÄÜÂëÎ´Ð£ÑéÍê±Ï
		
		else if(M_ChkFlag(SL_EE_FAIL)!=0) 		_MSG_SCOUT2=MSG_EE_FAIL;    //2=EEPROM¹ÊÕÏ

		else if(M_ChkFlag(SL_ESTOP)!=0) 	    _MSG_SCOUT2=MSG_ESTOP;      //3=½ô¼±Í£»ú¹ÊÕÏ

		else if(M_ChkFlag(SL_CBTRIP)!=0) 	    _MSG_SCOUT2=MSG_CBTRIP;     //4=Ö÷¶ÏÍÑ¿Û¹ÊÕÏ

		else if(M_ChkFlag(SL_EXFAIL)!=0) 	 	_MSG_SCOUT2=MSG_EXFAULT;    //5=±äÁ÷Æ÷Íâ²¿Ó²¼þ¹ÊÕÏ

		else if(M_ChkFlag(SL_CANOPENOVER)!=0) 	_MSG_SCOUT2=MSG_CAN_FAIL;   //6=CANÍ¨Ñ¶¹ÊÕÏ//controltest

		else if(M_ChkFlag(SL_ENCODFAULT)!=0) 	_MSG_SCOUT2=MSG_ENCODFAULT; //7=±àÂëÆ÷¹ÊÕÏ//controltest

		else if(M_ChkFlag(SL_ERROR_CB)!=0) 	 	_MSG_SCOUT2=MSG_CBERROR;    //8=Ö÷¶ÏºÏÕ¢¹ÊÕÏ

		else if(M_ChkFlag(SL_ERROR_PRE)!=0) 	_MSG_SCOUT2=MSG_PREERROR;   //9=Ô¤³äµç¹ÊÕÏ

		else if(M_ChkFlag(SL_ERROR_MAINK)!=0) 	_MSG_SCOUT2=MSG_MAINKERROR; //10=Ö÷½Ó´¥Æ÷±ÕºÏ¹ÊÕÏ

		else if(M_ChkFlag(SL_ERROR_MAINF)!=0) 	_MSG_SCOUT2=MSG_MAINFERROR; //11=Ö÷ÂË²¨Æ÷±ÕºÏ¹ÊÕÏ

		else if(M_ChkFlag(SL_ERROR_STAC)!=0) 	_MSG_SCOUT2=MSG_STACERROR;  //12=¶¨×ÓÓ´¥Æ÷±ÕºÏ¹ÊÕÏ

		else if(M_ChkFlag(SL_HIA1)!=0)			_MSG_SCOUT2=MSG_HIA1;     	//13=Íø²à±äÁ÷Æ÷AÏàSKIIP¹ÊÕÏ

		else if(M_ChkFlag(SL_HIB1)!=0)			_MSG_SCOUT2=MSG_HIB1;     	//14=Íø²à±äÁ÷Æ÷BÏàSKIIP¹ÊÕÏ

		else if(M_ChkFlag(SL_HIC1)!=0)			_MSG_SCOUT2=MSG_HIC1;     	//15=Íø²à±äÁ÷Æ÷CÏàSKIIP¹ÊÕÏ
		
		else if(M_ChkFlag(SL_HIA2)!=0)			_MSG_SCOUT2=MSG_HIA2;     	//16=µç»ú²à±äÁ÷Æ÷AÏàSKIIP¹ÊÏ

		else if(M_ChkFlag(SL_HIB2)!=0)			_MSG_SCOUT2=MSG_HIB2;     	//17=µç»ú²à±äÁ÷Æ÷BÏàSKIIP¹ÊÕÏ

		else if(M_ChkFlag(SL_HIC2)!=0)			_MSG_SCOUT2=MSG_HIC2;     	//18=µç»ú²à±äÁ÷Æ÷CÏàSKIIP¹ÊÕÏ

		else if(M_ChkFlag(SL_HUDCOV)!=0)		_MSG_SCOUT2=MSG_UDCOV;    	//19=±äÁ÷Æ÷Ö±Á÷Ä¸ÏßÓ²¼þ¹ýÑ¹¹ÊÕÏ

        else if(M_ChkFlag(SL_HIACOV1)!=0)		_MSG_SCOUT2=MSG_HIAC1;    	//20=Íø²àÓ²¼þ¹ýÁ÷¹ÊÕÏ

		else if(M_ChkFlag(SL_HIACOV2)!=0)		_MSG_SCOUT2=MSG_HIAC2;    	//21=µç»ú²àÓ²¼þ¹ýÁ÷¹ÊÕÏ

        else if(M_ChkFlag(SL_PDPASERIES)!=0)	_MSG_SCOUT2=MSG_PDPASERIES;	//22=Íø²àÑÏÖØ¹ÊÕÏ

		else if(M_ChkFlag(SL_PDPBSERIES)!=0)	_MSG_SCOUT2=MSG_PDPBSERIES;	//23=Íø²àÑÏÖØ¹ÊÕÏ

		else if(M_ChkFlag(SL_MSPOUT)!=0) 	    _MSG_SCOUT2=MSG_MSPEEDOUT;  //24=×ªËÙ³¬³ö·¶Î§¹ÊÕÏ

		else if(M_ChkFlag(SL_SIAC1)!=0)			_MSG_SCOUT2=MSG_SIAC1;    	//25=Íø²àÈí¼þ¹ýÁ÷¹ÊÕÏ

		else if(M_ChkFlag(SL_SIAC2)!=0)			_MSG_SCOUT2=MSG_SIAC2;    	//26=µç»ú²àÈí¼þ¹ýÁ÷¹ÊÕÏ

		else if(M_ChkFlag(SL_FE1)!=0)			_MSG_SCOUT2=MSG_FE1;      	//27=Íø²àÆµÂÊ²»·û¹ÊÕÏ//controltest

		else if(M_ChkFlag(SL_SUDCOV)!=0)		_MSG_SCOUT2=MSG_SUDCOV;   	//28=Èí¼þÖÐ¼äÖ±Á÷µçÑ¹¹ýÑ¹
		
		else if(M_ChkFlag(SL_UACOV1)!=0)		_MSG_SCOUT2=MSG_SUACOV1;  	//29=Èí¼þÍøÑ¹½»Á÷¹ýÑ¹

//		else if(M_ChkFlag(SL_SUDCLV)!=0)		_MSG_SCOUT2=MSG_SUDCLV;   	//30=Èí¼þÖÐ¼äÖ±Á÷µçÑ¹Ç·Ñ¹
		else if((M_ChkFlag(SL_IDCOV)!=0)||(M_ChkFlag(SL_HIDCOV)!=0))	_MSG_SCOUT2=MSG_SIDCOV;   	//30=¸ÄÎªdc-chopperµçÁ÷±£»¤BJTULVRT201204

//		else if(M_ChkFlag(SL_UACLV1)!=0 || M_ChkFlag(SL_UACLV2)!=0)		_MSG_SCOUT2=MSG_SUACLV1;  	//31=Èí¼þÍøÑ¹½»Á÷Ç·Ñ¹ 201007BJTULVRT				
		else if(M_ChkFlag(SL_UACLV1)!=0)		_MSG_SCOUT2=MSG_SUACLV1;  	//31=Èí¼þÍøÑ¹½»Á÷Ç·Ñ¹
		
		else if(M_ChkFlag(SL_UDCWAVE)!=0)		_MSG_SCOUT2=MSG_UDCWAVE;  	//32=ÖÐ¼äµçÑ¹²¨¶¯¹ÊÕÏ

        else if(M_ChkFlag(SL_SIOVST)!=0)		_MSG_SCOUT2=MSG_SIOVST;   	//33=Èí¼þ¼ì²â¶¨×Ó¹ýÁ÷¹ÊÕÏ
		
		else if(M_ChkFlag(SL_GFAULT1)!=0)		_MSG_SCOUT2=MSG_GFAULT1;   	//34=Íø²à½ÓµØ¹ÊÕÏ
		
		else if(M_ChkFlag(SL_GFAULT2)!=0)		_MSG_SCOUT2=MSG_GFAULT2;  	//35=»ú²à½ÓµØ¹ÊÕÏ

		else if(M_ChkFlag(SL_TAOV)!=0)			_MSG_SCOUT2=MSG_TOV1;	  	//36=Íø²àSKIIP³¬ÎÂ//controltest
		
		else if(M_ChkFlag(SL_TBOV)!=0)			_MSG_SCOUT2=MSG_TOV2;  	  	//37=µç»ú²àSKIIP³¬ÎÂ//controltest

		else if(M_ChkFlag(SL_SKTOV)!=0)			_MSG_SCOUT2=MSG_SKTOV;     	//38=Èí¼þÅÐ¶ÏSKIIP³¬ÎÂ

        else if(M_ChkFlag(SL_NPR_TLOV)!=0)		_MSG_SCOUT2=MSG_TLOV1;     	//39=µçÍø²àµç¸Ð³¬ÎÂ

		else if(M_ChkFlag(SL_MPR_TLOV)!=0)		_MSG_SCOUT2=MSG_TLOV2;     	//40=µç»ú²àµç¸Ð³¬ÎÂ

//		else if(M_ChkFlag(SL_PGOV)!=0)		    _MSG_SCOUT2=MSG_PGOV;      	//41=±äÁ÷Æ÷ÓÐ¹¦¹¦ÂÊ¹ýÔØ

//		else if(M_ChkFlag(SL_QGOV)!=0)	    	_MSG_SCOUT2=MSG_QGOV;      	//42=±äÁ÷Æ÷ÎÞ¹¦¹¦ÂÊ¹ýÔØ

		else if(M_ChkFlag(SL_SYNFAIL)!=0)	    _MSG_SCOUT2=MSG_STASYNFAIL; //43=¶¨×ÓÍ¬²½²¢ÍøÊ§°Ü¹ÊÕÏ

		else if(M_ChkFlag(SL_PHORDE)!=0) 		_MSG_SCOUT2=MSG_PHORDE;     //44=µçÍøÏàÐò´íÎó //controltest

//------------------------PCÊ¾²¨Æ÷¹ÊÕÏ²¨ÐÎ´æ´¢´¦Àí--20100308-------------------------------------------------------

//		if(_MSG_SCOUT2!=MSG_NONE)												//±¾´ÎÓÐ¹ÊÕÏ·¢Éú
		if((_MSG_SCOUT2!=MSG_NONE)&&(_MSG_SCOUT2!=6))		//±¾´ÎÓÐ¹ÊÕÏ·¢Éú ²»´æ´¢canopen¹ÊÕÏ20121129
		{
			if(_PCEROTRIG==0)					M_SetFlag(SL_TRIG_ERRDSAVE);	//ËùÓÐ¹ÊÕÏ´¥·¢Ëø´æ 201005atcpc
			else if(_MSG_SCOUT2==_PCEROTRIG)	M_SetFlag(SL_TRIG_ERRDSAVE);	//Ä³ÊÕÏ´¥·¢Ëø´æ
			else if((_PCEROTRIG==50)&&(_MSG_SCOUT2==14||_MSG_SCOUT2==15||_MSG_SCOUT2==16||_MSG_SCOUT2==21||_MSG_SCOUT2==23))
				M_SetFlag(SL_TRIG_ERRDSAVE);	 								//ËùÓÐÍø²àÓ²¼þ¹ÊÕÏ´¥·¢Ëø´æ
			else if((_PCEROTRIG==51)&&(_MSG_SCOUT2==17||_MSG_SCOUT2==18||_MSG_SCOUT2==19||_MSG_SCOUT2==22||_MSG_SCOUT2==24))
				M_SetFlag(SL_TRIG_ERRDSAVE);	 								//ËùÓÐ»ú²àÓ²¼þ¹ÊÕÏ´¥·¢Ëø´æ
		}
		
		if((_PCEROTRIG==99)&&(M_ChkFlag(SL_PC_ERRDATASAVE)!=0))
				M_SetFlag(SL_TRIG_ERRDSAVE);	 								//PC´¥·¢Íâ²¿RAMÊý¾ÝËø´æ 201005atcpc

//------------------------¹ÊÕÏ´¦Àí--systest-------------------------------------------------------

//NO1-------Ô­À´Ã»ÓÐ¹ÊÕÏ±¾´ÎÓÐ¹ÊÕÏ(»ò)Ô­À´ÓÐ¹ÊÕÏµ«±¾´Î¼¶±ð¸ü¸ß-------------------£¨¹ÊÕÏÉý¼¶£©------
		if(((_MSG_SCOUT2!=MSG_NONE)&&(M_ChkFlag(SL_ERROR)==0))||((_MSG_SCOUT2!=MSG_NONE)&&(M_ChkFlag(SL_ERROR)!=0)&&(TAB_MSG[_MSG_SCOUT2].rank > TAB_MSG[_MSG_SCOUT1].rank)))
		{													
			M_SetFlag(SL_ERROR);							//ÖÃ¹ÊÕÏ±êÖ¾Î»
			M_SetFlag(SL_DISPLAY5);                         //ÖÃÏµÍ³¹ÊÕÏÖ¸Ê¾

//----------------------------Â¹ÊÕÏÊôÐÔ±êÊ¾-------------------------------------------------------												
			if((TAB_MSG[_MSG_SCOUT2].attr & OFFCB)==OFFCB)		M_SetFlag(SL_OFFCB);	//ÑÏÖØ¼±Í£ÊôÐÔ
			else												M_ClrFlag(SL_OFFCB);

			if((TAB_MSG[_MSG_SCOUT2].attr & SHUT)==SHUT)		M_SetFlag(SL_SHUT);		//Ò»°ãÍ£»úÊôÐÔ
			else												M_ClrFlag(SL_SHUT);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & N_RCVR)==N_RCVR)	M_SetFlag(SL_NRCVR);	//²»ÄÜ»Ö¸´ÊôÐÔ
			else												M_ClrFlag(SL_NRCVR);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & I_RCVR)==I_RCVR)	M_SetFlag(SL_IRCVR);	//Á¢¼´»Ö¸´ÊôÐÔ
			else												M_ClrFlag(SL_IRCVR);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & D_RCVR)==D_RCVR)	M_SetFlag(SL_DRCVR);	//ÑÓÊ±»Ö¸´ÊôÐÔ
			else												M_ClrFlag(SL_DRCVR);
						
			if((TAB_MSG[_MSG_SCOUT2].attr & CNT)==CNT)			M_SetFlag(SL_CNT);		//¼Æ´ÎÊýÊôÐÔ
			else												M_ClrFlag(SL_CNT);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & OT_SER)==OT_SER)	M_SetFlag(SL_OTSER);	//³¬Ê±ÑÏÖØÊôÐÔ
			else												M_ClrFlag(SL_OTSER);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & SAVE)==SAVE)		M_SetFlag(SL_SAVE);		//ÐèÒª±£´æ¹ÊÕÏÐÅÏ¢ÊôÐÔ
			else												M_ClrFlag(SL_SAVE);		

			if((TAB_MSG[_MSG_SCOUT2].attr & WARNING)==WARNING)	M_SetFlag(SL_WARNING);	//±¨¾¯ÊôÐÔ
			else												M_ClrFlag(SL_WARNING);	

//----------------------------------¸ù¾Ý¹ÊÕÏÊôÐÔÅÐ¶ÏÏµÍ³¶¯×÷---------------------------------------
						
			if(M_ChkFlag(SL_NRCVR)!=0)		//²»¿É¸´Î»¹ÊÕÏ
			{
				M_SetFlag(SL_NO_RESET);		//²»¿É¸´Î»¹ÊÕÏ,ÖÃ ½ûÖ¹¸´Î» ±êÖ¾
				M_ClrFlag(SL_NRCVR);		//Çå²»¿É¸´Î»¹ÊÕÏ
			}

			if(M_ChkFlag(SL_DRCVR)!=0)		//ÑÓÊ±¸´Î»¹ÊÕÏ
			{
				M_SetFlag(SL_DL_RESET);		//Ê×ÏÈÖÃ ½ûÖ¹¸´Î» ±êÖ¾
			}											

/*			if(M_ChkFlag(SL_IRCVR)!=0)		//ÔÊÐíÁ¢¼´¸´Î»¹ÊÕÏ,Ã»ÓÐ²Ù×÷µÈ´ý¸´Î»
			{
//				M_ClrFlag(SL_NO_RESET);		//Çå ½ûÖ¹¸´Î» ±êÖ¾
				M_ClrFlag(SL_IRCVR);							
			}											
*/
            if(M_ChkFlag(SL_OFFCB)!=0)		//ÏÖØ¹ÊÕÏ
			{
				M_SetFlag(SL_SERIESTOP);	//ÖÃ ÑÏÖØ¹ÊÕÏÍ£»ú ±êÖ¾											
				M_ClrFlag(SL_OFFCB);
			}

            if(M_ChkFlag(SL_SHUT)!=0)		//Ò»°ã¹ÊÕÏ
			{
				M_SetFlag(SL_ERRSTOP);		//ÖÃ Ò»°ã¹ÊÕÏÍ£»ú ±êÖ¾												
				M_ClrFlag(SL_SHUT);
			}

    		if(M_ChkFlag(SL_CNT)!=0)
			{
				if(R_SCOUT == _MSG_SCOUT2)	_SY_RTRN++;				//ÖØ¸´·¢ÉúÍ¬Ò»¹ÊÕÏ
				else
				{
					R_SCOUT = _MSG_SCOUT2;
					_SY_RTRN=0;
				}
				if(_SY_RTRN>=_SC_RTRN)		M_SetFlag(SL_NO_RESET);	//ÖÃ ½ûÖ¹¸´Î» ±êÖ¾	
				M_ClrFlag(SL_CNT);				
			}

        	if(M_ChkFlag(SL_WARNING)!=0)   //ÔÝÊ±Ã»ÓÐ½øÐÐºÎ²Ù×÷,½öÍ¨¹ýÍ¨Ñ¶·¢¸øÖ÷¿Ø
			{
				M_ClrFlag(SL_WARNING);
			} 

			MAIN_LOOP.cnt_rcvr=0;			//ÇåÑÓÊ±»Ö¸´¼ÆÊýÆ÷	
			_MSG_SCOUT1 = _MSG_SCOUT2;		//±¾´Î¹ÊÕÏÐÅÏ¢×ª´æ
								
//----------------------------¸üÐÂ¹ÊÏ¼ÇÂ¼²¢ÏòÉÏÎ»»ú±¨¹ÊÕÏ------------------------------------------						
			if((M_ChkFlag(SL_SAVE)!=0)&&(M_ChkFlag(SL_EEBUSY_ERRSAVE)==0))			
			{
				MAIN_LOOP.cnt_rcvr=0;									//åÑÓÊ±»Ö¸´¼ÆÊý	

				M_SetFlag(SL_EEASK_ERRSAVE);							//EEPROM²Ù×÷ëÇó
				M_ClrFlag(SL_SAVE);
				_BA_ERR1 = _BA_ERR2;									//¹ÊÕÏÐÅÏ¢±£´æ
				_BA_ERR2 = _BA_ERR3;
				_BA_ERR3 = _BA_ERR4;
				_BA_ERR4 = _MSG_SCOUT2;
       			
				_BA_EURF   = (int16)RUN.urf;							//ÖÐ¼äÖ±Á÷µçÑ¹¸ø¶¨
				_BA_EUDC   = (int16)AD_OUT_UDC;							//ÖÐ¼äÖ±Á÷µçÑ¹
				_BA_EMIDRF = (int16)(RUN.mpridrf * 10);					//dÖáµçÁ÷¸ø¶¨
				_BA_ENIQRF = (int16)(RUN.npriqrf * 10);					//qÖáµçÁ÷¸ø¶¨
				_BA_EMIQRF = (int16)(RUN.mpriqrf * 10);					//qÖáµçÁ÷¸ø¶¨
				_BA_ETOQRF = (int16)RUN.toqrf;							//×ª¾ØµçÁ÷¸ø¶¨
                _BA_EAGLRF = (int16)RUN.aglrf;            			 //ÎÞ¹¦½Ç¶ÈÖ¸Áî

				_BA_EIA1  = (int16)(AD_OUT_NPR_I.a * 10);				//Íø²à±äÁ÷Æ÷,AÏàµçÁ÷Ë²Ê±Öµ
				_BA_EIB1  = (int16)(AD_OUT_NPR_I.b * 10);				//Íø²à±äÁ÷Æ÷,BÏàµçÁ÷Ë²Ê±Öµ
				_BA_EIC1  = (int16)(AD_OUT_NPR_I.c * 10);				//Íø²à±äÁ÷Æ÷,CÏàçÁ÷Ë²Ê±Öµ
				_BA_EIA2  = (int16)(AD_OUT_MPR_I.a * 10);				//µç»ú²à±äÁ÷Æ÷,AÏàµçÁ÷Ë²Ê±Öµ
				_BA_EIB2  = (int16)(AD_OUT_MPR_I.b * 10);	            //µç»ú²à±äÁ÷÷,BÏàµçÁ÷Ë²Ê±Öµ
				_BA_EIC2  = (int16)(AD_OUT_MPR_I.c * 10);	            //µç»ú²à±äÁ÷Æ÷,CÏàµçÁ÷Ë²Ê±Öµ
				
				_BA_EUAB1  = (int16)PRO.NPR_uab;						//Íø²àabÏßµçÑ¹
				_BA_EUBC1  = (int16)PRO.NPR_ubc;						//Íø²àbcÏßµçÑ¹
				_BA_EUAB2  = (int16)PRO.STA_uab;						//¶¨×Ó²àabÏßµçÑ¹
				_BA_EUBC2  = (int16)PRO.STA_ubc;						//¶¨×Ó²àbcÏßµçÑ¹

//				_BA_EUAB0    = (int16)DISP.grd_uab;			       		//Ö÷¶ÏÇ°Íø²àabÏßÑ¹
//            	_BA_EUBC0    = (int16)DISP.grd_ubc;			       		//Ö÷¶ÏÇ°Íø²àbcÏßÑ¹
				_BA_EUAB0    = (int16)(AD_OUT_DC_I.b);			       	//Ö÷¶ÏÇ°Íø²àabÏßÑ¹BJTULVRT201204
            	_BA_EUBC0    = (int16)GRD_Ut;			       			//Ö÷¶ÏÇ°Íø²àbcÏßÑ¹

				_BA_ETLAC  = (int16)PRO.NPR_TLOV;                           //Íø²àµç¸ÐÎÂ¶È
				_BA_ETLDUDT= (int16)PRO.MPR_TLOV;                           //»ú²àµç¸ÐÎÂ¶È
				_BA_ETSKIIP= (int16)AMUX.skiiptempmax;                      //SKIIPÎÂ¶È	

				_BA_EFREQ  = (int16)(CAP4.freq * 10);						//µçÍøÆµÂÊ
				_BA_ESPEED = (int16)(PRO.speed);							//µç»ú×ªËÙ 

	            _BA_ENPRUD = (int16)TRS_NGS_U.d;              			//dÖáÖ÷¶ÏºóÍøÑ¹·´À¡20091026atzy
				_BA_ENPRUQ = (int16)TRS_NGS_U.q;          				//qÖáÖ÷¶ÏºóÍøÑ¹·´À¡20091026atzy
	            _BA_ENPRUD2 = (int16)TRS_NGS_U.dflt;          			//dÖáÖ÷¶ÏºóÍøÑ¹·´À¡ ÂË²¨ºó20091026atzy
				_BA_ENPRUQ2 = (int16)TRS_NGS_U.qflt;          			//qÖáÖ÷¶ÏºóÍøÑ¹·´À¡ Ë²¨ºó20091026atzy
	            _BA_ENUDOUT = (int16)TRS_NPR_U.d;          				//Íø²àdÖáµçÑ¹Êä³ö20091026atzy
				_BA_ENUQOUT = (int16)TRS_NPR_U.q;          				//Íø²àqÖáµçÑ¹Êä³ö20091026atzy

				_BA_EPIONU  = (int16)(PI_NPR_U.out * 10);					//µ¥»A
				_BA_EPIONID = (int16)(PI_NPR_Id.out* 10);					//µ¥Î»V
				_BA_EPIONIQ	= (int16)(PI_NPR_Iq.out* 10);					//µ¥Î»V

				_BA_EMEXI   = (int16)(RUN_mpridrf_exi * 10);				//µ¥Î»A  »ú²àÀø´ÅÀíÂÛ¼ÆËãÖµÏÔÊ¾ cpc
				_BA_EPIOMID = (int16)(PI_MPR_Id.out* 10);					//µ¥»V
				_BA_EPIOMIQ	= (int16)(PI_MPR_Iq.out* 10);					//µ¥Î»V

//				_BA_ESTAIAC = (int16)(DISP.sta_iac * 10);
//				_BA_ESTAIBA = (int16)(DISP.sta_iba * 10);
//				_BA_ETOQFB  = (int16)DISP.toqfb;				        	//×ª¾Ø·´À¡
//				_BA_EPSTA   = (int16)(DISP.Psactive * 0.01);          		//¶¨×Ó²àÓÐ¹¦¹¦ÂÊÏÔÊ¾kW,DOT1 *10/1000=100
//				_BA_EPNPR   = (int16)(DISP.Pnactive * 0.01);         		//Íø²àÓÐ¹¦¹¦ÂÊÏÔÊ¾
//				_BA_EPGRID  = (int16)(DISP.Pgactive * 0.01);          		//²¢Íø×ÜµÄÓÐ¹¦¹¦ÂÊÏÔÊ¾ zuoyuntest
				_BA_ESTAIAC = (int16)(PRO.STA_iac * 10);				//BJTULVRT201204
				_BA_ESTAIBA = (int16)(PRO.STA_icb * 10);				//BJTULVRT201204
				_BA_ETOQFB  = (int16)Te_feedback;				        	//×ª¾Ø·´À¡//BJTULVRT201204
				_BA_EPSTA   = (int16)(PRO.Psactive * 0.01);          		//¶¨×Ó²àÓÐ¹¦¹¦ÂÊÏÔÊ¾kW,DOT1 *10/1000=100
				_BA_EPNPR   = (int16)(PRO.Pnactive * 0.01);         		//Íø²àÓÐ¹¦¹¦ÂÊÏÔÊ¾BJTULVRT201204
				_BA_EPGRID  = (int16)(PRO.Pgactive * 0.01);          		//²¢Íø×ÜµÄÓÐ¹¦¹¦ÂÊÏÔÊ¾ zuoyuntest

				_BA_TIME1_0=_BA_TIME2_0;									//¹ÊÕÏÊ±¿Ì1
				_BA_TIME1_1=_BA_TIME2_1;
				_BA_TIME1_2=_BA_TIME2_2;
				_BA_TIME1_3=_BA_TIME2_3;
				_BA_TIME1_4=_BA_TIME2_4;
				_BA_TIME1_5=_BA_TIME2_5;
				
				_BA_TIME2_0=_BA_TIME3_0;									//¹ÊÕÏÊ±¿Ì2
				_BA_TIME2_1=_BA_TIME3_1;
				_BA_TIME2_2=_BA_TIME3_2;
				_BA_TIME2_3=_BA_TIME3_3;
				_BA_TIME2_4=_BA_TIME3_4;
				_BA_TIME2_5=_BA_TIME3_5;
					
				_BA_TIME3_0=_BA_TIME4_0;									//¹ÊÕÏÊ±¿Ì3
				_BA_TIME3_1=_BA_TIME4_1;
				_BA_TIME3_2=_BA_TIME4_2;
				_BA_TIME3_3=_BA_TIME4_3;
				_BA_TIME3_4=_BA_TIME4_4;
				_BA_TIME3_5=_BA_TIME4_5;
					
				_BA_TIME4_0=RTIMER.time[0];									//¹ÊÕÏÊ±¿Ì4
				_BA_TIME4_1=RTIMER.time[1];                         		//×îÐÂ¹ÊÕÏÊ±¼ä
				_BA_TIME4_2=RTIMER.time[2];
				_BA_TIME4_3=RTIMER.time[3];
				_BA_TIME4_4=RTIMER.time[4];
				_BA_TIME4_5=RTIMER.time[5];
			}
		}

//NO2------Ô­ÓÐ¹ÊÕÏÏÖÔÚÒ²ÓÐ£¬ÇÒ¼¶±ðÏàÍ¬»òÕßµÍÓÚÖ®Ç°¼¶±ðµÄ¹ÊÕÏ------£¨¹ÊÕÏÆ½¼¶·ÇÁã¡¢½µ¼¶·ÇÁã£©----
		else if((_MSG_SCOUT2!=MSG_NONE)&&(M_ChkFlag(SL_ERROR)!=0)&&((TAB_MSG[_MSG_SCOUT2].rank <= TAB_MSG[_MSG_SCOUT1].rank)))					
		{
			MAIN_LOOP.cnt_rcvr=0;
			_MSG_SCOUT1 = _MSG_SCOUT2;									//±¾´Î¹ÊÕÏÐÅÏ¢×ª´æ

			if((TAB_MSG[_MSG_SCOUT2].attr & D_RCVR)==D_RCVR)	M_SetFlag(SL_DRCVR);	//¸ß¼¶¹ÊÕÏÎÞD_RCVR¶øµÍ¼¶¹ÊÕÏÓÐD_RCVRÇÒÇ°ÕßÊ×ÏÈ·¢ÉúÊ±ÐèÒª
			else												M_ClrFlag(SL_DRCVR);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & OT_SER)==OT_SER)	M_SetFlag(SL_OTSER);	//³¬Ê±ÑÏÖØÊôÐÔ,¸ß¼¶¹ÊÕÏÎÞD_RCVR¶øµÍ¼¶¹ÊÕÏÓÐD_RCVRÇÒÇ°ÕßÊ×ÏÈ·¢ÉúÊ±ÐèÒª
			else												M_ClrFlag(SL_OTSER);

			if(M_ChkFlag(SL_OTSER)!=0)
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_otser,DELAY_OTSER)>=0)	//ÑÓÊ±Ê±¼äµ½
				{
					M_SetFlag(SL_NO_RESET);								//ÖÃ ½ûÖ¹¸´Î» ±êÖ¾	
					M_ClrFlag(SL_OTSER);								//Çå³¬Ê±ÑÏÖØ±êÖ¾
				}	
			}
			else	MAIN_LOOP.cnt_otser=0;		
			
			if(M_ChkFlag(SL_DRCVR)!=0)									//ÑÓÊ±¸´Î»¹ÊÕÏ
			{
				M_SetFlag(SL_DL_RESET);									//Ê×ÏÈÖÃ ½ûÖ¹¸´Î» ±êÖ¾
			}											
		
		}

//NO3---------Ô­À´ÓÐ¹ÊÕÏ£¬±¾´ÎÃ»ÓÐ¹ÊÕÏ----------------------------------------£¨¹ÊÕÏ½µ¼¶ÎªÁã£©------			
		else if((_MSG_SCOUT2==MSG_NONE)&&(M_ChkFlag(SL_ERROR)!=0))					
		{				
			MAIN_LOOP.cnt_otser=0;  								//Çå³¬Ê±ÑÏÖØÑÓÊ±¼Æ
			if(M_ChkFlag(SL_IRCVR)!=0)								//ÔÊÐíÁ¢¼´»Ö¸´¹ÊÕÏ
			{
				M_ClrFlag(SL_IRCVR);								//ÇåÁ¢¼´»Ö¸´±êÖ¾
				M_ClrFlag(SL_ERROR);								//É¾³ý¹ÊÕÏÐÅºÅ
				M_ClrFlag(SL_ERRSTOP);
				M_ClrFlag(SL_SERIESTOP);							//20110829
//-------20130314--------
					M_ClrFlag(SL_OCS_NPRSTART);	          	//ÇåÖ÷¿ØÃüÁî	 
					M_ClrFlag(SL_OCS_MPRSTART);				//ÇåÖ÷¿ØÃüÁî								
					M_ClrFlag(SL_QEPPCO);

					M_ClrFlag(SL_EE_FAIL);					//Çå³ýEEROM¹ÊÕÏ±êÖ¾

					M_ClrFlag(SL_PDPINTA);					//Çå³ýÍø²àTZ1_PDP¹ÊÕÏ±êÖ¾
					M_ClrFlag(SL_PDPINTB);					//Çå³ý»ú²àTZ2_PDP¹ÊÕÏ±êÖ¾
					M_ClrFlag(SL_DISPLAY3);             	//ÃðPDP¹ÊÕÏÖ¸Ê¾µÆ
					ClrPdpint();							//PDPINTÖÐ¶ÏÇå¿Õ	
					EnPdpint();								//PDPINTÖÐ¶ÏÊ¹ÄÜ

					MAIN_LOOP.cnt_rcvr=0;                   //¹ÊÕÏÑÓÊ±»Ö¸´¼ÆÊ±
					MAIN_LOOP.cnt_otser=0;                  //³¬Ê±ÑÏÖØÑÓÊ±	
//----------------------
			}
			
//			if(M_ChkFlag(SL_DL_RESET)!=0)							//ÔÊÐíÑÓÊ±»Ö¸´ÇÒ±¾´ÎÃ»ÓÐ¹ÊÕÏ
			if((M_ChkFlag(SL_DL_RESET)!=0)&&(M_ChkFlag(SL_NO_RESET)==0))		//ÔÊÐíÑÓÊ±»Ö¸´ÇÒ±¾´ÎÃ»ÓÐ¹ÊÕÏ20121023
			{
				PRO.rcvr = _SC_RTRT * 1000;							//µ¥Î»±ä»»:s->ms
				if(M_ChkCounter(MAIN_LOOP.cnt_rcvr,PRO.rcvr)>=0)	//ÑÓÊ±Ê±¼äµ½
				{
					M_ClrFlag(SL_DRCVR);							//ÇåÑÓÊ±»Ö¸´±êÖ¾
					M_ClrFlag(SL_DL_RESET);							//Çå ½ûÖ¹¸´Î» ±ê¾
					M_ClrFlag(SL_ERROR);							//É¾³ý¹ÊÕÏÐÅºÅ20120310
					M_ClrFlag(SL_ERRSTOP);							//20120310
					M_ClrFlag(SL_SERIESTOP);						//20120310
//-------20121206--------
					M_ClrFlag(SL_OCS_NPRSTART);	          	//ÇåÖ÷¿ØÃüÁî	 
					M_ClrFlag(SL_OCS_MPRSTART);				//ÇåÖ÷¿ØÃüÁî								
					M_ClrFlag(SL_QEPPCO);

					M_ClrFlag(SL_EE_FAIL);					//Çå³ýEEROM¹ÊÕÏ±êÖ¾

					M_ClrFlag(SL_PDPINTA);					//Çå³ýÍø²àTZ1_PDP¹ÊÕÏ±êÖ¾
					M_ClrFlag(SL_PDPINTB);					//Çå³ý»ú²àTZ2_PDP¹ÊÕÏ±êÖ¾
					M_ClrFlag(SL_DISPLAY3);             	//ÃðPDP¹ÊÕÏÖ¸Ê¾µÆ
					ClrPdpint();							//PDPINTÖÐ¶ÏÇå¿Õ	
					EnPdpint();								//PDPINTÖÐ¶ÏÊ¹ÄÜ

					MAIN_LOOP.cnt_rcvr=0;                   //¹ÊÕÏÑÓÊ±»Ö¸´¼ÆÊ±
					MAIN_LOOP.cnt_otser=0;                  //³¬Ê±ÑÏÖØÑÓÊ±	
//----------------------
				}
			}
			else   MAIN_LOOP.cnt_rcvr=0;						
		}
//NO4---------------------------------Ô­À´Ã»ÓÐ¹ÊÕÏ±¾´ÎÒ²Ã»ÓÐ¹ÊÕÏ--------------£¨¹ÊÕÏÆ½¼¶ÎªÁã£©------
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
			M_ClrFlag(SL_DISPLAY5);                         //ÇåÏµÍ³¹ÊÕÏÖ¸Ê¾
		}
	}

//-------------------------¹ÊÕÏÍ£»úºó¸´Î»´¦Àí-------------------------------------------------------
	if((M_ChkFlag(SL_ERRSTOP)!=0 || M_ChkFlag(SL_SERIESTOP)!=0) &&(M_ChkFlag(SL_OCS_RESET)!=0)&&(M_ChkFlag(SL_NO_RESET)==0)&&(M_ChkFlag(SL_DL_RESET)==0)&&(_MSG_SCOUT2==0))   //Ã»ÓÐ¹ÊÕÏ²ÅÜ¸´Î»
	{	
			M_ClrFlag(SL_SERIESTOP);				//Çå³ýÑÏÖØÍ£»ú¹ÊÕÏ±êÖ¾
			M_ClrFlag(SL_ERRSTOP);					//Çå³ýÍ£»ú¹ÊÕÏ±êÖ¾
			M_ClrFlag(SL_ERROR);					//Çå³ý¹ÊÕÏ±êÖ¾
			
			M_ClrFlag(SL_OCS_NPRSTART);	          	//ÇåÖ÷¿ØÃüÁî	 
			M_ClrFlag(SL_OCS_MPRSTART);				//ÇåÖ÷¿ØÃüÁî								
			M_ClrFlag(SL_QEPPCO);

			M_ClrFlag(SL_EE_FAIL);					//Çå³ýEEROM¹ÊÕÏ±êÖ¾

			M_ClrFlag(SL_PDPINTA);					//Çå³ýÍø²àTZ1_PDP¹ÊÕÏ±êÖ¾
			M_ClrFlag(SL_PDPINTB);					//Çå³ý»ú²àTZ2_PDP¹ÊÕÏ±êÖ¾
			M_ClrFlag(SL_DISPLAY3);             	//ÃðPDP¹ÊÕÏÖ¸Ê¾µÆ
			ClrPdpint();							//PDPINTÖÐ¶ÏÇå¿Õ	
			EnPdpint();								//PDPINTÖÐ¶ÏÊ¹ÄÜ
	
			_MSG_SCOUT1=0;							//Çå¹ÊÕÏÐÅÏ¢Î»
			MAIN_LOOP.cnt_rcvr=0;                   //¹ÊÕÏÑÓÊ±»Ö¸´¼ÆÊ±
			MAIN_LOOP.cnt_otser=0;                  //³¬Ê±ÑÏÖØÑÓÊ±	
//			_SY_RTRN=0;								//¹ÊÕÏ¼ÆÊýÆ÷ÇåÁã

//			M_ClrFlag(SL_QEPPHE);
//			M_ClrFlag(SL_QEPPCDE);
//			M_ClrFlag(SL_CANOPENOVER);               //CANÍ¨Ñ¶
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
** º¯ýÃû³Æ: et_relay
** ¹¦ÄÜÃèÊö: ¹ýÔØ±£»¤¼ÆËã-NPR
** Êä¡¡Èë: 	 
** Êä:   
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void et_relay_N(void)
{
	int32 et1,et2,ev1,ev2,ev,et;
//Íø²à	
	ev = (int32)PRO.NPR_iac;
	et = ((ev-_SC_IACOV1) * 512) /_SC_IACOV1;

//200us
	if(abs(et)>255)
	{
		if(et>=0)
		{
			et1=et-256;
			if(et1>255) et1=255;   //³¬³ö200%µÄ»°¾ÍËã×÷200%
		
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

	if(ET_SUM1<=0)					//Ð¡ÓÚ¶î¶¨Öµ£¬ÇåÁã
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
//			M_SetFlag(SL_ERRDATASAVE);	//´¥·¢Íâ²¿RAMÊý¾Ý×ªæ20091109atzy
		}
		else M_ClrFlag(SL_SIAC1);
	}
}  
/*********************************************************************************************************
** º¯ÊýÃû³Æ: et_relay
** ¹¦ÜÃÊö: ¹ýÔØ±£»¤¼ÆËã--MPR
** Êä¡¡Èë: 	 
** Êä:   
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void et_relay_M(void)
{
	int32 et1,et2,ev1,ev2,ev,et;
//»ú²à	
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
//			M_SetFlag(SL_ERRDATASAVE);	//´¥·¢Íâ²¿RAMÊý¾Ý×ª´æ20091109atzy
		}
		else M_ClrFlag(SL_SIAC2);
	}
}  

/*********************************************************************************************************
** º¯ÊýÃû: CntCtrl
** ¹¦ÄÜÃèÊö: ¼ÆÊý¿ØÖÆ
** Êä¡¡Èë:
** Êä¡¡³ö       
** ×¢  ÊÍ: systest
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞÈË:
** ÈÕ¡¡ÆÚ:
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
					
	if(MAIN_LOOP.cnt_nprlamp!=65535)	MAIN_LOOP.cnt_nprlamp++;  //Ö¸Ê¾Íø²à±äÁ÷Æ÷¹¤×÷µÆ
	if(MAIN_LOOP.cnt_mprlamp!=65535)	MAIN_LOOP.cnt_mprlamp++;  //Ö¸Ê¾»ú²à±äÁ÷Æ÷¹¤×÷µÆ
				
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

	if(MAIN_LOOP.cnt_isteady0!=65535)	MAIN_LOOP.cnt_isteady0++;     //RunCtrlÍø²àÍ£»ú¸ø¶¨ÑÓÊ±
	if(MAIN_LOOP.cnt_mprsteady0!=65535)	MAIN_LOOP.cnt_mprsteady0++;   //RunCtrl»ú²àÍ£»ú¸ø¶¨ÑÓÊ±

//	if(MAIN_LOOP.cnt_senszfstdy!=60001)	MAIN_LOOP.cnt_senszfstdy++;		//´«¸ÐÆ÷ÁãÆ¯ÂË²¨¼ÆÊýÆ÷
	if(MAIN_LOOP.cnt_speedout!=65535)	MAIN_LOOP.cnt_speedout++;		//20090817
	if(MAIN_LOOP.cnt_uaclv1!=65535)		MAIN_LOOP.cnt_uaclv1++;		//200901027atzy
	if(MAIN_LOOP.cnt_encofault!=65535)	MAIN_LOOP.cnt_encofault++;		//200905atcpc

//sysctl_zl_start
	if(MAIN_LOOP.cnt_cbon!=65535)			MAIN_LOOP.cnt_cbon++;    		//Ö÷¶Ï±ÕºÏ×Ó³ÌÐò	
	if(MAIN_LOOP.cnt_errstop!=65535)	    MAIN_LOOP.cnt_errstop++;    	//Ò»°ã¹ÊÕÏÍ£»ú
	if(MAIN_LOOP.cnt_serstop!=65535)	    MAIN_LOOP.cnt_serstop++;        //ÑÏÖØ¹ÊÕÏÍ£»ú     
	if(MAIN_LOOP.cnt_norstop!=65535)		MAIN_LOOP.cnt_norstop++;        //Õý³£Í£»ú
	if(MAIN_LOOP.cnt_synrun!=65535)      	MAIN_LOOP.cnt_synrun++;         //²¢Íø
	if(MAIN_LOOP.cnt_epwmstop!=65535)      	MAIN_LOOP.cnt_epwmstop++;        //Ò»°ã¹ÊÕÏ·âÂö³åºóÑÓÊ±¶ÏÖ÷¶Ï20110829
	if(MAIN_LOOP.cnt_sysoff!=65535)			MAIN_LOOP.cnt_sysoff++;			//Õý³£Í£»úÑÓ³Ù£¬20120321
//sysctl_zl_end	

//ADctl_zl_start
    if(MAIN_LOOP.cnt_AMUX!=65535)      	    MAIN_LOOP.cnt_AMUX++;           //ÂýËÙAD²ÉÑùÑÓÊ±¼°Ê±
//ADctl_zl_end
    if(MAIN_LOOP.cnt_reset!=65535)      	MAIN_LOOP.cnt_reset++;           //ÉÏÎ»»úI/O¸´Î»£¬ÑÓÊ±2s£¬·ÀÖ¹ÎóÙ×÷
    if(MAIN_LOOP.cnt_clostacmd!=65535)      MAIN_LOOP.cnt_clostacmd++;       //ÏÎ»úÔÊÐíºÏ¶¨×Ó½Ó´¥Æ÷£¬ÑÓÊ±1s£¬·ÀÖ¹Îó²Ù
    if(MAIN_LOOP.cnt_nprcmd!=65535)       	MAIN_LOOP.cnt_nprcmd++;         //ÉÏÎ»»úÔÊÐíNPR·¢Âö³å¬ÑÓÊ±1s£¬·ÀÖ¹Îó²Ù×÷
    if(MAIN_LOOP.cnt_mprcmd!=65535)         MAIN_LOOP.cnt_mprcmd++;         //ÉÏÎ»»úÔÊÐíMPR·¢Âö³å£¬ÑÓÊ±1s£¬·À¹Îó²Ù×÷
	if(MAIN_LOOP.cnt_qcapspdin!=65535)		MAIN_LOOP.cnt_qcapspdin++;   
	if(MAIN_LOOP.cnt_qcapdisturb!=65535)	MAIN_LOOP.cnt_qcapdisturb++;		//QEP¿¹ÉÈÅ   
	if(MAIN_LOOP.cnt_qepcntok!=65535)		MAIN_LOOP.cnt_qepcntok++;   		//QEP¿¹¸ÉÈÅ   
	if(MAIN_LOOP.cnt_qepzdisturb!=65535)	MAIN_LOOP.cnt_qepzdisturb++;   		//QEP¿¹¸ÉÈÅ   
    if(MAIN_LOOP.cnt_ein!=65535)       		MAIN_LOOP.cnt_ein++;        	 //ÉÏ»»ºÏÖ÷¶Ï£¬ÑÓÊ±1s£¬·ÀÖ¹Îó²Ù×÷
    if(MAIN_LOOP.cnt_sysrun!=65535)       	MAIN_LOOP.cnt_sysrun++;        	 //ÉÏÎ»»úËÐÐ/µç»úÊÔÑé£¬ÑÓÊ±1s£¬·ÀÖ¹Îó²Ù×÷
    if(MAIN_LOOP.cnt_prec!=65535)       	MAIN_LOOP.cnt_prec++;         	 //ÉÏÎ»»úÔ¤³äµç£¬ÑÓÊ±1s£¬·ÀÖ¹Îó²Ù×÷
	   
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
    if(MAIN_LOOP.cnt_lv_rcv!=65535)    		MAIN_LOOP.cnt_lv_rcv++;       
    if(MAIN_LOOP.cnt_lv_time!=65535)    	MAIN_LOOP.cnt_lv_time++;       
    if(MAIN_LOOP.cnt_lv_test!=65535)    	MAIN_LOOP.cnt_lv_test++;       
    if(MAIN_LOOP.cnt_scrondelay!=65535)    	MAIN_LOOP.cnt_scrondelay++;       
    if(MAIN_LOOP.cnt_lv_fault1!=65535)    	MAIN_LOOP.cnt_lv_fault1++;       
    if(MAIN_LOOP.cnt_lv_fault2!=65535)    	MAIN_LOOP.cnt_lv_fault2++;       

    if(MAIN_LOOP.cnt_resetrn!=100000000)    MAIN_LOOP.cnt_resetrn++;   
	if(MAIN_LOOP.cnt_phorde!=65535)		    MAIN_LOOP.cnt_phorde++;				//201005atcpc

	if(MAIN_LOOP.cnt_freqrecv!=65535)		MAIN_LOOP.cnt_freqrecv++;			//201011PLL
//	if(MAIN_LOOP.cnt_lv_gving!=65535)		MAIN_LOOP.cnt_lv_gving++;			//201011PLL
	if(MAIN_LOOP.cnt_lv_unbalance!=65535)	MAIN_LOOP.cnt_lv_unbalance++;		//201011LVRT

//---------20121103atsp---------------
//---------BJTULVRT201204
	if(MAIN_LOOP.cnt_lv_qworking!=65535)	MAIN_LOOP.cnt_lv_qworking++;		
	if(MAIN_LOOP.cnt_lv_phicontrol!=65535)	MAIN_LOOP.cnt_lv_phicontrol++;		
	if(MAIN_LOOP.cnt_lv_lowpi!=65535)		MAIN_LOOP.cnt_lv_lowpi++;	
	if(MAIN_LOOP.cnt_lv_mpridvar!=65535)	MAIN_LOOP.cnt_lv_mpridvar++;	
	if(MAIN_LOOP.cnt_idcov!=65535)			MAIN_LOOP.cnt_idcov++;	
	if(MAIN_LOOP.cnt_idcovdelay!=65535)		MAIN_LOOP.cnt_idcovdelay++;	
	if(MAIN_LOOP.cnt_lv_utlv!=65535)		MAIN_LOOP.cnt_lv_utlv++;	
	if(MAIN_LOOP.cnt_lv_test2!=65535)		MAIN_LOOP.cnt_lv_test2++;	
//--------20130225--------
	if(MAIN_LOOP.cnt_lv_irqdelay!=65535)	MAIN_LOOP.cnt_lv_irqdelay++;	//20121210
	if(MAIN_LOOP.cnt_vdscron!=65535)		MAIN_LOOP.cnt_vdscron++;	//20121210
	if(MAIN_LOOP.cnt_vdscroff!=65535)		MAIN_LOOP.cnt_vdscroff++;	//20121210
//	if(MAIN_LOOP.cnt_lv_scroff1!=65535)		MAIN_LOOP.cnt_lv_scroff1++;	//20121210
	if(MAIN_LOOP.cnt_lv_scroff2!=65535)		MAIN_LOOP.cnt_lv_scroff2++;	//20121210
	if(MAIN_LOOP.cnt_lv_scroff3!=65535)		MAIN_LOOP.cnt_lv_scroff3++;	//20130221
	if(MAIN_LOOP.cnt_lv_unblcscroff1!=65535)		MAIN_LOOP.cnt_lv_unblcscroff1++;	//20121210
	if(MAIN_LOOP.cnt_lv_unblcscroff2!=65535)		MAIN_LOOP.cnt_lv_unblcscroff2++;	//20121210

	if(MAIN_LOOP.cnt_lv_unblcscroff3!=65535)	MAIN_LOOP.cnt_lv_unblcscroff3++;	//20130124
	if(MAIN_LOOP.cnt_lv_npriqrf!=65535)			MAIN_LOOP.cnt_lv_npriqrf++;		//20130124
	if(MAIN_LOOP.cnt_lv_mpridrf!=65535)			MAIN_LOOP.cnt_lv_mpridrf++;		//20130130
	if(MAIN_LOOP.cnt_lv_nprqworking!=65535)		MAIN_LOOP.cnt_lv_nprqworking++;	//20130124
	if(MAIN_LOOP.cnt_lv_kq!=65535)				MAIN_LOOP.cnt_lv_kq++;			//20130124
	if(MAIN_LOOP.cnt_lv_recmstop!=65535)		MAIN_LOOP.cnt_lv_recmstop++;	//20130124
	if(MAIN_LOOP.cnt_lv_dlrecmstop!=65535)		MAIN_LOOP.cnt_lv_dlrecmstop++;	//20130130
//	if(MAIN_LOOP.cnt_lv_xnzkkz!=65535)			MAIN_LOOP.cnt_lv_xnzkkz++;		//20130202
	if(MAIN_LOOP.cnt_jgvdscron!=65535)			MAIN_LOOP.cnt_jgvdscron++;		//20130205
	if(MAIN_LOOP.cnt_lv_reciq!=65535)			MAIN_LOOP.cnt_lv_reciq++;		//20130205
	if(MAIN_LOOP.cnt_lv_holdpow!=65535)			MAIN_LOOP.cnt_lv_holdpow++;		//20130205
	if(MAIN_LOOP.cnt_lv_scrkeepon!=65535)		MAIN_LOOP.cnt_lv_scrkeepon++;		//20130221
//--------20130801--------
	if(M_ChkFlag(SL_ECPOSSEARCH)!=0 && M_ChkFlag(SL_MPR_START)!=0)
	{
		if(MAIN_LOOP.cnt_synfail!=200000)	MAIN_LOOP.cnt_synfail++;   	//20130525
	}
	if(MAIN_LOOP.cnt_ecposcheck!=65535)		MAIN_LOOP.cnt_ecposcheck++;			//20130525
	if(MAIN_LOOP.cnt_ecpostrace!=65535)		MAIN_LOOP.cnt_ecpostrace++;			//20130525

}

/*********************************************************************************************************
** º¯ÊýÃû³Æ: Display
** ÄÜÃèÊö: ÏÔÊ¾ÖµÆËã
** Êä¡¡Èë: 	 
** Êä³ö:   
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Display(void)
{
	float temp1,temp2,temp3;

//------------------------ÖÐ¼äÖ±Á÷µçÑ¹¼°Ö¸ÁîÖµÏÔÊ¾Öµ¼ÆËã-------------------------------------------
	DISP.udc  = PRO.udc;							//µ¥Î»V
	DISP.urf  = RUN.urf;							//µ¥Î»V												//µ¥Î»V
	DISP.mpridrf = RUN.mpridrf;							//µ¥Î»A
	DISP.npriqrf = RUN.npriqrf;							//µ¥Î»A
	DISP.mpriqrf = RUN.mpriqrf;							//µ¥Î»A
	DISP.toqrf   = RUN.toqrf;							//µ¥Î»NM
	DISP.aglrf   = RUN.aglrf;							//µ¥Î» ½Ç¶È
    DISP.toqfb   = Te_feedback;    
//---------------------------ÏßµçÑ¹ÓÐÐ§ÖµÏÔÊ¾Öµ¼ÆËã------------------------------------------------
	DISP.npr_uab = PRO.NPR_uab;						//µ¥Î»V
	DISP.npr_ubc = PRO.NPR_ubc;						//µ¥Î»V
	DISP.sta_uab = PRO.STA_uab;						//µ¥Î»V
	DISP.sta_ubc = PRO.STA_ubc;						//µ¥Î»V

//---------------------------Ö÷¶ÏÇ°Íø²àÏßµçÑ¹ÓÐÐ§Öµ¼ÆËã-ÏÔÊ¾---------------------------
//	DISP.grd_uab = MEAN_DATA.uab * 1.110721;        //µ¥Î»V  rms=mean*PAI/(2*sqrt(2)) £» 1.110721=PAI * SQRT2 / 4
//	DISP.grd_ubc = MEAN_DATA.ubc * 1.110721;        //µ¥Î»V  rms=mean*PAI/(2*sqrt(2)) £» 1.110721=PAI * SQRT2 / 4
	DISP.grd_uab = MEAN_DATA.uab1 * 1.110721;        //µ¥Î»V  rms=mean*PAI/(2*sqrt(2)) £» 1.110721=PAI * SQRT2 / 4
	DISP.grd_ubc = MEAN_DATA.ubc1 * 1.110721;        //µ¥Î»V  rms=mean*PAI/(2*sqrt(2)) £» 1.110721=PAI * SQRT2 / 4

//---------------------------Íø²àºÍ»ú²àµç¸ÐÎÂ¶ÈÏÔÊ¾----------------------------
    DISP.Lac_temp      = MEAN_DATA.Lac_temp;                  //Íø²àµç¸ÐÎÂ¶È
	DISP.Ldudt_temp    = MEAN_DATA.Ldudt_temp;                //»ú²àµç¸ÐÎÂ¶È 
	DISP.NPR_skiptemp  = AMUX.NPR_skiiptemp;              //Íø²àSKIIPÎÂ¶È
	DISP.MPR_skiptemp  = AMUX.MPR_skiiptemp;              //»ú²àSKIIPÎÂ


//---------------------------Ë²Ê±µçÁ÷ÏÔÊ¾Öµ¼ÆËã----------------------------------------------------
	DISP.npr_iai = AD_OUT_NPR_I.a;					//µ¥Î»A
	DISP.npr_ibi = AD_OUT_NPR_I.b;					//µ¥Î»A
	DISP.npr_ici = AD_OUT_NPR_I.c;					//µ¥Î»A
	DISP.mpr_iai = AD_OUT_MPR_I.a;					//µ¥Î»A
	DISP.mpr_ibi = AD_OUT_MPR_I.b;					//µ¥Î»A
	DISP.mpr_ici = AD_OUT_MPR_I.c;					//µ¥Î»A

//------------------------Íø²à²¢ÍøµçÁ÷ÓÐÐ§ÖµÏÔÊ¾Öµ¼ÆËã----------------------------------------------
	DISP.npr_iar = PRO.npr_iar;							//µ¥Î»A
	DISP.npr_ibr = PRO.npr_ibr;							//µ¥Î»A
	DISP.npr_icr = PRO.npr_icr;							//µ¥Î»A

//------------------------»ú²àµçÁ÷ÓÐÐ§ÖµÏÔÊ¾Öµ¼ÆËã--------------------------------------------------
    temp1 = MEAN_DATA.ia2 * 1.110721;
	temp2 = MEAN_DATA.ib2 * 1.110721;
	temp3 = MEAN_DATA.ic2 * 1.110721;
	DISP.mpr_iar = temp1;							//µ¥Î»A
	DISP.mpr_ibr = temp2;							//µ¥Î»A
	DISP.mpr_icr = temp3;							//µ¥Î»A

//------------------------¶¨×ÓÏßµçÁ÷ÓÐÐ§ÖµÏÔÊ¾Öµ¼ÆËã--------------------------------------------------
    temp1 = MEAN_DATA.iac3 * 1.110721;
	temp2 = MEAN_DATA.icb3 * 1.110721;
	DISP.sta_iac = temp1;							//µ¥Î»A
	DISP.sta_iba = temp2;							//µ¥Î»A

//------------------------------¶¨×Ó²àºÍÍø²àºÍ×Ü²¢ÍøÓÐ¹¦¹¦ÂÊÏÔÊ¾-----------------------------------------------------
	DISP.Psactive   = PRO.Psactive;
	DISP.Psreactive = PRO.Psreactive;
	DISP.Ps		    = PRO.Ps;

	DISP.Pnactive   = PRO.Pnactive;
	DISP.Pnreactive = PRO.Pnreactive;
    DISP.Pn         = PRO.Pn;

	DISP.Pgactive   = PRO.Pgactive;
	DISP.Pgreactive = PRO.Pgreactive;
	DISP.Pg      	= PRO.Pg;

//------------------------------ÆµÂÊºÍ×ªËÙÏÔÊ¾-----------------------------------------------------	
	if(M_ChkFlag(SL_IN1_CBSTS)==0) 	DISP.freq=0;            //20090815
	else 							DISP.freq = CAP4.freqtmp;

	DISP.speed= PRO.speedflt;		//20090815

//------------------------------¶¨×ÓÍ¬²½²¢ÍøÇ°¶¨×ÓÇ°ºóµçÑ¹²îÖµÏÔÊ¾-----------------------------------------------------
	DISP.uab23 = MEAN_DATA.uab_d;
	DISP.ubc23 = MEAN_DATA.ubc_d;

//------------------------------PI»·Êä³öÏÔÊ¾-------------------------------------------------------
	DISP.pionu  = PI_NPR_U.out;						//µ¥Î»A
	DISP.pionid = PI_NPR_Id.out;					//µ¥Î»V
	DISP.pioniq	= PI_NPR_Iq.out;					//µ¥Î»V
	DISP.mexi  = RUN_mpridrf_exi;					//µ¥Î»A  »ú²àÀø´ÅÀíÂÛ¼ÆËãÖµÏÔÊ¾
	DISP.piomid = PI_MPR_Id.out;				    //µ¥Î»V
	DISP.piomiq	= PI_MPR_Iq.out;					//µ¥Î»V
} 
/*********************************************************************************************************
** º¯ÊýÃû³Æ: Bank()
** ¹¦ÄÜÃèÊö: ÏÔÊ¾¿ØÖÆ
** Êä¡¡Èë: 	 
** Êä³ö:   
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Bank(void)
{
	
//------------------------------------Íø²à¹¤×÷Ö¸Ê¾µÆÏÔÊ¾--------------------------------------------
	if(M_ChkFlag(SL_NPR_PWMOUT)!=0)
	{
		M_SetFlag(SL_DISPLAY0);
/*		if(M_ChkCounter(MAIN_LOOP.cnt_nprlamp,DELAY_NPRRUN)>=0)	//0µÆ¿ìËÙÉÁ¸,Ö¸Ê¾Íø²àÕýÔÚ·¢Âö³åÔËÐÐ
		{
			M_NotFlag(SL_DISPLAY0);
			MAIN_LOOP.cnt_nprlamp=0;
		}
*/	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_nprlamp,DELAY_NPRSTDBY)>=0)	//0µÆÂýËÙÁË¸
		{
		   M_NotFlag(SL_DISPLAY0);
		   MAIN_LOOP.cnt_nprlamp=0;
		} 
	}
			
//------------------------------------»ú²à¹¤×÷¸Ê¾µÆÏÔÊ¾--------------------------------------------
	if(M_ChkFlag(SL_MPR_PWMOUT)!=0)
	{
		M_SetFlag(SL_DISPLAY1);
/*		if(M_ChkCounter(MAIN_LOOP.cnt_mprlamp,DELAY_MPRRUN)>=0)	//1µÆ¿ìËÙÉÁ¸,Ö¸Ê¾»ú²àÕýÔÚ·¢Âö³åÔËÐÐ
		{
			M_NotFlag(SL_DISPLAY1);
			MAIN_LOOP.cnt_mprlamp=0;
		}
*/	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_mprlamp,DELAY_MPRSTDBY)>=0)	//1µÆÂýËÙÉÁË¸
		{
			M_NotFlag(SL_DISPLAY1);
			MAIN_LOOP.cnt_mprlamp=0;
		}
	}


	if(M_ChkFlag(SL_STEADYFB)!=0)	M_SetFlag(SL_DISPLAY2);			//VdcÎÈ¶¨Ö¸Ê¾
	else							M_ClrFlag(SL_DISPLAY2);

	if(M_ChkFlag(SL_UDSTACOK)!=0 && M_ChkFlag(SL_MPR_SYNOK)!=0)	M_SetFlag(SL_DISPLAY4);			//¶¨×Ó´ïµ½²¢ÍøÌõ¼þÖ¸Ê¾
	else if(M_ChkFlag(SL_UDSTACOK)!=0)							M_NotFlag(SL_DISPLAY4);
	else														M_ClrFlag(SL_DISPLAY4);

	if(M_ChkFlag(SL_CBCLOSED)!=0 && M_ChkFlag(SL_OCS_SYSRUN)==0 && M_ChkFlag(SL_SERIESTOPING)==0 && M_ChkFlag(SL_SERIESTOP)==0 && M_ChkFlag(SL_ERRSTOPING)==0 && M_ChkFlag(SL_ERRSTOP)==0 && M_ChkFlag(SL_OCS_EIN)!=0)	
			M_SetFlag(SL_DISPLAY7);									//Ö÷¶ÏÒÑÕºÏ,Ô¤³äµç´ý»úÖ¸Ê¾
	else	M_ClrFlag(SL_DISPLAY7);

//--------20130801--------
	if((M_ChkFlag(SL_POWERON)==0) && (M_ChkFlag(SL_MPR_START)==0))
		QEPDATA.encodpos =  - _encodpos;

//----------------------------------------ÔËÐÐ¼à¿Ø--------------------------------------------------
	_BA_URF   = (int16)DISP.urf;				//ÖÐ¼äµçÑ¹¸ø¶¨Öµ
	_BA_UDC   = (int16)DISP.udc;				//ÖÐ¼äÖ±÷µçÑ¹
	_BA_MIDRF  = (int16)(DISP.mpridrf * 10);		//dÖáµçÁ÷Ö¸Áî
	_BA_NIQRF  = (int16)(DISP.npriqrf * 10);		//qÖáµçÁ÷Ö¸Áî
	_BA_MIQRF  = (int16)(DISP.mpriqrf * 10);		//qÖáµçÁ÷Ö¸Áî
	_BA_TOQRF  = (int16)(DISP.toqrf);				//×ª¾ØÖ¸Áî
    _BA_AGLRF  = (int16)(DISP.aglrf);             //ÎÞ¹¦½Ç¶È¸Áî 20091027atzy

	_BA_IA1   = (int16)(DISP.npr_iar * 10);		//Íø²à,aÏà²¢ÍøµçÁ÷£¬¸ÄÎª1Î»¡Êý
	_BA_IB1   = (int16)(DISP.npr_ibr * 10);		//Íø²à,bÏà²¢ÍøµçÁ÷
	_BA_IC1   = (int16)(DISP.npr_icr * 10);		//ø²à,cÏà²¢ÍøµçÁ÷
	_BA_IA2   = (int16)(DISP.mpr_iar * 10);		//µç»ú²à,aàµçÁ
	_BA_IB2   = (int16)(DISP.mpr_ibr * 10);		//µç»ú²à,bÏàµçÁ÷
	_BA_IC2   = (int16)(DISP.mpr_icr * 10);		//µç»ú²à,cÏàµçÁ÷

	_BA_UAB0   = (int16)DISP.grd_uab;			//Ö÷¶ÏÇ°Íø²àabÏßÑ¹
	_BA_UBC0   = (int16)DISP.grd_ubc;			//Ö÷¶ÏÇ°Íø²àbcÏßÑ¹				
	_BA_UAB1  = (int16)DISP.npr_uab;			//Íø²àabÏßÑ¹
	_BA_UBC1  = (int16)DISP.npr_ubc;			//Íø²àbcÏßÑ¹
	_BA_UAB2  = (int16)DISP.sta_uab;			//¶¨×Ó²àabÏßÑ¹,¶¨×Ó²à 20091027atzy
	_BA_UBC2  = (int16)DISP.sta_ubc;			//¶¨×Ó²àbcÏßÑ¹,¶¨×Ó²à 20091027atzy
	_BA_GRDUD = (int16)TRS_NGS_U.dflt;          //dÖáÖ÷¶ÏºóÍøÑ¹·´À¡ ÂË²¨ºó
	_BA_GRDUQ = (int16)TRS_NGS_U.qflt;          //qÖáÖ÷¶ÏºóÍøÑ¹·´À¡ ÂË²¨ºó
	_BA_NPRID = (int16)TRS_NPR_I.dflt;          //dÖáÍø²àµçÁ÷·´À¡ ÂË²¨ºó
	_BA_NPRIQ = (int16)TRS_NPR_I.qflt;          //qÖáÍø²àµçÁ÷·´À¡ ÂË²¨ºó
//	_BA_EXCID = (int16)(TRS_MPR_I.d * 10);             //dÖá»úàµçÁ÷·´À¡ ÂË²¨Ç°
//	_BA_EXCIQ = (int16)(TRS_MPR_I.q * 10);             //qÖá»ú²àµçÁ÷·´À¡ ÂË²¨Ç°
    _BA_SCRIA = (int16)(AD_OUT_SCR_I.a * 10);             //SCRµçÁ÷a	20110906
    _BA_SCRIB = (int16)(AD_OUT_SCR_I.b * 10);             //SCRµçÁ÷b	20110906
	_BA_STAUD = (int16)TRS_MPR_U.d;
	_BA_STAUQ = (int16)TRS_MPR_U.q;

	_BA_TLAC  = (int16)DISP.Lac_temp;		    //Íø²àµç¸ÐÎÂ¶È
	_BA_TLDUDT= (int16)DISP.Ldudt_temp;	        //»ú²àµç¸ÐÎÂ¶È
	_BA_TNSKIIP= (int16)DISP.NPR_skiptemp;      //Íø²àSKIIPÎÂ¶È ÉãÊÏ¶È
	_BA_TMSKIIP= (int16)DISP.MPR_skiptemp;      //»ú²àSKIIPÎÂ¶È ÉãÊÏ¶È

	_BA_FREQ  = (int16)(DISP.freq * 10);		//µçÍøÆµÂÊ
	_BA_SPEED = (int16)DISP.speed;				//µç»ú×ªËÙ 

	_BA_PIONU  = (int16)(DISP.pionu  * 10);			    //NPRµçÑ¹»·Êä³ö 6.23change_zl¸ÄÎª1Î»Ð¡ý
	_BA_PIONID = (int16)(DISP.pionid * 100);			//NPRµçÁ÷»·dÊä³ö
	_BA_PIONIQ = (int16)(DISP.pioniq * 100);			//NPRµçÁ÷»·qÊä³ö
	_BA_MEXI   = (int16)(DISP.mexi  * 10);			    // 6.23change_zl¸ÄÎª1Î»Ð¡Êý ¸ÄÎªÀø´ÅµçÁ÷ÀíÂÛÖµÏÔÊ¾cpc
	_BA_PIOMID = (int16)(DISP.piomid * 100);			//MPRµçÁ÷»·dÊä³ö
	_BA_PIOMIQ = (int16)(DISP.piomiq * 100);			//MPRµçÁ÷»·qÊä³ö

	_BA_STAUABD = (int16)(DISP.uab23 * 10);             //¶¨×ÓÍ¬²½²¢ÍøÇ°¶¨×ÓÇ°ºóµçÑ¹²î 20091027atzy
	_BA_STAUBCD = (int16)(DISP.ubc23 * 10);				//¶¨×ÓÍ¬²½²¢ÍøÇ°¶¨×ÓÇ°ºóµçÑ¹²î 20091027atzy
	_BA_STAIAC = (int16)(DISP.sta_iac * 10);            //¶¨×ÓÏßµçÁ÷ÓÐÐ§ÖµÏÔÊ¾
	_BA_STAIBA = (int16)(DISP.sta_iba * 10);			//¶¨×ÓÏßµçÁ÷ÓÐÐ§ÖµÏÔÊ¾

	_BA_TOQFB  = (int16)DISP.toqfb;				        //×ª¾Ø·´À¡

	_BA_PSTA  = (int16)(DISP.Psactive * 0.01);          //¶¨×Ó²àÓÐ¹¦¹¦ÂÊÏÔÊ¾kW,DOT1 *10/1000=100
	_BA_PNPR  = (int16)(DISP.Pnactive * 0.01);          //Íø²àÓÐ¹¦¹¦ÂÊÏÔÊ¾
	_BA_PGRID = (int16)(DISP.Pgactive * 0.01);          //²¢Íø×ÜµÄÓÐ¹¦¹¦ÂÊÏÔÊ¾

	_BA_QSTA  = (int16)(DISP.Psreactive * 0.01);        //¶¨×Ó²àÎÞ¹¦¹¦ÂÊÏÔÊ¾kVAR,DOT1 *10/1000=100
	_BA_QNPR  = (int16)(DISP.Pnreactive * 0.01);        //Íø²àÎÞ¹¦¹¦ÂÊÏÔÊ¾
	_BA_QGRID = (int16)(DISP.Pgreactive * 0.01);        //²¢Íø×ÜµÄÎÞ¹¦¹¦ÂÊÏÔÊ¾

	_BA_SSTA  = (int16)(DISP.Ps * 0.01);              	//¶¨×Ó²àÊÓÔÚ¹¦ÂÊÏÔÊ¾kVA,DOT1 *10/1000=100
	_BA_SNPR  = (int16)(DISP.Pn * 0.01);              	//Íø²àÊÓÔÚ¹¦ÂÊÏÔÊ¾
	_BA_SGRID = (int16)(DISP.Pg * 0.01);              	//²¢Íø×ÜµÄÊÓÔÚ¹¦ÂÊÏÔÊ¾

//Í¨¹ýCANOPENÍ¨Ñ¶·´À¡¸øÖ÷¿ØµÄ¼à¿Ø±äÁ¿	

//	DISP.toqfb = 8000;  //cantest
//    DISP.speed = 1800;  //cantest
//	AMUX.skiiptempmax = 65; //cantest

	SCI_canopen.tx_watertempin  = 0;
	SCI_canopen.tx_watertempout = 0;
	SCI_canopen.tx_demand = 0;

}


/*********************************************************************************************************
** º¯ÊýÃû³Æ: ERROR_Datasave
** ¹¦  ÄÜ: ½«Êý¾ÝÐ´ÈëÍâ²¿RAM
** Êä¡¡Èë:
** Êä¡¡³ö:        
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 20100203atbjtu PCmodbus
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void ERROR_Datasave(void)
{
//--------------------------------External RAM Data Save-----------------------------------------
/*
	if(M_ChkFlag(SL_PC_ERRDATACLR)!=0)						//PCÖ¸ÁîÍâ²¿RAMÊý¾ÝÊÍ·Å
	{
		M_ClrFlag(SL_RAMERRORSAVE);						
		*(ERROR_RAMEND + 1) = 0x0000;
		M_ClrFlag(SL_TRIG_ERRDSAVE);
		M_ClrFlag(SL_PC_ERRDATACLR);						//ÊÍ·ÅÊý¾Ý/¸´Î»Íê³É,×Ô¶¯Çå³ý¸´Î»Ö¸Áî
		M_ClrFlag(SL_PC_ERRDATASAVE);						//ÊÍ·ÅÊý¾Ý/¸´Î»Íê³É,×Ô¶¯Çå³ýÊÖ¶¯´¥·¢´æ´¢Ö¸Áî 201005atcpc
		M_ClrFlag(SL_DISPLAY6);								//201007BJTULVRT
	} 

  	if(*(ERROR_RAMEND + 1) != 0x55AA)
  	{
		if(M_ChkFlag(SL_TRIG_ERRDSAVE)==0)  								//Ã»ÓÐ¹ÊÕÏ	
			MAIN_LOOP.cnt_datasave=0;
		else if(M_ChkCounter(MAIN_LOOP.cnt_datasave,DELAY_DATASAVE)>=0)	//¹ÊÕÏ·¢ÉúºóÑÓÊ±0.5s½ûÖ¹»­Í¼
		{
			M_SetFlag(SL_RAMERRORSAVE);
			M_ClrFlag(SL_TRIG_ERRDSAVE);
			*(ERROR_RAMEND + 1) = 0x55AA;			
			*(ERROR_RAMEND + 2) = RTIMER.time[0];				//×îÐÂ¹ÊÕÏÊ±¼äsystest 201005atcpc
			*(ERROR_RAMEND + 3) = RTIMER.time[1];
			*(ERROR_RAMEND + 4) = RTIMER.time[2];
			*(ERROR_RAMEND + 5) = RTIMER.time[3];
			*(ERROR_RAMEND + 6) = RTIMER.time[4];
			*(ERROR_RAMEND + 7) = RTIMER.time[5];				
			*(ERROR_RAMEND + 8) = _MSG_SCOUT2;				

			_BA_TIME5_0 = RTIMER.time[0];						//×îÐÂ¹ÊÕÏÊ±¼äsystest 201005atcpc
			_BA_TIME5_1 = RTIMER.time[1];
			_BA_TIME5_2 = RTIMER.time[2];
			_BA_TIME5_3 = RTIMER.time[3];
			_BA_TIME5_4 = RTIMER.time[4];
			_BA_TIME5_5 = RTIMER.time[5];				
			_BA_ERR5	= _MSG_SCOUT2;				
		}

		if(M_ChkFlag(SL_RAMERRORSAVE)==0)  										//ÓÐ¹ÊÕÏÖ®ºóÑÓÊ±2sºóÍ£Ö¹Êý¾Ý´æ´¢
		{
			*(ERROR_RAMSTART+ ERROR_RAMDATA_POS) = (int16)AD_OUT_UDC;										//0=ÖÐ¼äÖ±Á÷µçÑ¹
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 1 + ERROR_RAMDATA_POS)) = (int16)(CAP4.nprtrstheta*1000);	//1=NPR¶¨Ïò½Ç¶È
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 2 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NPR_I.a*10);		//2=NPR-AµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 3 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NPR_I.b*10);		//3=NPR-BµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 4 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NPR_I.c*10);		//4=NPR-CµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 5 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.ab*10);		//5=µçÍøµçÑ¹Vab
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 6 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.bc*10);		//6=µçÍøµçÑ¹Vbc
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 7 + ERROR_RAMDATA_POS)) = (int16)(TRS_NGS_U.d*10);			//7=øÑ¹dÖá·ÖÁ¿edÂË²¨Ç°
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 8 + ERROR_RAMDATA_POS)) = (int16)(TRS_NGS_U.q*10);			//8=ÍøÑ¹qÖá·ÖÁ¿eqÂË²¨Ç°
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 9 + ERROR_RAMDATA_POS)) = (int16)(TRS_NGS_U.dflt*10);		//9=ÍøÑ¹dÖá·ÖÁ¿edÂË²¨ºó
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 10 + ERROR_RAMDATA_POS)) = (int16)(TRS_NGS_U.qflt*10);		//10=ÍøÑ¹qÖá·ÖÁ¿eqÂË²¨ºó
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 11 + ERROR_RAMDATA_POS)) = (int16)(PI_NPR_Id.reference*10);//11=Íø²àIdÖ¸Áî
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 12 + ERROR_RAMDATA_POS)) = (int16)(TRS_NPR_I.dflt*10);		//12=Íø²àId·´¡
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 13 + ERROR_RAMDATA_POS)) = (int16)(PI_NPR_Id.out*10);		//13=Íø²àIdPIÊä³ö
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 14 + ERROR_RAMDATA_POS)) = (int16)(TRS_NPR_U.d*10);		//14=Íø²àUdÊä³ö
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 15 + ERROR_RAMDATA_POS)) = (int16)(PI_NPR_Iq.reference*10);//15=Íø²àIqÖ¸Áî
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 16 + ERROR_RAMDATA_POS)) = (int16)(PI_NPR_Iq.feedback*10);	//16=Íø²àIq·´À¡
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 17 + ERROR_RAMDATA_POS)) = (int16)(PI_NPR_Iq.out*10);		//17=Íø²àIqPIÊä³ö
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 18 + ERROR_RAMDATA_POS)) = (int16)(TRS_NPR_U.q*10);		//18=Íø²àUqÊä³ö
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 19 + ERROR_RAMDATA_POS)) = (int16)(CAP4.mprtrstheta*1000);	//19=MPR¶¨Ïò½Ç¶È
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 20 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_MPR_I.a*10);		//20=MPR-AµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 21 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_MPR_I.b*10);		//21=MPR-BµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 22 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_MPR_I.c*10);		//22=MPR-CµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 23 + ERROR_RAMDATA_POS)) = (int16)(CAP4.freqtmp*10);		//23=Êµ¼ÊÊµÊ±ÍøÆµ
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 24 + ERROR_RAMDATA_POS)) = (int16)PRO.speed;				//24=µç»ú×ªËÙ

			*(ERROR_RAMEND) = ERROR_RAMDATA_POS;															//±Ç°Êý¾Ý´æ´¢Î»ÖÃ×ª´æ

			ERROR_RAMDATA_POS++;
			if(ERROR_RAMDATA_POS >= RAM_BIAS)  ERROR_RAMDATA_POS=0;		
		}
  	}
*/
	//20121103
//--------------------------------External RAM Data Save-----------------------------------------

	if(M_ChkFlag(SL_PC_ERRDATACLR)!=0)						//PCÖ¸ÁîÍâ²¿RAMÊý¾ÝÊÍ·Å
	{
		M_ClrFlag(SL_RAMERRORSAVE);						
		*(ERROR_RAMEND + 1) = 0x0000;
		M_ClrFlag(SL_TRIG_ERRDSAVE);
		M_ClrFlag(SL_PC_ERRDATACLR);						//ÊÍ·ÅÊý¾Ý/¸´Î»Íê³É,×Ô¶¯Çå³ý¸´Î»Ö¸Áî
		M_ClrFlag(SL_PC_ERRDATASAVE);						//ÊÍ·ÅÊý¾Ý/¸´Î»Íê³É,×Ô¶¯Çå³ýÊÖ¶¯´¥·¢´æ´¢Ö¸Áî 201005atcpc
		M_ClrFlag(SL_DISPLAY6);								//201007BJTULVRT
		*(BANK_RAMEND + 1) = 0x0000;
		M_ClrFlag(SL_RAMBANKSAVE);							//
	} 

  	if(*(ERROR_RAMEND + 1) != 0x55AA)
  	{
		if(M_ChkFlag(SL_TRIG_ERRDSAVE)==0)  								//Ã»ÓÐ¹ÊÕÏ	
			MAIN_LOOP.cnt_datasave=0;
		else if(M_ChkCounter(MAIN_LOOP.cnt_datasave,DELAY_DATASAVE)>=0)	//¹ÊÕÏ·¢ÉúºóÑÓÊ±1.8s½ûÖ¹»­Í¼201205LVRTatZB
		{
			M_SetFlag(SL_RAMERRORSAVE);
//			M_ClrFlag(SL_TRIG_ERRDSAVE);
			*(ERROR_RAMEND + 1) = 0x55AA;			
			*(ERROR_RAMEND + 2) = RTIMER.time[0];				//×îÐÂ¹ÊÕÏÊ±¼äsystest 201005atcpc
			*(ERROR_RAMEND + 3) = RTIMER.time[1];
			*(ERROR_RAMEND + 4) = RTIMER.time[2];
			*(ERROR_RAMEND + 5) = RTIMER.time[3];
			*(ERROR_RAMEND + 6) = RTIMER.time[4];
			*(ERROR_RAMEND + 7) = RTIMER.time[5];				
			*(ERROR_RAMEND + 8) = _MSG_SCOUT2;				

			_BA_TIME5_0 = RTIMER.time[0];						//×îÐÂ¹ÊÕÏÊ±¼äsystest 201005atcpc
			_BA_TIME5_1 = RTIMER.time[1];
			_BA_TIME5_2 = RTIMER.time[2];
			_BA_TIME5_3 = RTIMER.time[3];
			_BA_TIME5_4 = RTIMER.time[4];
			_BA_TIME5_5 = RTIMER.time[5];				
			_BA_ERR5	= _MSG_SCOUT2;				
		}

		if(M_ChkFlag(SL_RAMERRORSAVE)==0)  										//ÓÐ¹ÊÕÏÖ®ºóÑÓÊ±2sºóÍ£Ö¹Êý¾Ý´æ´¢
		{
//----------LVRT--201205LVRTatZB
			*(ERROR_RAMSTART+ ERROR_RAMDATA_POS) = (int16)AD_OUT_UDC;										//0=ÖÐ¼äÖ±Á÷µçÑ¹
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 1 + ERROR_RAMDATA_POS)) = (int16)(CAP4.nprtrstheta*1000);	//1=NPR¶¨Ïò½Ç¶È
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 2 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NPR_I.a*10);		//2=NPR-AµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 3 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NPR_I.b*10);		//3=NPR-BµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 4 + ERROR_RAMDATA_POS)) = (int16)(GRD_Ut*10);				//4=**********µçÍø²ÐÑ¹**********
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 5 + ERROR_RAMDATA_POS)) = (int16)(NGS_Udq*10);				//5=*****µçÍøµçÑ¹******
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 6 + ERROR_RAMDATA_POS)) = (int16)(NGS_Udq_p*10);			//6=********µçÍøµçÑ¹½öº¬ÓÐÕýÐòPLL*******
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 7 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_SCR_I.a);		//7=SCRµçÁ÷BJTULVRT201204
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 8 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_SCR_I.b);		//8=SCRµçÁ÷BJTULVRT201204
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 9 + ERROR_RAMDATA_POS)) = (int16)(GIVE.toqrf);			//9=**********Ö÷¿Ø¸ø¶¨×ª¾Ø*Ê¾²¨Æ÷ÏÔÊ¾ËõÐ¡ÁË10±¶*************
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 10 + ERROR_RAMDATA_POS)) = (int16)(GIVE.toqrfrun);		//10=*******Ö÷¿Ø¸ø¶¨×ª¾Ø´óÂË²¨*Ê¾²¨Æ÷ÏÔÊ¾ËõÐ¡ÁË10±¶*********
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 11 + ERROR_RAMDATA_POS)) = (int16)(PI_NPR_Id.reference*10);//11=Íø²àIdÖ¸Áî
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 12 + ERROR_RAMDATA_POS)) = (int16)(TRS_NPR_I.dflt*10);		//12=Íø²àId·´À¡
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 13 + ERROR_RAMDATA_POS)) = (int16)(RUN.toqrf);			//13=*******ÔËÐÐ¸ø¶¨×ª¾Ø*Ê¾²¨Æ÷ÏÔÊ¾ËõÐ¡ÁË10±¶*********
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 14 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_DC_I.b*10);		//14=ÖÐ¼äÖ±Á÷Õ¶²¨µçÁ÷BJTULVRT201204
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 15 + ERROR_RAMDATA_POS)) = (int16)(RUN.mpridrf*10);		//15=»ú²àÀø´ÅµçÁ÷Ö¸Áî
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 16 + ERROR_RAMDATA_POS)) = (int16)(TRS_MPR_I.dflt*10);		//16=»ú²àÀø´ÅµçÁ÷·´À¡
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 17 + ERROR_RAMDATA_POS)) = (int16)(RUN.mpriqrf*10);		//17=»ú²à×ª¾ØµçÁ÷Ö¸Áî
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 18 + ERROR_RAMDATA_POS)) = (int16)(TRS_MPR_I.qflt*10);		//18=»ú²à×ª¾ØµçÁ÷·´À¡BJTULVRT201204
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 19 + ERROR_RAMDATA_POS)) = (int16)(NGS_Udq_epsilon* 10);	//19=MPR¶¨Ïò½Ç¶È
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 20 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_MPR_I.a*10);		//20=MPR-AµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 21 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_MPR_I.b*10);		//21=MPR-BµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 22 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_MPR_I.c*10);		//22=MPR-CµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 23 + ERROR_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_STATE) * 10);			//23=**********ÑÓÊ±»Ö¸´µÄµçÍø²ÐÑ¹**********
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 24 + ERROR_RAMDATA_POS)) = (int16)PRO.speed; 				//24=µç»ú×ªËÙ

			*(ERROR_RAMEND) = ERROR_RAMDATA_POS;															//±Ç°Êý¾Ý´æ´¢Î»ÖÃ×ª´æ

			ERROR_RAMDATA_POS++;
			if(ERROR_RAMDATA_POS >= RAM_BIAS)  ERROR_RAMDATA_POS=0;		
		}
  	}

//--------------------------------External RAM Data Read-----------------------------------------
} 

/*********************************************************************************************************
** º¯ÊýÃû³Æ: BANK_Datasave
** ¹¦  ÄÜ: ½«Êý¾ÝÐ´ÈëÍâ²¿RAM
** Êä¡¡Èë:
** Êä¡¡³ö:        
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 20100203atbjtu PCmodbus
**-------------------------------------------------------------------------------------------------------
** ÐÞÄË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void BANK_Datasave(void)
{
//--------------------------------External RAM Data Save-----------------------------------------
/*
		if(M_ChkFlag(SL_PC_BANKDATASAVE)==0)  																//PCÖ¸ÁîÎ´ÒªÇóÊý¾ÝËø´æ£¬´¦ÓÚË¢ÐÂ×´Ì¬
		{
			*(BANK_RAMSTART+ BANK_RAMDATA_POS) = (int16)(CAP4.nprtrstheta* 1000);							//0=Íø²à¶¨Ïò½Ç¶È
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 1 + BANK_RAMDATA_POS)) = (int16)(CAP4.mprtrstheta* 1000);	//1=»ú²à¶¨Ïò½Ç¶È
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 2 + BANK_RAMDATA_POS)) = (int16)(QEPDATA.rotposdisp* 1000);	//2=×ª×Ó½ÇÈ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 3 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NGF_U.ab* 10);		//3=µçÍøÂË²¨ABÏßµçÑ¹
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 4 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NGF_U.bc* 10);		//4=µçÍøÂË²¨BCÏßµçÑ¹
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 5 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_U.ab* 10);		//5=¶¨×ÓÂË²¨ABÏßµçÑ¹
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 6 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_U.bc* 10);		//6=¶¨×ÓÂË²¨BCÏßµçÑ¹
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 7 + BANK_RAMDATA_POS)) = (int16)AD_OUT_UDC;					//7=ÖÐ¼äÖ±Á÷µçÑ¹
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 8 + BANK_RAMDATA_POS)) = (int16)(TRS_NGS_U.d* 10);			//8=µçÍødÖáµçÑ¹·ÖÁ¿ÂË²¨Ç°
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 9 + BANK_RAMDATA_POS)) = (int16)(TRS_NGS_U.q* 10);			//9=µçÍøqÖáµçÑ¹·ÖÁ¿ÂË²¨Ç°
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 10 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NPR_I.a* 10);		//10=Íø²àAÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 11 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NPR_I.b* 10);		//11=Íø²àBÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NPR_I.c* 10);		//12=Íø²àCÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 13 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_MPR_I.a* 10);		//13=»ú²àAÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 14 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_MPR_I.b* 10);		//14=»ú²àBÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 15 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_MPR_I.c* 10);		//15=»ú²àCÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 16 + BANK_RAMDATA_POS)) = (int16)(CAP4.freqtmp* 10);			//16=µçÍøÊµÊ±ÆµÂÊ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 17 + BANK_RAMDATA_POS)) = (int16)PRO.speed;					//17=µç»ú×ªËÙ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 18 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.reference* 10);	//18=Íø²àµçÑ¹»·Êä³ö
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.out* 10);		//19=Íø²àdáµçÁ÷»·Êä³ö
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.out* 10);		//20=Íø²àqÖáµçÁ÷»·Êä³ö
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(DM_imrd* 10);				//21=»ú²à¹ãÒåÀø´ÅµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(RUN.mpridrf* 10);				//22=»ú²àÀø´ÅµçÁ÷Ö¸Áî
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(PI_MPR_Id.out* 10);		//23=»ú²àdÖáµçÁ÷»·Êä³ö
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(PI_MPR_Iq.out* 10);		//24=»ú²àqÖáµçÁ÷»·Êä³ö

			*(BANK_RAMEND) = BANK_RAMDATA_POS;																//µ±Ç°Êý¾Ý´æ´¢Î»ÖÃ×ª´æ

			BANK_RAMDATA_POS++;
			if(BANK_RAMDATA_POS >= RAM_BIAS)  BANK_RAMDATA_POS=0;	
			
			M_ClrFlag(SL_RAMBANKSAVE);												//ÏÔÊ¾²¨ÐÎÊý¾Ý´¦ÓÚË¢ÐÂ±êÖ¾
		}
		else	M_SetFlag(SL_RAMBANKSAVE);											//ÏÔÊ¾²¨ÐÎÊý¾Ý´¦ÓÚËø´æ±êÖ¾
*/
	//20121103
  	if(*(BANK_RAMEND + 1) != 0x55AA)
  	{
		if(M_ChkFlag(SL_TRIG_ERRDSAVE)==0)  								//Ã»ÓÐ¹ÊÕÏ	
			MAIN_LOOP.cnt_datasave2=0;
		else if(M_ChkCounter(MAIN_LOOP.cnt_datasave2,DELAY_DATASAVE)>=0)	//¹ÊÕÏ·¢ÉúºóÑÓÊ±1.8s½ûÖ¹»­Í¼201205LVRTatZB
		{
			M_SetFlag(SL_RAMBANKSAVE);
			*(BANK_RAMEND + 1) = 0x55AA;			
		}

		if(M_ChkFlag(SL_RAMBANKSAVE)==0)  																//PCÖ¸ÁîÎ´ÒªÇóÊý¾ÝËø´æ£¬´¦ÓÚË¢ÐÂ×´Ì¬
		{
			*(BANK_RAMSTART+ BANK_RAMDATA_POS) = (int16)(DIP_STA_I.qflt);							//0=Íø²à¶¨Ïò½Ç¶È
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 1 + BANK_RAMDATA_POS)) = (int16)(CAP4.mprtrstheta*1000);	//1=»ú²à¶¨ÏòÇ¶È
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 2 + BANK_RAMDATA_POS)) = (int16)(GRD_Utlv*10);		//2=×ª×Ó½ÇÈ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 3 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_STATE) * 10);		//3=µçÍøÂË²¨ABÏßµçÑ¹
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 4 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_QWORKING)* 10);		//4=µçÍøÂË²¨BCÏßçÑ¹
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 5 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_SCRRUNING)* 10);		//5=¶¨×ÓÂË²¨ABÏßµçÑ¹
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 6 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_SCRIZERO)* 10);		//6=¶¨×ÓÂË²¨BCÏßµçÑ¹
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 7 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_SYNCON)* 10);					//7=ÖÐ¼äÖ±Á÷µçÑ¹
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 8 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_UNBALANCE)* 10);			//8=µçÍødÖáµçÑ¹·ÖÁ¿ÂË²¨Ç°
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 9 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_MSTOP)* 10);			//9=µçÍøqÖáµçÑ¹·ÖÁ¿ÂË²¨Ç°
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 10 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_NSTOP)* 10);		//10=Íø²àAÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 11 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_I.ac);		//11=Íø²àBÏàµçÁ÷
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_PHICON)* 10);		//11=Íø²àBÏàµçÁ÷
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(GIVE.npriqrf_cl);		//12=Íø²àCÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(NGS_Udq_pflt* 10);		//12=Íø²àCÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 13 + BANK_RAMDATA_POS)) = (int16)(RUN.npriqrf* 10);		//13=»ú²àAÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 14 + BANK_RAMDATA_POS)) = (int16)(TRS_NPR_I.qflt* 10);		//14=»ú²àBÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 15 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NPR_I.c* 10);		//15=»ú²àCàµÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 16 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_STRICTLV) * 10);			//16=µçÍøÊµÊ±ÆµÂÊ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 17 + BANK_RAMDATA_POS)) = (int16)(kq * 100);					//17=µç»ú×ªËÙ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 18 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_I.cb);	//18=Íø²àµçÑ¹»·Êä³ö
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_SCRKEEPON)* 10);		//19=¶¨×ÓçÁ÷AÏàBJTULVRT201204
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_SCRDONE)* 10);		//19=¶¨×ÓçÁ÷AÏàBJTULVRT20130221
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_SCRKEEPON) * 10);		//20=¶¨×ÓµçÁ÷CÏà
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(TRS_NGS_U.d * 10);		//20=¶¨×ÓµçÁ÷CÏà
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(PHI_DATA_M.PHId * 10);		//21=ÖÐ¼äÖ±Á÷Õ¶²¨çÁ÷BJTULVRT201204
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_VDSCRON) * 10);		//21=20121210
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(PHI_DATA_M.PHIq* 10);			//22=»ú²àÀø´ÅµçÁ÷Ö¸Áî
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_IMUNBALANCE) * 10);		//21=20130116
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(PRO.Pgactive2 * 0.01);		//21=20130116
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(PHI_DATA_M.Id);		//23=SCRÖ§Â·µçÁ÷ BJTULVRT201204
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(TRS_NPR_U.d * 10);		//23=SCRÖ§Â·µçÁ÷ BJTULVRT201204
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(PHI_DATA_M.Iq);		//24=SCRÖ§Â·µçÁ÷ BJTULVRT201204
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(TRS_NPR_U.q * 10);		//24=SCRÖ§Â·µçÁ÷ BJTULVRT201204

			*(BANK_RAMEND) = BANK_RAMDATA_POS;																//±ÇÊý¾Ý´æ¢Î»ÖÃ×ª´æ

			BANK_RAMDATA_POS++;
			if(BANK_RAMDATA_POS >= RAM_BIAS)  BANK_RAMDATA_POS=0;	
			
		}
	}


//--------------------------------External RAM Data Read-----------------------------------------
}  

/*********************************************************************************************************
** º¯ÊýÃû³Æ: BANK_Datasave
** ¹¦  ÄÜ: ½«Êý¾ÝÐ´ÈëÍâ²¿RAM
** Êä¡¡Èë:
** Êä¡¡³ö:        
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 20100203atbjtu 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
/*void BANK_Datasave(void)	//testPCmodbus
{
//--------------------------------External RAM Data Save-----------------------------------------

		if(M_ChkFlag(SL_PC_BANKDATASAVE)==0)  																//PCÖ¸ÁîÎ´ÒªÇóÊý¾ÝËø´æ£¬´¦ÓÚË¢ÐÂ×´Ì¬
		{
			*(BANK_RAMSTART+ BANK_RAMDATA_POS) = (int16)(testtheta* 1000);									//0=Íø²à¶¨Ïò½Ç¶È
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 1 + BANK_RAMDATA_POS)) = (int16)(testtheta_120* 1000);		//1=»ú²à¶¨Ïò½Ç¶È
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 2 + BANK_RAMDATA_POS)) = (int16)(testtheta_sta* 1000);		//2=ª×Ó½Ç¶È
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 3 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.ab* 10);		//3=µçÍøÂË²¨ABÏßµçÑ¹
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 4 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.bc* 10);		//4=µçÍøÂË²¨BCÏßµçÑ¹
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 5 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_U.ab* 10);		//5=¶¨×ÓÂË²¨ABÏßçÑ¹
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 6 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_U.bc* 10);		//6=¶¨×ÓÂË²¨BCÏßµçÑ¹
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 7 + BANK_RAMDATA_POS)) = 1100;								//7=ÖÐ¼äÖ±Á÷µçÑ¹
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 8 + BANK_RAMDATA_POS)) = (int16)(testtheta+400);			//8=µçÍødÖáµçÑ¹·ÖÁ¿ÂË²¨Ç°
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 9 + BANK_RAMDATA_POS)) = (int16)(testtheta+450);			//9=µçÍøqÖáµçÑ¹·ÖÁ¿ÂË²¨Ç°
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 10 + BANK_RAMDATA_POS)) = (int16)(testtheta+500);			//10=Íø²àAÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 11 + BANK_RAMDATA_POS)) = (int16)(testtheta+550);			//11=Íø²àBàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(testtheta+600);			//12=Íø²àCÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 13 + BANK_RAMDATA_POS)) = (int16)(testtheta+650);			//13=»ú²àAÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 14 + BANK_RAMDATA_POS)) = (int16)(testtheta+700);			//14=»ú²àBÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 15 + BANK_RAMDATA_POS)) = (int16)(testtheta+750);			//15=»ú²àCÏàµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 16 + BANK_RAMDATA_POS)) = (int16)(testtheta+800);			//16=µçÍøÊµÊ±ÆµÂÊ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 17 + BANK_RAMDATA_POS)) = (int16)(testtheta+850);			//17=µç»ú×ªËÙ
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 18 + BANK_RAMDATA_POS)) = (int16)(testtheta+900);			//18=Íø²àµçÑ¹»·Êä³ö
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(testtheta+950);			//19=Íø²àdÖáµçÁ÷»·Êä³ö
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(testtheta+1000);			//20=Íø²àqÖáµçÁ÷»·Êä³ö
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(testtheta+1050);			//21=»ú²à¹ãÒåÀø´ÅµçÁ÷
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(testtheta+1100);			//22=»ú²àÀø´ÅµçÁ÷Ö¸Áî
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(testtheta+1150);			//23=»ú²àdÖáµçÁ÷»·Êä³ö
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(testtheta+1200);			//24=»ú²àqÖáµçÁ÷»·Êä³ö

			*(BANK_RAMEND) = BANK_RAMDATA_POS;																//µ±Ç°Êý¾Ý´æ´¢Î»ÖÃ×ª´æ

			BANK_RAMDATA_POS++;
			if(BANK_RAMDATA_POS >= RAM_BIAS)  BANK_RAMDATA_POS=0;		
			
			M_ClrFlag(SL_RAMBANKSAVE);												//ÏÔÊ¾²¨ÐÎÊý¾Ý´¦ÓÚË¢ÐÂ±êÖ¾
		}
		else	M_SetFlag(SL_RAMBANKSAVE);											//ÏÔÊ¾²¨ÐÎÊý¾Ý´¦ÓÚËø´æ±êÖ¾

//--------------------------------External RAM Data Read-----------------------------------------
}  

void ERROR_Datasave(void)	//testPCmodbus
{
//--------------------------------External RAM Data Save-----------------------------------------
	if(M_ChkFlag(SL_PC_ERRDATACLR)!=0)						//PCÖ¸ÁîÍâ²¿RAMÊý¾ÝÊÍ·Å
	{
		M_ClrFlag(SL_RAMERRORSAVE);						
		*(ERROR_RAMEND + 1) = 0x0000;
		M_ClrFlag(SL_TRIG_ERRDSAVE);
		M_ClrFlag(SL_PC_ERRDATACLR);						//ÊÍ·ÅÊý¾Ý/¸´Î»Íê³É,×Ô¶¯Çå³ý¸´Î»Ö¸Áî
	} 

  	if(*(ERROR_RAMEND + 1) != 0x55AA)
  	{
		if(M_ChkFlag(SL_TRIG_ERRDSAVE)==0)  								//Ã»ÓÐ¹ÊÕÏ	
			MAIN_LOOP.cnt_datasave=0;
		else if(M_ChkCounter(MAIN_LOOP.cnt_datasave,DELAY_DATASAVE)>=0)	//¹ÊÕÏ·¢ÉúºóÑÓÊ±0.5s½ûÖ¹»­Í¼
		{
			M_SetFlag(SL_RAMERRORSAVE);
			M_ClrFlag(SL_TRIG_ERRDSAVE);
			*(ERROR_RAMEND + 1) = 0x55AA;			
		}

		if(M_ChkFlag(SL_RAMERRORSAVE)==0)  										//ÓÐ¹ÊÕÏÖ®ºóÑÓÊ±2sºóÍ£Ö¹Êý¾Ý´æ´¢
		{
			*(ERROR_RAMSTART+ ERROR_RAMDATA_POS) = 1100;														//0=test
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 1 + ERROR_RAMDATA_POS)) = (int16)(testtheta* 1000);			//1=NPR¶¨Ïò½Ç¶È
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 2 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.ab* 10);		//2=NPR-AµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 3 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.bc* 10);		//3=NPR-BµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 4 + ERROR_RAMDATA_POS)) = (int16)(testtheta+400);				//4=NPR-Cµç
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 5 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.ab* 10);		//5=µçÍøµçÑ¹Vab
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 6 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.bc* 10);		//6=µçÍøµçÑ¹Vbc
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 7 + ERROR_RAMDATA_POS)) = (int16)(testtheta+700);				//7=ÍøÑ¹dÖá·ÖÁ¿edÂË²¨Ç°
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 8 + ERROR_RAMDATA_POS)) = (int16)(testtheta+800);				//8=ÍøÑ¹qÖá·ÖÁ¿eqÂË²¨Ç°
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 9 + ERROR_RAMDATA_POS)) = (int16)(testtheta+900);				//9=ÍøÑ¹dÖá·ÖÁ¿edÂË²¨ºó
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 10 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1000);			//10=ÍøÑ¹qÖá·ÖÁ¿eqÂË²¨ºó
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 11 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1100);			//11=Íø²àIdÖ¸Áî
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 12 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1200);			//12=Íø²àId·´À¡
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 13 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1300);			//13=Íø²àIdPIÊä³ö
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 14 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1400);			//14=Íø²àUdÊä³ö
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 15 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1500);			//15=Íø²àIqÖ¸Áî
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 16 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1600);			//16=Íø²àIq·´À¡
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 17 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1700);			//17=Íø²àIqPIÊä³ö
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 18 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1800);			//18=Íø²àUqÊä³ö
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 19 + ERROR_RAMDATA_POS)) = (int16)(testtheta_sta* 1000);		//19=MPR¶¨Ïò½Ç¶È
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 20 + ERROR_RAMDATA_POS)) = (int16)(testtheta+2000);			//20=MPR-AµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 21 + ERROR_RAMDATA_POS)) = (int16)(testtheta+2100);			//21=MPR-BµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 22 + ERROR_RAMDATA_POS)) = (int16)(testtheta+2200);			//22=MPR-CµçÁ÷
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 23 + ERROR_RAMDATA_POS)) = (int16)(testtheta+2300);			//23=Êµ¼ÊÊµÊ±ÍøÆµ
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 24 + ERROR_RAMDATA_POS)) = (int16)(testtheta+2400);			//24=µç»ú×ªËÙ

			*(ERROR_RAMEND) = ERROR_RAMDATA_POS;																//µ±Ç°Êý¾Ý´æ´¢Î»ÖÃ×ª´æ

			ERROR_RAMDATA_POS++;
			if(ERROR_RAMDATA_POS >= RAM_BIAS)  ERROR_RAMDATA_POS=0;		
		}
  	}

//--------------------------------External RAM Data Read-----------------------------------------
}  

*/
/*********************************************************************************************************
** º¯ÊýÃû³Æ: Draw
** ¹¦ÜÃèÊö: »æÆÍ¼ÐÎ
** Êä¡¡Èë:
** Êä¡¡³ö:        
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄË:
** ÈÕ¡¡ÆÚ:
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
