   /****************************************Copyright (c)**************************************************
**                       		     ±±	¾©	½»	Í¨	´ó	Ñ§
**                                        µçÆø¹¤³ÌÑ§Ôº
**                                         604ÊµÑéÊÒ
** 
**                              
**
**--------------ÎÄ¼þÐÅÏ¢--------------------------------------------------------------------------------
**ÎÄ   ¼þ   Ãû: user_work.c
**´´   ½¨   ÈË: 
**×îºóÐÞ¸ÄÈÕÆÚ: 
**Ãè        Êö: ÓÒÓñ1.5MWË«À¡·çµç»ú×é²¢Íø±äÁ÷Æ÷¿ØÖÆÈí¼þºËÐÄ¿ØÖÆ³ÌÐò----201005atcpc-201007atbjtu
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
#include "math.h"

//º¯ÊýÉùÃ÷systest201005atcpc
void RUN_SYN(void);
void NOR_STOP(void);
void SER_STOP(void);
void ERR_STOP(void);
void CB_ON(void); 
//void CONTACTOR_TEST(void);

//--------20121103--------
void PTransform_3s_2s_2r(struct TRANS_DATA *var2);		//201011PLL
void NTransform_3s_2s_2r(struct TRANS_DATA *var3);
void De_ONE_P_2r(struct TRANS_DATA *var4);
void De_ONE_N_2r(struct TRANS_DATA *var5);
void PLL_PI_Loop(struct PI_DATA *var,struct PI_PARA PI_var,struct TRANS_DATA *var1);
void I_Loop(struct PI_DATA *var6); 
//--------20121103--------

/*********************************************************************************************************
** º¯ÊýÃû³Æ: InitWork
** ¹¦ÄÜÃèÊö: ³õÊ¼»¯WORK²¿·Ö£¬°üÀ¨GIVE£¬RunCtrl£¬WAVE£¬SCOUT
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
void InitWork(void)
{
	M_SetFlag(SL_POWERON);		//ÖÃ³õ´ÎÉÏµç±êÖ¾
	_MSG_SCOUT1=MSG_NONE;		//¹ÊÕÏÐÅÏ¢Çå³ý
	_MSG_SCOUT2=MSG_NONE;		//¹ÊÕÏÐÅÏ¢Çå³ý
	MAIN_LOOP.pulse = 0;		//·ÖÊ±Âö³å¸³³õÖµ
	M_ClrFlag(SL_OCS_NPRSTART);
    M_ClrFlag(SL_OCS_MPRSTART);
	M_ACROWBAROFF();			//ACTIVECROWBAR¹Ø¶Ï
	if(GpioDataRegs.GPBDAT.bit.GPIO50==0)	DEBUG_OPER=1;  //mode0Ê¹ÄÜ£¬±äÁ¿³õÊ¼»¯³õÖµ
	else									DEBUG_OPER=0;  //20100401at27

}


/*********************************************************************************************************
** º¯ÊýÃû³Æ: PwmDrive
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
void PwmDrive(void)
{
    if((M_ChkFlag(SL_POWERON)==0)&&(M_ChkFlag(SL_CHARGEOK)!=0))		//DSP²»ÊÇÕýÔÚÉÏµç£¬ÒÑÉÏÍêµç,ÏµÍ³Ô¤³äµçÍê³É
 	{ 	
		    //--ÏÂÃæÖ´ÐÐµÄÊÇ¿ª¹ØÆµÂÊÅäÖÃ£¬ÔÚÍ£»ú×´Ì¬ÏÂÔÊÐí²Ù×÷Æ÷ÉèÖÃ¿ª¹ØÆµÂÊ£¬Ã¿´ÎÆô¶¯Ê±¸üÐÂÒ»´Î¿ª¹ØÆµÂÊ----
			if(M_ChkFlag(SL_OCS_NPRSTART)!=0)      					//Ö÷¿ØÒªÇó±äÁ÷Æ÷Æô¶¯
			{
			   if(M_ChkFlag(SL_CONFIGPWM)==0 && M_ChkFlag(SL_NPR_PWMOUT)==0 && M_ChkFlag(SL_MPR_PWMOUT)==0)  //Ö»ÓÐÍ£Âö³åÁË²ÅÔÊÐíÔÙÅäÖÃPWM						//Ã»ÓÐÅäÖÃ¿ª¹ØÆµÂÊ
		       {								  
				   ConfigPwm();	                					//ÅäÖÃ¿ª¹ØÆµÂÊ				
				   if(M_ChkCounter(MAIN_LOOP.cnt_pwmout,DELAY_PWMOUT)>=0)	M_SetFlag(SL_CONFIGPWM);  //Âö³å·¢ÉúÔÊÐíÑÓÊ±µ½	20091015 at zy						//Âö³åÔÊÐí,±£Ö¤EnPwm()Ö»ÄÜÔËÐÐÒ»´Î       
		       }
			   else	 MAIN_LOOP.cnt_pwmout=0;						//Çå³ýÏµÍ³¹¤×÷ÔÊÐíÑÓÊ±	
			}
			else   
			{
			   M_ClrFlag(SL_CONFIGPWM);
			   MAIN_LOOP.cnt_pwmout=0;								//Çå³ýÏµÍ³¹¤×÷ÔÊÐíÑÓÊ±
			}

//----------------ÏÂÃæÊÇÕæÕýµÄÂö³åÊ¹ÄÜ×îºóÒ»¹Ø----------------
	        
	 //--------Íø²à±äÁ÷Æ÷Âö³åÔÊÐí,ÇÒÃ»ÓÐ¹ÊÕÏ--------------
//	        if((M_ChkFlag(SL_NPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0)) 
	        if((M_ChkFlag(SL_NPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0)&&(M_ChkFlag(SL_LV_NSTOP)==0)) //201007BJTULVRT
	        {
		       if(M_ChkFlag(SL_NPR_PWMOUT)==0)  					//±£Ö¤Enepwmio_NPR()Ö»ÄÜÔËÐÐÒ»´Î
		       {
			      Enepwmio_NPR();
			          	
				  M_SetFlag(SL_RUN);
			      M_SetFlag(SL_NPR_PWMOUT);			        
		       }
	        }
			else       												//·¢Éú¹ÊÕÏ¾ÍÁ¢¼´·âÂö³å
	        {
		       Disepwmio_NPR(); 
		              
			   M_ClrFlag(SL_NPR_PWMOUT);
			   M_ClrFlag(SL_RUN);
	        }
     //------»ú²à±äÁ÷Æ÷Âö³åÊ¹ÄÜ,ÇÒÃ»ÓÐ¹ÊÕÏ---------------
//            if((M_ChkFlag(SL_MPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0))
            if((M_ChkFlag(SL_MPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0)&&(M_ChkFlag(SL_LV_MSTOP)==0)) //201007BJTULVRT
	        {
		       if(M_ChkFlag(SL_MPR_PWMOUT)==0)	//±£Ö¤Enepwmio_MPR()Ö»ÄÜÔËÐÐÒ»´Î
		       {
			      Enepwmio_MPR();      
			      M_SetFlag(SL_MPR_PWMOUT);
			      M_SetFlag(SL_LV_MPWMOUT);		// LVRT	         
		       }
			   MAIN_LOOP.cnt_lv_mpwmstop=0;
	        }
	        else                  //·¢Éú¹ÊÕÏ¾ÍÁ¢¼´·âÂö³å
	        {		       
			   Disepwmio_MPR();       
			   M_ClrFlag(SL_MPR_PWMOUT); 
			   if(M_ChkCounter(MAIN_LOOP.cnt_lv_mpwmstop,DELAY_MPWMSTOP)>0)	//0ms LVRT 20100125cpcÊÔÑéÈ·¶¨
			   		M_ClrFlag(SL_LV_MPWMOUT);	      						//Âö³å·âµôºóÑÓÊ±0msÔÊÐí¿ªÍ¨SCR
	        } 
	}
    else   //ÏµÍ³ÕýÔÚÉÏµç,»òÔ¤³äµçÕ¢Ã»ÓÐÇÐ³ý
	{      
		DisPwm();
		M_ClrFlag(SL_RUN);											//2010
		M_ClrFlag(SL_NPR_PWMOUT);
		M_ClrFlag(SL_MPR_PWMOUT);
		M_ClrFlag(SL_LV_MPWMOUT);	      							//Âö³å·âµôºóÑÓÊ±1msÔÊÐí¿ªÍ¨SCR
		M_ClrFlag(SL_CONFIGPWM);			        				//Âö³å·âËø
		MAIN_LOOP.cnt_pwmout=0;										//Çå³ýÏµÍ³¹¤×÷ÔÊÐíÑÓÊ±
		MAIN_LOOP.cnt_lv_mpwmstop=0;
	}
} 
//****************************************************************************
//±àºÅ£º
//Ãû³Æ£ºSVPWM
//¹¦ÄÜ£ºÂö³åµ÷ÖÆÊä³ö
//ÊäÈë£º

//Êä³ö£º
//×¢ÊÍ£º
//ÐÞ¸ÄËµÃ÷:
//****************************************************************************
//------------------Íø²à±äÁ÷Æ÷SVPWMµ÷ÖÆ------------------------------------//	
void SVPWM_NPR(float alfa, float beta)
{      
  float tempmin,tempmax,temp1,temp2,temp3;
  float t0,t1,t2;    						 					//¿Õ¼ä»ù±¾Ê¸Á¿µÄ×÷ÓÃÊ±¼ä
  Uint16 sector;      											//ÉÈÇø

  vdc = AD_OUT_UDC;  											//vdcÎªÊµ¼ÊÖ±Á÷µçÑ¹Öµ,cdcÈ¡Êµ¼ÊÖ±Á÷µçÑ¹ÖµÓÃÓÚµ÷ÖÆ
//-----------------------SVPWM ÉÈÇøÅÐ¶ÏºÍÊ¸Á¿×÷ÓÃÊ±¼ä¼ÆËã------------------------------------------
  if(beta>=0)
  {
	   if(SQRT3 * alfa >= beta) 
	   {
	      sector=1;
	      t1=SQRT3 * (alfa * SQRT3_2-beta * 0.5)/vdc;   		//SQRT3=sqrt(3)=1.73205081£¬ÔÚºê¶¨ÒåÀïÃæÊµÏÖ,0.8660254=SQRT3/2
	      t2=SQRT3 * beta/vdc;
	   }
	   else if(SQRT3 * alfa <= -beta) 
	   {
	      sector=3;                                    
	      t1=SQRT3 * beta/vdc;
	      t2=SQRT3 * (-beta * 0.5 - alfa * SQRT3_2)/vdc;      	//SQRT3_2=sqrt(3)/2
	   }
	   else 
       {
	      sector=2;
	      t1=SQRT3 * (alfa * SQRT3_2 + beta * 0.5)/vdc;
	      t2=SQRT3 * (beta * 0.5 - alfa * SQRT3_2)/vdc;
       }
  }
  else
  {
	   if(SQRT3 * alfa >= -beta)
	   {
		  sector=6;
		  t1=-SQRT3 * beta/vdc;
		  t2=SQRT3 * (beta * 0.5 + alfa * SQRT3_2)/vdc;
	   }
	   else if(SQRT3 * alfa <= beta)	
	   {
	      sector=4; 
		  t1=SQRT3 * (beta * 0.5 - alfa * SQRT3_2)/vdc;	   
	      t2=-SQRT3 * beta/vdc;
	   }
	   else 
	   {
	      sector=5;
	      t1=SQRT3 * (-alfa * SQRT3_2 - beta * 0.5)/vdc;
	      t2=SQRT3 * (-beta * 0.5 + alfa * SQRT3_2)/vdc;
	   }
  }
//---------------------test----test-----test------------------------------------------------------
	zsector = sector;
//---------------------test----test-----test------------------------------------------------------

//--------------------¹ýµ÷ÖÆ´¦Àí------------------------------------------------------------------ 
  	if(t1+t2>=1.0)           									//´ËÊ±Ts=1£¬¹ÊÅÐ¶ÏÊ±ÒÔ1×÷Îª±È½Ï
  	{
	  t1=t1/(t1+t2);
	  t2=1.0-t1;
  	} 

  	  t0 = 1.0 - t1 - t2;	
  	  t0 = t0 * 0.5;											//ÁãÊ¸Á¿×÷ÓÃÊ±¼ä
//------------------------ ±È½Ï¼Ä´æÆ÷Öµ¼ÆËã---------------------------------------------------------
  switch(sector)
    {
      	case 1:    temp1=(Uint16) (t0 * SW_NPR);				//SW_NPRÎªPWMÖÜÆÚ¼Ä´æÆ÷µ,Îª¿ª¹ØÖÜÆÚµÄÒ»°ë
           	       temp2=(Uint16)((t0 + t1) * SW_NPR);
               	   temp3=(Uint16)((t0 + t1 + t2) * SW_NPR);
            	   break;
     	case 2:    temp1=(Uint16)((t0 + t2) * SW_NPR);
           	       temp2=(Uint16) (t0 * SW_NPR);
               	   temp3=(Uint16)((t0 + t1 + t2) * SW_NPR);
       	           break;
        case 3:    temp1=(Uint16)((t0 + t1 + t2) * SW_NPR);           
            	   temp2=(Uint16) (t0 * SW_NPR);
               	   temp3=(Uint16)((t0 + t1) * SW_NPR);  
                   break;
    	case 4:    temp1=(Uint16)((t0 + t1 + t2) * SW_NPR);           
                   temp2=(Uint16)((t0 + t2) * SW_NPR);
               	   temp3=(Uint16) (t0 * SW_NPR);  
                   break;
    	case 5:    temp1=(Uint16)((t0 + t1) * SW_NPR);           
           	       temp2=(Uint16)((t0 + t1 + t2) * SW_NPR);
               	   temp3=(Uint16) (t0 * SW_NPR);  
                   break;
       	case 6:    temp1=(Uint16) (t0 * SW_NPR);           
            	   temp2=(Uint16)((t0 + t1 + t2) * SW_NPR);
              	   temp3=(Uint16)((t0 + t2) * SW_NPR);  
               	   break;  
        default:   break;         
   }

   tempmin = 0.5 * 75 * (_MINONTIME + _DEADTIME);   //_MINONTIME¶ÔÓ¦×îÐ¡Âö¿íµÄÊ±¼ä£¬µ¥Î»us
                                           	  		//_DEADTIME¶ÔÓ¦ËÀÇøµÄÊ±¼ä£¬µ¥Î»us
   tempmax = SW_NPR - tempmin;  

   if	  (temp1<tempmin) 	temp1=tempmin;
   else if(temp1>tempmax)	temp1=tempmax;
   if	  (temp2<tempmin) 	temp2=tempmin;
   else if(temp2>tempmax)	temp2=tempmax;
   if	  (temp3<tempmin) 	temp3=tempmin;
   else if(temp3>tempmax)	temp3=tempmax;

   EPwm1Regs.CMPA.half.CMPA = temp1;
   EPwm2Regs.CMPA.half.CMPA = temp2;
   EPwm3Regs.CMPA.half.CMPA = temp3;

}    
//****************************************************************************
//±àºÅ£º
//Ãû³Æ£ºSVPWM_MPR
//¹¦ÄÜ£ºÂö³åµ÷ÖÆÊä³ö
//ÊäÈë£º

//Êä³ö£º
//×¢ÊÍ£º »ú²à±äÁ÷Æ÷SVPWMµ÷ÖÆ
//ÐÞ¸ÄËµÃ÷:
//****************************************************************************
void SVPWM_MPR( float alfa, float beta)
{
  
   float tempmin,tempmax,temp1,temp2,temp3;
   float t0,t1,t2;
   Uint16 sector;

   vdc = AD_OUT_UDC;
//-----------------------SVPWM ÉÈÇøÅÐ¶ÏºÍÊ¸Á¿×÷ÓÃÊ±¼ä¼ÆËã-------------------------------------------	       
   if(beta>=0)
	{
	   if(SQRT3*alfa >= beta) 
	   	{
	      sector=1;
	      t1=SQRT3 * (alfa * SQRT3_2-beta * 0.5)/vdc;   		//SQRT3=sqrt(3)=1.73205081£¬ÔÚºê¶¨ÒåÀïÃæÊµÏÖ,0.8660254=SQRT3/2
	      t2=SQRT3 * beta/vdc;
	   	}
	   else if(SQRT3*alfa <= -beta) 
	   	{
	      sector=3;                                    
	      t1=SQRT3 * beta/vdc;
	      t2=SQRT3 * (-beta * 0.5-alfa * SQRT3_2)/vdc;    		//SQRT3_2=sqrt(3)/2
	   	}
	   else 
    	{
	      sector=2;
	      t1=SQRT3 * (alfa * SQRT3_2 + beta * 0.5)/vdc;
	      t2=SQRT3 * (beta * 0.5-alfa * SQRT3_2)/vdc;
     	}
    }
  else
	{
	   if(SQRT3*alfa >= -beta)
	   	{
		  sector=6;
		  t1=-SQRT3 * beta/vdc;
		  t2=SQRT3 * (beta * 0.5 + alfa * SQRT3_2)/vdc;
	    }
	   else if(SQRT3*alfa <= beta)	
	    {
	      sector=4; 
		  t1=SQRT3 * (beta * 0.5 - alfa * SQRT3_2)/vdc;	   
	      t2=-SQRT3 * beta/vdc;
	   	}
	   else 
	   	{
	      sector=5;
	      t1=SQRT3 * (-alfa * SQRT3_2 - beta * 0.5)/vdc;
	      t2=SQRT3 * (-beta * 0.5+alfa * SQRT3_2)/vdc;
	   	}
	} 

  if(t1+t2>=1.0) 
	{
	   t1=t1/(t1+t2);
	   t2=1.0-t1;
	} 
  t0=1.0-t1-t2; 
  t0 = t0 * 0.5;	    										//ÁãÊ¸Á¿×÷ÓÃÊ±¼ä
//------------------------ ±È½Ï¼Ä´æÆ÷Öµ¼ÆËã---------------------------------------------------------
  switch(sector)
    {
      	case 1:    temp1=(Uint16)(t0 * SW_MPR);
           	       temp2=(Uint16)((t0+t1) * SW_MPR);
               	   temp3=(Uint16)((t0+t1+t2) * SW_MPR);
            	   break;
     	case 2:    temp1=(Uint16)((t0+t2) * SW_MPR);
           	       temp2=(Uint16)(t0 * SW_MPR);
               	   temp3=(Uint16)((t0+t1+t2) * SW_MPR);
       	           break;
        case 3:    temp1=(Uint16)((t0+t1+t2) * SW_MPR);           
            	   temp2=(Uint16)(t0 * SW_MPR);
               	   temp3=(Uint16)((t0+t1) * SW_MPR);  
                   break;
    	case 4:    temp1=(Uint16)((t0+t1+t2) * SW_MPR);           
                   temp2=(Uint16)((t0+t2) * SW_MPR);
               	   temp3=(Uint16)(t0 * SW_MPR);  
                   break;
    	case 5:    temp1=(Uint16)((t0+t1) * SW_MPR);           
           	       temp2=(Uint16)((t0+t1+t2) * SW_MPR);
               	   temp3=(Uint16)(t0 * SW_MPR);  
                   break;
       	case 6:    temp1=(Uint16)(t0 * SW_MPR);           
            	   temp2=(Uint16)((t0+t1+t2) * SW_MPR);
              	   temp3=(Uint16)((t0+t2) * SW_MPR);  
               	   break;  
        default:   break;         
   }

   tempmin = 0.5 * 75 * (_MINONTIME + _DEADTIME);   //_MINONTIME¶ÔÓ¦×îÐ¡Âö¿íµÄÊ±¼ä£¬µ¥Î»us
                                           	  		//_DEADTIME¶ÔÓ¦ËÀÇøµÄÊ±¼ä£¬µ¥Î»us
   tempmax = SW_MPR - tempmin;  

   if	  (temp1<tempmin) 	temp1=tempmin;
   else if(temp1>tempmax)	temp1=tempmax;
   if	  (temp2<tempmin) 	temp2=tempmin;
   else if(temp2>tempmax)	temp2=tempmax;
   if	  (temp3<tempmin) 	temp3=tempmin;
   else if(temp3>tempmax)	temp3=tempmax;

   if(R_PHORDE==1)		//×ª×Ó·´ÏàÐò¿ØÖÆ201005atcpc
   {
 	  EPwm4Regs.CMPA.half.CMPA = temp2;	//×ª×Ó·´ÏàÐò¿ØÖÆ
 	  EPwm5Regs.CMPA.half.CMPA = temp1;
 	  EPwm6Regs.CMPA.half.CMPA = temp3; 
   }
   else
   {
	  EPwm4Regs.CMPA.half.CMPA = temp1;	//×ª×ÓÕýÏàÐò¿ØÖÆ
	  EPwm5Regs.CMPA.half.CMPA = temp2;
 	  EPwm6Regs.CMPA.half.CMPA = temp3;    
   }


}  

/*********************************************************************************************************
** º¯ÊýÃû³Æ: ×ø±ê±ä»»×Óº¯Êý
** ¹¦ÄÜÃèÊö: 3s/2s±ä»»£¬2s/2r±ä»»
** Êä¡¡Èë: 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:20090331
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Transform_3s_2s_2r(struct TRANS_DATA *var1)
{   
    var1->alfa = var1->a;												//²ÉÓÃµÈ·ù±ä»» ualfa=2/3(ua-0.5ub-0.5uc)   
	var1->beta = (var1->b - var1->c) * SQRT3_3;							//ubeta=2/3*sqrt(3)/2 (ub-uc)  SQRT3_3=sqrt(3)/3=0.57735026918962576450914878050196

    var1->d =  var1->alfa * var1->costheta + var1->beta * var1->sintheta;//ud=ualfa*cos(th)+ubeta*sin(th)
    var1->q = -var1->alfa * var1->sintheta + var1->beta * var1->costheta;//uq=-ualfa*sin(th)+ubeta*cos(th)
}  
/*********************************************************************************************************
** º¯ÊýÃû³Æ: ×ø±ê±ä»»×Óº¯Êý
** ¹¦ÄÜÃèÊö: 2r/2s±ä»»£¬2s/3s±ä»»
** Êä¡¡Èë: 
** Êä¡¡³ö: 
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:20090331
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Anti_Transform_2r_2s(struct TRANS_DATA *var2)
{
    var2->alfa = var2->d * var2->costheta - var2->q * var2->sintheta;//ualfa=ud*cos(th)-uq*sin(th)   
	var2->beta = var2->d * var2->sintheta + var2->q * var2->costheta;//*ubeta=ud*sin(th)+uq*cos(th)     			
}

/*********************************************************************************************************
** º¯ÊýÃû³Æ: PI_Loop
** ¹¦ÄÜÃèÊö: PIµ÷÷
** Êä¡¡Èë:   kp ±ÈÀý·Å´óÏµÊý
             ki »ý·ÖÏµÊý
			 outmax Êä³öÏÞ·ù
			 errmax Îó²î×î´óÖµÏÞ·ù
             errmin Îó²î×îÐ¡ÖµÏÞ·ù
             incrementmax Êä³öÔöÁ¿ÏÞ·ù

** Êä¡¡³ö:   
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:20090331
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void PI_Loop(struct PI_DATA *var,struct PI_PARA PI_var)
{
	float temp,ptemp,itemp,detemp,outtemp,du;

//----------------------¼ÆËã±¾´ÎÎó²î---------------------------------------------------------------
	temp = var->reference - var->feedback;  			

//----------------------´ÎÎó²îÕý×î´óÖµÏÞ·ù¡¢×îÐ¡ÖµÏÞ·ù-------------------------------------------
    if		(temp  	 	>  PI_var.errmax)  temp =  PI_var.errmax; 
    else if	(temp  	 	< -PI_var.errmax)  temp = -PI_var.errmax; 
	else 	 temp = temp;

	var->error = temp;													//ÐÞÕýºóÎó²î£¬¿¹¸ÉÈÅ
    detemp = var->error -  var->errorp;    

//----------------------±¾´ÎÔöÁ¿¡¢ÏÞ·ù-------------------------------------------------------------
	ptemp = PI_var.kp *  detemp;										//±ÈÀýÏî
	itemp = PI_var.kp * PI_var.ki *  var->error * 2.0e-4;				//»ý·ÖÏîÔöÁ¿,09.6.17

	du = ptemp + itemp;                                             	//±¾´ÎÔöÁ¿
	if     (du >  PI_var.incrementmax)    du =  PI_var.incrementmax;	//±¾´ÎÔöÁ¿ÏÞ·ù
	else if(du < -PI_var.incrementmax)    du = -PI_var.incrementmax;	//±¾´ÎÔöÁ¿ÏÞ·ù

//----------------------±¾´ÎÊä³ö¡¢ÏÞ·ù------------------------------------------------------------
	outtemp = var->out + du;

	if     (outtemp >  PI_var.outmax)  outtemp =  PI_var.outmax;		//Êä³öÏÞ·ù
	else if(outtemp < -PI_var.outmax)  outtemp = -PI_var.outmax;		//Êä³öÏÞ·ù

//---------------------Êä³ö¸³Öµ¡¢Ë¢ÐÂÉÏ´ÎÎó²îÖµ--------------------------------------------------	
	var->out    = outtemp;		
	var->errorp = var->error;												
} 

//--------20121103--------
/*********************************************************************************************************
** º¯ÊýÃû: PLLCtrl
** ¹¦ÄÜÃèÊö: ¼ÆÊý¿ØÖÆ
** Êä¡¡Èë:
** Êä¡¡³ö       
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void PLLCtrl(void)
{  
	float n,p;
 
//    I_Loop(&PI_DATA_PLL);		//201205LVRTatZB
//    pll_theta=I_DATA_PLL.last;
 
    TRS_GAC_U.a = AD_OUT_NGS_U.a;					//»ñÈ¡µçÍøµçÑ¹
	TRS_GAC_U.b = AD_OUT_NGS_U.b;
	TRS_GAC_U.c = AD_OUT_NGS_U.c;
    
 	TRS_GAC_U.costheta=cos(pll_theta);
    TRS_GAC_U.sintheta=sin(pll_theta);
    p=1;
    n=-1;
	ptheta=(p-n) * pll_theta;
	ntheta=(n-p) * pll_theta;
    TRS_GAC_U.de_pcostheta=cos(ptheta);
    TRS_GAC_U.de_psintheta=sin(ptheta);
    TRS_GAC_U.de_ncostheta=cos(ntheta);	 
    TRS_GAC_U.de_nsintheta=sin(ntheta);

	PTransform_3s_2s_2r(&TRS_GAC_U);					//×ø±ê±ä»»
    NTransform_3s_2s_2r(&TRS_GAC_U);

    De_ONE_P_2r(&TRS_GAC_U);
    De_ONE_N_2r(&TRS_GAC_U);

    PI_PARA_PLL.kp=1;
    PI_PARA_PLL.ki=80;
//    PI_PARA_PLL.kp=_stdby02; 	//dot1 1
//   PI_PARA_PLL.ki=_stdby03;	//dot0 80		
    PI_PARA_PLL.incrementmax=1000;	//201205LVRTatZB
    PI_PARA_PLL.outmax=10000;		//201205LVRTatZB
    PLL_PI_Loop(&PI_DATA_PLL,PI_PARA_PLL,&TRS_GAC_U);

    I_Loop(&PI_DATA_PLL);
    pll_theta=I_DATA_PLL.last;

   	temp_pd=TRS_GAC_U.de_pd;
   	temp_pq=TRS_GAC_U.de_pq;
   	temp_nd=TRS_GAC_U.de_nd;
   	temp_nq=TRS_GAC_U.de_nq;

    DataFilter(0.97,&TRS_GAC_U.de_fltpd,temp_pd);
	DataFilter(0.97,&TRS_GAC_U.de_fltpq,temp_pq);
	DataFilter(0.97,&TRS_GAC_U.de_fltnd,temp_nd);
	DataFilter(0.97,&TRS_GAC_U.de_fltnq,temp_nq);  

	//-------pll-Up-output-----------
	TRS_NGS_U.dpll = TRS_GAC_U.de_fltpd;		//201011PLL
	TRS_NGS_U.qpll = TRS_GAC_U.de_fltpq;		//201011PLL

	NGS_Udq_p = sqrt(TRS_GAC_U.de_fltpd * TRS_GAC_U.de_fltpd + TRS_GAC_U.de_fltpq * TRS_GAC_U.de_fltpq); 
	NGS_Udq_n = sqrt(TRS_GAC_U.de_fltnd * TRS_GAC_U.de_fltnd + TRS_GAC_U.de_fltnq * TRS_GAC_U.de_fltnq); 
	NGS_Udq_epsilon = 100 * NGS_Udq_n / NGS_Udq_p;
} 

/*********************************************************************************************************
** º¯ÊýÃû³Æ: PLL×ø±ê±ä»»×Óº¯Êý
** ¹¦ÄÜÃèÊö: 3s/2s±ä»»£¬2s/2r±ä»»
** Êä¡¡Èë: 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void PTransform_3s_2s_2r(struct TRANS_DATA *var2)
{   
    var2->palfa = var2->a;													//²ÉÓÃµÈ·ù±ä»» ualfa=2/3(ua-0.5ub-0.5uc)   
	var2->pbeta = (var2->b - var2->c) * SQRT3_3;							//ubeta=2/3*sqrt(3)/2 (ub-uc)  SQRT3_3=sqrt(3)/3=0.57735026918962576450914878050196

    var2->pd =  var2->palfa * var2->costheta + var2->pbeta * var2->sintheta;//ud=ualfa*cos(th)+ubeta*sin(th)
    var2->pq = -var2->palfa * var2->sintheta + var2->pbeta * var2->costheta;//uq=-ualfa*sin(th)+ubeta*cos(th)
}  
/*********************************************************************************************************
** º¯ÊýÃû³Æ: PLL×ø±ê±ä»»×Óº¯Êý
** ¹¦ÄÜÃèÊö: 3s/2s±ä»»£¬2s/2r±ä»»
** Êä¡¡Èë: 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void NTransform_3s_2s_2r(struct TRANS_DATA *var3)
{   
    var3->nalfa = var3->a;													//²ÉÓÃµÈ·ù±ä»» ualfa=2/3(ua-0.5ub-0.5uc)   
	var3->nbeta = (var3->b - var3->c) * SQRT3_3;							//ubeta=2/3*sqrt(3)/2 (ub-uc)  SQRT3_3=sqrt(3)/3=0.57735026918962576450914878050196

    var3->nd =  var3->nalfa * var3->costheta - var3->nbeta * var3->sintheta;//ud=ualfa*cos(th)+ubeta*sin(th)
    var3->nq = var3->nalfa * var3->sintheta + var3->nbeta * var3->costheta;//uq=-ualfa*sin(th)+ubeta*cos(th)
} 
/*********************************************************************************************************
** º¯ÊýÃû³Æ:PLLÕý¸ºÐò½âñîº¯Êý
** ¹¦ÄÜÃèÊö: 
** Êä¡¡Èë: 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** : 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void De_ONE_P_2r(struct TRANS_DATA *var4)
{
  var4->de_pd=var4->pd - var4->de_fltnd*var4->de_pcostheta-var4->de_fltnq*var4->de_psintheta;
  var4->de_pq=var4->pq +var4->de_fltnd*var4->de_psintheta- var4->de_fltnq*var4->de_pcostheta;
}

/*********************************************************************************************************
** º¯ÊýÃû³Æ:PLLÕý¸ºÐò½âñîº¯Êý
** ¹¦ÄÜÃèÊö: 
** Êä¡¡Èë: 
** Êä¡¡³ö:   
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void De_ONE_N_2r(struct TRANS_DATA *var5)
{
  var5->de_nd=var5->nd - var5->de_fltpd*var5->de_ncostheta - var5->de_fltpq*var5->de_nsintheta;
  var5->de_nq=var5->nq + var5->de_fltpd*var5->de_nsintheta - var5->de_fltpq*var5->de_ncostheta;
}

/*********************************************************************************************************
** º¯ÊýÃû³Æ: PLL-PI_Loop
** ¹¦ÄÜÃèÊö: PIµ÷½ÚÆ÷
** Êä¡¡Èë:   kp ±ÈÀý·Å´óÏµÊý
             ki »ý·ÖÏµÊý
			 outmax Êä³öÏÞ·ù
			 errmax Îó²î×î´óÖµÏÞ·ù
             errmin Îó²î×îÐ¡ÖµÏÞ·ù
             incrementmax Êä³öÔöÁ¿ÏÞ·ù

** Êä¡¡³ö:   
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void PLL_PI_Loop(struct PI_DATA *var,struct PI_PARA PI_var,struct TRANS_DATA *var1)
{
	float ptemp,itemp,detemp,outtemp,du;
													
    detemp = var1->de_pq - var->last;    

//----------------------±¾´ÎÔöÁ¿¡¢ÏÞ·ù-------------------------------------------------------------
	ptemp = PI_var.kp * detemp  ;										//±ÈÀýÏî
	itemp = PI_var.kp * PI_var.ki * var1->de_pq * 2.0e-4;				//»ý·ÖÏîÔöÁ¿,09.6.17

	du = ptemp + itemp;                                             	//±¾´ÎÔöÁ¿
	if     (du >  PI_var.incrementmax)    du =  PI_var.incrementmax;	//±¾´ÎÔöÁ¿ÏÞ·ù
	else if(du < -PI_var.incrementmax)    du = -PI_var.incrementmax;	//±¾´ÎÔöÁ¿ÏÞ·ù

//----------------------±¾´ÎÊäö¡¢ÏÞ·?-----------------------------------------------------------
	outtemp = var->out + du;

	if     (outtemp >  PI_var.outmax)  outtemp =  PI_var.outmax;		//Êä³öÏÞ·ù
	else if(outtemp < -PI_var.outmax)  outtemp = -PI_var.outmax;		//Êä³öÏÞ·ù

//---------------------Êä³ö¸³Öµ¡¢Ë¢ÐÂÉÏ´ÎÎó²îÖµ--------------------------------------------------	
	var->out    = outtemp;		
	var->last = var1->de_pq;												
} 
/*********************************************************************************************************
** º¯ÊýÃû³Æ: I_Loop
** ¹¦ÄÜÃèÊö: Integratorµ÷½ÚÆ÷
** Êä¡¡Èë:   
** Êä¡¡³ö:   
** ×¢  ÊÍ: 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void I_Loop(struct PI_DATA *var6)
{    
 	float thetanum3,thetanum4;
    
	I_DATA_PLL.out0=var6->out*2.0e-4+I_DATA_PLL.last; 

	if(I_DATA_PLL.out0>TWOPAI)
	{
    	thetanum3=(Uint32)(I_DATA_PLL.out0/TWOPAI);
    	thetanum4=I_DATA_PLL.out0/TWOPAI-thetanum3;	
    	I_DATA_PLL.out1=thetanum4*TWOPAI;  

		I_DATA_PLL.last =I_DATA_PLL.out1;
    } 
	else
	{
		I_DATA_PLL.last =I_DATA_PLL.out0;
	}
}

//--------20121103--------

/***************************************************************************************************
** º¯ÊýÃû³Æ: Magnet_Curve
** ¹¦ÄÜÃèÊö: µç»úÀø´ÅÇúÏßº¯Êý(·Ö¶ÎÏßÐÔ²åÖµ)
** Êä¡¡Èë:   ²åÖµ×Ô±äÁ¿x[n],²åÖµº¯ÊýÖµy[n],¼ÆËãº¯Êýµãin
** Êä¡¡³ö:   Àø´ÅµçÁ÷Ð£ÕýÏµÊýout  
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: MagnetCurve2013-12-13
** ÈÕ¡¡ÆÚ: 2013-12-13
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Magnet_Curve(struct MC_DATA *var)	//·Ö¶ÎÏßÐÔ²åÖµ
{
	float temp_m,temp_k;
	Uint16 i,j,s;

//---------------²åÖµµãÏÞ·ù------------------------------------------------    
	if(var->in < var->x[0])	var->in= var->x[0];
	else if(var->in > var->x[5])	var->in= var->x[5];

//---------------ÅÐ¶Ï²åÖµµãÇø¼ä[0,4]------------------------------------------------    
	if(var->in <= var->x[1])	s=0;
	else if((var->in > var->x[1])&&(var->in <= var->x[2]))	s=1;
	else if((var->in > var->x[2])&&(var->in <= var->x[3]))	s=2;
	else if((var->in > var->x[3])&&(var->in <= var->x[4]))	s=3;
	else if((var->in > var->x[4])&&(var->in <= var->x[5]))	s=4;


	temp_k = 0;
	for(i=s;i<(s+2);i++)
	{
		temp_m = 1;
		for(j=s;j<(s+2);j++)
		{
			if(i!=j)	
			{
				temp_m = temp_m * (var->in - var->x[j]) / (var->x[i] - var->x[j]);
			}
		}
		temp_k = temp_k + temp_m * var->y[i];
	}
	var->k = temp_k;
}  

/***************************************************************************************************
** º¯ÊýÃû³Æ: Give_Integral
** ¹¦ÄÜÃèÊö: ¸ø¶¨»ý·Ö×Óº¯Êý
** Êä¡¡Èë:   »ý·Ö²½³¤step£¬¸ø¶¨Ö¸ÁîÖµgive
** Êä¡¡³ö:   ¸ø¶¨»ý·ÖºóµÄÖ¸ÁîÖµout  
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 20090604
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:20090604
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
float Give_Integral(float give,float step,float out)
{
	if(out < give)
	{
		out += step;
		if(out > give)	 	out = give;
	}
	else if(out > give)
	{
		out -=  step;
		if(out < give)		out = give;
	}
	else 	out = give;
		
	return(out);
}  

/*********************************************************************************************************
** º¯ÊýÃû³Æ: Give
** ¹¦ÄÜÃèÊö: È·¶¨ÄÜ·ñ¹¤×÷ cpcÐÞ¸Ä
** Êä¡¡Èë:   
** Êä¡¡³ö:   
** ×¢  ÊÍ: 	 
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡? 
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:20090801ÐÞ¸Ä
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Give(void)
{	
	float temp_toqrf,temp_aglrf;
    
	if(M_ChkFlag(SL_NPR_START)==0 && M_ChkFlag(SL_MPR_START)==0)
	{
		M_ClrFlag(SL_NPR_RUN);			    						//Íø²à¹¤×÷ÔÊÐí±êÖ¾Çå0
		M_ClrFlag(SL_MPR_RUN);			    						//»ú¹¤×÷Ðí±êÖ¾Çå0
		GIVE.urf      = _URF;										//Íø²à¸ø¶¨µçÑ¹Îªµ±Ç°VdcÖµ
		GIVE.npriqrf  = 0;											//Íø²à¸ø¶¨µçÁ÷Îª0
        GIVE.toqrf    = 0;                  						//»ú²à¸ø¶¨×ª¾ØÎª0
        GIVE.anglerf  = 0;		            						//»ú²à¸ø¶¨¹¦ÂÊÒòÊý½Ç¶ÈÎª0
		GIVE.toqrfflt = 0;		//20121103
	}
	else
	{
//		if(M_ChkFlag(SL_LV_QWORKING)==0) 	NGS_Udq_p_ex = NGS_Udq_p;		//ÅÐ¶ÏLVRTÖ®Ç°ÍøÑ¹ 20130206
		if(M_ChkFlag(SL_LV_QWORKING)==0) 	NGS_Udq_p_ex = NGS_Udq_pflt;		//ÅÐ¶ÏLVRTÖ®Ç°ÍøÑ¹ 20130301

/*		if(M_ChkFlag(SL_OCS_NPREIN)!=0)					           //Íø²àÎÞ¹¦²¢Íø,Íø²à¸ø¶¨Ö±Á÷µçÑ¹ºÍÎÞ¹¦µçÁ÷Öµ
		{
		    M_ClrFlag(SL_MPR_RUN);

			if(M_ChkFlag(SL_NPR_START)!=0)  
			{
				M_SetFlag(SL_NPR_RUN); 								//Íø²à±äÁ÷Æ÷ÔËÐÐ
		    	GIVE.urf      = (int16) _URF;                       //¸øÍø²àÖÐ¼äÖ±Á÷µçÑ¹Ö¸Áî¸³Öµ
		    	GIVE.npriqrf  = (int16) _NIQRF;						//¸ø»ú²àÎÞ¹¦µçÁ÷Ö¸Áî¸³Öµ
			}
	        else  M_ClrFlag(SL_NPR_RUN);
		}	
			
		else 		                       							//±³¿¿±³¹¤×÷Ê±Íø²àÖ±Á÷µçÑ¹,ÎÞ¹¦µçÁ÷¸ø¶¨ºÍ»ú²à×ª¾ØºÍ¹¦ÂÊÒòÊý½Ç¸ø¶¨
		{		
*/			if(M_ChkFlag(SL_NPR_START)!=0)  		   				//Íø²àÔËÐÐÅÐ¶Ï
			{
			   M_SetFlag(SL_NPR_RUN);                  				//Íø²àÂö³åÔÊÐí		       
		       GIVE.urf       = (int16) _URF;          				//¸øÖÐ¼äÖ±Á÷µçÑ¹Ö¸Áî¸³Öµ
			   GIVE.npriqrf   = (int16) _NIQRF;        				//Íø²à¸ø¶¨¸ÐÐÔÎÞ¹¦±£Ö¤²¢Íø¹¦ÂÊÒòÊýÎª1
//			   GIVE.npriqrf   = 70.0;                  				//Íø²à¸ø¶¨¸ÐÐÔÎÞ¹¦±£Ö¤²¢Íø¹¦ÂÊÒòÊýÎª1£¬»¥À¡ÊµÑéÑéÖ¤ÐèÒª²¹70A

/*20121103 ÔÚRuncltÀï¸ø¶¨
			   if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   			//LVRTÍø·¢ÎÞ¹¦¹¦ÂÊ
			   {
					GIVE.npriqrf   = -(int16)_PROSTDBY1;   			//201007BJTULVRT
			   }
*/
			}
		    else     M_ClrFlag(SL_NPR_RUN);


	        if(M_ChkFlag(SL_MPR_START)!=0)  						//»ú²àÔËÐÐÅÐ¶Ï
	        {
	           M_SetFlag(SL_MPR_RUN);                   			//»ú²àÂö³åÔÊÐí
 
               if(M_ChkFlag(SL_ENPCOPER)!=0)	  
               {
					if(M_ChkFlag(SL_IN1_STATORK)!=0)				//¶¨×Ó½Ó´¥Æ÷±ÕºÏÒÔºóÔÙ½ÓÊÕ×ª¾ØºÍ½Ç¶ÈÖ¸Áî
					{ 
               			GIVE.toqrf  =  (int16) _TOQRF;              //ÕýÖµÎª·¢µç
						GIVE.anglerf=  (int16) _AGLRF;  			//¸ø¹¦ÂÊÒòÊý½ÇÖ¸Áî¸³Öµ,-360 -- 360

					    if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   	//LVRT»ú²à·¢ÓÐ¹¦»òÕßÎÞ¹¦
			            {
//					    	GIVE.toqrf  = (int16)_PROSTDBY2;   		//201007BJTULVRT
							GIVE.anglerf= 0;						//20110829
			   			}
					}
					else
      		   		{
      		   			GIVE.toqrf    = 0;                  		//»ú²à¸ø¶¨×ª¾ØÎª0
        				GIVE.anglerf  = 0;		            		//»ú²à¸ø¶¨¹¦ÂÊÒòÊý½Ç¶ÈÎª0
						GIVE.toqrfflt = 0;
     		   		} 
			   }
               else   
               {           
					if(M_ChkFlag(SL_IN1_STATORK)!=0)				//¶¨×Ó½Ó´¥Æ÷±ÕºÏÒÔºóÔÙ½ÓÊÕ×ª¾ØºÍ½Ç¶ÈÖ¸Áî
					{
               			temp_toqrf     =  (int16)SCI_canopen.rx_torque;               //ÕýÖµÎª·¢µç
      		   			GIVE.toqrf     =  temp_toqrf * CAN_TEN / 16383.0;             //»ú²à×ª¾ØÖ¸ÁîÊµ¼ÊÖµ  _TOQRF == -200%-200%

						if(GIVE.toqrf<0)	GIVE.toqrf=0;    //±äÁ÷Æ÷¶ÔÖ÷¿ØµÄ¸º×ª¾ØÖ¸Áî²»ÓèÏìÓ¦ 20090815
						if(GIVE.toqrf>9500)	GIVE.toqrf=9500;    //±äÁ÷Æ÷¶ÔÖ÷¿ØµÄ³¬ÏÞ×ª¾ØÖ¸Áî²»ÓèÏìÓ¦ 20130801

               			temp_aglrf     =  (int16)SCI_canopen.rx_angle;               
      		   			GIVE.anglerf   =  temp_aglrf * CAN_ANGLEN / 16383.0;  

					    if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   	//LVRT»ú²à·¢ÓÐ¹¦»òÕßÎÞ¹¦
			            {
//					    	GIVE.toqrf  = (int16)_PROSTDBY2;   		//201007BJTULVRT
							GIVE.anglerf= 0;						//20110829
			   			}
      		   		}
      		   		else
      		   		{
      		   			GIVE.toqrf    = 0;                  		//»ú²à¸ø¶¨×ª¾ØÎª0
        				GIVE.anglerf  = 0;		            		//»ú²à¸ø¶¨¹¦ÂÊÒòÊý½Ç¶ÈÎª0
						GIVE.toqrfflt = 0;
     		   		}             				
			   
			   }

				if     (GIVE.anglerf > 0)
					GIVE.anglerf = GIVE.anglerf * _stdby02;		//20140503LJDµçÄÜÖÊÁ¿
				else if     (GIVE.anglerf < 0)
					GIVE.anglerf = GIVE.anglerf * 1.1;			//20140503LJDµçÄÜÖÊÁ¿

			   if     (GIVE.anglerf <-30.0)                      GIVE.anglerf     = -30.0;
			   else if(GIVE.anglerf > 30.0)                      GIVE.anglerf     =  30.0;
			   else  											 GIVE.anglerf     = GIVE.anglerf;
                                         
	        }
            else    M_ClrFlag(SL_MPR_RUN);
//		}
	}
}
/*********************************************************************************************************
** º¯ÊýÃû³Æ: RunCtrl
** ¹¦ÄÜÃèÊö: ¹¤×÷¿ØÖÆ  cpcÐÞ¸Ä
** Êä¡¡Èë:   
** Êä¡¡³ö:   
** ×¢  ÊÍ: 	 
**--------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 
**--------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ¡¡ÆÚ:20090721ÐÞ¸Ä
**--------------------------------------------------------------------------------------------------
***********************************************************************************************/
void RunCtrl(void)
{
    float udc_max,id_max,iq_max,toq_max,agl_max,temp,temp_var;//2013-12-9ZZJÔö¼Ótemp_var
	Uint16 temp_n,temp_ud,temp_iqexi,temp_number;
//	float  temp_exi,temp_iqk;
//	float  temp_Qc,temp_Qg,temp_tan;
	float  temp_exi,temp_iqk,temp_npriqrf,temp_idrfmax,temp_iqrfmax;

//----------------------------ø²àÔËÐÐ¸ø¶¨¿ØÖÆ---------------------------------------------------------

//-----Íø²àÕýÔÚÔËÐÐÇÒÒªÇóÕý³£Í£»ú
    if((M_ChkFlag(SL_NPR_RUNING)!=0)&&(M_ChkFlag(SL_NPR_RUN)==0))  				//Íø²àÎÞ¹¦µçÁ÷½«Îª0ºóÔÙÍ£Âö³å
    {
		RUN.npriqrf = Give_Integral(0,RUN.npriqstep,RUN.npriqrf);  				//Ö¸Áî¼õÐ¡Ö±ÖÁµÈÓÚ0
            
        if(RUN.npriqrf==0)   
        {   
        	if(M_ChkCounter(MAIN_LOOP.cnt_isteady0,DELAY_ISTEADY0)>0)  M_ClrFlag(SL_NPR_RUNING);
		}
		else  MAIN_LOOP.cnt_isteady0= 0;
    }

//-----ÒªÇóÍø²àÔËÐÐ
    else if(M_ChkFlag(SL_NPR_RUN)!= 0) 
    {  
       M_SetFlag(SL_NPR_RUNING);                 								//Íø²àÕýËÐÐ£¬¿ªÊ¼·¢Íø²àÂö³å
       MAIN_LOOP.cnt_isteady0= 0;
	                
	   RUN.urf = Give_Integral(GIVE.urf,RUN.ustep,RUN.urf);     				//µçÑ¹¸ø¶¨»ý·Ö

//       if(M_ChkFlag(SL_LV_NSTOP)!=0)	RUN.urf=AD_OUT_UDC; 					//20121103							

       if(RUN.urf == GIVE.urf)	    M_SetFlag(SL_STEADYGV);        				//¸ø¶¨»ý·ÖÍê³É,ÖÃÎÈ¶¨±êÖ¾Î»
	   else							M_ClrFlag(SL_STEADYGV);

//-------Íø²àÎÞ¹¦µçÁ÷¿ØÖÆBJTULVRT20121103--------
		if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   			//LVRTÍø·¢ÎÞ¹¦¹¦ÂÊ
		{				
		 if(M_ChkCounter(MAIN_LOOP.cnt_lv_nprqworking,20)>=0)		//20130225
		 {
			temp_npriqrf   = -(int16)_PROSTDBY1;   			//Íø²àÎÞ¹¦Ö¸Áî¼ÆËã20111103
			if((_COMMAND2&0x0400)!=0)	//±¸ÓÃ10 20111106
			{
				if(M_ChkFlag(SL_LV_STATE)==0)
					temp_npriqrf   = -450; 	//1100V¶î¶¨ÍøÑ¹ÏÂÍø²àÎÞ¹¦µçÁ÷·åÖµÐëÐ¡ÓÚ460A
			}

			GIVE.npriqrf_cl   = -GIVE.lvwtiqrf - DIP_STA_I.qflt;   		//Itq=100A Isq201205LVRTatZB

//			if((_COMMAND2&0x0200)!=0)	//±¸ÓÃ9 201011BJTULVRT
//				GIVE.npriqrf   =  GIVE.npriqrf_cl;   		//Itq=100A Isq
//			else	
				GIVE.npriqrf = - kq * 400; //90kW-DFIGÍø²à30kW¶î¶¨µçÁ÷·åÖµ65A 1.5MWÊÇkq*296A ¼Ó´óµ½400A 20120601night

			if(GIVE.npriqrf < temp_npriqrf)	GIVE.npriqrf=temp_npriqrf;
			else if(GIVE.npriqrf > 0)		GIVE.npriqrf=0;
//--------20130124----Íø²à·¢ÎÞ¹¦Ìî²¹µøÂä³õÆÚÎÞ¹¦µÄ²»×ã----
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_npriqrf,120)<=0)
			{
//				GIVE.npriqrf = - kq * lv_ctrl5 * 400;
				if((NGS_Udq_epsilon < 30) && (NGS_Udq_p < (0.43 * NGS_Udq_p_ex)))		//Ö»3ph-20¼°35Çé¿öÏÂ²Å·¢	20130308
					GIVE.npriqrf = - kq * lv_ctrl5 * 400;	//20130308
			}
//--------20130124--------
//--------²»¶Ô³ÆµøÂäÍø²àÎÞ¹¦¿ØÖÆ20130221--------
//			if((NGS_Udq_epsilon > 27) && (PRO.Pgactive_lv>600000) && (NGS_Udq_p < (0.71 * NGS_Udq_p_ex)) && (M_ChkFlag(SL_LV_SCRRUNING)!=0))
//			if((NGS_Udq_epsilon > 27) && (NGS_Udq_p < (0.71 * NGS_Udq_p_ex)) && (M_ChkFlag(SL_LV_SCRRUNING)!=0))	//20%/35%ËùÓÐ¹¤¿ö	20130222
			if((NGS_Udq_epsilon > 38) && (PRO.Pgactive_lv>600000) && (NGS_Udq_p < (0.61 * NGS_Udq_p_ex)) && (M_ChkFlag(SL_LV_SCRRUNING)!=0))	//20%/35%ËùÓÐ¹¤¿ö	20130306
			{
				GIVE.npriqrf = - 10 * lv_ctrl1;		//³õÖµÉè¶¨Îª600A
			}
			else if(NGS_Udq_epsilon > 15)
			{
				GIVE.npriqrf = - _stdby03;			//³õÖµÉè¶¨Îª300A
//				GIVE.npriqrf = - 300;			//³õÖµÉè¶¨Îª300A
			}
//----------------------------------------------
//			if(M_ChkFlag(SL_LV_STRICTLV)==0 && (_STDBY9&0x0040 == 0))		//116.6	20130301
//				GIVE.npriqrf = - kq_lv * _stdby08;	//ÍøÑ¹»Ö¸´ºóÍø²àÌî²¹ÎÞ¹¦400A 20130228

			if(GIVE.npriqrf < -850)	GIVE.npriqrf = -850;
			else if(GIVE.npriqrf >0)		GIVE.npriqrf=0;
		 }
		}
		else		//20130124
		{
			MAIN_LOOP.cnt_lv_npriqrf = 0;
			MAIN_LOOP.cnt_lv_nprqworking = 0;		//20130124Íø²à·¢ÎÞ¹¦±Ü¹ý20ms
		}


 	   if(M_ChkFlag(SL_STEADYGV)!=0)											//´ýµçÑ¹¸ø¶¨Íê³ÉºóÔÙ¸ø¶¨ÎÞ¹¦µçÁ÷
	 	   RUN.npriqrf = Give_Integral(GIVE.npriqrf,RUN.npriqstep,RUN.npriqrf); //Íø²à¹¦µçÁ÷¸ø¶¨»ý·Ö	         

//	   if(M_ChkFlag(SL_LV_NSTOP)!=0)  	RUN.npriqrf=0; 							//201007BJTULVRT							
	   if((M_ChkFlag(SL_LV_NSTOP)!=0) || (M_ChkFlag(SL_LV_STATE)!=0 && M_ChkFlag(SL_LV_QWORKING)==0))  
	   		RUN.npriqrf=0;		//20121103		Íø²àÁÙÊ±·âÂö³åÊ±½«ÎÞ¹¦Ö¸ÁîÇåÁã					

//	   if(M_ChkFlag(SL_UNBALANCE)!=0 && (NGS_Udq_epsilon > 35))	//20121103
//	   		RUN.npriqrf=0; //²»¶Ô³ÆµøÂäºÜ´óÊ±·âÂö³åÊ±½«ÎÞ¹¦Ö¸ÁîÇåÁã					
//	   		RUN.npriqrf = - (int16)_PROSTDBY1 * lv_ctrl1;		//²»¶Ô³ÆµøÂä·¢ÉÙÁ¿ÎÞ¹¦ÒÔµÖÏûµçÍøÎÞ¹¦ÎüÊÕ20121210					
	}
//-----´ý»ú    
    else									//´ý»ú
    {
	   M_ClrFlag(SL_NPR_RUNING);
	   M_ClrFlag(SL_STEADYGV);
	   MAIN_LOOP.cnt_isteady0= 0;

	   RUN.urf   = AD_OUT_UDC;													//¸ø¶¨»ý·Ö³õÊ¼ÖµÎªµ±Ç°VdcÖµ
       RUN.npriqrf  = 0;														//¸ø¶¨»ý·Ö³õÊ¼ÖµÎª0
        
       udc_max    =  (float)FUNC[NO_URF].max;									//¶ÁÈ¡×î´óÖµ
       iq_max     =  (float)FUNC[NO_NIQRF].max;
       temp       =  (float)_RU_UDCT;
	   RUN.ustep   = abs(udc_max)/temp;											//¼ÆËãÖ±Á÷µçÑ¹¸ø¶¨²½³¤
	   temp       =  (float)_RU_IQT;
	   RUN.npriqstep  = abs(iq_max)/temp;										//¼ÆËãÎÞ¹¦µçÁ÷¸ø¶¨²½³¤

	   PI_NPR_U.reference = 0;
	   PI_NPR_U.feedback = 0;
	   PI_NPR_U.out = 0;
	   PI_NPR_U.integrator = 0;
	   PI_NPR_Id.reference = 0;
	   PI_NPR_Id.feedback = 0;
	   PI_NPR_Id.out = 0;
	   PI_NPR_Id.integrator = 0;
	   PI_NPR_Iq.reference = 0;
	   PI_NPR_Iq.feedback = 0;
	   PI_NPR_Iq.out = 0;
	   PI_NPR_Iq.integrator = 0;
    }

//----------------------------»ú²àÔËÐÐÓÐ¹¦µçÁ÷Öµ¸ø¶¨»ý·Ö-------------------------------------

//-----»ú²àÕýÔÚÔËÐÐ²¢ÍøÒªÇóÍ£»ú
    if((M_ChkFlag(SL_MPR_RUNING)!=0)&&(M_ChkFlag(SL_MPR_RUN)==0)) 
    {      
		if(M_ChkFlag(SL_MPR_SYNOK)!=0)												//systest201005atcpc
		{
		    RUN.toqrf = Give_Integral(0,RUN.toqstep,RUN.toqrf);  					//»ú²à×ª¾Ø¸ø¶¨»ý·Ö
		    RUN.aglrf = Give_Integral(0,RUN.aglstep,RUN.aglrf);  			    	//»ú²à¹¦ÂÊÒòÊý½Ç¸ø¶¨»ý·Ö


//		    RUN.mpriqrf_g  =  - RUN.toqrf  / (IRQTOTE * TRS_NGS_U.dflt);   			//¸ø¶¨qÖáµçÁ÷Ö¸Áî¼ÆËã
	    	RUN.mpriqrf_g  =  - RUN.toqrf * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * TRS_NGS_U.dflt / (314.15926 *  MPR_Ls));   //¸ø¶¨qÖáµçÁ÷Ö¸Áî¼ÆËã201005atcpc
//----MagnetCurve2013-12-13--ZZJ--ÒÔÏÂÁ½¾äÈ¡Ïû£¬Ã¿´ÎÍ£»ú°´ÕÕÔËÐÐÊ±idÖ¸ÁîÖµ MagnetCurve200909- 
//			RUN_mpridrf_exi  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));  //¸ººÅ:ÍøÑ¹edÓë»ú²àdÖáÏà²î180¶È             
//          RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          						//³Ëµ÷ÕûÏµÊý,ÓÉ²Ù×÷Æ÷¸ø¶¨ 20100622
/*--MagnetCurve2013-12-13---ZZJ
        	temp_ud = (Uint16)(TRS_NGS_U.dflt * SQRT3 / SQRT2);
        	if     (temp_ud < 540) 		temp_ud = 540;
        	else if(temp_ud > 800) 	    temp_ud = 800;
	    	temp_n = (temp_ud - 540) * 127 / 260;
        	temp_exi = TAB_EXI[temp_n];
        	RUN.mpridrf_exi =  RUN.mpridrf_exi * temp_exi;          				//²é±íµ÷ÕûÏµÊý,Óë¿ÕÔØ¸ÐÓ¦µçÊÆÓÐ¹Ø
//      	    RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          					//³Ëµ÷ÕûÏµÊý,ÓÉ²Ù×÷Æ÷¸ø¶¨		20100622		
///20091022atzy
			RUN_mpriq = (Uint16)(- RUN.mpriqrf_g);
			if     (RUN_mpriq < 290)   temp_iqexi = 290;
			else if(RUN_mpriq > 600)   temp_iqexi = 600;
			temp_iqexi = RUN_mpriq;  //20091019atzy
			temp_number = (temp_iqexi - 290) * 127 / 310;
			temp_iqk = TAB_IQEXI[temp_number];
        	RUN.mpridrf_exi = RUN.mpridrf_exi * temp_iqk;          				//²é±íµ÷ÕûÏµÊý,ÓÉ¸ºÔØµçÁ÷Irq¸ø¶¨È·¶¨ 
*/ //20091022atzy
	 	    RUN.radianrf    = RUN.aglrf * 0.017453292;							//½Ç¶È×ª»¡¶È360->2PAI	 0.01745329=2 * PAI / 360
			   	    
//		    temp_Qc = TRS_NGS_U.dflt * 314.15926 * 501.0e-6 / SQRT2;
//		    temp_Qg = PRO.Pgactive * sin(RUN.radianrf) / cos(RUN.radianrf);
//			temp_tan    = (temp_Qg - temp_Qc) / PRO.Psactive;
//		    RUN.mpridrf_var = RUN.mpriqrf_g * temp_tan;

	    	RUN.mpridrf_var = RUN.mpriqrf_g * sin(RUN.radianrf) / cos(RUN.radianrf); //Í£»úÊ±£¬±£Ö¤¶¨×ÓµçÁ÷Îª0£¬¼´¹¦ÂÊÒòÊýÎª1
        
 	        RUN.mpridrf_g   = RUN.mpridrf_exi + RUN.mpridrf_var;                                           

			RUN.mpridrf = Give_Integral(RUN.mpridrf_g,RUN.mpridstep,RUN.mpridrf); //»ú²à²¢ÍøÇ°dÖá¸ø¶¨¾ÍÎª-90A
			RUN.mpriqrf = Give_Integral(RUN.mpriqrf_g,RUN.mpriqstep,RUN.mpriqrf); //Ö¸Áî¼õÐ¡Ö±ÖÁµÈÓÚ0	  
		    if(RUN.mpriqrf>50)        M_SetFlag(SL_STACKEEPON);					  //¾ø¶ÔµÄ=0ÊÇÐ¿ÉÄÜ´ï²»µ½µÄsystest20100510
			else    			      M_ClrFlag(SL_STACKEEPON);
		    MAIN_LOOP.cnt_mprsteady0= 0; 		
		}
		else
		{
			RUN.mpriqrf = Give_Integral(0,RUN.mpriqstep,RUN.mpriqrf);  			   	//Ö¸Áî¼õÐ¡Ö±ÖÁµÈÓÚ0
			RUN.mpridrf = Give_Integral(0,RUN.mpridstep,RUN.mpridrf);  				//Ö¸Áî¼õÐ¡Ö±ÖÁµÈÓÚ0
		    if((RUN.mpridrf==0)&&(RUN.mpriqrf==0))  								//»ú²àÓÐ¹¦ºÍÎÞ¹¦µçÁ÷¸ø¶¨¶¼½µÁã 
		    { 
		        if(M_ChkCounter(MAIN_LOOP.cnt_mprsteady0,DELAY_MPRSTEADY0)>0)   M_ClrFlag(SL_MPR_RUNING); //0.05s
		    }
		    else   MAIN_LOOP.cnt_mprsteady0= 0; 		
		}
		
    }
   	 	   	
//-----»ú²àÒªÇóÔËÐÐÇÒVdcÎÈ¶¨
//    else if((M_ChkFlag(SL_MPR_RUN)!=0) && (M_ChkFlag(SL_STEADYFB)!=0))  
    else if(M_ChkFlag(SL_MPR_RUN)!=0)  //2010-4-27atBJTU for new Sysctrl systest
    {  
        M_SetFlag(SL_MPR_RUNING);
        M_SetFlag(SL_STACKEEPON);                 									//»ú²àÕýÔÚÔËÐÐ£¬¿ªÊ¼·¢»ú²àÂö³å
//        M_ClrFlag(SL_STACTRIPEN);                 								//systest 201005atcpc
        MAIN_LOOP.cnt_mprsteady0= 0;
//----20121103----
		if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   	//LVRT»ú²à·¢ÓÐ¹¦»òÕßÎÞ¹¦
		{
//			GIVE.toqrf  = GRD_Ut * GIVE.toqrf;   	//201205LVRTatZB
			GIVE.anglerf= 0;						//20110829
			RUN.toqrf = GIVE.toqrf;					//20130228
//			if(M_ChkFlag(SL_UNBALANCE)!=0)
//				GIVE.toqrfrun  = 0;   					//20121013LVRTatBJTU ²»¶Ô³ÆµøÂäÏÂ×ª¾Ø²»±£³Ö
		}
		else
		{
//			DataFilter(0.994,&GIVE.toqrfflt,GIVE.toqrf); //Ö÷¿Ø¸ø¶¨×ª¾ØÆ½¾ùÖµ1s Ã¿1msÖ´ÐÐÒ»´Î201205LVRTatZB
 			GIVE.toqrfrun = GIVE.toqrf;
 		}	   
//----------------
		RUN.toqrf = Give_Integral(GIVE.toqrf,RUN.toqstep,RUN.toqrf);  				//»ú²à×ª¾Ø¸ø¶¨»ý·Ö
		RUN.aglrf = Give_Integral(GIVE.anglerf,RUN.aglstep,RUN.aglrf);  			//»ú²à¹¦ÂÊÒòÊý½Ç¸ø¶¨»ý·Ö
//-----20121103-------
		if(M_ChkFlag(SL_LV_MSTOP)!=0 || (M_ChkFlag(SL_LV_STATE)!=0 && M_ChkFlag(SL_UNBALANCE)!=0))												//201007BJTULVRT
		{				
			RUN.toqrf = 0;  		//201205LVRTatZB ²»Æ½ºâÏÂµÄ×ª¾ØÔËÐÐÖ¸ÁîÒªÇåÁã		
			RUN.aglrf = 0;  			
		}
//-------------------
 
//	    RUN.mpriqrf_g  =  - RUN.toqrf  / (IRQTOTE * TRS_NGS_U.dflt);   				//¸ø¶¨qÖáµçÁ÷Ö¸Áî¼ÆËã
	    RUN.mpriqrf_g  =  - RUN.toqrf * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * TRS_NGS_U.dflt / (314.15926 *  MPR_Ls));   //¸ø¶¨qÖáµçÁ÷Ö¸Áî¼ÆËã201005atcpc
/*---MagnetCurve2013-12-13---ZZJ
		RUN_mpridrf_exi  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));  //¸ººÅ:ÍøÑ¹edÓë»ú²àdÖáÏà²î180¶È             
        RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          						//³Ëµ÷ÕûÏµÊý,ÓÉ²Ù×÷Æ÷¸ø¶¨ 20100622
        temp_ud = (Uint16)(TRS_NGS_U.dflt * SQRT3 / SQRT2);							//ÍøÑ¹ÏßµçÑ¹ÓÐÐ§Öµ 690V
        if     (temp_ud < 540) 		temp_ud = 540;
        else if(temp_ud > 800) 	    temp_ud = 800;
	    temp_n = (temp_ud - 540) * 127 / 260;
        temp_exi = TAB_EXI[temp_n];
        RUN.mpridrf_exi =  RUN.mpridrf_exi * temp_exi;          					//²é±íµ÷ÕûÏµÊý,ÓÉ²Ù×÷Æ÷¸ø¶¨ 
//      	RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          					//³Ëµ÷ÕûÏµÊý,ÓÉ²Ù×÷Æ÷¸ø¶¨	20100622			
*///---MagnetCurve2013-12-13---ZZJ
/*//20091022atzy 
		RUN_mpriq = (Uint16)(- RUN.mpriqrf_g);
		if     (RUN_mpriq < 290)   temp_iqexi = 290;
		else if(RUN_mpriq > 600)   temp_iqexi = 600;
        temp_iqexi = RUN_mpriq;  //20091019atzy
		temp_number = (temp_iqexi - 290) * 127 / 310;
		temp_iqk = TAB_IQEXI[temp_number];
        RUN.mpridrf_exi = RUN.mpridrf_exi * temp_iqk;          						//²é±íµ÷ÕûÏµÊý,ÓÉ¸ºÔØµçÁ÷Irq¸ø¶¨È·¶¨ 
*///20091022atzy 
//----MagnetCurve2013-12-13 ZZJÔö¼ÓÀø´Åµ÷ÕûÄÚÈÝ---
//----MagnetCurve200909 test_start-----------------------

   		if(_SC_MSTDBY==0)
   		{
			RUN_mpridrf_exi  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));
        	RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;

 			if(_MC_OK==1)				//µç»úÀø´ÅÇúÏß»æÖÆÍê³É
			{								
 				MC_DATA.in = 100 * TRS_NGS_U.dflt * SQRT3 / ((float)_SC_Usn * SQRT2);
				if(MC_DATA.in < 50)			MC_DATA.in = 50;		//ÊäÈëÏÞ·ù[50,110]
				else if(MC_DATA.in > 110)	MC_DATA.in = 110;		//ÊäÈëÏÞ·ù
 				Magnet_Curve(&MC_DATA);
        		RUN.mpridrf_exi =  RUN.mpridrf_exi * MC_DATA.k;
        	}
		}
		else
		{
			temp_exi  = - ((float)_SC_Usn * SQRT2 * STAROTRTO / (MPR_Lm * 314.15926));  //¼ÆËãÀíÂÛ¶î¶¨ÍøÑ¹ed=563Ê±»ú²àdÖáÀø´ÅµçÁ÷Öµ            
        	if(_SC_MSTDBY==1)		RUN_mpridrf_exi = 0.5  * _SC_EXISTY1 * 0.001 * temp_exi;
			else if(_SC_MSTDBY==2)	RUN_mpridrf_exi = 0.75 * _SC_EXISTY2 * 0.001 * temp_exi;
			else if(_SC_MSTDBY==3)	RUN_mpridrf_exi = 0.9  * _SC_EXISTY3 * 0.001 * temp_exi;
			else if(_SC_MSTDBY==4)	RUN_mpridrf_exi = 1.0  * _SC_EXISTY4 * 0.001 * temp_exi;
			else if(_SC_MSTDBY==5)	RUN_mpridrf_exi = 1.1  * _SC_EXISTY5 * 0.001 * temp_exi;
//--------------------Ð£Ñé»·½Ú------------------------------------------------------------
			else if(_SC_MSTDBY==99)
			{
				MC_DATA.in = 50 + MC_N_test * 10;	//[MC_test_n , Us]=[0;1;6 , 50;10;110]
				Magnet_Curve(&MC_DATA);
        		RUN_mpridrf_exi =  (0.5 + MC_N_test * 0.1) * temp_exi * MC_DATA.k;
			}

//----------------------------------------------------------------------------------------
			RUN.mpridrf_exi =  RUN_mpridrf_exi;     
		}		
//----MagnetCurve200909 test_end-----------------------


//----MagnetCurve2013-12-13 ZZJÔö¼ÓÀø´Åµ÷ÕûÄÚÈÝ---end


 	    RUN.radianrf    = RUN.aglrf * 0.017453292;									//½Ç¶È×ª»¡¶È360->2PAI	 0.01745329=2 * PAI / 360
	    RUN.mpridrf_var = RUN.mpriqrf_g * sin(RUN.radianrf) / cos(RUN.radianrf); //Í£»úÊ±£¬±£Ö¤¶¨×ÓµçÁ÷Îª0£¬¼´¹¦ÂÊÒòÊýÎª1
			   	    																//RUN.aglrf>0,×ª×ÓÀø´ÅÔö¼Ó,·¢³ö¸ÐÐÔÎÞ¹¦;·´Ö®,ÈÝÐÔ
//	    temp_Qc = TRS_NGS_U.dflt * 314.15926 * 501.0e-6 / SQRT2;					//ÔËÐÐÊ±£¬ÒÑ×Ü²¢Íø¹¦ÂÊÒòÊý1ÎªÄ¿±ê
//	    temp_Qg = PRO.Pgactive * sin(RUN.radianrf) / cos(RUN.radianrf);
//		temp_tan    = (temp_Qg - temp_Qc) / PRO.Psactive;
//	    RUN.mpridrf_var = RUN.mpriqrf_g * temp_tan;
//----MagnetCurve2013-12-13 ZZJÔö¼ÓÀø´Åµ÷ÕûÄÚÈÝ---
		if(_MC_OK==1)				//µç»úÀø´ÅÇúÏß»æÖÆÍê³ÉBJTULVRT201204
		{
	 		RUN.mpridrf_var = 	RUN.mpridrf_var	* MC_DATA.k;
		}
//----MagnetCurve2013-12-13 ZZJÔö¼ÓÀø´Åµ÷ÕûÄÚÈÝ---end

		temp_var = RUN.mpridrf_var;//2013-12-9ZZJÐÞ¸ÄÎÞ¹¦Ö¸ÁîÏìÓ¦ÎÊÌâ
//----20121103-----
//-------»ú²àÎÞ¹¦µçÁ÷¿ØÖÆBJTULVRT201204-------·ûºÅ20121129-----------
//		if(M_ChkFlag(SL_LV_STATE)!=0)  		   			//!!!!!!20120601night 201205LVRTatZB
		if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   			//LVRT»ú²à·¢ÎÞ¹¦µçÁ÷ 20120603night¸Ä»ØÑÓÊ±ÏìÓ¦Ö÷¿Ø×ª¾ØÖ¸Áî
		{
//			RUN.mpriqrf_g  = - GIVE.toqrfrun * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * GRD_UN / (314.15926 *  MPR_Ls));   //201205LVRTatZB
			RUN.mpriqrf_g  = - GIVE.toqrfrun * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * NGS_Udq_p_ex / (314.15926 *  MPR_Ls));   //20130301LVRTatZB
//×¢£º±£³ÖµøÂäÇ°×ª¾Ø²»±ä£¬ÈÏÎªÍøÑ¹¶î¶¨£¬ÔòµøÂäÆÚ¼äÓÐ¹¦µçÁ÷Î¬³Ö²»±ä£¬±ÜÃâIrq*Í»Ôö//20120531
			
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_utlv,DELAY_LVUTLV)<=0)	//500ms
//				GRD_Utlv = GRD_Ut;
			{
				GRD_Utlv = GRD_Ut;
				NGS_Udq_p_lv = NGS_Udq_p; 		//20130124
				kq_lv = kq;				//Íø²à¿ØÖÆÓÃ 20130124
				NGS_Udq_epsilon_lv = NGS_Udq_epsilon;	//20130301
//				Te_feedback_lv=Te_feedback; 	//20130124
//--------·ÖÇé¿ö·µ»ØÖ÷¿Ø×ª¾ØÖµ20130221--------
//				if(PRO.Pgactive_lv<600000)
				if((PRO.Pgactive_lv<600000) || (NGS_Udq_p > (NGS_Udq_p_ex * 0.83)))		//¼ÓÈë90%ºÍÁ½Ïà75%µÄÇé¿ö	20130303
					Te_feedback_lv = GIVE.toqrf_lv;		//·´À¡Ö÷¿Ø×ª¾ØÎ¬³Ö20130128
				else
					Te_feedback_lv = GIVE.toqrf_lv * GRD_Utlv;
//--------·ÖÇé¿ö·µ»ØÖ÷¿Ø×ª¾ØÖµ20130221--------
			}
			else
			{
//				RUN.mpriqrf_g  = - (GRD_Utlv/GRD_Ut) * GIVE.toqrfrun * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * GRD_UN / (314.15926 *  MPR_Ls));   //201205LVRTatZB
				RUN.mpriqrf_g  = - (GRD_Utlv/GRD_Ut) * GIVE.toqrfrun * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * NGS_Udq_p_ex / (314.15926 *  MPR_Ls));   //20130301LVRTatZB
													//20120531
//				if(PRO.Pgactive_lv<600000)		RUN.mpriqrf_g = RUN.mpriqrf_g_exlv;
				if((PRO.Pgactive_lv<600000) || (NGS_Udq_p_lv > (NGS_Udq_p_ex * 0.83)))	//¼ÓÈë90%ºÍÁ½Ïà75%µÄÇé¿ö	20130303
					RUN.mpriqrf_g = RUN.mpriqrf_g_exlv;
			}
//--------90%Î¬³Ö¹¦ÂÊ²»±ä20130205--------2013-12-6 È¡ÏûÁ½Ïà75%ºÍ90%ÓÐ¹¦±£³Ö¹¦ÄÜ
/*			if(M_ChkCounter(MAIN_LOOP.cnt_lv_holdpow,10)>=0)
			{
//				if((NGS_Udq_p>(NGS_Udq_p_ex * 0.85)) && M_ChkFlag(SL_LV_STRICTLV)!=0)		//20130222
//				if((NGS_Udq_p>(NGS_Udq_p_ex * 0.85)) && (NGS_Udq_epsilon<9) && M_ChkFlag(SL_LV_STRICTLV)!=0)		//2ph-75ÅÅ³ýÔÚÍâ 20130309
//					RUN.mpriqrf_g = RUN.mpriqrf_g * 1.14;

				if((_STDBY9&0x0008)==0 && (PRO.Pgactive_lv>600000) && (NGS_Udq_p>(NGS_Udq_p_ex * 0.83)) && (NGS_Udq_epsilon>8) && M_ChkFlag(SL_LV_STRICTLV)!=0)  //116.3	20130314
					RUN.mpriqrf_g = - 400;		//2ph-75´ó·çÓÐ¹¦¿ØÖÆ	20130314
				else if((NGS_Udq_p>(NGS_Udq_p_ex * 0.85)) && (NGS_Udq_epsilon<9) && M_ChkFlag(SL_LV_STRICTLV)!=0)		//2ph-75ÅÅ³ýÔÚÍâ 20130309
//					RUN.mpriqrf_g = RUN.mpriqrf_g * 1.14;
					RUN.mpriqrf_g = RUN.mpriqrf_g * 1.02;
			}
*/
//--------20130205--------

			if(M_ChkCounter(MAIN_LOOP.cnt_lv_mpridvar,DELAY_LVMIDVAR)>=0)	//ÑÓÊ±¸ø¶¨ÎÞ¹¦µçÁ÷201201
			{				
				if(M_ChkFlag(SL_LV_SYNCON)==0)	//201201
				{
//					RUN.mpridrf_var	= - kq * 605.0;				//20111201 Ir_varn=116A 1.5MWÊÇIrn=kq*505A
					RUN.mpridrf_var	= - kq * 645.0;				//20111201 Ir_varn=116A 1.5MWÊÇIrn=kq*505A	20130309
//					RUN.mpridrf_var	= - kq * 116.0;				//20111201 Ir_varn=116A
//					temp_idrfmax = -(int16)_PROSTDBY2;
//					if(RUN.mpridrf_var < temp_idrfmax)	RUN.mpridrf_var=temp_idrfmax;	//»ú²àÎÞ¹¦Ö¸ÁîÏÞ·ù
//					RUN.mpridrf_var  = -(int16)_PROSTDBY2;   		//·¢ÓÐ¹¦201011BJTULVRT

//				    RUN.mpriqrf_g  = (GRD_Utlv/GRD_Ut) * RUN.toqrf * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * GRD_UN / (314.15926 *  MPR_Ls));   //201205LVRTatZB
				}
				else	RUN.mpridrf_var = 0;
			}

			temp_idrfmax = - (int16)_PROSTDBY2;			//ÎÞ¹¦µçÁ÷ÏÞ·ù
			if(RUN.mpridrf_var < temp_idrfmax)	RUN.mpridrf_var=temp_idrfmax;	//»ú²àÎÞ¹¦Ö¸ÁîÏÞ·ù

//--------20130130----»ú²à·¢ÎÞ¹¦Ìî²¹µøÂä³õÆÚÎÞ¹¦µÄ²»×ã----
/*			if(M_ChkCounter(MAIN_LOOP.cnt_lv_mpridrf,100)<=0)
			{
				RUN.mpridrf_var = - kq * lv_ctrl4;
			}
			if(RUN.mpridrf_var < -900)	RUN.mpridrf_var = -900;	//»ú²àÎÞ¹¦Ö¸ÁîÏÞ·ù
*///--------20130130--------
	
//			temp_iqrfmax = sqrt(302500.0 - ((RUN.mpridrf_exi + RUN.mpridrf_var) * (RUN.mpridrf_exi + RUN.mpridrf_var)));//Irmax=550A	
//			temp_iqrfmax = 	- _stdby03; 					//BJTULVRT201204ÓÐ¹¦µçÁ÷ÏÞ·ù ¸øÕýÊý

//--------20121202--------
//			if(M_ChkFlag(SL_LV_SYNCON)!=0)
			if(M_ChkFlag(SL_LV_SYNCON)!=0 && M_ChkFlag(SL_LV_SCRRUNING)!=0)		//20130202
			{
				RUN.mpriqrf_g = 0;
				MAIN_LOOP.cnt_lv_irqdelay=0;
			}
//			else
			else if(M_ChkFlag(SL_LV_STRICTLV)!=0)				//ÍøÑ¹»Ö¸´ºó²»ÑÓ³Ù·¢ÓÐ¹¦ 20130228
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_lv_irqdelay,60)<=0)		RUN.mpriqrf_g = 0;
			}
//--------20121202--------

			if(RUN.mpriqrf_g < -680)	RUN.mpriqrf_g = -680;	//ÓÐ¹¦µçÁ÷ÏÞ·ù
			else if(RUN.mpriqrf_g > 0)			RUN.mpriqrf_g = 0;
		}
		else
		{
			GRD_Utlv = GRD_Ut;
			MAIN_LOOP.cnt_lv_mpridvar = 0;
			MAIN_LOOP.cnt_lv_utlv = 0;	//201205LVRTatZB
			MAIN_LOOP.cnt_lv_mpridrf = 0;	//20130130
			RUN.mpridrf_var	= temp_var;		//2013-12-9ZZJÐÞ¸ÄÎÞ¹¦Ö¸ÁîÏìÓ¦ÎÊÌâ
			NGS_Udq_p_lv = NGS_Udq_p; 		//20130124
//			Te_feedback_lv=Te_feedback; 	//20130124
			GIVE.toqrf_lv=GIVE.toqrf; 	//20130128
			kq_lv = kq;				//Íø²à¿ØÖÆÓÃ 20130124
			RUN.mpriqrf_g_exlv = RUN.mpriqrf_g;		//20130221

			MAIN_LOOP.cnt_lv_holdpow = 0;	//20130205
		}
//			temp_iqrfmax = 	_stdby03; 					//BJTULVRT201204ÓÐ¹¦µçÁ÷ÏÞ·ù ¸øÕýÊý
//			if(RUN.mpriqrf_g > temp_iqrfmax)	RUN.mpriqrf_g = temp_iqrfmax;	//ÓÐ¹¦µçÁ÷ÏÞ·ù
//			else if(RUN.mpriqrf_g < 0)			RUN.mpriqrf_g = 0;


//-----------------------------------------------------------------
		if(M_ChkFlag(SL_LV_STATE)!=0 && M_ChkFlag(SL_UNBALANCE)!=0)//201205LVRTatZB
		{
//			RUN.mpriqrf_g   = 0;	//¼ì²âµ½²»Æ½ºâºó²»·¢ÓÐ¹¦ºÍÎÞ¹¦
			if(_STDBY9&0x0010!=0)	RUN.mpriqrf_g   = 0;	//116.4	20130228
			else	RUN.mpriqrf_g   = - kq * lv_ctrl2;		//20130228
			RUN.mpridrf_var = 0;
//			RUN.mpridrf_var = - (int16)_PROSTDBY2 * lv_ctrl5;		//²»¶Ô³ÆµøÂä·¢ÉÙÁ¿ÎÞ¹¦ÒÔµÖÏûµçÍøÎÞ¹¦ÎüÊÕ20121210
		}
//-----------
        RUN.mpridrf_g   = RUN.mpridrf_exi + RUN.mpridrf_var;


		RUN.mpridrf = Give_Integral(RUN.mpridrf_g,RUN.mpridstep,RUN.mpridrf);  		//»ú²àÓÐ¹¦µçÁ÷¸ø¶¨»ý·Ö
		RUN.mpriqrf = Give_Integral(RUN.mpriqrf_g,RUN.mpriqstep,RUN.mpriqrf);  		//»ú²àÎÞ¹¦µçÁ÷¸ø¶¨»ý·Ö

		if(M_ChkFlag(SL_LV_MSTOP)!=0)	 											//201007BJTULVRT							
		{
			RUN.mpridrf=0;				
			RUN.mpriqrf=0;
		}			
//------ÔÊíÊÊ±¦Àø´Å¸ø¶¨»ý·ÖBJTULVRT20121103
		id_max     =  (float)FUNC[NO_MIDRF].max;
	    temp       =  (float)_RU_IDT;
	    RUN.mpridstep  = abs(id_max)/temp;											//¼ÆËãÓÐ¦µçÁ÷¸ø¶¨²½³¤ 

    } 
//-----´ý»ú
    else                                    										//´ý»ú
    {
	   	M_ClrFlag(SL_MPR_RUNING);
		M_ClrFlag(SL_STACKEEPON);	//systest										//201005atcpc
	    MAIN_LOOP.cnt_mprsteady0= 0;
        
        RUN.mpridrf= 0;															    //¸ø¶¨»ý·Ö³õÊ¼ÖµÎª0
        RUN.mpriqrf= 0;															    //¸ø¶¨»ý·Ö³õÊ¼ÖµÎª0
        RUN.mpridrf_g= 0;															//¸ø¶¨»ý·Ö³õÊ¼ÖµÎª0
        RUN.mpriqrf_g= 0;	
        id_max     =  (float)FUNC[NO_MIDRF].max;
	    temp       =  (float)_RU_IDT;
	    RUN.mpridstep  = abs(id_max)/temp;											//¼ÆËãÓÐ¹¦µçÁ÷¸ø¶¨²½³¤ 
        iq_max     =  (float)FUNC[NO_MIQRF].max;
	    temp       =  (float)_RU_IQT;
	    RUN.mpriqstep  = abs(iq_max)/temp;											//¼ÆËãÎÞ¹¦µçÁ÷¸ø¶¨²½³¤  
 
        RUN.toqrf  = 0;																//¸ø¶¨»ý·Ö³õÊ¼ÖµÎª0
        RUN.aglrf  = 0;																//¸ø¶¨»ý·Ö³õÊ¼ÖµÎª0
        toq_max    =  (float)FUNC[NO_TOQRF].max;
	    temp       =  (float)_RU_TOQT;		//µ¥Î»ms
	    RUN.toqstep=  abs(toq_max)/temp;											//¼ÆËã×ª¾Ø¸ø¶¨²½³¤ 
        agl_max    =  (float)FUNC[NO_AGLRF].max;
	    temp       =  (float)_RU_AGLT;
//		temp       =  temp * 1000.0;		//µ¥Î»s									//ÎªCPCÖÐÐÄÊµÑéÌ¨½µµÍÓÐ¹¦¸ø¶¨ËÙ¶È201005atcpc
	    RUN.aglstep=  abs(agl_max)/temp; 											//¼ÆËã¹¦ÂÊÒòÊý½Ç¸ø¶¨²½³¤ 
//20121103
        GIVE.toqrfrun = GIVE.toqrf;	//201205LVRTatZB
		GRD_Utlv = GRD_Ut;
//20121103

	    PI_MPR_U.reference = 0;
	    PI_MPR_U.feedback = 0;
	    PI_MPR_U.out = 0;
	    PI_MPR_U.integrator = 0;
	    PI_MPR_Id.reference = 0;
	    PI_MPR_Id.feedback = 0;
	    PI_MPR_Id.out = 0;
	    PI_MPR_Id.integrator = 0;
	    PI_MPR_Iq.reference = 0;
	    PI_MPR_Iq.feedback = 0;
	    PI_MPR_Iq.out = 0;
 	    PI_MPR_Iq.integrator = 0;

    } 

	if(M_ChkFlag(SL_LV_MSTOP)!=0)	//LVRT
	{
	    PI_MPR_Id.reference = 0;
	    PI_MPR_Id.feedback = 0;
	    PI_MPR_Id.out = 0;
	    PI_MPR_Id.integrator = 0;
	    PI_MPR_Iq.reference = 0;
	    PI_MPR_Iq.feedback = 0;
	    PI_MPR_Iq.out = 0;
 	    PI_MPR_Iq.integrator = 0;	
	} 

	if(M_ChkFlag(SL_LV_NSTOP)!=0)	//LVRT
	{
	   PI_NPR_U.reference = 0;
	   PI_NPR_U.feedback = 0;
	   PI_NPR_U.out = 0;
	   PI_NPR_U.integrator = 0;
	   PI_NPR_Id.reference = 0;
	   PI_NPR_Id.feedback = 0;
	   PI_NPR_Id.out = 0;
	   PI_NPR_Id.integrator = 0;
	   PI_NPR_Iq.reference = 0;
	   PI_NPR_Iq.feedback = 0;
	   PI_NPR_Iq.out = 0;
	   PI_NPR_Iq.integrator = 0;	
	}  

}  

 
 
/*************************************************************************************************
** º¯Êýû³:  QepEncodPos
** ¹¦ÄÜÃèÊö:  µ÷ÕûÎ»ÖÃ¼ÆÊýÆ÷ÎªÔö¼ÆÊýÄ£Ê½£¬±àÂëÆ÷Î»ÖÃ¼ì²â
** Êä¡¡Èë:    ÎÈÌ¬ÏÂµÄ×ª×ÓµçÁ÷£¬¶¨×ÓÑ¹
** Êä¡¡ö:    ±àÂëÆ÷Î»ÖÃrad  
** ×¢  ÊÍ:    ¶¨×Ó½Ó´¥Æ÷ºÏÕ¢Ç°ÔËÐÐ£¬ºÏÕ¢ºó½ûÖ¹ÔËÐÐ¡£Ã¿¸ö²ÉÑùÖÜÆÚÔËÐÐÒ»´Î¡£·ÇÌØ±ðÖ¸Ã÷µÄ½Ç¶È¾ùÎªµç½Ç¶È
**--------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 	 
** ÈÕ¡¡ÆÚ: 	 20090330
**--------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ  ÆÚ:  20090409
**--------------------------------------------------------------------------------------------------
***********************************************************************************************/
 void QepEncodPos(void)
{
//	Uint16 temp1;
	float temp_pos;
/*//20091021atzy¾­¼ì²âÅÐ¶Ï£¬¼ÆÊýABÐÅºÅ²»±Ø¶Ôµ÷£¬Ô­ÏÈ¾ÍÊÇÔö¼ÆÊý£¬ÏÖ³¡½«¶Ôµ÷³ÌÐòÆÁ±ÎÁË
//----------------------ÅÐ¶ÏPOSCNT¼ÆÊý½Ïò£¬È·±£Ôö¼ÆÊýÄ£Ê½------------------------------------------
	if(M_ChkFlag(SL_QCAPSPDIN)!=0 && M_ChkFlag(SL_SPEED_HIGH)!=0)	//20091020atzy·ç»ú´ý»úÊ±µç»úÖá»á×óÓÒª¶¯//×ªËÙ¼ì²âÕý³£ºó£¬µ÷Õû±àÂëÆ÷¹¤×÷Ä£Ê½
	{
		temp1 = EQep2Regs.QEPSTS.bit.QDF;   				//¼ÆÊý·½Ïò  0-ÄæÊ±Õë-¼õ   1-Ë³Ê±Õë-Ôö
		if (temp1 == 0 && M_ChkFlag(SL_QEPSWP)==0) 			//Èç¹ûµ±Ç°´¦ÓÚ¼õ¼ÆÊýÄ£Ê½£¬²¢ÇÒÎ´¶Ôµ÷¹ýAB
		{
			temp1 = EQep2Regs.QDECCTL.bit.SWAP;				//²»ÂÛABÊÇ·ñ¶Ôµ÷¹ý£¬¶Ôµ÷µ±Ç°µÄABÐÅºÅ
		
			if (temp1 == 0) 
			{
				EQep2Regs.QPOSCNT = 1000;					//·ÀÖ¹¶Ôµ÷ºóÁ¢Âí±¨ÉÏÒç¹ÊÕÏ			
				EQep2Regs.QDECCTL.bit.SWAP  = 1;			//½»»»A¡¢BÊäÈëÐÅºÅ			
				EQep2Regs.QPOSCNT = 0; 					
			}
			else 
			{ 
				EQep2Regs.QPOSCNT = 1000;				 	//·ÀÖ¹¶Ôµ÷ºóÁ¢Âí±¨ÉÏÒç¹ÊÕÏ
				EQep2Regs.QDECCTL.bit.SWAP  = 0;			//»¹Ô­A¡¢BÊäÈëÐÅºÅ
				EQep2Regs.QPOSCNT = 0; 						
			} 
		
			M_SetFlag(SL_QEPSWP);					    	//ÖÃAB¶Ôµ÷Íê³É±êÖ¾Î»£¬·ÀÖ¹¶à´Î¶Ôµ÷AB
		}
//		else if (temp1==0 && M_ChkFlag(SL_QEPSWP)!=0)
//			M_SetFlag(SL_QEPPCDE);  						//ÖÃÎ»ÖÃ¼ÆÊýÆ÷·½Ïò´íÎó±êÖ¾Î»,ÎªÌá¸ß¿¹¸ÉÈÅÐÔÄÜ,¸Ã¹ÊÕÏ²»×öÅÐ¶Ï 20090804ÓÚcpc

	}
*/

//--------20121103--------
	DIP_STA_I.sintheta = TRS_NGS_U.sintheta; 
	DIP_STA_I.costheta = TRS_NGS_U.costheta; 
//------------------¶¨×Ó²àµçÁ÷dqÖµ--------------------------------------------------------------------
   	DIP_STA_I.a     = AD_OUT_STA_I.ac;
   	DIP_STA_I.b     = AD_OUT_STA_I.ba;
   	DIP_STA_I.c     = AD_OUT_STA_I.cb;
	Transform_3s_2s_2r(&DIP_STA_I);
//	DataFilter(0.99,&DIP_STA_I.dflt,DIP_STA_I.d); 	//×¢£º¸ÄÂË²¨ÊÇ400us/2.5kHzÖ´ÐÐÒ»´Î¡£¶¨×Ó²àµçÁ÷·´À¡ÖµÂË²¨,Ts=200us,ÂËµô¿ª¹ØÆµÂÊ´Î0.97
	DataFilter(0.99,&DIP_STA_I.qbsf,DIP_STA_I.q); 	//c=0.99->8Hz; c=0.9->88Hz

//	BS_STA_Id.in = DIP_STA_I.dbsf;					//50Hz notch ½«IsÖÐdcÁ¿ÂËµô
	BS_STA_Iq.in = DIP_STA_I.qbsf;
//	BS_Loop_S(&BS_STA_Id,BS_PARA_STAI);	//ÏÝ²¨Æ÷Ê¹ÓÃÐè2e-4Ö´ÐÐÒ»´Î£¬ÕâÀïÊÇ200usÖ´ÐÐÒ»´Î201205LVRTatZB
	BS_Loop_S(&BS_STA_Iq,BS_PARA_STAI);
//	DIP_STA_I.dflt = BS_STA_Id.out;
	DIP_STA_I.qflt = BS_STA_Iq.out;

//   	TRS_MPR_I.a     = AD_OUT_MPR_I.a + 0;	//ÑéÖ¤¼ÆËã´ÅÁ´ºÍ×ª×Ó¿ØÖÆ¶¼½øÐÐ³õÊ¼ÁãÆ¯BJTULVRT201204
//   	TRS_MPR_I.b     = AD_OUT_MPR_I.b + 0;
//   	TRS_MPR_I.c     = AD_OUT_MPR_I.c + 0;
   	TRS_MPR_I.a     = AD_OUT_MPR_I.a;	//²»½øÐÐ³õÊ¼ÁãÆ¯BJTULVRT201204
   	TRS_MPR_I.b     = AD_OUT_MPR_I.b;
  	TRS_MPR_I.c     = AD_OUT_MPR_I.c;
	Transform_3s_2s_2r(&TRS_MPR_I);
	DataFilter(0.4,&TRS_MPR_I.dflt,TRS_MPR_I.d); 			//»ú²àµçÁ÷·´À¡ÖµË²¨£¬ Ts=200us,fh=1.2kHz,ÂËµô¿ª¹ØÆµÂÊ´Î
	DataFilter(0.4,&TRS_MPR_I.qflt,TRS_MPR_I.q); 			//»ú²àµçÁ÷·´À¡ÖµÂË²¨£¬ Ts=200us,fh=1.2kHz,ÂËµô¿ª¹ØÆµÂÊ´Î

//--------20121103--------

//--------QCAPÕý³£¹¤×÷ºó¼ÆÊý±àÂëÆ÷Î»ÖÃ--------------------------------------------------------------
	if(M_ChkFlag(SL_QCAPSPDIN)!=0 && M_ChkFlag(SL_MPR_SYNOK)==0)//×ªËÙ¼ì²âÕý³£ÇÒ¶¨×Ó²¢ÍøÇ°£¬¼ÆËã±àÂëÆ÷Î»ÖÃ 201005atcpc
	{
//		temp_pos = _encodpos;
//		QEPDATA.encodpos =  - temp_pos;   	//ÓÉ²Ù×÷Æ÷Ö±½Ó¸ø¶¨³õÊ¼Î»ÖÃ½Ç¶È										
//--------20130801--------
	   if((_STDBY9&0x0040)!=0)	//116.6¿ª¹ØÁ¿±¸ÓÃ
	   {
		temp_pos = _encodpos;
		QEPDATA.encodpos =  - temp_pos;   	//ÓÉ²Ù×÷Æ÷Ö±½Ó¸ø¶¨³õÊ¼Î»ÖÃ½Ç¶È	
	   }
	   else
	   {
		if(M_ChkFlag(SL_MPR_START)==0)
		{
			M_SetFlag(SL_ENCODEPOS_OK);		//20130525
			MAIN_LOOP.cnt_ecposcheck = 0;
			MAIN_LOOP.cnt_ecpostrace = 0;
		}
		else if(M_ChkCounter(MAIN_LOOP.cnt_ecposcheck,1500)>0)		//»ú²à¹¤×÷1.5sºó¿ªÊ¼ÅÐ¶ÏÑ¹²î
		{
			if(abs(MEAN_DATA.ubc_t)>40)		//20130604
			{
				M_ClrFlag(SL_ENCODEPOS_OK);
				M_SetFlag(SL_ECPOSSEARCH);		//20130604
			}
		}

		if(M_ChkFlag(SL_MPR_START)!=0 && M_ChkFlag(SL_ECPOSSEARCH)!=0)
		{
			if(M_ChkFlag(SL_ENCODEPOS_OK)==0)
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_ecpostrace,600)>0)	//600ms delay
				{

					if((abs(MEAN_DATA.ubc_t)<=90) && (ecpostep1 == 0) && (ecpostep2 == 0) && (ecpostep3 == 0))
					{
//						QEPDATA.encodpos = QEPDATA.encodpos - 0.5235988;		//30¶È
						QEPDATA.encodpos = QEPDATA.encodpos - 0.5061455;		//29¶È	20130604
						if (QEPDATA.encodpos < - TWOPAI)  	QEPDATA.encodpos = QEPDATA.encodpos + TWOPAI;
						ecpostep2 = 1;
						ecpostep3 = 1;
						MAIN_LOOP.cnt_ecpostrace = 0;
					}
					else if((abs(MEAN_DATA.ubc_t)>90) && (ecpostep2 == 0))
					{
//						QEPDATA.encodpos = QEPDATA.encodpos + 0.5235988;		//30¶È
						QEPDATA.encodpos = QEPDATA.encodpos + 0.5410521;		//31¶È	20130604
						if (QEPDATA.encodpos > 0)  	QEPDATA.encodpos = QEPDATA.encodpos - TWOPAI;
						ecpostep1 = 1;
						MAIN_LOOP.cnt_ecpostrace = 0;
					}
					else if(((abs(MEAN_DATA.ubc_t)<=90) && (ecpostep2 == 0)) || (ecpostep1 == 1))
					{
//						QEPDATA.encodpos = QEPDATA.encodpos - 0.3490659;		//20¶È
						QEPDATA.encodpos = QEPDATA.encodpos - 0.3839724;		//22¶È	20130604
						if (QEPDATA.encodpos > 0)  	QEPDATA.encodpos = QEPDATA.encodpos - TWOPAI;
						else if (QEPDATA.encodpos < - TWOPAI)  	QEPDATA.encodpos = QEPDATA.encodpos + TWOPAI;
						ecpostep1 = 0;
						ecpostep2 = 1;
						MAIN_LOOP.cnt_ecpostrace = 0;
					}
					else if(ecpostep2 == 1)
					{
						if(abs(MEAN_DATA.ubc_t)<=30)	//30V	beiyong3 20130604
						{
							M_SetFlag(SL_ENCODEPOS_OK);
							M_ClrFlag(SL_ECPOSSEARCH);
							ecpostep1 = 0;
							ecpostep2 = 0;
							ecpostep3 = 0;
							MAIN_LOOP.cnt_synfail = 0;
						}
						else
						{
							QEPDATA.encodpos = QEPDATA.encodpos + 0.08727;		//5¶È=0.08727
							if (QEPDATA.encodpos > 0)  	QEPDATA.encodpos = QEPDATA.encodpos - TWOPAI;
							MAIN_LOOP.cnt_ecpostrace = 0;
						}
					}
				}
			}
		}
	   }
	}
	else if(M_ChkFlag(SL_MPR_SYNOK)!=0)
	{
		M_SetFlag(SL_ENCODEPOS_OK);
		M_ClrFlag(SL_ECPOSSEARCH);
		ecpostep1 = 0;
		ecpostep2 = 0;
		ecpostep3 = 0;
		MAIN_LOOP.cnt_synfail = 0;
	}

//------------------------
} 

/***************************************************************************************************
** º¯ÊýÃû³Æ: PhaseOrderChk
** ¹¦ÄÜÃèÊö: ¼ì²éÏàÐòÊÇ·ñÕýÈ·¡£
** Êä¡¡Èë:
** Êä¡¡³ö:   
** ×¢  ÊÍ: ÔÚºÏÖ÷¶ÏÇ°ÔËÐÐ£¬ºÏÖ÷¶Ïºó²»ÔÙµ÷ÓÃ´Ë×Óº¯Êý cap5-ubc   cap6-uab
**--------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡ÆÚ: 	20090627
**--------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:	
** ÈÕ  ÆÚ:	
**--------------------------------------------------------------------------------------------------
***********************************************************************************************/
 void PhaseOrderChk(void)
{
//	float temp,phaseshift;
	float temp;

//------------¼ÆËãÍøÑ¹ÆµÂÊ¡¢½ÇÆµÂÊ¡¢CAPµ¥Î»Ê±¼äÄÚÍøÑ¹ÏàÎ»ÔöÁ¿--------------------------------------
	temp=(float)ECap5Regs.CAP1;									//¼ÇÂ¼²¶»ñÖµ

	if(temp>0)
	{
		CAP5.prd = temp;										
		CAP5.freqtmp   = 150.0e6/CAP5.prd;						//¼ÆËã±¾´ÎÆµÂÊ
	}

//	if (abs(CAP5.freqtmp-50)<1)									//Èç¹ûÆµÂÊÕý³£Ôò¸üÐÂÆµÂÊ
	if ((CAP5.freqtmp>_sc_freq2) && (CAP5.freqtmp<_sc_freq1))	//Èç¹ûÆµÂÊÕý³£Ôò¸üÐÂÆµÂÊ201005atcpc
	{
		CAP5.freq   = CAP5.freqtmp;    							//¸üÐÂ
		CAP5.omigasyn  = TWOPAI * CAP5.freq;						
		CAP5.radpertb  = TWOPAI/CAP5.prd;						//¼ÆËãÒ»¸ö¼ÆÊýÖÜÆÚ½Ç¶ÈÔöÁ¿
//		M_ClrFlag(SL_GRDFQE);									//ÇåÆµÂÊÊ§°Ü±êÖ¾
	}
//	else	M_SetFlag(SL_GRDFQE);								//·ñÔòÖÃÆµÂÊÓÐÎó±êÖ¾Î»	
		
/*
//----------------¼ÆËãUbc¬UabÏà»-----------------------------------------------------------------
	temp = (float)ECap5Regs.TSCTR;
	CAP5.phase = temp * CAP5.radpertb;							//Ö÷¶ÏÇ°ÍøÑ¹UbcÏàÎ» 
	temp = (float)ECap6Regs.TSCTR;
	temp = temp * CAP5.radpertb;								//Ö÷¶ÏÇ°ÍøÑ¹UabÏàÎ»
	
//---------------¼ÆËãÏàÎ»²î------------------------------------------------------------------------
	phaseshift = temp - CAP5.phase;								//¼ÆËãÖ÷¶ÏÇ°ÍøÑ¹UabÓëUbcµÄÏàÎ»²î
	if (phaseshift<0)   phaseshift = phaseshift + TWOPAI;		//Ïà²îÏÞÖÆÔÚ0µ½2 * PAIÖ®¼ä

//---------------¼ì²éÏàÐòÊÇ·ñÕýÈ·------------------------------------------------------------------
	if(abs(phaseshift - TWOPAI_3) >  PAI_3)  					//µ±Ïà²îÎó²î³¬¹ýPAI/3Ê±±¨´í
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_phorde,DELAY_PHORDE)>0)	//12s ÉÏµçÑÓÊ±ÅÐ¶ÏÍø²àÏàÐò¹ÊÕÏ 201005atcpc
			M_SetFlag(SL_PHORDE);								//DSPÉÏµçºó	ÏàÐòÓÐÎó±êÖ¾Î»ÖÃ1 20090817
	}
	else 
	{
		M_ClrFlag(SL_PHORDE);									// ÇåÁãÏàÐòÓÐÎó±êÖ¾Î»
		MAIN_LOOP.cnt_phorde=0;
	}
*/
//--------¸ü¸ÄÏàÐò¼ì²â·½·¨20120228	BJTULVRT201204--------
//----------------¼ÆËãUbc£¬UabÏàÎ»-----------------------------------------------------------------
	temp = (float)ECap5Regs.TSCTR;
	CAP5.phase = temp * CAP5.radpertb;							//Ö÷¶ÏÇ°ÍøÑ¹UbcÏàÎ» 
//---------------¼ì²éÏàÐòÊÇ·ñÕýÈ·------------------------------------------------------------------
	if((CAP5.phase < TWOPAI_12 ))  					//µ±Ïà²îÎó²î³¬¹ýPAI/3Ê±±¨´í
	{
		if(AD_OUT_NGS_U.ab < 0)
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_phorde,DELAY_PHORDE)>0)	//12s ÉÏµçÑÓÊ±ÅÐ¶ÏÍø²àÏàÐò¹ÊÕÏ 201005atcpc
			M_SetFlag(SL_PHORDE);			//DSPÉÏµçºó	ÏàÐòÓÐó±êÖ¾Î»ÖÃ1 20090817
		}
		else
		{
			M_ClrFlag(SL_PHORDE);									// ÇåÁãÏàÐòÓÐÎó±êÖ¾Î»
			MAIN_LOOP.cnt_phorde=0;
		}
	}
}

/**************************************************************************************************
** º¯ÊýÃû³Æ: CapQepCtrl
** ¹¦ÄÜÃèÊö: compute vector position and frequency
** Êä  Èë:	  
** Êä¡¡³ö:    
** ×¢  ÊÍ:    
			//Cap4.adsmptsctr=ECap4Regs.TSCTR; 		//¼ÓÔÚAD²ÉÑùÆô¶¯Óï¾äºó
			//QEPDATA.adsmposcnt=EQep2Regs.QPOSCNT;	//¼ÓÔÚAD²ÉÑùÆô¶¯Óï¾äºó
**--------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 
** ÈÕ¡¡:	20090409 
**--------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ  ÆÚ:	20090812 at cpc
**--------------------------------------------------------------------------------------------------
**************************************************************************************************/
 void CapQepCtrl(void)
{
	float	temp,temp1,radpert0;
	Uint32  temp2,temp3;

 if((_COMMAND2&0x0001)!=0)	//¿ª¹ØÁ¿±¸ÓÃ0  201011PLL
 {

//------------¼ÆËãÍø²àÍøÑ¹ÂÊ¡¢÷Ö½ÇÆµÂÊ¡¢CAP¥Î»Ê±¼äÄÚÍøÑ¹ÏàÎ»ÔöÁ¿-----------------------------------
	temp2 = ECap4Regs.CAP1;
	temp  =	(float)temp2;									//²¶»ñÊÂ¼þÖÜÆÚÖµ
	
	if(temp>0)
	{
		CAP4.prd = temp;									
		CAP4.freqtmp   = 150.0e6/CAP4.prd;					//¼ÆËã±¾´ÎÍøÑ¹ÆµÂÊ
	}

//	if (abs(CAP4.freqtmp - 50)<1)							//Èç¹ûÆµÂÊÕý³£Ôò¸üÐÂÍøÑ¹ÆµÂÊ
	if ((CAP4.freqtmp>_sc_freq2) && (CAP4.freqtmp<_sc_freq1))	//Èç¹ûÆµÂÊÕý³£Ôò¸üÐÂÆµÂÊ201005atcpc
	{
		CAP4.freq      = CAP4.freqtmp;				    	//¸üÐÂÍøÑ¹ÆµÂÊ CAP4.freqtmpÊÇÊµ¼ÊÊµÊ±ÍøÆµ
		CAP4.omigasyn  = TWOPAI * CAP4.freq;						
		CAP4.radpertb  = TWOPAI * CAP4.freq / 150.0e6;		//¼ÆËãÒ»¸ö¼ÆÊýÖÜÆÚ½Ç¶ÈÔöÁ¿
		M_ClrFlag(SL_GRDFQE);					   	   	 	//ÇåÆµÂÊÊ§°Ü±êÖ¾
	}
	else	M_SetFlag(SL_GRDFQE);							//ÆµÂÊÓÐÎó±êÖ¾Î»ÖÃ1	
		


//-----------¼ÆËãÍø²àËã·¨Õý±ä»»½Ç¶È----------------------------------------------------------------                         
	
	radpert0       = TWOPAI * CAP4.freq * 0.2e-3;   		//¼ÆËãÒ»¸öT0ÖÐÏÖÜÆÚ½Ç¶ÈÔöÁ¿ T0=0.2ms
  
    if(M_ChkFlag(SL_GRDFQE)!=0)
	{
	   CAP4.nprtrstheta =CAP4.nprtrstheta + radpert0;              
       if(CAP4.nprtrstheta >= TWOPAI)	CAP4.nprtrstheta = CAP4.nprtrstheta - TWOPAI; 
	}
	else	
	{	
	  if(CAP4.adsmptsctr > 3.5e6)
	  {
	  	 CAP4.nprtrstheta =CAP4.nprtrstheta + radpert0;               
       	 if(CAP4.nprtrstheta >= TWOPAI)	CAP4.nprtrstheta = CAP4.nprtrstheta - TWOPAI; 
	  }
	  else
	  {
	     CAP4.nprtrstheta = (float)CAP4.adsmptsctr * CAP4.radpertb;
	  	 if(CAP4.nprtrstheta >= TWOPAI)	CAP4.nprtrstheta = CAP4.nprtrstheta - TWOPAI; 
      }
	}
 }		//20121103
 else	//Ïò½Ç¶È²ÉÓÃPLL
 {
//-----------PLL¼ÆËãÍø²à½ÇÆµÂÊºÍ½Ç¶È-201011PLL--------------------------------------------------------------
	DataFilter(0.999,&PI_DATA_PLL.outflt,PI_DATA_PLL.out); 	//PLLÊä³ö½ÇÆµÂÊÔÚµ÷½ÚÆÚ¼ä»áÕñµ´,¼ÓÈëÊÊµ±ÂË²¨
	temp    = PI_DATA_PLL.outflt;
	
	if(temp>0)	CAP4.freqtmp   = temp / TWOPAI;				//¼ÆËã±¾´ÎÍøÑ¹ÆµÂÊ

//	if (abs(CAP4.freqtmp - 50)<1)							//Èç¹ûÆµÂÊÕý³£Ôò¸üÐÂÍøÑ¹ÆµÂÊ
	if ((CAP4.freqtmp>_sc_freq2) && (CAP4.freqtmp<_sc_freq1))//Èç¹ûÆµÂÊÕý³£ÇÒ±£³Ö10ms»Ö¸´ÆµÂÊË¢ÐÂ201011PLL
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_freqrecv,DELAY_FREQRECV)>=0)
		{
			CAP4.freq      = CAP4.freqtmp;				    	//¸üÐÂÍøÑ¹ÆµÂÊ CAP4.freqtmpÊÇÊµ¼ÊÊµÊ±ÍøÆµ
			CAP4.omigasyn  = temp;			
			CAP4.radpertb  = CAP4.omigasyn / 150.0e6;			//¼ÆËãÒ»¸ö¼ÆÊýÖÜÆÚ½Ç¶ÈÔöÁ¿			
			M_ClrFlag(SL_GRDFQE);					   	   	 	//ÇåÆµÂÊ§°Ü±êÖ¾	
		}
	}
	else	
	{
		M_SetFlag(SL_GRDFQE);								//ÆµÂÊÓÐÎó±êÖ¾Î»ÖÃ1	
		MAIN_LOOP.cnt_freqrecv=0;
	}

//-----------¼ÆËãÍø²àËã·¨Õý±ä»»½Ç¶È-201011PLL----------------------------------------------------------------                         
	
	radpert0 = CAP4.omigasyn * 0.2e-3;   					//¼ÆËãÒ»¸öT0ÖÐ¶ÏÖÜÆÚ½Ç¶ÈÔöÁ¿ T0=0.2ms
  
    if(M_ChkFlag(SL_GRDFQE)!=0)
	{
	   CAP4.nprtrstheta =CAP4.nprtrstheta + radpert0;              
       if(CAP4.nprtrstheta >= TWOPAI)	CAP4.nprtrstheta = CAP4.nprtrstheta - TWOPAI; 
	}
	else	
	{	
	   CAP4.nprtrstheta = pll_theta;
	}
 }

//-------------LVÆÚ¼ä»ú²à¶¨Ïò½Ç¶È¿¹¸ÉÈÅ¿ØÖÆ20121103LVRT-------------------------
	if(M_ChkFlag(SL_LV_STATE)!=0)
	{
	   CAP4.nprtrstheta_lv =CAP4.nprtrstheta_lv + radpert0;              
       if(CAP4.nprtrstheta_lv >= TWOPAI)	CAP4.nprtrstheta_lv = CAP4.nprtrstheta_lv - TWOPAI; 		
	}
	else
	{
		CAP4.nprtrstheta_lv =CAP4.nprtrstheta;              
	}
//¿ª¹ØÁ¿±¸ÓÃ2  
	if((_COMMAND2&0x0004)==0)	CAP4.nprtrstheta_lv =CAP4.nprtrstheta;	//=1ÊÇLVÆÚ¼ä»ú²à½Ç¶È¿¹¸ÉÈÅµÝÔö,ÈÏÎªÍøÑ¹ÏàÎ»²»±ä

/*
//-------------×ª×Ó»úÐµ½ÇÆµÂÊºÍµç½ÇÆµÂÊ------------------------------------------------------------	

	if(M_ChkFlag(SL_SPEED_HIGH)!=0)
	{
		QEPDATA.qposlat2 = EQep2Regs.QPOSLAT;                 		//²ÉÓÃSYSCLK=150M£¬QUPRD=1.5M,µ¥Î»ÆµÂÊÎª100Hz
		if(QEPDATA.qposlat2 < QEPDATA.qposlat1)
			QEPDATA.qposlat  = QEPDATA.qposlat2 + _SC_PLSPRVL * 4 - QEPDATA.qposlat1;  //20090817
		else
			QEPDATA.qposlat  = QEPDATA.qposlat2 - QEPDATA.qposlat1;
		
		QEPDATA.qposlat1  = QEPDATA.qposlat2;
		
		temp = 50.0 *  PAI * (float)QEPDATA.qposlat / _SC_PLSPRVL; 		//×ª×Ó»úÐµ½ÇÆµÂÊrad/s  QEPDATA.omigamec = 2 * PAI * (150M/1.5M) *  QEPDATA.qposlat/(PLSPRVL * 4 )  20090817	 						                            		
	}
	else
	{
		QEPDATA.qcapprd = EQep2Regs.QCPRD;	                		//¶ÔeQEPÄ£¿éµÄQCLK½øÐÐ32·ÖÆµ£¬QCAPÊ±ÖÓ²ÉÓÃSYSCLKOUT/128  20090817
		temp = 37.5e6 *  PAI/(_SC_PLSPRVL  * (float)QEPDATA.qcapprd); //×ª×Ó»úÐµ½ÇÆµÂÊrad/s  QEPDATA.omigamec = 2 * PAI * (150M/128) *  64/(QCPRD * PLSPRVL * 4 )  20090817	 						                            		
	}
*/
//--------20121103--------
//-------------×ª×Ó»úÐµ½ÇÆµÂÊºÍµç½ÇÆµÂÊ------------------------------------------------------------	

	if(M_ChkFlag(SL_SPEED_HIGH)!=0)
	{
		QEPDATA.qposlat2 = EQep2Regs.QPOSLAT;                 		//²ÉÓÃSYSCLK=150M£¬QUPRD=1.5M,µ¥Î»ÆµÂÊÎª100Hz
//====================ÖØÒªÐÞÕý 201011BJTULVRT======================================================================
		if(QEPDATA.qposlat2 < QEPDATA.qposlat1)				//201011LVRT DSPÄÚ²¿µÄEQep2Regs.QPOSLAT¸üÐÂËÙ¶ÈÃ»ÓÐT0ÕâÃ´¿ì£¡Ë¢ÐÂ100HzºÍ²ÉÑùÎª5kHz
		{
			QEPDATA.qposlat  = QEPDATA.qposlat2 + _SC_PLSPRVL * 4 - QEPDATA.qposlat1;  //20090817
			temp = 50.0 *  PAI * (float)QEPDATA.qposlat / _SC_PLSPRVL; 		//×ª×Ó»úÐµ½ÇÆµÂÊrad/s  QEPDATA.omigamec = 2 * PAI * (150M/1.5M) *  QEPDATA.qposlat/(PLSPRVL * 4 )  20090817	 						                            		
		}			
		else if(QEPDATA.qposlat2 == QEPDATA.qposlat1)
		{
			temp = QEPDATA.omigamec;
		}
		else
		{
			QEPDATA.qposlat  = QEPDATA.qposlat2 - QEPDATA.qposlat1;
			temp = 50.0 *  PAI * (float)QEPDATA.qposlat / _SC_PLSPRVL; 		//×ª×Ó»úÐµ½ÇÆµÂÊrad/s  QEPDATA.omigamec = 2 * PAI * (150M/1.5M) *  QEPDATA.qposlat/(PLSPRVL * 4 )  20090817	 						                            		
		}
			
		QEPDATA.qposlat1  = QEPDATA.qposlat2;
	}
	else
	{
		QEPDATA.qcapprd = EQep2Regs.QCPRD;	                		//¶ÔeQEPÄ£¿éµÄQCLK½øÐÐ32·ÖÆµ£¬QCAPÊ±ÖÓ²ÉÓÃSYSCLKOUT/128  20090817
		temp = 37.5e6 *  PAI/(_SC_PLSPRVL  * (float)QEPDATA.qcapprd); //×ª×Ó»úÐµ½ÇÆµÂÊrad/s  QEPDATA.omigamec = 2 * PAI * (150M/128) *  64/(QCPRD * PLSPRVL * 4 )  20090817	 						                            		
	}
//--------20121103--------

	temp2 = EQep2Regs.QEPSTS.bit.COEF;						//Capture overflow error--0.43ms question
	if(temp2!=0)	
	{
		EQep2Regs.QEPSTS.bit.COEF = 1;						//clear overflow flag
		M_ClrFlag(SL_QCAPSPDIN); 
		MAIN_LOOP.cnt_qcapspdin = 0;
	}	 		
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_qcapspdin,DELAY_QCAPSPDIN)>=0)  //120ms 20090817
			M_SetFlag(SL_QCAPSPDIN); 
	} 			
		 
   	if(M_ChkFlag(SL_QCAPSPDIN)==0)			//20090813 cpc
   	{
   		QEPDATA.omigamec = 0;  				//¼ì²â·¶Î§Ö®Íâ 
		M_ClrFlag(SL_SPEED_HIGH);           //20090817
		MAIN_LOOP.cnt_qcapdisturb=0;
	}
	else
	{
		if(QEPDATA.omigamec==0)		
		{	
			QEPDATA.omigamec = temp;
			MAIN_LOOP.cnt_qcapdisturb=0;
		}
		else if(temp >= (0.75 * QEPDATA.omigamec) && temp <= (1.25 * QEPDATA.omigamec))	//8·ÖÆµÊ±¶ªÊ§1¸öA»òBÐÅºÅ²âËÙ½«ÊÇÉÏÒ»´ÎµÄ1.25±¶£¬¶à³ö1¸öÔòÊÇ0.75±¶
		{
			QEPDATA.omigamec = temp;												//Èç¹û×ªËÙÔÚÕýÈ··¶Î§ÄÚ£¬¸üÐÂ;·ñÔò±£³ÖÉÏÒ»´ÎµÄÖµ 20090804ÓÚcpc
			MAIN_LOOP.cnt_qcapdisturb=0;
			M_ClrFlag(SL_QCAPDISTURB);		
		}
		else
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_qcapdisturb,DELAY_QCAPDISTURB)>=0)		//ÑÓÊ±100ms,Á¬Ðø10´Î¾ù±»¸ÉÈÅ,±¨¹ÊÕÏ 20090817cpc
			{
				M_SetFlag(SL_QCAPDISTURB);
				QEPDATA.omigamec = temp;    //20090817
			}
		}
	}
	 
   		  		
	QEPDATA.omigarote = POLEPAIRES * QEPDATA.omigamec;								//×ª×Óµç½ÇÆµÂÊrad/s
//	DataFilter(0.985,&QEPDATA.omigaroteflt,QEPDATA.omigarote); 						//×ªËÙ·´À¡ÖµÂË²¨£¬ Ts=200us,Tr=100ms 20090815
	QEPDATA.rotradpret0 = QEPDATA.omigarote * 0.2e-3;								//Ã¿´ÎT0ÖÜÆÚ×ª×Óµç½Ç¶ÈÔöÁ¿£¬µ¥Î»»¡¶È 20090815

//-----------×ª²î½ÇÆµÂÊ----------------------------------------------------------------------------
	CAP4.omigaslp = CAP4.omigasyn - QEPDATA.omigarote;  	//×ª²î½ÇÆµÂÊ  20090815


//------------¼ì²â±àÂëÆ÷ÊÇ·ñÓÐ¹ÊÕÏ------------------------------------------------------------------	
	temp = EQep2Regs.QFLG.bit.PCO;
	if((temp!=0) && M_ChkFlag(SL_QCAPSPDIN)!=0)		
	{
	   _NQEPPCO++;
//	   if(_NQEPPCO >10)		   M_SetFlag(SL_QEPPCO);    	//ÖÃ¼ÆÊýÆ÷ÉÏÒç±êÖ¾Î»
	   if(_NQEPPCO >1000)		   M_SetFlag(SL_QEPPCO);    	//ÖÃ¼ÆÊýÆ÷ÉÏÒç±êÖ¾Î»	ÂË²¨Ê±¼ä2s,20120310
	   EQep2Regs.QCLR.bit.PCO = 1;
	   MAIN_LOOP.cnt_qepcntok=0;
	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_qepcntok,DELAY_QEPCNTOK)>=0) 				//1s Á¬Ðø1sÎ´·¢ÉúÉÏÒç´íÎó
		{
			_NQEPPCO=0;
			M_ClrFlag(SL_QEPPCO);
		}
	}

//--------20121103--------
//-----µç»ú·´×ª/ABÐÅºÅ·´½Ó¹ÊÕÏÅÐ¶Ï-20110829HC---------------------	
	if(M_ChkFlag(SL_QCAPSPDIN)!=0)							//×ªËÙÔÚ·¶Î§ÄÚ
	{
		QEP_CNT1 = QEP_CNT2;
		QEP_CNT2 = QEPDATA.adsmposcnt;
		if(QEP_CNT2 < QEP_CNT1)		
		{
			QEPPHE_N++;
			if(QEPPHE_N > 2)  
			{
				M_SetFlag(SL_QEPPHE);
				QEPPHE_N =3;
			}	
		}
		else if(QEP_CNT2 > QEP_CNT1)
		{		
			QEPPHE_N--;
			if(QEPPHE_N <= 0)
			{
				M_ClrFlag(SL_QEPPHE);
				QEPPHE_N = 0;	
			}
		}
	}
	else
	{
		QEP_CNT1 = 0;
		QEP_CNT2 = 0;
		QEPPHE_N = 0;
		M_ClrFlag(SL_QEPPHE);
	}

//--------20121103--------

/*						
	temp = EQep2Regs.QFLG.bit.PHE;
	if(temp!=0)		
	{
	    M_SetFlag(SL_QEPPHE);								//QEPµÄABÐÅºÅÏàÎ»¹ÊÕÏ,Ïà²î·Ç90¶È,¹ÊÕÏ³ÌÐòÖÐ²»ÔÙ¼ì²â¸Ã¹ÊÕÏ
	    EQep2Regs.QCLR.bit.PHE = 1; 
	}
*/
//---------ÅÐ±ðA/BÐÅºÅÏûÊ§-201007BJTULVRT----------------------------------------------------------------------
	if(M_ChkFlag(SL_QCAPSPDIN)!=0)							//×ªËÙÔÚ·¶Î§ÄÚ
	{
		temp = EQep2Regs.QFLG.bit.QDC;						//QPOSCNT¼ÆÊý·½Ïò¸Ä±ä
		if(temp!=0)		
		{
	  		EQep2Regs.QCLR.bit.QDC = 1;
			M_SetFlag(SL_QEPPCDE);
		}
		else	M_ClrFlag(SL_QEPPCDE);
	}
	else
	{
		M_ClrFlag(SL_QEPPCDE);
		EQep2Regs.QCLR.bit.QDC = 1;
	}

//----------»ú²àËã·¨Õý±ä»»½Ç¶È--------------------------------------------------------------------- 
    QEPDATA.posilat = EQep2Regs.QPOSILAT;								//ZÐÅºÅµ½À´Ê±POSCNTµÄ¼ÆÊýÖµ
//	if(QEPDATA.posilat < (_SC_PLSPRVL * 4 -20))
//	if((QEPDATA.posilat < (_SC_PLSPRVL * 4 -20)) || M_ChkFlag(SL_QEPPCDE)!=0)	//201007BJTULVRT
	if(((QEPDATA.posilat < (_SC_PLSPRVL * 4 -20)) || M_ChkFlag(SL_QEPPCDE)!=0)&&(M_ChkFlag(SL_SPEED_HIGH)!=0)) //¸ß×ªËÙÇøÔÙ¼ì²â,·ÀÖÎÎó±¨20110829HC
	{	
		QEPDATA.rotpos = QEPDATA.rotpos + QEPDATA.rotradpret0;
		if(QEPDATA.rotpos > 2* TWOPAI) QEPDATA.rotpos = QEPDATA.rotpos - 2* TWOPAI;
//		if((M_ChkCounter(MAIN_LOOP.cnt_qepzdisturb,DELAY_QEPZDISTURB)>=0)&&(M_ChkFlag(SL_QCAPSPDIN)!=0))    //1s ×ªËÙ¼ì²âÕý³£ºó£¬¼ÆËã±àÂëÆ÷Î»ÖÃ201007BJTULVRT
//		if(M_ChkCounter(MAIN_LOOP.cnt_qepzdisturb,DELAY_QEPZDISTURB)>=0)   //1s
//			M_SetFlag(SL_QEPZDISTRUB);
	}
	else
	{														//temp1 = POLEPAIRES  * 2 * PAI * QEPDATA.adsmposcnt/(PLSPRVL * 4);
//		temp1 = 1.53398e-3 * QEPDATA.adsmposcnt; 			// POLEPAIRES=2, PLSPRVL=2048cpc								
		temp1 = 1.5708 * POLEPAIRES * QEPDATA.adsmposcnt / ((float)_SC_PLSPRVL); 			//201005atcpc							
		QEPDATA.rotpos  = 	temp1;                          //0--4PIE
		MAIN_LOOP.cnt_qepzdisturb=0;
		M_ClrFlag(SL_QEPZDISTRUB);
	}

//=========testing==201011LVRT=======20121103====================
	QEPDATA.rotpos = 1.5708 * POLEPAIRES * QEPDATA.adsmposcnt / ((float)_SC_PLSPRVL); 			//201005atcpc							
//=========testing=============================

	temp1 = QEPDATA.rotpos - QEPDATA.encodpos;				//AD²ÉÑùÊ±¿Ì×ª×ÓÎ»ÖÃ - ³õÊ¼Î»ÖÃ½Ç¶È
                                                            // -2PIE -- 6PIE
	if 		(temp1 < 0) 	  	temp1 = temp1 + TWOPAI;	
	else if (temp1 > TWOPAI)  	temp1 = temp1 - TWOPAI;		//AD²ÉÑùÊ±¿Ì×ª×ÓÎ»ÖÃÈ¡Ä£(0-TWOPAI)
	if 		(temp1 < 0) 	  	temp1 = temp1 + TWOPAI;	
	else if (temp1 > TWOPAI)  	temp1 = temp1 - TWOPAI;		//AD²ÉÑùÊ±¿Ì×ª×ÓÎ»ÖÃÈ¡Ä£(0-TWOPAI)
							
//	temp = TWOPAI_3 + CAP4.nprtrstheta - temp1;				//Õý±ä»»½Ç¶È£¬Î´È¡Ä££¬£¨-1.3PAI---2.7PAI£© 
	temp = TWOPAI_3 + CAP4.nprtrstheta_lv - temp1;			//Õý±ä»»½Ç¶È£¨-1.3PAI---2.7PAI£© 20121103LVRT
															//¶¨×ÓÈÆ×é½Ç½Ó,BÏàµçÑ¹¹ýÁãÊ±£¬µçÑ¹Ê¸Á¿Îª30degree
	if		(temp > TWOPAI)	 	temp = temp - TWOPAI;	
	else if	(temp < 0)	  		temp = temp + TWOPAI;		//È¡Ä£ (0-TWOPAI)
//	if		(temp > TWOPAI)	 	temp = temp - TWOPAI;	
//	else if	(temp < 0)	  		temp = temp + TWOPAI;		//È¡Ä£ (0-TWOPAI)
		
	CAP4.mprtrstheta = temp;								//»ú²àÕý±ä»»½Ç¶È

	QEPDATA.rotposdisp = QEPDATA.rotpos;	//BJTULVRT201204
//	if 		(QEPDATA.rotpos < 0) 	  	QEPDATA.rotposdisp = QEPDATA.rotpos + TWOPAI;	
//	else if (QEPDATA.rotpos > TWOPAI)  	QEPDATA.rotposdisp = QEPDATA.rotpos - TWOPAI;	//QEPDATA.rotposdispÓÃÓÚDAÏÔÊ¾
	if 		(QEPDATA.rotposdisp < 0) 	  	QEPDATA.rotposdisp = QEPDATA.rotposdisp + TWOPAI;	
	else if (QEPDATA.rotposdisp > TWOPAI)  	QEPDATA.rotposdisp = QEPDATA.rotposdisp - TWOPAI;	//QEPDATA.rotposdispÓÃÓÚDAÏÔÊ¾ 201005atcpc


//-----------¼ÆËãÍø²à·´±ä»»½Ç¶È--------------------------------------------------------------------
	if(M_ChkT1Direction() == 0)  							//CTR¼õ¼ÆÊý 
    	temp3 = EPwm1Regs.TBPRD + EPwm1Regs.TBCTR;
	else 													//CTRö¼ÆÊý
		temp3 = 3 * EPwm1Regs.TBPRD - EPwm1Regs.TBCTR;

	temp3 = temp3 * 2; 										//±ä»»µ½CAP4µÄÊ±ÖÓÆµÂÊ£º150M

//	temp2 = ECap4Regs.TSCTR;     //problem:²»ÄÜÓÐÐ§¿¹ÍøÆµ¸ÉÈÅ
//	temp2 = temp2 + temp3;									//¹À¼ÆÏÂ´ÎÂö³å·¢Ê±ECap4Regs.TSCTRµÄÖµ		
//	temp  = temp2 * CAP4.radpertb;							//Íø²à´±ä»»½Ç¶È£¬Î´È¡Ä££¬Ô¼£¨0---2PAI£©
	temp  = CAP4.nprtrstheta + (float)temp3 * CAP4.radpertb;//²ÉÓÃ¿¹¸ÉÈÅºó½Ç¶È¼ÆËã·´±ä»»½Ç¶È201005atcpc		
	
	if (temp > TWOPAI)	temp = temp - TWOPAI;				//È¡Ä£2 * PAI
			
	CAP4.npratitheta = temp;								//Íø²à·´±ä»»½Ç¶È
	
//----------¼ÆËã»ú²à·´±ä»»½Ç¶È---------------------------------------------------------------------
//	temp2 = ECap4Regs.TSCTR;           //problem:²»ÄÜÓÐÐ§¿¹ÍøÆµ¸ÉÈÅ
//	temp  = (float)temp2;
//	temp  = temp * CAP4.radpertb + TWOPAI_3;  				//±Ç°¶¨×Ó´ÅÁ´Î»ÖÃrad PAI_2=PAI/2=1.5707963	¶¨×ÓÈÆ×é½Ç½Ó zlquestion
/*	temp  = CAP4.nprtrstheta + TWOPAI_3;					//²ÉÓÃ¿¹¸ÉÈÅºó½Ç¶È¼ÆËã·´±ä»»½Ç¶È201005atcpc		
  													
//	temp2 = EQep2Regs.QPOSCNT;
//	temp1 = (float)temp2;									//QEPÄ£¿éÎ»ÖÃ¼ÆÊýÆ÷µÄÖµ

//	temp1 = 1.53398e-3 * temp1;								//×ª×ÓÏà¶ÔÓÚÖÜÆÚÐÅºÅµÄ»Örad(µç½Ç¶È) //temp1 = POLEPAIRES  * PAI * temp1/(PLSPRVL * 2.0);
	temp1 = 1.5708 * POLEPAIRES * QEPDATA.adsmposcnt / ((float)_SC_PLSPRVL); 			//201005atcpc							

	temp1 = temp1 - QEPDATA.encodpos;						//±Ç°×ª×ÓÊµ¼ÊÎ»ÖÃrad	

	if 		(temp1 < 0) 	   	temp1 = temp1 + TWOPAI;
	else if (temp1 > TWOPAI) 	temp1 = temp1 - TWOPAI;		//µ±Ç°×ª×ÓÎ»ÖÃÈ¡Ä£
		
	temp = temp - temp1; 	 								//µ±Ç°·´±ä»»½Ç¶È
*/	
	temp = CAP4.mprtrstheta; 	 							//µ±Ç°·´±ä»»½Ç¶È 201011BJTULVRT

	if(M_ChkT3Direction() == 0) temp1 = EPwm4Regs.TBPRD + EPwm4Regs.TBCTR;		//CTR¼õ¼ÆÊý  	
	else 						temp1 = 3 * EPwm4Regs.TBPRD - EPwm4Regs.TBCTR;	//CTRÔö¼ÆÊý
		
	temp1 = temp1 * CAP4.omigaslp * 1.333333e-8; 			//Ê£ÓàÊ±¼ä¶ÔÓ¦µÄ½Ç¶ÈÔöÁ¿PWMclk:75M, 1/75.0e6=1.3333e-8
															//²¹³¥½Ç¶Ètemp1·ûºÅËæ×ÅCAP4.omigaslp·ûºÅ±ä»¯
	temp = temp + temp1; 									//·´±ä»»½Ç¶È£¬Î´È¡Ä££¬Ô¼£¨-1.5PAI---2.5PAI£©		

	if		(temp > TWOPAI)		temp = temp - TWOPAI;		
	else if	(temp < 0)		  	temp = temp + TWOPAI;		//È¡Ä£2 * PAI
	if		(temp > TWOPAI)		temp = temp - TWOPAI;		
	else if	(temp < 0)		  	temp = temp + TWOPAI;		//È¡Ä£2 * PAI 201005atcpc

	CAP4.mpratitheta = temp; 								//»ú²à·´±ä»»½Ç¶È

//--------------------¸³Öµ¸ø3/2±ä»»½á¹¹Ìå±äÁ¿-------------------------------------------------------

	TRS_NPR_I.sintheta = sin(CAP4.nprtrstheta);	
	TRS_NPR_I.costheta = cos(CAP4.nprtrstheta);				//Íø²à¶¨Ïò½Ç¶È
	TRS_NGS_U.costheta = TRS_NPR_I.costheta;	
	TRS_NGS_U.sintheta = TRS_NPR_I.sintheta;				//Íø²à¶¨Ïò½Ç¶È
	TRS_NPR_U.sintheta = sin(CAP4.npratitheta);	
	TRS_NPR_U.costheta = cos(CAP4.npratitheta);				//Íø²à¿ØÖÆµçÑ¹½Ç¶È 

	TRS_MPR_I.sintheta = sin(CAP4.mprtrstheta);				//»ú²à¿ØÖÆÓÃ
	TRS_MPR_I.costheta = cos(CAP4.mprtrstheta);		
	TRS_MPR_U.sintheta = sin(CAP4.mpratitheta);				//×ª×Ó¿ØÖÆµçÑ¹½Ç¶È 
	TRS_MPR_U.costheta = cos(CAP4.mpratitheta);		

}

/**************************************************************************************************
** º¯ÊýÃû³Æ: 	MPR_CONTROL
** ¹¦ÄÜÃèÊö:  	»ú²à±äÁ÷Æ÷¿ØÖÆËã·¨
** Êä¡¡Èë:   	
** Êä¡¡³ö: 		 
** ×¢  ÊÍ: 		÷³ö×ª×ÓÎªÕý	
**-------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 		
** ÈÕ¡¡ÆÚ: 	
**-------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:	 
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/
void MPR_CONTROL(void)
{
   float temp_d,temp_q,temp_exi;
//	Uint16 temp_n,temp_ud;

//------------------»ú²àµçÁ÷dqÖµ--------------------------------------------------------------------
/*   	TRS_MPR_I.a     = AD_OUT_MPR_I.a;
   	TRS_MPR_I.b     = AD_OUT_MPR_I.b;
   	TRS_MPR_I.c     = AD_OUT_MPR_I.c;
	Transform_3s_2s_2r(&TRS_MPR_I);
	DataFilter(0.4,&TRS_MPR_I.dflt,TRS_MPR_I.d); 			//»ú²àµçÁ÷·´À¡ÖµÂË²¨£¬ Ts=200us,fh=1.2kHz,ÂËµô¿ª¹ØÆµÂÊ´Î
	DataFilter(0.4,&TRS_MPR_I.qflt,TRS_MPR_I.q); 			//»ú²àµçÁ÷·´À¡ÖµÂË²¨£¬ Ts=200us,fh=1.2kHz,ÂËµô¿ª¹ØÆµÂÊ´Î
*///²»×ªÒÆµ½QepEncodPosÀïÃæ¼ÆËã£¬µÃµ½¿ì¶Á´ÅÁ´ÖµBJTULVRT201204

//----------------ÔËÐÐ»ú²àµçÁ÷»·-------------------------------------------------------------------
	if(M_ChkFlag(SL_MPR_RUNING)!=0)							//»ú²àµ÷½ÚÐèÒªÔËÐÐ
	{	
/*
		PI_MPR_Id.reference      = RUN.mpridrf;
		PI_MPR_Id.feedback       = TRS_MPR_I.dflt; 			//»ú²àdÖáµçÁ÷»·
		PI_Loop(&PI_MPR_Id,PI_PARA_MPRID);   

		PI_MPR_Iq.reference      = RUN.mpriqrf;		  	  
		PI_MPR_Iq.feedback       = TRS_MPR_I.qflt;			//»ú²àqÖáµçÁ÷»·
  		PI_Loop(&PI_MPR_Iq,PI_PARA_MPRIQ);
*/
//-----------Ó¦ÓÃLV´ÅÁ´Ë¥¼õ¿ØÖÆ-BJTULVRT201204 
//			if((_COMMAND2&0x0008)!=0)		//±¸ÓÃ3 201201
//			{
//				PI_MPR_Id.reference    = RUN.mpridrf;
//				PR_M_Id.reference      = RUN.mpridrf;
//			}
//			else
//			{
//				if(M_ChkFlag(SL_LV_PHICON)!=0)
//				{
//					 PI_MPR_Id.reference    = RUN.mpridrf + PHI_DATA_M.Id;		//¶¯Ì¬ÅÁ´µÖÏû
//					 PR_M_Id.reference      = RUN.mpridrf + PHI_DATA_M.Id;		//¶¯Ì¬´ÅÁ´µÖÏûPR				
//				}
//				else 
//				{
					PI_MPR_Id.reference    = RUN.mpridrf;
//					PR_M_Id.reference      = RUN.mpridrf;
//				}
//			}
		
			PI_MPR_Id.feedback     = TRS_MPR_I.dflt; 			//»ú²àdÖáµçÁ÷»·
//			PR_M_Id.feedback       = TRS_MPR_I.dflt; 			//»ú²àdÖáµçÁ÷»·

//			PI_Loop(&PI_MPR_Id,PI_PARA_MPRID);

//			if(M_ChkFlag(SL_LV_SYNCON)!=0 && ((_STDBY9&0x0001)!=0)) //±¸ÓÃ0 ¶¯Ì¬PI²ÎÊýµ÷½Ú
//			{
//				PI_Loop(&PI_MPR_Id,PI_PARA_DYMI);   		
//			}
//			else
//			{
				PI_Loop(&PI_MPR_Id,PI_PARA_MPRID);   
//			}

		
//			if((_COMMAND2&0x0010)!=0)	//±¸ÓÃ4 201201
//			{
//				PI_MPR_Iq.reference    = RUN.mpriqrf;	
//				PR_M_Iq.reference      = RUN.mpriqrf;	
//			}
//			else
//			{
//				if(M_ChkFlag(SL_LV_PHICON)!=0)
//				{
						PI_MPR_Iq.reference    = RUN.mpriqrf + PHI_DATA_M.Iq;		 //¶¯Ì¬´ÅÁ´µÖÏû 
//						PR_M_Iq.reference      = RUN.mpriqrf + PHI_DATA_M.Iq;		 //¶¯Ì¬´ÅÁ´µÖÏû 
//				}
//				else	
//				{
					PI_MPR_Iq.reference    = RUN.mpriqrf;		  
//					PR_M_Iq.reference      = RUN.mpriqrf;		  
//				}
//			}	  	  
		
			PI_MPR_Iq.feedback     = TRS_MPR_I.qflt;			//»ú²àqÖáµçÁ÷»·
//			PR_M_Iq.feedback       = TRS_MPR_I.qflt;			//»ú²àqÖáµçÁ÷»·

//  			PI_Loop(&PI_MPR_Iq,PI_PARA_MPRIQ);	  	  	  
		

//			if(M_ChkFlag(SL_LV_SYNCON)!=0 && ((_STDBY9&0x0001)!=0)) //¶¯Ì¬PI²ÎÊýµ÷½Ú
//			{
//  				PI_Loop(&PI_MPR_Iq,PI_PARA_DYMI);	  	  	  	
//			}
//			else
//			{
  				PI_Loop(&PI_MPR_Iq,PI_PARA_MPRIQ);	  	  	  
//			} 
   
/*			if((_STDBY9&0x0008)!=0)  //116±¸ÓÃ3 Ó¦ÓÃPRÊä³ö½á¹û20120315
			{
				PR3_Loop(&PR_M_Id,PR_PARA_MID);   		
				PR3_Loop(&PR_M_Iq,PR_PARA_MIQ);   		
	
				PIR_M_IdO = PI_MPR_Id.out + PR_M_Id.out;
				PIR_M_IqO = PI_MPR_Iq.out + PR_M_Iq.out;
			}
			else
			{
				PIR_M_IdO = PI_MPR_Id.out;
				PIR_M_IqO = PI_MPR_Iq.out;			
			} 
*///20121103

	}
   
//--------------MPRÊä³öµçÑ¹Ëã¨---------------------------------------------------------------------

//   DM_imrd = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * CAP4.omigasyn));	//20100121
//   DM_imrq = - (TRS_NGS_U.qflt * SQRT3 * STAROTRTO / (MPR_Lm * CAP4.omigasyn));  //ÍøÑ¹qÖáÇ°À¡½âñîÏî
/*	DM_imrd  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));  		//¸ººÅ:ÍøÑ¹edÓë»ú²àdÖáÏà²î180¶È             
    temp_ud = (Uint16)(TRS_NGS_U.dflt * SQRT3 / SQRT2);								//ÍøÑ¹ÏßµçÑ¹ÓÐÐ§Öµ 690V
    if     (temp_ud < 540) 		temp_ud = 540;
    else if(temp_ud > 800) 	    temp_ud = 800;
	temp_n = (temp_ud - 540) * 127 / 260;
    temp_exi = TAB_EXI[temp_n];
    DM_imrd =  DM_imrd * temp_exi;          										//²é±íµ÷ÕûÏµÊý,ÓÉ²Ù×÷Æ÷¸ø¶¨ 

*/
//--------------MPRÊä³öµçÑ¹¼ÆËãBJTULVRT20121103--------------------------------------------------------------------
//	if(M_ChkFlag(SL_MPR_SYNOK)!=0 || (_SC_MSTDBY!=0))  //201201	
	if(M_ChkFlag(SL_MPR_SYNOK)!=0)  //20121103	
		DM_imrd =  - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * CAP4.omigasyn));	////201112fluxobs
	else
		DM_imrd =  - RUN.mpridrf / RUN.mpridrf_g * (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * CAP4.omigasyn));	//201112fluxobs
/*
	temp_d = - PI_MPR_Id.out + SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Iq.feedback;	//½âñîÏî¼ÆËã

//	temp_d = temp_d	- CAP4.omigaslp * MPR_Lr * DM_imrq;								//ÍøÑ¹qÖáÇ°À¡½âñîÏî

  	temp_q = - PI_MPR_Iq.out - SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Id.feedback;	//½âñîÏî¼ÆËã
	temp_q = temp_q	- CAP4.omigaslp * MPR_Lr * DM_imrd;                               
*/
	if((M_ChkFlag(SL_UNBALANCE)==0) && (M_ChkFlag(SL_LV_STATE)!=0) && (NGS_Udq_p_lv < (0.8 * NGS_Udq_p_ex)))		  //20130223
	{
		temp_d = - PI_MPR_Id.out + SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Iq.feedback + TRS_MPR_I.dflt * _stdby02;	//½âñîÏî¼ÆËã

	  	temp_q = - PI_MPR_Iq.out - SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Id.feedback;	//½âñîÏî¼ÆËã
		temp_q = temp_q	- CAP4.omigaslp * MPR_Lr * DM_imrd + TRS_MPR_I.qflt * _stdby02;

	}
	else
	{
		temp_d = - PI_MPR_Id.out + SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Iq.feedback;	//½âñîÏî¼ÆËã

	 	temp_q = - PI_MPR_Iq.out - SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Id.feedback;	//½âñîÏî¼ÆËã
		temp_q = temp_q	- CAP4.omigaslp * MPR_Lr * DM_imrd;

	}


//	temp_d = temp_d - MPR_Rr * PI_MPR_Id.feedback;
//	temp_q = temp_q - MPR_Rr * PI_MPR_Iq.feedback;									//×ª×Óµç×èÑ¹½µ
  
   	TRS_MPR_U.d = temp_d / STAROTRTO2;
	TRS_MPR_U.q = temp_q / STAROTRTO2;
/*    
    temp1 = (float)_SC_MPR_UD;
	temp2 = (float)_SC_MPR_UQ;

	if     (TRS_MPR_U.d >  temp1)     TRS_MPR_U.d =  temp1;	    //jutsttest
	else if(TRS_MPR_U.d < -temp1)     TRS_MPR_U.d = -temp1;	    //jutsttest
	if     (TRS_MPR_U.q >  temp2)     TRS_MPR_U.q =  temp2;	    //jutsttest
	else if(TRS_MPR_U.q < -temp2)     TRS_MPR_U.q = -temp2;	    //jutsttest
*/
	Anti_Transform_2r_2s(&TRS_MPR_U);							//×ø±ê·´±ä»»µ½Á½Ïà¾²Ö¹Ïµ

//------------SVMÂö¿í¼ÆËãºÍÂö³å·¢Éú-----------------------------------------------------------------	
	SVPWM_MPR(TRS_MPR_U.alfa,TRS_MPR_U.beta);					//Âö³å·¢Éú

//------------TeÊµ¼Ê×ª¾Ø·´À¡Öµ¼ÆËã-----------------------------------------------------------------	
//	DataFilter(0.8,&DIP_MPR_I.qflt,TRS_MPR_I.q); 					//¶¨×Ó²à¹¦ÂÊ¼ÆËã,»ú²àqµçÁ÷£¬ Ts=200us,fh=Hz
	DataFilter(0.8,&DIP_MPR_I.qflt,TRS_MPR_I.qflt); 					//¶¨×Ó²à¹¦ÂÊ¼ÆËã,»ú²àqµçÁ÷£¬ Ts=200us,fh=Hz
	Te_feedback = - (1.5 * SQRT3 * POLEPAIRES * MPR_Lm  / (314.15926 * STAROTRTO * MPR_Ls)) * DIP_NPR_U.dflt *  DIP_MPR_I.qflt;     // Te=-1.5*np*(Us/w1)*(Irq/K)*(Lm/Ls),MPR_iÎª¸ºÖµ
//	if(Te_feedback<=0)	Te_feedback=0;   							//20090817	20121129È¡Ïûatsp
//-----BJTULVRT20121103
	Te_instant  = - (1.5 * SQRT3 * POLEPAIRES * MPR_Lm  / (314.15926 * STAROTRTO * MPR_Ls)) * TRS_NGS_U.dflt *  TRS_MPR_I.qflt;     // Te=-1.5*np*(Us/w1)*(Irq/K)*(Lm/Ls),MPR_iÎª¸ºÖµ

}  


/**************************************************************************************************
** º¯ÊýÃû³Æ: 	NPR_CONTROL
** ¹¦ÄÜÃèÊö:  	Íø²à±äÁ÷Æ÷¿ØÖÆËã·¨
** Êä¡¡Èë:   	
** Êä¡¡³ö: 		 
** ×¢  ÊÍ: 		µçÁ÷Á÷ÏòµçÍøÎªÕý	
**-------------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 		
** ÈÕ¡¡ÆÚ: 		
**-------------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÕß:
** ÈÕÆÚ:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void NPR_CONTROL(void)
{ 
//	float temp_d,temp_q;

//-----------¼ÆËãÍø²à±äÁ÷Æ÷µçÁ÷dqÖµ-----------------------------------------------------------------
	TRS_NPR_I.a = AD_OUT_NPR_I.a;  							//»ñÈ¡Íø²à±äÁ÷Æ÷µçÁ÷
	TRS_NPR_I.b = AD_OUT_NPR_I.b;
	TRS_NPR_I.c = AD_OUT_NPR_I.c;
	Transform_3s_2s_2r(&TRS_NPR_I);							//×ø±ê±ä»»
	DataFilter(0.4,&TRS_NPR_I.dflt,TRS_NPR_I.d); 			//Íø²àµçÁ÷·´À¡ÖµÂË²¨£¬ Ts=200us,fh=1.2kHz,ÂËµô¿ª¹ØÆµÂÊ´Î
	DataFilter(0.4,&TRS_NPR_I.qflt,TRS_NPR_I.q); 			//Íø²àµçÁ÷´À¡ÖµÂË²¨£¬ Ts=200us,fh=1.2kHz£¬20090615¸Ä

//----------¼ÆËãÍøÑ¹dqÖµ----------------------------------------------------------------------------
	TRS_NGS_U.a = AD_OUT_NGS_U.a;							//»ñÈ¡µçÍøµçÑ¹
	TRS_NGS_U.b = AD_OUT_NGS_U.b;
	TRS_NGS_U.c = AD_OUT_NGS_U.c;
	Transform_3s_2s_2r(&TRS_NGS_U);							//×ø±ê±ä»»

	if(TRS_NGS_U.q > 200)			TRS_NGS_U.q = 200;		//20130228
	else if(TRS_NGS_U.q < -200)		TRS_NGS_U.q = -200;		//20130228

	DataFilter(0.4,&TRS_NGS_U.dflt,TRS_NGS_U.d); 			//ÍøÑ¹·´ÖµÂË²¨£¬Ts=200us,fh=1.2kHz 20090608change to ok
	DataFilter(0.4,&TRS_NGS_U.qflt,TRS_NGS_U.q); 			//ÍøÑ¹·´À¡µÂË²¨£¬Ts=200us,fh=1.2kHz 20090608change to ok
//	DataFilter(0.1,&TRS_NGS_U.dflt2,TRS_NGS_U.d); 			//ÍøÑ¹·´À¡ÖµÂË²¨£¬Ts=200us,fh=7.9kHz,126us,Îª¼à²âÍøµø£¬20091026


	if(M_ChkFlag(SL_NPR_RUNING)!=0)							//Íø²àPIÔËÐÐ¿ØÖÆÌõ¼þ
	{

//----------ÔËÐÐµçÑ¹Íâ»·----------------------------------------------------------------------------
	  if(M_ChkFlag(SL_STEADYGV)==0)  						//VdcÃ»ÓÐÎÈ¶¨£¬ÇÒ²ÉÓÃÎÈÌ¬PI²ÎÊý
	  {

		PI_NPR_U.reference     = - RUN.urf;   				//»ñÈ¡ÖÐ¼äµçÑ¹Ö¸Áî
		PI_NPR_U.feedback      = - AD_OUT_UDC;				//»ñÈ¡ÖÐäµçÑ¹·´À¡Öµ
    	PI_Loop(&PI_NPR_U,PI_PARA_DYNU);
      }	
	  else                                                   //²ÉÓÃÎÈÌ¬PI²ÎÊý
	  {
		PI_NPR_U.reference     = - RUN.urf;   				//»ñÈ¡ÖÐ¼äµçÑ¹Ö¸Áî
		PI_NPR_U.feedback      = - AD_OUT_UDC;				//»ñÈ¡ÖÐ¼äµçÑ¹·´À¡Öµ
    	PI_Loop(&PI_NPR_U,PI_PARA_NPRU); 
      }
//--------¶Ô35%-2ph¹¤¿ö×öµçÑ¹»·ÏÞ·ù20130222--------
		if(M_ChkFlag(SL_LV_STATE)!=0)
		{
			if(M_ChkFlag(SL_UNBALANCE)!=0 && (NGS_Udq_epsilon < 50) && M_ChkFlag(SL_LV_SCRRUNING)==0)	//³ý20%-2ph£¬ÆäÓà2ph¶¼ÏÞÖµ
			{
				if(PI_NPR_U.out > lv_ctrl6)				PI_NPR_U.out = lv_ctrl6;	//20130223
				else if(PI_NPR_U.out < -lv_ctrl6)		PI_NPR_U.out = -lv_ctrl6;	//20130223
			}
		}

//---------ÔËÐÐdÖáµçÁ÷»·----------------------------------------------------------------------------
   		PI_NPR_Id.reference      = PI_NPR_U.out; 			//»ñÈ¡dÖáµçÁ÷¸Áî£¬×¢ÒâÕý¸ººÅ
		PI_NPR_Id.feedback       = TRS_NPR_I.dflt;			//»ñÈ¡dÖáµçÁ÷·´À¡ÂË²¨Öµ
		PI_Loop(&PI_NPR_Id,PI_PARA_NPRID);

//---------ÔËÐÐqÖáµçÁ÷»·----------------------------------------------------------------------------
		PI_NPR_Iq.reference      = RUN.npriqrf; 			//qÖáµçÁ÷Ö¸Áî
		PI_NPR_Iq.feedback       = TRS_NPR_I.qflt;	  		//»ñÈ¡qáµçÁ÷·´À¡ÂË²¨Öµ
		PI_Loop(&PI_NPR_Iq,PI_PARA_NPRIQ);	  	  
	}

//---------ÆËãÐý×ªÏµÏÂNPR¿ØÖÆµçÑ¹------------------------------------------------------------------

//	TRS_NPR_U.d = PI_NPR_Id.out  + TRS_NGS_U.dflt - CAP4.omigasyn * NPR_L * PI_NPR_Iq.feedback;
//	TRS_NPR_U.q = PI_NPR_Iq.out  + TRS_NGS_U.qflt + CAP4.omigasyn * NPR_L * PI_NPR_Id.feedback;
//	if((_STDBY9&0x0020)==0)		  //116.5 È¡ÏûÇ°À¡ÂË²¨2013022
//	{
		TRS_NPR_U.d = PI_NPR_Id.out  + TRS_NGS_U.dflt - CAP4.omigasyn * NPR_L * PI_NPR_Iq.feedback;
		TRS_NPR_U.q = PI_NPR_Iq.out  + TRS_NGS_U.qflt + CAP4.omigasyn * NPR_L * PI_NPR_Id.feedback;
//	}
//	else
//	{
//		TRS_NPR_U.d = PI_NPR_Id.out  + TRS_NGS_U.d - CAP4.omigasyn * NPR_L * PI_NPR_Iq.feedback;
//		TRS_NPR_U.q = PI_NPR_Iq.out  + TRS_NGS_U.q + CAP4.omigasyn * NPR_L * PI_NPR_Id.feedback;
//	}
	
//  	TRS_NPR_U.d = temp_d;									//201007BJTULVRT
//  	TRS_NPR_U.q = temp_q;

	Anti_Transform_2r_2s(&TRS_NPR_U); 						//antitransform to static axis

//---------SVMÂö¿í¼ÆËã-----------------------------------------------------------------------------
	SVPWM_NPR(TRS_NPR_U.alfa,TRS_NPR_U.beta);				//Âö³å·¢Éú

//--20121103---Íø²àÓÐ¹¦¹¦ÂÊ¼ÆËã(3Ïà)-----------------------------------------------------------------	
	DataFilter(0.99,&DIP_NPR_U.dflt,TRS_NGS_U.dpll); 			//¶¨×Ó²à¹¦ÂÊ¼ÆËã,ÍøÑ¹ed£¬ Ts=200us,Tr=250ms

//------------Íø²àÓÐ¹¦¹¦ÂÊ¼ÆËã(3Ïà)-----------------------------------------------------------------	
/*	DataFilter(0.99,&DIP_NPR_I.dflt,TRS_NPR_I.d); 			//¶¨×Ó²à¹¦ÂÊ¼ÆËã,ÍøÑ¹ed£¬ Ts=200us,fh=88Hz
	DataFilter(0.99,&DIP_NPR_I.qflt,TRS_NPR_I.q); 			//¶¨×Ó²à¹¦ÂÊ¼ÆËã,»ú²àqµçÁ÷£¬ Ts=200us,fh=88Hz
	DataFilter(0.99,&DIP_NPR_U.qflt,TRS_NGS_U.q); 			//¶¨×Ó²à¹¦ÂÊ¼ÆËã,»ú²àqµçÁ÷£¬ Ts=200us,fh=88Hz
	DataFilter(0.995,&DIP_NPR_U.dflt,TRS_NGS_U.d); 			//¶¨×Ó²à¹¦ÂÊ¼ÆËã,ÍøÑ¹ed£¬ Ts=200us,Tr=250ms

	Pnactive   = 1.5 * (DIP_NPR_U.dflt * DIP_NPR_I.dflt + DIP_NPR_U.qflt * DIP_NPR_I.qflt);   	// Pnactive=3*Ud*Id/(squt2*squt2)
	Pnreactive = 1.5 * (DIP_NPR_U.qflt * DIP_NPR_I.dflt - DIP_NPR_U.dflt * DIP_NPR_I.qflt);   	// Pnactive=3*Ud*Id/(squt2*squt2)
*///20121103
}


/**************************************************************************************************
** Ãû³Æ: SysCtrl
** ¹¦ÄÜÃèÊö: 1.5MW±äÁ÷Æ÷ÏµÍ³Âß¼­¿ØÖÆ-Ö÷³ÌÐò 	
** Êä¡¡Èë:   	
** Êä¡¡³ö: 		 
** ×¢  ÊÍ: 	systest		
**-------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 	 
** ÈÕ¡¡ÆÚ: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void SysCtrl(void)         
{
	if(M_ChkFlag(SL_SERIESTOPING)!=0)							//ÕýÔÚÑÏÖØ¹ÊÕÏÍ£»úÖÐ
	{
		SER_STOP();												//Ö´ÐÐÑÏÖØ¹ÊÕÏÍ£»ú,ÏÈ¶ÏÖ÷¶Ï
	}
	else if(M_ChkFlag(SL_SERIESTOP)!=0)							//´æÔÚÑÏÖØ¹ÊÕÏ
	{
		M_SetFlag(SL_SERIESTOPING);								//ÖÃ ÕýÔÚÑÏÖØ¹ÊÕÏÍ£»úÖÐ ±êÖ¾
	}	
	else if(M_ChkFlag(SL_ERRSTOPING)!=0)						//ÕýÔÚÒ»°ã¹ÊÕÏÍ£»úÖÐ
	{
		ERR_STOP();												//Ö´ÐÐÒ»°ã¹ÊÕÏÍ£»ú,Ôö¼ÓÊÖ¶¯¶Ï¿ªCB¿ØÖÆ201005atcpc
	}
	else if(M_ChkFlag(SL_ERRSTOP)!=0)							//ÅÐ¶ÏÊÇ·ñ´æÔÚÒ»°ã¹ÊÕÏ
	{
		M_SetFlag(SL_ERRSTOPING);								//ÖÃ ÕýÔÚÒ»°ã¹ÊÕÏÍ£»úÖÐ ±êÖ¾
	}
	else if(M_ChkFlag(SL_NORSTOPING)!=0)						//ÕýÔÚÕý³£Í£»úÖÐ
	{
		NOR_STOP();												//Ö´ÐÐÕý³£Í£»ú,Ôö¼ÓÊÖ¶¯¶Ï¿ªCB¿ØÖÆ201005atcpc
	}
	else if(M_ChkFlag(SL_OCS_EIN)==0)							//ÊÇ·ñ½ÓÊÕµ½Ö÷¿ØBIT0±ÕºÏÖ÷¶ÏÖ¸Áî
	{
		M_SetFlag(SL_NORSTOPING);								//ÖÃ ÕýÔÚÕý³£Í£»úÖÐ ±êÖ¾
	}
	else if(M_ChkFlag(SL_CBCLOSED)==0)							//Ö÷¶Ï±ÕºÏÊÇ·ñÍê³É
	{
		CB_ON();												//Ö´ÐÐÖ÷¶Ï±ÕºÏ²Ù×÷
	}
	else if(M_ChkFlag(SL_OCS_SYSRUN)==0)						//ÊÇ·ñ½ÓÊÕµ½Ö÷¿ØBIT2Í¬²½²¢ÍøÖ¸Áî
	{
		M_SetFlag(SL_NORSTOPING);								//ÖÃ ÕýÔÚÕý³£Í£»úÖÐ ±êÖ¾
	}
	else if(M_ChkFlag(SL_MPR_SYNOK)==0)							//¶¨×Ó²¢ÍøÊÇ·ñÒÑÍê³É
	{
		RUN_SYN();												//Ö´ÐÐÔ¤äµçµ½Í¬²½²¢Íø²Ù×÷
	}
	else														//Ö´ÐÐ¶¨×Ó²¢ÍøºóÔËÐÐ¿ØÖÆ
	{
		if((AD_OUT_UDC>(_URF - 20))&&(AD_OUT_UDC<(_URF + 20))) 	//ÔËÐÐÖÐÅÐ¶ÏÖ±Á÷µçÑ¹ÊÇ·ñÎÈ¶¨+-20V  
				M_SetFlag(SL_STEADYFB);		
		else 	M_ClrFlag(SL_STEADYFB);
/*
		M_SetFlag(CL_CBON);
		M_SetFlag(CL_CBLVC);
		M_SetFlag(CL_CBFCON);
		M_SetFlag(CL_MAINK);
		M_SetFlag(CL_STATORK);
		M_SetFlag(SL_OCS_NPRSTART);
		M_SetFlag(SL_NPR_START);
		M_SetFlag(SL_MPR_START);
*/
	}
}

/**************************************************************************************************
** Ãû³Æ: RUN_SYN
** ¹¦ÄÜÃèö: 1.5MW±äÁ÷Æ÷ÏµÍ³Âß¼­¿ØÖÆ-Í¬²½²¢Íø	
** Êä¡¡Èë:   	
** Êä¡¡³ö: 		 
** ×¢  ÊÍ: 			
**-------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 	 
** ÈÕ¡¡ÆÚ: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void RUN_SYN(void)         
{ 
	if(M_ChkFlag(SL_SYN_FH)!=0)										//FH=²¢ÍøÍê³É
	{
		if(M_ChkFlag(SL_IN1_STATORK)!=0)							//ÅÐ¶Ï¶¨×Ó½Ó´¥Æ÷ÊÇ·ñÒÑ±ÕºÏ
		{		
			M_SetFlag(SL_MPR_SYNOK);								//ÖÃ¶¨×Ó²¢ÍøÍê³É
			M_ClrFlag(SL_SYN_FH);									//Çå²½Öè±êÖ¾Î»
			M_ClrFlag(SL_SYN_S3);									//Çå²½Öè±êÖ¾Î»
			M_ClrFlag(SL_SYN_S2);									//Çå²½Öè±êÖ¾Î»
			M_ClrFlag(SL_SYN_S1);									//Çå²½Öè±êÖ¾Î»
		}	
	}
	else if(M_ChkFlag(SL_SYN_S3)!=0)								//S3=Æô¶¯»ú²à¶¨×Ó²¢Íø
	{
		M_SetFlag(SL_MPR_START);									//»ú²à±äÁ÷Æ÷ÔËÐÐ

		if(MEAN_DATA.uab_d <= _SC_UDSTAC && MEAN_DATA.ubc_d <= _SC_UDSTAC)    //¶¨×Ó·ûºÏ²¢ÍøÌõ¼þ    					//¶¨×Ó½Ó´¥Æ÷Ç°ºó°ë²¨Æ½¾ùÖµ²îÔÚ40VÒÔÄÚ
		{
			M_SetFlag(SL_UDSTACOK);									//ÖÃ ¶¨×Ó´ïµ½²¢ÍøÌõ¼þ ±êÖ¾

		    if(M_ChkFlag(SL_OCS_OPENSTAC)==0)	
		    {														//Íâ²¿¿ØÖÆ£¬Îª0²ÅÔÊÐí±ÕºÏSTAC
		      	M_SetFlag(CL_STATORK);  							//ÉÏÎ»»úÔÊÐí²¢Íø,±ÕºÏ¶¨×Ó½Ó´¥Æ÷
				M_SetFlag(SL_SYN_FH);								//ÖÃ²½Öè±êÖ¾Î»
				MAIN_LOOP.cnt_synrun=0;           					//Çå¼ÆÊýÆ÷		
		    }                             					
			else	M_ClrFlag(CL_STATORK);							//ÉÏÎ»»ú½ûÖ¹²¢Íø,¶Ï¿ª¶¨×Ó½Ó´¥Æ÷	        
		}
		else	M_ClrFlag(SL_UDSTACOK);								//Çå ¶¨×Ó´ïµ½²¢ÍøÌõ¼þ ±êÖ¾
	}
	else if(M_ChkFlag(SL_SYN_S2)!=0)								//S2=Æô¶¯Íø²à
	{
		M_SetFlag(SL_OCS_NPRSTART);									//Æô¯DSP_PWMÅäÖÃ

		if(M_ChkFlag(SL_CONFIGPWM)!=0)   M_SetFlag(SL_NPR_START);	//pwmdriveÅäÖÃPWMÍê³É,ø²à±Á÷Æ÷ÔËÐÐ		  
		else   	                         M_ClrFlag(SL_NPR_START); 

		if(AD_OUT_UDC>(_URF - 20) && AD_OUT_UDC<(_URF + 20) && M_ChkFlag(SL_NPR_PWMOUT)!=0) //Íø²àÂö³åÒÑ·¢³öÇÒVdcÎÈ¶¨
   		{
   			if(M_ChkCounter(MAIN_LOOP.cnt_synrun,DELAY_STEADYFB)>=0)
			{
				M_SetFlag(SL_STEADYFB);          					//ÖÃVdcÎÈ¶¨
				M_SetFlag(SL_SYN_S3);								//ÖÃ²½Öè±êÖ¾Î»
				MAIN_LOOP.cnt_synrun=0;           					//Çå¼ÆÊýÆ÷		
			}
   		}
		else
		{
			M_ClrFlag(SL_STEADYFB);          						//ÇåVdcÎÈ¶¨
			MAIN_LOOP.cnt_synrun=0;           						//å¼ÆÊýÆ÷		
		}	
	}
	else if(M_ChkFlag(SL_SYN_S1)!=0)								//S1=Ô¤³äµç
	{
		if( AD_OUT_UDC>=_SC_VDCON && M_ChkCounter(MAIN_LOOP.cnt_synrun,DELAY_PRECOK)>=0) //8sÇÒVdc>700V 
		{
			M_SetFlag(CL_MAINK);        							//Ô¤³äµçOK£¬±ÕºÏÖ÷½Ó´¥Æ÷ 
      	
	      	if(M_ChkFlag(SL_IN1_MIANK)!=0 && M_ChkFlag(SL_IN1_MIANFILTER)!=0 && AD_OUT_UDC>=870)  //Ö÷½Ó´¥Æ÷ºÍÂË²¨Æ÷¾ù±ÕºÏ
			{
				M_ClrFlag(CL_PRE);          	 					//¶Ï¿ªÔ¤³äµç½Ó´¥Æ÷
		 		M_SetFlag(SL_CHARGEOK);               				//Ô¤³äµçÍê³É         
				M_SetFlag(SL_FAN_WORK);								//·ç»úÔËÐÐ
				M_SetFlag(SL_SYN_S2);								//ÖÃ²½Öè±êÖ¾Î»
				MAIN_LOOP.cnt_synrun=0;           					//Çå¼ÆÊýÆ÷	
			}
		}
	}
	else															//S0=ºÏÔ¤³äµç½Ó´¥Æ÷
	{
      	if(M_ChkFlag(SL_SPEED_IN_RANGE)!=0)     					//×ªËÙÔÚ·¶Î§ÄÚ
		{
			M_SetFlag(CL_PRE);          	 						//±ÕºÏ¤³äµç½Ó´¥Æ÷
			M_SetFlag(SL_SYN_S1);									//ÖÃ²½Öè±êÖ¾Î»
			MAIN_LOOP.cnt_synrun=0;           						//Çå¼ÆÊýÆ÷	
		}
	}
}

/**************************************************************************************************
** Ãû³Æ: NOR_STOP
** ¹¦ÄÜÃèÊö: 1.5MW±äÁ÷Æ÷ÏµÍ³Âß¼­¿ØÖÆ-Õý³£Í£»ú
** Êä¡¡Èë:   	
** Êä¡¡³ö: 		 
** ×¢  ÊÍ: 			
**-------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 	 
** ÈÕ¡¡ÆÚ: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void NOR_STOP(void)         
{ 
	if(M_ChkFlag(SL_NORSTOP_FH)!=0)									//FH=¶Ï¿ªÖ÷½Ó´¥Æ÷
	{
		M_ClrFlag(CL_MAINK);    		  							//¶ÏÖ÷½Ó´¥Æ÷ºÍÂË²¨Æ÷
		M_ClrFlag(CL_PRE); 											//ÔÙ¶ÏÔ¤³äµç½Ó´¥Æ÷
	   	M_ClrFlag(SL_CHARGEOK);          							//ÇåÔ¤³äµçÍê³É

		if(M_ChkFlag(SL_IN1_MIANK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)==0)
		{
			M_ClrFlag(SL_FAN_WORK);									//Í£Ö¹·ç»úÔËÐÐ
			if(M_ChkFlag(SL_PCOCS_CBOFF)!=0 && M_ChkFlag(SL_OCS_EIN)==0)//PCÊÖ¶¯¿ØÖÆ¶Ï¿ªCB 201005atcpc
			{
			   	M_ClrFlag(CL_CBON);    		     					//ÇåÖ÷¶ÏºÏÕ¢Ö¸Áî
			   	M_ClrFlag(CL_CBLVC);         						//ÇåÇ·Ñ¹Á´¶Ï¿ª£¬Ö÷¶Ï¶Ï¿ª 
			   	M_ClrFlag(CL_CBFCON);            					//¶Ï¿ª¹ÊÕÏÁ´
				M_ClrFlag(SL_CBCLOSED);			 					//ÇåÖ÷¶Ï±ÕºÏÍê³É
				M_ClrFlag(SL_PCOCS_CBOFF);            				//Çå¶Ï¿ªÖ¸Áî
				M_ClrFlag(SL_CBON_FH);								//ÇåCBON²½Öè±êÖ¾Î»
			} 
/*		   	M_ClrFlag(SL_NORSTOPING);          						//Çå ÕýÔÚÕý³£Í£»úÖÐ ±êÖ¾
			M_ClrFlag(SL_NORSTOP_FH);								//Çå²½Öè±êÖ¾Î»
			M_ClrFlag(SL_NORSTOP_S2);								//Çå²½Öè±êÖ¾Î»
			M_ClrFlag(SL_NORSTOP_S1);								//Çå²½Öè±êÖ¾Î»
*/
//--------20120321--------
			if(M_ChkFlag(SL_SYSOFF)==0)
			{
			   	M_ClrFlag(SL_NORSTOPING);          						//Çå ÕýÔÚÕý³£Í£»úÖÐ ±êÖ¾
				M_ClrFlag(SL_NORSTOP_FH);								//Çå²½Öè±êÖ¾Î»
				M_ClrFlag(SL_NORSTOP_S2);								//Çå²½Öè±êÖ¾Î»
				M_ClrFlag(SL_NORSTOP_S1);								//Çå²½Öè±êÖ¾Î»
			}
//------------------------

		}	
	}
	else if(M_ChkFlag(SL_NORSTOP_S2)!=0)							//S2=Í£Íø²à
	{
		M_ClrFlag(SL_NPR_START);									//ÇåÍø²àÂö³åÖ¸Áî
		M_ClrFlag(SL_OCS_NPRSTART);									//ÇåÍø²àÔËÐÐÖ¸Áî

		if(M_ChkFlag(SL_NPR_PWMOUT)==0)								//Íø²àÂö³åÒÑÍ£Ö¹
		{
			M_ClrFlag(SL_STEADYFB);          						//ÇåVdcÎÈ¶¨
			M_SetFlag(SL_NORSTOP_FH);								//ÖÃ²½Öè±êÖ¾Î»
			MAIN_LOOP.cnt_norstop=0;           						//Çå¼ÆÊýÆ÷	
		}
	}
	else if(M_ChkFlag(SL_NORSTOP_S1)!=0)							//S1=¶Ï¿ª¶¨Ó½Ó´¥Æ÷Í£»ú²à
	{        
		if(M_ChkFlag(SL_IN1_STATORK)==0)							//¶¨×Ó½Ó´¥Æ÷¶Ï¿ª
		{
			M_ClrFlag(SL_MPR_SYNOK);								//Çå¶¨×Ó²¢ÍøÍê³É,ÓëRUNCtrlÓÐ¹Ø,²»ÒªÇáÒ×ÐÞ¸Ä		
			M_ClrFlag(SL_UDSTACOK);									//Çå ¶¨×Ó´ïµ½²¢ÍøÌõ¼þ ±êÖ¾
			
			if(M_ChkFlag(SL_MPR_PWMOUT)==0)							//»ú²àÂö³åÍ£Ö¹
			{
				M_SetFlag(SL_NORSTOP_S2);							//ÖÃ²½Öè±êÖ¾Î»
				MAIN_LOOP.cnt_norstop=0;           					//Çå¼ÆÊýÆ÷	
			}
		}      								
	}
	else															//S0=Í£»ú²àÂö³å
	{
		M_ClrFlag(SL_MPR_START);									//Çå»ú²àÂö³åÖ¸Áî,»ú²àÓÐ¹¦½µµÍ

        if(M_ChkFlag(SL_STACKEEPON)==0)	  
        {
        	M_ClrFlag(CL_STATORK);    								//´ïµ½¶¨×Ó¶Ï¿ªÌõ¼þ, ¶Ï¿ª¶¨×Ó½Ó´¥Æ÷
			M_SetFlag(SL_NORSTOP_S1);								//ÖÃ²½Öè±êÖ¾Î»
			MAIN_LOOP.cnt_norstop=0;           						//Çå¼ÆÊýÆ÷	
        }	

		M_ClrFlag(SL_SYN_FH);										//ÇåSYN²½Öè±êÖ¾Î»
		M_ClrFlag(SL_SYN_S3);										//ÇåSYN²½Öè±êÖ¾Î»
		M_ClrFlag(SL_SYN_S2);										//ÇåSYN²½Öè±êÖ¾Î»
		M_ClrFlag(SL_SYN_S1);										//ÇåSYN²½Öè±êÖ¾Î»	
	}
}

/**************************************************************************************************
** Ãû³Æ: SER_STOP
** ¹¦ÄÜÃèÊö: 1.5MW±äÁ÷Æ÷ÏµÍ³Âß¼­¿ØÖÆ-ÑÏÖØ¹ÊÕÏÍ£»ú 	
** Êä¡¡Èë:   	
** Êä¡¡³ö: 		 
** ×¢  ÊÍ: 			
**-------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 	 
** ÈÕ¡¡ÆÚ: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void SER_STOP(void)         
{ 
	if(M_ChkFlag(SL_SERSTOP_FH)!=0)
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_serstop,DELAY_SERSTOP)>0)		//2s Ö÷¶Ï¶Ï¿ªÑÓÊ±ºóÔÙ¶Ï¶¨×Ó½Ó´¥Æ÷ºÍÖ÷½Ó´¥Æ÷
		{
			M_ClrFlag(CL_MAINK);    		  						//¶ÏÖ÷½Ó´¥Æ÷ºÍÂË²¨÷
		   	M_ClrFlag(CL_PRE); 										//Ù¶ÏÔ¤³äµç½Ó´¥Æ÷
		   	M_ClrFlag(CL_STATORK);    	  							//¶Ï¶¨×Ó½Ó´¥Æ÷	
		   	M_ClrFlag(SL_CHARGEOK);          						//ÇåÔ¤³äµçÍê³É
			M_ClrFlag(SL_FAN_WORK);									//Í£Ö¹·ç»úÔËÐÐ
		   	M_ClrFlag(SL_MPR_SYNOK);          						//Çå¶¨×Ó²¢ÍøÍê³É
		   	M_ClrFlag(SL_UDSTACOK);          						//Çå¶¨×ÓÂú×ã²¢ÍøÌõ¼þ
		
			if(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(SL_IN1_MIANK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)==0 && M_ChkFlag(SL_IN1_STATORK)==0)
			{
				M_ClrFlag(SL_SERSTOP_FH);							//Çå²½Öè±êÖ¾Î»
				M_ClrFlag(SL_SERIESTOPING);							//Çå ÕýÔÚÑÏÖØ¹ÊÕÏÍ£»úÖÐ ±êÖ¾
			}
		}
	}
	else
	{
	   	M_ClrFlag(CL_CBON);    		     							//ÇåÖ÷¶ÏºÏÕ¢Ö¸Áî
	   	M_ClrFlag(CL_CBLVC);         								//ÇåÇ·Ñ¹Á´¶Ï¿ª£¬Ö÷¶Ï¶Ï¿ª 
	   	M_ClrFlag(CL_CBFCON);            							//¶Ï¿ª¹ÊÕÏÁ´
	  	M_ClrFlag(SL_OCS_NPRSTART);         						//ÇåÍø²àÔËÐÐÖ¸Áî
	  	M_ClrFlag(SL_OCS_EIN);         								//ÇåÖ÷¶Ï±ÕºÏÖ¸Áî 201005atcpc
	  	M_ClrFlag(SL_OCS_SYSRUN);         							//ÇåÔ¤³äµçÖ¸Áî	201005atcpc
		M_ClrFlag(SL_NPR_START);									//ÇåÍø²àÂö³åÖ¸Áî
		M_ClrFlag(SL_MPR_START);									//Çå»ú²àÂö³åÖ¸Áî
	   	M_ClrFlag(SL_CBCLOSED);			 							//ÇåÖ÷¶Ï±ÕºÏÍê³É
	   	M_ClrFlag(SL_STEADYFB);          							//ÇåVdcÎÈ¶¨
		M_SetFlag(SL_SERSTOP_FH);									//ÖÃ²½Öè±êÖ¾Î»
		MAIN_LOOP.cnt_serstop=0;           							//Çå¼ÆÊýÆ÷	

		M_ClrFlag(SL_SYN_FH);										//ÇåSYN²½Öè±êÖ¾Î»
		M_ClrFlag(SL_SYN_S3);										//ÇåSYN²½Öè±êÖ¾Î»
		M_ClrFlag(SL_SYN_S2);										//ÇåSYN²½Öè±êÖ¾Î»
		M_ClrFlag(SL_SYN_S1);										//ÇåSYN²½Öè±êÖ¾Î»
		M_ClrFlag(SL_NORSTOPING);          							//Çå ýÔÚÕý³£Í£»úÖÐ ±êÖ¾
		M_ClrFlag(SL_NORSTOP_FH);									//ÇåNORSTOP²½Öè±êÖ¾Î»
		M_ClrFlag(SL_NORSTOP_S2);									//ÇåNORSTOP²½Öè±êÖ¾Î»
		M_ClrFlag(SL_NORSTOP_S1);									//ÇåNORSTOP²½Öè±êÖ¾Î»
		M_ClrFlag(SL_ERRSTOPING);									//Çå ÕýÔÚÒ»°ã¹ÊÕÏÍ£»úÖÐ ±êÖ¾
		M_ClrFlag(SL_ERRSTOP_FH);									//ÇåERRSTOP²½Öè±êÖ¾Î»
		M_ClrFlag(SL_CBON_FH);										//ÇåCBON²½Öè±êÖ¾Î»
	}
}


/**************************************************************************************************
** Ãû³Æ: ERR_STOP
** ¹¦ÄÜÃèÊö: 1.5MW±äÁ÷Æ÷ÏµÍ³Âß¼­¿ØÖÆ-»°ã¹ÊÕÏÍ£»ú 	
** Êä¡¡Èë:   	
** Êä¡¡³ö: 		 
** ×¢  ÊÍ: 			
**-------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 	 
** ÈÕ¡¡ÆÚ: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:	20121103¸Ä³ÉÓë»ª´´Ò»ù£¬ÏÈ¶ÏÖ÷¶ÏÔÙ¶Ï¶¨×Ó½Ó´¥Æ÷
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void ERR_STOP(void)         
{
	if(M_ChkFlag(SL_ERRSTOP_FH)!=0)
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_errstop,DELAY_ERRSTOP)>0)		//2s Ö÷¶Ï¶Ï¿ªÑÓÊ±ºóÔÙ¶Ï¶¨×Ó½Ó´¥Æ÷ºÍÖ÷½Ó´¥Æ÷
		{
			M_ClrFlag(CL_MAINK);    		  						//¶ÏÖ÷½Ó´¥Æ÷ºÍÂË²¨Æ÷
		   	M_ClrFlag(CL_PRE); 										//ÔÙ¶ÏÔ¤³äµç½Ó´¥Æ÷
		   	M_ClrFlag(CL_STATORK);    	  							//¶Ï¶¨×Ó½Ó´¥Æ÷	
		   	M_ClrFlag(SL_CHARGEOK);          						//ÇåÔ¤³äµçÍê³É
			M_ClrFlag(SL_FAN_WORK);									//Í£Ö¹·çúÔËÐÐ
		   	M_ClrFlag(SL_MPR_SYNOK);          						//Çå¶¨×Ó²¢ÍøÍê³É
		   	M_ClrFlag(SL_UDSTACOK);          						//Çå¶¨×ÓÂú×ã²¢ÍøÌõ¼þ
		
			if(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(SL_IN1_MIANK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)==0 && M_ChkFlag(SL_IN1_STATORK)==0)
			{
				M_ClrFlag(SL_ERRSTOP_FH);							//Çå²½Öè±êÖ¾Î»
				M_ClrFlag(SL_ERRSTOP_S1);							//Çå²½Öè±êÖ¾Î»
				M_ClrFlag(SL_ERRSTOPING);							//Çå ÕýÔÚÑÏÖØ¹ÊÕÏÍ£»úÖÐ ±ê¾
			}
		}
	}
	else if(M_ChkFlag(SL_ERRSTOP_S1)!=0)
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_epwmstop,DELAY_EPWMSTOP)>0)		//500ms
		{
			M_ClrFlag(CL_CBON);    		     							//ÇåÖ÷¶ÏºÏÕ¢Ö¸Áî
	   		M_ClrFlag(CL_CBLVC);         								//ÇåÇ·Ñ¹Á´¶Ï¿ª£¬Ö÷¶Ï¶Ï¿ª 
	   		M_ClrFlag(CL_CBFCON);            							//¶Ï¿ª¹ÊÕÏÁ´
	   		M_ClrFlag(SL_CBCLOSED);			 							//ÇåÖ÷¶Ï±ÕºÏÍê³É			
			M_SetFlag(SL_ERRSTOP_FH);									//ÖÃ²½Öè±êÖ¾Î»		
		}		
		MAIN_LOOP.cnt_errstop=0;           							//Çå¼ÆÊýÆ÷	
	}
	else
	{
	  	M_ClrFlag(SL_OCS_NPRSTART);         						//ÇåÍø²àÔËÐÐÖ¸Áî
	  	M_ClrFlag(SL_OCS_EIN);         								//ÇåÖ÷¶Ï±ÕºÏÖ¸Áî 201005atcpc
	  	M_ClrFlag(SL_OCS_SYSRUN);         							//ÇåÔ¤³äµçÖ¸Áî	201005atcpc
		M_ClrFlag(SL_NPR_START);									//ÇåÍø²àÂö³åÖ¸Áî
		M_ClrFlag(SL_MPR_START);									//Çå»ú²àÂö³åÖ¸Áî
	   	M_ClrFlag(SL_STEADYFB);          							//ÇåVdcÎÈ¶¨
		MAIN_LOOP.cnt_errstop=0;           							//Çå¼ÆÊýÆ÷	
		MAIN_LOOP.cnt_epwmstop=0;
		M_SetFlag(SL_ERRSTOP_S1);									//ÖÃ²½Öè±êÖ¾Î»			

		M_ClrFlag(SL_SYN_FH);										//ÇåSYN²½Öè±êÖ¾Î»
		M_ClrFlag(SL_SYN_S3);										//ÇåSYN²½Öè±êÖ¾Î»
		M_ClrFlag(SL_SYN_S2);										//ÇåSYN²½Öè±êÖ¾Î»
		M_ClrFlag(SL_SYN_S1);										//ÇåSYN²½Öè±êÖ¾Î»
		M_ClrFlag(SL_NORSTOPING);          							//Çå ÕýÔÚÕý³£Í£»úÖÐ ±êÖ¾
		M_ClrFlag(SL_NORSTOP_FH);									//ÇåNORSTOP²½Öè±êÖ¾Î»
		M_ClrFlag(SL_NORSTOP_S2);									//ÇåNORSTOP²½Öè±êÖ¾Î»
		M_ClrFlag(SL_NORSTOP_S1);									//ÇåNORSTOP²½Öè±êÖ¾Î»
	} 
/*	if(M_ChkFlag(SL_ERRSTOP_FH)!=0)
	{
		M_ClrFlag(CL_PRE); 											//¶ÏÔ¤³äµç½Ó´¥Æ÷
		M_ClrFlag(CL_MAINK);    		  							//¶ÏÖ÷½Ó´¥Æ÷ºÍÂË²¨Æ÷
		M_ClrFlag(SL_CHARGEOK);          							//ÇåÔ¤³äµçÍê³É

		if(M_ChkFlag(SL_IN1_MIANK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)==0)
		{
			M_ClrFlag(SL_FAN_WORK);									//Í£Ö¹·ç»úÔËÐÐ
			if(M_ChkFlag(SL_PCOCS_CBOFF)!=0 && M_ChkFlag(SL_OCS_EIN)==0)//PCÊÖ¶¯¿ØÖÆ¶Ï¿ªCB 201005atcpc
			{
			   	M_ClrFlag(CL_CBON);    		     					//ÇåÖ÷¶ÏºÏÕ¢Ö¸Áî
			   	M_ClrFlag(CL_CBLVC);         						//ÇåÇ·Ñ¹Á´¶Ï¿ª£¬Ö÷¶Ï¶Ï¿ª 
			   	M_ClrFlag(CL_CBFCON);            					//¶Ï¿ª¹ÊÕÏÁ´
				M_ClrFlag(SL_CBCLOSED);			 					//ÇåÖ÷¶Ï±ÕºÏÍê³É
				M_ClrFlag(SL_PCOCS_CBOFF);            				//Çå¶Ï¿ªÖ¸Áî
				M_ClrFlag(SL_CBON_FH);								//ÇåCBON²½Öè±êÖ¾Î»
			} 
			M_ClrFlag(SL_ERRSTOP_FH);								//Çå²½Öè±êÖ¾Î»
			M_ClrFlag(SL_ERRSTOPING);								//Çå ÕýÔÚÒ»°ã¹ÊÕÏÍ£»úÖÐ ±êÖ¾
		}	
	}
	else
	{
	  	M_ClrFlag(SL_OCS_EIN);         								//ÇåÖ÷¶Ï±ÕºÏÖ¸Áî
	  	M_ClrFlag(SL_OCS_SYSRUN);         							//ÇåÔ¤³äµçÖ¸Áî 201005atcpc
	  	M_ClrFlag(SL_OCS_NPRSTART);         						//ÇåÍø²àÔËÐÐÖ¸Áî
		M_ClrFlag(SL_NPR_START);									//ÇåÍø²àÂö³åÖ¸Áî
		M_ClrFlag(SL_MPR_START);									//Çå»ú²àÂö³åÖ¸Áî
		M_ClrFlag(CL_STATORK);    	  								//¶Ï¶¨×Ó½Ó´¥Æ÷	
		M_ClrFlag(SL_UDSTACOK);          							//Çå¶¨×ÓÂúã²¢ÍøÌõ¼þ
		M_ClrFlag(SL_MPR_SYNOK);          							//Çå¶¨×Ó²¢ÍøÍê³É

//		if(M_ChkFlag(SL_NPR_PWMOUT)==0 && M_ChkFlag(SL_MPR_PWMOUT)==0 && M_ChkFlag(SL_IN1_STATORK)==0)
		if(M_ChkFlag(SL_NPR_PWMOUT)==0 && M_ChkFlag(SL_MPR_PWMOUT)==0)//20100510
		{
	   		M_ClrFlag(SL_STEADYFB);          							//ÇåVdcÎÈ¶¨
			M_SetFlag(SL_ERRSTOP_FH);								//ÖÃ²½Öè±êÖ¾Î»
			MAIN_LOOP.cnt_errstop=0;           						//Çå¼ÆÊýÆ÷	
		}

		M_ClrFlag(SL_SYN_FH);										//ÇåSYN²½Öè±êÖ¾Î»
		M_ClrFlag(SL_SYN_S3);										//ÇåSYN²½Öè±êÖ¾Î»
		M_ClrFlag(SL_SYN_S2);										//ÇåSYN²½Öè±êÖ¾Î»
		M_ClrFlag(SL_SYN_S1);										//ÇåSYN²½Öè±êÖ¾Î»
		M_ClrFlag(SL_NORSTOPING);          							//Çå ÕýÔÚÕý³£Í£»úÖÐ ±êÖ¾
		M_ClrFlag(SL_NORSTOP_FH);									//ÇåNORSTOP²½Öè±êÖ¾Î»
		M_ClrFlag(SL_NORSTOP_S2);									//ÇåNORSTOP²½Öè±êÖ¾Î»
		M_ClrFlag(SL_NORSTOP_S1);									//ÇåNORSTOP²½Öè±êÖ¾Î»
	}
*/
}

/**************************************************************************************************
** Ãû³Æ: CB_ON
** ¹¦ÄÜÃèÊö: 1.5MW±äÁ÷Æ÷ÏµÍ³Âß¼­¿ØÖÆ-Ö÷¶Ï±ÕºÏ 	
** Êä¡¡Èë:   	
** Êä¡¡³ö: 		 
** ×¢  ÊÍ: 			
**-------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 	 
** ÈÕ¡¡ÆÚ: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void CB_ON(void)         
{ 
	if(M_ChkFlag(SL_CBON_FH)!=0)
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_cbon,DELAY_CBON)>0)		//1s ÑÓÊ±·¢Ö÷¶ÏºÏÕ¢Ö¸Áî
		{
		   	M_SetFlag(CL_CBON);    		     					//·¢Ö÷¶ÏºÏÕ¢Ö¸Áî
		}

		if(M_ChkFlag(SL_IN1_CBSTS)!=0)
		{
	   		M_SetFlag(SL_CBCLOSED);			 					//ÖÃÖ÷¶Ï±ÕºÏÍê³É
		   	M_ClrFlag(CL_CBON);    		     					//ÇåÖ÷¶ÏºÏÕ¢Ö¸Áî
			M_ClrFlag(SL_CBON_FH);								//Çå²½Öè±êÖ¾Î»
		}	
	}
	else
	{
	   	M_SetFlag(CL_CBLVC);         							//±ÕºÏÖ÷¶ÏÇ·Ñ¹´
	   	M_SetFlag(CL_CBFCON);            						//±ÕºÏ¹ÊÕÏÁ´
		M_SetFlag(SL_CBON_FH);									//ÖÃ²½Öè±ê¾Î»
		MAIN_LOOP.cnt_cbon=0;           						//Çå¼ÆÊýÆ÷	
	}
}


/**************************************************************************************************
** Ãû³Æ: CONTACTOR_TEST
** ¹¦ÄÜÃèÊö: 1.5MW±äÁ÷Æ÷ÏµÍ³Âß¼­¿ØÖÆ-½Ó´¥Æ÷²âÊÔ 	
** Êä¡¡Èë:   	
** Êä¡¡³ö: 		 
** ×¢  ÊÍ: 			
**-------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 	 
** ÈÕ¡¡ÆÚ: 2010-5-22		
**-------------------------------------------------------------------------------------------------
** ÐÞ¸ÄË:
** ÈÕÆÚ:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
/*	else if(_STDBY8==7 && M_ChkFlag(SL_RUN)==0 && M_ChkFlag(SL_IN1_CBSTS)==0 && AD_OUT_UDC<50) //²»·¢Âö³å,CB¶Ï¿ª,VdcÃ»µçÇé¿öÏÂÔÊÐí²âÊÔ							//
	{
		CONTACTOR_TEST();										//Ö´ÐÐ½Ó´¥Æ÷²âÊÔ²Ù×÷ 201005atcpc
	}


void CONTACTOR_TEST(void)         								//systest 201005atcpc
{ 
	M_ClrFlag(CL_CBON);    		     							//ÇåÖ÷¶ÏºÏÕ¢Ö¸Áî
   	M_ClrFlag(CL_CBLVC);         								//ÇåÇ·Ñ¹Á´Ï¿ª£¬Ö÷¶Ï¶Ï¿ª 
   	M_ClrFlag(CL_CBFCON);            							//¶Ï¿ª¹ÊÕÏÁ´
  	M_ClrFlag(SL_OCS_NPRSTART);         						//ÇåÍøàÔËÐÐÖ¸Áî
	M_ClrFlag(SL_NPR_START);									//ÇåÍø²àÂö³åÖ¸Áî
	M_ClrFlag(SL_MPR_START);									//Çå»ú²àÂö³åÖ¸Áî
   	M_ClrFlag(SL_CBCLOSED);			 							//ÇåÖ÷¶Ï±ÕºÏÍê³É
   	M_ClrFlag(SL_CHARGEOK);          							//ÇåÔ¤³äµçÍê³É
	M_ClrFlag(SL_FAN_WORK);										//Í£Ö¹·ç»úÔËÐÐ
  	M_ClrFlag(SL_MPR_SYNOK);          							//Çå¶¨×Ó²¢ÍøÍê³É
   	M_ClrFlag(SL_UDSTACOK);          							//Çå¶¨×ÓÂú×ã²¢ÍøÌõ¼þ

   	if(M_ChkFlag(SL_OCS_SYSRUN)!=0)								//Íâ²¿¿ØÖÆ£¬Îª1²Å±ÕºÏPREC/MC/MF
   	{
   		M_SetFlag(CL_MAINK);    		  						//¶ÏÖ÷½Ó´¥Æ÷ºÍÂË²¨Æ÷
  		M_SetFlag(CL_PRE); 										//ÔÙ¶ÏÔ¤³äµç½Ó´¥Æ÷
  	}
	else
	{
   		M_ClrFlag(CL_MAINK);    		  						//¶ÏÖ÷½Ó´¥Æ÷ºÍÂË²¨Æ÷
  		M_ClrFlag(CL_PRE); 										//Ù¶ÏÔ¤³äµç½Ó´¥Æ÷	
	}

    if(M_ChkFlag(SL_OCS_OPENSTAC)==0)							//Íâ²¿¿ØÖÆ£¬Îª0²ÅÔÊÐí±ÕºÏSTAC
     		M_SetFlag(CL_STATORK);  							//ÉÏÎ»»úÔÊÐí²¢Íø,±ÕºÏ¶¨×Ó½Ó´¥Æ÷                            					
	else	M_ClrFlag(CL_STATORK);								//ÉÏÎ»»ú½ûÖ¹²¢Íø,¶Ï¿ª¶¨×Ó½Ó´¥Æ÷	        

}

*/
/////////////////no more///////////////////
