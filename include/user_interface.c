/****************************************Copyright (c)**************************************************
**                       		     ±±	¾©	½»	Í¨	´ó	Ñ§
**                                        µçÆø¹¤³ÌÑ§Ôº
**                                         614ÊµÑéÊÒ
**
**                              
**
**--------------ÎÄ¼þÐÅÏ¢--------------------------------------------------------------------------------
**ÎÄ   ¼þ   Ãû: user_interface.c
**´´   ½¨   ÈË: 
**×îºóÐÞ¸ÄÈÕÆÚ: 
**Ãè        Êö: 1.5MWË«À¡µç»ú×ª×Ó²à±äÁ÷Æ÷ÍâÉè¿ØÖÆ³ÌÐò--Ë«À¡--×óÔÆ·ç³¡
				°üÀ¨¶Ôeeprom,sci,spi,ad,daµÄ¿ØÖÆ³ÌÐò
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
#include "DSP2833x_Device.h"     // Headerfile Include File
#include "DSP2833x_Examples.h"   // Examples Include File
//º¯ÊýÉùÃ÷
Uint16 		CheckCode(Uint16 index);
Uint16 		SciDatpro(void);
/*********************************************************************************************************
** º¯ÊýÃû³Æ: EeStart
** ¹¦ÄÜÃèÊö: ¿ªÊ¼¶ÔeepromµÄ²Ù×÷
** Êä¡¡Èë: 
** Êä¡¡³ö:        
** ×¢  ÊÍ: 	 Ê±ÖÓÏß¸ßÊ±Êý¾ÝÏßÏÂ½µÑØÎª¿ªÊ¼
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeStart(void)
{
	EALLOW;
    GpioDataRegs.GPBSET.bit.GPIO32 = 1;     	 //Êý¾Ý¸ß
	GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;  		//Êý¾ÝÏß±äÎªÊä³ö¿Ú
	EDIS;
	DELAY_US(DELAY_EE);
    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//Ê±ÖÓ¸ß
	DELAY_US(DELAY_EE);
	GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;		 	//Êý¾ÝµÍ
	DELAY_US(DELAY_EE);
   	GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//Ê±ÖÓµÍ
	DELAY_US(DELAY_EE);
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: EeStop
** ¹¦ÄÜÃèÊö: ½áÊø¶ÔeepromµÄ²Ù×÷
** Êä¡¡Èë: 
** Êä¡¡³ö:        
** ×¢  ÊÍ: 	 Ê±ÖÓÏß¸ßÊ±Êý¾ÝÏßÉÏÉýÑØÎª½áÊø
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeStop(void)
{
	EALLOW;
    GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;     	 //Êý¾ÝµÍ
	GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;  		//Êý¾ÝÏß±äÎªÊä³ö¿Ú
	EDIS;
	DELAY_US(DELAY_EE);
    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//Ê±ÖÓ¸ß
	DELAY_US(DELAY_EE);
    GpioDataRegs.GPBSET.bit.GPIO32 = 1;     	 //Êý¾Ý¸ß
	DELAY_US(DELAY_EE);
   	GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//Ê±ÖÓµÍ
	EALLOW;
	GpioCtrlRegs.GPBDIR.bit.GPIO32= 0;			//Êý¾ÝÏß±äÎªÊäÈë¿Ú
	EDIS;
	DELAY_US(DELAY_EE);
}

/*********************************************************************************************************
** º¯ÊýÃû³Æ: EeWrite
** ¹¦ÄÜÃèÊö: ½«number¸ö×Ö½ÚÊý¾Ý£¨²»°üÀ¨µØÖ·£©Á¬ÐøÐ´Èëµ½eeprom£¨Ò»°ãÒªÇóÔÚÍ¬Ò»Ò³Ãæ£©
** Êä¡¡Èë: 	 number,±íÊ¾ÒªÐ´µÄ×Ö½ÚÊý
** Êä¡¡³ö:        
** ×¢  ÊÍ: 	 EEPROM.data[0]:Ð´¿ØÖÆ×Ö;
**			 EEPROM.data[1-2]:´ýÐ´Êý¾ÝµØÖ·; 
**			 EEPROM.data[3-x]:´ýÐ´Êý¾Ý;
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeWrite(unsigned char number)
{
	int16 i,j;
//----------------------------------------------//·¢ËÍÆðÊ¼Î»
	EeStart();
//----------------------------------------------//¿ªÊ¼·¢ËÍÊý¾Ý
	for(j=0;j<number+3;j++)						//ÏÈ·¢µÍ×Ö½Ú
	{
		for(i=0;i<8;i++)						//Ã¿¸ö×Ö½ÚÏÈ·¢¸ß×ÖÎ»
		{
			if((EEPROM.data[j] & ONEBYTE[i])==0)	//Òª·¢0
			{
				GpioDataRegs.GPBCLEAR.bit.GPIO32 =1;	//Êý¾ÝµÍ
				DELAY_US(DELAY_EE);					
    			GpioDataRegs.GPBSET.bit.GPIO33 = 1; //Ê±ÖÓ¸ß
				DELAY_US(DELAY_EE);				
   	   		    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;//Ê±ÖÓµÍ
				DELAY_US(DELAY_EE);				
			}
			else									//Òª·¢1
			{
				GpioDataRegs.GPBSET.bit.GPIO32 =1;	//Êý¾Ý¸ß
				DELAY_US(DELAY_EE);				
   				GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//Ê±ÖÓ¸ß
				DELAY_US(DELAY_EE);				
   	   		    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//Ê±ÖÓµÍ
				DELAY_US(DELAY_EE);				
			}
		}
//----------------------------------------------//·¢ÍêÒ»¸ö×Ö½Ú		
		EALLOW;
		GpioCtrlRegs.GPBDIR.bit.GPIO32= 0;		//Êý¾ÝÏß±äÎªÊäÈë¿Ú
		EDIS;
    	GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//Ê±ÖÓ¸ß
		DELAY_US(DELAY_EE);
		
		if(GpioDataRegs.GPBDAT.bit.GPIO32==1)	//Èç¹ûÊý¾ÝÏß¶Áµ½1±íÊ¾Ã»ÓÐÓ¦´ð
		{
			M_SetFlag(SL_EE_NOACK);				//ÖÃÎÞÓ¦´ð±êÖ¾
		}
		
   	    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//Ê±ÖÓµÍ

		EALLOW;
		GpioDataRegs.GPBCLEAR.bit.GPIO32= 1;		//Êý¾ÝµÍ
		GpioCtrlRegs.GPBDIR.bit.GPIO32= 1;		//Êý¾ÝÏß±äÎªÊä³ö¿Ú
		EDIS;
		DELAY_US(DELAY_EE);
	}
//----------------------------------------------//·¢ËÍÍ£Ö¹Î»
	EeStop();
}

/*********************************************************************************************************
** º¯ÊýÃû³Æ: EeRead
** ¹¦ÄÜÃèÊö: ´ÓeepromÁ¬Ðø¶Á³önumber¸ö×Ö½ÚÊý¾Ý
** Êä¡¡Èë: 	 number,±íÊ¾Òª¶ÁµÄ×Ö½ÚÊý
** Êä¡¡³ö:   EEPROM.data[0-1]:¶Á³öµÄÊý¾Ý     
** ×¢  ÊÍ: 	 ÏÈÐ´Ð´¿ØÖÆ×Ö,ÔÙÐ´´ý¶ÁÊý¾ÝµØÖ·,ÔÙÐ´¶Á¿ØÖÆ×Ö,ÔÚ¶Á³öÊý¾Ý
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeRead(unsigned char number)
{
	int16 i,j;
	
//----------------------------------------------//·¢ËÍÆðÊ¼Î»
	EeStart();
//----------------------------------------------//¿ªÊ¼·¢ËÍµØÖ·Êý¾Ý
	for(j=0;j<3;j++)							//ÏÈ·¢µÍ×Ö½Ú
	{
		for(i=0;i<8;i++)						//Ã¿¸ö×Ö½ÚÏÈ·¢¸ß×ÖÎ»
		{
			if((EEPROM.data[j] & ONEBYTE[i])==0)	//Òª·¢0
			{
				GpioDataRegs.GPBCLEAR.bit.GPIO32 =1;	//Êý¾ÝµÍ
				DELAY_US(DELAY_EE);				
   				GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//Ê±ÖÓ¸ß
				DELAY_US(DELAY_EE);				
   	    		GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//Ê±ÖÓµÍ
				DELAY_US(DELAY_EE);				
			}
			else									//Òª·¢1
			{
				GpioDataRegs.GPBSET.bit.GPIO32 =1;	//Êý¾Ý¸ß
				DELAY_US(DELAY_EE);				
    			GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//Ê±ÖÓ¸ß
				DELAY_US(DELAY_EE);				
   	   		    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//Ê±ÖÓµÍ
				DELAY_US(DELAY_EE);				
			}
		}
//----------------------------------------------//·¢ÍêÒ»¸ö×Ö½Ú		
		EALLOW;
		GpioCtrlRegs.GPBDIR.bit.GPIO32= 0;		//Êý¾ÝÏß±äÎªÊäÈë¿Ú
		EDIS;
   	    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//Ê±ÖÓ¸ß
		DELAY_US(DELAY_EE);
		
		if(GpioDataRegs.GPBDAT.bit.GPIO32==1)	//Èç¹ûÊý¾ÝÏß¶Áµ½1±íÊ¾Ã»ÓÐÓ¦´ð
		{
			M_SetFlag(SL_EE_NOACK);				//ÖÃÎÞÓ¦´ð±êÖ¾
		}
		
   	    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//Ê±ÖÓµÍ
		
		EALLOW;
		GpioDataRegs.GPBCLEAR.bit.GPIO32 =1;		//Êý¾ÝµÍ
		GpioCtrlRegs.GPBDIR.bit.GPIO32= 1;		//Êý¾ÝÏß±äÎªÊä³ö¿Ú
		EDIS;
		
		DELAY_US(DELAY_EE);
	}
	
//----------------------------------------------//ÔÙ´Î·¢ËÍÆðÊ¼Î»
	EeStart();
//----------------------------------------------//ÔÙ´Î·¢ËÍÆðÊ¼Î»
	EEPROM.data[0] |= 0x01;						//¸ÄÎª¶ÁÖ¸Áî
	for(i=0;i<8;i++)							//ÏÈ·¢¸ß×ÖÎ»
	{
		if((EEPROM.data[0] & ONEBYTE[i])==0)		//Òª·¢0
		{
			GpioDataRegs.GPBCLEAR.bit.GPIO32 =1;		//Êý¾ÝµÍ
			DELAY_US(DELAY_EE);					
   		    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//Ê±ÖÓ¸ß
			DELAY_US(DELAY_EE);					
   	  	    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//Ê±ÖÓµÍ
			DELAY_US(DELAY_EE);					
		}
		else										//Òª·¢1
		{
			GpioDataRegs.GPBSET.bit.GPIO32 =1;		//Êý¾Ý¸ß	
			DELAY_US(DELAY_EE);					
   		    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//Ê±ÖÓ¸ß
			DELAY_US(DELAY_EE);					
   	        GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//Ê±ÖÓµÍ
			DELAY_US(DELAY_EE);					
		}
	}
//----------------------------------------------//·¢ÍêÒ»¸ö×Ö½Ú
	EALLOW;
	GpioCtrlRegs.GPBDIR.bit.GPIO32 =0;			//Êý¾ÝÏß±äÎªÊäÈë¿Ú
	EDIS;
    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//Ê±ÖÓ¸ß
	DELAY_US(DELAY_EE);
	
	if(GpioDataRegs.GPBDAT.bit.GPIO32==1)		//Èç¹ûÊý¾ÝÏß¶Áµ½1±íÊ¾Ã»ÓÐÓ¦´ð
	{
		M_SetFlag(SL_EE_NOACK);					//ÖÃÎÞÓ¦´ð±êÖ¾
	}
	
   	GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//Ê±ÖÓµÍ
	DELAY_US(DELAY_EE);
//----------------------------------------------//¿ªÊ¼¶ÁÊý¾Ý
	for(j=0;j<number;j++)						//ÏÈ¶ÁµÍ×Ö½Ú
	{
		EEPROM.data[j]=0;
		for(i=0;i<8;i++)						//Ã¿¸ö×Ö½ÚÏÈ¶Á¸ßÎ»
		{
   		    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//Ê±ÖÓ¸ß
			DELAY_US(DELAY_EE);	
			if(GpioDataRegs.GPBDAT.bit.GPIO32==1)//Êý¾ÝÎª1
			{
				EEPROM.data[j] |= ONEBYTE[i];	//ÎªÁãÔò²»±ä
			}
   	   	    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//Ê±ÖÓµÍ
			DELAY_US(DELAY_EE);
		}
//----------------------------------------------//ÊÕÍêÒ»¸ö×Ö½Ú
		if(j!=number-1)							//×îºóÒ»¸ö×Ö½Ú²»·¢³öÓ¦´ð
		{
			EALLOW;
			GpioDataRegs.GPBCLEAR.bit.GPIO32 =1;	//Êä³öµÍÀ´Ó¦´ð
			GpioCtrlRegs.GPBDIR.bit.GPIO32= 1;	//Êý¾ÝÏßÎªÊä³ö¿Ú
			EDIS;
			DELAY_US(DELAY_EE);
		}
   	    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//Ê±ÖÓ¸ß
		DELAY_US(DELAY_EE);
   	    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//Ê±ÖÓµÍ
		EALLOW;
		GpioCtrlRegs.GPBDIR.bit.GPIO32= 0;		//Êý¾ÝÏßÎªÊäÈë¿Ú
		EDIS;
		DELAY_US(DELAY_EE);		
	}
//----------------------------------------------//½ÓÊÕÍê±Ï,·¢ËÍÍ£Ö¹Î»
	EeStop();
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: EeWpre
** ¹¦ÄÜÃèÊö: ×¼±¸Ð´ÈëEEPROMµÄµØÖ·ºÍÊý¾Ý
** Êä¡¡Èë: 	 index:´ýÐ´Èë±äÁ¿µÄÐòºÅ
** Êä¡¡³ö:   EEPROM.data[0-4]:Ð´¿ØÖÆ×Ö¡¢´ýÐ´Êý¾ÝµÄµØÖ·ºÍ´ýÐ´Êý¾Ý    
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeWpre(unsigned char index)
{
	EEPROM.data[0]=0xA0;									//Slave ADdress
	EEPROM.data[1]=((index<<1)&0xFF00)>>8;					//MSB ADdress
	EEPROM.data[2]=(index<<1)&0xFF;							//LSB ADdress
	EEPROM.data[3]=*(FUNC[index].para_add) & 0x00ff;		//µÍ8Î»Êý¾Ý
	EEPROM.data[4]=(*(FUNC[index].para_add) & 0xff00)>>8;	//¸ß8Î»Êý¾Ý
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: EeRpre
** ¹¦ÄÜÃèÊö: ×¼±¸¶ÁÈ¡EEPROMÊý¾ÝµÄµØÖ·
** Êä¡¡Èë: 	 index:´ý¶ÁÈ¡±äÁ¿µÄÐòºÅ
** Êä¡¡³ö:   EEPROM.data[0-2]:¶Á¿ØÖÆ×ÖºÍ´ý¶ÁÈ¡Êý¾ÝµÄµØÖ·    
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeRpre(unsigned char index)
{
	EEPROM.data[0]=0xA0;							//Slave ADdress
	EEPROM.data[1]=((index<<1)&0xFF00)>>8;			//MSB ADdress
	EEPROM.data[2]=(index<<1)&0xFF;					//LSB ADdress
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: EeWrword
** ¹¦ÄÜÃèÊö: Ð´Ò»¸ö×ÖµÄÊý¾Ýµ½EEPROM²¢¶Á³öÀ´Ð£Ñé
** Êä¡¡Èë: 	 index:´ýÐ´±äÁ¿µÄÐòºÅ
** Êä¡¡³ö:   
** ×¢  ÊÍ: 	 ÏÈ½«´ýÐ´Êý¾ÝÐ´ÈëeepromÔÙ¶Á³öÀ´¼ìÑé
**			 Èç¹û¼ìÑé²»¶ÔÔÚÖÃ±êÖ¾Î»SL_EE_FAIL
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeWrword(unsigned char index)
{
	Uint16 data_rd;
	
	EeWpre(index);									
	EeWrite(2);											//Ð´Èë2×Ö½ÚÊý¾Ý
	EeRead(2);											//½«2×Ö½ÚÊý¾Ý¶Á³ö
	
	data_rd=(EEPROM.data[1]<<8) | EEPROM.data[0];		//data_rdÎª¶Á³öµÄÊý¾Ý
	
	if(data_rd!=*(FUNC[index].para_add))
		M_SetFlag(SL_EE_FAIL);
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: InitEeprom
** ¹¦ÄÜÃèÊö: ³õÊ¼»¯eeprom
** Êä¡¡Èë: 	 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 	 ÏÈ½«eepromÖÐµÄÔ­ÓÐÊý¾Ý¶Á³ö²¢½øÐÐ¼ìÑé
**			 Èç¹û³öÏÖÊý¾Ý´íÎóÔò½«eepromÖÐÊý¾ÝÈ«²¿³õÊ¼»¯
**			 ¼ì²âÔÚ³õÊ¼»¯¹ý³ÌÖÐÊÇ·ñ³öÏÖeeprom²Ù×÷´íÎó
**			 Èç¹ûÃ»ÓÐ´íÎóÔòÖÃSL_CODEOK±íÊ¾eeprom¹¤×÷Õý³£,·ñÔòÇåSL_CODEOK±íÊ¾eeprom¹¤×÷´íÎó
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void InitEeprom(void)
{
	Uint16 i,j;
//	Uint16 j;
	if(DEBUG_OPER==0)
	{
//----------------------------------------------//½«eepromµÄÊý¾ÝÈ«²¿¶ÁÈëram
		for(i=0;i<PARA_NUM;i++)					
		{
			EeRpre(i);								//¿ØÖÆ×ÖºÍµØÖ·ÌîÈëEEPROM.data[]
			EeRead(2);
												//¶Á³öµÄÊý¾Ý·ÅÈëRAM
			*FUNC[i].para_add=(EEPROM.data[1]<<8) | EEPROM.data[0];	
		
			if(CheckCode(i)==1)						//¼ì²é³ÌÐòµÄ·µ»ØÖµ=1Ôò±íÊ¾ÓÐ´íÎó
			{
				for(j=0;j<PARA_NUM;j++)
				{
					*FUNC[j].para_add=FUNC[j].init;	//RAMÊý¾Ý»Ö¸´³õÖµ
					EeWrword(j);					//Ð´Èë2×Ö½ÚÊý¾Ý
				}
				break;								//Êý¾ÝÐ£ÑéÓÐ´íÔòÌø³ö
			}
		}
	}
	else
	{
//-----------------------------------------//µ÷ÊÔÊ±Ö±½Ó½«EEPROM³õÊ¼»¯!!!
		for(j=0;j<PARA_NUM;j++)
		{
			*FUNC[j].para_add=FUNC[j].init;	//RAMÊý¾Ý»Ö¸´³õÖµ
			EeWrword(j);					//Ð´Èë2×Ö½ÚÊý¾Ý
		}
	}
//-----------------------------------------	
	if(M_ChkFlag(SL_EE_FAIL)==0)					
		M_SetFlag(SL_CODEOK);					//EEPROMÕý³£
	else
		M_ClrFlag(SL_CODEOK);					//EEPROM¹ÊÕÏ
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: CheckCode
** ¹¦ÄÜÃèÊö: ¼ìÑéeepromÖÐÊý¾ÝÊÇ·ñÕýÈ·
** Êä¡¡Èë: 	 index,´ý¼ìÑéÊý¾ÝµÄÐòºÅ
** Êä¡¡³ö:   j,j=1±íÊ¾Êý¾Ý´íÎó;j=0±íÊ¾Êý¾ÝÕýÈ·
** ×¢  ÊÍ: 	 ¼ìÑéÊý¾ÝÊÇ·ñÔÚÖ¸¶¨µÄ·¶Î§Ö®ÄÚ
**			 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
Uint16 CheckCode(Uint16 index)						
{
	Uint16 j,data,max,min;
	int16  temp,max_temp,min_temp;
	
	data=*FUNC[index].para_add;
//----------------------------------------------//ÅÐ¶Ï¸ÃÊý¾ÝµÄÊôÐÔ	
	if((FUNC[index].attr & MAX_PT)==1)			//¾ßÓÐ×î´óÖµÖ¸ÕëÊôÐÔ 
		max=*FUNC[index].max_add;
	else										//²»¾ßÓÐ×î´óÖµÖ¸ÕëÊôÐÔ 
		max=FUNC[index].max;					//¶Á³öÕâ¸öÁ¿µÄ×î´óÖµ
//----------------------------------------------//¾ßÓÐ×îÐ¡ÖµÖ¸ÕëÊôÐÔ			
	if((FUNC[index].attr & MIN_PT)==1)
		min=*FUNC[index].min_add;
	else 										//²»¾ßÓÐ×îÐ¡ÖµÖ¸ÕëÊôÐÔ
		min=FUNC[index].min;					//¶Á³öÕâ¸öÁ¿µÄ×îÐ¡Öµ
//----------------------------------------------//¿ªÊ¼Ð£Ñé
	j=0;										//ÖÃÎÞ¹ÊÕÏ±êÖ¾
	if((FUNC[index].attr & SIG)==0)				//ÎÞ·ûºÅÊý
	{
		if(data>max)				
			j=1;								//´óÓÚ×î´óÖµ£¬ÓÐ´í
		else if(data<min)						//Ð¡ÓÚ×îÐ¡Öµ
		{
			if((FUNC[index].attr & VA0)==0)		
				j=1;							//ÓÖ²»¾ßÓÐVA0ÊôÐÔ£¬ÓÐ´í
			else if(data!=0)					//¾ßÓÐVA0ÊôÐÔµ«ÓÖ²»ÎªÁã£¬ÓÐ´í
				j=1;
		}
	}					
	else										//ÓÐ·ûºÅÊý
	{
		temp=(int)data;							//È«²¿×ªÎªÓÐ·ûºÅÊý
		max_temp=(int)max;
		min_temp=(int)min;
		if(temp>max_temp)						//´óÓÚ×î´óÖµ£¬ÓÐ´í
			j=1;
		else if(temp<min_temp)					//Ð¡ÓÚ×îÐ¡Öµ£¬ÓÐ´í
			j=1;								//ÓÐ·ûºÅÊýÃ»ÓÐVA0ÊôÐÔ
	}
//----------------------------------------------//·µ»ØÖµÎªj	
	return j;
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: EeCtrl
** ¹¦ÄÜÃèÊö: ³ÌÐòÔËÐÐ¹ý³ÌÖÐ¶ÔeepromÊý¾ÝµÄ²Ù×÷
** Êä¡¡Èë: 	 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 	 ÔÚeepromÕýÈ·µÄÇ°ÌáÏÂ£¬¼ì²â±êÖ¾Î»
**			 SL_INIEE:		ÊÇ·ñÐèÒª½øÐÐ³õÊ¼»¯eeprom²Ù×÷;
							Èç¹ûÊÇÔòÖÃSL_EEBUSY_INIEE,È«²¿³õÊ¼»¯ÍêÒÔºóÔÙÇåSL_INIEEºÍSL_EEBUSY_INIEE
**			 SL_MCODE:		ÊÇ·ñÐèÒªÐÞ¸Ä¹¦ÄÜÂëÖµ
							Èç¹ûÊÇÔòÖÃSL_EEBUSY_MCODE,½«EEPROM.mcodeÖÐÖ¸¶¨µÄÊý¾ÝÐ´Èëeeprom
							²Ù×÷Íê³ÉºóÔÙÇåSL_MCODEºÍSL_EEBUSY_MCODE
**			 SL_ERRSAVE:	ÊÇ·ñÐèÒª±£´æ¹ÊÕÏÐÅÏ¢
							Èç¹ûÊÇÔòÖÃSL_EEBUSY_ERRSAVE,½«TAB_ERRÖÐµÄÊý¾ÝÐ´Èëeeprom
							²Ù×÷Íê³ÉºóÔÙÇåSL_ERRSAVEºÍSL_EEBUSY_ERRSAVE
**			 SL_POFSAVE:	ÊÇ·ñÐèÒª±£´æµôµçÐÅÏ¢
							Èç¹ûÊÇÔòÖÃSL_EEBUSY_POFSAVE,½«TAB_POFÖÐµÄÊý¾ÝÐ´Èëeeprom
							²Ù×÷Íê³ÉºóÔÙÇåSL_POFSAVEºÍSL_EEBUSY_POFSAVE
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeCtrl(void)
{
	if(M_ChkFlag(SL_EE_FAIL)==0)					//EEPROMÎÞ¹ÊÕÏ?
	{
//----------------------------------------------//»Ö¸´³ö³§²ÎÊý½øÐÐÖÐ
		if(M_ChkFlag(SL_EEBUSY_INI)!=0)			//RAMÊý¾Ý»Ö¸´³õÖµ
		{
			*FUNC[EEPROM.point].para_add=FUNC[EEPROM.point].init;	
			EeWrword(EEPROM.point);				//Ð´Èë2×Ö½ÚÊý¾Ý
			EEPROM.point++;
			
			if(EEPROM.point>=PARA_NUM)			//²Ù×÷Íê³É?
			{
				EEPROM.point=0;
				M_ClrFlag(SL_EEBUSY_INI);
				M_ClrFlag(SL_EEASK_INI);
			}
		}
//----------------------------------------------//ÐÞ¸Ä¹¦ÄÜÂë½øÐÐÖÐ
		else if(M_ChkFlag(SL_EEBUSY_MCODE)!=0)
		{
			EeWrword(EEPROM.mcode);				//Ð´Èë2×Ö½ÚÊý¾Ý
			M_ClrFlag(SL_EEBUSY_MCODE);
			M_ClrFlag(SL_EEASK_MCODE);
		}
//----------------------------------------------//±£´æ¹ÊÕÏÐÅÏ¢½øÐÐÖÐ
		else if(M_ChkFlag(SL_EEBUSY_ERRSAVE)!=0)
		{
			EeWrword(TAB_ERR[EEPROM.point]);	//Ð´Èë2×Ö½ÚÊý¾Ý
			EEPROM.point++;
			
			if(EEPROM.point>=ERRO_NUM)			//²Ù×÷Íê³É?
			{
				EEPROM.point=0;
				M_ClrFlag(SL_EEBUSY_ERRSAVE);
				M_ClrFlag(SL_EEASK_ERRSAVE);
			}
		}
//----------------------------------------------//ÊÇ·ñ´æÔÚ±£´æµôµçÐÅÏ¢ÇëÇó
/*
		else if(M_ChkFlag(SL_EEBUSY_POFSAVE)!=0)
		{
			EeWrword(TAB_POF[EEPROM.point]);	//Ð´Èë2×Ö½ÚÊý¾Ý
			EEPROM.point++;
			
			if(EEPROM.point>=POFF_NUM)		//²Ù×÷Íê³É?
			{
				EEPROM.point=0;
				M_ClrFlag(SL_EEBUSY_POFSAVE);
				M_ClrFlag(SL_EEASK_POFSAVE);
			}
		}		
*/
//----------------------------------------------//EEPROMÃ»ÓÐ²Ù×÷½øÐÐÖÐ
		else
		{
			EEPROM.point=0;
			if(M_ChkFlag(SL_EEASK_INI)!=0)			//ÊÇ·ñ´æÔÚ»Ö¸´³ö³§²ÎÊýÇëÇó
				M_SetFlag(SL_EEBUSY_INI);
			else if(M_ChkFlag(SL_EEASK_MCODE)!=0)	//ÊÇ·ñ´æÔÚÐÞ¸Ä¹¦ÄÜÂëÇëÇó
				M_SetFlag(SL_EEBUSY_MCODE);
			else if(M_ChkFlag(SL_EEASK_ERRSAVE)!=0)	//ÊÇ·ñ´æÔÚ±£´æ¹ÊÕÏÐÅÏ¢ÇëÇó
				{M_SetFlag(SL_EEBUSY_ERRSAVE);
//				     M_SetFlag(SL_PHASEA);          //²âÁ¿SaveÕ¼ÓÃÊ±¼ä,²âÁ¿DSP°åÉÏT1¶Ë×Ó 20090803
//    				*OUT3_ADDR = _OUT3_DATA;		//²âÁ¿SaveÕ¼ÓÃÊ±¼ä,²âÁ¿DSP°åÉÏT1¶Ë×Ó 20090803
				}
//			M_ClrFlag(SL_PHASEA);           //²âÁ¿CPUÕ¼ÓÐÂÊ,²âÁ¿DSP°åÉÏT1¶Ë×Ó
//    		*OUT3_ADDR = _OUT3_DATA;		//²âÁ¿SaveÕ¼ÓÃÊ±¼ä,²âÁ¿DSP°åÉÏT1¶Ë×Ó 20090803



//			else if(M_ChkFlag(SL_EEASK_POFSAVE)!=0)	//ÊÇ·ñ´æÔÚ±£´æµôµçÐÅÏ¢ÇëÇó
//				M_SetFlag(SL_EEBUSY_POFSAVE);
		}
	}
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: SetRtimer
** ¹¦ÄÜÃèÊö: ÊµÊ±Ê±ÖÓÉè¶¨
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
void SetRtimer(void)
{
	Uint16 temp;
	
//----------------------------------------------//Ð´ÈëÊ±¼äÖµ	
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x02;						//STATUS register
	EeWrite(1);									//Ð´ÔÊÐí
	
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x06;						//STATUS register
	EeWrite(1);									//Ð´¼Ä´æÆ÷ÔÊÐí
	
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x30;						//LSB ADdress
	
	temp=RTIMER.time[0]/10;
	EEPROM.data[3]=(RTIMER.time[0]-temp*10)|(temp<<4);	//Ãë
	
	
	temp=RTIMER.time[1]/10;
	EEPROM.data[4]=(RTIMER.time[1]-temp*10)|(temp<<4);	//·Ö
	
	temp=RTIMER.time[2]/10;
	EEPROM.data[5]=0x80 | ((RTIMER.time[2]-temp*10)|(temp<<4));	//Ê±
	
	temp=RTIMER.time[3]/10;
	EEPROM.data[6]=(RTIMER.time[3]-temp*10)|(temp<<4);	//ÈÕ
	
	temp=RTIMER.time[4]/10;
	EEPROM.data[7]=(RTIMER.time[4]-temp*10)|(temp<<4);	//ÔÂ
	
	temp=RTIMER.time[5]/10;
	EEPROM.data[8]=(RTIMER.time[5]-temp*10)|(temp<<4);	//Äê
	
	EEPROM.data[9]=0x05;						//ÐÇÆÚ
	EEPROM.data[10]=0x20;						//19/20
	EeWrite(8);									//Ð´ÈëÊ±¼äÖµ

	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x0;							//STATUS register
	EeWrite(1);									//½ûÖ¹Ð´Èë
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: RtRead
** ¹¦ÄÜÃèÊö: ÊµÊ±Ê±ÖÓ¶ÁÈ¡
** Êä¡¡Èë: 	 
** Êä¡¡³ö:   RTIMER.time[0~5]--[Ãë ·Ö Ê± ÈÕ ÔÂ Äê]
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void RtRead(void)
{
	Uint16 i;

	if(M_ChkFlag(SL_SETRTIMER)!=0)				//ÐèÒªÐ£ÕýÊµÊ±Ê±ÖÓ?
	{
		SetRtimer();
		M_ClrFlag(SL_SETRTIMER);
	}
	else
	{
		//----------------------------------------------//¶ÁÈ¡Ê±¼äÖµµ½data[0~7]
		EEPROM.data[0]=0xDE;						//Slave ADdress
		EEPROM.data[1]=0;							//MSB ADdress
		EEPROM.data[2]=0x30;						//LSB ADdress
		EeRead(8);
		//----------------------------------------------
		M_ClrBit(EEPROM.data[2],0x80);				//Çå³ý24Ð¡Ê±ÉèÖÃÎ»
	
		for(i=0;i<6;i++)
		{
			RTIMER.time[i]=((EEPROM.data[i] & 0xF0)>>4)*10+(EEPROM.data[i] & 0x0F);
		}
	}
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: InitRtimer
** ¹¦ÄÜÃèÊö: ÊµÊ±Ê±ÖÓ³õÊ¼»¯
** Êä¡¡Èë: 	 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 	 Ö»ÔÚÊµÊ±Ê±ÖÓ³õÊ¼»¯µÄÊ±ºò½«ÐèÒªÉè¶¨µÄÊ±¼äÖµÉè¶¨µ½ÏàÓ¦Î»ÖÃ
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void InitRtimer(void)
{
//----------------------------------------------//Ð´Èë¿ØÖÆ×Ö
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x0;							//STATUS register
	EeWrite(1);									//½ûÖ¹Ð´Èë

	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x02;						//STATUS register
	EeWrite(1);									//Ð´ÔÊÐí

	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x06;						//STATUS register
	EeWrite(1);									//Ð´¼Ä´æÆ÷ÔÊÐí

	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x10;						//LSB ADdress
	EEPROM.data[3]=0x0;							//Control register 0
	EEPROM.data[4]=0x0;							//Control register 1
	EEPROM.data[5]=0x0;							//Control register 2
	EEPROM.data[6]=0x0;							//Control register 3
	EeWrite(4);									//Ð´Èë4×Ö½Ú¿ØÖÆ×Ö
	
	DELAY_US(10000L);							//ÑÓÊ±10ms

//----------------------------------------------//Ð´ÈëÊ±¼äÖµ	
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x0;							//STATUS register
	EeWrite(1);									//½ûÖ¹Ð´Èë

	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x02;						//STATUS register
	EeWrite(1);									//Ð´ÔÊÐí
	
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x06;						//STATUS register
	EeWrite(1);									//Ð´¼Ä´æÆ÷ÔÊÐí
	
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x30;						//LSB ADdress
	EEPROM.data[3]=0;							//Ãë
	EEPROM.data[4]=0x00;						//·Ö
	EEPROM.data[5]=0x80 | 0x00;					//Ê±(0x80Ö¸µÄÊÇ24Ð¡Ê±Ä£Ê½)
	EEPROM.data[6]=0x15;						//ÈÕ
	EEPROM.data[7]=0x04;						//ÔÂ
	EEPROM.data[8]=0x09;						//Äê
	EEPROM.data[9]=0x05;						//ÐÇÆÚ
	EEPROM.data[10]=0x20;						//19/20
	EeWrite(8);									//Ð´ÈëÊ±¼äÖµ

	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x0;							//STATUS register
	EeWrite(1);									//½ûÖ¹Ð´Èë
	
//----------------------------------------------//¶ÁÈ¡Ê±¼äÖµµ½data[0~7]
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x30;						//LSB ADdress
	EeRead(8);
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: Sci485_TxInit
** ¹¦ÄÜÃèÊö: 485·¢ËÍ³õÊ¼»¯
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
void Sci485_TxInit(void)
{
	Uint32 temp32;
	Uint16 temp16;
	
	M_EN485TXD();
	M_EnTxb();
	temp32=LSPCLK/8;
	temp16=temp32/_SCIB_BAUD-1;				// LSPCLK/(8*BAUD)-1
//----------------------------------------------------------------------------
	SciaRegs.SCIFFTX.all=0xC000;			// Reset TX FIFO's
	SciaRegs.SCICCR.all = 0x0007;			// 1 stop bit, No parity, 8-bit character, No loopback
	
	SciaRegs.SCIHBAUD = (temp16&0xFF00)>>8;	// BAUDRATE
	SciaRegs.SCILBAUD = temp16&0x00FF;
	
	SciaRegs.SCIFFTX.bit.TXFIFOXRESET=1;	// Re-enable TX FIFO operation
	
	//M_EnTxbInt();
	SciaRegs.SCICTL1.all =0x0022;     		// Relinquish SCI from Reset
//----------------------------------------------------------------------------
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: Sci485_RxInit
** ¹¦ÄÜÃèÊö: 485½ÓÊÕ³õÊ¼»¯
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
void Sci485_RxInit(void)
{
//----------------------------------------------------------------------------
	M_EN485RXD();
	M_EnRxb();
	SciaRegs.SCIFFRX.all=0x000A;			// Reset RX FIFO's
	//ScibRegs.SCICCR.all = 0x0007;			// 1 stop bit, No parity, 8-bit character, No loopback
	
	SciaRegs.SCIFFRX.bit.RXFIFORESET=1;		// Re-enable RX FIFO operation
	
	M_ClrRxFifoOvr();
	
	//M_EnRxbInt();
	SciaRegs.SCICTL1.all =0x0021;     		// Relinquish SCI from Reset
//----------------------------------------------------------------------------
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: ScibDatpro
** ¹¦ÄÜÃèÊö: ¶Ôsci½ÓÊÕµ½µÄÊý¾Ý½øÐÐ½âÎöÐ£Ñé
** Êä¡¡Èë: 	 
** Êä¡¡³ö:   response=0±íÊ¾ÐèÒª²»ÐèÒª»Ø¸´;response=1±íÊ¾ÐèÒªÁ¢¼´»Ø¸´;response=2±íÊ¾²»Á¢¼´»Ø¸´,½ø³ÌÍêºóÔÙ»Ø¸´(Èç»Ö¸´³ö³§²ÎÊý)
** ×¢  ÊÍ: 	 Í¨ÐÅÐ­ÒéËµÃ÷ÈçÏÂ
				SCI.rxb[0]:±¨Í·(0x7E)
				SCI.rxb[1]:ÏÂÎ»»úµØÖ·
				SCI.rxb[2]:ÃüÁî×Ö(µÍ×Ö½Ú)
				SCI.rxb[3]:ÃüÁî×Ö(¸ß×Ö½Ú)
				SCI.rxb[4]:¹¦ÄÜÂëÐòºÅ
				SCI.rxb[5]:¹¦ÄÜÂëÊýÖµ(µÍ×Ö½Ú)
				SCI.rxb[6]:¹¦ÄÜÂëÊýÖµ(¸ß×Ö½Ú)
				SCI.rxb[7]:×´Ì¬×Ö(µÍ×Ö½Ú)
				SCI.rxb[8]:×´Ì¬×Ö(¸ß×Ö½Ú)
				SCI.rxb[9]:Òì»òÐ£Ñé
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
Uint16 ScibDatpro(void)
{
	Uint16 response,i,xor_data,opration_fail;
	Uint16 rx_command,rx_funcnum,rx_funcval,rx_state;
	Uint16 tx_state;
	
	if(SCI.rxb[0] == COM_HEAD)									//±¨Í·=COM_HEAD?
	{
		//·¢ËÍÊý×¼±¸(±¨Í·×Ö½Ú µØÖ·×Ö½Ú)
		SCI.txb[0] = COM_HEAD;									//±¨Í·×Ö½Ú
		SCI.txb[1] = SCI.rxb[1];								//µØÖ·×Ö½Ú
		
		//µØÖ·ÅÐ¶Ï
		if(((SCI.rxb[1]&COM_OPERATOR)!=COM_OPERATOR)&&(SCI.rxb[1]!=_SCIB_ADDR))		//ÉÏÎ»»ú&½ÚµãµØÖ·²»·û?
			response=0;											//·Ç·¢ËÍ¸ø±¾»úµÄÃüÁî,²»½øÐÐ´¦Àí»Ø¸´,µÈ´ýÏÂÒ»Ö¡Êý¾Ý
		else													//²Ù×÷Æ÷¿ØÖÆ»òÉÏÎ»»ú¿ØÖÆÇÒ½ÚµãµØÖ·Ïà·û
		{
			if(SCI.rxb[1]==0)									//¹ã²¥Ä£Ê½?
				response=0;										//¹ã²¥Ä£Ê½²»»Ø¸´
			else
				response=1;										//Á¢¼´»Ø¸´
			
			xor_data=SCI.rxb[0];								//Òì»òÐ£Ñé
			for(i=1;i<9;i++)
				xor_data ^= SCI.rxb[i];
			
			if(xor_data != SCI.rxb[9])							//Òì»òÐ£Ñé·ûºÏ?
				opration_fail=1;								//ÖÃ²Ù×÷Ê§°Ü±êÖ¾
			else
			{
				opration_fail=0;								//Çå²Ù×÷Ê§°Ü
				
				//ÓÕÊý¾Ý×ª´æ
				rx_command=SCI.rxb[3];							//±£´æÃüÁî×Ö
				rx_command=((rx_command<<8)&0xFF00)|SCI.rxb[2];
				
				rx_funcnum=SCI.rxb[4];							//±£´æ¹¦ÄÜÂëÐòºÅ
				
				rx_funcval=SCI.rxb[6];							//±£´æ¹¦ÄÜÂëÖµ
				rx_funcval=(SCI.rxb[6]<<8)|SCI.rxb[5];
				
				rx_state=SCI.rxb[8];							//±£´æ×´Ì¬×Ö
				rx_state=(SCI.rxb[8]<<8)|SCI.rxb[7];
				
				if((rx_state&COM_KEYFWD)==COM_KEYFWD)			//ÉÏÎ»»úÒªÇó
				{
//					M_SetFlag();						//ÖÃÆô¶¯±êÖ¾
				}
			    if((rx_state&COM_KEYREV)==COM_KEYREV)		    //ÉÏÎ»»úÒªÇó
				{
//					M_SetFlag();						//ÖÃÆô¶¯±êÖ¾
				}
				if((rx_state&COM_KEYSTOP)==COM_KEYSTOP)			//ÉÏÎ»»úÒªÇóÍ£Ö¹?
				{              	    			
//                    M_ClrFlag();	
				}

				//ÃüÁî×Ö´¦Àí
				//if((((rx_command&COM_NEEDSTOP)==COM_NEEDSTOP)&&(M_ChkFlag(SL_RUNING)!=0))||(rx_funcnum>PARA_NUM)||((rx_state&COM_UPNORM)!=COM_UPNORM))	//ÃüÁîÐèÒªÍ£»ú&ÕýÔÚÔËÐÐ »ò ¹¦ÄÜÂëÐòºÅ³¬ÏÞ »ò ÉÏÎ»»ú²»Õý³£?
				if((((rx_command&COM_NEEDSTOP)==COM_NEEDSTOP)&&(M_ChkFlag(SL_RUN)!=0))||(rx_funcnum>PARA_NUM)||((rx_state&COM_UPNORM)!=COM_UPNORM))	//ÃüÁîÐèÒªÍ£»ú&ÕýÔÚÔËÐÐ »ò ¹¦ÄÜÂëÐòºÅ³¬ÏÞ »ò ÉÏÎ»»ú²»Õý³£? Modified by ljd 05-12-7
					opration_fail=1;							//ÖÃ²Ù×÷Ê§°Ü±êÖ¾
				else
				{
					switch (rx_command&0xFFEF)					//ÃüÁî½âÎö
					{
						case COM_RD:							//¶ÁÈ¡¹¦ÄÜÂë(¶ÁÈ¡ÏÂÎ»»úEEPROMÖÐµÄ¹¦ÄÜÂëÖµÐ´Èëµ½ÉÏÎ»»úRAMÖÐ)
							if((rx_funcnum>=BANK_FIRST)&&(rx_funcnum<=BANK_END))	//ÊôÓÚ¼à¿Ø±äÁ¿?
							{
								i=*FUNC[rx_funcnum].para_add;			//Ö±½Ó½«RAMÖÐµÄÊý¾Ý·¢ËÍ
								EEPROM.data[0]=i&0x00FF;
								EEPROM.data[1]=(i&0xFF00)>>8;
							}
//							else if((rx_funcnum>=TAB_ERR_FIRST)&&(rx_funcnum<=TAB_ERR_END))	//ÊôÓÚ¹ÊÕÏ±äÁ¿?
//							{
//								i=*FUNC[rx_funcnum].para_add;			//Ö±½Ó½«RAMÖÐµÄÊý¾Ý·¢ËÍ
//								EEPROM.data[0]=i&0x00FF;
//								EEPROM.data[1]=(i&0xFF00)>>8;
//							}
							else										//ÆÕÍ¨±äÁ¿
							{
								EeRpre(rx_funcnum);						//¿ØÖÆ×ÖºÍµØÖ·ÌîÈëEEPROM.data[]
								EeRead(2);								//¶Á³öÁ½×Ö½Ú
								*FUNC[rx_funcnum].para_add=(EEPROM.data[1]<<8) | EEPROM.data[0];	//ÐÞ¸ÄRAMÖÐ¹¦ÄÜÂëÊý¾Ý
							}
							
							SCI.txb[2]=SCI.rxb[2];						//×¼±¸·¢ËÍÊý¾Ý(²Ù×÷³É¹¦)
							SCI.txb[3]=SCI.rxb[3]|COM_SUCCESS;
							SCI.txb[4]=SCI.rxb[4];
							SCI.txb[5]=EEPROM.data[0];
							SCI.txb[6]=EEPROM.data[1];
							
							break;
							
						case COM_WR:									//ÐÞ¸Ä¹¦ÄÜÂë(ÐÞ¸ÄÏÂÎ»»úRAMÖÐµÄ¹¦ÄÜÂëÖµ)
							//if(((FUNC[rx_funcnum].attr&RDO)==RDO)||(((FUNC[rx_funcnum].attr&WR1)==WR1)&&(M_ChkFlag(SL_RUNING)!=0)))		//Ö»¶Á»òÔËÐÐÖÐ²»¿ÉÐÞ¸Ä?
							if(((FUNC[rx_funcnum].attr&RDO)==RDO)||(((FUNC[rx_funcnum].attr&WR1)==WR1)&&(M_ChkFlag(SL_RUN)!=0)))		//Ö»¶Á»òÔËÐÐÖÐ²»¿ÉÐÞ¸Ä? Modified by ljd 05-12-7
							{
								opration_fail=1;						//ÖÃ²Ù×÷Ê§°Ü±êÖ¾
							}
							else
							{
								*FUNC[rx_funcnum].para_add=rx_funcval;	//ÐÞ¸ÄRAMRAMÖÐµÄ¹¦ÄÜÂëÖµ
								
								SCI.txb[2]=SCI.rxb[2];					//×¼±¸·¢ËÍÊý¾Ý(²Ù×÷³É¹¦)
								SCI.txb[3]=SCI.rxb[3]|COM_SUCCESS;
								SCI.txb[4]=SCI.rxb[4];
								SCI.txb[5]=SCI.rxb[5];
								SCI.txb[6]=SCI.rxb[6];
							}
							
							break;
							
						case COM_SAVE:									//ÐÞ¸Ä¹¦ÄÜÂë²¢´æ´¢(ÐÞ¸ÄÏÂÎ»»úRAMÖÐµÄ¹¦ÄÜÂëÖµ²¢±£´æµ½ÏÂÎ»»úµÄEEPROM)
							//if(((FUNC[rx_funcnum].attr&RDO)==RDO)||(((FUNC[rx_funcnum].attr&WR1)==WR1)&&(M_ChkFlag(SL_RUNING)!=0)))		//Ö»¶Á»òÔËÐÐÖÐ²»¿ÉÐÞ¸Ä?
							if(((FUNC[rx_funcnum].attr&RDO)==RDO)||(((FUNC[rx_funcnum].attr&WR1)==WR1)&&(M_ChkFlag(SL_RUN)!=0)))		//Ö»¶Á»òÔËÐÐÖÐ²»¿ÉÐÞ¸Ä? Modified by ljd 05-12-7
								opration_fail=1;						//ÖÃ²Ù×÷Ê§°Ü±êÖ¾
							else
							{
								*FUNC[rx_funcnum].para_add=rx_funcval;	//ÐÞ¸ÄRAMÖÐµÄ¹¦ÄÜÂëÖµ
								
								if((rx_funcnum>=TIME_FIRST)&&(rx_funcnum<=TIME_END))
									M_SetFlag(SL_SETRTIMER);			//ÉèÊµÊ±Ê±ÖÓÐèÒªÖØÐÂÉèÖÃ±êÖ¾
								else
								{
									EEPROM.mcode=rx_funcnum;			//Ð´Èë2×Ö½ÚÊý¾Ý
									M_SetFlag(SL_EEASK_MCODE);			//ÉèEEPROMÐÞ¸Ä¹¦ÄÜÂëÇëÇó±êÖ¾
								}
								
								SCI.txb[2]=SCI.rxb[2];					//×¼±¸·¢ËÍÊý¾Ý(²Ù×÷³É¹¦)
								SCI.txb[3]=SCI.rxb[3]|COM_SUCCESS;
								SCI.txb[4]=SCI.rxb[4];
								SCI.txb[5]=SCI.rxb[5];
								SCI.txb[6]=SCI.rxb[6];
							}
							
							break;
							
						case COM_RESUME:								//»Ö¸´³ö³§²ÎÊý
						//	if(rx_funcval==RESUME_KEY)					//»Ö¸´³ö³§²ÎÊýÐ£ÑéÂëÕýÈ·?
						//	{
								M_SetFlag(SL_RESUME);					//Éè»Ö¸´³ö³§²ÎÊý½øÐÐÖÐ±êÖ¾
								
								EEPROM.point=0;							//Ð´Èë2×Ö½ÚÊý¾Ý
								M_SetFlag(SL_EEASK_INI);				//ÉèEEPROMÐÞ¸Ä¹¦ÄÜÂëÇëÇó±êÖ¾
								
								SCI.txb[2]=SCI.rxb[2];					//×¼±¸·¢ËÍÊý¾Ý(²Ù×÷³É¹¦)
								SCI.txb[3]=SCI.rxb[3]|COM_SUCCESS;
								SCI.txb[4]=SCI.rxb[4];
								SCI.txb[5]=SCI.rxb[5];
								SCI.txb[6]=SCI.rxb[6];
								
								response=2;								//»Ö¸´³ö³§²ÎÊý²»Á¢¼´»Ø¸´,»Ö¸´½øÐÐÍê²Å»Ø¸´
						//	}
						//	else
						//		opration_fail=1;						//ÖÃ²Ù×÷Ê§°Ü±êÖ¾
							
							break;
							
						case COM_SAVEALL:								//ÐÞ¸Ä¹¦ÄÜÂë²¢´æ´¢(°üÀ¨Ö»¶Á¹¦ÄÜÂë)
							*FUNC[rx_funcnum].para_add=rx_funcval;		//ÐÞ¸ÄRAMÖÐµÄ¹¦ÄÜÂëÖµ
							
							EEPROM.mcode=rx_funcnum;					//Ð´Èë2×Ö½ÚÊý¾Ý
							M_SetFlag(SL_EEASK_MCODE);					//ÉèEEPROMÐÞ¸Ä¹¦ÄÜÂëÇëÇó±êÖ¾
							
							SCI.txb[2]=SCI.rxb[2];						//×¼±¸·¢ËÍÊý¾Ý(²Ù×÷³É¹¦)
							SCI.txb[3]=SCI.rxb[3]|COM_SUCCESS;
							SCI.txb[4]=SCI.rxb[4];
							SCI.txb[5]=SCI.rxb[5];
							SCI.txb[6]=SCI.rxb[6];
							
							break;
/*						
						case COM_VGIVE:									//ÐÞ¸ÄDCDCµçÑ¹¸ø¶¨Öµ
							if((rx_funcnum == GIVE_FIRST)&&(rx_funcval<=FUNC[NO_VCER].max))
							{
								_DC_VREF2 = rx_funcval;
								SCI.txb[2]=SCI.rxb[2];					//×¼±¸·¢ËÍÊý¾Ý(²Ù×÷³É¹¦)
							    SCI.txb[3]=SCI.rxb[3]|COM_SUCCESS;
								SCI.txb[4]=SCI.rxb[4];
								SCI.txb[5]=SCI.rxb[5];
								SCI.txb[6]=SCI.rxb[6];
							}
							else
							{
								opration_fail=1;						//ÖÃ²Ù×÷Ê§°Ü±êÖ¾
							}
							
							break;
						
						case COM_IGIVE:									//ÐÞ¸ÄDCDCµçÁ÷¸ø¶¨Öµ
							if((rx_funcnum == (GIVE_FIRST+1))&&(rx_funcval<=FUNC[NO_ICER].max))
							{
								_DC_IREF2 = rx_funcval;
								SCI.txb[2]=SCI.rxb[2];					//×¼±¸·¢ËÍÊý¾Ý(²Ù×÷³É¹¦)
								SCI.txb[3]=SCI.rxb[3]|COM_SUCCESS;
								SCI.txb[4]=SCI.rxb[4];
								SCI.txb[5]=SCI.rxb[5];
								SCI.txb[6]=SCI.rxb[6];
							}
							else
							{
								opration_fail=1;						//ÖÃ²Ù×÷Ê§°Ü±êÖ¾
							}
							
							break;
*/								
						default:
							SCI.txb[2]=SCI.rxb[2];						//×¼±¸·¢ËÍÊý¾Ý(²Ù×÷³É¹¦)
							SCI.txb[3]=SCI.rxb[3]|COM_SUCCESS;
							SCI.txb[4]=SCI.rxb[4];
							SCI.txb[5]=SCI.rxb[5];
							SCI.txb[6]=SCI.rxb[6];
					}
				}
				
				//²Ù×÷Ê§°ÜµÄ´¦Àí
				if(opration_fail==1)									//²Ù×÷Ê§°Ü?
				{
					SCI.txb[3]=SCI.rxb[3];								//×¼±¸·¢ËÍÊý¾Ý(²Ù×÷Ê§°Ü)
					SCI.txb[4]=SCI.rxb[4];
					SCI.txb[5]=SCI.rxb[5];
					SCI.txb[6]=SCI.rxb[6];
				}
				
				//ÏÂÎ»»ú·´À¡×´Ì¬×Ö´¦Àí
				tx_state=0;
				
				if(M_ChkFlag(SL_ERROR)==0)								//¹¤×÷ÖÐÎÞ¹ÊÕÏ?
					tx_state |= COM_SLAVENORM;							//ÉèÏÂÎ»»úÕý³£Î»
				if(M_ChkFlag(SL_CODEOK)!=0)
					tx_state |= COM_SLAVEINIT;							//ÉèÏÂÎ»»ú³õÊ¼»¯Íê³ÉÎ»
				if(M_ChkFlag(SL_RUN)!=0)								//SL_RUN×÷Îª×ÜÔËÐÐ±êÖ¾Î»
					tx_state |= COM_SLAVERUN;							//ÉèÏÂÎ»»úÔËÐÐÎ»
//				if(_WORK_MODE==1)										//³äµç
//					tx_state |= COM_SLAVEFWD;							
//				if(_WORK_MODE==2)										//·Åµç
//					tx_state |= COM_SLAVEREV;							
//				if(_WORK_MODE==3)										
//					tx_state |= COM_SLAVEDEB;							//µ÷ÊÔ
				
				SCI.txb[7]=tx_state&0x00FF;
				SCI.txb[8]=(tx_state&0xFF00)>>8;
				
				//Òì»òÐ£Ñé×Ö½Ú
				xor_data=SCI.txb[0];									//Òì»òÐ£Ñé
				for(i=1;i<9;i++)
					xor_data ^= SCI.txb[i];
				
				SCI.txb[9] = xor_data;
			}
		}
	}
	
	return response;
}

/*********************************************************************************************************
** º¯ÊýÃû³Æ: Sci485Ctrl
** ¹¦ÄÜÃèÊö: ¶ÔsciµÄ½ÓÊÕ·¢ËÍ½øÐÐ×ÛºÏ¿ØÖÆ
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
void Sci485Ctrl(void)
{
	Uint16 i,response;
	
	if(M_ChkCounter(SCI.cnt_sciover,DELAY_SCIOVER)>=0)				//·¢ËÍ/½ÓÊÕ³¬Ê±?
	{
		SCI.cnt_sciover=0;
		M_ClrFlag(SL_TX);
		M_ClrFlag(SL_RX);
		Sci485_RxInit();											//485½ÓÊÕ³õÊ¼»¯
	}
	else if(M_ChkFlag(SL_TX)!=0)									//·¢ËÍ?
	{
		if(SciaRegs.SCIFFTX.bit.TXFFST == 0)						//·¢ËÍÍê³É?
		{
			SCI.cnt_sciover=0;										//Çå³ý·¢ËÍ/½ÓÊÕ³¬Ê±¶¨Ê±Æ÷
			if(SciaRegs.SCICTL2.bit.TXEMPTY==1)						//·¢ËÍ¼Ä´æÆ÷Îª¿Õ?
		 	{
				M_ClrFlag(SL_TX);
				Sci485_RxInit();									//485½ÓÊÕ³õÊ¼»¯
			}
		}
	}
	else															//½ÓÊÕ
	{
		if(SciaRegs.SCIFFRX.bit.RXFFST == 0)						//Î´¿ªÊ¼½ÓÊÕ»ò½ÓÊÕÍê³É?
		{
			SCI.cnt_sciover=0;										//Çå³ý·¢ËÍ/½ÓÊÕ³¬Ê±¶¨Ê±Æ÷
			M_ClrRxFifoOvr();
			
			if(M_ChkFlag(SL_RX)!=0)									//½ÓÊÕÍê³É?
			{
				if(M_ChkCounter(SCI.cnt_scispace,DELAY_SCISPACE)>=0)//½ÓÊÕµ½·¢ËÍ¼ä¸ôµ½Ê±?
	    		{
					if(M_ChkFlag(SL_RESUME)!=0)						//»Ö¸´³ö³§²ÎÊý½øÐÐÖÐ?
					{
						if(M_ChkFlag(SL_EEASK_INI)==0)				//EEPROMÐÞ¸Ä¹¦ÄÜÂë²Ù×÷Íê³É?
						{
							M_ClrFlag(SL_RESUME);					//Çå»Ö¸´³ö³§²ÎÊý½øÐÐÖÐ±êÖ¾
							M_SetFlag(SL_TX);						//ÖÃ·¢ËÍÈÎÎñ±êÖ¾Î»
							M_ClrFlag(SL_RX);
							
							Sci485_TxInit();						//485·¢ËÍ³õÊ¼»¯
							
							for(i=0;i<SCI485NUM;i++)
								SciaRegs.SCITXBUF=SCI.txb[i];
						}
					}
					else
					{
						response=ScibDatpro();						//µ÷ÓÃÊý¾Ý½âÎö³ÌÐò
						
						if(response==1)								//±íÊ¾ÒªÁ¢¼´»Ø¸´
						{
							M_SetFlag(SL_TX);						//ÖÃ·¢ËÍÈÎÎñ±êÖ¾Î»
							M_ClrFlag(SL_RX);
							
							Sci485_TxInit();						//485·¢ËÍ³õÊ¼»¯
							
							//for(i=0;i<SCI485NUM;i++)				//·¢ËÍ»º´æµÈÓÚ½ÓÊÕ»º´æ(µ÷ÊÔÓÃ)
							//	SCI.txb[i]=SCI.rxb[i];
							
							for(i=0;i<SCI485NUM;i++)
								SciaRegs.SCITXBUF=SCI.txb[i];
						}
						else if(response==0)						//²»ÐèÒª»Ø¸´
						{
							M_ClrFlag(SL_RX);
							Sci485_RxInit();						//485½ÓÊÕ³õÊ¼»¯
						}
					}
				}
			}
		}
		else if((SciaRegs.SCIFFRX.bit.RXFFST >= 1)&&(M_ChkFlag(SL_HEADOK)==0))	//¿ªÊ¼½ÓÊÕÇÒ»¹Ã»ÓÐÊÕµ½±¨Í·?
		{
			SCI.rxb[0]=SciaRegs.SCIRXBUF.all&0x00FF;
			if(SCI.rxb[0]==COM_HEAD)
				M_SetFlag(SL_HEADOK);
			else
				Sci485_RxInit();									//485½ÓÊÕ³õÊ¼»¯
		}
		else if(SciaRegs.SCIFFRX.bit.RXFFST >= SCI485NUM-1)			//½ÓÊÕÍê³É?
		{
			SCI.cnt_sciover=0;										//Çå³ý·¢ËÍ/½ÓÊÕ³¬Ê±¶¨Ê±Æ÷
			M_DisTxRxb();
			M_ClrRxFifoOvr();
			
			for(i=1;i<SCI485NUM;i++)								//¶Á³ö½ÓÊÕ»º´æ(²»°üÀ¨±¨Í·)
				SCI.rxb[i]=SciaRegs.SCIRXBUF.all&0x00FF;
			
			M_ClrFlag(SL_HEADOK);
			M_SetFlag(SL_RX);										//ÖÃ½ÓÊÕÍê³É±êÖ¾Î»
			SCI.cnt_scispace=0;										//Çå³ý½ÓÊÕµ½·¢ËÍ¼ä¸ô¶¨Ê±Æ÷
		}
	}
}

/*********************************************************************************************************
** º¯ÊýÃû³Æ: Sci_canopenrx
** ¹¦ÄÜÃèÊö: sci_CANOPEN³õÊ¼»¯
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
void Scicanopen_Init(void)
{
//----------------------------------------------------------------------------
	ScibRegs.SCICCR.all =0x0007;   			// 1 stop bit,No loopback 
                                  			// no parity,8 char bits
	ScibRegs.SCICTL1.all =0x0003;  			// Disable TX, RX, internal SCICLK, 
                                  			// Disable RX_ERR, SLEEP, TXWAKE
	ScibRegs.SCICTL2.all =0;		 		// fifo mode,they are ignored 

   	ScibRegs.SCIFFTX.all=0xC000;			// disable tx232_int,reset scia_fifo 
    ScibRegs.SCIFFRX.all=0x000A;			// disable rx232_int 
    ScibRegs.SCIFFCT.all=0x00;
    
	ScibRegs.SCICTL1.all =0x0023;     		// Relinquish SCI from Reset 

	ScibRegs.SCIFFTX.bit.TXFIFOXRESET=1;
	ScibRegs.SCIFFRX.bit.RXFIFORESET=1;
//----------------------------------------------------------------------------
}  
/*********************************************************************************************************
** º¯ÊýÃû³Æ: Sci_canopenrx
** ¹¦ÄÜÃèÊö: ¶Ôsci_CANOPENµÄ½ÓÊÕ·¢ËÍ½øÐÐ×ÛºÏ¿ØÖÆ
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
void Sci_canopenrx(void)
{
	Uint16 i,heartbeat,xor_data;
	
	if(M_ChkCounter(SCI_canopen.cnt_sciover,DELAY_SCICANOPENOVER)>=0)				//½ÓÊÕ³¬Ê±?
	{
		SCI_canopen.cnt_sciover=0;
		Scicanopen_Init();
	}
	else
	{
		if((ScibRegs.SCIFFRX.bit.RXFFST >= 1)&&(M_ChkFlag(SL_CANOPENHEADOK)==0))	//¿ªÊ¼½ÓÊÕÇÒ»¹Ã»ÓÐÊÕµ½±¨Í·?
		{
			SCI_canopen.cnt_sciover=0;

			SCI_canopen.rxb[0]=ScibRegs.SCIRXBUF.all&0x00FF;
			if(SCI_canopen.rxb[0]==CANOPENCOM_HEAD)
				M_SetFlag(SL_CANOPENHEADOK);
			else
			{	
				ScibRegs.SCIFFRX.bit.RXFIFORESET=1;		// Re-enable RX FIFO operation
				ScibRegs.SCIFFRX.bit.RXFFOVRCLR=1;
			}
		}
		else if(ScibRegs.SCIFFRX.bit.RXFFST >= SCICANOPENRXNUM-1)			//½ÓÊÕÍê³É?
		{
			
			//	ScibRegs.SCICTL1.bit.RXENA =0;
				for(i=1;i<SCICANOPENRXNUM;i++)								//¶Á³ö½ÓÊÕ»º´æ(²»°üÀ¨±¨Í·)
					SCI_canopen.rxb[i]=ScibRegs.SCIRXBUF.all&0x00FF;
				M_ClrFlag(SL_CANOPENHEADOK);
				
				heartbeat= SCI_canopen.rxb[2] & ONEBYTE[0];
				if(heartbeat!=SCI_canopen.heartbeat)
					SCI_canopen.cnt_heartbeat=0;
				
				SCI_canopen.heartbeat = heartbeat;

				xor_data=SCI_canopen.rxb[0];								//Òì»òÐ£Ñé
				for(i=1;i<(SCICANOPENRXNUM-1);i++)
					xor_data ^= SCI_canopen.rxb[i];
			
				if(xor_data == SCI_canopen.rxb[SCICANOPENRXNUM-1])							//Òì»òÐ£Ñé·ûºÏ?
				{
					if((SCI_canopen.rxb[1]|SCI_canopen.rxb[2]|SCI_canopen.rxb[3]|SCI_canopen.rxb[4]|SCI_canopen.rxb[5]|SCI_canopen.rxb[6]|SCI_canopen.rxb[7]|SCI_canopen.rxb[8])!=0) //ÌÞ³öÊý¾Ý¶¼µÈÓÚ0µÄ»µ°ü20090817
					{			
					//Êý¾Ý×ª´æ
					SCI_canopen.rx_controlword=(SCI_canopen.rxb[2]<<8)|SCI_canopen.rxb[1];							//controlword
				
					SCI_canopen.rx_torque=(SCI_canopen.rxb[4]<<8)|SCI_canopen.rxb[3];							//torque_ref
				
					SCI_canopen.rx_angle=(SCI_canopen.rxb[6]<<8)|SCI_canopen.rxb[5];							//angle_ref
					}	
					
					if(M_ChkFlag(SL_CANOPENOVER)!=0)	//201105atzuoyun
					{
			 	  		 SCI_canopen.rx_controlword=0;
						 SCI_canopen.rx_torque=0;
						 SCI_canopen.rx_angle=0; 
					}						
				}

		//		ScibRegs.SCICTL1.bit.RXENA =1;
				ScibRegs.SCIFFRX.bit.RXFIFORESET=1;		// Re-enable RX FIFO operation
				ScibRegs.SCIFFRX.bit.RXFFOVRCLR=1;
				SCI_canopen.cnt_sciover=0;

		}
	}
} 

/*********************************************************************************************************
** º¯ÊýÃû³Æ: Sci_canopenrx
** ¹¦ÄÜÃèÊö: ¶Ôsci_CANOPENµÄ½ÓÊÕ·¢ËÍ½øÐÐ×ÛºÏ¿ØÖÆ
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
void Sci_canopentx(void)
{
	Uint16 i,xor_data;

//if(SCI_canopen.tx_state1!=0)  SCI_canopen.tx_state1=0x0000;   //cantest
//else SCI_canopen.tx_state1=0xFFFF;                            //cantest
	
		SCI_canopen.txb[0] = CANOPENCOM_HEAD;
//		SCI_canopen.txb[1] = CANOPENCOM_TX1;
		SCI_canopen.txb[1]=SCI_canopen.tx_torque&0x00FF;
		SCI_canopen.txb[2]=(SCI_canopen.tx_torque&0xFF00)>>8;
		SCI_canopen.txb[3]=SCI_canopen.tx_speed&0x00FF;
		SCI_canopen.txb[4]=(SCI_canopen.tx_speed&0xFF00)>>8;
		SCI_canopen.txb[5]=SCI_canopen.tx_state1&0x00FF;
		SCI_canopen.txb[6]=(SCI_canopen.tx_state1&0xFF00)>>8;
		SCI_canopen.txb[7]=SCI_canopen.tx_state2&0x00FF;
		SCI_canopen.txb[8]=(SCI_canopen.tx_state2&0xFF00)>>8;
		SCI_canopen.txb[9]=SCI_canopen.tx_watertempin&0x00FF;
		SCI_canopen.txb[10]=(SCI_canopen.tx_watertempin&0xFF00)>>8;
		SCI_canopen.txb[11]=SCI_canopen.tx_watertempout&0x00FF;
		SCI_canopen.txb[12]=(SCI_canopen.tx_watertempout&0xFF00)>>8;
		SCI_canopen.txb[13]=SCI_canopen.tx_skiiptempmax&0x00FF;
		SCI_canopen.txb[14]=(SCI_canopen.tx_skiiptempmax&0xFF00)>>8;
		SCI_canopen.txb[15]=SCI_canopen.tx_demand&0x00FF;
		SCI_canopen.txb[16]=(SCI_canopen.tx_demand&0xFF00)>>8;

		xor_data=SCI_canopen.txb[0];									//Òì»òÐ£Ñé
		for(i=1;i<SCICANOPENTXNUM-1;i++)
			xor_data ^= SCI_canopen.txb[i];
				
		SCI_canopen.txb[SCICANOPENTXNUM-1] = xor_data;
		
		for(i=0;i<SCICANOPENTXNUM-2;i++)
			ScibRegs.SCITXBUF=SCI_canopen.txb[i];
		
} 
/*********************************************************************************************************
** º¯ÊýÃû³Æ: DataFilter
** ¹¦ÄÜÃèÊö: Êý¾ÝÂË²¨
** Êä¡¡Èë: 	Y(k-1)ÎªÉÏ´ÎÂË²¨½á¹û£¬X(k)ÎªÐÂ²ÉÑùÖµ¡£¡£
** Êä¡¡³ö: £ºY(k)Îª±¾´ÎÂË²¨½á¹û¡£  
** ×¢  ÊÍ: 	 ÂË²¨¹«Ê½Îª£ºY(k)=cY(k-1)+(1-c)X(k),ÆäÖÐ£¬c=1/(1+2*PAI*fh/fs),fhÎªµÍÍ¨ÂË²¨Æ÷µÄ½ØÖ¹ÆµÂÊ£¬fsÎª²ÉÑùÆµÂÊ.
			ÔÚÒ»½×µÍÍ¨ÂË²¨ÖÐ£¬X(k)¼´ÎªY(k)¡£
			Ö±Á÷Á¿Îó²î1£¥ÎªÎÈ¶¨Ê±¼ä¡£
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void DataFilter( float c,float *out,float in)
{
   *out = c * *out + (1 - c) * in;
} 

/*********************************************************************************************************
** º¯ÊýÃû³Æ: Ad8364Ctrl
** ¹¦ÄÜÃèÊö: ¶ÁÈ¡²¢´¦ÀíÇ°Ò»´ÎµÄ×ª»»½á¹û£¬Í¬Ê±Æô¶¯ÏÂÒ»´ÎAD×ª»»
** Êä¡¡Èë: 	 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 	 Ã¿´Î¶¼ÔËÐÐ
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Ad8364Ctrl(void)
{
	float tempa,tempb,tempc;

//------------------------------Êý¾Ý¶ÁÈ¡(¹²18Â·)---------------------------------
//¸Ã×éADÊý¾ÝÊÇ100usÖ®Ç°µÄ½á¹û
	AD.dat[0] = *AD_ASTART;	       // STA.Ubc µç»ú¶¨×Ó²àµçÑ¹Ubc
	AD.dat[1] = *(AD_ASTART+1);    // AMUX£¬ÂýËÙÐÅºÅ£¬16Ñ¡1¹ýÀ´µÄÐÅºÅ
	AD.dat[2] = *(AD_ASTART+2);    // GRD.Ubc Ö÷¶ÏÇ°Ubc
//	AD.dat[3] = *(AD_ASTART+3);    // Ic ±¸ÓÃSKIIPµçÁ÷·´À¡
	AD.dat[4] = *(AD_ASTART+4);    // GRD.Uab Ö÷¶ÏÇ°Uab
	AD.dat[5] = *(AD_ASTART+5);    // MPR.ic, »ú²à±äÁ÷Æ÷MPRµÄµçÁ÷

	AD.dat[6]  = *AD_BSTART;       // NGS.Uab Íø²àµçÑ¹Ubc´óÂË²¨Í¨µÀ
	AD.dat[7]  = *(AD_BSTART+1);   // MPR.ib£¬»ú²à±äÁ÷Æ÷NPRµÄµçÁ÷
	AD.dat[8]  = *(AD_BSTART+2);   // NGS.Ubc Íø²àµçÑ¹Ubc´óÂË²¨Í¨µÀ 
	AD.dat[9]  = *(AD_BSTART+3);   // MPR.ia£¬»ú²à±äÁ÷Æ÷NPRµÄµçÁ÷ 
	AD.dat[10] = *(AD_BSTART+4);   // Udc ÖÐ¼äÖ±Á÷µçÑ¹ 
	AD.dat[11] = *(AD_BSTART+5);   // NPR.ic£¬Íø²à±äÁ÷Æ÷NPRµÄµçÁ÷

//	AD.dat[12] = *AD_CSTART;       // Vdc2 
	AD.dat[13] = *(AD_CSTART+1);   // NPR.ib£¬Íø²à±äÁ÷Æ÷NPRµÄµçÁ÷
	AD.dat[14] = *(AD_CSTART+2);   // NGS.Uab Íø²àµçÑ¹Uab
	AD.dat[15] = *(AD_CSTART+3);   // NPR.ia£¬Íø²à±äÁ÷Æ÷NPRµÄµçÁ÷
	AD.dat[16] = *(AD_CSTART+4);   // STA.Uab µç»ú¶¨×Ó²àµçÑ¹Uab
	AD.dat[17] = *(AD_CSTART+5);   // NGS.Ubc Íø²àµçÑ¹Ubc

//----------------------------Êý¾Ý¶ÁÈ¡½áÊø------------------------------
										
	ADFINAL.ia1  = AD.dat[15];		// NPR.ia£¬Íø²à±äÁ÷Æ÷NPRµÄµçÁ÷
	ADFINAL.ib1  = AD.dat[13];	    // NPR.ib£¬Íø²à±äÁ÷Æ÷NPRµÄµçÁ÷
	ADFINAL.ic1  = AD.dat[11];	    // NPR.ic£¬Íø²à±äÁ÷Æ÷NPRµÄµçÁ÷

	ADFINAL.ia2  = AD.dat[9];	    // MPR.ia£¬»ú²à±äÁ÷Æ÷NPRµÄµçÁ÷
	ADFINAL.ib2  = AD.dat[7];		// MPR.ib£¬»ú²à±äÁ÷Æ÷NPRµÄµçÁ÷	
	ADFINAL.ic2  = AD.dat[5];		// MPR.ic, »ú²à±äÁ÷Æ÷MPRµÄµçÁ÷	

	ADFINAL.uab   = AD.dat[4];       // GRD.Uab Ö÷¶ÏÇ°Uab
    ADFINAL.ubc   = AD.dat[2];		 // GRD.Ubc Ö÷¶ÏÇ°Ubc

	ADFINAL.uab1 = AD.dat[14];		// Uab Íø²àµçÑ¹		
	ADFINAL.ubc1 = AD.dat[17];		// Ubc Íø²àµçÑ¹
	
	ADFINAL.uab2 = AD.dat[16];		// Uab µç»ú¶¨×Ó²àµçÑ¹		
	ADFINAL.ubc2 = AD.dat[0];		// Ubc µç»ú¶¨×Ó²àµçÑ¹

	ADFINAL.uab3 = AD.dat[6];		// Uab Íø²àµçÑ¹	´óÂË²¨Í¨µÀ 	
	ADFINAL.ubc3 = AD.dat[8];		// Ubc Íø²àµçÑ¹ ´óÂË²¨Í¨µÀ 


	ADFINAL.udc  = AD.dat[10];		//ÖÐ¼äÖ±Á÷µçÑ¹¼ì²â

    if(ADFINAL.udc < 0) ADFINAL.udc=0;

	ADFINAL.AMUX = AD.dat[1];       //ÂýËÙADÊäÈë

//-----------------------------------------------------------
                 
	switch(_OUT_AMUX)
	{
		 case(8): {AMUX.NPR_tempa=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//NPRµÄAÏàÎÂ¶È£¬Skiip·´À¡,=10V*10/(32768) +20 			   				
		 case(10):{AMUX.NPR_tempb=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//NPRµÄBÏàÎÂ¶È£¬Skiip·´À¡,=10V*10/(32768) +20		    
		 case(12):{AMUX.NPR_tempc=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//NPRµÄCÏàÎÂ¶È£¬Skiip·´À¡,=10V*10/(32768) +20			    
		 case(11):{AMUX.MPR_tempa=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//MPRµÄAÏàÎÂ¶È£¬Skiip·´À¡,=10V*10/(32768) +20			    
		 case(9): {AMUX.MPR_tempb=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//MPRµÄBÏàÎÂ¶È£¬Skiip·´À¡,=10V*10/(32768) +20	     
		 case(2): {AMUX.MPR_tempc=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//MPRµÄCÏàÎÂ¶È£¬Skiip·´À¡,=10V*10/(32768) +20
         case(1): {AMUX.Lac_temp =ADFINAL.AMUX   * 0.007825;		 break; }	//Íø²àµç¸ÐÎÂ¶È£¬PT100(Rt=100R+0.39*T),=10V*50R/(32768*5V*0.39)
		 case(0): {AMUX.Ldudt_temp= ADFINAL.AMUX * 0.007825;	     break; }	//»ú²àdu/dtµÄÎÂ¶È£¬PT100
		 case(5): {AD_OUT_STA_I.ac=ADFINAL.AMUX  * 0.0732422;		 break; }	//µç»ú¶¨×Ó²àAÏàÏßµçÁ÷£¬»¥¸ÐÆ÷1200A:1A,Rt=5R,=1200A*10V/(5R*1A*32768)			     
		 case(4): {AD_OUT_STA_I.ba=ADFINAL.AMUX  * 0.0732422;		 break; }	//µç»ú¶¨×Ó²àBÏàÏßµçÁ÷£¬»¥¸ÐÆ÷
	     default: break; 			       
	}


	      
	_OUT_AMUX++;
	if(_OUT_AMUX > 12) _OUT_AMUX=0;
			
	_OUT4_DATA = _OUT_AMUX;
	*OUT4_ADDR = _OUT4_DATA;	      
/*
//--------------¿É²Î¿¼¸Ä½ø---------------------------------------------	      
	switch(_OUT4_DATA)
	{
		case(8): {AMUX.NPR_tempa=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//NPRµÄAÏàÎÂ¶È£¬Skiip·´À¡,=10V*10/(32768) +20 			   				
		case(10):{AMUX.NPR_tempb=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//NPRµÄBÏàÎÂ¶È£¬Skiip·´À¡,=10V*10/(32768) +20		    
		case(12):{AMUX.NPR_tempc=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//NPRµÄCÏàÎÂ¶È£¬Skiip·´À¡,=10V*10/(32768) +20			    
		case(11):{AMUX.MPR_tempa=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//MPRµÄAÏàÎÂ¶È£¬Skiip·´À¡,=10V*10/(32768) +20			    
		case(9): {AMUX.MPR_tempb=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//MPRµÄBÏàÎÂ¶È£¬Skiip·´À¡,=10V*10/(32768) +20	     
		case(2): {AMUX.MPR_tempc=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//MPRµÄCÏàÎÂ¶È£¬Skiip·´À¡,=10V*10/(32768) +20
        case(1): {
                  AMUX.Lac_R=(ADFINAL.AMUX+49152) /(491.52- 0.001* ADFINAL.AMUX);//new PCB PT100(Rt=100R+0.39*T)
             	  AMUX.Lac_temp=2.5641 * (AMUX.Lac_R - 100);
             	  break; 
             	 }
        case(0): {
                  AMUX.Ldudt_R=(ADFINAL.AMUX+49152) /(491.52- 0.001* ADFINAL.AMUX);//new PCB PT100(Rt=100R+0.39*T)
             	  AMUX.Ldudt_temp=2.5641 * (AMUX.Ldudt_R - 100);
             	  break; 
             	 }
		case(5): {AD_OUT_STA_I.ac=ADFINAL.AMUX  * 0.0732422;		break; }	//µç»ú¶¨×Ó²àAÏàÏßµçÁ÷£¬»¥¸ÐÆ÷1200A:1A,Rt=5R,=1200A*10V/(5R*1A*32768)			     
		case(4): {AD_OUT_STA_I.ba=ADFINAL.AMUX  * 0.0732422;		break; }	//µç»ú¶¨×Ó²àBÏàÏßµçÁ÷£¬»¥¸ÐÆ÷
		case(7): {AD_OUT_SCR_I.a =ADFINAL.AMUX  * 0.0305176;		break; }	//ActiveCROWBARÖÐAÏàSCRµçÁ÷£¬µçÁ÷LEM/µçÑ¹ÐÍ·´À¡4A--400V =10V*400A/(32768*4V)			     
		case(6): {AD_OUT_SCR_I.b =ADFINAL.AMUX  * 0.0305176;		break; }	//ActiveCROWBARÖÐBÏàSCRµçÁ÷£¬µçÁ÷LEM/µçÑ¹ÐÍ·´À¡4A--400V =10V*400A/(32768*4V)
		default: break; 			       
	}

	if(M_ChkCounter(MAIN_LOOP.cnt_AMUX,DELAY_AMUX)>=0)
	{
	    MAIN_LOOP.cnt_AMUX=0;                         							//5ms¶ÁÒ»´ÎÂýËÙAD	     
	     _OUT_AMUX1++;
		 if(_OUT_AMUX1 >= 12) _OUT_AMUX1=0;
		 _OUT4_DATA = _OUT_AMUX1;
		 *OUT4_ADDR = _OUT4_DATA;
    } 
	else
	{
		_OUT_AMUX2++;
		if((_OUT_AMUX2 > 7) || (_OUT_AMUX2 < 4)) _OUT_AMUX2=4; 
		_OUT4_DATA = _OUT_AMUX2;
		*OUT4_ADDR = _OUT4_DATA;	
	}
*/
//-------------------¶¨×ÓµçÁ÷£¨µ¥Î»A£©-------------------------------------------------------------
	AD_OUT_STA_I.a  =  (AD_OUT_STA_I.ac - AD_OUT_STA_I.ba) * 0.3333333;
	AD_OUT_STA_I.b  =   AD_OUT_STA_I.a  + AD_OUT_STA_I.ba;	
    AD_OUT_STA_I.c  = - AD_OUT_STA_I.a  - AD_OUT_STA_I.b; 

//------------------Íø²à±äÁ÷Æ÷µçÁ÷------------------------------------------------------------------
    AD_OUT_NPR_I.a = - (ADFINAL.ia1 * 0.0572204);  // SKIIP·´À¡µçÁ÷(Á÷³öÇÅ±ÛÎªÕý),µ×°åÓÐÒ»¸ö·´Ïò£¬10V=1875A,=10V*1875A/(32768*10V)
	AD_OUT_NPR_I.b = - (ADFINAL.ib1 * 0.0572204);  // SKIIP·´À¡µçÁ÷(Á÷³öÇÅ±ÛÎªÕý)£¬¿ØÖÆËã·¨ÒÔÁ÷³öSKIIPý
	AD_OUT_NPR_I.c = - (ADFINAL.ic1 * 0.0572204);  // SKIIP·´À¡µçÁ÷(Á÷³öÇÅ±ÛÎªÕý)

//-------------------»ú²à±äÁ÷Æ÷µçÁ÷×ªÎªÊµ¼ÊÖµ------------------------------------------------------
    AD_OUT_MPR_I.a =  (ADFINAL.ia2 * 0.0572204);  // SKIIP·´À¡µçÁ÷(Á÷³öÇÅ±ÛÎªÕý),µ×°åÓÐÒ»¸ö·´Ïò£¬10V=1875A,=10V*1875A/(32768*10V)
	AD_OUT_MPR_I.b =  (ADFINAL.ib2 * 0.0572204);  // SKIIP·´À¡µçÁ÷(Á÷³öÇÅ±ÛÎªÕý)£¬¿ØÖÆËã·¨ÒÔÁ÷³öSKIIPÎªÕý
	AD_OUT_MPR_I.c =  (ADFINAL.ic2 * 0.0572204);  // SKIIP·´À¡µçÁ÷(Á÷³öÇÅ±ÛÎªÕý)

//---------------------Ö±Á÷µçÑ¹----------------------------------------------------------------------
    AD_OUT_UDC      = ADFINAL.udc * 0.0448788;   // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
	DataFilter(0.44,&MEAN_DATA.udc,AD_OUT_UDC);  //VdcÖ±Á÷ÂË²¨£¬fh=1kHz
    AD_OUT_UDC      = MEAN_DATA.udc;
													
//---------------------Ö÷¶ÏÇ°µçÍøµçÑ¹---------------------------------------------------------------------
	AD_OUT_GRD_U.ab = ADFINAL.uab * 0.0448788;   // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
	AD_OUT_GRD_U.bc = ADFINAL.ubc * 0.0448788;   // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA) 

//---------------------Ö÷¶ÏºóµçÍøµçÑ¹---------------------------------------------------------------------
	AD_OUT_NGS_U.ab = ADFINAL.uab1 * 0.0448788;   // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
	AD_OUT_NGS_U.bc = ADFINAL.ubc1 * 0.0448788;   // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
 
//-------------------µç»ú¶¨×Ó²àÏßµçÑ¹---------------------------------------------
	AD_OUT_STA_U.ab  = ADFINAL.uab2 * 0.0448788;  // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
	AD_OUT_STA_U.bc  = ADFINAL.ubc2 * 0.0448788;  // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)

//-------------------Ö÷¶ÏºóµçÍøµçÑ¹,´óÂË²¨Í¨µÀ---------------------------------------------
	AD_OUT_NGF_U.ab  = ADFINAL.uab3 * 0.0448788;  // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
	AD_OUT_NGF_U.bc  = ADFINAL.ubc3 * 0.0448788;  // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)

/* ÆÁ±ÎÁãÆ¯ÂË²¨20100507atzuoyun
//---------------------ÁãÆ¯ÂË²¨-------------------------------------------------------------------	
	DataFilter(0.999985,&MEAN_DATA.zfia1,AD_OUT_NPR_I.a); //Íø²àµçÁ÷´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S
	DataFilter(0.999985,&MEAN_DATA.zfib1,AD_OUT_NPR_I.b); //Íø²àµçÁ÷´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S
	DataFilter(0.999985,&MEAN_DATA.zfic1,AD_OUT_NPR_I.c); //Íø²àµçÁ÷´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S

	DataFilter(0.999985,&MEAN_DATA.zfia3,AD_OUT_STA_I.a); //¶¨×Ó²àµçÁ÷´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S
	DataFilter(0.999985,&MEAN_DATA.zfib3,AD_OUT_STA_I.b); //¶¨×Ó²àµçÁ÷´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S
	DataFilter(0.999985,&MEAN_DATA.zfic3,AD_OUT_STA_I.c); //¶¨×Ó²àµçÁ÷´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S

	DataFilter(0.999985,&MEAN_DATA.zfuab,AD_OUT_GRD_U.ab); //Ö÷¶ÏÇ°ÍøÑ¹µçÑ¹´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S
	DataFilter(0.999985,&MEAN_DATA.zfubc,AD_OUT_GRD_U.bc); //Ö÷¶ÏÇ°ÍøÑ¹µçÑ¹´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S 

	DataFilter(0.999985,&MEAN_DATA.zfuab1,AD_OUT_NGS_U.ab); //ÍøÑ¹µçÑ¹´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S
	DataFilter(0.999985,&MEAN_DATA.zfubc1,AD_OUT_NGS_U.bc); //ÍøÑ¹µçÑ¹´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S


//------------------³ýÈ¥ÁãÆ¯---------------------------------------------------------------------
	if(M_ChkFlag(SL_SENSZFSTDY)!=0)
	{
		AD_OUT_NPR_I.a = AD_OUT_NPR_I.a - MEAN_DATA.zfia1;  //Íø²àµçÁ÷
		AD_OUT_NPR_I.b = AD_OUT_NPR_I.b - MEAN_DATA.zfib1;
		AD_OUT_NPR_I.c = AD_OUT_NPR_I.c - MEAN_DATA.zfic1;

		AD_OUT_STA_I.a = AD_OUT_STA_I.a - MEAN_DATA.zfia3;  //¶¨×Ó²àµçÁ÷
		AD_OUT_STA_I.b = AD_OUT_STA_I.b - MEAN_DATA.zfib3; 
		AD_OUT_STA_I.c = AD_OUT_STA_I.c - MEAN_DATA.zfic3; 

		AD_OUT_GRD_U.ab = AD_OUT_GRD_U.ab - MEAN_DATA.zfuab; //Ö÷¶ÏÇ°ÍøÑ¹µçÑ¹
		AD_OUT_GRD_U.bc = AD_OUT_GRD_U.bc - MEAN_DATA.zfubc; 

		AD_OUT_NGS_U.ab = AD_OUT_NGS_U.ab - MEAN_DATA.zfuab1; //ÍøÑ¹µçÑ¹
		AD_OUT_NGS_U.bc = AD_OUT_NGS_U.bc - MEAN_DATA.zfubc1;	
	}
*/
//-------------------Ö÷¶ÏÇ°µçÍøµçÑ¹£¬ÓÉÏßÑ¹×ªÎªÏàÑ¹------------------------------------------------------
	AD_OUT_GRD_U.b  = (AD_OUT_GRD_U.bc - AD_OUT_GRD_U.ab) * 0.3333333;
	AD_OUT_GRD_U.a  = AD_OUT_GRD_U.b + AD_OUT_GRD_U.ab;	
	AD_OUT_GRD_U.c  = - AD_OUT_GRD_U.a - AD_OUT_GRD_U.b; 

//-------------------µçÍøµçÑ¹£¬ÓÉÏßÑ¹×ªÎªÏàÑ¹------------------------------------------------------
	AD_OUT_NGS_U.b  = (AD_OUT_NGS_U.bc - AD_OUT_NGS_U.ab) * 0.3333333;
	AD_OUT_NGS_U.a  = AD_OUT_NGS_U.b + AD_OUT_NGS_U.ab;	
	AD_OUT_NGS_U.c  = - AD_OUT_NGS_U.a - AD_OUT_NGS_U.b;

//-------------------µç»ú¶¨×Ó²àµçÑ¹£¬ÓÉÏßÑ¹ËãÏàÑ¹------------------------------------------------------
	AD_OUT_STA_U.b  = (AD_OUT_STA_U.bc - AD_OUT_STA_U.ab) * 0.3333333;
	AD_OUT_STA_U.a  = AD_OUT_STA_U.b + AD_OUT_STA_U.ab;	
    AD_OUT_STA_U.c  = - AD_OUT_STA_U.a - AD_OUT_STA_U.b;

//---------------------Æ½¾ùÖµÂË²¨------------------------------------------------------------------	
	tempa = abs(AD_OUT_NPR_I.a);
	tempb = abs(AD_OUT_NPR_I.b);
	tempc = abs(AD_OUT_NPR_I.c);
	DataFilter(0.9999,&MEAN_DATA.ia1,tempa); //Íø²àµçÁ÷	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.ib1,tempb); //Íø²àµçÁ÷	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.ic1,tempc); //Íø²àµçÁ÷	Æ½¾ùÖµÂË²¨10S

	tempa = abs(AD_OUT_MPR_I.a);
	tempb = abs(AD_OUT_MPR_I.b);
	tempc = abs(AD_OUT_MPR_I.c);
	DataFilter(0.9999,&MEAN_DATA.ia2,tempa); //»ú²àµçÁ÷	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.ib2,tempb); //»ú²àµçÁ÷	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.ic2,tempc); //»ú²àµçÁ÷	Æ½¾ùÖµÂË²¨10S	

	tempa = abs(AD_OUT_GRD_U.ab);
	tempb = abs(AD_OUT_GRD_U.bc);
	DataFilter(0.9999,&MEAN_DATA.uab,tempa); //Ö÷¶ÏÇ°Íø²àµçÑ¹	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.ubc,tempb); //Ö÷¶ÏÇ°Íø²àµçÑ¹	Æ½¾ùÖµÂË²¨10S 

	tempa = abs(AD_OUT_NGS_U.ab);
	tempb = abs(AD_OUT_NGS_U.bc);
	DataFilter(0.9999,&MEAN_DATA.uab1,tempa); //Íø²àµçÑ¹	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.ubc1,tempb); //Íø²àµçÑ¹	Æ½¾ùÖµÂË²¨10S

	tempa = abs(AD_OUT_STA_U.ab);
	tempb = abs(AD_OUT_STA_U.bc);
	DataFilter(0.9999,&MEAN_DATA.uab2,tempa); //¶¨×Ó²àµçÑ¹	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.ubc2,tempb); //¶¨×Ó²àµçÑ¹	Æ½¾ùÖµÂË²¨10S


	tempa = abs(AD_OUT_NGS_U.a);
	tempb = abs(AD_OUT_NGS_U.b);
	tempc = abs(AD_OUT_NGS_U.c);
	DataFilter(0.9999,&MEAN_DATA.ua1,tempa); //Íø²àµçÑ¹	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.ub1,tempb); //Íø²àµçÑ¹	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.uc1,tempc); //Íø²àµçÑ¹	Æ½¾ùÖµÂË²¨10S

//--------------ÇóÍøÑ¹ºÍ¶¨×ÓµçÑ¹²îÖµÆ½¾ùÖµ------------------------------------------
    AD_OUT_STAD_U.ab = AD_OUT_NGF_U.ab - AD_OUT_STA_U.ab;
    AD_OUT_STAD_U.bc = AD_OUT_NGF_U.bc - AD_OUT_STA_U.bc;

	tempa = abs(AD_OUT_STAD_U.ab);
	tempb = abs(AD_OUT_STAD_U.bc);
	DataFilter(0.9999,&MEAN_DATA.uab_d,tempa);  //²îÖµÆ½¾ùÖµ    Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.ubc_d,tempb);  //²îÖµÆ½¾ùÖµ	Æ½¾ùÖµÂË²¨10S
//----------------------------------------------------------------------------------

	tempa = abs(AD_OUT_STA_I.ac);
	tempb = abs(AD_OUT_STA_I.ba);
	DataFilter(0.9999,&MEAN_DATA.iac3,tempa); //¶¨×Ó²àµçÁ÷	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.iba3,tempb); //¶¨×Ó²àµçÁ÷	Æ½¾ùÖµÂË²¨10S

	tempa = abs(AD_OUT_STA_I.a);
	tempb = abs(AD_OUT_STA_I.b);
	tempc = abs(AD_OUT_STA_I.c);
	DataFilter(0.9999,&MEAN_DATA.ia3,tempa); //¶¨×Ó²àµçÁ÷	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.ib3,tempb); //¶¨×Ó²àµçÁ÷	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.ic3,tempc); //¶¨×Ó²àµçÁ÷	Æ½¾ùÖµÂË²¨10S


//--------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
	*AD_DA_CTRL     = AD8364_CONVERT;	               	  	//Æô¶¯ÏÂ»AD×ª»»
    CAP4.adsmptsctr = ECap4Regs.TSCTR;						//±£´æÍøÑ¹¶¨ÏòÊ±¼ä
	QEPDATA.adsmposcnt=EQep2Regs.QPOSCNT;					//±£´æ±àÂëÆ÷Î»ÖÃÐÅÏ¢  

}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: Output
** ¹¦ÄÜÃèö: 10Â·ÐÅºÅÊä³ö; 8Â·LEDÏÔÊ¾Êä³ö
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
void Output(void)
{
//------------------------------Êý¾ÝÊä³ö-----------------------------------

   *OUT1_ADDR = _OUT1_DATA;
   *OUT2_ADDR = _OUT2_DATA;

//------------------------------¹¦ÂÊ·ç»úÐÇÈý½Ç±ä»»--------------------------------------------------------------------------

	 if(AMUX.skiiptempmax < _SC_FAN_TL)           M_ClrFlag(SL_FANSELECT);   //ÎÂ¶ÈÐ¡ÓÚ40¶È,ÐÇ½Ó                      
	 else if(AMUX.skiiptempmax > _SC_FAN_TH)      M_SetFlag(SL_FANSELECT);   //ÎÂ¶È´óÓÚ50¶È,±äÎª½Ç½Ó        


   if(M_ChkFlag(SL_FAN_WORK)!=0)            								//¹¦ÂÊ·ç»úÐÇÈý½Ç±ä»»,¸ù¾ÝSKIIPÎÂ¶ÈÅÐ¶Ï
   { 
     if(M_ChkFlag(SL_FANSELECT)==0) 
     {
        M_ClrFlag(CL_FANTRIANGLE);
 	    if(M_ChkCounter(MAIN_LOOP.cnt_fanstar,DELAY_FANSTAR)>=0)	    M_SetFlag(CL_FANSTAR);       //ÑÓÊ±ºóÔÙ±ÕºÏÐÇ½Ó½Ó´¥Æ÷
		MAIN_LOOP.cnt_fantriangle=0;		 
     } 
	 else 
	 {
	 	M_ClrFlag(CL_FANSTAR);
	 	if(M_ChkCounter(MAIN_LOOP.cnt_fantriangle,DELAY_FANTRIANGLE)>=0) M_SetFlag(CL_FANTRIANGLE);     //ÑÓÊ±ºóÔÙ±ÕºÏ½Ç½Ó½Ó´¥Æ÷
		MAIN_LOOP.cnt_fanstar=0;	 
	 }
   }
   else
   {
      MAIN_LOOP.cnt_fanstar=0;
	  MAIN_LOOP.cnt_fantriangle=0;
	  M_ClrFlag(CL_FANSTAR);
      M_ClrFlag(CL_FANTRIANGLE);
   }

//---------------------CANOPEN·´À¡Ö÷¿ØµÄ×´Ì¬±äÁ¿---------------------------------------------------
/*
    M_SetFlag(SL_CBCLOSED);   //cantest
	M_SetFlag(SL_STEADYFB);   //cantest
	M_SetFlag(SL_MPR_SYNOK);   //cantest
    M_SetFlag(SL_SPEED_IN_RANGE);   //cantest
	M_ClrFlag(SL_SERIESTOP); //cantest
    M_ClrFlag(SL_ERRSTOP); //cantest
	M_SetFlag(SL_PGOV);   //cantest
    M_SetFlag(SL_QGOV);   //cantest
	M_NotFlag(SL_TEST); //cantest
*/
//-----------------------------------tx_state1----------------------------------------	
	if(M_ChkFlag(SL_CBCLOSED)!=0)	  		SCI_canopen.tx_state1 |= COM_NPRREADY;
	else									SCI_canopen.tx_state1 &= COM_NPRREADY_NOT;

	if(M_ChkFlag(SL_STEADYFB)!=0)	  		SCI_canopen.tx_state1 |= COM_NPRON;
	else 									SCI_canopen.tx_state1 &= COM_NPRON_NOT;

	if(M_ChkFlag(SL_MPR_SYNOK)!=0)	  		SCI_canopen.tx_state1 |= COM_READYGENERATION;
	else 									SCI_canopen.tx_state1 &= COM_READYGENERATION_NOT;

	if(M_ChkFlag(SL_SPEED_IN_RANGE)!=0)	  	SCI_canopen.tx_state1 |= COM_SPEEDINRANGE;
	else 									SCI_canopen.tx_state1 &= COM_SPEEDINRANGE_NOT;

//201105atzuoyun
	if((M_ChkFlag(SL_SERIESTOP)!=0 || M_ChkFlag(SL_ERRSTOP)!=0) && M_ChkFlag(SL_REPORT_OCS)!=0)	  	  SCI_canopen.tx_state1 |= COM_FAILURE;
	else  if((M_ChkFlag(SL_SERIESTOP)==0 && M_ChkFlag(SL_ERRSTOP)==0)||M_ChkFlag(SL_REPORT_OCS)==0)	  SCI_canopen.tx_state1 &= COM_FAILURE_NOT;

	if(M_ChkFlag(SL_SERIESTOP)!=0 && M_ChkFlag(SL_REPORT_OCS)!=0)	SCI_canopen.tx_state1 |= COM_FAILUREA;
	else 															SCI_canopen.tx_state1 &= COM_FAILUREA_NOT;

	if(M_ChkFlag(SL_ERRSTOP)!=0 && M_ChkFlag(SL_REPORT_OCS)!=0)	  	SCI_canopen.tx_state1 |= COM_FAILUREB;
	else 															SCI_canopen.tx_state1 &= COM_FAILUREB_NOT;
//201105atzuoyun

	if(M_ChkFlag(SL_PGOV_COM)!=0)	  		SCI_canopen.tx_state1 |= COM_PLIM;
    else 									SCI_canopen.tx_state1 &= COM_PLIM_NOT;

	if(M_ChkFlag(SL_QGOV_COM)!=0)	  		SCI_canopen.tx_state1 |= COM_QLIM;
	else 									SCI_canopen.tx_state1 &= COM_QLIM_NOT;
	

	SCI_canopen.tx_state1 &= 0x7FFF;
	SCI_canopen.tx_state1 |= (SCI_canopen.heartbeat <<8);

//-----------------------------------tx_state2----------------------------------------	

//	if(((PRO.STA_iac* 1.05)>_SC_IACOVST)||((PRO.STA_iba* 1.05)>_SC_IACOVST))	  	SCI_canopen.tx_state2 |= COM_ILIM;
//	else																			SCI_canopen.tx_state2 &= COM_ILIM_NOT;

	if(M_ChkFlag(SL_IN1_EXESTOP)!=0)	  	SCI_canopen.tx_state2 |= COM_ESTOP;     //20090815
	else 									SCI_canopen.tx_state2 &= COM_ESTOP_NOT;

	if(M_ChkFlag(SL_CBTRIP)!=0)	  			SCI_canopen.tx_state2 |= COM_CBTRIP;
	else 									SCI_canopen.tx_state2 &= COM_CBTRIP_NOT;

	if(M_ChkFlag(SL_IN1_CBSTS)!=0)	  		SCI_canopen.tx_state2 |= COM_CBCLOSED;			//1=CB closed 0=CB open
	else 									SCI_canopen.tx_state2 &= COM_CBCLOSED_NOT;

//	if(PRO.Pg>1.8e6)	  					SCI_canopen.tx_state2 |= COM_POWLIM;
//    else if(PRO.Pg<1.75e6)					SCI_canopen.tx_state2 &= COM_POWLIM_NOT;	
   
//-----------------------------ÏÔÊ¾µÆÊä³ö----------------------------------
	if(M_ChkFlag(SL_DISPLAY0)!=0) 	GpioDataRegs.GPBCLEAR.bit.GPIO56 = 1;
	else							GpioDataRegs.GPBSET.bit.GPIO56 = 1;

	if(M_ChkFlag(SL_DISPLAY1)!=0)  	GpioDataRegs.GPBCLEAR.bit.GPIO57 = 1;
	else							GpioDataRegs.GPBSET.bit.GPIO57 = 1;
	
	if(M_ChkFlag(SL_DISPLAY2)!=0) 	GpioDataRegs.GPBCLEAR.bit.GPIO58 = 1;
	else							GpioDataRegs.GPBSET.bit.GPIO58 = 1;

	if(M_ChkFlag(SL_DISPLAY3)!=0)  	GpioDataRegs.GPBCLEAR.bit.GPIO59 = 1;
	else							GpioDataRegs.GPBSET.bit.GPIO59 = 1;

	if(M_ChkFlag(SL_DISPLAY4)!=0) 	GpioDataRegs.GPBCLEAR.bit.GPIO60 = 1;
	else							GpioDataRegs.GPBSET.bit.GPIO60 = 1;

	if(M_ChkFlag(SL_DISPLAY5)!=0)  	GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1;
	else							GpioDataRegs.GPBSET.bit.GPIO61 = 1;

	if(M_ChkFlag(SL_DISPLAY6)!=0) 	GpioDataRegs.GPBCLEAR.bit.GPIO62 = 1;
	else							GpioDataRegs.GPBSET.bit.GPIO62 = 1;

	if(M_ChkFlag(SL_DISPLAY7)!=0)  	GpioDataRegs.GPBCLEAR.bit.GPIO63 = 1;
	else							GpioDataRegs.GPBSET.bit.GPIO63 = 1;

} 
/*********************************************************************************************************
** º¯ÊýÃû³Æ: Input
** ¹¦ÄÜÃèÊö: 16Â·ÐÅºÅÊäÈë; 
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
void Input(void)
{
	Uint16 tempa,tempb,tempc,tempda,tempdb,tempdc;

//--------------------------------Êý¾ÝÊäÈë----------------------------------
	tempa = *IN1_ADDR; 
	tempb = *IN2_ADDR;
	tempc = *IN3_ADDR;

	DELAY_US(100L);        //100usÑÓÊ±£¬IOÊäÈë·À¶¶

	tempda = *IN1_ADDR;    
	tempdb = *IN2_ADDR;
	tempdc = *IN3_ADDR;

//--------------------------------Êý¾ÝÊäÈë----------------------------------	
	if((tempa==tempda)&&(tempb==tempdb))	
		_IN12_DATA = (tempa & 0x00FF) | ((tempb<<8) & 0xFF00);

	if(tempc==tempdc)		
		_IN34_DATA = (tempc & 0x00FF);

if(_CANOPER==0)
{
//---------------------ÉÏÎ»»úÒªÇóºÏÖ÷¶Ï------------------------------------------------------
	if(_EIN!= 0)							
	{ 
	  if(M_ChkCounter(MAIN_LOOP.cnt_nprcmd,DELAY_NPRCMD)>0)	M_SetFlag(SL_OCS_EIN);     //ÑÓÊ±1s£¬·ÀÖ¹Îó²Ù×÷
	}				
	else 	
	{
		M_ClrFlag(SL_OCS_EIN);              
		MAIN_LOOP.cnt_nprcmd=0;		
    } 

//---------------------ÉÏÎ»»úÒªÇó¸´Î»¹ÊÕÏ------------------------------------------------------
	if(_RESET != 0)							//ÉÏÎ»»úÒªÇó¸´Î»¹ÊÕÏ,ÓÉÍâ²¿I/O¸ø¶¨
	{ 
	  if(M_ChkCounter(MAIN_LOOP.cnt_reset,DELAY_RESET)>0)	 M_SetFlag(SL_OCS_RESET);     //ÑÓÊ±2s£¬·ÀÖ¹Îó²Ù×÷
	}				
	else 	
	{
		M_ClrFlag(SL_OCS_RESET);
		MAIN_LOOP.cnt_reset=0;		
    }	
//---------------------ÉÏÎ»»úÒªÇó±äÁ÷Æ÷Æô¶¯------------------------------------------------------
	if(_SYSRUN!= 0)							
	{ 
	  if(M_ChkCounter(MAIN_LOOP.cnt_clostacmd,DELAY_CLOSTACMD)>0)	 M_SetFlag(SL_OCS_SYSRUN);     //ÑÓÊ±1s£¬·ÀÖ¹Îó²Ù×÷
	}				
	else 	
	{
		M_ClrFlag(SL_OCS_SYSRUN);
		MAIN_LOOP.cnt_clostacmd=0;		
    }
 
}
else
{ 
//---------------------ÉÏÎ»»úÒªÇóºÏÖ÷¶Ï------------------------------------------------------
	if((SCI_canopen.rx_controlword & COM_EIN)==COM_EIN)	    
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_ocsein1,DELAY_OCSEIN1)>0)		M_SetFlag(SL_OCS_EIN);	//20090817 250ms
		MAIN_LOOP.cnt_ocsein2=0;	
	}								
	else 		  											
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_ocsein2,DELAY_OCSEIN2)>0)		M_ClrFlag(SL_OCS_EIN);	//20090817 250ms 
		MAIN_LOOP.cnt_ocsein1=0;
	}

//---------------------ÉÏÎ»»úÒªÇó¸´Î»¹ÊÕÏ------------------------------------------------------
	if((SCI_canopen.rx_controlword & COM_OCSRESET)==COM_OCSRESET)	M_SetFlag(SL_OCS_RESET);									
	else 	   														M_ClrFlag(SL_OCS_RESET);													

	
//---------------------ÉÏÎ»»úÒªÇó±äÁ÷Æ÷Æô¶¯------------------------------------------------------
	if((SCI_canopen.rx_controlword & COM_SYSRUN)==COM_SYSRUN)	
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_ocssysrun1,DELAY_OCSSYSRUN1)>0)	M_SetFlag(SL_OCS_SYSRUN);	//20090817 250ms
		MAIN_LOOP.cnt_ocssysrun2=0;	
	}									
	else 		  											
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_ocssysrun2,DELAY_OCSSYSRUN2)>0)	M_ClrFlag(SL_OCS_SYSRUN);	//20090817  250ms
		MAIN_LOOP.cnt_ocssysrun1=0;
	}

//---------------------ÉÏÎ»»úÒªÇóÍø²à±äÁ÷Æ÷µ¥ÀÔËÐÐÎÞ¹¦²¢Íø------------------------------------------------------
//	if((SCI_canopen.rx_controlword & COM_NPREIN)==COM_NPREIN)	M_SetFlag(SL_OCS_NPREIN);  //ÔÝÊ±ÆÁ±Î										
//	else 		  												M_ClrFlag(SL_OCS_NPREIN); 
}	 
//---------------------------½ÓÊÕ²Ù×÷Æ÷À´µÄPI»·²ÎÊý-------------------------------------------------
	PI_PARA_NPRU.kp           = _NPR_U_Kp/100.00;			//DOT2ÊôÐÔ
    PI_PARA_NPRU.ki           = _NPR_U_Ki/10.00;			//DOT1ÊôÐÔ
	PI_PARA_NPRU.kd           = _NPR_U_Kd/1000.00;			//DOT3ÊôÐÔ
	PI_PARA_NPRU.outmax       = _NPR_U_outmax;
	PI_PARA_NPRU.errmax       = _NPR_U_errmax/10.00;		//DOT1ÊôÐÔ		//NPRµçÑ¹»·²ÎÊý
	PI_PARA_NPRU.errmin       = _NPR_U_errmin/1000.00;	    //DOT3ÊôÐÔ
	PI_PARA_NPRU.incrementmax = _NPR_U_incrementmax/10.00;  //DOT1ÊôÐÔ

	PI_PARA_NPRID.kp           = _NPR_ID_Kp/100.00;			//DOT2ÊôÐÔ
    PI_PARA_NPRID.ki           = _NPR_ID_Ki/10.00;			//DOT1ÊôÐÔ
    PI_PARA_NPRID.kd           = _NPR_ID_Kd/1000.00;			//DOT3ÊôÐÔ
	PI_PARA_NPRID.outmax       = _NPR_ID_outmax;		 					    //NPRµçÁ÷»·²ÎÊý
	PI_PARA_NPRID.errmax       = _NPR_ID_errmax/10.00;		//DOT1ÊôÐÔ
	PI_PARA_NPRID.errmin       = _NPR_ID_errmin/1000.00;		//DOT3ÊôÐÔ
	PI_PARA_NPRID.incrementmax = _NPR_ID_incrementmax/10.00;	//DOT1ÊôÐÔ

	PI_PARA_NPRIQ.kp           = _NPR_IQ_Kp/100.00;			//DOT2ÊôÐÔ
    PI_PARA_NPRIQ.ki           = _NPR_IQ_Ki/10.00;			//DOT1ÊôÐÔ
    PI_PARA_NPRIQ.kd           = _NPR_IQ_Kd/1000.00;			//DOT3ÊôÐÔ
	PI_PARA_NPRIQ.outmax       = _NPR_IQ_outmax;		 					    //NPRµçÁ÷»·²ÎÊý
	PI_PARA_NPRIQ.errmax       = _NPR_IQ_errmax/10.00;		//DOT1ÊôÐÔ
	PI_PARA_NPRIQ.errmin       = _NPR_IQ_errmin/1000.00;		//DOT3ÊôÐÔ
	PI_PARA_NPRIQ.incrementmax = _NPR_IQ_incrementmax/10.00;	//DOT1ÊôÐÔ

	PI_PARA_MPRID.kp           = _MPR_ID_Kp/100.00;			//DOT2ÊôÐÔ
    PI_PARA_MPRID.ki           = _MPR_ID_Ki/10.00;			//DOT1ÊôÐÔ
    PI_PARA_MPRID.kd           = _MPR_ID_Kd/1000.00;			//DOT3ÊôÐÔ
	PI_PARA_MPRID.outmax       = _MPR_ID_outmax;   							//MPRµçÁ÷»·²ÎÊý
	PI_PARA_MPRID.errmax       = _MPR_ID_errmax/10.00;		//DOT1ÊôÐÔ
	PI_PARA_MPRID.errmin       = _MPR_ID_errmin/1000.00;		//DOT3ÊôÐÔ
	PI_PARA_MPRID.incrementmax = _MPR_ID_incrementmax/10.00;	//DOT1ÊôÐÔ

	PI_PARA_MPRIQ.kp           = _MPR_IQ_Kp/100.00;			//DOT2ÊôÐÔ
    PI_PARA_MPRIQ.ki           = _MPR_IQ_Ki/10.00;			//DOT1ÊôÐÔ
    PI_PARA_MPRIQ.kd           = _MPR_IQ_Kd/1000.00;			//DOT3ÊôÐÔ
	PI_PARA_MPRIQ.outmax       = _MPR_IQ_outmax;   							//MPRµçÁ÷»·²ÎÊý
	PI_PARA_MPRIQ.errmax       = _MPR_IQ_errmax/10.00;		//DOT1ÊôÐÔ
	PI_PARA_MPRIQ.errmin       = _MPR_IQ_errmin/1000.00;		//DOT3ÊôÐÔ
	PI_PARA_MPRIQ.incrementmax = _MPR_IQ_incrementmax/10.00;	//DOT1ÊôÐÔ

	PI_PARA_MPRU.kp           = _MPR_U_Kp/100.00;			//DOT2ÊôÐÔ
    PI_PARA_MPRU.ki           = _MPR_U_Ki/10.00;			//DOT3ÊôÐÔ
    PI_PARA_MPRU.kd           = _MPR_U_Kd/1000.00;			//DOT1ÊôÐÔ
	PI_PARA_MPRU.outmax       = _MPR_U_outmax;   							//MPRµçÑ¹»·²ÎÊý
	PI_PARA_MPRU.errmax       = _MPR_U_errmax/10.00;		//DOT1ÊôÐÔ
	PI_PARA_MPRU.errmin       = _MPR_U_errmin/1000.00;		//DOT3ÊôÐÔ
	PI_PARA_MPRU.incrementmax = _MPR_U_incrementmax/10.00;  //DOT1ÊôÐÔ

	PI_PARA_DYNU.kp           = _DYN_U_Kp/100.00;			//DOT2ÊôÐÔ
    PI_PARA_DYNU.ki           = _DYN_U_Ki/1000.00;			//DOT3ÊôÐÔ	//½«´í¾Í´í201105atzuoyun
    PI_PARA_DYNU.kd           = _DYN_U_Kd/10.00;			//DOT1ÊôÐÔ
	PI_PARA_DYNU.outmax       = _DYN_U_outmax;   							//¶¯Ì¬µçÑ¹»·²ÎÊý
	PI_PARA_DYNU.errmax       = _DYN_U_errmax/10.00;		//DOT1ÊôÐÔ
	PI_PARA_DYNU.errmin       = _DYN_U_errmin/1000.00;		//DOT3ÊôÐÔ
	PI_PARA_DYNU.incrementmax = _DYN_U_incrementmax/10.00;  //DOT1ÊôÐÔ

	_eidco   = _EIDCO/1000.00;
	_encodpos= _ENCODPOS/1000.00;
	_stdby01 = _STDBY1/1000.00;				//±¸ÓÃ¾­¹ýÐ¡Êýµã´¦ÀíºóÖµ
	_stdby02 = _STDBY2/100.00;				//±¸ÓÃ¾­¹ýÐ¡Êýµã´¦ÀíºóÖµ
	_stdby03 = _STDBY3/10.00;				//±¸ÓÃ¾­¹ýÐ¡Êýµã´¦ÀíºóÖµ
    _stdby04 = _STDBY4;			        	//±¸ÓÃ
	_stdby05 = _STDBY5;			        	//±¸ÓÃ

} 
/*********************************************************************************************************
** º¯ÊýÃû³Æ: Disepwmio_NPR
** ¹¦ÄÜÃèÊö: 
** Êä¡¡Èë: 	 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡ß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Disepwmio_NPR(void)
{
	EALLOW;
	
	EPwm1Regs.AQCSFRC.bit.CSFA = 1;		//force low  AQCSFRC = Action Qualifier Continuous S/W force Register Set
	EPwm1Regs.AQCSFRC.bit.CSFB = 2;		//force high
	EPwm2Regs.AQCSFRC.bit.CSFA = 1;		//force low
	EPwm2Regs.AQCSFRC.bit.CSFB = 2;		//force high
	EPwm3Regs.AQCSFRC.bit.CSFA = 1;		//force low
	EPwm3Regs.AQCSFRC.bit.CSFB = 2;		//force high

	EPwm1Regs.TBCTR = 0x0000;           // Clear counter  TBCTR = Time-base Counter Register
    EPwm2Regs.TBCTR = 0x0000;           // Clear counter
    EPwm3Regs.TBCTR = 0x0000;           // Clear counter

	EDIS;
}  
/*********************************************************************************************************
** º¯ÊýÃû³Æ: Disepwmio_MPR
** ¹¦ÄÜÃèÊö: 
** Êä¡È? 	 
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
void Disepwmio_MPR(void)
{
	EALLOW;
	
	EPwm4Regs.AQCSFRC.bit.CSFA = 1;		//force low  AQCSFRC = Action Qualifier Continuous S/W force Register Set
	EPwm4Regs.AQCSFRC.bit.CSFB = 2;		//force high
	EPwm5Regs.AQCSFRC.bit.CSFA = 1;		//force low
	EPwm5Regs.AQCSFRC.bit.CSFB = 2;		//force high
	EPwm6Regs.AQCSFRC.bit.CSFA = 1;		//force low
	EPwm6Regs.AQCSFRC.bit.CSFB = 2;		//force high 
  
    EPwm4Regs.TBCTR = 0x0000;           // Clear counter  TBCTR = Time-base Counter Register
    EPwm5Regs.TBCTR = 0x0000;           // Clear counter
    EPwm6Regs.TBCTR = 0x0000;           // Clear counter 

	EDIS;
}  
/*********************************************************************************************************
** º¯ÊýÃû³Æ£º DisPwm
** ¹¦ÄÜÃèÊö£ºÂö³å½ûÖ¹
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
void DisPwm(void)
{
	Disepwmio_NPR();
	Disepwmio_MPR();
} 


/*********************************************************************************************************
** º¯ÊýÃû³Æ: Enepwmio_NPR
** ¹¦ÄÜÃèÊö: 
** Êä¡¡Èë: 	 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡Æ?
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Enepwmio_NPR(void)
{
	EALLOW;

	EPwm1Regs.AQCSFRC.bit.CSFA = 3;		//forcing disabled  AQCSFRC = Action Qualifier Continuous S/W force Register Set
	EPwm1Regs.AQCSFRC.bit.CSFB = 3;		//forcing disabled
	EPwm2Regs.AQCSFRC.bit.CSFA = 3;		//forcing disabled
	EPwm2Regs.AQCSFRC.bit.CSFB = 3;		//forcing disabled
	EPwm3Regs.AQCSFRC.bit.CSFA = 3;		//forcing disabled
	EPwm3Regs.AQCSFRC.bit.CSFB = 3;		//forcing disabled

	EDIS;
}  
/*********************************************************************************************************
** º¯ÊýÃû³Æ£ºEnepwmio_MPR
** ¹¦ÄÜÃèÊö: 
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
void Enepwmio_MPR(void)
{
	EALLOW;
	
	EPwm4Regs.AQCSFRC.bit.CSFA = 3;		//forcing disabled
	EPwm4Regs.AQCSFRC.bit.CSFB = 3;		//forcing disabled
	EPwm5Regs.AQCSFRC.bit.CSFA = 3;		//forcing disabled
	EPwm5Regs.AQCSFRC.bit.CSFB = 3;		//forcing disabled
	EPwm6Regs.AQCSFRC.bit.CSFA = 3;		//forcing disabled
	EPwm6Regs.AQCSFRC.bit.CSFB = 3;		//forcing disabled

	EDIS;
}    
/*********************************************************************************************************
** º¯ÊýÃû³Æ: ConfigPwm
** ¹¦ÄÜÃèÊö: ½øÐÐPWM¿ª¹ØÆµÂÊºÍËÀÇøÊ±¼äµÄÉèÖÃ
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
void ConfigPwm(void)						
{
	Disepwmio_NPR();
	Disepwmio_MPR();
	EALLOW;
//----------NPRµÄPWM1-PWM6ÉèÖÃ---------------//
    EPwm1Regs.TBPRD = 3750000/_SW_FR;           // ¿ª¹ØÆµÂÊÎª²Ù×÷Æ÷¸ø¶¨,_SW_FR=200¶ÔÓ¦2kHz
    EPwm2Regs.TBPRD = 3750000/_SW_FR;           // PWMÊ±ÖÓÎª75MHz
	EPwm3Regs.TBPRD = 3750000/_SW_FR;           // TBPRD = Time-base Period Register
    SW_NPR=3750000.0/_SW_FR;

    EPwm1Regs.DBRED = 75 * _DEADTIME;              //ËÀÇø£¬_DEADTIMEµ¥Î»Îªus 
    EPwm1Regs.DBFED = 75 * _DEADTIME;              //
    EPwm2Regs.DBRED = 75 * _DEADTIME;              //DBRED = Dead-Band Generator Rising Edge Delay Count Register
    EPwm2Regs.DBFED = 75 * _DEADTIME;              //DBFED = Dead-Band Generator Falling Edge Delay Count Register
    EPwm3Regs.DBRED = 75 * _DEADTIME;
    EPwm3Regs.DBFED = 75 * _DEADTIME;   

//----------MPRµÄPWM7-PWM12ÉèÖÃ---------------//
	EPwm4Regs.TBPRD = 3750000/_SW_FR;           // 
    EPwm5Regs.TBPRD = 3750000/_SW_FR;           // 
    EPwm6Regs.TBPRD = 3750000/_SW_FR;           // 
    SW_MPR=3750000.0/_SW_FR;

    EPwm4Regs.DBRED = 75 * _DEADTIME;
    EPwm4Regs.DBFED = 75 * _DEADTIME;
    EPwm5Regs.DBRED = 75 * _DEADTIME;
    EPwm5Regs.DBFED = 75 * _DEADTIME;
    EPwm6Regs.DBRED = 75 * _DEADTIME;
    EPwm6Regs.DBFED = 75 * _DEADTIME; 

   GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;  // GPIO0 = PWM1A
   GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;  // GPIO1 = PWM1B
   GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;  // GPIO2 = PWM2A
   GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;  // GPIO3 = PWM2B
   GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;  // GPIO4 = PWM3A
   GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;  // GPIO5 = PWM3B 
   GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1;  // GPIO6 = PWM4A
   GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;  // GPIO7 = PWM4B
   GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;  // GPIO8 = PWM5A
   GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;  // GPIO9 = PWM5B
   GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;  // GPIO10 = PWM6A
   GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;  // GPIO11 = PWM6B  
    
	EDIS; 
}

/*********************************************************************************************************
** º¯ÊýÃû³Æ: EnPdpint
** ¹¦ÄÜÃèÊö: 
** Êä¡¡Èë: 	 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡? 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EnPdpint(void)
{
	EALLOW;
	
   // Enable TZ interrupt
    EPwm1Regs.TZEINT.bit.OST = 1;
    EPwm2Regs.TZEINT.bit.OST = 1;
    EPwm3Regs.TZEINT.bit.OST = 1;
	EPwm4Regs.TZEINT.bit.OST = 1;
    EPwm5Regs.TZEINT.bit.OST = 1;
    EPwm6Regs.TZEINT.bit.OST = 1;

	EDIS;
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: DisPdpint
** ¹¦ÄÜÃèÊö: 
** Êä¡¡Èë: 	 
** Êä¡¡:   
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void DisPdpint(void)
{
	EALLOW;
	
   // disable TZ interrupt
    EPwm1Regs.TZEINT.bit.OST = 0;
    EPwm2Regs.TZEINT.bit.OST = 0;
    EPwm3Regs.TZEINT.bit.OST = 0;
	EPwm4Regs.TZEINT.bit.OST = 0;
    EPwm5Regs.TZEINT.bit.OST = 0;
    EPwm6Regs.TZEINT.bit.OST = 0;

	EDIS;
} 
/*********************************************************************************************************
** º¯ÊýÃû³Æ: ClrPdpint
** ¹¦ÄÜÃèÊö: 
** Êä¡¡Èë: 	 
** Êä¡³?   
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void ClrPdpint(void)
{
	EALLOW;
	
    EPwm1Regs.TZCLR.bit.OST = 1;   
    EPwm1Regs.TZCLR.bit.INT = 1;
	EPwm2Regs.TZCLR.bit.OST = 1;   
    EPwm2Regs.TZCLR.bit.INT = 1;
	EPwm3Regs.TZCLR.bit.OST = 1;   
    EPwm3Regs.TZCLR.bit.INT = 1;
	EPwm4Regs.TZCLR.bit.OST = 1;   
    EPwm4Regs.TZCLR.bit.INT = 1;
	EPwm5Regs.TZCLR.bit.OST = 1;   
    EPwm5Regs.TZCLR.bit.INT = 1;
	EPwm6Regs.TZCLR.bit.OST = 1;   
    EPwm6Regs.TZCLR.bit.INT = 1;

	EDIS;
}   

//===========================================================================
// No more.
//==========================================================================