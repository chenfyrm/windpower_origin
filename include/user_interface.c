/****************************************Copyright (c)**************************************************
**                       		     ±±	¾©	½»	Í¨	´ó	Ñ§
**                                        µçÆø¹¤³ÌÑ§Ôº
**                                         604ÊµÑéÊÒ
**
**                              
** 
**--------------ÎÄ¼þÐÅÏ¢--------------------------------------------------------------------------------
**ÎÄ   ¼þ   Ãû: user_interface.c
**´´   ½¨   ÈË: 
**×îºóÐÞ¸ÄÈÕÆÚ: 
**Ãè        Êö: ÓÒÓñ1.5MWË«À¡·çµç»ú×é²¢Íø±äÁ÷Æ÷¿ØÖÆÈí¼þÍâÉè¿ØÖÆ³ÌÐò--201005atcpc-201007atbjtu
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
#include "math.h"				//20121103
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
		
   	    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//Ê±
		
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
** º¯ÊýÃû³Æ: CRC-16
** ¹¦ÄÜÃèÊö:¼ÆËãÊý¾ÝÁ÷* pDataµÄ16Î»CRCÐ£ÑéÂë£¬Êý¾ÝÁ÷³¤¶ÈÎªnLength 
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
const unsigned char auchCRCHi[] =
{
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,0x40, 0x01, 
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40
} ;

const unsigned  char auchCRCLo[] = 
{
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 
	0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 
	0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 
	0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 
	0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 
	0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 
	0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 
	0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 
	0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 
	0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 
	0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 
	0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 
	0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 
	0x4C, 0x8C, 0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40
} ;

/*********************************************************************************************************
** º¯ÊýÃû³Æ: CRC-16
** ¹¦ÄÜÃèÊö:¼ÆËãÊý¾ÝÁ÷* pDataµÄ16Î»CRCÐ£ÑéÂë£¬Êý¾ÝÁ÷³¤¶ÈÎªnLength 
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
unsigned short GetCrc_16(unsigned int *puchMsg , unsigned int usDataLen)
{
   unsigned char uchCRCHi = 0xFF ;    		/* high byte of CRC initialized */
   unsigned char uchCRCLo = 0xFF ;		 /* low byte of CRC initialized */
   unsigned uIndex ; 					/* will index into CRC lookup table */
   while (usDataLen--)				/* pass through message buffer */
   {
     uIndex = uchCRCHi ^ *puchMsg++ ; 				/* calculate the CRC */
     uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
     uchCRCLo = auchCRCLo[uIndex] ;
   }
  return (uchCRCLo << 8 | uchCRCHi) ;
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
	ScibRegs.SCIFFTX.all=0xC000;			// Reset TX FIFO's
	ScibRegs.SCICCR.all = 0x0007;			// 1 stop bit, No parity, 8-bit character, No loopback
	
	ScibRegs.SCIHBAUD = (temp16&0xFF00)>>8;	// BAUDRATE
	ScibRegs.SCILBAUD = temp16&0x00FF;
	
	ScibRegs.SCIFFTX.bit.TXFIFOXRESET=1;	// Re-enable TX FIFO operation
	
	ScibRegs.SCICTL1.all =0x0022;     		// Relinquish SCI from Reset
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
	Uint16 temp16;			//20100727¼ÓÈë²¨ÌØÂÊÉè¶¨
	Uint32 temp32;

//----------------------------------------------------------------------------
	M_EN485RXD();
	M_EnRxb();
	
	temp32=LSPCLK/8;
	temp16=temp32/_SCIB_BAUD-1;				// LSPCLK/(8*BAUD)-1	
	ScibRegs.SCIHBAUD = (temp16&0xFF00)>>8;	// BAUDRATE
	ScibRegs.SCILBAUD = temp16&0x00FF;
//----------------------------------------------------------------------------

	ScibRegs.SCIFFRX.all=0x000A;			//   different!!!Reset RX FIFO's//RXFFIL4--0: Receive FIFO interrupt level ==10	
	ScibRegs.SCIFFRX.bit.RXFIFORESET=1;		// Receive FIFO reset :Re-enable RX FIFO operation
	
	M_ClrRxFifoOvr();
	
	ScibRegs.SCICTL1.all =0x0021;     		// Relinquish SCI from Reset //bit5:SCI software reset
//----------------------------------------------------------------------------
}


/*********************************************************************************************************
** º¯ÊýÃû³Æ: READSTATE
** ¹¦ÄÜÃèÊö: ¶ÁÈ¡±äÁ÷Æ÷×´Ì¬-¿ª¹ØÁ¿
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
void READSTATE(void)
{
//----------------------------------------------------------------------------
//----COMMAND1------------------------------------------------------------------
	if(M_ChkFlag(SL_ENPCOPER)!=0)	  	_COMMAND1 |= 0x0001;	//0 PC¿ØÖÆÊ¹ÄÜ   0-CAN¿ØÖÆ
	else								_COMMAND1 &= 0xFFFE;
	if(M_ChkFlag(SL_OPERATION)!=0)	  	_COMMAND1 |= 0x0002;	//1 ÔËÐÐÄ£Ê½Ñ¡Ôñ 0-±³¿¿±³ÔËÐÐ
	else								_COMMAND1 &= 0xFFFD;
	if(M_ChkFlag(SL_OCS_EIN)!=0)	  	_COMMAND1 |= 0x0004;	//2 Ö÷¶ÏÂ·Æ÷¿ØÖÆÖ¸ÁîCANÄ£Ê½ÏÂÊ¹ÓÃ
	else								_COMMAND1 &= 0xFFFB;
	if(M_ChkFlag(SL_OCS_SYSRUN)!=0)	  	_COMMAND1 |= 0x0008;	//3 Í¬²½²¢ÍøÖ¸ÁîCANÄ£Ê½ÏÂÊ¹ÓÃ
	else								_COMMAND1 &= 0xFFF7;
	if(M_ChkFlag(SL_OCS_PREC)!=0)	  	_COMMAND1 |= 0x0010;	//4 Ô¤³äµçÖ¸ÁîPCÄ£Ê½ÏÂÊ¹ÓÃ
	else								_COMMAND1 &= 0xFFEF;
	if(M_ChkFlag(SL_OCS_NPRSTART)!=0)	_COMMAND1 |= 0x0020;	//5 Íø²à±äÁ÷Æ÷Æô¶¯PCÄ£Ê½ÏÂÊ¹ÓÃ
	else								_COMMAND1 &= 0xFFDF;
	if(M_ChkFlag(SL_OCS_MPRSTART)!=0)	_COMMAND1 |= 0x0040;	//6 »ú²à±äÁ÷Æ÷Æô¶¯PCÄ£Ê½ÏÂÊ¹ÓÃ
	else								_COMMAND1 &= 0xFFBF;
	if(M_ChkFlag(SL_OCS_OPENSTAC)!=0)	_COMMAND1 |= 0x0080;	//7 ¶¨×Ó½Ó´¥Æ÷¿ØÖÆÖ¸ÁîPCÄ£Ê½ÏÂÊ¹ÓÃ
	else								_COMMAND1 &= 0xFF7F;
	if(M_ChkFlag(SL_OCS_RESET)!=0)		_COMMAND1 |= 0x0100;	//8 ±äÁ÷Æ÷¹ÊÕÏ¸´Î»Ö¸ÁîCAN/PCÄ£Ê½ÏÂÊ¹ÓÃ
	else								_COMMAND1 &= 0xFEFF;
	if(M_ChkFlag(SL_PC_ERRDATASAVE)!=0)	_COMMAND1 |= 0x0200;	//9 Íâ²¿RAMËø´æ¹ÊÕÏÊý¾ÝÖ¸Áî
	else								_COMMAND1 &= 0xFDFF;
	if(M_ChkFlag(SL_PC_ERRDATACLR)!=0)	_COMMAND1 |= 0x0400;	//10 Íâ²¿RAMÊÍ·Å¹ÊÕÏÊý¾ÝÖ¸Áî
	else								_COMMAND1 &= 0xFBFF;
	if(M_ChkFlag(SL_PC_BANKDATASAVE)!=0)_COMMAND1 |= 0x0800;	//11 Íâ²¿RAMËø´æÏÔÊ¾Êý¾ÝÖ¸Áî
	else								_COMMAND1 &= 0xF7FF;
	if(M_ChkFlag(SL_PCOCS_CBOFF)!=0)	_COMMAND1 |= 0x1000;	//12 Ö¸ÁîPCÄ£Ê½ÏÂÊ¹ÓÃ PC¿ØÖÆÊÖ¶¯¶Ï¿ªCB 1=¶Ï¿ª 2010atcpc
	else								_COMMAND1 &= 0xEFFF;

	
//----STATE1------------------------------------------------------------------
	if(M_ChkFlag(SL_RUN)!=0)	  		_STATE1 |= 0x0001;	//±äÁ÷Æ÷·¢Âö³åÔËÐÐ
	else								_STATE1 &= 0xFFFE;
	if(M_ChkFlag(SL_ERROR)!=0)	  		_STATE1 |= 0x0002;	//±äÁ÷Æ÷´¦ÓÚ¹ÊÕÏ
	else								_STATE1 &= 0xFFFD;
	if(M_ChkFlag(SL_CBCLOSED)!=0)	  	_STATE1 |= 0x0004;	//Ö÷¶ÏÂ·Æ÷±ÕºÏÍê³É
	else								_STATE1 &= 0xFFFB;
	if(M_ChkFlag(SL_CHARGEOK)!=0)	  	_STATE1 |= 0x0008;	//Ô¤³äµçÍê³É
	else								_STATE1 &= 0xFFF7;
	if(M_ChkFlag(SL_NPR_PWMOUT)!=0)	  	_STATE1 |= 0x0010;	//Íø²à±äÁ÷Æ÷ÔËÐÐ
	else								_STATE1 &= 0xFFEF;
	if(M_ChkFlag(SL_STEADYFB)!=0)	  	_STATE1 |= 0x0020;	//ÖÐ¼äÖ±Á÷µçÑ¹ÎÈ¶¨
	else								_STATE1 &= 0xFFDF;
	if(M_ChkFlag(SL_MPR_PWMOUT)!=0)	  	_STATE1 |= 0x0040;	//»ú²à±äÁ÷Æ÷ÔËÐÐ
	else								_STATE1 &= 0xFFBF;
	if(M_ChkFlag(SL_MPR_SYNOK)!=0)	  	_STATE1 |= 0x0080;	//¶¨×Ó²àÍ¬²½²¢ÍøÍê³É
	else								_STATE1 &= 0xFF7F;
	if(M_ChkFlag(SL_SPEED_IN_RANGE)!=0)	_STATE1 |= 0x0100;	//µç»ú×ªËÙÔÚÔËÐÐ·¶Î§ÄÚ
	else								_STATE1 &= 0xFEFF;
	if(M_ChkFlag(SL_RAMERRORSAVE)!=0)	_STATE1 |= 0x0200;	//Íâ²¿RAM¹ÊÕÏÐÅÏ¢Íê³É´æ´¢±êÖ¾
	else								_STATE1 &= 0xFDFF;
	if(M_ChkFlag(SL_RAMBANKSAVE)!=0)	_STATE1 |= 0x0400;	//Íâ²¿RAMÏÔÊ¾ÐÅÏ¢Ëø´æ±êÖ¾
	else								_STATE1 &= 0xFBFF;
//----MagnetCurve2013-12-13
	if(M_ChkFlag(SL_MC_OK)!=0)			_STATE1 |= 0x0800;	//Àø´ÅÇúÏßMagnetCurve200909
	else								_STATE1 &= 0xF7FF;
	if(M_ChkFlag(SL_MC_FAIL)!=0)		_STATE1 |= 0x1000;	//Àø´ÅÇúÏßMagnetCurve200909
	else								_STATE1 &= 0xEFFF;

//----DISPLAY------------------------------------------------------------------
	if(M_ChkFlag(SL_DISPLAY0)!=0)	  	_DISPLAY |= 0x0001;	//Íø²à±äÁ÷Æ÷ÔËÐÐ
	else								_DISPLAY &= 0xFFFE;
	if(M_ChkFlag(SL_DISPLAY1)!=0)	  	_DISPLAY |= 0x0002;	//»ú²à±äÁ÷Æ÷ÔËÐÐ
	else								_DISPLAY &= 0xFFFD;
	if(M_ChkFlag(SL_DISPLAY2)!=0)	  	_DISPLAY |= 0x0004;	//ÖÐ¼äÖ±Á÷µçÑ¹ÎÈ¶¨
	else								_DISPLAY &= 0xFFFB;
	if(M_ChkFlag(SL_DISPLAY3)!=0)	  	_DISPLAY |= 0x0008;	//ÑÏÖØ¹ÊÕÏ
	else								_DISPLAY &= 0xFFF7;
	if(M_ChkFlag(SL_DISPLAY4)!=0)	  	_DISPLAY |= 0x0010;	//¶¨×Ó²à´ïµ½Í¬²½²¢ÍøÌõ¼þ
	else								_DISPLAY &= 0xFFEF;
	if(M_ChkFlag(SL_DISPLAY5)!=0)	  	_DISPLAY |= 0x0020;	//¹ÊÕÏ
	else								_DISPLAY &= 0xFFDF;
	if(M_ChkFlag(SL_DISPLAY6)!=0)	  	_DISPLAY |= 0x0040;	//LVRT¾§Õ¢¹Ü¿ªÍ¨Ö¸Ê¾
	else								_DISPLAY &= 0xFFBF;
	if(M_ChkFlag(SL_DISPLAY7)!=0)	  	_DISPLAY |= 0x0080;	//µÈ´ýÔ¤³äµçÖ¸ÁîÖ¸Ê¾
	else								_DISPLAY &= 0xFF7F;
			
//----IO/INPUT------------------------------------------------------------------
	if(M_ChkFlag(SL_IN1_CBSTS)!=0)	  	_INPUT |= 0x0001;	//P4.1Ö÷¶ÏÂ·Æ÷×´Ì¬·´À¡£º	1=±ÕºÏ£»0=¶Ï¿ª
	else								_INPUT &= 0xFFFE;
	if(M_ChkFlag(SL_IN1_CBRESET)!=0)	_INPUT |= 0x0002;	//P4.2Ö÷¿ØÕýÔÚ¸´Î»Ö÷¶ÏÂ·Æ÷£º1=¸´Î»£»0=Î´¸´Î»
	else								_INPUT &= 0xFFFD;
	if(M_ChkFlag(SL_IN1_STATORK)!=0)	_INPUT |= 0x0004;	//P4.3¶¨×Ó½Ó´¥Æ÷×´Ì¬·´À¡£º	1=±ÕºÏ£»0=¶Ï¿ª
	else								_INPUT &= 0xFFFB;
	if(M_ChkFlag(SL_IN1_MIANK)!=0)	  	_INPUT |= 0x0008;	//P4.4Ö÷½Ó´¥Æ÷×´Ì¬·´À¡£º	1=±ÕºÏ£»0=¶Ï¿ª
	else								_INPUT &= 0xFFF7;
	if(M_ChkFlag(SL_IN1_MIANFILTER)!=0)	_INPUT |= 0x0010;	//P4.5Ö÷ÂË²¨Æ÷×´Ì¬·´À¡£º	1=±ÕºÏ£»0=¶Ï¿ª
	else								_INPUT &= 0xFFEF;
	if(M_ChkFlag(SL_IN1_EXFAULTOK)!=0)	_INPUT |= 0x0020;	//P4.6Íâ²¿¹ÊÕÏÁ´×´Ì¬·´À¡£º	1=Õý³££¬0=¹ÊÕÏ	
	else								_INPUT &= 0xFFDF;
	if(M_ChkFlag(SL_IN1_MIANFAN)!=0)	_INPUT |= 0x0040;	//P4.7¹¦ÂÊ·ç»ú×´Ì¬·´À¡£º	1=Õý³££¬0=¹ÊÕÏ 
	else								_INPUT &= 0xFFBF;
	if(M_ChkFlag(SL_IN1_EXESTOP)!=0)	_INPUT |= 0x0080;	//P4.8Íâ²¿¼±Í£¹ÊÕÏ·´À¡£º 	1=¼±Í£°´Å¥°´ÏÂ£¬0=¼±Í£°´Å¥°ÎÆð
	else								_INPUT &= 0xFF7F;
	if(M_ChkFlag(SL_IN3_CBTRIP)!=0)		_INPUT |= 0x0100;	//P4.9Ö÷¶Ï¹ýÁ÷ÍÑ¿Û¹ÊÕÏ·´À¡£º1=¹ÊÕÏ£»0=Õý³£
	else								_INPUT &= 0xFEFF;

//----IO/OUTPUT------------------------------------------------------------------
	if(M_ChkFlag(CL_CBLVC)!=0)	  		_OUTPUT |= 0x0001;	//P5.5Ö÷¶ÏÂ·Æ÷Ç·Ñ¹Á´¿ª¹ØÖ¸Áî£º	1=±ÕºÏ£»0=¶Ï¿ª
	else								_OUTPUT &= 0xFFFE;
	if(M_ChkFlag(CL_CBON)!=0)	  		_OUTPUT |= 0x0002;	//P5.6Ö÷¶ÏÂ·Æ÷ºÏÕ¢Ö¸Áî£º		1=ºÏÕ¢
	else								_OUTPUT &= 0xFFFD;
	if(M_ChkFlag(CL_PRE)!=0)	  		_OUTPUT |= 0x0004;	//P5.7Ô¤³äµçÖ¸Áî£º				1=Ô¤³äµç£»0=½ûÖ¹Ô¤³äµç
	else								_OUTPUT &= 0xFFFB;
	if(M_ChkFlag(CL_MAINK)!=0)	  		_OUTPUT |= 0x0008;	//P5.8Ö÷½Ó´¥Æ÷ºÍÂË²¨Æ÷Æ÷Ö¸Áî£º	1=±ÕºÏ£»0=¶Ï¿ª
	else								_OUTPUT &= 0xFFF7;
	if(M_ChkFlag(CL_STATORK)!=0)	  	_OUTPUT |= 0x0010;	//P5.9¶¨×Ó½Ó´¥Æ÷Ö¸Áî£º			1=±ÕºÏ£»0=¶Ï¿ª
	else								_OUTPUT &= 0xFFEF;
	if(M_ChkFlag(CL_CBFCON)!=0)	  		_OUTPUT |= 0x0020;	//P5.10Ö÷¶ÏÂ·Æ÷Ç·Ñ¹Á´¿ØÖÆ£¬		1=±ÕºÏ£»0=¶Ï¿ª
	else								_OUTPUT &= 0xFFDF;
	if(M_ChkFlag(CL_FANWORK)!=0)	  	_OUTPUT |= 0x0040;	//P5.11¹¦ÂÊ·çÉÈ¿ØÖÆ£º			1=ÔËÐÐ£»0=Í£Ö¹
	else								_OUTPUT &= 0xFFBF;

//----LVRT------------------------------------------------------------------
	if(M_ChkFlag(SL_LV_MSTOP)!=0)	  	_LVRT |= 0x0001;	//
	else								_LVRT &= 0xFFFE;
	if(M_ChkFlag(SL_LV_NSTOP)!=0)	  	_LVRT |= 0x0002;	//
	else								_LVRT &= 0xFFFD;
	if(M_ChkFlag(SL_LV_SCRON)!=0)	  	_LVRT |= 0x0004;	//
	else								_LVRT &= 0xFFFB;
	if(M_ChkFlag(SL_LV_SCRRUNING)!=0)	_LVRT |= 0x0008;	//
	else								_LVRT &= 0xFFF7;
	if(M_ChkFlag(SL_LV_SCRIZERO)!=0)	_LVRT |= 0x0010;	//
	else								_LVRT &= 0xFFEF;
	if(M_ChkFlag(SL_LV_MPWMOUT)!=0)	    _LVRT |= 0x0020;	//
	else								_LVRT &= 0xFFDF;
	if(M_ChkFlag(SL_LV_STATE)!=0)	  	_LVRT |= 0x0040;	//
	else								_LVRT &= 0xFFBF;
//----------------------------------------------------------------------------
}

/*********************************************************************************************************
** º¯ÊýÃû³Æ: WRCOMMAND
** ¹¦ÄÜÃèÊö: Ð´Èë±äÁ÷Æ÷Ö¸Áî-¿ª¹ØÁ¿
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
void WRCOMMAND(void)
{
//----------------------------------------------------------------------------
 	if(M_ChkFlag(SL_ENPCOPER)!=0)		
 	{
		if((_COMMAND1&0x0002)!=0)	M_SetFlag(SL_OPERATION);
		else						M_ClrFlag(SL_OPERATION);
		if(((_COMMAND1&0x0004)!=0) && (M_ChkFlag(SL_ERROR)==0))	//201005atcpc
		{
	 		if(M_ChkCounter(MAIN_LOOP.cnt_ein,DELAY_EIN)>0)	M_SetFlag(SL_OCS_EIN);     		//ÑÓÊ±1s£¬·ÀÖ¹Îó²Ù×÷		
		}
		else 	
		{
			M_ClrFlag(SL_OCS_EIN);              
			MAIN_LOOP.cnt_ein=0;		
    	}
     
		if(((_COMMAND1&0x0008)!=0) && (M_ChkFlag(SL_ERROR)==0))		//201005atcpc					
		{ 
	  		if(M_ChkCounter(MAIN_LOOP.cnt_sysrun,DELAY_SYSRUN)>0)	 M_SetFlag(SL_OCS_SYSRUN); //ÑÓÊ±1s£¬·ÀÖ¹Îó²Ù×÷
		}				
		else 	
		{
			M_ClrFlag(SL_OCS_SYSRUN);
			MAIN_LOOP.cnt_sysrun=0;		
    	}
	
		if(((_COMMAND1&0x0010)!=0) && (M_ChkFlag(SL_ERROR)==0))		//201005atcpc					
		{ 
	  		if(M_ChkCounter(MAIN_LOOP.cnt_prec,DELAY_PREC)>0)	 M_SetFlag(SL_OCS_PREC); //ÑÓÊ±1s£¬·ÀÖ¹Îó²Ù×÷
		}				
		else 	
		{
			M_ClrFlag(SL_OCS_PREC);
			MAIN_LOOP.cnt_prec=0;		
   		} 

		if(((_COMMAND1&0x0020)!=0) && (M_ChkFlag(SL_ERROR)==0))		//201005atcpc					
		{ 
	  		if(M_ChkCounter(MAIN_LOOP.cnt_nprcmd,DELAY_NPRCMD)>0)	 M_SetFlag(SL_OCS_NPRSTART); //ÑÓÊ±1s£¬·ÀÖ¹Îó²Ù×÷
		}				
		else 	
		{
			M_ClrFlag(SL_OCS_NPRSTART);
			MAIN_LOOP.cnt_nprcmd=0;		
    	} 

		if(((_COMMAND1&0x0040)!=0) && (M_ChkFlag(SL_ERROR)==0))		//201005atcpc					
		{ 
	  		if(M_ChkCounter(MAIN_LOOP.cnt_mprcmd,DELAY_MPRCMD)>0)	 M_SetFlag(SL_OCS_MPRSTART); //ÑÓÊ±1s£¬·ÀÖ¹Îó²Ù×÷
		}				
		else 	
		{
			M_ClrFlag(SL_OCS_MPRSTART);
			MAIN_LOOP.cnt_mprcmd=0;		
    	} 

		if((_COMMAND1&0x0080)!=0)							
		{ 
	  		if(M_ChkCounter(MAIN_LOOP.cnt_clostacmd,DELAY_CLOSTACMD)>0)	 M_SetFlag(SL_OCS_OPENSTAC); //ÑÓÊ±1s£¬·ÀÖ¹Îó²Ù×÷
		}				
		else 	
		{
			M_ClrFlag(SL_OCS_OPENSTAC);
			MAIN_LOOP.cnt_clostacmd=0;		
    	} 

		if((_COMMAND1&0x0100)!=0)																	//ÉÏÎ»»úÒªÇó¸´Î»¹ÊÕÏ,ÓÉÍâ²¿I/O¸ø¶¨
		{ 
	  		if(M_ChkCounter(MAIN_LOOP.cnt_reset,DELAY_RESET)>0)	 M_SetFlag(SL_OCS_RESET);     		//ÑÓÊ±2s£¬·ÀÖ¹Îó²Ù×÷
		}				
		else 	
		{
			M_ClrFlag(SL_OCS_RESET);
			MAIN_LOOP.cnt_reset=0;		
    	}	
    	
		if((_COMMAND1&0x1000)!=0)	M_SetFlag(SL_PCOCS_CBOFF);	 //12 Ö¸ÁîPCÄ£Ê½ÏÂÊ¹ÓÃ PC¿ØÖÆÊÖ¶¯¶Ï¿ªCB 1=¶Ï¿ª 2010atcpc
		else						M_ClrFlag(SL_PCOCS_CBOFF);
   	}

	if((_COMMAND1&0x0200)!=0)	M_SetFlag(SL_PC_ERRDATASAVE);	 //9 Íâ²¿RAMËø´æ¹ÊÕÏÊý¾ÝÖ¸ÁîPC/CANÄ£Ê½ÏÂ¶¼¿ÉÊ¹ÓÃ
	else						M_ClrFlag(SL_PC_ERRDATASAVE);	
	if((_COMMAND1&0x0400)!=0)	M_SetFlag(SL_PC_ERRDATACLR);	 //10 Íâ²¿RAMÊÍ·Å¹ÊÕÏÊý¾ÝÖ¸ÁîPC/CANÄ£Ê½ÏÂ¶¼¿ÉÊ¹ÓÃ
//	else						M_ClrFlag(SL_PC_ERRDATACLR);
	else if(M_ChkFlag(SL_LV_CLRERRAM)==0)			M_ClrFlag(SL_PC_ERRDATACLR);		//20130306
//	if((_COMMAND1&0x0800)!=0)	M_SetFlag(SL_PC_BANKDATASAVE);	 //11 Íâ²¿RAMËø´æÏÔÊ¾Êý¾ÝÖ¸ÁîPC/CANÄ£Ê½ÏÂ¶¼¿ÉÊ¹ÓÃ
//	else						M_ClrFlag(SL_PC_BANKDATASAVE);

//20130724
	if((_COMMAND1&0x0800)!=0)
	{
		M_SetFlag(SL_PC_BANKDATASAVE);
		M_SetFlag(SL_TRIG_ERRDSAVE);	 //11 Íâ²¿RAMËø´æÏÔÊ¾Êý¾ÝÖ¸ÁîPC/CANÄ£Ê½ÏÂ¶¼¿ÉÊ¹ÓÃ
	}
	else						M_ClrFlag(SL_PC_BANKDATASAVE);

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
	Uint16 response,i,j;
	Uint16 rx_command,rx_startnum,rx_opernum,rx_crc,cRc_16;
	Uint16 rx_ramwavenum,rx_ramsegstart,error_pos,bank_pos;
	
	
//------------------------------------------------------------------------------------------------		
		if(SCI.rxb[0]!=_SCIB_ADDR)	//ÉÏÎ»»ú&½ÚµãµØÖ·²»·û,²¢·Ç²Ù×÷Æ÷·¢µÄÊý¾Ý
			response=0;				//²»½øÐÐ´¦Àí»Ø¸´,µÈ´ýÏÂÒ»Ö¡Êý¾Ý
		
		else							//²Ù×÷Æ÷¿ØÖÆ»òÉÏÎ»»ú¿ØÖÆÇÒ½ÚµãµØÖ·Ïà·û
		{
			SCI.txb[0] = SCI.rxb[0];								//ÏÂÎ»»úµØÖ·×Ö½Ú
			SCI.txb[1] = SCI.rxb[1];
			
			rx_crc=SCI.rxb[7];										//±£´æÐ£ÑéºÍ1¸ö×Ö16Î»
			rx_crc=((rx_crc<<8)&0xFF00)|SCI.rxb[6];					//[6][7]two words

			length485=SCI485NUM_RX-2;
            cRc_16=GetCrc_16(&SCI.rxb[0],length485);				//CRCÐ£Ñé
           
            if(rx_crc!=cRc_16)
              response=0;	
           
            else
            {
                response=1;
				
				//½ÓÊÕÊý¾Ý×ª´æ
				rx_command=SCI.rxb[1];							//±£´æÃüÁî×Ö
				
				rx_startnum=SCI.rxb[2];							//±£´æ¹¦ÄÜÂëÐòºÅ
				rx_startnum=((rx_startnum<<8)&0xFF00)|SCI.rxb[3];
			    
				rx_opernum=SCI.rxb[4];							//±£´æ¹¦ÄÜÂëÖµ
				rx_opernum=((rx_opernum<<8)&0xFF00)|SCI.rxb[5];
				
				//ÃüÁî×Ö¦Àí
				
				switch (rx_command&0xFFFF)					//ÃüÁî½âÎö???¸úFFFF×öÓëÔËËãÃ»ÓÐ×÷ÓÃ°¡£¿£¿£¿
				{
//--------------------NO.1--READ------
					case COM_RDAO:		//¶ÁÄ£ÄâÁ¿
					{
						 if((rx_startnum+rx_opernum)>PARA_NUM)
                              response=0;
						 
						 else
						 {
						    
						    j=0; 
							READSTATE();							    
							for(i=rx_startnum;i<(rx_startnum+rx_opernum);i++)
                            {
								SCI.txb[3+j]=(*FUNC[i].para_add&0xFF00)>>8; //201005atcpc ±£Ö¤ÄÜÓëCANÍ¬Ê±Í¨Ñ¶
								SCI.txb[4+j]=(*FUNC[i].para_add)&0x00FF;	//Ö±½Ó½«RAMÖÐµÄÊý¾Ý·¢ËÍ
								j++;
								j++; 
							}

							SCI.txb[2]=rx_opernum * 2;
							length485=SCI.txb[2]+3;
						
						    cRc_16=GetCrc_16(&SCI.txb[0],length485);
						
							SCI.txb[length485+1]=(cRc_16&0xFF00)>>8;  //-------------------------
							SCI.txb[length485]=cRc_16&0x00FF;
						   
						    length485=length485+2;
						}	
							
					break;
					}

//---------------------NO.3--WRITE A--------
					case COM_WRAO:			//Ð´Ä£ÄâÁ¿
					{		
							 if(rx_startnum>PARA_NUM)
                               response=0;
						     
						     else
					         {
					        	if((((FUNC[rx_startnum].attr&WR1)==0)&&((FUNC[rx_startnum].attr&WR0)==0)&&((FUNC[rx_startnum].attr&WR3)==0))||(((FUNC[rx_startnum].attr&WR1)==WR1)&&(M_ChkFlag(SL_RUN)!=0)))//·ÇÐ´²Ù×÷»òÔËÐÐÖÐ²»¿ÉÐÞ
									response=0;						//ÖÃ²Ù×÷Ê§°Ü±êÖ¾

								else
								{
					        		if((FUNC[rx_startnum].attr&WR3)==WR3)//ÊôÓÚÖ»ÐÞ¸ÄRAMÊý¾ÝµÄÐ´ÀàÐÍ
					        		{
								   		*FUNC[rx_startnum].para_add=rx_opernum;	//ÐÞ¸ÄRAMÀïµÄÖµ

					        			if(M_ChkFlag(SL_RUN)==0)			//ÔËÐÐÖÐ²»ÔÊÐíÐÞ¸Ä¿ØÖÆÄ£Ê½201005atcpc
										{
					        				if((_COMMAND1&0x0001)!=0)	M_SetFlag(SL_ENPCOPER);
											else						M_ClrFlag(SL_ENPCOPER);	
										}
					        			
					        			WRCOMMAND();	//Ö»ÓÐÊ¹ÄÜÁËPCOPERÖ®ºó²ÅÔÊÐíÐÞ¸ÄÆäËüÁ¿
					        		}
					        		else
					        		{
								   		*FUNC[rx_startnum].para_add=rx_opernum;	//ÐÞ¸ÄRAMºÍEEROM
								
										EEPROM.mcode=rx_startnum;			//Ð´Èë2×Ö½ÚÊý¾Ý
										M_SetFlag(SL_EEASK_MCODE);			//ÉèEEPROMÐÞ¸Ä¹¦ÄÜÂëÇëÇó±êÖ¾					        			
					        		}   
																	
									if((rx_startnum>=TIME_FIRST)&&(rx_startnum<=TIME_END))
										M_SetFlag(SL_SETRTIMER);			//ÖÃÊµÊ±Ê±ÖÓÐèÒªÖØÐÂÉè¶¨±êÖ¾
								
									SCI.txb[2]=SCI.rxb[2];					//×¼±¸·¢ËÍÊý¾Ý(²Ù×÷³É¹¦)
									SCI.txb[3]=SCI.rxb[3];
									SCI.txb[4]=SCI.rxb[4];
									SCI.txb[5]=SCI.rxb[5];
							   		
							    	length485=6;
									cRc_16=GetCrc_16(&SCI.txb[0],length485);
								
									SCI.txb[7]=(cRc_16&0xFF00)>>8;  
					        		SCI.txb[6]=cRc_16&0x00FF;
                                
                               	 	length485=length485+2;
								}
							}	
					break;
					}

//--------------------NO.3--READ exRAM------
					case COM_RDRAM:		//¶ÁexRAM
						   
						 if((rx_startnum+rx_opernum)>PARA_NUM)
                              response=0;
						 
						 else
						 {						    
						    j=0; 
						    i=0;
							rx_ramwavenum = rx_startnum;									//0-49´ú±íÊý¾Ý²¨ÐÎ,50Îª¹ÊÕÏÊ±¿ÌÊý¾Ý 201005atcpc
							rx_ramsegstart = rx_opernum * RAM_SEGMENT;						//±¾´Î²¨ÐÎ¶Î´«Êä¿ªÊ¼Êý¾ÝµØÖ· 0-(n-1)
							 
							if(rx_ramwavenum<25)											//BANK_RAM:0-24
							{
								bank_pos = BANK_RAMDATA_POS + rx_ramsegstart;				//±¾´Î²¨ÐÎ¶Î´«Êä¿ªÊ¼Êý¾ÝµØÖ·+¸Ã²¨ÐÎµÄ³õÊ¼µØÖ·
								if(bank_pos >= RAM_BIAS)  bank_pos=bank_pos-RAM_BIAS;		//error_pos=0-3999
								
								for(i=0;i<RAM_SEGMENT;i++)
								{
									SCI.txb[8+j]=(*(BANK_RAMSTART+((Uint32)RAM_BIAS * rx_ramwavenum + bank_pos))&0xFF00)>>8;
									SCI.txb[9+j]=(*(BANK_RAMSTART+((Uint32)RAM_BIAS * rx_ramwavenum + bank_pos)))&0x00FF;				
									j++;
									j++; 									
									bank_pos++;
									if(bank_pos >= RAM_BIAS)  bank_pos=0;					//bank_pos=0-3999
								}								
							}
							else if(rx_ramwavenum<50)										//ERROR_RAM:25-49 2010atcpc
							{								
								error_pos = ERROR_RAMDATA_POS + rx_ramsegstart;				//±¾´Î²¨ÐÎ¶Î´«Êä¿ªÊ¼Êý¾ÝµØÖ·+¸Ã²¨ÐÎµÄ³õÊ¼µØÖ·
								if(error_pos >= RAM_BIAS)  error_pos=error_pos-RAM_BIAS;	//error_pos=0-3999
								
								for(i=0;i<RAM_SEGMENT;i++)
								{
									SCI.txb[8+j]=(*(ERROR_RAMSTART+((Uint32)RAM_BIAS * (rx_ramwavenum-25) + error_pos))&0xFF00)>>8;
									SCI.txb[9+j]=(*(ERROR_RAMSTART+((Uint32)RAM_BIAS * (rx_ramwavenum-25) + error_pos)))&0x00FF;				
									j++;
									j++; 									
									error_pos++;
									if(error_pos >= RAM_BIAS)  error_pos=0;					//error_pos=0-3999
								}																
							}

							SCI.txb[2]=SCI.rxb[2];
							SCI.txb[3]=SCI.rxb[3];
							SCI.txb[4]=SCI.rxb[4];
							SCI.txb[5]=SCI.rxb[5];
							
//							SCI.txb[6]=((RAM_SEGMENT * 2)&0xFF00)>>8;
//							SCI.txb[7]=(RAM_SEGMENT * 2)&0x00FF;
							SCI.txb[6]=(j&0xFF00)>>8;
							SCI.txb[7]=j&0x00FF;
						
							length485=j+8;						
//							length485=RAM_SEGMENT * 2+8;						
						    cRc_16=GetCrc_16(&SCI.txb[0],length485);
						
							SCI.txb[length485+1]=(cRc_16&0xFF00)>>8;  
							SCI.txb[length485]=cRc_16&0x00FF;
						   
						    length485=length485+2;						//+CRC_16 2bits
						}								
					break;

					default:
					response=0;
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
	Uint16 i,j,response;

	if(M_ChkCounter(SCI.cnt_sciover,DELAY_SCIOVER)>=0)				//·¢ËÍ/½ÓÊÕ³¬Ê±?
	{
		SCI.cnt_sciover=0;
		M_ClrFlag(SL_TX);
		M_ClrFlag(SL_RX);
		Sci485_RxInit();											//485½ÓÊÕ³õÊ¼»¯
	}
	else if(M_ChkFlag(SL_TX)!=0)									//·¢ËÍ?
	{
		if(M_ChkFlag(SL_TXLONGDATA)==0)								//·¢ËÍÊý¾ÝÍê³É?
		{
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
	else															//½ÓÊÕ
	{
		if(ScibRegs.SCIFFRX.bit.RXFFST == 0)						//Î´¿ªÊ¼½ÓÊÕ»ò½ÓÊÕÍê³É?
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
							M_SetFlag(SL_TX);						//ÖÃ·¢ÎÎñ±êÖ¾Î»
							M_ClrFlag(SL_RX);
							
							Sci485_TxInit();						//485·¢ËÍ³õÊ¼»¯
							
//							for(i=0;i<length485;i++)				//different here!!!change
//							ScibRegs.SCITXBUF=SCI.txb[i];
							j=0;
							for(i=0;i<length485;i=j)				//have corected
							{                               
								if(ScibRegs.SCICTL2.bit.TXRDY==1)
								{
//									DELAY_US(150);
									ScibRegs.SCITXBUF=SCI.txb[i];
									j++;
								}
							}												
						}
					}
					else
					{
						response=ScibDatpro();						//µ÷ÓÃÊý¾Ý½âÎö³ÌÐò
						
						if(response==1)								//±íÊ¾ÒªÁ¢¼´»Ø¸´
						{
//							M_SetFlag(SL_TX);						//ÖÃ·¢ËÍÈÎÎñ±êÖ¾Î»
							M_ClrFlag(SL_RX);
							
							Sci485_TxInit();						//485·¢ËÍ³õÊ¼»¯
							
							if(length485<=16)
							{
								for(i=0;i<length485;i++)
									ScibRegs.SCITXBUF=SCI.txb[i];	//201005atcpc
							}
							else
							{
								for(i=0;i<15;i++)
									ScibRegs.SCITXBUF=SCI.txb[i];	//201005atcpc

								M_SetFlag(SL_TXLONGDATA);							
							}																					
							M_SetFlag(SL_TX);						//ÖÃ·¢ËÍÈÎÎñ±êÖ¾Î»120625
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
		else if((ScibRegs.SCIFFRX.bit.RXFFST >= 1)&&(M_ChkFlag(SL_HEADOK)==0))	//¿ªÊ¼½ÓÊÕÇÒ»¹Ã»ÓÐÊÕµ½±¨Í·?
		{
			SCI.rxb[0]=ScibRegs.SCIRXBUF.all&0x00FF;
			if(SCI.rxb[0]==_SCIB_ADDR)								//different!!! 
				M_SetFlag(SL_HEADOK);
			else
				Sci485_RxInit();									//485½ÓÊÕ³õÊ¼»¯
		}
		else if(ScibRegs.SCIFFRX.bit.RXFFST >= SCI485NUM_RX-1)		//change½ÓÊÕÍê³É?SCIFFRX.bit.RXFFST£ºReceive FIFO has x words
		{
			SCI.cnt_sciover=0;										//Çå³ý·¢ËÍ/½ÓÊÕ³¬Ê±¶¨Ê±Æ÷
			M_DisTxRxb();
			M_ClrRxFifoOvr();
			
			for(i=1;i<SCI485NUM_RX;i++)								//¶Á³ö½ÓÊÕ»º´æ(²»°üÀ¨±¨Í·) 9Î»Êý¾Ý
				SCI.rxb[i]=ScibRegs.SCIRXBUF.all&0x00FF;
			
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
	SciaRegs.SCICCR.all =0x0007;   			// 1 stop bit,No loopback 
                                  			// no parity,8 char bits
	SciaRegs.SCICTL1.all =0x0003;  			// Disable TX, RX, internal SCICLK, 
                                  			// Disable RX_ERR, SLEEP, TXWAKE
	SciaRegs.SCICTL2.all =0;		 		// fifo mode,they are ignored 

   	SciaRegs.SCIFFTX.all=0xC000;			// disable tx232_int,reset scia_fifo 
    SciaRegs.SCIFFRX.all=0x000A;			// disable rx232_int 
    SciaRegs.SCIFFCT.all=0x00;
    
	SciaRegs.SCICTL1.all =0x0023;     		// Relinquish SCI from Reset 

	SciaRegs.SCIFFTX.bit.TXFIFOXRESET=1;
	SciaRegs.SCIFFRX.bit.RXFIFORESET=1;
//-----------------------------------------------------------------------------
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
	Uint16 i,heartbeat,xor_data,temp;
	
	if(M_ChkCounter(SCI_canopen.cnt_sciover,DELAY_SCICANOPENOVER)>=0)				//½ÓÊÕ³¬Ê±?
	{
		SCI_canopen.cnt_sciover=0;
		Scicanopen_Init();
		M_ClrFlag(SL_CANOPENHEAD);			//20130801
		M_ClrFlag(SL_CANOPENHEADOK);		//20130801
	}
	else
	{
/*120507		if((SciaRegs.SCIFFRX.bit.RXFFST >= 1)&&(M_ChkFlag(SL_CANOPENHEADOK)==0))	//¿ªÊ¼½ÓÊÕÇÒ»¹Ã»ÓÐÊÕµ½±¨Í·?
		{
			SCI_canopen.cnt_sciover=0;

			SCI_canopen.rxb[0]=SciaRegs.SCIRXBUF.all&0x00FF;
			if(SCI_canopen.rxb[0]==CANOPENCOM_HEAD)
				M_SetFlag(SL_CANOPENHEADOK);
			else
			{	
				SciaRegs.SCIFFRX.bit.RXFIFORESET=1;		// Re-enable RX FIFO operation
				SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;
			}
		}
		else if(SciaRegs.SCIFFRX.bit.RXFFST >= SCICANOPENRXNUM-1)			//½ÓÊÕÍê³É?
		{
			
			//	SciaRegs.SCICTL1.bit.RXENA =0;
				for(i=1;i<SCICANOPENRXNUM;i++)								//¶Á³ö½ÓÊÕ»º´æ(²»°üÀ¨±¨Í·)
					SCI_canopen.rxb[i]=SciaRegs.SCIRXBUF.all&0x00FF;
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
				}

		//		SciaRegs.SCICTL1.bit.RXENA =1;
				SciaRegs.SCIFFRX.bit.RXFIFORESET=1;		// Re-enable RX FIFO operation
				SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;
				SCI_canopen.cnt_sciover=0;

		}
*/
/*		if((SciaRegs.SCIFFRX.bit.RXFFST >= 2)&&(M_ChkFlag(SL_CANOPENHEADOK)==0))	//¿ªÊ¼½ÓÊÕÇÒ»¹Ã»ÓÐÊÕµ½±¨Í·?
		{
			SCI_canopen.cnt_sciover=0;
	
			for(i=0;i<2;i++)								//Ð£Ñé½ÓÊÕ°üÀ¨±¨Í·µÄÇ°5Î»ÐÅÏ¢
				SCI_canopen.rxb[i]=SciaRegs.SCIRXBUF.all&0x00FF;
			
			if(SCI_canopen.rxb[0]==CANOPENCOM_HEAD && SCI_canopen.rxb[1]==0x11)
				M_SetFlag(SL_CANOPENHEADOK);	//[0]=0x7E,[1]=ÃüÁîÂë0x11,[2]=Êý¾Ý³¤¶È9¸ö×Ö½Ú,[3]=ÌØ¶¨²ÎÊý0x11,[4]=µØÖ·Æ«ÒÆ0x00

			else
			{	
				SciaRegs.SCIFFRX.bit.RXFIFORESET=1;		// Re-enable RX FIFO operation
				SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;
			}
		}
*/
//---------20130801----------
		if((SciaRegs.SCIFFRX.bit.RXFFST >= 1)&&(M_ChkFlag(SL_CANOPENHEADOK)==0))	//¿ªÊ¼½ÓÊÕÇÒ»¹Ã»ÓÐÊÕµ½±¨Í·?	20121215
		{
			SCI_canopen.cnt_sciover=0;

			if(M_ChkFlag(SL_CANOPENHEAD)==0)
			{
				SCI_canopen.rxb[0]=SciaRegs.SCIRXBUF.all&0x00FF;

				if(SCI_canopen.rxb[0]==CANOPENCOM_HEAD)
					M_SetFlag(SL_CANOPENHEAD);
				else
				{
					SciaRegs.SCIFFRX.bit.RXFIFORESET=1;		// Re-enable RX FIFO operation
					SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;

					temp = SciaRegs.SCIFFRX.bit.RXFFST;
					for(i=0;i<temp;i++)								//Ð£Ñé½ÓÊÕ°üÀ¨±¨Í·µÄÇ°5Î»ÐÅÏ¢
						SCI_canopen.rxb[0]=SciaRegs.SCIRXBUF.all&0x00FF;
					
				}
			}
			else
			{
				SCI_canopen.rxb[1]=SciaRegs.SCIRXBUF.all&0x00FF;

				if(SCI_canopen.rxb[1]==0x11)
					M_SetFlag(SL_CANOPENHEADOK);
				else
				{
					SciaRegs.SCIFFRX.bit.RXFIFORESET=1;		// Re-enable RX FIFO operation
					SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;
					M_ClrFlag(SL_CANOPENHEAD);

					temp = SciaRegs.SCIFFRX.bit.RXFFST;
					for(i=0;i<temp;i++)								//Ð£Ñé½ÓÊÕ°üÀ¨±¨Í·µÄÇ°5Î»ÐÅÏ¢
						SCI_canopen.rxb[0]=SciaRegs.SCIRXBUF.all&0x00FF;
				}
				
			}
		}
//---------20130801----------
		else if(SciaRegs.SCIFFRX.bit.RXFFST >= SCICANOPENRXNUM-2)			//½ÓÊÕÍê³É?
		{
//				M_NotFlag(SL_PHASEA);            							//2014-05-06ÁÙÊ±²âÊÔCANopen
//				*OUT3_ADDR = _OUT3_DATA;
			
				for(i=2;i<SCICANOPENRXNUM;i++)								//¶Á³ö½ÓÊÕ»º´æ(²»°üÀ¨±¨Í·)
					SCI_canopen.rxb[i]=SciaRegs.SCIRXBUF.all&0x00FF;	//[5]-[12]ÊÇ4¸ö×ÖµÄÖ¸ÁîÊý¾Ý,[13]ÊÇÐ£ÑéÎ»
				M_ClrFlag(SL_CANOPENHEADOK);
				M_ClrFlag(SL_CANOPENHEAD);
				
				heartbeat= SCI_canopen.rxb[6] & ONEBYTE[0];  //201105CPC ³£Ç£Í¨Ñ¶"ÐÄÌøÎ»"ÊÇBit15
				if(heartbeat!=SCI_canopen.heartbeat)
				{
					SCI_canopen.cnt_heartbeat=0;
//    				M_NotFlag(SL_PHASEB);								//2014-05-06ÁÙÊ±²âÊÔCANopen
//    				*OUT3_ADDR = _OUT3_DATA;
				}
				SCI_canopen.heartbeat = heartbeat;

				xor_data=SCI_canopen.rxb[0];								//Òì»òÐ£Ñé
				for(i=1;i<(SCICANOPENRXNUM-1);i++)
					xor_data ^= SCI_canopen.rxb[i];
			
				if(xor_data == SCI_canopen.rxb[SCICANOPENRXNUM-1])							//Òì»òÐ£Ñé·ûºÏ?
				{
//					if((SCI_canopen.rxb[5]|SCI_canopen.rxb[6]|SCI_canopen.rxb[7]|SCI_canopen.rxb[8]|SCI_canopen.rxb[9]|SCI_canopen.rxb[10]|SCI_canopen.rxb[11]|SCI_canopen.rxb[12])!=0) //ÌÞ³öÊý¾Ý¶¼µÈÓÚ0µÄ»µ°ü20090817
//					{			
					//Êý¾Ý×ª´æ
					SCI_canopen.rx_controlword=(SCI_canopen.rxb[6]<<8)|SCI_canopen.rxb[5];						//controlword
				
					SCI_canopen.rx_torque=(SCI_canopen.rxb[8]<<8)|SCI_canopen.rxb[7];							//torque_ref
				
					SCI_canopen.rx_angle=(SCI_canopen.rxb[10]<<8)|SCI_canopen.rxb[9];							//angle_ref
//					}
//					else
//					{
//						temp=1;		//201105Xgate_testing
//					}		
				}

/*				if(M_ChkFlag(SL_CANOPENOVER)!=0)	//201101NR
				{
			   		 SCI_canopen.rx_controlword=0;
					 SCI_canopen.rx_torque=0;
					 SCI_canopen.rx_angle=0; 
				}
*/
		//		SciaRegs.SCICTL1.bit.RXENA =1;
				SciaRegs.SCIFFRX.bit.RXFIFORESET=1;		// Re-enable RX FIFO operation
				SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;
				SCI_canopen.cnt_sciover=0;

		}
	}
} 
/*********************************************************************************************************
** º¯ÊýÃû³Æ: Sci_canopentx_read
** ¹¦ÄÜÃèÊö: ¶ÔG-GateÍø¹ØÏÂ·¢¶ÁÈ¡Êý¾ÝÖ¸Áî
** Êä¡¡Èë: 	 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:120507
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Sci_canopentx_read(void)
{
	Uint16 i,xor_data;

	SCI_canopen.txb[0]= CANOPENCOM_HEAD;						//±¨Í·
	SCI_canopen.txb[1]= 0x0011;									//¶ÁÃüÁî×Ö
	SCI_canopen.txb[2]= 0x0002;									//ÃüÁîÐÅÏ¢³¤¶È=µØÖ·Æ«ÒÆÁ¿+Êý¾Ý×Ö½ÚÊýÁ¿
	SCI_canopen.txb[3]= 0x0011;									//ÌØ¶¨²ÎÊý ºãÎª0x11
	SCI_canopen.txb[4]= 0x0000;									//µØÖ·Æ«ÒÆÁ¿ ÉèÎª0
	SCI_canopen.txb[5]=	0x0008;									//Êý¾ÝÁ¿8¸ö×Ö½Ú

	xor_data=SCI_canopen.txb[0];								//Òì»òÐ£Ñé
	for(i=1;i<SCICANOPENREADNUM-1;i++)
		xor_data ^= SCI_canopen.txb[i];
				
	SCI_canopen.txb[SCICANOPENREADNUM-1] = xor_data;
		
	for(i=0;i<SCICANOPENREADNUM;i++)							//2011XGate
		SciaRegs.SCITXBUF=SCI_canopen.txb[i];
			
}  

/*********************************************************************************************************
** º¯ÊýÃû³Æ: Sci_canopentx
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
/*120507	
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
			SciaRegs.SCITXBUF=SCI_canopen.txb[i];
*/		
//CPCÍ¨Ñ¶Ð­Òé120507	
		SCI_canopen.txb[0]= CANOPENCOM_HEAD;						//±¨Í·
		SCI_canopen.txb[1]= 0x0010;									//Ð´ÃüÁî×Ö
		SCI_canopen.txb[2]= 17;										//ÃüÁîÐÅÏ¢³¤¶È=µØÖ·Æ«ÒÆÁ¿+Êý¾Ý×Ö½ÚÊýÁ¿
		SCI_canopen.txb[3]= 0x0011;									//ÌØ¶¨²ÎÊý ºãÎª0x11
		SCI_canopen.txb[4]= 0x0000;									//µØÖ·Æ«ÒÆÁ¿ ÉèÎª0

		SCI_canopen.txb[5]=SCI_canopen.tx_torque&0x00FF;
		SCI_canopen.txb[6]=(SCI_canopen.tx_torque&0xFF00)>>8;
		SCI_canopen.txb[7]=SCI_canopen.tx_speed&0x00FF;
		SCI_canopen.txb[8]=(SCI_canopen.tx_speed&0xFF00)>>8;
		SCI_canopen.txb[9]=SCI_canopen.tx_state1&0x00FF;
		SCI_canopen.txb[10]=(SCI_canopen.tx_state1&0xFF00)>>8;
		SCI_canopen.txb[11]=SCI_canopen.tx_state2&0x00FF;
		SCI_canopen.txb[12]=(SCI_canopen.tx_state2&0xFF00)>>8;
		SCI_canopen.txb[13]=SCI_canopen.tx_watertempin&0x00FF;
		SCI_canopen.txb[14]=(SCI_canopen.tx_watertempin&0xFF00)>>8;
		SCI_canopen.txb[15]=SCI_canopen.tx_watertempout&0x00FF;
		SCI_canopen.txb[16]=(SCI_canopen.tx_watertempout&0xFF00)>>8;
		SCI_canopen.txb[17]=SCI_canopen.tx_skiiptempmax&0x00FF;
		SCI_canopen.txb[18]=(SCI_canopen.tx_skiiptempmax&0xFF00)>>8;
		SCI_canopen.txb[19]=SCI_canopen.tx_demand&0x00FF;
		SCI_canopen.txb[20]=(SCI_canopen.tx_demand&0xFF00)>>8;

		xor_data=SCI_canopen.txb[0];								//Òì»òÐ£Ñé
		for(i=1;i<SCICANOPENTXNUM-1;i++)
			xor_data ^= SCI_canopen.txb[i];
				
		SCI_canopen.txb[SCICANOPENTXNUM-1] = xor_data;
		
		for(i=0;i<SCICANOPENTXNUM-6;i++)							//2011XGate
			SciaRegs.SCITXBUF=SCI_canopen.txb[i];
} 
/*********************************************************************************************************
** º¯ÊýÃû³Æ: DataFilter
** ¹¦ÄÜÃèÊö: Êý¾ÝÂË²¨
** Êä¡¡Èë: 	Y(k-1)ÎªÉÏ´ÎÂË²¨½á¹û¬X(k)ªÐÂ²ÉÑùÖµ¡£¡£
** Êä¡¡³ö: £ºY(k)Îª±¾´ÎÂË²¨½á¹û¡£  
** ×¢  ÊÍ: 	 ÂË²¨¹«Ê½Îª£ºY(k)=cY(k-1)+(1-c)X(k),ÆäÖÐ£¬c=1/(1+2*PAI*fh/fs),fhÎªµÍÍ¨ÂË²¨Æ÷µÄ½ØÖ¹ÆµÂÊ£¬fsÎª²ÉÑùÆµÂÊ.
			ÔÚÒ»½×µÍÍ¨ÂË¨ÖÐ£¬X(k)´ÎªY(k)¡£
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
** º¯ÊýÃû³Æ: Bandstop filter 
** ¹¦ÄÜÃèÊö: ÏÝ²¨Æ÷
** Êä¡¡Èë:   w0 ÖÐÐÄÆµÂÊ
			 wb °ë¸ö´ø×èÆµÂÊ
			 Ts=2e-4
			 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 201110bjtu
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:   201110 BJTULVRT201204
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
/*void BS_Loop(struct BS_DATA *var,struct BS_PARA BS_var)
{

	float outtemp,w0,wb,w2,W;
//test
//	w0 = _stdby07 * TWOPAI;  
//test
	w0 = BS_var.f0 * TWOPAI;
	wb = BS_var.fb * TWOPAI;
	w2 = 4 * tan(0.0001 * w0) * tan(0.0001 * w0);                   //w0^2ÖÐÐÄÆµÂÊÆ½·½
	W  = 4 * (tan(0.0001 * (w0 + wb)) - tan(0.0001 * (w0 - wb)));   //W´ø¿í
    	
    outtemp = ((4+w2) * var->in + (2* w2-8) * var->inp + (4+w2) * var->infp - (2* w2-8) * var->outp - (4-W+w2) * var->outfp) /(4+W+w2);     //Êä³öº¯Êý£¬ÓÉ£¨5£©Ê½µÃ¡£

	if     (outtemp >  BS_var.outmax)  outtemp =  BS_var.outmax;		//Êä³öÏÞ·ù
	else if(outtemp < -BS_var.outmax)  outtemp = -BS_var.outmax;		//Êä³öÏÞ·ù

	var->out  = outtemp;		

	var->infp  = var->inp;      //in(k-1)-->in(k-2)
	var->inp   = var->in;       //in(k)-->in(k-1)
	var->outfp = var->outp;     //out(k-1)-->out(k-2)
	var->outp  = var->out;      //out(k)-->out(k-1)				
} 

/*********************************************************************************************************
** º¯ÊýÃû³Æ: Bandstop filter without pre-warped
** ¹¦ÄÜÃèÊö: ÏÝ²¨Æ÷-ÎÞÔ¤»û±ä
** Êä¡¡Èë:   w0 ÖÐÐÄÆµÂÊ
			 wb °ë¸ö´ø×èÆµÂÊ
			 Ts=2e-4
			 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 201110bjtu
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:   201110 BJTULVRT201204
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void BS_Loop_S(struct BS_DATA *var,struct BS_PARA BS_var)
{

	float outtemp,w0,wb,w2,W;

	w0 = BS_var.f0 * TWOPAI;
	wb = BS_var.fb * TWOPAI;
	w2 = 4e-8 * w0 * w0;            //w0^2ÖÐÐÄÆµÂÊÆ½·½ ÎÞÔ¤»û±ä
	W  = 8e-4  * wb;   				//W´ø¿í
    	
    outtemp = ((4+w2) * var->in + (2* w2-8) * var->inp + (4+w2) * var->infp - (2* w2-8) * var->outp - (4-W+w2) * var->outfp) /(4+W+w2);     //Êä³öº¯Êý£¬ÓÉ£¨5£©Ê½µÃ¡£

	if     (outtemp >  BS_var.outmax)  outtemp =  BS_var.outmax;		//Êä³öÏÞ·ù
	else if(outtemp < -BS_var.outmax)  outtemp = -BS_var.outmax;		//Êä³öÏÞ·ù

	var->out  = outtemp;		

	var->infp  = var->inp;      //in(k-1)-->in(k-2)
	var->inp   = var->in;       //in(k)-->in(k-1)
	var->outfp = var->outp;     //out(k-1)-->out(k-2)
	var->outp  = var->out;      //out(k)-->out(k-1)				
} 

/*********************************************************************************************************
** º¯ÊýÃû³Æ: 100Hz Bandstop filter 
** ¹¦ÄÜÃèÊö: ÏÝ²¨Æ÷
** Êä¡¡Èë:   w0=100 ÖÐÐÄÆµÂÊ
			 wb=+-50Hz °ë¸ö´ø×èÆµÂÊ
			 Ts=2e-4
			 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 201110bjtu
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:   201110 BJTULVRT201204
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void BS_Loop100(struct BS_DATA *var,struct BS_PARA BS_var)
{
	float outtemp;
    	
    outtemp = 0.9409 * var->in - 1.8669 * var->inp + 0.9409 * var->infp + 1.8669 * var->outp - 0.8817 * var->outfp;     //Êä³öº¯Êý£¬ÓÉ£¨5£©Ê½µÃ¡£

	if     (outtemp >  BS_var.outmax)  outtemp =  BS_var.outmax;		//Êä³öÏÞ·ù
	else if(outtemp < -BS_var.outmax)  outtemp = -BS_var.outmax;		//Êä³öÏÞ·ù

	var->out  = outtemp;		

	var->infp  = var->inp;      //in(k-1)-->in(k-2)
	var->inp   = var->in;       //in(k)-->in(k-1)
	var->outfp = var->outp;     //out(k-1)-->out(k-2)
	var->outp  = var->out;      //out(k)-->out(k-1)				
} 


/*********************************************************************************************************
** º¯ÊýÃû³Æ: Ad8364Ctrl
** ¹¦ÄÜÃèÊö: ¶ÁÈ¡²¢´¦ÀíÇ°Ò»ÎµÄ×ª»»½á¹û£¬Í¬Ê±Æô¶¯ÏÂÒ»´ÎAD×ª»»
** ä¡¡Èë: 	 
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
	float tempa,tempb,tempc,temp;

//------------------------------Êý¾Ý¶ÁÈ¡(¹²18Â·)---------------------------------
//¸Ã×éADÊý¾ÝÊÇ100usÖ®Ç°µÄ½á¹û
	AD.dat[0] = *AD_ASTART;	       // STA.Ubc µç»ú¶¨×Ó²àµçÑ¹Ubc
	AD.dat[1] = *(AD_ASTART+1);    // AMUX£¬ÂýËÙÐÅºÅ£¬16Ñ¡1¹ýÀ´µÄÐÅºÅ
//	AD.dat[2] = *(AD_ASTART+2);    // GRD.Ubc Ö÷¶ÏÇ°Ubc
	AD.dat[2] = *(AD_ASTART+2);    // chopperµçÁ÷idc	20130805
//	AD.dat[3] = *(AD_ASTART+3);    // Ic ±¸ÓÃSKIIPµçÁ÷·´À¡
//	AD.dat[4] = *(AD_ASTART+4);    // GRD.Uab Ö÷¶ÏÇ°Uab
	AD.dat[5] = *(AD_ASTART+5);    // MPR.ic, »ú²à±äÁ÷Æ÷MPRµÄµçÁ÷

	AD.dat[6]  = *AD_BSTART;       // NGS.Uab Íø²àµçÑ¹Ubc´óÂË²¨Í¨µÀ
	AD.dat[7]  = *(AD_BSTART+1);   // MPR.ib£¬»ú²à±äÁ÷Æ÷NPR
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

//	ADFINAL.uab   = AD.dat[4];       // GRD.Uab Ö÷¶ÏÇ°Uab
//	ADFINAL.ubc   = AD.dat[2];		 // GRD.Ubc Ö÷¶ÏÇ°Ubc
//	ADFINAL.isa  = AD.dat[2];		// ¶¨×ÓµçÁ÷AÏà ÒÔÁ÷³öµç»úÎªÕý
//	ADFINAL.isc  = AD.dat[4];	    // ¶¨×ÓµçÁ÷CÏà BJTULVRT201204
	ADFINAL.idc  = AD.dat[2];		//chopperµçÁ÷idc	20130805

	ADFINAL.uab1 = AD.dat[14];		// Uab Íø²àµçÑ¹		
	ADFINAL.ubc1 = AD.dat[17];		// Ubc Íø²àµçÑ¹
	
	ADFINAL.uab2 = AD.dat[16];		// Uab ç»ú¶¨×Ó²àµçÑ¹		
	ADFINAL.ubc2 = AD.dat[0];		// Ubc µç»ú¶¨×Ó²àµçÑ¹

	ADFINAL.uab3 = AD.dat[6];		// Uab Íø²àµçÑ¹	´óÂË²¨Í¨µÀ 	
	ADFINAL.ubc3 = AD.dat[8];		// Ubc Íø²àµçÑ¹ ´óÂË²¨Í¨µÀ 


	ADFINAL.udc  = AD.dat[10];		//ÖÐ¼äÖ±Á÷µçÑ¹¼ì²â

    if(ADFINAL.udc < 0) ADFINAL.udc=0;

	ADFINAL.AMUX = AD.dat[1];       //ÂýËÙADÊäÈë

//-----------------------------------------------------------	     
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
//---------------ÖÐ¼äÖ±Á÷Õ¶²¨Æ÷-IGBTµçÁ÷BJTULVRT201204---------------------------
/*		case(5): {
					ADFINAL.idc = ADFINAL.AMUX ;
					AD_OUT_DC_I.b = ADFINAL.idc * 7.6294e-2 * 1.0000000;  //20Å·²ÉÓÃµç×è,700A¶ÔÓ¦140mA(1:5000) =10V*700A/(32768*20R*140mA)
					break;
				 }
*///		case(5): {AD_OUT_STA_I.ac=ADFINAL.AMUX  * 0.0732422;		break; }	//µç»ú¶¨×Ó²àAÏàÏßµçÁ÷£¬»¥¸ÐÆ÷1200A:1A,Rt=5R,=1200A*10V/(5R*1A*32768)			     
//		case(4): {AD_OUT_STA_I.ba=ADFINAL.AMUX  * 0.0732422;		break; }	//µç»ú¶¨×Ó²àBÏàÏßµçÁ÷£¬»¥¸ÐÆ÷
//20130805
		case(5): {AD_OUT_STA_I.ac=ADFINAL.AMUX  * 0.0732422;		break; }	//µç»ú¶¨×Ó²àAÏàÏßµçÁ÷£¬»¥¸ÐÆ÷1200A:1A,Rt=5R,=1200A*10V/(5R*1A*32768)			     
		case(4): {AD_OUT_STA_I.ba=ADFINAL.AMUX  * 0.0732422;		break; }	//µç»ú¶¨×Ó²àBÏàÏßµçÁ÷£¬»¥¸ÐÆ÷
//--------
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

//----20130805----
	AD_OUT_STA_I.cb = - AD_OUT_STA_I.ac - AD_OUT_STA_I.ba;
	AD_OUT_DC_I.b = ADFINAL.idc * 7.6294e-2 * 1.0000000;  //20Å·²ÉÓÃµç×è,700A¶ÔÓ¦140mA(1:5000) =10V*700A/(32768*20R*140mA)
//----------------

//-------------------¶¨×ÓµçÁ÷£¨µ¥Î»A£©-------------------------------------------------------------
/*	AD_OUT_STA_I.a  =  (AD_OUT_STA_I.ac - AD_OUT_STA_I.ba) * 0.3333333;
	AD_OUT_STA_I.b  =   AD_OUT_STA_I.a  + AD_OUT_STA_I.ba;	
    AD_OUT_STA_I.c  = - AD_OUT_STA_I.a  - AD_OUT_STA_I.b; 
*/
//-------------------¶¨×ÓµçÁ÷£¨µ¥Î»A£©-BJTULVRT201204------------------------------------------------------------
/*	AD_OUT_STA_I.ac =   ADFINAL.isa  * 0.114441;	//LEM2000Aµç»ú¶¨×Ó²àAÏàÏßµçÁ÷£¬1500A-4V;==(1500/4*10)/32768  20111128
	AD_OUT_STA_I.cb =   ADFINAL.isc  * 0.114441;	//LEM2000Aµç»ú¶¨×Ó²àBÏàÏßµçÁ÷£¬200A-100mA;50.0ohm²ÉÑù,==10V*200A*D/(100mA*50R*32768*1) INA114·´Ïà 20111128
	AD_OUT_STA_I.ba = - AD_OUT_STA_I.ac - AD_OUT_STA_I.cb;
*/
	AD_OUT_STA_I.a  =  (AD_OUT_STA_I.ac - AD_OUT_STA_I.ba) * 0.3333333;	//201112fluxObs
	AD_OUT_STA_I.b  =   AD_OUT_STA_I.a  + AD_OUT_STA_I.ba;
    AD_OUT_STA_I.c  = - AD_OUT_STA_I.a  - AD_OUT_STA_I.b;
	tempa = abs(AD_OUT_STA_I.ac);
	tempb = abs(AD_OUT_STA_I.cb);
	DataFilter(0.9999,&MEAN_DATA.iac3,tempa); //¶¨×Ó²àµçÁ÷	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.icb3,tempb); //¶¨×Ó²àµçÁ÷	Æ½¾ùÖµÂË²¨10S

	tempa = abs(AD_OUT_STA_I.a);
	tempb = abs(AD_OUT_STA_I.b);
	tempc = abs(AD_OUT_STA_I.c);
	DataFilter(0.9999,&MEAN_DATA.ia3,tempa); //¶¨×Ó²àµçÁ÷	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.ib3,tempb); //¶¨×Ó²àµçÁ÷	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.ic3,tempc); //¶¨×Ó²àµçÁ÷	Æ½¾ùÖµÂË²¨10S

//------------------Íø²à±äÁ÷Æ÷µçÁ÷------------------------------------------------------------------
    AD_OUT_NPR_I.a = - (ADFINAL.ia1 * 0.0572204);  // SKIIP·´À¡µçÁ÷(Á÷³öÇÅ±ÛÎªÕý),µ×°åÓÐÒ»¸ö·´Ïò£¬10V=1875A,=10V*1875A/(32768*10V)
	AD_OUT_NPR_I.b = - (ADFINAL.ib1 * 0.0572204);  // SKIIP·´À¡µçÁ÷(Á÷³öÇÅ±ÛÎªÕý)£¬¿ØÖÆËã·¨ÒÔÁ÷³öSKIIPý
	AD_OUT_NPR_I.c = - (ADFINAL.ic1 * 0.0572204);  // SKIIP·´À¡µçÁ÷(Á÷³öÇÅ±ÛÎªÕý)
//---20121103-----------------²¢ÍøµçÁ÷Ë²Ê±Öµ±£»¤-201011LVRT--------------------------
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

//-------------------»ú²à±äÁ÷Æ÷µçÁ÷×ªÎªÊµ¼ÊÖµ-201005atcpc-----------------------------------------------------
	if(R_PHORDE==1)		//·´ÏàÐò¿ØÖÆ201005atcpc
	{
    	AD_OUT_MPR_I.b =  (ADFINAL.ia2 * 0.0572204);  // SKIIP·´À¡µçÁ÷(Á÷³öÇÅ±ÛÎªÕý),µ×°åÓÐÒ»¸ö·´Ïò£¬10V=1875A,=10V*1875A/(32768*10V)
		AD_OUT_MPR_I.a =  (ADFINAL.ib2 * 0.0572204);  // SKIIP·´À¡µçÁ÷(Á÷³öÇÅ±ÛÎªÕý)£¬¿ØÖÆËã·¨ÒÔÁ÷³öSKIIPÎªý
		AD_OUT_MPR_I.c =  (ADFINAL.ic2 * 0.0572204);  // SKIIP·´À¡µçÁ÷(Á÷³öÇÅ±ÛÎªÕý)
	}
	else
	{
	    AD_OUT_MPR_I.a =  (ADFINAL.ia2 * 0.0572204);  // SKIIP·´À¡µçÁ÷(Á÷³öÇÅ±ÛÎªÕý),µ×°åÓÐÒ»¸ö·´Ïò£¬10V=1875A,=10V*1875A/(32768*10V)
		AD_OUT_MPR_I.b =  (ADFINAL.ib2 * 0.0572204);  // SKIIP·´À¡µçÁ÷(Á÷³öÇÅ±ÛÎªÕý)£¬¿ØÖÆËã·¨ÒÔÁ÷³öSKIIPÎªÕý
		AD_OUT_MPR_I.c =  (ADFINAL.ic2 * 0.0572204);  // SKIIP·´À¡µçÁ÷(Á÷³öÇÅ±ÛÎªÕý)
	}
//----20121103----------------×ª×ÓµçÁ÷Ë²Ê±Öµ±£»¤-----201011LVRT----------------------
	if(abs(AD_OUT_MPR_I.a)>abs(AD_OUT_MPR_I.b))
	{
		if(abs(AD_OUT_MPR_I.a)>abs(AD_OUT_MPR_I.c))		temp=abs(AD_OUT_MPR_I.a);
		else											temp=abs(AD_OUT_MPR_I.c);
	}
	else
	{
		if(abs(AD_OUT_MPR_I.b)>abs(AD_OUT_MPR_I.c))		temp=abs(AD_OUT_MPR_I.b);
		else											temp=abs(AD_OUT_MPR_I.c);
	}

	PRO.MPR_iac = temp * 100 / MPR_IACN;		//×ª×Óµç÷´óÖµ£¬»ù×¼µç÷µÄ°Ù·ÖÖµ

//---------------------Ö±Á÷µçÑ¹----------------------------------------------------------------------
    AD_OUT_UDC      = ADFINAL.udc * 0.0448788;   // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
	DataFilter(0.44,&MEAN_DATA.udc,AD_OUT_UDC);  //VdcÖ±Á÷ÂË²¨£¬fh=1kHz
//	DataFilter(0.7,&MEAN_DATA.udc,AD_OUT_UDC);   //VdcÖ±Á÷ÂË²¨£¬fh=340Hz 201005atcpc
    AD_OUT_UDC      = MEAN_DATA.udc;
//    AD_OUT_UDC      = 1100;	//controltest
//----20121103------------ÖÐ¼äÖ±Á÷µçÑ¹±£»¤Öµ¼ÆËã-201011LVRT----------------------------

	PRO.udc  = AD_OUT_UDC;	                                 //µ¥Î»V
						
//---------------------Ö÷¶ÏÇ°µçÍøµçÑ¹---------------------------------------------------------------------
//	AD_OUT_GRD_U.ab = ADFINAL.uab * 0.0448788;   // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
//	AD_OUT_GRD_U.bc = ADFINAL.ubc * 0.0448788;   // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA) 

//---------------------Ö÷¶ÏºóµçÍøµçÑ¹---------------------------------------------------------------------
	AD_OUT_NGS_U.ab = ADFINAL.uab1 * 0.0448788;   // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
	AD_OUT_NGS_U.bc = ADFINAL.ubc1 * 0.0448788;   // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)

//-------------------µç»ú¶¨×Ó²àÏßµçÑ¹---------------------------------------------
	AD_OUT_STA_U.ab  = ADFINAL.uab2 * 0.0448788;  // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
	AD_OUT_STA_U.bc  = ADFINAL.ubc2 * 0.0448788;  // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)

//-------------------Ö÷¶ÏºóµçÍøµçÑ¹,´óÂË²¨Í¨µÀ---------------------------------------------
	AD_OUT_NGF_U.ab  = ADFINAL.uab3 * 0.0448788;  // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
	AD_OUT_NGF_U.bc  = ADFINAL.ubc3 * 0.0448788;  // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
/* 
//---------------------ÁãÆ¯ÂË²¨201005atcpc-------------------------------------------------------------------	
	DataFilter(0.999985,&MEAN_DATA.zfia1,AD_OUT_NPR_I.a); //Íø²àµçÁ÷´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S
	DataFilter(0.999985,&MEAN_DATA.zfib1,AD_OUT_NPR_I.b); //Íø²àµçÁ÷´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S
	DataFilter(0.999985,&MEAN_DATA.zfic1,AD_OUT_NPR_I.c); //Íø²àµçÁ÷´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S

	DataFilter(0.999985,&MEAN_DATA.zfia3,AD_OUT_STA_I.a); //¶¨×Ó²àµçÁ÷´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S
	DataFilter(0.999985,&MEAN_DATA.zfib3,AD_OUT_STA_I.b); //¶¨×Ó²àµçÁ÷´«¸ÐÆ÷	ÁãÆ¯Ë²¨60S
	DataFilter(0.999985,&MEAN_DATA.zfic3,AD_OUT_STA_I.c); //¶¨×Ó²àµçÁ÷´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S

	DataFilter(0.999985,&MEAN_DATA.zfuab,AD_OUT_GRD_U.ab); //Ö÷¶ÏÇ°ÍøÑ¹µçÑ¹´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S
	DataFilter(0.999985,&MEAN_DATA.zfubc,AD_OUT_GRD_U.bc); //Ö÷¶ÏÇ°ÍøÑ¹µçÑ¹´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S 

	DataFilter(0.999985,&MEAN_DATA.zfuab1,AD_OUT_NGS_U.ab); //ÍøÑ¹µçÑ¹´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S
	DataFilter(0.999985,&MEAN_DATA.zfubc1,AD_OUT_NGS_U.bc); //ÍøÑ¹µçÑ¹´«¸ÐÆ÷	ÁãÆ¯ÂË²¨60S


//------------------³ý¥ÁÆ¯--µç»úÊÔÑé²»ÔÊÐí¶Ô»ú²àµçÁ÷½øÐÐÁãÆ¯ÂË²¨£¡201005atcpc-------------------------------------------------------------------
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
//-------------------Ö÷¶ÏÇ°µçÍøµçÑ¹£¬ÓÉßÑ¹×ªÎªÏàÑ¹------------------------------------------------------
//	AD_OUT_GRD_U.b  = (AD_OUT_GRD_U.bc - AD_OUT_GRD_U.ab) * 0.3333333;
//	AD_OUT_GRD_U.a  = AD_OUT_GRD_U.b + AD_OUT_GRD_U.ab;	
//	AD_OUT_GRD_U.c  = - AD_OUT_GRD_U.a - AD_OUT_GRD_U.b; 

//-------------------µçÍøµçÑ¹£¬ÓÉÏßÑ¹×ªÎªÏàÑ¹------------------------------------------------------
	AD_OUT_NGS_U.b  = (AD_OUT_NGS_U.bc - AD_OUT_NGS_U.ab) * 0.3333333;
	AD_OUT_NGS_U.a  = AD_OUT_NGS_U.b + AD_OUT_NGS_U.ab;	
	AD_OUT_NGS_U.c  = - AD_OUT_NGS_U.a - AD_OUT_NGS_U.b;

//-------------------µç»ú¨×Ó²àµçÑ¹£¬ÓÉÏßÑ¹ËãÏàÑ¹------------------------------------------------------
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

//	tempa = abs(AD_OUT_GRD_U.ab);
//	tempb = abs(AD_OUT_GRD_U.bc);
//	DataFilter(0.9999,&MEAN_DATA.uab,tempa); //Ö÷¶ÏÇ°Íø²àµçÑ¹	Æ½¾ùÖµÂË²¨10S
//	DataFilter(0.9999,&MEAN_DATA.ubc,tempb); //Ö÷¶ÏÇ°Íø²àµçÑ¹	Æ½¾ùÖµÂË²¨10S 

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

	tempa = abs(AD_OUT_NGF_U.ab);			  //MagnetCurve2013-12-13
	tempb = abs(AD_OUT_NGF_U.bc);
	DataFilter(0.9999,&MEAN_DATA.uab3,tempa); //Ö÷¶ÏºóµçÍøµçÑ¹,´óÂË²¨Í¨µÀ	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.ubc3,tempb); //Ö÷¶ÏºóµçÍøµçÑ¹,´óÂË²¨Í¨µÀ	Æ½¾ùÖµÂË²¨10S

//	tempa = abs(AD_OUT_NGF_U.ab);			  //MagnetCurve200909
//	tempb = abs(AD_OUT_NGF_U.bc);
//	DataFilter(0.9999,&MEAN_DATA.uab3,tempa); //Ö÷¶ÏºóµçÍøµçÑ¹,´óÂË²¨Í¨µÀ	Æ½¾ùÖµÂË²¨10S
//	DataFilter(0.9999,&MEAN_DATA.ubc3,tempb); //Ö÷¶ÏºóµçÍøµçÑ¹,´óÂË²¨Í¨µÀ	Æ½¾ùÖµÂË²¨10S

//--------------ÇóÍøÑ¹ºÍ¶¨×ÓµçÑ¹²îÖµÆ½¾ùÖµ------------------------------------------
    AD_OUT_STAD_U.ab = AD_OUT_NGF_U.ab - AD_OUT_STA_U.ab;
    AD_OUT_STAD_U.bc = AD_OUT_NGF_U.bc - AD_OUT_STA_U.bc;

	tempa = abs(AD_OUT_STAD_U.ab);
	tempb = abs(AD_OUT_STAD_U.bc);
	DataFilter(0.9999,&MEAN_DATA.uab_d,tempa);  //²îÖµÆ½¾ùÖµ    Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.ubc_d,tempb);  //²îÖµÆ½¾ùÖµ	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.997,&MEAN_DATA.ubc_t,tempb);  //²îÖµÆ½¾ùÖµ	Æ½¾ùÖµÂË²¨1S	//20130801
//----------------------------------------------------------------------------------
/*	//201204
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
*/

//--------------------------------------------------------------------------------------------------
//----------------------Íø²àºÍ»ú²àµç¸ÐÎÂ¶ÈÂË²¨---------------------------------------------------------------------
	tempa = abs(AMUX.Lac_temp);
	tempb = abs(AMUX.Ldudt_temp);
	DataFilter(0.9999,&MEAN_DATA.Lac_temp,tempa); 	//Íø²àµç¸ÐÎÂ¶È	Æ½¾ùÖµÂË²¨10S
	DataFilter(0.9999,&MEAN_DATA.Ldudt_temp,tempb); //»ú²àµç¸ÐÎÂ¶È	Æ½¾ùÖµÂË²¨10S

//--------------------Àø´Å²ÎÊý±æÊ¶MagnetCurve2013-12-13--ZZJ--------------------------------------------------------------
//BJTULVRT201204¼ÆËã±ÈÂÊUns_ftÒÆµ½inputÀï½ÚÔ¼T0×ÊÔ´		
//	Uns_ft = 0.5 * ((MEAN_DATA.uab3/MEAN_DATA.uab1)+(MEAN_DATA.ubc3/MEAN_DATA.ubc1));//´óÂË²¨·ùÖµË¥¼õ±ÈÀý
//	Uns_ft = 0.848;		//R=20k,C=0.1uF,fh=80Hz,ÀíÂÛË¥¼õ±ÈÀýAvh=0.848,Êµ¼Ê²ÉÓÃÉÏÊ½²âÁ¿¼ÆËãÖµÎª0.83×óÓÒ£¡2010-10-2

//--------20121011--------

	if(_SC_MSTDBY==1)	 //Us=50%
	{
		MC_U_test = (float)_SC_Usn * 0.5 * Uns_ft - (0.5 * 1.110721 * (MEAN_DATA.uab2+MEAN_DATA.ubc2));
		MC_DATA.temp[1] = _SC_EXISTY1 * 0.001;
		MAIN_LOOP.cnt_mc_ack=0;	
		MC_F_cnt = 0;
		MC_N_test= 0;
	}
	else if(_SC_MSTDBY==2)//Us=75%
	{
		MC_U_test =  (float)_SC_Usn * 0.75 * Uns_ft - (0.5 * 1.110721 * (MEAN_DATA.uab2+MEAN_DATA.ubc2));
		MC_DATA.temp[2] = _SC_EXISTY2 * 0.001;	
		MAIN_LOOP.cnt_mc_ack=0;	
		MC_F_cnt = 0;
		MC_N_test= 0;
	}
	else if(_SC_MSTDBY==3)//Us=90%
	{
		MC_U_test =  (float)_SC_Usn * 0.9 * Uns_ft - (0.5 * 1.110721 * (MEAN_DATA.uab2+MEAN_DATA.ubc2));
		MC_DATA.temp[3] = _SC_EXISTY3 * 0.001;	
		MAIN_LOOP.cnt_mc_ack=0;	
		MC_F_cnt = 0;
		MC_N_test= 0;
	}
	else if(_SC_MSTDBY==4)//Us=100%
	{
		MC_U_test =  (float)_SC_Usn * 1.0 * Uns_ft - (0.5 * 1.110721 * (MEAN_DATA.uab2+MEAN_DATA.ubc2));
		MC_DATA.temp[4] = _SC_EXISTY4 * 0.001;
		MAIN_LOOP.cnt_mc_ack=0;	
		MC_F_cnt = 0;
		MC_N_test= 0;
	}
	else if(_SC_MSTDBY==5)//Us=110% 
	{
		MC_U_test =  (float)_SC_Usn * 1.1 * Uns_ft - (0.5 * 1.110721 * (MEAN_DATA.uab2+MEAN_DATA.ubc2));
		MC_DATA.temp[5] = _SC_EXISTY5 * 0.001;
		MAIN_LOOP.cnt_mc_ack=0;	
		MC_F_cnt = 0;
		MC_N_test= 0;
	}
	else if(_SC_MSTDBY==99)//È·¶¨Àø´Å²ÎÊý±æÊ¶Íê³É
	{
		if(M_ChkFlag(SL_MC_FAIL)!=0)
		{
			_MC_OK = 0;
			MC_DATA.y[0] = 1;
			MC_DATA.y[1] = 1;
			MC_DATA.y[2] = 1;
			MC_DATA.y[3] = 1;
			MC_DATA.y[4] = 1;
			MC_DATA.y[5] = 1;			
		} 
		else if(M_ChkFlag(SL_MC_OK)==0)
		{
	    	MC_DATA.y[0] = _SC_EXISTY1 * 0.001;
 	   		MC_DATA.y[1] = _SC_EXISTY1 * 0.001;
			MC_DATA.y[2] = _SC_EXISTY2 * 0.001;
			MC_DATA.y[3] = _SC_EXISTY3 * 0.001;
			MC_DATA.y[4] = _SC_EXISTY4 * 0.001;
			MC_DATA.y[5] = _SC_EXISTY5 * 0.001; 
			
			MC_U_test =  (float)_SC_Usn * (0.5 + MC_N_test * 0.1) * Uns_ft - (0.5 * 1.110721 * (MEAN_DATA.uab2+MEAN_DATA.ubc2));
			MC_F_cnt++;

//			if(MC_F_cnt>32766) MC_F_cnt=32766;//test

			if(abs(MC_U_test) < 25)
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_mc_ack,DELAY_MC_ACK)>=0)	//0.5s
				{
					MC_F_cnt = 0;
					MAIN_LOOP.cnt_mc_ack=0;
					if(MC_N_test < 6)	 MC_N_test++;	//0-6			
					else
					{
						M_SetFlag(SL_MC_OK);
						MC_N_test = 0;
					}					
				}
			}		
			else if(MC_F_cnt > 5e4)		//³¬Ê±´ïµ½10s,ÈÏÎªÀø´ÅÇúÏß²â»æÊ§°Ü,ÐèÒªÖØÐÂ²âÁ¿
			{
				M_SetFlag(SL_MC_FAIL);
				MC_F_cnt = 0;
				MC_N_test= 0;
				MAIN_LOOP.cnt_mc_ack=0;
			}
			else	MAIN_LOOP.cnt_mc_ack=0;		
		}
		else
		{
			_MC_OK = 1;
		}
					
	}
	else if(_SC_MSTDBY==100)		//ÓÃ»§ÒªÇóÖØÐÂ²â»æÀø´ÅÇúÏß
	{
		M_ClrFlag(SL_MC_OK);
		M_ClrFlag(SL_MC_FAIL);
		MC_F_cnt  = 0;
		MC_N_test = 0;
		_MC_OK 	  = 0;	
		MAIN_LOOP.cnt_mc_ack=0;	
	}
	else
	{
		MC_U_test = MEAN_DATA.uab3 + MEAN_DATA.ubc3 - (MEAN_DATA.uab2+MEAN_DATA.ubc2);		//BJTULVRT_201011
		MC_F_cnt  = 0;
		MC_N_test = 0;
		MAIN_LOOP.cnt_mc_ack=0;		
	}
//---------2013-12-13---ZZJ--------

//-------------------------------------------------------------------------------------------------
	*AD_DA_CTRL     = AD8364_CONVERT;	               	  	//Æô¶¯ÏÂ»AD×ª»»
    CAP4.adsmptsctr = ECap4Regs.TSCTR;						//±£´æÍøÑ¹¶¨ÏòÊ±¼ä
	QEPDATA.adsmposcnt=EQep2Regs.QPOSCNT;					//±£´æ±àÂëÆ÷Î»ÖÃÐÅÏ¢  

}

/*********************************************************************************************************
** º¯ÊýÃû³Æ: Da5344Manage
** ¹¦ÄÜÃèÊö£ºThe value of 4 da channels are chosen. 
** Êä¡¡Èë: 	 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 	The da output is used to help debug or display
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:2012.4
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Da5344Manage(void)
{ 
	Uint16 i;

	for(i=0;i<=3;i++)
	{
		switch(daindex[i])
		{												//4096-(10V),0-(-10V)

			case 0: {if(M_ChkFlag(SL_LV_STATE)==0)		zys[i] = 2048;
				     else   							zys[i] = 4095;	break;} 
			case 1: {if(M_ChkFlag(SL_LV_SCRON)==0)		zys[i] = 2048;
				     else   							zys[i] = 4095;	break;}  
			case 2: {if(M_ChkFlag(SL_LV_SCRRUNING)==0)	zys[i] = 2048;
				     else   							zys[i] = 4095;	break;}  
			case 3: {if(M_ChkFlag(SL_LV_SCRIZERO)==0)	zys[i] = 2048;
				     else   							zys[i] = 4095;	break;} 
			case 4: {if(M_ChkFlag(SL_UNBALANCE)==0)		zys[i] = 2048;
				     else   							zys[i] = 4095;	break;}  
			case 5: {if(M_ChkFlag(SL_LV_MSTOP)==0)		zys[i] = 2048;
				     else   							zys[i] = 4095;	break;}  
			case 6: {if(M_ChkFlag(SL_LV_NSTOP)==0)		zys[i] = 2048;
				     else   							zys[i] = 4095;	break;}  
			case 7: {if(M_ChkFlag(SL_LV_QWORKING)==0)	zys[i] = 2048;
				     else   							zys[i] = 4095;	break;}  
			case 8: {if(M_ChkFlag(SL_ERROR)==0)			zys[i] = 2048;
					 else   							zys[i] = 4095;	break;}  
//			case 9: {if(M_ChkFlag(SL_LV_PHICON)==0)		zys[i] = 2048;
//				     else   							zys[i] = 4095;	break;}  //=1LV»ú²à¶¯Ì¬´ÅÁ´¿ØÖÆ±êÖ¾Î» 
			case 10:{if(M_ChkFlag(SL_LV_SYNCON)==0)		zys[i] = 2048;				
				     else   							zys[i] = 4095;	break;}  //=1LV»ú²à¶¯Ì¬µ÷½Ú±êÖ¾Î»
			case 11: {if(M_ChkFlag(SL_QCAPSPDIN)==0)			zys[i] = 2048;
					 else   							zys[i] = 4095;	break;}  
			case 12: {if(M_ChkFlag(SL_QCAPDISTURB)==0)		zys[i] = 2048;
				     else   							zys[i] = 4095;	break;}  //=1LV»ú²à¶¯Ì¬´ÅÁ´¿ØÖÆ±êÖ¾Î» 
			case 13:{if(M_ChkFlag(SL_QEPPCO)==0)		zys[i] = 2048;				
				     else   							zys[i] = 4095;	break;}  //=1LV»ú²à¶¯Ì¬µ÷½Ú±êÖ¾Î»
//			case 14:{if(M_ChkFlag(SL_QEPPHE)==0)		zys[i] = 2048;				
//				     else   							zys[i] = 4095;	break;}  //=1LV»ú²à¶¯Ì¬µ÷½Ú±êÖ¾Î»
			case 15:{if(M_ChkFlag(SL_IN1_CBSTS)==0)		zys[i] = 2048;				
				     else   							zys[i] = 4095;	break;}  //=1LV»ú²à¶¯Ì¬µ÷½Ú±êÖ¾Î»
			case 16:{if(M_ChkFlag(SL_GRDFQE)==0)		zys[i] = 2048;				
				     else   							zys[i] = 4095;	break;}  //=1LV»ú²à¶¯Ì¬µ÷½Ú±êÖ¾Î»
			case 17:{if(M_ChkFlag(SL_HIDCOV)==0)		zys[i] = 2048;				
				     else   							zys[i] = 4095;	break;}  //=1
			case 18:{if(M_ChkFlag(SL_IDCOV)==0)			zys[i] = 2048;				
				     else   							zys[i] = 4095;	break;}  //=1
			case 19:{if(M_ChkFlag(SL_LV_SCRKEEPON)==0)	zys[i] = 2048;				
				     else   							zys[i] = 4095;	break;}  //=1


//------------------------------------------------------------------/·Å´ó±¶Êý magnify times----------------------
 			//----------------------AD OUT variables---------------------------------------------------
 			case 20:	{zys[i] = (int16)((AD_OUT_NGF_U.bc			* 0.005) * 204.8) + 2048; 	break;}//AD OUT variables 
 			case 21:	{zys[i] = (int16)((AD_OUT_STA_U.ab			* 0.005) * 204.8) + 2048;	break;}  
 			case 22:	{zys[i] = (int16)((AD_OUT_STA_U.bc			* 0.005) * 204.8) + 2048;	break;} 

 			case 23:	{zys[i] = (int16)((AD_OUT_SCR_I.a			* 0.01) * 204.8) + 2048;	break;}//
			case 24:	{zys[i] = (int16)((AD_OUT_SCR_I.b			* 0.01) * 204.8) + 2048;	break;}//
 		 
   			case 25:	{zys[i] = (int16)((AD_OUT_DC_I.b			* 0.01) * 204.8) + 2048; 	break;}  
   			case 26:	{zys[i] = (int16)((AD_OUT_NGS_U.bc			* 0.005) * 204.8) + 2048; 	break;}  

			case 27:	{zys[i] = (int16)((AD_OUT_UDC				* 0.005) * 204.8) + 2048;	break;}//AD OUT variables 

			case 28:	{zys[i] = (int16)((QEPDATA.omigamec			* 0.01) * 204.8) + 2048;break;}  
 			case 29:	{zys[i] = (int16)((CAP4.omigaslp			*  0.1) * 204.8) + 2048;break;} 
			case 30:	{zys[i] = (int16)((CAP4.omigasyn			* 0.01) * 204.8) + 2048;break;}  
			case 31:	{zys[i] = (int16)((QEPDATA.omigarote		* 0.01) * 204.8) + 2048;break;} 

			case 32:	{zys[i] = (int16)((AD_OUT_STA_I.a			* 0.01) * 204.8) + 2048;break;} 
			case 33:	{zys[i] = (int16)((AD_OUT_STA_I.b			* 0.01) * 204.8) + 2048;break;} 
			case 34:	{zys[i] = (int16)((AD_OUT_STA_I.c			* 0.01) * 204.8) + 2048;break;} 
			case 35:	{zys[i] = (int16)((AD_OUT_STA_I.ac			* 0.005) * 204.8) + 2048;break;} 
			case 36:	{zys[i] = (int16)((AD_OUT_STA_I.cb			* 0.005) * 204.8) + 2048;break;} 

			case 37:	{zys[i] = (int16)((TRS_MPR_I.a				* 0.01) * 204.8) + 2048;break;} 
			case 38:	{zys[i] = (int16)((TRS_MPR_I.b				* 0.01) * 204.8) + 2048;break;}  
			case 39:	{zys[i] = (int16)((TRS_MPR_I.c				* 0.01) * 204.8) + 2048;break;}

 			case 40:	{zys[i] = (int16)((TRS_MPR_I.d				* 0.01) * 204.8) + 2048;break;} 
 			case 41:	{zys[i] = (int16)((TRS_MPR_I.q				* 0.01) * 204.8) + 2048;break;}  

//---------------------------------MPR pi loop variables------------------------------------------------------------
 			case 42:	{zys[i] = (int16)((RUN.mpridrf				* 0.01) * 204.8) + 2048;break;}//
			case 43:	{zys[i] = (int16)((RUN.mpriqrf				* 0.01) * 204.8) + 2048;break;} 
			case 44:	{zys[i] = (int16)((CAP4.mprtrstheta			*    1) * 204.8) + 2048;break;}  
			case 45:	{zys[i] = (int16)((QEPDATA.rotposdisp		*    1) * 204.8) + 2048;break;}// 

			case 46:	{zys[i] = (int16)((PI_MPR_Id.reference		* 0.01) * 204.8) + 2048;break;}//MPR pi loop variables 
			case 47:	{zys[i] = (int16)((PI_MPR_Id.feedback		* 0.01) * 204.8) + 2048;break;} 
			case 48:	{zys[i] = (int16)((PI_MPR_Id.out			*  0.1) * 204.8) + 2048;break;} 

			case 49:	{zys[i] = (int16)((PI_MPR_Iq.reference		* 0.01) * 204.8) + 2048;break;}//MPR pi loop variables 
			case 50:	{zys[i] = (int16)((PI_MPR_Iq.feedback		* 0.01) * 204.8) + 2048;break;} 
			case 51:	{zys[i] = (int16)((PI_MPR_Iq.out			*  0.1) * 204.8) + 2048;break;}  

			case 52:	{zys[i] = (int16)((CAP4.omigaslp * MPR_Lr * DM_imrd		* 0.01) * 204.8) + 2048;	break;}//MPR pi loop variables 
			case 53:	{zys[i] = (int16)((SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Id.feedback	* 0.01) * 204.8) + 2048;	break;} 

			case 54:	{zys[i] = (int16)((DM_imrd					* 0.01) * 204.8) + 2048;break;}  

			case 55:	{zys[i] = (int16)((TRS_MPR_U.d				* 0.01) * 204.8) + 2048;break;} 
			case 56:	{zys[i] = (int16)((TRS_MPR_U.q				* 0.01) * 204.8) + 2048;break;} 

			case 57:	{zys[i] = (int16)((RUN.mpridrf_g			* 0.01) * 204.8) + 2048;break;} 
			case 58:	{zys[i] = (int16)((RUN.mpridrf_exi			* 0.01) * 204.8) + 2048;break;} 
			case 59:	{zys[i] = (int16)((RUN.mpridrf_var			* 0.01) * 204.8) + 2048;break;} 

			case 60:	{zys[i] = (int16)((QEPDATA.adsmposcnt		* 0.001) * 204.8) + 2048;break;}// 

//---------------------------------NPR pi loop variables------------------------------------------------------------
 			case 61:	{zys[i] = (int16)((TRS_NGS_U.d				* 0.01) * 204.8) + 2048;break;} 
			case 62:	{zys[i] = (int16)((TRS_NGS_U.q				* 0.01) * 204.8) + 2048;break;} 
			case 63:	{zys[i] = (int16)((TRS_NGS_U.dflt			* 0.01) * 204.8) + 2048;break;}
			case 64:	{zys[i] = (int16)((TRS_NGS_U.qflt			* 0.01) * 204.8) + 2048;break;}  

			case 65:	{zys[i] = (int16)((NGS_Udq_p				* 0.01) * 204.8) + 2048;break;} 
			case 66:	{zys[i] = (int16)((NGS_Udq_n				* 0.01) * 204.8) + 2048;break;}
	
			case 67:	{zys[i] = (int16)((TRS_NPR_U.sintheta		* 	 1)  * 204.8) + 2048;break;} 
			case 68:	{zys[i] = (int16)((TRS_NPR_U.costheta		*    1)  * 204.8) + 2048;break;}  
		 
 			case 69:	{zys[i] = (int16)((PI_NPR_U.reference		* 0.005) * 204.8) + 2048;break;}//NPR pi loop variables 
			case 70:	{zys[i] = (int16)((PI_NPR_U.feedback		* 0.005) * 204.8) + 2048;break;} 
 			case 71:	{zys[i] = (int16)((PI_NPR_U.out				* 0.01) * 204.8) + 2048;break;} 

			case 72:	{zys[i] = (int16)((TRS_NPR_I.a				* 0.01) * 204.8) + 2048;break;} 
			case 73:	{zys[i] = (int16)((TRS_NPR_I.b				* 0.01) * 204.8) + 2048;break;} 
			case 74:	{zys[i] = (int16)((TRS_NPR_I.c				* 0.01) * 204.8) + 2048;break;} 

	 	  	case 75:	{zys[i] = (int16)((TRS_NPR_I.d				* 0.01) * 204.8) + 2048;break;} 
 			case 76:	{zys[i] = (int16)((TRS_NPR_I.q				* 0.01) * 204.8) + 2048;break;}  

		 	case 77:	{zys[i] = (int16)((TRS_NPR_I.dflt			* 0.01) * 204.8) + 2048;break;} 
		  	case 78:	{zys[i] = (int16)((TRS_NPR_I.qflt			* 0.01) * 204.8) + 2048;break;}  

			case 79:	{zys[i] = (int16)((NGS_Udq_epsilon			*  0.1) * 204.8) + 2048;break;}

//-----------------------------NPR pi loop variables------------------------------------------------------------------
			case 80:	{zys[i] = (int16)((PI_NPR_Id.reference		* 0.01) * 204.8) + 2048;break;}//NPR pi loop variables  *200
			case 81:	{zys[i] = (int16)((PI_NPR_Id.feedback		* 0.01) * 204.8) + 2048;break;} 
			case 82:	{zys[i] = (int16)((PI_NPR_Id.out			* 0.01) * 204.8) + 2048;break;}  

			case 83:	{zys[i] = (int16)((PI_NPR_Iq.reference		* 0.01) * 204.8) + 2048;break;}//NPR pi loop variables 
			case 84:	{zys[i] = (int16)((PI_NPR_Iq.feedback		* 0.01) * 204.8) + 2048;break;} 
			case 85:	{zys[i] = (int16)((PI_NPR_Iq.out			* 0.01) * 204.8) + 2048;break;}   
	 		
			case 86:	{zys[i] = (int16)((TRS_NPR_U.d				* 0.01) * 204.8) + 2048;break;}  		 
 			case 87:	{zys[i] = (int16)((TRS_NPR_U.q				* 0.01) * 204.8) + 2048;break;} 

			case 88:	{zys[i] = (int16)((CAP4.nprtrstheta			*    1)  * 204.8) + 2048;break;} 
			case 89:	{zys[i] = (int16)((CAP4.nprtrstheta_lv		*    1)  * 204.8) + 2048;break;}

			case 91:	{zys[i] = (int16)((CAP4.omigasyn * NPR_L * PI_NPR_Iq.feedback	* 0.01)  * 204.8) + 2048;	break;}
			case 92:	{zys[i] = (int16)((CAP4.omigasyn * NPR_L * PI_NPR_Id.feedback	* 0.01)  * 204.8) + 2048;	break;}

 			case 93:	{zys[i] = (int16)((AD_OUT_SCR_I.a			* 0.01) * 204.8) + 2048;	break;}//
			case 94:	{zys[i] = (int16)((AD_OUT_SCR_I.b			* 0.01) * 204.8) + 2048;	break;}//

//------------------------------LVRT----------------------------------------------------------------------------------
			case 95:	{zys[i] = (int16)((NGS_Udq				    * 0.01) * 204.8) + 2048;break;}

			case 96:	{zys[i] = (int16)((PRO.Pnactive 	* 0.001	*  0.005) * 204.8) + 2048;break;}
			case 97:	{zys[i] = (int16)((PRO.Pnreactive 	* 0.001	*  0.01) * 204.8) + 2048;break;}
			case 98:	{zys[i] = (int16)((PRO.Pgactive 	* 0.001	*  0.005) * 204.8) + 2048;break;}
			case 99:	{zys[i] = (int16)((PRO.Pgreactive 	* 0.001	*  0.01) * 204.8) + 2048;break;}
			case 100:	{zys[i] = (int16)((PRO.Psactive 	* 0.001	*  0.005) * 204.8) + 2048;break;}
			case 101:	{zys[i] = (int16)((PRO.Psreactive 	* 0.001	*  0.01) * 204.8) + 2048;break;}
			case 102:	{zys[i] = (int16)((PRO.Pgactive 	* 0.001	*  0.005) * 204.8) + 2048;break;}

			case 103:	{zys[i] = (int16)(((DIP_NPR_I.qflt+DIP_STA_I.qflt)* 0.05) * 204.8) + 2048;break;}
			case 104:	{zys[i] = (int16)((DIP_STA_I.qflt				* 0.005) * 204.8) + 2048;break;}
			case 105:	{zys[i] = (int16)((DIP_NPR_I.qflt				* 0.05) * 204.8) + 2048;break;}

 			case 106:	{zys[i] = (int16)((AD_OUT_NGS_U.a			* 0.01) * 204.8) + 2048;	break;}//
			case 107:	{zys[i] = (int16)((AD_OUT_NGS_U.b 			* 0.01) * 204.8) + 2048;	break;}//
 			case 108:	{zys[i] = (int16)((AD_OUT_NGS_U.c			* 0.01) * 204.8) + 2048;	break;}//
			case 109:	{zys[i] = (int16)((AD_OUT_NGS_U.ab			* 0.01) * 204.8) + 2048;	break;}//

			case 110:	{zys[i] = (int16)((0				      	   ) * 204.8) + 2048;break;}

			case 111:	{zys[i] = (int16)((kq						* 1) * 204.8) + 2048;break;}
			case 112:	{zys[i] = (int16)((Pcreactive 	    * 0.001	*  0.1) * 204.8) + 2048;break;}
			case 113:	{zys[i] = (int16)((1				* 0.01) * 204.8) + 2048;break;}
			case 114:	{zys[i] = (int16)((1				*    1) * 204.8) + 2048;break;}
			case 115:	{zys[i] = (int16)((1				*    1) * 204.8) + 2048;break;}
			case 116:	{zys[i] = (int16)((1				*    1) * 204.8) + 2048;break;}
			case 117:	{zys[i] = (int16)((1				*    1) * 204.8) + 2048;break;}
			case 118:	{zys[i] = (int16)((1				*    1) * 204.8) + 2048;break;}
			case 119:	{zys[i] = (int16)((1				*    1) * 204.8) + 2048;break;}
			case 120:	{zys[i] = (int16)((1				*    1) * 204.8) + 2048;break;}
			case 121:	{zys[i] = (int16)((1				*    1) * 204.8) + 2048;break;}

//-------------------20111208´ÅÁ´¹Û²â------------------------------------------		
//-----------theta Phisdq ²¹³¥idq
/*			case 122:	{zys[i] = (int16)((PHI_DATA_M.theta				*    1) * 204.8) + 2048;break;}

			case 123:	{zys[i] = (int16)((PHI_DATA_M.PHId				*    1) * 204.8) + 2048;break;}
			case 124:	{zys[i] = (int16)((PHI_DATA_M.PHIq				*    1) * 204.8) + 2048;break;}
			case 125:	{zys[i] = (int16)((PHI_DATA_M.PHIdflt			*    1) * 204.8) + 2048;break;}
			case 126:	{zys[i] = (int16)((PHI_DATA_M.PHIqflt			*    1) * 204.8) + 2048;break;}
			case 127:	{zys[i] = (int16)((PHI_DATA_M.PHIdflt2			*    1) * 204.8) + 2048;break;}
			case 128:	{zys[i] = (int16)((PHI_DATA_M.PHIqflt2			*    1) * 204.8) + 2048;break;}
			case 129:	{zys[i] = (int16)((PHI_DATA_M.Id				* 0.01) * 204.8) + 2048;break;}
			case 130:	{zys[i] = (int16)((PHI_DATA_M.Iq				* 0.01) * 204.8) + 2048;break;}

//-----------Rs*IsDQ
			case 131:	{zys[i] = (int16)((MPR_Rs * PHI_DATA_M.ISD		* 0.01) * 204.8) + 2048;break;}
			case 132:	{zys[i] = (int16)((MPR_Rs * PHI_DATA_M.ISQ		* 0.01) * 204.8) + 2048;break;}
//-----------LP input PhisDQ		
			case 133:	{zys[i] = (int16)((PHI_DATA_M.ISD				* 0.01) * 204.8) + 2048;break;}
			case 134:	{zys[i] = (int16)((PHI_DATA_M.ISQ				* 0.01) * 204.8) + 2048;break;}
			case 135:	{zys[i] = (int16)((1				* 0.01) * 204.8) + 2048;break;}
			case 136:	{zys[i] = (int16)((1				* 0.01) * 204.8) + 2048;break;}

			case 137:	{zys[i] = (int16)((PHI_DATA_M.PHIMd				*    1) * 204.8) + 2048;break;}
			case 138:	{zys[i] = (int16)((PHI_DATA_M.PHIMq				*    1) * 204.8) + 2048;break;}
			case 139:	{zys[i] = (int16)((PHI_DATA_M.PHIMdflt			*    1) * 204.8) + 2048;break;}
			case 140:	{zys[i] = (int16)((PHI_DATA_M.PHIMqflt			*    1) * 204.8) + 2048;break;}
			case 141:	{zys[i] = (int16)((MPR_Rs / MPR_Ls * PHI_DATA_M.PHIMdflt*    0.1) * 204.8) + 2048;break;}
			case 142:	{zys[i] = (int16)((MPR_Rs / MPR_Ls * PHI_DATA_M.PHIMqflt*    0.1) * 204.8) + 2048;break;}
			case 143:	{zys[i] = (int16)((QEPDATA.omigarote * PHI_DATA_M.PHIMdflt*    0.01) * 204.8) + 2048;break;}
			case 144:	{zys[i] = (int16)((QEPDATA.omigarote * PHI_DATA_M.PHIMqflt*    0.01) * 204.8) + 2048;break;}
*/
			case 145:	{zys[i] = (int16)((TRS_NGS_U.dflt * SQRT3		* 0.01) * 204.8) + 2048;break;}
/*	
			case 146:	{zys[i] = (int16)((RS_phid                      *    0.1) * 204.8) + 2048;break;}
			case 147:	{zys[i] = (int16)((RS_phiq                      *    0.1) * 204.8) + 2048;break;}
			case 148:	{zys[i] = (int16)((CM_vrd                       *   0.01) * 204.8) + 2048;break;}
			case 149:	{zys[i] = (int16)((CM_vrq                       *   0.01) * 204.8) + 2048;break;}
*/
//			case 150:	{zys[i] = (int16)((TRS_MPR_U.ab                 *   0.01) * 204.8) + 2048;break;}
//			case 151:	{zys[i] = (int16)((TRS_MPR_U.bc                 *   0.01) * 204.8) + 2048;break;}
			case 152:	{zys[i] = (int16)((TRS_MPR_U.a                  *   0.01) * 204.8) + 2048;break;}
			case 153:	{zys[i] = (int16)((TRS_MPR_U.b                  *   0.01) * 204.8) + 2048;break;}
			case 154:	{zys[i] = (int16)((TRS_MPR_U.c                  *   0.01) * 204.8) + 2048;break;}

//---------PHIObs-I_model_201201
/*			case 155:	{zys[i] = (int16)((PHI_DATA_M.Isd               *   0.01) * 204.8) + 2048;break;}
			case 156:	{zys[i] = (int16)((PHI_DATA_M.Isq               *   0.01) * 204.8) + 2048;break;}
			case 157:	{zys[i] = (int16)((PHI_DATA_M.Isdflt            *   0.01) * 204.8) + 2048;break;}
			case 158:	{zys[i] = (int16)((PHI_DATA_M.Isqflt            *   0.01) * 204.8) + 2048;break;}
			case 159:	{zys[i] = (int16)((TRS_SCR_I.dflt               *   0.01) * 204.8) + 2048;break;}
			case 160:	{zys[i] = (int16)((TRS_SCR_I.qflt               *   0.01) * 204.8) + 2048;break;}
			case 161:	{zys[i] = (int16)((PHI_DATA_M.Ird               *   0.01) * 204.8) + 2048;break;}
			case 162:	{zys[i] = (int16)((PHI_DATA_M.Irq               *   0.01) * 204.8) + 2048;break;}
			case 163:	{zys[i] = (int16)((PHI_DATA_M.PHId              *      1) * 204.8) + 2048;break;}
			case 164:	{zys[i] = (int16)((PHI_DATA_M.PHIq              *      1) * 204.8) + 2048;break;}
			case 165:	{zys[i] = (int16)((PHI_DATA_M.PHId0             *      1) * 204.8) + 2048;break;}
			case 166:	{zys[i] = (int16)((PHI_DATA_M.PHIdflt2          *      1) * 204.8) + 2048;break;}
			case 167:	{zys[i] = (int16)((TRS_SCR_I.a               	*   0.01) * 204.8) + 2048;break;}
			case 168:	{zys[i] = (int16)((TRS_SCR_I.b               	*   0.01) * 204.8) + 2048;break;}
*/
			case 169:	{zys[i] = (int16)((PI_DATA_PLL.outflt        *      0.01) * 204.8) + 2048;break;}
			case 170:	{zys[i] = (int16)((CAP4.freqtmp		         *      0.01) * 204.8) + 2048;break;}
			case 171:	{zys[i] = (int16)((1           *      1) * 204.8) + 2048;break;}
			case 172:	{zys[i] = (int16)((1           *      1) * 204.8) + 2048;break;}

//			case 173:	{zys[i] = (int16)((MC_DATA.k		            *      1) * 204.8) + 2048;break;}
//			case 174:	{zys[i] = (int16)((PHI_DATA_M.k		            *      1) * 204.8) + 2048;break;}

			case 175:	{zys[i] = (int16)((1		        *      0.01) * 204.8) + 2048;break;}
			case 176:	{zys[i] = (int16)((1		        *      0.01) * 204.8) + 2048;break;}
			case 177:	{zys[i] = (int16)((TRS_STA_I.d		      		*      0.01) * 204.8) + 2048;break;}
			case 178:	{zys[i] = (int16)((TRS_STA_I.q			        *      0.01) * 204.8) + 2048;break;}
//			case 179:	{zys[i] = (int16)((PHI_DATA_M.PHIq0             *      1) * 204.8) + 2048;break;}

//------------Te and Isdq feedback
			case 184:	{zys[i] = (int16)((Te_feedback		    		*      0.001) * 204.8) + 2048;break;}

			case 185:	{zys[i] = (int16)((Te_instant			        *      0.001) * 204.8) + 2048;break;}
/*			case 186:	{zys[i] = (int16)((Te_instant2			        *      0.001) * 204.8) + 2048;break;}
			case 187:	{zys[i] = (int16)((Te_instant3			        *      0.001) * 204.8) + 2048;break;}
			case 188:	{zys[i] = (int16)((Isd_feedback		    		*      0.005) * 204.8) + 2048;break;}
			case 189:	{zys[i] = (int16)((Isq_feedback			        *      0.005) * 204.8) + 2048;break;}
*/

			case 200:	{zys[i] = (int16)((GIVE.toqrf			    	*      0.001) * 204.8) + 2048;break;}
			case 201:	{zys[i] = (int16)((GIVE.toqrfflt			    *      0.001) * 204.8) + 2048;break;}
			case 202:	{zys[i] = (int16)((GIVE.toqrfrun			    *      0.001) * 204.8) + 2048;break;}
			case 203:	{zys[i] = (int16)((RUN.toqrf			    	*      0.001) * 204.8) + 2048;break;}
			case 204:	{zys[i] = (int16)((RUN.mpriqrf_g		   		*      0.01) * 204.8) + 2048;break;}

			case 205:	{zys[i] = (int16)((DIP_STA_U.dflt			    *      0.01) * 204.8) + 2048;break;}
			case 206:	{zys[i] = (int16)((DIP_STA_U.qflt			    *      0.01) * 204.8) + 2048;break;}
			case 207:	{zys[i] = (int16)((DIP_STA_I.dflt			    *      0.005) * 204.8) + 2048;break;}
			case 208:	{zys[i] = (int16)((DIP_STA_I.qflt		   		*      0.005) * 204.8) + 2048;break;}
			case 209:	{zys[i] = (int16)((DIP_NPR_U.dflt			    *      0.01) * 204.8) + 2048;break;}
			case 210:	{zys[i] = (int16)((DIP_NPR_U.qflt			    *      0.01) * 204.8) + 2048;break;}
			case 211:	{zys[i] = (int16)((DIP_NPR_I.dflt			    *      0.01) * 204.8) + 2048;break;}
			case 212:	{zys[i] = (int16)((DIP_NPR_I.qflt		   		*      0.01) * 204.8) + 2048;break;}

			case 213:	{zys[i] = (int16)((TRS_MPR_I.qflt		   		*      0.01) * 204.8) + 2048;break;}
			case 214:	{zys[i] = (int16)((DIP_MPR_I.qflt		   		*      0.01) * 204.8) + 2048;break;}

			case 215:	{zys[i] = (int16)((PRO.Pnactive2 	* 0.001	*  0.005) * 204.8) + 2048;break;}
			case 216:	{zys[i] = (int16)((PRO.Pnreactive2 	* 0.001	*  0.01) * 204.8) + 2048;break;}
			case 217:	{zys[i] = (int16)((PRO.Pgactive2 	* 0.001	*  0.005) * 204.8) + 2048;break;}
			case 218:	{zys[i] = (int16)((PRO.Pgreactive2 	* 0.001	*  0.01) * 204.8) + 2048;break;}
			case 219:	{zys[i] = (int16)((PRO.Psactive2 	* 0.001	*  0.005) * 204.8) + 2048;break;}
			case 220:	{zys[i] = (int16)((PRO.Psreactive2 	* 0.001	*  0.01) * 204.8) + 2048;break;}

			case 221:	{zys[i] = (int16)((CNT_SCRON				*  1) * 204.8) + 2048;break;}
			case 222:	{zys[i] = (int16)((NGS_Udq_pflt				*  0.01) * 204.8) + 2048;break;}


//------------²»Æ½ºâ ÏÝ²¨Æ÷
/*			case 200:	{zys[i] = (int16)((TRS_NGS_U.dflt			    *      0.01) * 204.8) + 2048;break;}
			case 201:	{zys[i] = (int16)((TRS_NGS_U.qflt			    *      0.01) * 204.8) + 2048;break;}
			case 202:	{zys[i] = (int16)((TRS_NGS_U.dnflt			    *      0.01) * 204.8) + 2048;break;}
			case 203:	{zys[i] = (int16)((TRS_NGS_U.qnflt		   		*      0.01) * 204.8) + 2048;break;}
			case 204:	{zys[i] = (int16)((TRS_NGS_U.dbsf			    *      0.01) * 204.8) + 2048;break;}
			case 205:	{zys[i] = (int16)((TRS_NGS_U.qbsf			    *      0.01) * 204.8) + 2048;break;}
			case 206:	{zys[i] = (int16)((TRS_NGS_U.dnbsf		    	*      0.01) * 204.8) + 2048;break;}
			case 207:	{zys[i] = (int16)((TRS_NGS_U.qnbsf			    *      0.01) * 204.8) + 2048;break;}
			case 208:	{zys[i] = (int16)((NGS_Udq_p2			        *      0.01) * 204.8) + 2048;break;}
			case 209:	{zys[i] = (int16)((NGS_Udq_n2		    		*      0.01) * 204.8) + 2048;break;}
			case 210:	{zys[i] = (int16)((NGS_Udq_epsilon2			    *      0.1)  * 204.8) + 2048;break;}
*/
           default:   break;	
		}
	}

//------------------DA channel adjust----------------------------------------------------------
		zys[0] = zys[0] - 0;	//adjusst ad channel  201205LVRTatZB
		zys[1] = zys[1] + 0;	//adjusst ad channel
		zys[2] = zys[2] + 0;	//adjusst ad channel
		zys[3] = zys[3] - 0;	//adjusst ad channel

		if		(zys[0]>=4095)  	zys[0] = 4095;
		else if	(zys[0]<=0)			zys[0] = 0; 

		if		(zys[1]>=4095)  	zys[1] = 4095;
		else if	(zys[1]<=0)			zys[1] = 0;  

		if		(zys[2]>=4095)  	zys[2] = 4095;
		else if	(zys[2]<=0)			zys[2] = 0;  

		if		(zys[3]>=4095)  	zys[3] = 4095;
		else if	(zys[3]<=0)			zys[3] = 0;  
		 
	*DA_ADD0 =	zys[0];
	*DA_ADD1 =	zys[1];
	*DA_ADD2 =	zys[2];
	*DA_ADD3 =	zys[3];

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
** ÐÞË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Output(void)
{
//------------------------------Êý¾ÝÊä³ö-----------------------------------

   *OUT1_ADDR = _OUT1_DATA;
   *OUT2_ADDR = _OUT2_DATA;

//-----------------------------Æô¶¯¿ª¹ØÒâÍâ¶¯×÷¼ì²â110818-------------------------

	if(state_n==0)
	{
		action1 = (_OUT1_DATA & 0x003F);
		state_n=1;
	}
	else
	{
		action2 = (_OUT1_DATA & 0x003F);
		state_n=0;
	}

	if(action1 != action2)
	{
		M_ClrFlag(SL_SWITCHDETECT);									//´¦ÓÚÕý³£¶¯×÷¼ì²â
		state_m=0;
		MAIN_LOOP.cnt_detectdelay = 0;
	}

	if(state_m==0)
	{
 	    if(M_ChkCounter(MAIN_LOOP.cnt_detectdelay,DELAY_DETECT)>=0)		//´ÓÕý³£¼ì²â×ª»»ÖÁÒâÍâ¼ì²âÖ®¼äÑÓÊ±_STDBY4 ms
 	    {
 	    	state_m=1;

			MAIN_LOOP.cnt_cberror=0;								//Çå¶¨Ê±Æ÷
			MAIN_LOOP.cnt_mainkerror=0;								//Çå¶¨Ê±Æ÷
			MAIN_LOOP.cnt_mainferror=0;								//Çå¶¨Ê±Æ÷
			MAIN_LOOP.cnt_stacerror=0;								//Çå¶¨Ê±Æ÷
		}

	}

	else if((action1 == action2) && (state_m==1))
	{
	    M_SetFlag(SL_SWITCHDETECT);									//´¦ÓÚÒâÍâ¶¯×÷¼ì²â

	}

//---------20121107----------testCL_ZKLVRT--------
	if((_STDBY9&0x0002)!=0)		M_SetFlag(CL_ZKLVRT);	//116.1	atsp

//---------20121107----------testCL_ZKLVRT--------

//-----------------------------¹¦ÂÊÉ¢ÈÈ·ç»ú¿ØÖÆ----------------------------------
	if(M_ChkFlag(SL_FAN_WORK)!=0) 	M_SetFlag(CL_FANWORK);
	else							M_ClrFlag(CL_FANWORK); 
/*
//-----------------------------¹¦ÂÊÉ¢ÈÈ·ç»ú¿ØÖÆ--20100429systest--------------------------------
	if(M_ChkFlag(SL_FAN_WORK)!=0) 	
	{
		if(AMUX.skiiptempmax>35.0)	 	M_SetFlag(CL_FANWORK);          //Æô¶¯¹¦ÂÊ×é¼þ·ç»ú
		else if(AMUX.skiiptempmax<15.0)	M_ClrFlag(CL_FANWORK);          //¹Ø±Õ¹¦ÂÊ×é¼þ·ç»ú
	}
	else
	{
//		if(AMUX.skiiptempmax<50.0)	 M_ClrFlag(CL_FANWORK);          	//¹Ø±Õ¹¦ÂÊ×é¼þ·ç»ú
		M_ClrFlag(CL_FANWORK); 
	}							
*/
//---------------------CANOPEN·´À¡Ö÷¿ØµÄ×´Ì¬±äÁ¿---------------------------------------------------
//-----------------------------------tx_state1----------------------------------------	
	if(M_ChkFlag(SL_CBCLOSED)!=0)	  		SCI_canopen.tx_state1 |= COM_NPRREADY;
	else									SCI_canopen.tx_state1 &= COM_NPRREADY_NOT;

	if(M_ChkFlag(SL_NPR_PWMOUT)!=0)	  		SCI_canopen.tx_state1 |= COM_NPRON;		//201005atcpc
	else 									SCI_canopen.tx_state1 &= COM_NPRON_NOT;

	if(M_ChkFlag(SL_MPR_SYNOK)!=0)	  		SCI_canopen.tx_state1 |= COM_READYGENERATION;
	else 									SCI_canopen.tx_state1 &= COM_READYGENERATION_NOT;

	if(M_ChkFlag(SL_SPEED_IN_RANGE)!=0)	  	SCI_canopen.tx_state1 |= COM_SPEEDINRANGE;
	else 									SCI_canopen.tx_state1 &= COM_SPEEDINRANGE_NOT;

	if((M_ChkFlag(SL_SERIESTOP)!=0)||(M_ChkFlag(SL_ERRSTOP)!=0))	  	  SCI_canopen.tx_state1 |= COM_FAILURE;
	else  if((M_ChkFlag(SL_SERIESTOP)==0)&&(M_ChkFlag(SL_ERRSTOP)==0))	  SCI_canopen.tx_state1 &= COM_FAILURE_NOT;

	if(M_ChkFlag(SL_SERIESTOP)!=0)	  		SCI_canopen.tx_state1 |= COM_FAILUREA;
	else 									SCI_canopen.tx_state1 &= COM_FAILUREA_NOT;

	if(M_ChkFlag(SL_ERRSTOP)!=0)	  		SCI_canopen.tx_state1 |= COM_FAILUREB;
	else 									SCI_canopen.tx_state1 &= COM_FAILUREB_NOT;

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

//--------120507---------
/*	if(temp_flag3==0)	
	{
		if(M_ChkFlag(SL_LV_QWORKING)!=0)	 //20130225
			SCI_canopen.tx_torque= (int16)(Te_feedback_lv * 16383.0 / CAN_TEN);
		else
			SCI_canopen.tx_torque= (int16)(Te_feedback * 16383.0 / CAN_TEN);
//		SCI_canopen.tx_torque= (int16)(DISP.toqfb * 16383.0 / CAN_TEN);
//		SCI_canopen.tx_torque= (int16)(5 * 16383.0 / CAN_TEN);
		SCI_canopen.tx_skiiptempmax = (int16)(AMUX.skiiptempmax * 16383.0 / CAN_TEMP);
//		SCI_canopen.tx_skiiptempmax = (int16)(50 * 16383.0 / CAN_TEMP);
		temp_flag3 =1;
	}
	else
	{
		if(M_ChkFlag(SL_LV_QWORKING)!=0)	 //20130225
			SCI_canopen.tx_torque= (int16)(Te_feedback_lv * 16383.0 / CAN_TEN);
		else
			SCI_canopen.tx_torque= (int16)(Te_feedback * 16383.0 / CAN_TEN);
//		SCI_canopen.tx_torque= (int16)(DISP.toqfb * 16383.0 / CAN_TEN) + 1;
//		SCI_canopen.tx_torque= (int16)(5 * 16383.0 / CAN_TEN) + 1;
		SCI_canopen.tx_skiiptempmax = (int16)(AMUX.skiiptempmax * 16383.0 / CAN_TEMP) + 1;
//		SCI_canopen.tx_skiiptempmax = (int16)(50 * 16383.0 / CAN_TEMP) + 1;
		temp_flag3 =0;
	}
*/
//--------20130308--------
		if(M_ChkFlag(SL_IN1_STATORK)!=0)
		{
			if(M_ChkFlag(SL_LV_QWORKING)!=0)	 //20130225
				SCI_canopen.tx_torque= (int16)(Te_feedback_lv * 16383.0 / CAN_TEN);
			else
				SCI_canopen.tx_torque= (int16)(Te_feedback * 16383.0 / CAN_TEN);
		}
		else	SCI_canopen.tx_torque= 0;

		if(temp_flag3==0)
		{
			SCI_canopen.tx_skiiptempmax = (int16)(AMUX.skiiptempmax * 16383.0 / CAN_TEMP);
			temp_flag3 =1;
		}
		else
		{
			SCI_canopen.tx_skiiptempmax = (int16)(AMUX.skiiptempmax * 16383.0 / CAN_TEMP) + 1;
			temp_flag3 =0;
		}



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
	float temp_Usn,temp_Uro;
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

//---------------------CANOPENÖ¸Áî¿ØÖÆ------------------------------------------------------

if(M_ChkFlag(SL_ENPCOPER)==0)
{ 
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

/*//----------20120321--------------
	if(((SCI_canopen.rx_controlword & COM_EIN)==COM_EIN) && (M_ChkFlag(SL_SYSOFF)==0))	    
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_ocsein1,DELAY_OCSEIN1)>0)		
		{
			M_SetFlag(SL_OCS_EIN);		//20090817 250ms
			M_SetFlag(SL_OCS_SYSRUN);	//20110707·ûºÏ»ª´´Í¨Ñ¶,Ô­2¸öÖ¸ÁîºÏ¶þÎªÒ»
		}
		MAIN_LOOP.cnt_ocsein2=0;	
	}
	else
	{
		if(M_ChkFlag(SL_SYSOFF)!=0)
		{
			M_ClrFlag(SL_OCS_EIN);
			M_ClrFlag(SL_OCS_SYSRUN);
		}
		
		if(M_ChkCounter(MAIN_LOOP.cnt_ocsein2,DELAY_OCSEIN2)>0)		
		{
			M_ClrFlag(SL_OCS_EIN);		//20090817 250ms 
			M_ClrFlag(SL_OCS_SYSRUN); 	//20110707·ûºÏ»ª´´Í¨Ñ¶,Ô­2¸öÖ¸ÁîºÏþÎªÒ»
		}
		MAIN_LOOP.cnt_ocsein1=0;
	}
//-----------------------------
*/

//---------------------ÉÏÎ»»úÒªÇó¸´Î»¹ÊÕÏ------------------------------------------------------
	if((SCI_canopen.rx_controlword & COM_OCSRESET)==COM_OCSRESET)	M_SetFlag(SL_OCS_RESET);									
	else 	   														M_ClrFlag(SL_OCS_RESET);													

/*	
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
*/
//---------------------ÉÏÎ»»úÒªÇó±äÁ÷Æ÷Æô¶¯-------20121206---------------------------------------
	if(((SCI_canopen.rx_controlword & COM_SYSRUN)==COM_SYSRUN) && (M_ChkFlag(SL_SYSOFF)==0) && (PRO.speedflt>950))
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_ocssysrun1,DELAY_OCSSYSRUN1)>0)	M_SetFlag(SL_OCS_SYSRUN);	//20090817 250ms
		MAIN_LOOP.cnt_ocssysrun2=0;	
	}
	else
	{
		if(M_ChkFlag(SL_SYSOFF)!=0)			M_ClrFlag(SL_OCS_SYSRUN);

		if(M_ChkCounter(MAIN_LOOP.cnt_ocssysrun2,DELAY_OCSSYSRUN2)>0)	M_ClrFlag(SL_OCS_SYSRUN);	//20090817  250ms
		MAIN_LOOP.cnt_ocssysrun1=0;
	}

//---------20130306--------
	if(M_ChkFlag(SL_LV_CLRERRAM)!=0)
	{
		cnt_lv_clrerram ++ ;		//20130309
		M_SetFlag(SL_PC_ERRDATACLR);
		if(cnt_lv_clrerram>=10)		//20130309
		{
			M_ClrFlag(SL_LV_CLRERRAM);
			cnt_lv_clrerram = 0;
		}
	}
//-------------------------

//---------------------ÉÏÎ»»úÒªÇóÍø²à±äÁ÷Æ÷µ¥ÀÔËÐÐÎÞ¹¦²¢Íø------------------------------------------------------
//	if((SCI_canopen.rx_controlword & COM_NPREIN)==COM_NPREIN)	M_SetFlag(SL_OCS_NPREIN);  //ÔÝÊ±ÆÁ±Î										
//	else 		  												M_ClrFlag(SL_OCS_NPREIN); 
}	 
//---------------------------½ÓÊÕ²Ù×÷Æ÷À´µÄPI»·²ÎÊý-------------------------------------------------
	PI_PARA_NPRU.kp           = _NPR_U_Kp/100.00;			//DOT2ÊôÐÔ
    PI_PARA_NPRU.ki           = _NPR_U_Ki/10.00;			//DOT1ÊôÐÔ
//	PI_PARA_NPRU.kd           = _NPR_U_Kd/1000.00;			//DOT3ÊôÐÔ
	PI_PARA_NPRU.outmax       = _NPR_U_outmax;
	PI_PARA_NPRU.errmax       = _NPR_U_errmax/10.00;		//DOT1ôÐÔ		//NPRµçÑ¹»·²ÎÊý
	PI_PARA_NPRU.errmin       = _NPR_U_errmin/1000.00;	    //DOT3ÊôÐÔ
	PI_PARA_NPRU.incrementmax = _NPR_U_incrementmax/10.00;  //DOT1ÊôÐÔ
/*
	PI_PARA_NPRID.kp           = _NPR_ID_Kp/100.00;			//DOT2ÊôÐÔ
    PI_PARA_NPRID.ki           = _NPR_ID_Ki/10.00;			//DOT1ÊôÐÔ
//    PI_PARA_NPRID.kd           = _NPR_ID_Kd/1000.00;			//DOT3ÊôÐÔ
	PI_PARA_NPRID.outmax       = _NPR_ID_outmax;		 					    //NPRµçÁ÷»·²ÎÊý
	PI_PARA_NPRID.errmax       = _NPR_ID_errmax/10.00;		//DOT1ÊôÐÔ
	PI_PARA_NPRID.errmin       = _NPR_ID_errmin/1000.00;		//DOT3ÊôÐÔ
	PI_PARA_NPRID.incrementmax = _NPR_ID_incrementmax/10.00;	//DOT1ÊôÐÔ
*/
//--------20121210--------
	lv_ctrl1           = _NPR_ID_Kp/100.00;			//DOT2ÊôÐÔ
    lv_ctrl2           = _NPR_ID_Ki/10.00;			//DOT1ÊôÐÔ
	lv_ctrl3	       = _NPR_ID_outmax;		 					    //NPRµçÁ÷»·²ÎÊý
	lv_ctrl4 			= _NPR_ID_errmax/10.00;		//DOT1ÊôÐÔ
	lv_ctrl5			= _NPR_ID_errmin/1000.00;		//DOT3ÊôÐÔ
	lv_ctrl6			= _NPR_ID_incrementmax/10.00;	//DOT1ÊôÐÔ

	PI_PARA_NPRID.kp           = 0.4;			//DOT2ÊôÐÔ
    PI_PARA_NPRID.ki           = 167;			//DOT1ÊôÐÔ
	PI_PARA_NPRID.outmax       = 200;		    //NPRµçÁ÷»·²ÎÊý
	PI_PARA_NPRID.errmax       = 50;			//DOT1ÊôÐÔ
	PI_PARA_NPRID.errmin       = 0;				//DOT3ÊôÐÔ
	PI_PARA_NPRID.incrementmax = 55;			//DOT1ÊôÐÔ
//--------20121210--------

	PI_PARA_NPRIQ.kp           = _NPR_IQ_Kp/100.00;			//DOT2ÊôÐÔ
    PI_PARA_NPRIQ.ki           = _NPR_IQ_Ki/10.00;			//DOT1ÊôÐÔ
//    PI_PARA_NPRIQ.kd           = _NPR_IQ_Kd/1000.00;			//DOT3ÊôÐÔ
	PI_PARA_NPRIQ.outmax       = _NPR_IQ_outmax;		 					    //NPRµçÁ÷»·²ÎÊý
	PI_PARA_NPRIQ.errmax       = _NPR_IQ_errmax/10.00;		//DOT1ÊôÐÔ
	PI_PARA_NPRIQ.errmin       = _NPR_IQ_errmin/1000.00;		//DOT3ÊôÐÔ
	PI_PARA_NPRIQ.incrementmax = _NPR_IQ_incrementmax/10.00;	//DOT1ÊôÐÔ

	PI_PARA_MPRID.kp           = _MPR_ID_Kp/100.00;			//DOT2ÊôÐÔ
    PI_PARA_MPRID.ki           = _MPR_ID_Ki/10.00;			//DOT1ÊôÐÔ
 //   PI_PARA_MPRID.kd           = _MPR_ID_Kd/1000.00;			//DOT3ÊôÐÔ
	PI_PARA_MPRID.outmax       = _MPR_ID_outmax;   							//MPRµçÁ÷»·²ÎÊý
	PI_PARA_MPRID.errmax       = _MPR_ID_errmax/10.00;		//DOT1ÊôÐÔ
	PI_PARA_MPRID.errmin       = _MPR_ID_errmin/1000.00;		//DOT3ÊôÐÔ
	PI_PARA_MPRID.incrementmax = _MPR_ID_incrementmax/10.00;	//DOT1ÊôÐÔ

	PI_PARA_MPRIQ.kp           = _MPR_IQ_Kp/100.00;			//DOT2ÊôÐÔ
    PI_PARA_MPRIQ.ki           = _MPR_IQ_Ki/10.00;			//DOT1ÊôÐÔ
//    PI_PARA_MPRIQ.kd           = _MPR_IQ_Kd/1000.00;			//DOT3ÊôÐÔ
	PI_PARA_MPRIQ.outmax       = _MPR_IQ_outmax;   							//MPRµçÁ÷»·²ÎÊý
	PI_PARA_MPRIQ.errmax       = _MPR_IQ_errmax/10.00;		//DOT1ÊôÔ
	PI_PARA_MPRIQ.errmin       = _MPR_IQ_errmin/1000.00;		//DOT3ÊôÐÔ
	PI_PARA_MPRIQ.incrementmax = _MPR_IQ_incrementmax/10.00;	//DOT1ÊôÐÔ
/*
	PI_PARA_MPRU.kp           = _MPR_U_Kp/100.00;			//DOT2ÊôÐÔ
    PI_PARA_MPRU.ki           = _MPR_U_Ki/10.00;			//DOT1ÊôÐÔ
    PI_PARA_MPRU.kd           = _MPR_U_Kd/1000.00;			//DOT3ÊôÐÔ
	PI_PARA_MPRU.outmax       = _MPR_U_outmax;   							//MPRµçÑ¹»·²ÎÊý
	PI_PARA_MPRU.errmax       = _MPR_U_errmax/10.00;		//DOT1ÊôÐÔ
	PI_PARA_MPRU.errmin       = _MPR_U_errmin/1000.00;		//DOT3ÊôÐÔ
	PI_PARA_MPRU.incrementmax = _MPR_U_incrementmax/10.00;  //DOT1ÊôÐÔ

	PI_PARA_DYNU.kp           = _DYN_U_Kp/100.00;			//DOT2ÊôÐÔ
    PI_PARA_DYNU.ki           = _DYN_U_Ki/10.00;			//DOT1ÊôÐÔ
    PI_PARA_DYNU.kd           = _DYN_U_Kd/1000.00;			//DOT3ÊôÐÔ
	PI_PARA_DYNU.outmax       = _DYN_U_outmax;   							//¶¯Ì¬µçÑ¹»·²ÎÊý
	PI_PARA_DYNU.errmax       = _DYN_U_errmax/10.00;		//DOT1ÊôÐÔ
	PI_PARA_DYNU.errmin       = _DYN_U_errmin/1000.00;		//DOT3ÊôÐÔ
	PI_PARA_DYNU.incrementmax = _DYN_U_incrementmax/10.00;  //DOT1ÊôÐÔ
*/
//----Íø²àµçÑ¹»·Æô¶¯¶¯Ì¬PI¸ø¹Ì¶¨ÖµBJTULVRT201204
	PI_PARA_DYNU.kp           = 0.15;						//DOT2ÊôÐÔ
    PI_PARA_DYNU.ki           = 3;							//DOT1ÊôÐÔ
    PI_PARA_DYNU.kd           = 0;							//DOT3ÊôÐÔ
	PI_PARA_DYNU.outmax       = 600;   						//¶¯Ì¬µçÑ¹»·²ÎÊý
	PI_PARA_DYNU.errmax       = 100;						//DOT1ÊôÐÔ
	PI_PARA_DYNU.errmin       = 0;							//DOT3ÊôÐÔ
	PI_PARA_DYNU.incrementmax = 100;  						//DOT1ÊôÐÔ
//-----»ú²à¶¯Ì¬PI²ÎÊýBJTULVRT201204	_MPR_U_KpÔÚµ÷ÊÔ½çÃæÀïÃ»ÓÐ ÔÝÊ±²»ÓÃ,Ê×Ñ¡PR
	PI_PARA_DYMI.kp           = _DYN_U_Kp/100.00;			//DOT2ÊôÐÔ
    PI_PARA_DYMI.ki           = _DYN_U_Ki/10.00;			//DOT1ÊôÐÔ
    PI_PARA_DYMI.kd           = _DYN_U_Kd/1000.00;			//DOT3ÊôÐÔ
	PI_PARA_DYMI.outmax       = _DYN_U_outmax;   							//MPRµçÑ¹»·²ÎÊý
	PI_PARA_DYMI.errmax       = _DYN_U_errmax/10.00;		//DOT1ÊôÐÔ
	PI_PARA_DYMI.errmin       = _DYN_U_errmin/1000.00;		//DOT3ÊôÐÔ
	PI_PARA_DYMI.incrementmax = _DYN_U_incrementmax/10.00;  //DOT1ÊôÐÔ
//----Í¬Ê±ÓÃ×÷¶¯Ì¬PIºÍPR²ÎÊýÉè¶¨

	_eidco   = _EIDCO/1000.00;
	_encodpos= _ENCODPOS/1000.00;
	_sc_freq1= _SC_FREQ1/10.00;
	_sc_freq2= _SC_FREQ2/10.00;
	_stdby01 = _STDBY1/1000.00;								//±¸ÓÃ¾­¹ýÐ¡Êýã´¦ÀíºóÖµ
	_stdby02 = _STDBY2/100.00;								//±¸ÓÃ¾­¹ýÐ¡Êýµã´¦ÀíºóÖµ
	_stdby03 = _STDBY3/10.00;								//±¸ÓÃ¾­¹ýÐ¡Êýµã´¦ÀíºóÖµ
    _stdby04 = _STDBY4;			        					//±¸ÓÃ
	_stdby05 = _STDBY5;			        					//±¸ÓÃ
    _stdby06 = _STDBY6;			        					//±¸ÓÃ
	_stdby07 = _STDBY7;			        					//±¸ÓÃ
//    _stdby08 = _STDBY8;			        					//±¸ÓÃ
//¼ÆËã¶¨×ÓµçÑ¹AD´óÂË²¨±ÈÂÊ2013-12-13--ZZJ
	Uns_ft = 0.5 * ((MEAN_DATA.uab3/MEAN_DATA.uab1)+(MEAN_DATA.ubc3/MEAN_DATA.ubc1));//´óÂË²¨·ùÖµË¥¼õ±ÈÀý

	daindex[0] = (Uint16)_NPR_U_Kd;	//da output select BJTULVRT201204
	daindex[1] = (Uint16)_NPR_ID_Kd;
	daindex[2] = (Uint16)_NPR_IQ_Kd;
	daindex[3] = (Uint16)_MPR_ID_Kd;

//×ª×ÓµçÁ÷´ø×èÂË²¨²ÎÊý BJTULVRT201204
	BS_PARA_MPRI.f0 = 300;
	BS_PARA_MPRI.fb = 50;
	BS_PARA_MPRI.outmax = 1800;//201205LVRTatZB
//¶¨×ÓµçÁ÷´ø×èÂË²¨²ÎÊý BJTULVRT201204
	BS_PARA_STAI.f0 = 50;
	BS_PARA_STAI.fb = 40;
	BS_PARA_STAI.outmax = 8000; //201205LVRTatZB

//µç»ú²ÎÊýÉè¶¨201005atcpc

	MPR_Ls = (_SC_Lls * 1e-6)+(_SC_Lm * 1e-6);				//¶¨×Ó×Ô¸Ð	--µ¥Î»H
	MPR_Lr = (_SC_Llr * 1e-6)+(_SC_Lm * 1e-6);				//×ª×Ó×Ô¸Ð	--µ¥Î»H	
	MPR_Lm = _SC_Lm * 1e-6;									//¶¨×ª×Ó»¥¸Ð--µ¥Î»H
	MPR_Rs = _SC_Rs * 1e-6;									//¶¨×Óµç×è	--µ¥Î»ohm
	MPR_Rr = _SC_Rr * 1e-6;									//×ª×Óµç×è	--µ¥Î»ohm
	SIGMA  = 1- (MPR_Lm * MPR_Lm /(MPR_Ls * MPR_Lr));		//(1-MPR_Lm*MPR_Lm/(MPR_Ls*MPR_Lr))	
	
	temp_Usn  = (float)_SC_Usn;								//¶¨×Ó¶î¶¨µçÑ¹(V)
	temp_Uro  = (float)_SC_Uro;								//×ª×Ó¿ªÂ·µçÑ¹(V)
	STAROTRTO = temp_Usn * SQRT3 / temp_Uro;				//¶¨×ª×Ó±ä±È Ö»ÊÊÓÃ¶¨×Ó½Ç½Ó×ª×ÓÐÇ½ÓµÄµç»ú
	STAROTRTO2= STAROTRTO * STAROTRTO;						//¶¨×ª×Ó±ä±ÈµÄÆ½·½
	POLEPAIRES= (float)_SC_POLES;							//¼«¶ÔÊý

 	if(_SC_PLSPRVL != 2048)									//201007BJTULVRT Èç¹û²»ÊÇÈ±Ê¡Öµ2048,ÔòÖØÐÂ¸³Öµ
	{
		if(EQep2Regs.QPOSMAX != (4 * _SC_PLSPRVL))
		{
			EALLOW;			
 			EQep2Regs.QPOSMAX   = 4 * _SC_PLSPRVL;			// Î»ÖÃ¼ÆÊýÆ÷×î´óÖµ
			EDIS;	
		}
	}
   
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
    EPwm1Regs.TBPRD = 3750000/_SW_FR;           // ¿ª¹ØÆµÂÊÎª×÷¸ø¨,_SW_FR=200¶ÔÓ¦2kHz
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
//===========================================================================
