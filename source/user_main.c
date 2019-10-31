/****************************************Copyright (c)**************************************************
**                       		     ±±	¾©	½»	Í¨	´ó	Ñ§
**                                         µçÆø¹¤³ÌÑ§Ôº
**                                         614ÊµÑéÊÒ
**
**                              
**
**--------------ÎÄ¼þÐÅÏ¢--------------------------------------------------------------------------------
**ÎÄ   ¼þ   Ãû: user_main.c
**´´   ½¨   ÈË: 
**×îºóÐÞ¸ÄÈÕÆÚ: 
**Ãè        Êö: 1.5MWË«À¡±äÁ÷Æ÷¿ØÖÆÈí¼þÖ÷³ÌÐò----×óÔÆ·ç³¡
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
#include "IQmathLib.h"
#include "qmath.h"
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
/*--------------------×Ô¶¨ÒåÍ·ÎÄ¼þ----------------------------*/
#include "user_header.h"  						//±äÁ¿³£Á¿¶¨Òå
#include "user_macro.h"							//ºêº¯Êý
#include "user_database.h"						//Êý¾Ý¿â   
#include "user_interface.c"						//½Ó¿Ú²ã
#include "user_work.c"							//¹¤×÷¿ØÖÆ
#include "math.h"

/*-----------------------ÖÐ¶ÏÉùÃ÷-----------------------------*/
interrupt void CpuTimer0Isr(void);				//Ö÷¶¨Ê±Æ÷ÖÐ¶Ï			
interrupt void EPWM1_TZ1_Isr(void);			//TZ1ÖÐ¶Ï
interrupt void EPWM2_TZ2_Isr(void);			//TZ2ÖÐ¶Ï
/*-----------------------º¯ÊýÉùÃ÷-----------------------------*/

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
	PieVectTable.TINT0       = &CpuTimer0Isr;		  //¶¨Ê±Æ÷T0ÖÜÆÚÖÐ¶Ï
	PieVectTable.EPWM1_TZINT = &EPWM1_TZ1_Isr;      //TZ1 ¹¦ÂÊ±£»¤ÖÐ¶Ï 
	PieVectTable.EPWM2_TZINT = &EPWM2_TZ2_Isr;      //TZ2 ¹¦ÂÊ±£»¤ÖÐ¶Ï 

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
		Sci_canopenrx();					//ÏµÍ³ÊäÈë¶ÁÈ¡


//-----------------------------Ò»¼¶Ñ­»·5msÔËÐÐÒ»´Î----------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt1,DELAY_MAIN1)>=0)		
		{
			MAIN_LOOP.cnt1=0;				//Çå¿Õ¼ÆÊýÆ÷
			Input();						//ÏµÍ³ÊäÈë¶ÁÈ¡
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
		}
//-----------------------------Áù¼¶Ñ­»·1000msÔËÐÐÒ»´Î---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt6,DELAY_MAIN6)>=0)
		{
			MAIN_LOOP.cnt6=0;
//			M_NotFlag(SL_DISPLAY4);

			if(M_ChkFlag(SL_CODEOK)!=0)		
				RtRead();							//ÔÚeepromÕý³£µÄÇé¿öÏÂ//¶ÁÈ¡ÊµÊ±Ê±ÖÓ,ºÜºÄÊ±£¬Òª13ms.20090801,CPC				
										
			if(M_ChkCounter(MAIN_LOOP.cnt_senszfstdy,DELAY_SENSZFSTDY)>=0)
				M_SetFlag(SL_SENSZFSTDY);   		//ÑÓ³ÙÊ±¼äµ½ºóÖÃÁãÆ¯ÂË²¨ÎÈ¶¨±êÖ¾Î»


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
    Uint16 i;
    
    M_SetFlag(SL_PHASEA);            //²âÁ¿CPUÕ¼ÓÐÂÊ,²âÁ¿DSP°åÉÏT1¶Ë×Ó
    *OUT3_ADDR = _OUT3_DATA;
		
	if(M_ChkFlag(SL_IN1_CBSTS)==0)   //ÔÚºÏÖ÷¶ÏÇ°ÍøÑ¹ÏàÐò¼ì²â×Óº¯Êý cap5-ubc   cap6-uab
	{
	  PhaseOrderChk();        
	} 
 	QepEncodPos(); 					


    CapQepCtrl();  		  			//²âÍøÑ¹ÏàÎ»£¬ÍøÑ¹ÆµÂÊ£¬×ø±êÕý¡¢·´±ä»»µÄ½Ç¶È
   	Ad8364Ctrl();             		//ÓÐ´«¸ÐÆ÷ºÍ»¥¸ÐÆ÷øÐAD²ÉÑùºÍ×ª»»

    NPR_CONTROL();            		//Íø²à±äÁ÷Æ÷¿ØÖÆËã·¨
    MPR_CONTROL();            		//»ú²à±äÁ÷Æ÷¿ØÖÆËã·¨

	et_relay_N();					//200usÖ´ÐÐÒ»´Î
	et_relay_M();					//200usÖ´ÐÐÒ»´Î

	testtheta = testtheta + 1;		//hoteltest20091109
	if(testtheta>=3721)	testtheta= -1879;//hoteltest20091109

//------------------¼àÊÓÍøÑ¹µøÂä20091027atzy-----------------------------------
  if(M_ChkFlag(SL_CHARGEOK)!=0)
  {
//70%
	if((TRS_NGS_U.dflt2<=394) && (_BA_UAB2<65535) && (M_ChkFlag(SL_GRIDLOSE70)==0))			//ed=563V; *0.7=394V
	{
		_BA_UAB2 = _BA_UAB2+1;
		M_SetFlag(SL_DISPLAY6);		//µÆÖ¸Ê¾
	}
	else if(_BA_UAB2!=0)	M_SetFlag(SL_GRIDLOSE70);

//50%
	if((TRS_NGS_U.dflt2<=282) && (_BA_UBC2<65535) && (M_ChkFlag(SL_GRIDLOSE50)==0))			//ed=563V; *0.5=282V
	{
		_BA_UBC2 = _BA_UBC2+1;
		M_SetFlag(SL_DISPLAY7);		//µÆÖ¸Ê¾
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
			break;
		}
		case 1:
		{	
			PwmDrive();	 		   //Êä³öÂö³å¿ØÖÆ  				
		    CntCtrl(); 				//¼ÆÊý1ms¼ÆÒ»¸öÊý£¬¶¨Ê±Æ÷ÖÐ¶ÏÖÜÆÚÎª200us
			Datasave();				//0.4msÒ»´Î20091109
			break;
		}
		case 2:
		{
			Protect();			   //protect calculation		
			Scout();			   //¹ÊÕÏ±£»¤
			break;
		}
		case 3:
		{	
			PwmDrive();			   //Êä³öÂö³å¿ØÖÆ				
			RunCtrl();			   //¸ø¶¨»ý·Ö
			Datasave();				//0.4msÒ»´Î20091109
			break;
		}
		case 4:
		{	
			Protect();			   //protect calculation	
			Scout();			   //¹ÊÕÏ±£»¤
			break;
		}
		case 5:
		{	
			PwmDrive();			   //Êä³öÂö³å¿ØÖÆ				
			Display();
			Datasave();				//0.4msÒ»´Î20091109
			break;
		}
		case 6:
		{	
			CntCtrl(); 	           //¼ÆÊý£¬1ms¼ÆÒ»¸öÊý£¬¶¨Ê±Æ÷ÖÐ¶ÏÖÜÆÚÎª200us
			Protect();			   //protect calculation	
			Scout();			   //¹ÊÕÏ±£»¤
			break;
		}
		case 7:
		{
			PwmDrive();			   //Êä³öÂö³å¿ØÖÆ				
			Datasave();				//0.4msÒ»´Î20091109
			break;
		}
		case 8:
		{	
			Protect();												//slow AD, protect calculation	
			Scout();												//¹ÊÕÏ±£»¤
			break;
		}
		case 9:
		{	
			PwmDrive();			   //Êä³öÂö³å¿ØÖÆ				
			RunCtrl();			   //¸ø¶¨»ý·Ö
			Datasave();				//0.4msÒ»´Î20091109
			break;
		}
//--------------------------------------------------------------------------------------------------
		default:
			break;
	}

//--------------------DA×ª»»------------------------------------------------------------------------
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
	zys1 = (int16)(TRS_NGS_U.dflt2     * 2.0) + 2048;  //edÐ¡ÂË²¨£¬¼à²âÍøÑ¹µøÂä20091027atzy
	zys2 = (int16)(AD_OUT_STA_U.bc	   * 2.0) + 2048;   //
	zys3 = (int16)(AD_OUT_STAD_U.bc    * 20.0)+ 2048;   //
	zys4 = (int16)(CAP4.mprtrstheta    * 200.0) + 2048;    //
//--------ÏÔÊ¾ÂË²¨ºó×ªËÙ-------
//    zys1 = (int16)(PRO.speedflt     * 1.0) + 2048;  //×ªËÙ

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


//--Ó¦´ðÖÐ¶Ï20091109atzy
//	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	

//canopen
//-----------------------------canopenÔËÐÐÒ»´Î----------------------------
	if(M_ChkCounter(MAIN_LOOP.canopen_tx,DELAY_CANOPENTX)>=0)    //13ms		
	{
		MAIN_LOOP.canopen_tx=0;				//Çå¿Õ¼ÆÊýÆ÷
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
		if(ScibRegs.SCIFFTX.bit.TXFFST == 0)						//·¢ËÍÍê³É?
		{
			if(ScibRegs.SCICTL2.bit.TXEMPTY==1)	
			{
				ScibRegs.SCIFFTX.bit.TXFIFOXRESET=1;	// Re-enable TX FIFO operation
			}
		}
	}

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
	_BA_EPGRID++;				//20100506ÁÙÊ±ÐÞ¸Ä ¼ÇÂ¼Ó²¹ÊÕÏ´ÎÊý			
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
** º¯ÊýÃû³Æ: Protect
** ¹¦ÄÜÃèÊö: ±£»¤Öµ¼ÆËã
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
void Protect(void)
{
	float temp;
	
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

//---------------------------Íø²àÏßµçÑ¹ÓÐÐ§Öµ¼ÆËã----------------------------
    PRO.NPR_uab= MEAN_DATA.uab1 * 1.110721;                      //Íø²àUabÓÐÐ§Öµµ¥Î»V rms=mean*PAI/(2*sqrt(2)) 
	PRO.NPR_ubc= MEAN_DATA.ubc1 * 1.110721;                      //Íø²àUbcÓÐÐ§Öµµ¥Î»V 1.110721=PAI * SQRT2 / 4
	PRO.GID_uab = MEAN_DATA.uab * 1.110721;        //20091026atzy µçÍø²àUabÓÐÐ§Öµµ¥Î»V µ¥Î»V  rms=mean*PAI/(2*sqrt(2)) £» 1.110721=PAI * SQRT2 / 4
    PRO.GID_ubc = MEAN_DATA.ubc * 1.110721;        //20091026atzy µçÍø²àUabÓÐÐ§Öµµ¥Î»V µ¥Î»V  rms=mean*PAI/(2*sqrt(2)) £» 1.110721=PAI * SQRT2 / 4

//---------------------------¶¨×Ó²àÏßµçÑ¹ÓÐÐ§Öµ¼ÆËã----------------------------
    PRO.STA_uab= MEAN_DATA.uab2 * 1.110721;                    //µç²àUabÓÐÐ§Öµµ¥Î»V
	PRO.STA_ubc= MEAN_DATA.ubc2 * 1.110721;                    //µç»ú²àUbcÓÐÐ§Öµµ¥Î»V

//------------------¹¦ÂÊ±£»¤¼°ÏÔÊ¾Öµ¼ÆËã-----------------------------------------
//------------------¶¨×Ó²àdq±ä»»¶¨Ïà½Ç¶ÈÖµ--------------------------------------------------------------------
	DIP_STA_I.sintheta = sin(CAP4.nprtrstheta);		//¼ÆËã¶¨×Ó²à¹¦ÂÊ dq±ä»»½Ç¶È
	DIP_STA_I.costheta = cos(CAP4.nprtrstheta);		//¼ÆËã¶¨×Ó²à¹¦ÂÊ dq±ä»»½Ç¶È
	DIP_STA_U.sintheta = DIP_STA_I.sintheta;
	DIP_STA_U.costheta = DIP_STA_I.costheta; 

//------------------¶¨×Ó²àµçÁ÷dqÖµ--------------------------------------------------------------------
   	DIP_STA_I.a     = AD_OUT_STA_I.a;
   	DIP_STA_I.b     = AD_OUT_STA_I.b;
   	DIP_STA_I.c     = AD_OUT_STA_I.c;
	Transform_3s_2s_2r(&DIP_STA_I);
	DataFilter(0.99,&DIP_STA_I.dflt,DIP_STA_I.d); 	//¶¨×Ó²àµçÁ÷·´À¡ÖµÂË²¨,Ts=200us,fh=88Hz,ÂËµô¿ª¹ØÆµÂÊ´Î
	DataFilter(0.99,&DIP_STA_I.qflt,DIP_STA_I.q); 	//¶¨×Ó²àµçÁ÷·´À¡ÖµÂË²¨£¬ Ts=200us,fh=88Hz,ÂËµô¿ª¹ØÆµÂÊ´Î

//------------------¶¨×Ó²àµçÑ¹dqÖµ--------------------------------------------------------------------
   	DIP_STA_U.a     = AD_OUT_NGS_U.a;    //²ÉÓÃÍøÑ¹×÷Îª¶¨×ÓµçÑ¹£¬ÒòÎª¶¨×ÓV-LEM·´À¡µçÑ¹¾­¹ýÁË´ó³£ÊýÂË²¨
   	DIP_STA_U.b     = AD_OUT_NGS_U.b;
   	DIP_STA_U.c     = AD_OUT_NGS_U.c;
	Transform_3s_2s_2r(&DIP_STA_U);
	DataFilter(0.99,&DIP_STA_U.dflt,DIP_STA_U.d); 	//¶¨×Ó²àµçÑ¹·´À¡ÖµÂË²¨£¬ Ts=200us,fh=88Hz,ÂËµô¿ª¹ØÆµÂÊ´Î
	DataFilter(0.99,&DIP_STA_U.qflt,DIP_STA_U.q); 	//¶¨×Ó²àµçÑ¹·´À¡ÖµÂË²¨£¬ Ts=200us,fh=88Hz,ÂËµô¿ª¹ØÆµÂÊ´Î

//------------------------¶¨×ÓÏàµçÑ¹ÓÐÐ§ÖµÏÔÊ¾Öµ¼ÆËã(Îª¹¦ÂÊ¼ÆËã)--------------------------------------------------
    PRO.sta_uar = MEAN_DATA.ua1 * 1.110721;         //µ¥Î»V ²ÉÓÃÍøÑ¹×÷Îª¶¨×ÓµçÑ¹£¬ÒòÎª¶¨×ÓV-LEM·´À¡µçÑ¹¾­¹ýÁË´ó³£ÊýÂË²¨
	PRO.sta_ubr = MEAN_DATA.ub1 * 1.110721;
	PRO.sta_ucr = MEAN_DATA.uc1 * 1.110721;

//------------------------¶¨×ÓÏàµçÁ÷ÓÐÐ§ÖµÏÔÊ¾Öµ¼ÆËã(Îª¹¦ÂÊ¼ÆËã)--------------------------------------------------
    PRO.sta_iar = MEAN_DATA.ia3 * 1.110721;			//µ¥Î»A
	PRO.sta_ibr = MEAN_DATA.ib3 * 1.110721;
	PRO.sta_icr = MEAN_DATA.ic3 * 1.110721;

//------------------¶¨×Ó²àÓÐ¹¦ºÍÎÞ¹¦¼ÆËãÖµ--------------------------------------------------------------------
    PRO.Psactive   = 1.5 * SQRT3 * (DIP_STA_U.dflt * DIP_STA_I.dflt + DIP_STA_U.qflt * DIP_STA_I.qflt); 
    PRO.Psreactive = 1.5 * SQRT3 * (DIP_STA_U.qflt * DIP_STA_I.dflt - DIP_STA_U.dflt * DIP_STA_I.qflt); 
	PRO.Ps		   = SQRT3 * (PRO.sta_uar * PRO.sta_iar + PRO.sta_ubr * PRO.sta_ibr + PRO.sta_ucr * PRO.sta_icr);//20091007

//------------------------Íø²à²¢ÍøµçÁ÷ÓÐÐ§ÖµÏÔÊ¾Öµ¼ÆËã----------------------------------------------
    PRO.npr_iar = MEAN_DATA.ia1 * 1.110721;     			 //1.110721=PAI * SQRT2 / 4  	//µ¥Î»A
	PRO.npr_ibr = MEAN_DATA.ib1 * 1.110721;
	PRO.npr_icr = MEAN_DATA.ic1 * 1.110721;

//------------------------Íø²àÏàµçÑ¹ÓÐÐ§ÖµÏÔÊ¾Öµ¼ÆËã----------------------------------------------
    PRO.npr_uar = MEAN_DATA.ua1 * 1.110721;      			//1.110721=PAI * SQRT2 / 4  //µ¥Î»A
	PRO.npr_ubr = MEAN_DATA.ub1 * 1.110721;
	PRO.npr_ucr = MEAN_DATA.uc1 * 1.110721;

//------------------Íø²àÓÐ¹¦ºÍÎÞ¹¦¼ÆËãÖµ--------------------------------------------------------------------
 	PRO.Pnactive   = Pnactive;
	PRO.Pnreactive = Pnreactive;
    PRO.Pn         = PRO.npr_iar * PRO.npr_uar + PRO.npr_ibr * PRO.npr_ubr + PRO.npr_icr * PRO.npr_ucr;

//------------------------------¶¨×Ó²àºÍÍø²à×Ü²¢Íø¹¦ÂÊÏÔÊ¾-----------------------------------------------------
	PRO.Pgactive   = PRO.Psactive   + PRO.Pnactive;
	PRO.Pgreactive = PRO.Psreactive + PRO.Pnreactive;

    if(CAP4.omigaslp >= 0)	 PRO.Pg = PRO.Ps - PRO.Pn;
	else 					 PRO.Pg = PRO.Ps + PRO.Pn;

//---------------------------Íø²àºÍ»ú²àµç¸ÐÎÂ¶ÈÖµ----------------------------
    PRO.NPR_TLOV= AMUX.Lac_temp;                    //Íø²àµç¸ÐÎÂ¶È
	PRO.MPR_TLOV= AMUX.Ldudt_temp;                  //»ú²àµç¸ÐÎÂ¶È

//--------------------------- ¶¨×Ó²àÏßµçÁ÷±£»¤Öµ----------------------------
	PRO.STA_iac = MEAN_DATA.iac3 * 1.110721  * 100/ STA_IACN;
	PRO.STA_iba = MEAN_DATA.iba3 * 1.110721  * 100/ STA_IACN;

//---------------------------×ªËÙ±£»¤Öµ¼ÆËã----------------------------------
	PRO.speed   = 9.5492966 * QEPDATA.omigamec;		//µ¥Î»£º×ª/·Ö£»n=60*w/2pai=*w
//	PRO.speed   = 1200;		//µ¥Î»£º×ª/·Ö£»n=60*w/2pai=*w

//	if(_stdby05!=0)   	DataFilter(_stdby01,&PRO.speedflt,PRO.speed); 	//Ö÷¿Ø×ªËÙ·´À¡ÖµÂË²¨£¬ Ts=200us,Tr=248ms 20090815
//	else   				PRO.speedflt= PRO.speed;         //20090816test

//	DataFilter(0.8,&PRO.speedflt,PRO.speed); 				//Ö÷¿Ø×ªËÙ·´À¡ÖµÂË²¨£¬ Ts=200us,Tr=5ms 20091021atzyÖ®Ç°123msÂË²¨Ì«´óÁË
	DataFilter(0.45,&PRO.speedflt,PRO.speed); 				//Ö÷¿Ø×ªËÙ·´À¡ÖµÂË²¨£¬ Ts=200us,Tr=1ms 20111116
	
	if(PRO.speedflt>700)		M_SetFlag(SL_SPEED_HIGH);	//½øÈë¸ß×ªËÙÇø   20091021atzy 650¸ÄÎª700£¬¼Ó´óÖÍ»·
    else if(PRO.speedflt<600)	M_ClrFlag(SL_SPEED_HIGH);   //650r/min²âÖÜ·¨PRD=845.²âÆµ·¨POSLAT=887.

    if((PRO.speedflt > _SC_MSPEED1) && (PRO.speedflt < _SC_MSPEED2)) 	 M_SetFlag(SL_SPEED_IN_RANGE);  //20091021atzy	
	else													 M_ClrFlag(SL_SPEED_IN_RANGE);

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

  float temp_pgactive,temp_pgreactive;
//------------------¸ù¾Ý×Ó³ÌÐòÉÏ±¨µÄÐÅÏ¢£¬ÅÐ¶ÏÊÇTAB_MSGÖÐÄÄÒ»¸ö-------------------------------------
	if(M_ChkFlag(SL_POWERON)==0)										//ÈôµçÍê³É¹ÊÕÏÔò¼ì²â¹ÊÕÏ
	{	
//---------------------------------TZ1ÖÐ¶Ï±£»¤------------------------------------------------------
		if(M_ChkFlag(SL_PDPINTA)!=0)	
		{	
			if(M_ChkFlag(SL_IN2_IOVA1)!=0)			M_SetFlag(SL_HIA1);		//Íø²àAÏàSKiiP¹ÊÕÏ
			else if(M_ChkFlag(SL_IN2_IOVB1)!=0)	   	M_SetFlag(SL_HIB1);		//Íø²àBÏàSKiiP¹ÊÕÏ
		    else if(M_ChkFlag(SL_IN2_IOVC1)!=0)	   	M_SetFlag(SL_HIC1);		//Íø²àCÏàSKiiP¹ÊÕÏ
			else if(M_ChkFlag(SL_IN3_VDCOV)!=0)   	M_SetFlag(SL_HUDCOV);	//Ö±Á÷²à¹ýÑ¹¹ÊÕÏ
			else if(M_ChkFlag(SL_IN3_NPRIOV)!=0)  	M_SetFlag(SL_HIACOV1);	//Íø²àÓ²¼þ¹ýÁ÷¹ÊÕÏ
			else if(M_ChkFlag(SL_PDPASERIES)!=0)	M_ClrFlag(SL_PDPINTA);	//ÔÙÔÊÐíÆô¶¯±äÁ÷Æ÷	20091107atzy						
			else									M_SetFlag(SL_PDPASERIES);//·¢ÉúÓ²¼þ¹ÊÕÏ,CPLDÃ»ÓÐ´æµ½¹ÊÕÏ
			
			M_SetFlag(SL_ERRDATASAVE);	//´¥·¢Íâ²¿RAMÊý¾Ý×ª´æ20091109atzy
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
			if(M_ChkFlag(SL_IN2_IOVA2)!=0)			M_SetFlag(SL_HIA2);			//µç»ú²àAÏàSKiiP¹ÊÕÏ
			else if(M_ChkFlag(SL_IN2_IOVB2)!=0)	    M_SetFlag(SL_HIB2);			//µç»ú²àBÏàSKiiP¹ÊÕÏ
		    else if(M_ChkFlag(SL_IN2_IOVC2)!=0)	    M_SetFlag(SL_HIC2);			//µç»ú²àCÏàSKiiP¹ÊÕÏ
		    else if(M_ChkFlag(SL_IN3_MPRIOV)!=0)    M_SetFlag(SL_HIACOV2);		//µç»ú²àÓ²¼þ¹ýÁ÷¹ÊÕÏ
			else if(M_ChkFlag(SL_PDPBSERIES)!=0)	M_ClrFlag(SL_PDPINTB);		//ÔÊÐíÆô¶¯±äÁ÷Æ÷	20091107atzy						
			else									M_SetFlag(SL_PDPBSERIES);	//·¢ÉúÓ²¼þ¹ÊÕÏ,CPLDÃ»ÓÐ´æµ½¹ÊÕÏ

//			M_SetFlag(SL_ERRDATASAVE);	//´¥·¢Íâ²¿RAMÊý¾Ý×ª´æ20091109atzy
		}
		else
		{
			M_ClrFlag(SL_HIA2);    											//Çå¹ÊÕÏ±êÖ¾
			M_ClrFlag(SL_HIB2);	   		
			M_ClrFlag(SL_HIC2);	    
			M_ClrFlag(SL_HIACOV2);
			M_ClrFlag(SL_PDPBSERIES);	
		}

//---------------------------------E-STOP±£»¤-------------------------------------------------------
		if(M_ChkFlag(SL_IN1_EXESTOP)!=0)								//Íâ²¿¼±Í£¹ÊÕÏ  ²Ù×÷°åÐÅºÅ£¬·ç³¡Ã»ÓÃ								
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_estop,DELAY_ESTOP)>=0)   M_SetFlag(SL_ESTOP);		//½ô¼±Í£Ö¹ÑÓ³ÙÊ±¼äµ½£¿
			else M_ClrFlag(SL_ESTOP);									//Çå±êÖ¾Î»
		}
		else 
		{
			MAIN_LOOP.cnt_estop=0;										//Çå¶¨Ê±Æ÷
			M_ClrFlag(SL_ESTOP);
		}  

//---------------------------------Íâ²¿Ó²¼þ¹ÊÕÏ±£»¤-------------------------------------------------
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
	    if(_CANOPER!=0) 											//ÉÏµçÇÒÑ¡ÔñCAN¿ØÖÆ²Å½øÐÐÍ¨Ñ¶¹ÊÕÏ
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
	    	    
//-----------------------Ö÷¶ÏºÏÕ¢/·ÖÕ¢/ÒâÍâ¶Ï¿ª¹ÊÕÏ---------------------------------------
	    if((M_ChkFlag(CL_CB)!=0 && M_ChkFlag(SL_IN1_CBSTS)==0)||(M_ChkFlag(CL_CBENGSTRG)==0 && M_ChkFlag(SL_IN1_CBSTS)!=0)||(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(CL_CBENGSTRG)!=0 && M_ChkFlag(SL_CBCLOSED)!=0 && M_ChkFlag(SL_IN3_CBTRIP)==0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_cberror,DELAY_CBERROR)>=0)	M_SetFlag(SL_ERROR_CB);	  //2s  
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

//----------------------Ô¤³äµç¹ÊÕÏ--------------------------------------- 
	    if((M_ChkFlag(CL_PRE)!=0)&&(M_ChkFlag(CL_MAINK)==0))	     //							
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
	    if(((M_ChkFlag(SL_QEPPCO)!=0)||(M_ChkFlag(SL_QCAPDISTURB)!=0)||(M_ChkFlag(SL_QEPZDISTRUB)!=0))&&(M_ChkFlag(SL_OCS_EIN)!=0)&&(M_ChkFlag(SL_QCAPSPDIN)!=0))	//QEPÉÏÒç»òÕßQCAP¡¢QEP ZÐÅºÅÊÜ¸ÉÈÅ¹ÊÕÏ  20090804ÓÚcpc							
//	    if((M_ChkFlag(SL_QEPPCO)!=0)||(M_ChkFlag(SL_QCAPDISTURB)!=0)||(M_ChkFlag(SL_QEPZDISTRUB)!=0))	//QEPÉÏÒç»òÕßQCAP¡¢QEP ZÐÅºÅÊÜ¸ÉÈÅ¹ÊÕÏ  20090804ÓÚcpc	//201105atzuoyun						
     	{
			M_SetFlag(SL_ENCODFAULT);				     			    //ÖÃ±êÖ¾Î»
		}
		else 
		{
			M_ClrFlag(SL_ENCODFAULT);									//Çå±êÖ¾Î»
		} 

//-----------------------µç»ú¹¤×÷×ªËÙ·¶Î§ÅÐ¶Ï-----------------------------------------------------
	    if(M_ChkFlag(SL_CHARGEOK)!=0)									//Ô¤³äµçOKºó²Å½øÐÐ×ªËÙ³¬³ö·¶Î§ÅÐ¶Ï 20090815
		{
			if((M_ChkFlag(SL_MSPOUT)==0)&& ((PRO.speedflt<_SC_MSPEED1)||(PRO.speedflt>_SC_MSPEED2)))  //¼Ó¸öÑÓÊ±ÅÐ¶Ï20090817
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_speedout,DELAY_SPEEDOUT)>=0)	M_SetFlag(SL_MSPOUT);	//50ms 20091022atzy									//ÖÃ³¬³ö×ªËÙ·¶§±êÖ¾Î»
			}
			else  MAIN_LOOP.cnt_speedout=0;
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
			M_SetFlag(SL_SIAC1);									//ÖÃÈí¼þ¹ýÁ÷±êÖ¾Î»
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
//-------------------------ÖÐ¼äÖ±Á÷µçÑ¹Èí¼þÇ·Ñ¹ÅÐ¶Ï------ÓÐÎÊÌâ£¡-------------------------------------------
	    if((M_ChkFlag(SL_CHARGEOK)!=0)&&(M_ChkFlag(SL_NPR_PWMOUT)!=0))//Ô¤³äµçÍê³Éºó²Å½øÐÇ·Ñ¹ÅÐ¶Ï
		{
			if((M_ChkFlag(SL_SUDCLV)==0)&&(PRO.udc<_SC_UDCLV))	//ÏÖ³¡ÔÝÊ±¸ÄÎª0ÁË
			{
				M_SetFlag(SL_SUDCLV);									//Ö®Ç°Ã»ÓÐÇ·Ñ¹£¬´Ë¿Ì¼ì²â³öÇ·£¬ÖÃÇ·Ñ¹±êÖ¾
			}	
			else if((M_ChkFlag(SL_SUDCLV)!=0)&&(PRO.udc>=(_SC_UDCLV+SC_UDC_HW)))	
			{
				M_ClrFlag(SL_SUDCLV);									//Ö®Ç°Ç·Ñ¹£¬´Ë¿Ì¼ì²â³ö³¬¹ý(Ç·Ñ¹Öµ¼Ó»Ø²î)£¬ÇåÇ·Ñ¹±êÖ¾
			}		
		}
		else	M_ClrFlag(SL_SUDCLV);	//20100511		

//------------------------ÖÐ¼äÖ±Á÷µçÑ¹Èí¼þ¹ýÑ¹ÅÐ¶Ï--------------------------------------------------
		if((M_ChkFlag(SL_SUDCOV)==0)&&(PRO.udc>_SC_UDCOV))	                   
		{
			M_SetFlag(SL_SUDCOV);	 	//
//			M_SetFlag(SL_ERRDATASAVE);	//´¥·¢Íâ²¿RAMÊý¾Ý×ª´æ20091109atzy
		}
		else if((M_ChkFlag(SL_SUDCOV)!=0)&&(PRO.udc<=(_SC_UDCOV-SC_UDC_HW)))   M_ClrFlag(SL_SUDCOV);  //Ö®Ç°¹ýÑ¹£¬´Ë¿Ì¼ì²âµÍÓÚ¹ýÑ¹Öµ¼õ»Ø²î	

//------------------------ÖÐ¼äÖ±Á÷µçÑ¹Èí¼þ¹ýÑ¹ÅÐ¶Ï(µçÍøµçÑ¹µøÂä) --------------------------------------------------
//		if((M_ChkFlag(SL_SUDCOVH)==0)&&(PRO.udc>_SC_UDCOVH))	                   M_SetFlag(SL_SUDCOVH);	 //Ö®Ç°Ã»ÓÐ¹ýÑ¹£¬´Ë¿Ì¼ì²âö¹ýÑ¹£¬ÖÃÇ·Ñ¹±êÖ¾

//		else if((M_ChkFlag(SL_SUDCOVH)!=0)&&(PRO.udc<=(_SC_UDCOVH-SC_UDC_HW)))     M_ClrFlag(SL_SUDCOVH);  //Ö®Ç°¹ýÑ¹£¬´Ë¿Ì¼ì²âµÍÓÚ¹ýÑ¹Öµ¼õ»Ø²î	

 
//-----------------------ÍøÑ¹Ç·Ñ¹Èí¼þÅÐ¶Ï(ÏßÑ¹ÓÐÐ§Öµ)-----------------------------------------------
	   if(M_ChkCounter(MAIN_LOOP.cnt_uaclv1,DELAY_UACLV1)>0)	//10s ÉÏµçÑÓÊ±ÅÐ¶ÏÍø²àÈíÇ·Ñ¹¹ÊÕÏ
	   {
		if(M_ChkFlag(SL_UACLV1)==0)
		{
			if((PRO.GID_uab<_SC_UACLV1)||(PRO.GID_ubc<_SC_UACLV1))	M_SetFlag(SL_UACLV1);  //20091026atzy Á½¸öÏßµçÑ¹ÈÎÒâÒ»¸ö²»´ïÒªÇó,ÖÃÇ·±êÖ¾
		}
		else 
		{
			if((PRO.GID_uab>=(_SC_UACLV1+SC_UAC_HW))&&(PRO.GID_ubc>=(_SC_UACLV1+SC_UAC_HW)))	M_ClrFlag(SL_UACLV1); //20091026atzy Á½¸öÏßµçÑ¹¾ù»Ø¸´µ½Õý³£ºóÇåÇ·¹±êÖ¾	
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

/*
//-----------------------¶¨×Ó²àÍøÑ¹Ç·Ñ¹Èí¼þÅÐ¶Ï(ÏßÑ¹ÓÐÐ§Öµ)-----------------------------------------------
		if(M_ChkFlag(SL_UACLV2)==0)
		{
			if((PRO.STA_uab<_SC_UACLV2)||(PRO.STA_ubc<_SC_UACLV2))	 M_SetFlag(SL_UACLV2);	  //Á½¸öÏßµçÑ¹ÈÎÒâÒ»¸ö²»´ïÒªÇó,ÖÃÇ·Ñ¹±êÖ¾

		}
		else 
		{
			if((PRO.STA_uab>=(_SC_UACLV2+SC_UAC_HW))&&(PRO.STA_ubc>=(_SC_UACLV2+SC_UAC_HW)))  M_ClrFlag(SL_UACLV2);		//Á½¸öÏßµçÑ¹¾ù»Ø¸´µ½Õý³£ºóÇåÇ·Ñ¹±êÖ¾

		}
						
//----------------------¶¨×Ó²àÍøÑ¹¹ýÑ¹Èí¼þÅÐ¶Ï(ÏßÑ¹ÓÐÐ§Öµ)------------------------------------------------
		if(M_ChkFlag(SL_UACOV2)==0)
		{
			if((PRO.STA_uab>_SC_UACOV2)||(PRO.STA_ubc>_SC_UACOV2))	 M_SetFlag(SL_UACOV2);

		}
		else
 		{
   			if((PRO.STA_uab<=(_SC_UACOV2-SC_UAC_HW))&&(PRO.STA_ubc<=(_SC_UACOV2-SC_UAC_HW)))   M_ClrFlag(SL_UACOV2);

		}
*/ 
//------------------------µçÍøÆµÂÊ¹ÊÕÏ--------------------------------------------------------------
		if(M_ChkFlag(SL_IN1_CBSTS)!=0)     									//Ö÷¶Ï±ÕºÏÔÙÅÐ¶ÏÍøÆµ¹ÊÕÏ 20090816
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_cbfreq,DELAY_CBFREQ)>=0)      	//1s
			{
				if(M_ChkFlag(SL_GRDFQE)!=0)									//10¸öÍøÑ¹ÖÜÆÚ(200ms),ÔòÖÃÆµÂÊ´íÎó±êÖ¾
				{
					if(M_ChkCounter(MAIN_LOOP.cnt_freq,DELAY_FREQ)>=0)	 M_SetFlag(SL_FE1);	
				}	
				else
				{
					M_ClrFlag(SL_FE1);									   	//·ñÔòÇåÆµÂÊ´íÎó±êÖ¾
					MAIN_LOOP.cnt_freq=0;                                  	//¼ÆÊ±Æ÷Ã¿1ms¼Ó1
				}
			}
		}
		else   	
		{
			MAIN_LOOP.cnt_cbfreq=0;	
			M_ClrFlag(SL_FE1);
		}
	
//-----------------------¶¨×ÓµçÁ÷¹ýÁ÷ÅÐ¶Ï-----------------------------------------------------------
		if((M_ChkFlag(SL_SIOVST)==0)&&((PRO.STA_iac>_SC_IACOVST)||(PRO.STA_iba>_SC_IACOVST)))
		{
			M_SetFlag(SL_SIOVST);									//ÖÃ¶¨×ÓµçÁ÷¹ýÁ÷±êÖ¾Î»
		}
		else if((M_ChkFlag(SL_SIOVST)!=0)&&(PRO.STA_iac<(_SC_IACOVST-SC_IAC_HW))&&(PRO.STA_iba<(_SC_IACOVST-SC_IAC_HW))) 
		{
			M_ClrFlag(SL_SIOVST);									//Çå¶¨×ÓµçÁ÷¹ýÁ÷±êÖ¾Î»
		}

//-----------------------ÖÐ¼äµçÑ¹²»ÎÈ¶¨-------------------------------------------------------------
        if((M_ChkFlag(SL_STEADYFB)!=0)&&(M_ChkFlag(SL_NPR_PWMOUT)!=0))
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

		if((M_ChkFlag(SL_SKTOV)==0)&&(AMUX.skiiptempmax > 100.0))
		{
			M_SetFlag(SL_SKTOV);									//ÖÃSKIIP³¬ÎÂ¹ÊÕÏ±êÖ¾Î»
		}
		else if((M_ChkFlag(SL_SKTOV)!=0)&&(AMUX.skiiptempmax < 90.0)) 
		{
			M_ClrFlag(SL_SKTOV);									//ÇåSKIIP³¬ÎÂ¹ÊÕÏ±êÖ¾Î»
		}
        
//----------------------Íø²àSKIIP³¬ÎÂ¹ÊÕÏ----------------------------------------------------------			
		if((M_ChkFlag(SL_IN2_TAOV)!=0)&&(M_ChkFlag(SL_TAOVONCE)==0))
		{	
			M_SetFlag(SL_TAOVONCE);           		//ÖÃÎÂ¶È¼ì²â¿¹¸ÉÈÅ±êÖ¾Î»
		}
		else if((M_ChkFlag(SL_IN2_TAOV)!=0)&&(M_ChkFlag(SL_TAOVONCE)!=0))
		{
			M_SetFlag(SL_TAOV); 					//ÖÃ³¬ÎÂ±êÖ¾Î»
		}		
       	else
		{
			M_ClrFlag(SL_TAOVONCE); 				//Çå³¬ÎÂ¼ì²â¿¹¸ÉÈÅ±êÖ¾Î»
			M_ClrFlag(SL_TAOV);						//Çå³¬ÎÂ±êÖ¾Î»
		}

//--------------------µç»ú²àSKIIP³¬ÎÂ¹ÊÕÏ-----------------------------------------------------------	
		if((M_ChkFlag(SL_IN2_TBOV)!=0)&&(M_ChkFlag(SL_TBOVONCE)==0))
		{	
			M_SetFlag(SL_TBOVONCE);            		//ÖÃÎÂ¶È¼ì²â¿¹¸ÉÈÅ±êÖ¾Î»
		}
		else if((M_ChkFlag(SL_IN2_TBOV)!=0)&&(M_ChkFlag(SL_TBOVONCE)!=0))
		{
		   	M_SetFlag(SL_TBOV); 					//ÖÃ³¬ÎÂ±êÖ¾Î»
		}		
       	else
		{
			M_ClrFlag(SL_TBOVONCE); 				//Çå³¬ÎÂ¼ì²â¿¹¸ÉÈÅ±êÖ¾Î»
			M_ClrFlag(SL_TBOV);						//Çå³¬ÎÂ±êÖ¾Î»
		}


//----------------------Íø²àµç¸Ð³¬ÎÂ¹ÊÕÏ----------------------------------------------------------			
		if((M_ChkFlag(SL_NPR_TLOV)==0)&&(PRO.NPR_TLOV>_SC_NPR_TLOV))
		{
			M_SetFlag(SL_NPR_TLOV);									//ÖÃÍø²àµç¸Ð³¬ÎÂ±êÖ¾Î»
		}
		else if((M_ChkFlag(SL_NPR_TLOV)!=0)&&(PRO.NPR_TLOV<(_SC_NPR_TLOV - 10))) 
		{
			M_ClrFlag(SL_NPR_TLOV);									//ÇåÍø²àµç¸Ð³¬ÎÂ±êÖ¾Î»
		} 


//--------------------µç»ú²àµç¸Ð³¬ÎÂ¹ÊÕÏ-----------------------------------------------------------	
		if((M_ChkFlag(SL_MPR_TLOV)==0)&&(PRO.MPR_TLOV>_SC_MPR_TLOV))
		{
			M_SetFlag(SL_MPR_TLOV);									//ÖÃ»ú²àµç¸Ð³¬ÎÂ±êÖ¾Î»
		}
		else if((M_ChkFlag(SL_MPR_TLOV)!=0)&&(PRO.MPR_TLOV<(_SC_MPR_TLOV - 10))) 
		{
			M_ClrFlag(SL_MPR_TLOV);									//Çå»ú²àµç¸Ð³¬ÎÂ±êÖ¾Î»
		} 

//-----------------------±äÁ÷Æ÷ÓÐ¹¦¹¦ÂÊ¹ýÔØÅÐ¶Ï-----------------------------------------------------------
		temp_pgactive = abs(PRO.Pgactive);							//20090816
		temp_pgactive = temp_pgactive * 0.001;
		if((M_ChkFlag(SL_PGOV)==0)&&(temp_pgactive>_SC_PGOV))
		{
			M_SetFlag(SL_PGOV_COM);
			if(M_ChkCounter(MAIN_LOOP.cnt_pgovload,DELAY_PGOVLOAD)>0)  	//10s
				M_SetFlag(SL_PGOV);										//ÖÃ±äÁ÷Æ÷ÓÐ¹¦¹¦ÂÊ¹ýÔØ±êÖ¾Î»
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
 
//--------------------------------µÃµ½TAB_MSGÖÐµÄ¹ÊÕÏÐòºÅ-------------------------------------------
		_MSG_SCOUT2 = MSG_NONE;												//ÏÈ½«MSGÇåÁã
		
		if(M_ChkFlag(SL_CODEOK)==0)				_MSG_SCOUT2=MSG_CODEOK;      //1=¹¦ÄÜÂëÎ´Ð£ÑéÍê±Ï
		
		else if(M_ChkFlag(SL_EE_FAIL)!=0) 		_MSG_SCOUT2=MSG_EE_FAIL;     //2=EEPROM¹ÊÕÏ

		else if(M_ChkFlag(SL_CANOPENOVER)!=0) 	_MSG_SCOUT2=MSG_CAN_FAIL;    //3=CANÍ¨Ñ¶¹ÊÕÏ

		else if(M_ChkFlag(SL_PHORDE)!=0) 		_MSG_SCOUT2=MSG_PHORDE;      //4=ÏàÐò´íÎó

		else if(M_ChkFlag(SL_ENCODFAULT)!=0) 	_MSG_SCOUT2=MSG_ENCODFAULT;  //5=±àÂëÆ÷¹ÊÕÏ

		else if(M_ChkFlag(SL_ESTOP)!=0) 	    _MSG_SCOUT2=MSG_ESTOP;       //6=½ô¼±Í£»ú¹ÊÕÏ

		else if(M_ChkFlag(SL_CBTRIP)!=0) 	    _MSG_SCOUT2=MSG_CBTRIP;      //7=Ö÷¶ÏÍÑ¿Û¹ÊÕÏ

		else if(M_ChkFlag(SL_EXFAIL)!=0) 	 	_MSG_SCOUT2=MSG_EXFAULT;     //8=Íâ²¿Ó²¼þ¹ÊÕÏ

		else if(M_ChkFlag(SL_ERROR_CB)!=0) 	 	_MSG_SCOUT2=MSG_CBERROR;     //9=÷¶ÏºÏÕ¢¹ÊÕÏ

		else if(M_ChkFlag(SL_ERROR_PRE)!=0) 	_MSG_SCOUT2=MSG_PREERROR;    //10=Ô¤³äµç¹ÊÕÏ

		else if(M_ChkFlag(SL_ERROR_MAINK)!=0) 	_MSG_SCOUT2=MSG_MAINKERROR;  //11=Ö÷½Ó´¥Æ÷±ÕºÏ¹ÊÕÏ

		else if(M_ChkFlag(SL_ERROR_MAINF)!=0) 	_MSG_SCOUT2=MSG_MAINFERROR;  //12=Ö÷ÂË²¨Æ÷±ÕºÏ¹ÊÕÏ

		else if(M_ChkFlag(SL_ERROR_STAC)!=0) 	_MSG_SCOUT2=MSG_STACERROR;     //13=¶¨×ÓÓ´¥Æ÷±ÕºÏ¹ÊÕÏ

		else if(M_ChkFlag(SL_MSPOUT)!=0) 	    _MSG_SCOUT2=MSG_MSPEEDOUT;     //14=×ªËÙ³¬³ö·¶Î§¹ÊÕÏ
					
		else if(M_ChkFlag(SL_HIA1)!=0)			_MSG_SCOUT2=MSG_HIA1;     //15=Íø²à±äÁ÷Æ÷AÏàSKIIP¹ÊÕÏ

		else if(M_ChkFlag(SL_HIB1)!=0)			_MSG_SCOUT2=MSG_HIB1;     //16=Íø²à±äÁ÷Æ÷BÏàSKIIP¹ÊÕÏ

		else if(M_ChkFlag(SL_HIC1)!=0)			_MSG_SCOUT2=MSG_HIC1;     //17=Íø²à±äÁ÷Æ÷CÏàSKIIP¹ÊÕÏ
		
		else if(M_ChkFlag(SL_HIA2)!=0)			_MSG_SCOUT2=MSG_HIA2;     //18=µç»ú²à±äÁ÷Æ÷AÏàSKIIP¹ÊÏ

		else if(M_ChkFlag(SL_HIB2)!=0)			_MSG_SCOUT2=MSG_HIB2;     //19=µç»ú²à±äÁ÷Æ÷BÏàSKIIP¹ÊÕÏ

		else if(M_ChkFlag(SL_HIC2)!=0)			_MSG_SCOUT2=MSG_HIC2;     //20=µç»ú²à±äÁ÷Æ÷CÏàSKIIP¹ÊÕÏ

		else if(M_ChkFlag(SL_HUDCOV)!=0)		_MSG_SCOUT2=MSG_UDCOV;    //21=±äÁ÷Æ÷Ö±Á÷Ä¸ÏßÓ²¼þ¹ýÑ¹¹ÊÕÏ

        else if(M_ChkFlag(SL_HIACOV1)!=0)		_MSG_SCOUT2=MSG_HIAC1;    //22=Íø²àÓ²¼þ¹ýÁ÷¹ÊÕÏ

		else if(M_ChkFlag(SL_HIACOV2)!=0)		_MSG_SCOUT2=MSG_HIAC2;    //23=µç»ú²àÓ²¼þ¹ýÁ÷¹ÊÕÏ

        else if(M_ChkFlag(SL_PDPASERIES)!=0)	_MSG_SCOUT2=MSG_PDPASERIES;//24=Íø²àÑÏÖØ¹ÊÕÏ

		else if(M_ChkFlag(SL_PDPBSERIES)!=0)	_MSG_SCOUT2=MSG_PDPBSERIES;//25=Íø²àÑÏÖØ¹ÊÕÏ

		else if(M_ChkFlag(SL_SIAC1)!=0)			_MSG_SCOUT2=MSG_SIAC1;    //26=Íø²àÈí¼þ¹ýÁ÷¹ÊÕÏ

		else if(M_ChkFlag(SL_SIAC2)!=0)			_MSG_SCOUT2=MSG_SIAC2;    //27=µç»ú²àÈí¼þ¹ýÁ÷¹ÊÕÏ

		else if(M_ChkFlag(SL_FE1)!=0)			_MSG_SCOUT2=MSG_FE1;      //28=Íø²àÆµÂÊ²»·û¹ÊÕÏ

		else if(M_ChkFlag(SL_SUDCOV)!=0)		_MSG_SCOUT2=MSG_SUDCOV;   //29=Èí¼þÖÐ¼äÖ±Á÷µçÑ¹¹ýÑ¹
		
		else if(M_ChkFlag(SL_UACOV1)!=0)		_MSG_SCOUT2=MSG_SUACOV1;  //30=Èí¼þÍøÑ¹½»Á÷¹ýÑ¹

//		else if(M_ChkFlag(SL_SUDCLV)!=0)		_MSG_SCOUT2=MSG_SUDCLV;   //31=Èí¼þÖÐ¼äÖ±Á÷µçÑ¹Ç·Ñ¹ 20100507atzuoyun
				
		else if(M_ChkFlag(SL_UACLV1)!=0)		_MSG_SCOUT2=MSG_SUACLV1;  //32=Èí¼þÍøÑ¹½»Á÷Ç·Ñ¹
		
//		else if(M_ChkFlag(SL_UDCWAVE)!=0)		_MSG_SCOUT2=MSG_UDCWAVE;  //33=ÖÐ¼äµçÑ¹²¨¶¯¹ÊÕÏ

        else if(M_ChkFlag(SL_SIOVST)!=0)		_MSG_SCOUT2=MSG_SIOVST;   //34=Èí¼þ¼ì²â¶¨×Ó¹ýÁ÷¹ÊÕÏ
		
		else if(M_ChkFlag(SL_TAOV)!=0)			_MSG_SCOUT2=MSG_TOV1;	  //35=Íø²àSKIIP³¬ÎÂ
		
		else if(M_ChkFlag(SL_TBOV)!=0)			_MSG_SCOUT2=MSG_TOV2;  	  //36=µç»ú²àSKIIP³¬ÎÂ

		else if(M_ChkFlag(SL_SKTOV)!=0)			_MSG_SCOUT2=MSG_SKTOV;     //37=Èí¼þÅÐ¶ÏSKIIP³¬ÎÂ

        else if(M_ChkFlag(SL_NPR_TLOV)!=0)		_MSG_SCOUT2=MSG_TLOV1;     //38=µçÍø²àµç¸Ð³¬ÎÂ

		else if(M_ChkFlag(SL_MPR_TLOV)!=0)		_MSG_SCOUT2=MSG_TLOV2;     //39=µç»ú²àµç¸Ð³¬ÎÂ

        else if(M_ChkFlag(SL_PGOV)!=0)		    _MSG_SCOUT2=MSG_PGOV;      //40=±äÁ÷Æ÷ÓÐ¹¦¹¦ÂÊ¹ýÔØ

		else if(M_ChkFlag(SL_QGOV)!=0)	    	_MSG_SCOUT2=MSG_QGOV;      //41=±äÁ÷Æ÷ÎÞ¹¦¹¦ÂÊ¹ýÔØ

//------------------------¹ÊÕÏ´¦Àí---------------------------------------------------------

//NO1-------Ô­À´Ã»ÓÐ¹ÊÕÏ±¾´ÎÓÐ¹ÊÕÏ(»ò)Ô­À´ÓÐ¹ÊÕÏµ«±¾´Î¼¶±ð¸ü¸ß-------------------£¨¹ÊÕÏÉý¼¶£©------
		if(((_MSG_SCOUT2!=MSG_NONE)&&(M_ChkFlag(SL_ERROR)==0))||((_MSG_SCOUT2!=MSG_NONE)&&(M_ChkFlag(SL_ERROR)!=0)&&(TAB_MSG[_MSG_SCOUT2].rank > TAB_MSG[_MSG_SCOUT1].rank)))
		{													
			M_SetFlag(SL_ERROR);							//ÖÃ¹ÊÕÏ±êÖ¾Î»
			M_SetFlag(SL_DISPLAY5);                         //ÖÃÏµÍ³¹ÊÕÏÖ¸Ê¾

//----------------------------ÐÂ¹ÊÕÏÊôÐÔ±êÊ¾-------------------------------------------------------												
			if((TAB_MSG[_MSG_SCOUT2].attr & SHUT)==SHUT)		M_SetFlag(SL_SHUT);		//Í£»úÊôÐÔ
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
			
			if((TAB_MSG[_MSG_SCOUT2].attr & SAVE)==SAVE)		M_SetFlag(SL_SAVE);		//ÐèÒª´æ´¢
			else												M_ClrFlag(SL_SAVE);		

			if((TAB_MSG[_MSG_SCOUT2].attr & WARNING)==WARNING)	M_SetFlag(SL_WARNING);	//±¨¾¯ÊôÐÔ
			else												M_ClrFlag(SL_WARNING);	
	
			if((TAB_MSG[_MSG_SCOUT2].attr & OFFCB)==OFFCB)		M_SetFlag(SL_OFFCB);	//ÏÈ¶ÏÖ÷¶ÏÊôÐÔ
			else												M_ClrFlag(SL_OFFCB);	

			if((TAB_MSG[_MSG_SCOUT2].attr & REPORT)==REPORT)	M_SetFlag(SL_REPORT);	//ÉÏ±¨Ö÷¿ØÊôÐÔ
			else												M_ClrFlag(SL_REPORT);	

//----------------------------------¸ù¾Ý¹ÊÕÏÊôÐÔÅÐ¶ÏÏµÍ³¶¯×÷---------------------------------------
						
			if(M_ChkFlag(SL_NRCVR)!=0)		//²»¿ÉÖØÊÔ¹ÊÕÏ
			{
				M_SetFlag(SL_FORBIDRESET);	//¹ÊÕÏ´ÎÊý³¬³öÏÞÖÆ,½ûÖ¹Ö÷¿Ø¸´Î» 201105atzuoyun	
				M_SetFlag(SL_ERRSTOP);		
				M_SetFlag(SL_SERIESTOP);    //ÑÏÖØ¹ÊÕÏ£¬ÐëÊÖ¶¯¸´Î»
				M_ClrFlag(SL_NRCVR);		
			}

			if(M_ChkFlag(SL_OFFCB)!=0)		//ÑÏÖØ¹ÊÕÏ,Ö±½Ó¶ÏÖ÷¶Ï 201105atzuoyun
			{
				M_SetFlag(SL_ERRSTOP);		
				M_SetFlag(SL_SERIESTOP);    //ÑÏÖØ¹ÊÕÏ£¬ÔÊÐíÖ÷¿Ø¸´Î»
				M_ClrFlag(SL_OFFCB);	
			}


        	if(M_ChkFlag(SL_REPORT)!=0)   //¸Ã¹ÊÕÏÐèÒªÉÏ±¨¸øÖ÷¿Ø201105atzuoyun
			{
				M_SetFlag(SL_REPORT_OCS);  //ÉÏ±¨¸øÖ÷¿Ø			
				M_ClrFlag(SL_REPORT);												
			} 
			
            if(M_ChkFlag(SL_SHUT)!=0)
			{
				M_SetFlag(SL_ERRSTOP);												
				M_ClrFlag(SL_SHUT);
			}

/*			if(M_ChkFlag(SL_IRCVR)!=0)		//ÔÊÐíÁ¢¼´»Ö¸´¹ÊÕÏ ±»ÆÁ±Î,ÔÊÐí¹ÊÕÏÁ¢¼´»Ö¸´201105atzuoyun
			{
				M_ClrFlag(SL_IRCVR);							
				M_SetFlag(SL_ERRSTOP);
			}											
*/					

			if(M_ChkFlag(SL_CNT)!=0)
			{
				_SY_RTRN++;
//				if(_SY_RTRN>_SC_RTRN)	M_SetFlag(SL_SERIESTOP);		//³¬¹ý´ÎÊý,ÑÏÖØ¹ÊÕÏ	
				if(_SY_RTRN>_SC_RTRN)	M_SetFlag(SL_FORBIDRESET);		//¹ÊÕÏ´ÎÊý³¬³öÏÞÖÆ,½ûÖ¹Ö÷¿Ø¸´Î» 201105atzuoyun	

				M_ClrFlag(SL_CNT);				
			}

			if(M_ChkFlag(SL_OTSER)!=0)
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_otser,DELAY_OTSER)>=0)	//ÑÓÊ±Ê±¼äµ½
				{
					M_SetFlag(SL_SERIESTOP);							//ÑÏÖØ¹ÊÕÏ
					M_ClrFlag(SL_OTSER);								//Çå³¬Ê±ÑÏÖØ±êÖ¾
				}	
			}	

			MAIN_LOOP.cnt_rcvr=0;			//ÇåÑÓÊ±»Ö¸´¼ÆÊýÆ÷	
			_MSG_SCOUT1 = _MSG_SCOUT2;		//±¾´Î¹ÊÕÏÐÅÏ¢×ª´æ
								
//----------------------------¸üÐÂ¹ÊÕÏ¼ÇÂ¼²¢ÏòÉÏÎ»»ú±¨¹ÊÕÏ------------------------------------------						
			if((M_ChkFlag(SL_SAVE)!=0)&&(M_ChkFlag(SL_EEBUSY_ERRSAVE)==0))			
			{
				MAIN_LOOP.cnt_rcvr=0;			//ÇåÑÓÊ±»Ö¸´¼ÆÊýÆ÷	

				M_SetFlag(SL_EEASK_ERRSAVE);							//EEPROM²Ù×÷ÇëÇó
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
                _BA_EAGLRF = (int16)(DISP.aglrf);            			 //ÎÞ¹¦½Ç¶ÈÖ¸Áî

				_BA_EIA1  = (int16)(AD_OUT_NPR_I.a * 10);				//Íø²à±äÁ÷Æ÷,AÏàµçÁ÷Ë²Ê±Öµ
				_BA_EIB1  = (int16)(AD_OUT_NPR_I.b * 10);				//Íø²à±äÁ÷Æ÷,BÏàµçÁ÷Ë²Ê±Öµ
				_BA_EIC1  = (int16)(AD_OUT_NPR_I.c * 10);				//Íø²à±äÁ÷Æ÷,CÏàçÁ÷Ë²Ê±Öµ
				_BA_EIA2  = (int16)(AD_OUT_MPR_I.a * 10);				//µç»ú²à±äÁ÷Æ÷,AÏàµçÁ÷Ë²Ê±Öµ
				_BA_EIB2  = (int16)(AD_OUT_MPR_I.b * 10);	            //µç»ú²à±äÁ÷Æ÷,BÏàµçÁ÷Ë²Ê±Öµ
				_BA_EIC2  = (int16)(AD_OUT_MPR_I.c * 10);	            //µç»ú²à±äÁ÷Æ÷,CÏàµçÁ÷Ë²Ê±Öµ
				
				_BA_EUAB1  = (int16)PRO.NPR_uab;						//Íø²àabÏßµçÑ¹
				_BA_EUBC1  = (int16)PRO.NPR_ubc;						//Íø²àbcÏßµçÑ¹
				_BA_EUAB2  = (int16)PRO.STA_uab;						//¶¨×Ó²àabÏßµçÑ¹
				_BA_EUBC2  = (int16)PRO.STA_ubc;						//¶¨×Ó²àbcÏßµçÑ¹

				_BA_EUAB0    = (int16)DISP.grd_uab;			       		//Ö÷¶ÏÇ°Íø²àabÏßÑ¹
            	_BA_EUBC0    = (int16)DISP.grd_ubc;			       		//Ö÷¶ÏÇ°Íø²àbcÏßÑ¹

				_BA_ETLAC  = (int16)PRO.NPR_TLOV;                           //Íø²àµç¸ÐÎÂ¶È
				_BA_ETLDUDT= (int16)PRO.MPR_TLOV;                           //»ú²àµç¸ÐÎÂ¶È
				_BA_ETSKIIP= (int16)AMUX.skiiptempmax;                      //SKIIPÎÂ¶È	

				_BA_EFREQ  = (int16)(CAP4.freq * 10);						//µçÍøÆµÂÊ
				_BA_ESPEED = (int16)(PRO.speed);							//µç»ú×ªËÙ 

	            _BA_ENPRUD = (int16)TRS_NGS_U.d;              			//dÖáÖ÷¶ÏºóÍøÑ¹·´À¡20091026atzy
				_BA_ENPRUQ = (int16)TRS_NGS_U.q;          				//qÖáÖ÷¶ÏºóÍøÑ¹·´À¡20091026atzy
	            _BA_ENPRUD2 = (int16)TRS_NGS_U.dflt;          			//dÖáÖ÷¶ÏºóÍøÑ¹·´À¡ ÂË²¨ºó20091026atzy
				_BA_ENPRUQ2 = (int16)TRS_NGS_U.qflt;          			//qÖáÖ÷¶ÏºóÍøÑ¹·´À¡ ÂË²¨ºó20091026atzy
	            _BA_ENUDOUT = (int16)TRS_NPR_U.d;          				//Íø²àdÖáµçÑ¹Êä³ö20091026atzy
				_BA_ENUQOUT = (int16)TRS_NPR_U.q;          				//Íø²àqÖáµçÑ¹Êä³ö20091026atzy

				_BA_EPIONU  = (int16)(PI_NPR_U.out * 10);					//µ¥»A
				_BA_EPIONID = (int16)(PI_NPR_Id.out* 10);					//µ¥Î»V
				_BA_EPIONIQ	= (int16)(PI_NPR_Iq.out* 10);					//µ¥Î»V

				_BA_EMEXI   = (int16)(RUN_mpridrf_exi * 10);				//µ¥Î»A  »ú²àÀø´ÅÀíÂÛ¼ÆËãÖµÏÔÊ¾ cpc
				_BA_EPIOMID = (int16)(PI_MPR_Id.out* 10);					//µ¥»V
				_BA_EPIOMIQ	= (int16)(PI_MPR_Iq.out* 10);					//µ¥Î»V

				_BA_ESTAIAC = (int16)(DISP.sta_iac * 10);
				_BA_ESTAIBA = (int16)(DISP.sta_iba * 10);
				_BA_ETOQFB  = (int16)DISP.toqfb;				        	//×ª¾Ø·´À¡
				_BA_EPSTA   = (int16)(DISP.Psactive * 0.01);          		//¶¨×Ó²àÓÐ¹¦¹¦ÂÊÏÔÊ¾kW,DOT1 *10/1000=100
				_BA_EPNPR   = (int16)(DISP.Pnactive * 0.01);         		//Íø²àÓÐ¹¦¹¦ÂÊÏÔÊ¾
//				_BA_EPGRID  = (int16)(DISP.Pgactive * 0.01);          		//²¢Íø×ÜµÄÓÐ¹¦¹¦ÂÊÏÔÊ¾
				_BA_EPGRID  = (int16)(_BA_EPGRID);          				//¼ÇÂ¼PDP¹ÊÕÏ´ÎÊý 20100507atzuoyun

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
			_MSG_SCOUT1 = _MSG_SCOUT2;		//±¾´Î¹ÊÕÏÐÅÏ¢×ª´æ

			if(M_ChkFlag(SL_OTSER)!=0)
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_otser,DELAY_OTSER)>=0)	//ÑÓÊ±Ê±¼äµ½
				{
					M_SetFlag(SL_ERRSTOP);								//¹ÊÕÏ
					M_SetFlag(SL_SERIESTOP);							//ÑÏÖØ¹ÊÕÏ
					M_ClrFlag(SL_OTSER);								//Çå³¬Ê±ÑÏÖØ±êÖ¾
				}	
			}
			else	MAIN_LOOP.cnt_otser=0;				
		}

//NO3---------Ô­À´ÓÐ¹ÊÕÏ£¬±¾´ÎÃ»ÓÐ¹ÊÕÏ----------------------------------------£¨¹ÊÕÏ½µ¼¶ÎªÁã£©------			
		else if((_MSG_SCOUT2==MSG_NONE)&&(M_ChkFlag(SL_ERROR)!=0))					
		{				
		
			if(M_ChkFlag(SL_OTSER)!=0)		
			{
				M_ClrFlag(SL_OTSER);								//³¬Ê±ÑÏÖØ,±¾´ÎÃ»ÓÐ¹ÊÕÏ
				MAIN_LOOP.cnt_otser=0;  							//Çå³¬Ê±ÑÏÖØÑÓÊ±¼ÆÊýÆ÷
			}

			if(M_ChkFlag(SL_IRCVR)!=0)								//ÔÊÐíÁ¢¼´»Ö¸´¹ÊÕÏ
			{
				M_ClrFlag(SL_IRCVR);								//ÇåÁ¢¼´»Ö¸´±êÖ¾
				if(M_ChkFlag(SL_FORBIDRESET)==0)
				{
					M_ClrFlag(SL_ERROR);							//É¾³ý¹ÊÕÏÐÅºÅ
					M_ClrFlag(SL_ERRSTOP);							//É¾³ý¹ÊÕÏÐÅºÅ  new
					M_ClrFlag(SL_SERIESTOP);						//ÑÓÊ±»Ö¸´ÔÊÐíÇå³ýÏÖØ¹ÊÕÏ±êÖ¾201105atzuoyun
					M_ClrFlag(SL_REPORT_OCS); 						//²»ÔÙ³ÖÐø¹ÊÕÏÉÏ±¨Ö÷¿Ø	201105atzuoyun
				} 
			}
			
			if(M_ChkFlag(SL_DRCVR)!=0)								//ÔÊÐíÑÓÊ±»Ö¸´ÇÒ±¾´ÎÃ»ÓÐ¹ÊÕÏ
			{
				PRO.rcvr = _SC_RTRT * 1000;							//µ¥Î»±ä»»:s->ms
				if(M_ChkCounter(MAIN_LOOP.cnt_rcvr,PRO.rcvr)>=0)	//ÑÓÊ±Ê±¼äµ½
				{
					M_ClrFlag(SL_DRCVR);							//ÇåÑÓÊ±»Ö¸´±êÖ¾
					if(M_ChkFlag(SL_FORBIDRESET)==0)
					{
						M_ClrFlag(SL_ERROR);							//É¾³ý¹ÊÕÏÐÅºÅ
						M_ClrFlag(SL_ERRSTOP);							//É¾³ý¹ÊÕÏÐÅºÅ  new
						M_ClrFlag(SL_SERIESTOP);						//ÑÓÊ±»Ö¸´ÔÊÐíÇå³ýÏÖØ¹ÊÕÏ±êÖ¾201105atzuoyun
						M_ClrFlag(SL_REPORT_OCS); 						//²»ÔÙ³ÖÐø¹ÊÕÏÉÏ±¨Ö÷¿Ø	201105atzuoyun
					}
				}
			}
			else	
				MAIN_LOOP.cnt_rcvr=0;		
		}
//NO4---------------------------------Ô­À´Ã»ÓÐ¹ÊÕÏ±¾´ÎÒ²Ã»ÓÐ¹ÊÕÏ--------------£¨¹ÊÕÏÆ½¼¶ÎªÁã£©------
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
			M_ClrFlag(SL_DISPLAY5);                         //ÇåÏµÍ³¹ÊÕÏÖ¸Ê¾
		}
	}//if((M_ChkFlag(SL_POWERON)==0)&&(M_ChkFlag(SL_ERRSTOP)==0))

//-------------------------¹ÊÕÏÍ£»úºó¸´Î»´¦Àí-------------------------------------------------------
	if(((M_ChkFlag(SL_ERRSTOP)!=0)||(M_ChkFlag(SL_SERIESTOP)!=0)) &&(M_ChkFlag(SL_RESET)!=0)&&(_MSG_SCOUT2==0)&&(M_ChkFlag(SL_FORBIDRESET)==0))   //Ã»ÓÐ¹ÊÕÏÊ±²ÅÄÜ¸´Î»
	{																											//Ò»¶¨Ê±¼äÄÚ¹ÊÕÏ³¬³ö´ÎÊýÔò½ûÖ¹Ö÷¿Ø¸´Î» 201105atzuoyun
			M_ClrFlag(SL_SERIESTOP);				//Çå³ýÑÏÖØÍ£»ú¹ÊÕÏ±êÖ¾
			M_ClrFlag(SL_ERRSTOP);					//Çå³ýÍ£»ú¹ÊÕÏ±êÖ¾
			M_ClrFlag(SL_ERROR);					//Çå³ý¹ÊÕÏ±êÖ¾
//			_SY_RTRN=0;								//¹ÊÕÏ¼ÆÊýÆ÷ÇåÁã		201105atzuoyun
			M_ClrFlag(SL_REPORT_OCS); 				//²»ÔÙ³ÖÐø¹ÊÕÏÉÏ±¨Ö÷¿Ø	201105atzuoyun
			
			M_ClrFlag(SL_OCS_NPRSTART);	          	//ÇåÖ÷¿ØÃüÁî	 
			M_ClrFlag(SL_OCS_MPRSTART);				//ÇåÖ÷¿ØÃüÁî								

			M_ClrFlag(SL_EE_FAIL);					//Çå³ýEEROM¹ÊÕÏ±êÖ¾

			M_ClrFlag(SL_PDPINTA);					//Çå³ýÍø²àTZ1_PDP¹ÊÕÏ±êÖ¾
			M_ClrFlag(SL_PDPINTB);					//Çå³ý»ú²àTZ2_PDP¹ÊÕÏ±êÖ¾
			M_ClrFlag(SL_QEPPCO);
//			M_ClrFlag(SL_QEPPHE);
//			M_ClrFlag(SL_QEPPCDE);

			M_ClrFlag(SL_DISPLAY3);             	//ÃðPDP¹ÊÕÏÖ¸Ê¾µÆ
			ClrPdpint();							//PDPINTÖÐ¶ÏÇå¿Õ	
			EnPdpint();								//PDPINTÖÐ¶ÏÊ¹ÄÜ
	
			_MSG_SCOUT1=0;											//Çå¹ÊÕÏÐÅÏ¢Î»
			_MSG_SCOUT2=0;
			MAIN_LOOP.cnt_rcvr=0;                                   //¹ÊÕÏÑÓÊ±»Ö¸´¼ÆÊ±
			MAIN_LOOP.cnt_otser=0;                                  //³¬Ê±ÑÏÖØÑÓÊ±	

//			M_ClrFlag(SL_CANOPENOVER);               //CANÍ¨Ñ¶
	}

	if(_SY_RTRN!=0)
	{
		PRO.reset = _stdby05 * 1000;							//µ¥Î»±ä»»:s->ms

		if(M_ChkCounter(MAIN_LOOP.cnt_resetrn,PRO.reset)>=0)	//ÓÉ±¸ÓÃ_STDBY5Éè¶¨0-32767,µ¥Î»s
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
** º¯ÊýÃû³Æ: et_relay
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
//			M_SetFlag(SL_ERRDATASAVE);	//´¥·¢Íâ²¿RAMÊý¾Ý×ª´æ20091109atzy
		}
		else M_ClrFlag(SL_SIAC1);
	}
}  
/*********************************************************************************************************
** º¯ÊýÃû³Æ: et_relay
** ¹¦ÄÜÃèÊö: ¹ýÔØ±£»¤¼ÆËã--MPR
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
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
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
	if(MAIN_LOOP.cnt_freq2!=65535)		MAIN_LOOP.cnt_freq2++;							
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

	if(MAIN_LOOP.cnt_offcb!=65535)		MAIN_LOOP.cnt_offcb++;
	if(MAIN_LOOP.cnt_cbtp!=65535)		MAIN_LOOP.cnt_cbtp++;
	if(MAIN_LOOP.cnt_cberror!=65535)		MAIN_LOOP.cnt_cberror++;
	if(MAIN_LOOP.cnt_mainkerror!=65535)		MAIN_LOOP.cnt_mainkerror++;
	if(MAIN_LOOP.cnt_mainferror!=65535)		MAIN_LOOP.cnt_mainferror++;
	if(MAIN_LOOP.cnt_stacerror!=65535)		MAIN_LOOP.cnt_stacerror++;
	if(MAIN_LOOP.cnt_preerror!=65535)		MAIN_LOOP.cnt_preerror++;

	if(SCI.cnt_sciover!=65535)			SCI.cnt_sciover++;
	if(SCI.cnt_scispace!=65535)			SCI.cnt_scispace++;

	if(MAIN_LOOP.cnt_isteady0!=65535)		MAIN_LOOP.cnt_isteady0++;     //RunCtrlÍø²àÍ£»ú¸ø¶¨ÑÓÊ±
	if(MAIN_LOOP.cnt_mprsteady0!=65535)		MAIN_LOOP.cnt_mprsteady0++;   //RunCtrl»ú²àÍ£»ú¸ø¶¨ÑÓÊ±

	if(MAIN_LOOP.cnt_senszfstdy!=60001)		MAIN_LOOP.cnt_senszfstdy++;		//´«¸ÐÆ÷ÁãÆ¯ÂË²¨¼ÆÊýÆ÷
	if(MAIN_LOOP.cnt_speedout!=65535)		MAIN_LOOP.cnt_speedout++;		//20090817
	if(MAIN_LOOP.cnt_uaclv1!=65535)		MAIN_LOOP.cnt_uaclv1++;		//200901027atzy

//sysctl_zl_start
	if(MAIN_LOOP.cnt_opencontac!=65535)		MAIN_LOOP.cnt_opencontac++;    //ÑÏÖØ²»¿É»Ö¸´¹ÊÕÏÏÂ£¬·¢³ö¶ÏÖ÷¶ÏÖ¸Áîºóµ½·¢³ö¶Ï¶¨×Ó½Ó´¥Æ÷ºÍÖ÷½Ó´¥Æ÷µÄÑÓÊ±
	
	if(MAIN_LOOP.cnt_mkoff!=65535)	    	MAIN_LOOP.cnt_mkoff++;         //½ÓÊÕµ½ÏµÍ³Í£»úÖ¸Áîºó£¬ÑÓÊ±¶ÏÖ÷½Ó´¥Æ÷¼ÆÊý
	if(MAIN_LOOP.cnt_cboff!=65535)	    	MAIN_LOOP.cnt_cboff++;         //½ÓÊÕµ½ÏµÍ³Í£»úÖ¸Áîºó£¬ÑÓÊ±¶ÏÖ÷¶Ï¼ÆÊý
      
	if(MAIN_LOOP.cnt_closecb!=65535)		MAIN_LOOP.cnt_closecb++;        //Õý³£Æô¶¯£¬Ö÷¶Ï´¢ÄÜµ½Ö÷¶ÏºÏÕ¢µÄÑÓÊ±¼ÆÊý
	if(MAIN_LOOP.cnt_precok!=65535)      	MAIN_LOOP.cnt_precok++;         //Õý³£Æô¶¯£¬Ô¤³äµçÊ±¼ä

	if(MAIN_LOOP.cnt_steadyfb!=65535)      	MAIN_LOOP.cnt_steadyfb++;       //Õý³£Æô¶¯/Í£Ö¹£¬¼ì²âµ½VdcÎÈ¶¨ÔÚ1100Vµ½ÔÊÐí»ú²àÂö³åµÄÑÓÊ±¼ÆÊý
	if(MAIN_LOOP.cnt_mprstart!=65535)      	MAIN_LOOP.cnt_mprstart++;       //Õý³£Æô¶¯£¬VdcÎÈ¶¨ºóMPR¢Âö³åµÄÑÓÊ±¼ÆÊý 
//sysctl_zl_end	

//ADctl_zl_start
    if(MAIN_LOOP.cnt_AMUX!=65535)      	    MAIN_LOOP.cnt_AMUX++;           //ÂýËÙAD²ÉÑùÑÓÊ±¼°Ê±
//ADctl_zl_end
    if(MAIN_LOOP.cnt_reset!=65535)      	MAIN_LOOP.cnt_reset++;           //ÉÏÎ»»úI/O¸´Î»£¬ÑÓÊ±2s£¬·ÀÖ¹Îó²Ù×÷
    if(MAIN_LOOP.cnt_clostacmd!=65535)      MAIN_LOOP.cnt_clostacmd++;       //ÏÎ»úÔÊÐíºÏ¶¨×Ó½Ó´¥Æ÷£¬ÑÓÊ±1s£¬Ö¹Îó²Ù
    if(MAIN_LOOP.cnt_nprcmd!=65535)         MAIN_LOOP.cnt_nprcmd++;         //ÉÏÎ»»úÔÊÐíNPR·¢Âö³å¬ÑÓÊ±1s£¬·ÀÖ¹Îó²Ù×÷
    if(MAIN_LOOP.cnt_mprcmd!=65535)         MAIN_LOOP.cnt_mprcmd++;         //ÉÏÎ»»úÔÊÐíMPR·¢Âö³å£¬ÑÓÊ±1s£¬·À¹Îó²Ù×÷
    if(MAIN_LOOP.cnt_fanstar!=65535)        MAIN_LOOP.cnt_fanstar++;          //·ç»úÐÇÈý½Ç±ä»»ÑÓÊ±¼ÆÊý
    if(MAIN_LOOP.cnt_fantriangle!=65535)    MAIN_LOOP.cnt_fantriangle++;      //·ç»úÐÇÈý½Ç±ä»»ÑÓÊ±¼ÆÊý 
//    if(MAIN_LOOP.cnt_fanstop!=65535)        MAIN_LOOP.cnt_fanstop++;          //ÏµÍ³Í£»úºó£¬¿ØÖÆ·ç»úÒÔÐÇ½Ó¹¤×÷ÑÓÊ±¼ÆÊý 
	if(MAIN_LOOP.cnt_qcapspdin!=65535)		MAIN_LOOP.cnt_qcapspdin++;   
	if(MAIN_LOOP.cnt_async!=65535)		    MAIN_LOOP.cnt_async++;   
	if(MAIN_LOOP.cnt_qcapdisturb!=65535)	MAIN_LOOP.cnt_qcapdisturb++;		//QEP¿¹ÉÈÅ   
	if(MAIN_LOOP.cnt_qepcntok!=65535)		MAIN_LOOP.cnt_qepcntok++;   		//QEP¿¹¸ÉÈÅ   
	if(MAIN_LOOP.cnt_qepzdisturb!=65535)	MAIN_LOOP.cnt_qepzdisturb++;   		//QEP¿¹¸ÉÈÅ   
	   
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
** º¯ÊýÃû³Æ: Display
** ¹¦ÄÜÃèÊö: ÏÔÊ¾Öµ¼ÆËã
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
	DISP.grd_uab = MEAN_DATA.uab * 1.110721;        //µ¥Î»V  rms=mean*PAI/(2*sqrt(2)) £» 1.110721=PAI * SQRT2 / 4
    DISP.grd_ubc = MEAN_DATA.ubc * 1.110721;        //µ¥Î»V  rms=mean*PAI/(2*sqrt(2)) £» 1.110721=PAI * SQRT2 / 4

//---------------------------Íø²àºÍ»ú²àµç¸ÐÎÂ¶ÈÏÔÊ¾----------------------------
    DISP.Lac_temp      = AMUX.Lac_temp;                  //Íø²àµç¸ÐÎÂ¶È
	DISP.Ldudt_temp    = AMUX.Ldudt_temp;                //»ú²àµç¸ÐÎÂ¶È 
	DISP.NPR_skiptemp  = AMUX.NPR_skiiptemp;              //Íø²àSKIIPÎÂ¶È
	DISP.MPR_skiptemp  = AMUX.MPR_skiiptemp;              //»ú²àSKIIPÎÂ¶È


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
	temp2 = MEAN_DATA.iba3 * 1.110721;
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
		if(M_ChkCounter(MAIN_LOOP.cnt_nprlamp,DELAY_NPRRUN)>=0)	//0µÆ¿ìËÙÉÁË¸,Ö¸Ê¾Íø²àÕýÔÚ·¢Âö³åÔËÐÐ
		{
			M_NotFlag(SL_DISPLAY0);
			MAIN_LOOP.cnt_nprlamp=0;
		}
	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_nprlamp,DELAY_NPRSTDBY)>=0)	//0µÆÂýËÙÁË¸
		{
		   M_NotFlag(SL_DISPLAY0);
		   MAIN_LOOP.cnt_nprlamp=0;
		} 
	}
			
//------------------------------------»ú²à¹¤×÷Ö¸Ê¾µÆÏÔÊ¾--------------------------------------------
	if(M_ChkFlag(SL_MPR_PWMOUT)!=0)
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_mprlamp,DELAY_MPRRUN)>=0)	//1µÆ¿ìËÙÉÁË¸,¸Ê¾»ú²àÕýÔÚ·¢Âö³åÔËÐÐ
		{
			M_NotFlag(SL_DISPLAY1);
			MAIN_LOOP.cnt_mprlamp=0;
		}
	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_mprlamp,DELAY_MPRSTDBY)>=0)	//1µÆÂýËÙÉÁË¸
		{
			M_NotFlag(SL_DISPLAY1);
			MAIN_LOOP.cnt_mprlamp=0;
		}
	}

//----------------------------------------ÔËÐÐ¼à¿Ø--------------------------------------------------
	_BA_URF   = (int16)DISP.urf;				//ÖÐ¼äµçÑ¹¸ø¶¨Öµ
	_BA_UDC   = (int16)DISP.udc;				//ÖÐ¼äÖ±Á÷µçÑ¹
	_BA_MIDRF  = (int16)(DISP.mpridrf * 10);		//dÖáµçÁ÷Ö¸Áî
	_BA_NIQRF  = (int16)(DISP.npriqrf * 10);		//qÖáµçÁ÷Ö¸Áî
	_BA_MIQRF  = (int16)(DISP.mpriqrf * 10);		//qÖáµçÁ÷Ö¸Áî
	_BA_TOQRF  = (int16)(DISP.toqrf);				//×ª¾ØÖ¸Áî
    _BA_AGLRF  = (int16)(DISP.aglrf);             //ÎÞ¹¦½Ç¶ÈÖ¸Áî 20091027atzy

	_BA_IA1   = (int16)(DISP.npr_iar * 10);		//Íø²à,aÏà²¢ÍøµçÁ÷£¬¸ÄÎª1Î»¡Êý
	_BA_IB1   = (int16)(DISP.npr_ibr * 10);		//Íø²à,bÏà²¢ÍøµçÁ÷
	_BA_IC1   = (int16)(DISP.npr_icr * 10);		//Íø²à,cÏà²¢ÍøµçÁ÷
	_BA_IA2   = (int16)(DISP.mpr_iar * 10);		//µç»ú²à,aÏàµçÁ÷
	_BA_IB2   = (int16)(DISP.mpr_ibr * 10);		//µç»ú²à,bÏàµçÁ÷
	_BA_IC2   = (int16)(DISP.mpr_icr * 10);		//µç»ú²à,cÏàµçÁ÷

	_BA_UAB0   = (int16)DISP.grd_uab;			//Ö÷¶ÏÇ°Íø²àabÏßÑ¹
	_BA_UBC0   = (int16)DISP.grd_ubc;			//Ö÷¶ÏÇ°Íø²àbcÏßÑ¹				
	_BA_UAB1  = (int16)DISP.npr_uab;			//Íø²àabÏßÑ¹
	_BA_UBC1  = (int16)DISP.npr_ubc;			//Íø²àbcÏßÑ¹
//	_BA_UAB2  = (int16)DISP.sta_uab;			//¶¨×Ó²àabÏßÑ¹,¶¨×Ó²à 20091027atzy
//	_BA_UBC2  = (int16)DISP.sta_ubc;			//¶¨×Ó²àbcÏßÑ¹,¶¨×Ó²à 20091027atzy
	_BA_GRDUD = (int16)TRS_NGS_U.dflt;          //dÖáÖ÷¶ÏºóÍøÑ¹·´À¡ ÂË²¨ºó
	_BA_GRDUQ = (int16)TRS_NGS_U.qflt;          //qÖáÖ÷¶ÏºóÍøÑ¹·´À¡ ÂË²¨ºó
	_BA_STAID = (int16)TRS_NPR_I.dflt;          //dÖáÍø²àµçÁ÷·´À¡ ÂË²¨ºó
	_BA_STAIQ = (int16)TRS_NPR_I.qflt;          //qÖáÍø²àµçÁ÷·´À¡ ÂË²¨ºó
    _BA_EXCID = (int16)(TRS_MPR_I.d * 10);             //dÖá»ú²àµçÁ÷·´À¡ ÂË²¨Ç°
    _BA_EXCIQ = (int16)(TRS_MPR_I.q * 10);             //qÖá»ú²àµçÁ÷·´À¡ ÂË²¨Ç°
	_BA_STAUD = (int16)TRS_MPR_U.d;
	_BA_STAUQ = (int16)TRS_MPR_U.q;

	_BA_TLAC  = (int16)DISP.Lac_temp;		    //Íø²àµç¸ÐÎÂ¶È
	_BA_TLDUDT= (int16)DISP.Ldudt_temp;	        //»ú²àµç¸ÐÎÂ¶È
	_BA_TNSKIIP= (int16)DISP.NPR_skiptemp;      //Íø²àSKIIPÎÂ¶È ÉãÊÏ¶È
	_BA_TMSKIIP= (int16)DISP.MPR_skiptemp;      //»ú²àSKIIPÎÂ¶È ÉãÊÏ¶È

	_BA_FREQ  = (int16)(DISP.freq * 10);		//µçÍøÆµÂÊ
	_BA_SPEED = (int16)DISP.speed;				//µç»ú×ªËÙ 

	_BA_PIONU  = (int16)(DISP.pionu  * 10);			    //NPRµçÑ¹»·Êä³ö 6.23change_zl¸ÄÎª1Î»Ð¡Êý
	_BA_PIONID = (int16)(DISP.pionid * 100);			//NPRµçÁ÷»·dÊä³ö
	_BA_PIONIQ = (int16)(DISP.pioniq * 100);			//NPRµçÁ÷»·qÊä³ö
	_BA_MEXI   = (int16)(DISP.mexi  * 10);			    // 6.23change_zl¸ÄÎª1Î»Ð¡Êý ¸ÄÎªÀø´ÅµçÁ÷ÀíÂÛÖµÏÔÊ¾cpc
	_BA_PIOMID = (int16)(DISP.piomid * 100);			//MPRµçÁ÷»·dÊä³ö
	_BA_PIOMIQ = (int16)(DISP.piomiq * 100);			//MPRµçÁ÷»·qÊä³ö

//	_BA_STAUABD = (int16)(DISP.uab23 * 10);             //¶¨×ÓÍ¬²½²¢ÍøÇ°¶¨×ÓÇ°ºóµçÑ¹²î 20091027atzy
//	_BA_STAUBCD = (int16)(DISP.ubc23 * 10);				//¶¨×ÓÍ¬²½²¢ÍøÇ°¶¨×ÓÇ°ºóµçÑ¹²î 20091027atzy
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
	_BA_SGRID = (int16)(DISP.Pg * 0.01);              	//²¢Íø×ÜµÄÊÓÔÚ¹¦ÂÊÏÔÊ¾ 20100506

//Í¨¹ýCANOPENÍ¨Ñ¶·´À¡¸øÖ÷¿ØµÄ¼à¿Ø±äÁ¿	

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
** º¯ÊýÃû³Æ: Datasave
** ¹¦  ÄÜ: ½«Êý¾ÝÐ´ÈëÍâ²¿RAM
** Êä¡¡Èë:
** Êä¡¡³ö:        
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 20091109atzy
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Datasave(void)
{
//--------------------------------External RAM Data Save-----------------------------------------
//	if(_STDBY2!=0)	M_SetFlag(SL_ERRDATASAVE);	//´¥·¢Íâ²¿RAMÊý¾Ý×ª´æ20091109athotel

  	if(*(RAM_END - 1) != 0x55AA)
  	{
		if(M_ChkFlag(SL_ERRDATASAVE)==0)  								//Ã»ÓÐ¹ÊÕÏ	
			MAIN_LOOP.cnt_datasave=0;
		else if(M_ChkCounter(MAIN_LOOP.cnt_datasave,DELAY_DATASAVE)>=0)	//¹ÊÕÏ·¢ÉúºóÑÓÊ±2s½ûÖ¹»­Í¼
		{
			M_SetFlag(SL_DRAW);
			M_ClrFlag(SL_ERRDATASAVE);
			*(RAM_END - 1) = 0x55AA;

			*(RAM_END - 2) = RTIMER.time[0];							 //×îÐÂ¹ÊÕÏÊ±¼ä
			*(RAM_END - 3) = RTIMER.time[1];
			*(RAM_END - 4) = RTIMER.time[2];
			*(RAM_END - 5) = RTIMER.time[3];
			*(RAM_END - 6) = RTIMER.time[4];
			*(RAM_END - 7) = RTIMER.time[5];			
		}

		if(M_ChkFlag(SL_DRAW)==0)  										//ÓÐ¹ÊÕÏÖ®ºóÑÓÊ±2sºóÍ£Ö¹Êý¾Ý´æ´¢
		{
			*(RAM_START+ RAMDATA_POS) = (int16)AD_OUT_UDC;							//0=ÖÐ¼äÖ±Á÷µçÑ¹
//			*(RAM_START+ RAMDATA_POS) = (int16)testtheta;	//1=test
			*(RAM_START+((Uint32)RAM_BIAS * 1 + RAMDATA_POS)) = (int16)CAP4.nprtrstheta;	//1=NPR¶¨Ïò½Ç¶È
			*(RAM_START+((Uint32)RAM_BIAS * 2 + RAMDATA_POS)) = (int16)AD_OUT_NPR_I.a;		//2=NPR-AµçÁ÷
			*(RAM_START+((Uint32)RAM_BIAS * 3 + RAMDATA_POS)) = (int16)AD_OUT_NPR_I.b;		//3=NPR-BµçÁ÷
			*(RAM_START+((Uint32)RAM_BIAS * 4 + RAMDATA_POS)) = (int16)AD_OUT_NPR_I.c;		//4=NPR-CµçÁ÷
			*(RAM_START+((Uint32)RAM_BIAS * 5 + RAMDATA_POS)) = (int16)AD_OUT_GRD_U.ab;		//5=µçÍøµçÑ¹Vab
			*(RAM_START+((Uint32)RAM_BIAS * 6 + RAMDATA_POS)) = (int16)AD_OUT_GRD_U.bc;		//6=µçÍøµçÑ¹Vbc
			*(RAM_START+((Uint32)RAM_BIAS * 7 + RAMDATA_POS)) = (int16)TRS_NGS_U.d;			//7=ÍøÑ¹dÖá·ÖÁ¿edÂË²¨Ç°
			*(RAM_START+((Uint32)RAM_BIAS * 8 + RAMDATA_POS)) = (int16)TRS_NGS_U.q;			//8=ÍøÑ¹qÖá·ÖÁ¿eqÂË²¨Ç°
			*(RAM_START+((Uint32)RAM_BIAS * 9 + RAMDATA_POS)) = (int16)TRS_NGS_U.dflt;		//9=ÍøÑ¹dÖá·ÖÁ¿edÂË²¨ºó
			*(RAM_START+((Uint32)RAM_BIAS * 10 + RAMDATA_POS)) = (int16)TRS_NGS_U.qflt;		//10=ÍøÑ¹qÖá·ÖÁ¿eqÂË²¨ºó
			*(RAM_START+((Uint32)RAM_BIAS * 11 + RAMDATA_POS)) = (int16)PI_NPR_Id.reference;//11=Íø²àIdÖ¸Áî
			*(RAM_START+((Uint32)RAM_BIAS * 12 + RAMDATA_POS)) = (int16)TRS_NPR_I.dflt;		//12=Íø²àId·´À¡
			*(RAM_START+((Uint32)RAM_BIAS * 13 + RAMDATA_POS)) = (int16)PI_NPR_Id.out;		//13=Íø²àIdPIÊä³ö
			*(RAM_START+((Uint32)RAM_BIAS * 14 + RAMDATA_POS)) = (int16)TRS_NPR_U.d;		//14=Íø²àUdÊä³ö
			*(RAM_START+((Uint32)RAM_BIAS * 15 + RAMDATA_POS)) = (int16)PI_NPR_Iq.reference;//15=Íø²àIqÖ¸Áî
			*(RAM_START+((Uint32)RAM_BIAS * 16 + RAMDATA_POS)) = (int16)PI_NPR_Iq.feedback;	//16=Íø²àIq·´À¡
			*(RAM_START+((Uint32)RAM_BIAS * 17 + RAMDATA_POS)) = (int16)PI_NPR_Iq.out;		//17=Íø²àIqPIÊä³ö
			*(RAM_START+((Uint32)RAM_BIAS * 18 + RAMDATA_POS)) = (int16)TRS_NPR_U.q;		//18=Íø²àUqÊä³ö
			*(RAM_START+((Uint32)RAM_BIAS * 19 + RAMDATA_POS)) = (int16)CAP4.mprtrstheta;	//19=MPR¶¨Ïò½Ç¶È
			*(RAM_START+((Uint32)RAM_BIAS * 20 + RAMDATA_POS)) = (int16)AD_OUT_MPR_I.a;		//20=MPR-AµçÁ÷
			*(RAM_START+((Uint32)RAM_BIAS * 21 + RAMDATA_POS)) = (int16)AD_OUT_MPR_I.b;		//21=MPR-BµçÁ÷
			*(RAM_START+((Uint32)RAM_BIAS * 22 + RAMDATA_POS)) = (int16)AD_OUT_MPR_I.c;		//22=MPR-CµçÁ÷
			*(RAM_START+((Uint32)RAM_BIAS * 23 + RAMDATA_POS)) = (int16)RUN.mpriqrf;		//23=MPR-Iq²Î¿¼Öµ
			*(RAM_START+((Uint32)RAM_BIAS * 24 + RAMDATA_POS)) = (int16)CAP4.freqtmp;		//24=Êµ¼ÊÊµÊ±ÍøÆµ
			*(RAM_START+((Uint32)RAM_BIAS * 25 + RAMDATA_POS)) = (int16)PRO.speed;			//25=µç»ú×ªËÙ

			*(RAM_END) = RAMDATA_POS;												//µ±Ç°Êý¾Ý´æ´¢Î»ÖÃ×ª´æ

			RAMDATA_POS++;
			if(RAMDATA_POS >= RAM_BIAS)  RAMDATA_POS=0;		
		}
  	}
	
	if(_STDBY1!=0)		
	{
		M_ClrFlag(SL_DRAW);							//Íâ²¿±¸ÓÃ±êÖ¾1·ÇÁãÖÃ»­Í¼Ê¹ÄÜ±êÖ¾
		*(RAM_END - 1) = 0x0000;
		MAIN_LOOP.cnt_datasave=0;					//20100129atbjtu
		_BA_EPGRID = 0;								//¼ÇÂ¼Íø²àPDPÓ²¼þ¹ÊÕÏ·¢Éú´ÎÊý,ÇåÁã2010atzuoyun
	} 
//--------------------------------External RAM Data Read-----------------------------------------
} 


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
