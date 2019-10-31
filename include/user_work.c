   /****************************************Copyright (c)**************************************************
**                       		     ±±	¾©	½»	Í¨	´ó	Ñ§
**                                        µçÆø¹¤³ÌÑ§Ôº
**                                         614ÊµÑéÊÒ
** 
**                              
**
**--------------ÎÄ¼þÐÅÏ¢--------------------------------------------------------------------------------
**ÎÄ   ¼þ   Ãû: user_work.c
**´´   ½¨   ÈË: 
**×îºóÐÞ¸ÄÈÕÆÚ: 
**Ãè        Êö: 1.5MWË«À¡±äÁ÷Æ÷ºËÐÄ¿ØÖÆ³ÌÐò----×óÔÆ·ç³¡
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

//º¯ÊýÉùÃ÷
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
			   if(M_ChkFlag(SL_CONFIGPWM)==0 && M_ChkFlag(SL_NPR_PWMOUT)==0 && M_ChkFlag(SL_MPR_PWMOUT)==0) //Ã»ÓÐÅäÖÃ¿ª¹ØÆµÂÊÇÒÂö³å·âËø 20100507atzuoyun
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
	        if((M_ChkFlag(SL_NPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0))
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
            if((M_ChkFlag(SL_MPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0))
	        {
		       if(M_ChkFlag(SL_MPR_PWMOUT)==0)						//±£Ö¤Enepwmio_MPR()Ö»ÄÜÔËÐÐÒ»´Î
		       {
			      Enepwmio_MPR();      
			      M_SetFlag(SL_MPR_PWMOUT);			         
		       }
	        }
	        else                  									//·¢Éú¹ÊÕÏ¾ÍÁ¢¼´·âÂö³å
	        {		       
			   Disepwmio_MPR();       
			   M_ClrFlag(SL_MPR_PWMOUT);        
	        }
	}
    else   //ÏµÍ³ÕýÔÚÉÏµç,»òÔ¤³äµçÕ¢Ã»ÓÐÇÐ³ý
	{      
		DisPwm();
		M_ClrFlag(SL_NPR_PWMOUT);
		M_ClrFlag(SL_MPR_PWMOUT);
		M_ClrFlag(SL_CONFIGPWM);			        				//Âö³å·âËø
		MAIN_LOOP.cnt_pwmout=0;										//Çå³ýÏµÍ³¹¤×÷ÔÊÐíÑÓÊ±
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
      	case 1:    temp1=(Uint16) (t0 * SW_NPR);				//SW_NPRÎªPWMÖÜÆÚ¼Ä´æÆ÷µÄÖµ,Îª¿ª¹ØÖÜÆÚµÄÒ»°ë
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
   EPwm4Regs.CMPA.half.CMPA = temp1;
   EPwm5Regs.CMPA.half.CMPA = temp2;
   EPwm6Regs.CMPA.half.CMPA = temp3; 
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
		M_ClrFlag(SL_MPR_RUN);			    						//»ú¹¤×÷ÔÊÐí±êÖ¾Çå0
		GIVE.urf      = _URF;										//Íø²à¸ø¶¨µçÑ¹Îªµ±Ç°VdcÖµ
		GIVE.npriqrf  = 0;											//Íø²à¸ø¶¨µçÁ÷Îª0
        GIVE.toqrf    = 0;                  						//»ú²à¸ø¶¨×ª¾ØÎª0
        GIVE.anglerf  = 0;		            						//»ú²à¸ø¶¨¹¦ÂÊÒòÊý½Ç¶ÈÎª0
	}
	else
	{

		if(M_ChkFlag(SL_OCS_NPREIN)!=0)					           //Íø²àÎÞ¹¦²¢Íø,Íø²à¸ø¶¨Ö±Á÷µçÑ¹ºÍÎÞ¹¦µçÁ÷Öµ
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
			if(M_ChkFlag(SL_NPR_START)!=0)  		   				//Íø²àÔËÐÐÅÐ¶Ï
			{
			   M_SetFlag(SL_NPR_RUN);                  				//Íø²àÂö³åÔÊÐí		       
		       GIVE.urf       = (int16) _URF;          				//¸øÖÐ¼äÖ±Á÷µçÑ¹Ö¸Áî¸³Öµ
			   GIVE.npriqrf   = (int16) _NIQRF;        				//Íø²à¸ø¶¨¸ÐÐÔÎÞ¹¦±£Ö¤²¢Íø¹¦ÂÊÒòÊýÎª1
//			   GIVE.npriqrf   = 70.0;                  				//Íø²à¸ø¶¨¸ÐÐÔÎÞ¹¦±£Ö¤²¢Íø¹¦ÂÊÒòÊýÎª1£¬»¥À¡ÊµÑéÑéÖ¤ÐèÒª²¹70A
			}
		    else     M_ClrFlag(SL_NPR_RUN);


	        if(M_ChkFlag(SL_MPR_START)!=0)  						//»ú²àÔËÐÐÅÐ¶Ï
	        {
	           M_SetFlag(SL_MPR_RUN);                   			//»ú²àÂö³åÔÊÐí
 
               if(_CANOPER==0)	  
               {
					if(M_ChkFlag(SL_IN1_STATORK)!=0)				//¶¨×Ó½Ó´¥Æ÷±ÕºÏÒÔºóÔÙ½ÓÊÕ×ª¾ØºÍ½Ç¶ÈÖ¸Áî
					{ 
               			GIVE.toqrf  =  (int16) _TOQRF;              //ÕýÖµÎª·¢µç
						GIVE.anglerf=  (int16) _AGLRF;  			//¸ø¹¦ÂÊÒòÊý½ÇÖ¸Áî¸³Öµ,-360 -- 360
					}
					else
      		   		{
      		   			GIVE.toqrf    = 0;                  		//»ú²à¸ø¶¨×ª¾ØÎª0
        				GIVE.anglerf  = 0;		            		//»ú²à¸ø¶¨¹¦ÂÊÒòÊý½Ç¶ÈÎª0
      		   		} 
			   }
               else   
               {           
					if(M_ChkFlag(SL_IN1_STATORK)!=0)				//¶¨×Ó½Ó´¥Æ÷±ÕºÏÒÔºóÔÙ½ÓÊÕ×ª¾ØºÍ½Ç¶ÈÖ¸Áî
					{
               			temp_toqrf     =  (int16)SCI_canopen.rx_torque;               //ÕýÖµÎª·¢µç
      		   			GIVE.toqrf     =  temp_toqrf * CAN_TEN / 16383.0;             //»ú²à×ª¾ØÖ¸ÁîÊµ¼ÊÖµ  _TOQRF == -200%-200%

						if(GIVE.toqrf<0)	GIVE.toqrf=0;    //±äÁ÷Æ÷¶ÔÖ÷¿ØµÄ¸º×ª¾ØÖ¸Áî²»ÓèÏìÓ¦ 20090815
			   
               			temp_aglrf     =  (int16)SCI_canopen.rx_angle;               
      		   			GIVE.anglerf   =  temp_aglrf * CAN_ANGLEN / 16383.0;  
      		   		}
      		   		else
      		   		{
      		   			GIVE.toqrf    = 0;                  		//»ú²à¸ø¶¨×ª¾ØÎª0
        				GIVE.anglerf  = 0;		            		//»ú²à¸ø¶¨¹¦ÂÊÒòÊý½Ç¶ÈÎª0
      		   		}             				
			   
			   }

			   if     (GIVE.anglerf <-30.0)                      GIVE.anglerf     = -30.0;
			   else if(GIVE.anglerf > 30.0)                      GIVE.anglerf     =  30.0;
			   else  											 GIVE.anglerf     = GIVE.anglerf;
                                         
	        }
            else    M_ClrFlag(SL_MPR_RUN);
		}
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
    float udc_max,id_max,iq_max,toq_max,agl_max,temp;
	Uint16 temp_n,temp_ud,temp_iqexi,temp_number;
	float  temp_exi,temp_iqk;
	float  temp_Qc,temp_Qg,temp_tan;

//----------------------------Íø²àÔËÐÐ¸ø¶¨¿ØÖÆ---------------------------------------------------------

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
       M_SetFlag(SL_NPR_RUNING);                 								//Íø²àÕýÔÚÔËÐÐ£¬¿ªÊ¼·¢Íø²àÂö³å
       MAIN_LOOP.cnt_isteady0= 0;
	                
	   RUN.urf = Give_Integral(GIVE.urf,RUN.ustep,RUN.urf);     				//µçÑ¹¸ø¶¨»ý·Ö
       if(RUN.urf == GIVE.urf)	    M_SetFlag(SL_STEADYGV);        				//¸ø¶¨»ý·ÖÍê³É,ÖÃÎÈ¶¨±êÖ¾Î»
	   else							M_ClrFlag(SL_STEADYGV);

 	   if(M_ChkFlag(SL_STEADYGV)!=0)											//´ýµçÑ¹¸ø¶¨Íê³ÉºóÔÙ¸ø¶¨ÎÞ¹¦µçÁ÷
	 	   RUN.npriqrf = Give_Integral(GIVE.npriqrf,RUN.npriqstep,RUN.npriqrf); //Íø²à¹¦µçÁ÷¸ø¶¨»ý·Ö	         
    }
//-----´ý»ú    
    else                                   										//´ý»ú
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

//-----»ú²àÕýÔÚÔËÐÐ²¢ÒªÇóÍ£»ú
    if((M_ChkFlag(SL_MPR_RUNING)!=0)&&(M_ChkFlag(SL_MPR_RUN)==0)) 
    {      
		if(M_ChkFlag(SL_IN1_STATORK)!=0)
		{
		    RUN.toqrf = Give_Integral(0,RUN.toqstep,RUN.toqrf);  				//»ú²à×ª¾Ø¸ø¶¨»ý·Ö
		    RUN.aglrf = Give_Integral(0,RUN.aglstep,RUN.aglrf);  			    //»ú²à¹¦ÂÊÒòÊý½Ç¸ø¶¨»ý·Ö


		    RUN.mpriqrf_g  =  - RUN.toqrf  / (IRQTOTE * TRS_NGS_U.dflt);   		//¸ø¶¨qÖáµçÁ÷Ö¸Áî¼ÆËã
//	    	RUN.mpriqrf_g  =  RUN.toqrf * STAROTRTO / (1.5 * POLEPAIRES * MPR_Lm * TRS_NGS_U.dflt / (314.15926 *  MPR_Ls));   //¸ø¶¨qÖáµçÁ÷Ö¸Áî¼ÆËã
 
			RUN_mpridrf_exi  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));  //¸ººÅ:ÍøÑ¹edÓë»ú²àdÖáÏà²î180¶È             
      	    RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          				//³Ëµ÷ÕûÏµÊý,ÓÉ²Ù×÷Æ÷¸ø¶¨ 
        	temp_ud = (Uint16)(TRS_NGS_U.dflt * SQRT3 / SQRT2);
        	if     (temp_ud < 540) 		temp_ud = 540;
        	else if(temp_ud > 800) 	    temp_ud = 800;
	    	temp_n = (temp_ud - 540) * 127 / 260;
        	temp_exi = TAB_EXI[temp_n];
        	RUN.mpridrf_exi =  RUN.mpridrf_exi * temp_exi;          			//²é±íµ÷ÕûÏµÊý,Óë¿ÕÔØ¸ÐÓ¦µçÊÆÓÐ¹Ø
/*//20091022atzy
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
		    if(RUN.mpriqrf==0)        M_SetFlag(SL_STACTRIPEN);
			else    			      M_ClrFlag(SL_STACTRIPEN);
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
    else if((M_ChkFlag(SL_MPR_RUN)!=0) && (M_ChkFlag(SL_STEADYFB)!=0))  
    {  
        M_SetFlag(SL_MPR_RUNING);
        M_ClrFlag(SL_STACTRIPEN);                 									//»ú²àÕýÔÚÔËÐÐ£¬¿ªÊ¼·¢»ú²àÂö³å
        MAIN_LOOP.cnt_mprsteady0= 0;
 			   
		RUN.toqrf = Give_Integral(GIVE.toqrf,RUN.toqstep,RUN.toqrf);  				//»ú²à×ª¾Ø¸ø¶¨»ý·Ö
		RUN.aglrf = Give_Integral(GIVE.anglerf,RUN.aglstep,RUN.aglrf);  			//»ú²à¹¦ÂÊÒòÊý½Ç¸ø¶¨»ý·Ö
 
	    RUN.mpriqrf_g  =  - RUN.toqrf  / (IRQTOTE * TRS_NGS_U.dflt);   				//¸ø¶¨qÖáµçÁ÷Ö¸Áî¼ÆËã
//	    RUN.mpriqrf  =  GIVE.toqrf * STAROTRTO / (1.5 * POLEPAIRES * MPR_Lm * TRS_NGS_U.dflt / (314.15926 *  MPR_Ls));   //¸ø¶¨qÖáµçÁ÷Ö¸Áî¼ÆËã
 
		RUN_mpridrf_exi  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));  //¸ººÅ:ÍøÑ¹edÓë»ú²àdÖáÏà²î180¶È             
        RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          						//³Ëµ÷ÕûÏµÊý,ÓÉ²Ù×÷Æ÷¸ø¶¨ 
        temp_ud = (Uint16)(TRS_NGS_U.dflt * SQRT3 / SQRT2);							//ÍøÑ¹ÏßµçÑ¹ÓÐÐ§Öµ 690V
        if     (temp_ud < 540) 		temp_ud = 540;
        else if(temp_ud > 800) 	    temp_ud = 800;
	    temp_n = (temp_ud - 540) * 127 / 260;
        temp_exi = TAB_EXI[temp_n];
        RUN.mpridrf_exi =  RUN.mpridrf_exi * temp_exi;          					//²é±íµ÷ÕûÏµÊý,ÓÉ²Ù×÷Æ÷¸ø¶¨ 
/*//20091022atzy 
		RUN_mpriq = (Uint16)(- RUN.mpriqrf_g);
		if     (RUN_mpriq < 290)   temp_iqexi = 290;
		else if(RUN_mpriq > 600)   temp_iqexi = 600;
        temp_iqexi = RUN_mpriq;  //20091019atzy
		temp_number = (temp_iqexi - 290) * 127 / 310;
		temp_iqk = TAB_IQEXI[temp_number];
        RUN.mpridrf_exi = RUN.mpridrf_exi * temp_iqk;          						//²é±íµ÷ÕûÏµÊý,ÓÉ¸ºÔØµçÁ÷Irq¸ø¶¨È·¶¨ 
*///20091022atzy 
   
 	    RUN.radianrf    = RUN.aglrf * 0.017453292;									//½Ç¶È×ª»¡¶È360->2PAI	 0.01745329=2 * PAI / 360
	    RUN.mpridrf_var = RUN.mpriqrf_g * sin(RUN.radianrf) / cos(RUN.radianrf); //Í£»úÊ±£¬±£Ö¤¶¨×ÓµçÁ÷Îª0£¬¼´¹¦ÂÊÒòÊýÎª1
			   	    																//RUN.aglrf>0,×ª×ÓÀø´ÅÔö¼Ó,·¢³ö¸ÐÐÔÎÞ¹¦;·´Ö®,ÈÝÐÔ
//	    temp_Qc = TRS_NGS_U.dflt * 314.15926 * 501.0e-6 / SQRT2;					//ÔËÐÐÊ±£¬ÒÑ×Ü²¢Íø¹¦ÂÊÒòÊý1ÎªÄ¿±ê
//	    temp_Qg = PRO.Pgactive * sin(RUN.radianrf) / cos(RUN.radianrf);
//		temp_tan    = (temp_Qg - temp_Qc) / PRO.Psactive;
//	    RUN.mpridrf_var = RUN.mpriqrf_g * temp_tan;
        
        RUN.mpridrf_g   = RUN.mpridrf_exi + RUN.mpridrf_var;                                           


		RUN.mpridrf = Give_Integral(RUN.mpridrf_g,RUN.mpridstep,RUN.mpridrf);  		//»ú²àÓÐ¹¦µçÁ÷¸ø¶¨»ý·Ö
		RUN.mpriqrf = Give_Integral(RUN.mpriqrf_g,RUN.mpriqstep,RUN.mpriqrf);  		//»ú²àÎÞ¹¦µçÁ÷¸ø¶¨»ý·Ö

    } 
//-----´ý»ú
    else                                    										//´ý»ú
    {
	   	M_ClrFlag(SL_MPR_RUNING);
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
//	    temp       =  temp * 1000.0;        //ÎªCPCÖÐÐÄÊµÑéÌ¨Ìá¸ßÓÐ¹¦¸ø¶¨ËÙ¶È cpc 20090815 ÈÃ×ª¾Ø¸ø¶¨ÒÔmsÎªµ¥Î»
	    RUN.toqstep=  abs(toq_max)/temp;											//¼ÆËã×ª¾Ø¸ø¶¨²½³¤ 
        agl_max    =  (float)FUNC[NO_AGLRF].max;
	    temp       =  (float)_RU_AGLT;
		temp       =  temp * 1000.0;		//µ¥Î»s									//ÎªCPCÖÐÐÄÊµÑéÌ¨½µµÍÓÐ¹¦¸ø¶¨ËÙ¶È cpc test
	    RUN.aglstep=  abs(agl_max)/temp; 											//¼ÆËã¹¦ÂÊÒòÊý½Ç¸ø¶¨²½³¤ 

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
}  
 
 

/*************************************************************************************************
** º¯ÊýÃû³Æ:  QepEncodPos
** ¹¦ÄÜÃèÊö:  µ÷ÕûÎ»ÖÃ¼ÆÊýÆ÷ÎªÔö¼ÆÊýÄ£Ê½£¬±àÂëÆ÷Î»ÖÃ¼ì²â
** Êä¡¡Èë:    ÎÈÌ¬ÏÂµÄ×ª×ÓµçÁ÷£¬¶¨×ÓµçÑ¹
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
//----------------------ÅÐ¶ÏPOSCNT¼ÆÊý·½Ïò£¬È·±£Ôö¼ÆÊýÄ£Ê½------------------------------------------
	if(M_ChkFlag(SL_QCAPSPDIN)!=0 && M_ChkFlag(SL_SPEED_HIGH)!=0)	//20091020atzy·ç»ú´ý»úÊ±µç»úÖá»á×óÓÒ×ª¶¯//×ªËÙ¼ì²âÕý³£ºó£¬µ÷Õû±àÂëÆ÷¹¤×÷Ä£Ê½
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
//--------QCAPÕý³£¹¤×÷ºó¼ÆÊý±àÂëÆ÷Î»ÖÃ--------------------------------------------------------------
	if(M_ChkFlag(SL_QCAPSPDIN)!=0)			//×ªËÙ¼ì²âÕý³£ºó£¬¼ÆËã±àÂëÆ÷Î»ÖÃ
	{
  		temp_pos = _encodpos;
		QEPDATA.encodpos =  - temp_pos;   	//ÓÉ²Ù×÷Æ÷Ö±½Ó¸ø¶¨³õÊ¼Î»ÖÃ½Ç¶È										
	}
	
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
	float temp,phaseshift;

//------------¼ÆËãÍøÑ¹ÆµÂÊ¡¢½ÇÆµÂÊ¡¢CAPµ¥Î»Ê±¼äÄÚÍøÑ¹ÏàÎ»ÔöÁ¿--------------------------------------
	temp=(float)ECap5Regs.CAP1;									//¼ÇÂ¼²¶»ñÖµ

	if(temp>0)
	{
		CAP5.prd = temp;										
		CAP5.freqtmp   = 150.0e6/CAP5.prd;						//¼ÆËã±¾´ÎÆµÂÊ
	}

	if (abs(CAP5.freqtmp-50)<1)									//Èç¹ûÆµÂÊÕý³£Ôò¸üÐÂÆµÂÊ
	{
		CAP5.freq   = CAP5.freqtmp;    							//¸üÐÂ
		CAP5.omigasyn  = TWOPAI * CAP5.freq;						
		CAP5.radpertb  = TWOPAI/CAP5.prd;						//¼ÆËãÒ»¸ö¼ÆÊýÖÜÆÚ½Ç¶ÈÔöÁ¿
		M_ClrFlag(SL_GRDFQE);									//ÇåÆµÂÊÊ§°Ü±êÖ¾
	}
	else	M_SetFlag(SL_GRDFQE);								//·ñÔòÖÃÆµÂÊÓÐÎó±êÖ¾Î»	
		

//----------------¼ÆËãUbc£¬UabÏàÎ»-----------------------------------------------------------------
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
		if(M_ChkFlag(SL_POWERON)==0)	M_SetFlag(SL_PHORDE);	//DSPÉÏµçºó	ÏàÐòÓÐÎó±êÖ¾Î»ÖÃ1 20090817
	}
	else 
		M_ClrFlag(SL_PHORDE);									// ÇåÁãÏàÐòÓÐÎó±êÖ¾Î» 
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
** ÈÕ¡¡ÆÚ:	20090409 
**--------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕ  ÆÚ:	20090812 at cpc
**--------------------------------------------------------------------------------------------------
**************************************************************************************************/
 void CapQepCtrl(void)
{
	float	temp,temp1,radpert0;
	Uint32  temp2,temp3;

//------------¼ÆËãÍø²àÍøÑ¹ÂÊ¡¢÷Ö½ÇÆµÂÊ¡¢CAPµ¥Î»Ê±¼äÄÚÍøÑ¹ÏàÎ»ÔöÁ¿-----------------------------------
	temp2 = ECap4Regs.CAP1;
	temp  =	(float)temp2;									//²¶»ñÊÂ¼þÖÜÆÚÖµ
	
	if(temp>0)
	{
		CAP4.prd = temp;									
		CAP4.freqtmp   = 150.0e6/CAP4.prd;					//¼ÆËã±¾´ÎÍøÑ¹ÆµÂÊ
	}

	if (abs(CAP4.freqtmp - 50)<1)							//Èç¹ûÆµÂÊÕý³£Ôò¸üÐÂÍøÑ¹ÆµÂÊ
	{
		CAP4.freq      = CAP4.freqtmp;				    	//¸üÐÂÍøÑ¹ÆµÂÊ CAP4.freqtmpÊÇÊµ¼ÊÊµÊ±ÍøÆµ
		CAP4.omigasyn  = TWOPAI * CAP4.freq;						
		CAP4.radpertb  = TWOPAI * CAP4.freq / 150.0e6;		//¼ÆËãÒ»¸ö¼ÆÊýÖÜÆÚ½Ç¶ÈÔöÁ¿
		M_ClrFlag(SL_GRDFQE);					   	   	 	//ÇåÆµÂÊÊ§°Ü±êÖ¾
	}
	else	M_SetFlag(SL_GRDFQE);							//ÆµÂÊÓÐÎó±êÖ¾Î»ÖÃ1	
		


//-----------¼ÆËãÍø²àËã·¨Õý±ä»»½Ç¶È----------------------------------------------------------------                         
	
	radpert0       = TWOPAI * CAP4.freq * 0.2e-3;   		//¼ÆËãÒ»¸öT0ÖÐ¶ÏÖÜÆÚ½Ç¶ÈÔöÁ¿ T0=0.2ms
  
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
//-------------¶¨×Ó´ÅÁ´Î»ÖÃ-------------------------------------------------------------------------
//	CAP4.stavectheta = 	CAP4.nprtrstheta + TWOPAI_3;			//¶¨×Ó´ÅÁ´Ïà¶ÔÓÚAÏàÖáÏßµÄÎ»ÖÃ


//-------------×ª×Ó»úÐµ½ÇÆµÂÊºÍµç½ÇÆµÂÊ------------------------------------------------------------	
//	QEPDATA.qcapprd = EQep2Regs.QCPRD;	                		//¶ÔeQEPÄ£¿éµÄQCLK½øÐÐ512·ÖÆµ£¬QCAPÊ±ÖÓ²ÉÓÃSYSCLKOUT/128  20090817
//	temp = 300.0e6 *  PAI/(PLSPRVL  * (float)QEPDATA.qcapprd); 	//×ª×Ó»úÐµ½ÇÆµÂÊrad/s  QEPDATA.omigamec = 2 * PAI * (150M/128) *  512/(QCPRD * PLSPRVL * 4 )  20090817	 						                            

//    if(M_ChkFlag(SL_SPEED_HIGH)!=0)		temp = 2400.0e6 *  PAI/(PLSPRVL  * (float)QEPDATA.qcapprd);  //×ª×Ó»úÐµ½ÇÆµÂÊrad/s  QEPDATA.omigamec = 2 * PAI * 37.5M * 128/(QCPRD * PLSPRVL * 4 )  20090815atcpc
//    else  								temp = 150.0e6 *  PAI/(PLSPRVL  * (float)QEPDATA.qcapprd);   //×ª×Ó»úÐµ½ÇÆµÂÊrad/s  QEPDATA.omigamec = 2 * PAI * 37.5M * 8/(QCPRD * PLSPRVL * 4 )  20090815atcpc	

	if(M_ChkFlag(SL_SPEED_HIGH)!=0)
	{
		QEPDATA.qposlat2 = EQep2Regs.QPOSLAT;                 		//²ÉÓÃSYSCLK=150M£¬QUPRD=1.5M,µ¥Î»ÆµÂÊÎª100Hz
/*
		if(QEPDATA.qposlat2 < QEPDATA.qposlat1)
			QEPDATA.qposlat  = QEPDATA.qposlat2 + PLSPRVL * 4 - QEPDATA.qposlat1;  //20090817
		else
			QEPDATA.qposlat  = QEPDATA.qposlat2 - QEPDATA.qposlat1;
		
		QEPDATA.qposlat1  = QEPDATA.qposlat2;
		
		temp = 50.0 *  PAI * (float)QEPDATA.qposlat / PLSPRVL; 		//×ª×Ó»úÐµ½ÇÆµÂÊrad/s  QEPDATA.omigamec = 2 * PAI * (150M/1.5M) *  QEPDATA.qposlat/(PLSPRVL * 4 )  20090817	 						                            		
	}
	else
	{
		QEPDATA.qcapprd = EQep2Regs.QCPRD;	                		//¶ÔeQEPÄ£¿éµÄQCLK½øÐÐ32·ÖÆµ£¬QCAPÊ±ÖÓ²ÉÓÃSYSCLKOUT/128  20090817
		temp = 37.5e6 *  PAI/(PLSPRVL  * (float)QEPDATA.qcapprd); //×ª×Ó»úÐµ½ÇÆµÂÊrad/s  QEPDATA.omigamec = 2 * PAI * (150M/128) *  64/(QCPRD * PLSPRVL * 4 )  20090817	 						                            		
	}
*/

//====================ÖØÒªÐÞÕý 201105atzuoyun======================================================================
		if(QEPDATA.qposlat2 < QEPDATA.qposlat1)				//201011LVRT DSPÄÚ²¿µÄEQep2Regs.QPOSLAT¸üÐÂËÙ¶ÈÃ»ÓÐT0ÕâÃ´¿ì£¡Ë¢ÐÂ100HzºÍ²ÉÑùÎª5kHz
		{
			QEPDATA.qposlat  = QEPDATA.qposlat2 + PLSPRVL * 4 - QEPDATA.qposlat1;  //20090817
			temp = 50.0 *  PAI * (float)QEPDATA.qposlat / PLSPRVL; 		//×ª×Ó»úÐµ½ÇÆµÂÊrad/s  QEPDATA.omigamec = 2 * PAI * (150M/1.5M) *  QEPDATA.qposlat/(PLSPRVL * 4 )  20090817	 						                            		
		}			
		else if(QEPDATA.qposlat2 == QEPDATA.qposlat1)
		{
			temp = QEPDATA.omigamec;
		}
		else
		{
			QEPDATA.qposlat  = QEPDATA.qposlat2 - QEPDATA.qposlat1;
			temp = 50.0 *  PAI * (float)QEPDATA.qposlat / PLSPRVL; 		//×ª×Ó»úÐµ½ÇÆµÂÊrad/s  QEPDATA.omigamec = 2 * PAI * (150M/1.5M) *  QEPDATA.qposlat/(PLSPRVL * 4 )  20090817	 						                            		
		}
			
		QEPDATA.qposlat1  = QEPDATA.qposlat2;
	}
	else
	{
		QEPDATA.qcapprd = EQep2Regs.QCPRD;	                		//¶ÔeQEPÄ£¿éµÄQCLK½øÐÐ32·ÖÆµ£¬QCAPÊ±ÖÓ²ÉÓÃSYSCLKOUT/128  20090817
		temp = 37.5e6 *  PAI/(PLSPRVL  * (float)QEPDATA.qcapprd); //×ª×Ó»úÐµ½ÇÆµÂÊrad/s  QEPDATA.omigamec = 2 * PAI * (150M/128) *  64/(QCPRD * PLSPRVL * 4 )  20090817	 						                            		
	}

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
//	if(temp!=0)		
	if((temp!=0) && M_ChkFlag(SL_QCAPSPDIN)!=0)		//201105atzuoyun
	{
	   _NQEPPCO++;
	   if(_NQEPPCO >10)		   M_SetFlag(SL_QEPPCO);    	//ÖÃ¼ÆÊýÆ÷ÉÏÒç±êÖ¾Î»
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
/*						
	temp = EQep2Regs.QFLG.bit.PHE;
	if(temp!=0)		
	{
	    M_SetFlag(SL_QEPPHE);								//QEPµÄABÐÅºÅÏàÎ»¹ÊÕÏ,Ïà²î·Ç90¶È,¹ÊÕÏ³ÌÐòÖÐ²»ÔÙ¼ì²â¸Ã¹ÊÕÏ
	    EQep2Regs.QCLR.bit.PHE = 1; 
	}
*/
//----------»ú²àËã·¨Õý±ä»»½Ç¶È--------------------------------------------------------------------- 
    QEPDATA.posilat = EQep2Regs.QPOSILAT;								//ZÐÅºÅµ½À´Ê±POSCNTµÄ¼ÆÊýÖµ
	if(QEPDATA.posilat < (PLSPRVL * 4 -20))
	{	
		QEPDATA.rotpos = QEPDATA.rotpos + QEPDATA.rotradpret0;
		if(QEPDATA.rotpos > 2* TWOPAI) QEPDATA.rotpos = QEPDATA.rotpos - 2* TWOPAI;
//		if((M_ChkCounter(MAIN_LOOP.cnt_qepzdisturb,DELAY_QEPZDISTURB)>=0)&&(M_ChkFlag(SL_QCAPSPDIN)!=0))    //1s //×ªËÙ¼ì²âÕý³£ºó£¬¼ÆËã±àÂëÆ÷Î»ÖÃ
		if(M_ChkCounter(MAIN_LOOP.cnt_qepzdisturb,DELAY_QEPZDISTURB)>=0)   //1s
			M_SetFlag(SL_QEPZDISTRUB);
	}
	else
	{														//temp1 = POLEPAIRES  * 2 * PAI * QEPDATA.adsmposcnt/(PLSPRVL * 4);
		temp1 = 1.53398e-3 * QEPDATA.adsmposcnt; 			// POLEPAIRES=2, PLSPRVL=2048cpc								
		QEPDATA.rotpos  = 	temp1;                          //0--4PIE
		MAIN_LOOP.cnt_qepzdisturb=0;
		M_ClrFlag(SL_QEPZDISTRUB);
	}

	temp1 = QEPDATA.rotpos - QEPDATA.encodpos;				//AD²ÉÑùÊ±¿Ì×ª×ÓÎ»ÖÃ - ³õÊ¼Î»ÖÃ½Ç¶È
                                                            // -2PIE -- 6PIE
	if 		(temp1 < 0) 	  	temp1 = temp1 + TWOPAI;	
	else if (temp1 > TWOPAI)  	temp1 = temp1 - TWOPAI;		//AD²ÉÑùÊ±¿Ì×ª×ÓÎ»ÖÃÈ¡Ä£(0-TWOPAI)
	if 		(temp1 < 0) 	  	temp1 = temp1 + TWOPAI;	
	else if (temp1 > TWOPAI)  	temp1 = temp1 - TWOPAI;		//AD²ÉÑùÊ±¿Ì×ª×ÓÎ»ÖÃÈ¡Ä£(0-TWOPAI)
							
	temp = TWOPAI_3 + CAP4.nprtrstheta - temp1;				//Õý±ä»»½Ç¶È£¬Î´È¡Ä££¬£¨-1.3PAI---2.7PAI£© 
															//¶¨×ÓÈÆ×é½Ç½Ó,BÏàµçÑ¹¹ýÁãÊ±£¬µçÑ¹Ê¸Á¿Îª30degree
	if		(temp > TWOPAI)	 	temp = temp - TWOPAI;	
	else if	(temp < 0)	  		temp = temp + TWOPAI;		//È¡Ä£ (0-TWOPAI)
//	if		(temp > TWOPAI)	 	temp = temp - TWOPAI;	
//	else if	(temp < 0)	  		temp = temp + TWOPAI;		//È¡Ä£ (0-TWOPAI)
		
	CAP4.mprtrstheta = temp;								//»ú²àÕý±ä»»½Ç¶È

	if 		(QEPDATA.rotpos < 0) 	  	QEPDATA.rotposdisp = QEPDATA.rotpos + TWOPAI;	
	else if (QEPDATA.rotpos > TWOPAI)  	QEPDATA.rotposdisp = QEPDATA.rotpos - TWOPAI;	//QEPDATA.rotposdispÓÃÓÚDAÏÔÊ¾


//-----------¼ÆËãÍø²à·´±ä»»½Ç¶È----201105atzuoyun----------------------------------------------------------------
	if(M_ChkT1Direction() == 0)  							//CTR¼õ¼ÆÊý 
    	temp3 = EPwm1Regs.TBPRD + EPwm1Regs.TBCTR;
	else 													//CTRÔö¼ÆÊý
		temp3 = 3 * EPwm1Regs.TBPRD - EPwm1Regs.TBCTR;

	temp3 = temp3 * 2; 										//±ä»»µ½CAP4µÄÊ±ÖÓÆµÂÊ£º150M
//	temp2 = ECap4Regs.TSCTR;     //problem:²»ÄÜÓÐÐ§¿¹ÍøÆµ¸ÉÈÅ
//	temp2 = temp2 + temp3;									//¹À¼ÆÏÂ´ÎÂö³å·¢Ê±ECap4Regs.TSCTRµÄÖµ		
//	temp  = temp2 * CAP4.radpertb;							//Íø²à·´±ä»»½Ç¶È£¬Î´È¡Ä££¬Ô¼£¨0---2PAI£©
	temp  = CAP4.nprtrstheta + (float)temp3 * CAP4.radpertb;//²ÉÓÃ¿¹¸ÉÈÅºó½Ç¶È¼ÆËã·´±ä»»½Ç¶È201105atzuoyun		

//	temp2 =  temp3 * CAP4.radpertb;									
//	temp  = CAP4.nprtrstheta + temp2;						//Íø²à·´±ä»»½Ç¶È£¬Î´È¡Ä££¬Ô¼£¨0---2PAI£©
	
	if (temp > TWOPAI)	temp = temp - TWOPAI;				//È¡Ä£2 * PAI
			
	CAP4.npratitheta = temp;								//Íø²à·´±ä»»½Ç¶È
	
//----------¼ÆËã»ú²à·´±ä»»½Ç¶È---201105atzuoyun------------------------------------------------------------------
/*	temp2 = ECap4Regs.TSCTR;           //problem:²»ÄÜÓÐÐ§¿¹ÍøÆµ¸ÉÈÅ
	temp  = (float)temp2;
	temp  = temp * CAP4.radpertb + TWOPAI_3;  				//µ±Ç°¶¨×Ó´ÅÁ´Î»ÖÃrad PAI_2=PAI/2=1.5707963	¶¨×ÓÈÆ×é½Ç½Ó zlquestion
    													
	temp2 = EQep2Regs.QPOSCNT;
	temp1 = (float)temp2;									//QEPÄ£¿éÎ»ÖÃ¼ÆÊýÆ÷µÄÖµ

	temp1 = 1.53398e-3 * temp1;								//×ª×ÓÏà¶ÔÓÚÖÜÆÚÐÅºÅµÄÎ»ÖÃrad(µç½Ç¶È) //temp1 = POLEPAIRES  * PAI * temp1/(PLSPRVL * 2.0);
              												//0.002618=POLEPAIRES  * PAI/(PLSPRVL * 2.0); POLEPAIRES=2, PLSPRVL=2048cpc                          						
	temp1 = temp1 - QEPDATA.encodpos;						//µ±Ç°×ª×ÓÊµ¼ÊÎ»ÖÃrad	

	if 		(temp1 < 0) 	   	temp1 = temp1 + TWOPAI;
	else if (temp1 > TWOPAI) 	temp1 = temp1 - TWOPAI;		//µ±Ç°×ª×ÓÎ»ÖÃÈ¡Ä£
		
	temp = temp - temp1; 	 								//µ±Ç°·´±ä»»½Ç¶È
*/	
	temp = CAP4.mprtrstheta; 	 							//µ±Ç°·´±ä»»½Ç¶È 201105atzuoyun

	if(M_ChkT3Direction() == 0) temp1 = EPwm4Regs.TBPRD + EPwm4Regs.TBCTR;		//CTR¼õ¼ÆÊý  	
	else 						temp1 = 3 * EPwm4Regs.TBPRD - EPwm4Regs.TBCTR;	//CTRÔö¼ÆÊý
		
	temp1 = temp1 * CAP4.omigaslp * 1.333333e-8; 			//Ê£ÓàÊ±¼ä¶ÔÓ¦µÄ½Ç¶ÈÔöÁ¿PWMclk:75M, 1/75.0e6=1.3333e-8

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
   float temp_d,temp_q,temp1,temp2;

//------------------»ú²àµçÁ÷dqÖµ--------------------------------------------------------------------
   	TRS_MPR_I.a     = AD_OUT_MPR_I.a;
   	TRS_MPR_I.b     = AD_OUT_MPR_I.b;
   	TRS_MPR_I.c     = AD_OUT_MPR_I.c;
	Transform_3s_2s_2r(&TRS_MPR_I);
	DataFilter(0.4,&TRS_MPR_I.dflt,TRS_MPR_I.d); 			//»ú²àµçÁ÷·´À¡ÖµÂË²¨£¬ Ts=200us,fh=1.2kHz,ÂËµô¿ª¹ØÆµÂÊ´Î
	DataFilter(0.4,&TRS_MPR_I.qflt,TRS_MPR_I.q); 			//»ú²àµçÁ÷·´À¡ÖµÂË²¨£¬ Ts=200us,fh=1.2kHz,ÂËµô¿ª¹ØÆµÂÊ´Î


//----------------ÔËÐÐ»ú²àµçÁ÷»·-------------------------------------------------------------------
	if(M_ChkFlag(SL_MPR_RUNING)!=0)							//»ú²àµ÷½ÚÐèÒªÔËÐÐ
	{	

		PI_MPR_Id.reference      = RUN.mpridrf;
		PI_MPR_Id.feedback       = TRS_MPR_I.dflt; 			//»ú²àdÖáµçÁ÷»·
		PI_Loop(&PI_MPR_Id,PI_PARA_MPRID);   

		PI_MPR_Iq.reference      = RUN.mpriqrf;		  	  
		PI_MPR_Iq.feedback       = TRS_MPR_I.qflt;			//»ú²àqÖáµçÁ÷»·
  		PI_Loop(&PI_MPR_Iq,PI_PARA_MPRIQ);
    }
   
//--------------MPRÊä³öµçÑ¹Ëã¨---------------------------------------------------------------------

    DM_imrd = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * CAP4.omigasyn));
//    DM_imrq = - (TRS_NGS_U.qflt * SQRT3 * STAROTRTO / (MPR_Lm * CAP4.omigasyn));  //ÍøÑ¹qÖáÇ°À¡½âñîÏî

	temp_d = - PI_MPR_Id.out + SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Iq.feedback;	//½âñîÏî¼ÆËã

//	temp_d = temp_d	- CAP4.omigaslp * MPR_Lr * DM_imrq;								//ÍøÑ¹qÖáÇ°À¡½âñîÏî

  	temp_q = - PI_MPR_Iq.out - SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Id.feedback;	//½âñîÏî¼ÆËã
	temp_q = temp_q	- CAP4.omigaslp * MPR_Lr * DM_imrd;                               


	temp_d = temp_d - MPR_Rr * PI_MPR_Id.feedback;
	temp_q = temp_q - MPR_Rr * PI_MPR_Iq.feedback;									//×ª×Óµç×èÑ¹½µ
  
   	TRS_MPR_U.d = temp_d / STAROTRTO2;
	TRS_MPR_U.q = temp_q / STAROTRTO2;
    
    temp1 = (float)_SC_MPR_UD;
	temp2 = (float)_SC_MPR_UQ;

	if     (TRS_MPR_U.d >  temp1)     TRS_MPR_U.d =  temp1;	    //jutsttest
	else if(TRS_MPR_U.d < -temp1)     TRS_MPR_U.d = -temp1;	    //jutsttest
	if     (TRS_MPR_U.q >  temp2)     TRS_MPR_U.q =  temp2;	    //jutsttest
	else if(TRS_MPR_U.q < -temp2)     TRS_MPR_U.q = -temp2;	    //jutsttest

	Anti_Transform_2r_2s(&TRS_MPR_U);							//×ø±ê·´±ä»»µ½Á½Ïà¾²Ö¹Ïµ

//------------SVMÂö¿í¼ÆËãºÍÂö³å·¢Éú-----------------------------------------------------------------	
	SVPWM_MPR(TRS_MPR_U.alfa,TRS_MPR_U.beta);					//Âö³å·¢Éú

//------------TeÊµ¼Ê×ª¾Ø·´À¡Öµ¼ÆËã-----------------------------------------------------------------	
//	Te_feedback = 1.5 * POLEPAIRES * MPR_Lm * TRS_NGS_U.dflt * SQRT3 * TRS_MPR_I.qflt / (314.15926 * STAROTRTO * MPR_Ls);   // Te=1.5*np*(Us/w1)*(Irq/K)*(Lm/Ls)
//	Te_feedback = IRQTOTE * TRS_NGS_U.dflt *  PI_MPR_Iq.feedback;   // Te=1.5*np*(Us/w1)*(Irq/K)*(Lm/Ls)
//	DataFilter(0.999,&DIP_MPR_I.qflt,TRS_MPR_I.q); 					//¶¨×Ó²à¹¦ÂÊ¼ÆËã,»ú²àqµçÁ÷£¬ Ts=200us,fh=Hz
	DataFilter(0.8,&DIP_MPR_I.qflt,TRS_MPR_I.q); 					//20111116 ¼õÐ¡×ª¾ØÂË²¨Ê±¼ä³£Êý ÓÉ1s¸ÄÎª4ms
	Te_feedback = - IRQTOTE * DIP_NPR_U.dflt *  DIP_MPR_I.qflt;     // Te=-1.5*np*(Us/w1)*(Irq/K)*(Lm/Ls),MPR_iÎª¸ºÖµ
	if(Te_feedback<=0)	Te_feedback=0;   							//20090817
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
	float temp_d,temp_q;

//-----------¼ÆËãÍø²à±äÁ÷Æ÷µçÁ÷dqÖµ-----------------------------------------------------------------
	TRS_NPR_I.a = AD_OUT_NPR_I.a;  							//»ñÈ¡Íø²à±äÁ÷Æ÷µçÁ÷
	TRS_NPR_I.b = AD_OUT_NPR_I.b;
	TRS_NPR_I.c = AD_OUT_NPR_I.c;		
	Transform_3s_2s_2r(&TRS_NPR_I);							//×ø±ê±ä»»
	DataFilter(0.4,&TRS_NPR_I.dflt,TRS_NPR_I.d); 			//Íø²àµçÁ÷·´À¡ÖµÂË²¨£¬ Ts=200us,fh=1.2kHz,ÂËµô¿ª¹ØÆµÂÊ´Î
	DataFilter(0.4,&TRS_NPR_I.qflt,TRS_NPR_I.q); 			//Íø²àµçÁ÷·´À¡ÖµÂË²¨£¬ Ts=200us,fh=1.2kHz£¬20090615¸Ä

//----------¼ÆËãÍøÑ¹dqÖµ----------------------------------------------------------------------------
	TRS_NGS_U.a = AD_OUT_NGS_U.a;							//»ñÈ¡µçÍøµçÑ¹
	TRS_NGS_U.b = AD_OUT_NGS_U.b;
	TRS_NGS_U.c = AD_OUT_NGS_U.c;
	Transform_3s_2s_2r(&TRS_NGS_U);							//×ø±ê±ä»»

	DataFilter(0.4,&TRS_NGS_U.dflt,TRS_NGS_U.d); 			//ÍøÑ¹·´À¡ÖµÂË²¨£¬Ts=200us,fh=1.2kHz t=132us 20090608change to ok
	DataFilter(0.4,&TRS_NGS_U.qflt,TRS_NGS_U.q); 			//ÍøÑ¹·´À¡ÖµÂË²¨£¬Ts=200us,fh=1.2kHz t=132us 20090608change to ok
//	DataFilter(0.062,&TRS_NGS_U.dflt,TRS_NGS_U.d); 			//ÍøÑ¹·´À¡ÖµÂË²¨£¬Ts=200us,fh=12kHz 20100507change at zuoyun
//	DataFilter(0.062,&TRS_NGS_U.qflt,TRS_NGS_U.q); 			//ÍøÑ¹·´À¡ÖµÂË²¨£¬Ts=200us,fh=12kHz 20100507change at zuoyun

	DataFilter(0.1,&TRS_NGS_U.dflt2,TRS_NGS_U.d); 			//ÍøÑ¹·´À¡ÖµÂË²¨£¬Ts=200us,fh=7.9kHz,126us,Îª¼à²âÍøµø£¬20091026


	if(M_ChkFlag(SL_NPR_RUNING)!=0)							//Íø²àPIÔËÐÐ¿ØÖÆÌõ¼þ
	{

//----------ÔËÐÐµçÑ¹Íâ»·----------------------------------------------------------------------------
	  if(M_ChkFlag(SL_STEADYGV)==0)  						//VdcÃ»ÓÐÎÈ¶¨£¬ÇÒ²ÉÓÃÎÈÌ¬PI²ÎÊý
	  {

		PI_NPR_U.reference     = - RUN.urf;   				//»ñÈ¡ÖÐ¼äµçÑ¹Ö¸Áî
		PI_NPR_U.feedback      = - AD_OUT_UDC;				//»ñÈ¡ÖÐ¼äµçÑ¹·´À¡Öµ
    	PI_Loop(&PI_NPR_U,PI_PARA_DYNU);
      }	
	  else                                                   //²ÉÓÃÎÈÌ¬PI²ÎÊý
	  {
		PI_NPR_U.reference     = - RUN.urf;   				//»ñÈ¡ÖÐ¼äµçÑ¹Ö¸Áî
		PI_NPR_U.feedback      = - AD_OUT_UDC;				//»ñÈ¡ÖÐ¼äµçÑ¹·´À¡Öµ
    	PI_Loop(&PI_NPR_U,PI_PARA_NPRU); 
      }
    	
//---------ÔËÐÐdÖáµçÁ÷»·----------------------------------------------------------------------------
   		PI_NPR_Id.reference      = PI_NPR_U.out; 			//»ñÈ¡dÖáµçÁ÷¸Áî£¬×¢ÒâÕý¸ººÅ
		PI_NPR_Id.feedback       = TRS_NPR_I.dflt;			//»ñÈ¡dÖáµçÁ÷·´À¡ÂË²¨Öµ
		PI_Loop(&PI_NPR_Id,PI_PARA_NPRID);

//---------ÔËÐÐqÖáµçÁ÷»·----------------------------------------------------------------------------
		PI_NPR_Iq.reference      = RUN.npriqrf; 			//qÖáµçÁ÷Ö¸Áî
		PI_NPR_Iq.feedback       = TRS_NPR_I.qflt;	  		//»ñÈ¡qÖáµçÁ÷·´À¡ÂË²¨Öµ
		PI_Loop(&PI_NPR_Iq,PI_PARA_NPRIQ);	  	  
	}

//---------ÆËãÐý×ªÏµÏÂNPR¿ØÖÆµçÑ¹------------------------------------------------------------------

	temp_d = PI_NPR_Id.out  + TRS_NGS_U.dflt - CAP4.omigasyn * NPR_L * PI_NPR_Iq.feedback;
	temp_q = PI_NPR_Iq.out  + TRS_NGS_U.qflt + CAP4.omigasyn * NPR_L * PI_NPR_Id.feedback;
	
  	TRS_NPR_U.d = temp_d;
  	TRS_NPR_U.q = temp_q;

	Anti_Transform_2r_2s(&TRS_NPR_U); 						//antitransform to static axis

//---------SVMÂö¿í¼ÆËã-----------------------------------------------------------------------------
	SVPWM_NPR(TRS_NPR_U.alfa,TRS_NPR_U.beta);				//Âö³å·¢Éú

//------------Íø²àÓÐ¹¦¹¦ÂÊ¼ÆËã(3Ïà)-----------------------------------------------------------------	
	DataFilter(0.99,&DIP_NPR_I.dflt,TRS_NPR_I.d); 			//¶¨×Ó²à¹¦ÂÊ¼ÆËã,ÍøÑ¹ed£¬ Ts=200us,fh=88Hz
	DataFilter(0.99,&DIP_NPR_I.qflt,TRS_NPR_I.q); 			//¶¨×Ó²à¹¦ÂÊ¼ÆËã,»ú²àqµçÁ÷£¬ Ts=200us,fh=88Hz
	DataFilter(0.99,&DIP_NPR_U.qflt,TRS_NGS_U.q); 			//¶¨×Ó²à¹¦ÂÊ¼ÆËã,»ú²àqµçÁ÷£¬ Ts=200us,fh=88Hz
	DataFilter(0.995,&DIP_NPR_U.dflt,TRS_NGS_U.d); 			//¶¨×Ó²à¹¦ÂÊ¼ÆËã,ÍøÑ¹ed£¬ Ts=200us,Tr=250ms

	Pnactive   = 1.5 * (DIP_NPR_U.dflt * DIP_NPR_I.dflt + DIP_NPR_U.qflt * DIP_NPR_I.qflt);   	// Pnactive=3*Ud*Id/(squt2*squt2)
	Pnreactive = 1.5 * (DIP_NPR_U.qflt * DIP_NPR_I.dflt - DIP_NPR_U.dflt * DIP_NPR_I.qflt);   	// Pnactive=3*Ud*Id/(squt2*squt2)

}


/**************************************************************************************************
** Ãû³Æ: SysCtrl
** ¹¦ÄÜÃèÊö: 1.5MW±äÁ÷Æ÷ÏµÍ³Âß¼­¿ØÖÆ³ÌÐò--CPCµç»úÊµÑé 	
** Êä¡¡Èë:   	
** Êä¡¡³ö: 		 
** ×¢  ÊÍ: 			
**-------------------------------------------------------------------------------------------------
** ×÷¡¡Õß: 	 
** ÈÕ¡¡ÆÚ: 		
**-------------------------------------------------------------------------------------------------
** ÐÞ¸ÄÈË:
** ÈÕÆÚ:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void SysCtrl(void)         
{    

//1-----·¢ÉúPDP»òÕßÑÏÖØ²»¿É»Ö¸´¹ÊÕÏ-----------------------//     
 if(M_ChkFlag(SL_SERIESTOP)!=0 || M_ChkFlag(SL_SERIESTOPING)!=0)   //ÏµÍ³·¢ÉúÑÏÖØ¹ÊÕÏ»òÕßÕýÔÚÍ£»ú 20091015 at zy
 {

		M_SetFlag(SL_SERIESTOPING);         //ÖÃÑÏÖØ¹ÊÕÏÕýÔÚÍ£»ú±êÖ¾ 20091015 at zy
//		M_ClrFlag(SL_DISPLAY6);          	//ÇåNPRÂö³å´ý»úÖ¸Ê¾
//    	M_ClrFlag(SL_DISPLAY7);          	//Çå±äÁ÷Æ÷ÔËÐÐ´ý»úÖ¸Ê¾
		MAIN_LOOP.cnt_cboff=0;           	//if.2
          MAIN_LOOP.cnt_closecb=0;       	//if.3
	  
	   	M_ClrFlag(CL_CB);    		     	//ÇåÖ÷¶ÏºÏÕ¢Ö¸Áî
	   	M_ClrFlag(CL_CBENGSTRG);         	//ÇåÖ÷¶Ï´¢ÄÜÖ¸Áî£¬Í¬Ê±Ç·Ñ¹Á´¶Ï¿ª£¬Ö÷¶Ï¶Ï¿ª 
	   	M_ClrFlag(CL_CBFCON);            	//¹ÊÕÏÁ´¶Ï¿ª

	  	M_ClrFlag(SL_OCS_NPRSTART);         //ÇåÍø²àÔËÐÐÖ¸Áî 20090815
//	   	M_ClrFlag(SL_STEADYFB);          	//ÇåVdcÎÈ¶¨±êÖ¾  20091015atzy
	   	M_ClrFlag(SL_OCS_MPRSTART);	   	 	//Çå»ú²àÔËÐÐÖ¸Áî 20090815
//      M_ClrFlag(SL_CHARGEOK);          	//ÇåÔ¤³äµçÍê³É   20091015atzy
	   	M_ClrFlag(SL_CBCLOSED);			 	//ÇåÖ÷¶Ï±ÕºÏÍê³É 20091022atzy
	   
		 
	   if(M_ChkCounter(MAIN_LOOP.cnt_opencontac,DELAY_OPENCONTAC)>0)	//2s Ö÷¶Ï¶Ï¿ªÑÓÊ±ºóÔÙ¶Ï¶¨×Ó½Ó´¥Æ÷ºÍÖ÷½Ó´¥Æ÷
	   {
		   M_ClrFlag(CL_MAINK);    		  	//¶ÏÖ÷½Ó´¥Æ÷ºÍÂË²¨Æ÷
		   M_ClrFlag(CL_PRE); 				//ÇåÔ¤³äµç½Ó´¥Æ÷
		   M_ClrFlag(CL_STATORK);    	  	//¶Ï¶¨×Ó½Ó´¥Æ÷	
		   M_ClrFlag(SL_CHARGEOK);          //ÇåÔ¤³äµçÍê³É    20091015atzy
	   }

	   if(AMUX.skiiptempmax<50.0)	 M_ClrFlag(SL_FAN_WORK);          	//¹Ø±Õ¹¦ÂÊ×é¼þ·ç»ú  20090816

	   if(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(SL_IN1_MIANK)==0 && M_ChkFlag(SL_IN1_STATORK)==0) M_ClrFlag(SL_SERIESTOPING); //20091015 at zy

 }	//end of NO.1 if

  

//2----½ÓÊÕµ½SL_OCS_EIN/BIT0=0Ê±£¬¶Ï¶¨×Ó½Ó´¥Æ÷¡¢Í£Âö³å£¬¶ÏÖ÷½Ó´¥Æ÷ÂË²¨Æ÷ºÍÖ÷¶Ï------------------------- 

  else if((M_ChkFlag(SL_OCS_EIN)==0) || (M_ChkFlag(SL_EINSTOPING)!=0))
  {
	   	MAIN_LOOP.cnt_opencontac=0;        			//if.1
         MAIN_LOOP.cnt_closecb=0;         			//if.3

		M_SetFlag(SL_EINSTOPING);    				//20090817
		M_ClrFlag(SL_OCS_SYSRUN); 					//ÇåÏµÍ³ÔËÐÐºÍ·¢Âö³åÖ¸Áî£¬µÈ´ýÂö³åÍ£Ö¹ºÍMC¶Ï¿ª
    	M_ClrFlag(SL_OCS_NPRSTART);
		M_ClrFlag(SL_OCS_MPRSTART);
  
		if((M_ChkFlag(SL_NPR_PWMOUT)==0)&&(M_ChkFlag(SL_MPR_PWMOUT)==0)&&(M_ChkFlag(SL_CHARGEOK)==0))
		{
	  		if(M_ChkFlag(SL_IN1_CBSTS)!=0)    		//Ö÷½Ó´¥Æ÷\Ö÷ÂË²¨Æ÷ÒÑ¾­¶Ï¿ª ÇÒÖ÷¶ÏÂ·Æ÷±ÕºÏ×´Ì¬
	  		{
	    		if(M_ChkCounter(MAIN_LOOP.cnt_cboff,DELAY_CBOFF)>0)  //1s
				{
			  		M_ClrFlag(CL_CB);    		    //ÇåÖ÷¶ÏºÏÕ¢Ö¸Áî
			  		M_ClrFlag(CL_CBENGSTRG);      	//ÇåÖ÷¶Ï´¢ÄÜÖ¸Áî£¬Í¬Ê±Ç·Ñ¹Á´¶Ï¿ª£¬Ö÷¶Ï¶Ï¿ª
			 		M_ClrFlag(CL_CBFCON);
				}	     
	 
	  		}
	  		else   
	  		{
//	        	M_ClrFlag(SL_DISPLAY6);           	//ÇåNPRÂö³å´ý»úÖ¸Ê¾
//				M_ClrFlag(SL_DISPLAY7);           	//Çå±äÁ÷Æ÷ÔËÐÐ´ý»úÖ¸Ê¾
				M_ClrFlag(CL_CB);    		      	//ÇåÖ÷¶ÏºÏÕ¢Ö¸Áî
		    	M_ClrFlag(CL_CBENGSTRG);          	//ÇåÖ÷¶Ï´¢ÄÜÖ¸Áî
		    	M_ClrFlag(CL_CBFCON);	
		    	M_ClrFlag(CL_PRE);                	//ÇåÔ¤³ä½Ó´¥Æ÷
		    	M_ClrFlag(SL_CHARGEOK);           	//ÇåÔ¤³äµçÍê³É
            	M_ClrFlag(SL_OCS_NPRSTART);        	//ÇåÉÏÎ»»úNPR·¢Âö³åÖ¸Áî
	        	M_ClrFlag(SL_OCS_MPRSTART);        	//ÇåÉÏÎ»»úMPR·¢Âö³åÖ¸Áî
				M_ClrFlag(CL_MAINK);    		  	//¶ÏÖ÷½Ó´¥Æ÷ºÍÂË²¨Æ÷
				M_ClrFlag(SL_CBCLOSED);				//ÇåÖ÷¶Ï±ÕºÏÍê³É 
				if(AMUX.skiiptempmax<50.0)	 M_ClrFlag(SL_FAN_WORK);     //¹Ø±Õ¹¦ÂÊ×é¼þ·ç»ú 20090816
				M_ClrFlag(SL_EINSTOPING);    		//20090817    		
	  		}
		}
		else    MAIN_LOOP.cnt_cboff=0;
	 	
   } //end of NO.2 if


//3----------- ½ÓÊÕµ½SL_OCS_EIN/BIT0=1µÄÏµÍ³Æô¶¯Ö¸Áî£¬ºÏÖ÷¶Ï£¬Ô¤³äµç£¬ºÏÖ÷½Ó´¥Æ÷/ÂË²¨Æ÷£¬·¢Âö³å

  else                                         		//ÏµÍ³ÎÞ¹ÊÕÏ£¬½ÓÊÕµ½BIT0=1£¬Ö´ÐÐÏµÍ³Æô¶¯Ö¸ÁîÖ±ÖÁÖ÷¶Ï±ÕºÏ 
  {
      	MAIN_LOOP.cnt_opencontac=0;        			//if.1
	   	 MAIN_LOOP.cnt_cboff=0;            			//if.2
   		if(M_ChkFlag(SL_CBCLOSED)==0) 				//Ö÷¶Ï»¹Ã»ÓÐ±ÕºÏ£¬Ö´ÐÐÖ÷¶Ï±ÕºÏ³ÌÐò
   		{
      		if(M_ChkFlag(SL_IN1_CBSTS)==0)      	//Ö÷¶ÏÂ·Æ÷¶Ï¿ª
	  		{
	     	 	M_SetFlag(CL_CBENGSTRG);     		//½ÓÊÕµ½±ÕºÏCBÑÓÊ±µ½,¿ªÊ¼´¢ÄÜ
			 	M_SetFlag(CL_CBFCON);

			 	if(M_ChkCounter(MAIN_LOOP.cnt_closecb,DELAY_CLOSE_CB)>0)  M_SetFlag(CL_CB);   //·¢³öCBºÏÕ¢Ö¸Áî(Âö³å¼´¿É) 5s
	  		}      
      		else	 
      		{
      			M_SetFlag(SL_CBCLOSED);             //ÏµÍ³Ö÷¶ÏºÏÕ¢Íê³É
				M_ClrFlag(CL_CB);  					//Ö÷¶Ï±ÕºÏºó£¬¾ÍÇå³ýCBºÏÕ¢(ÒòÎªºÏÕ¢½öÐèÒªÒ»¸öÂö³å¼´¿É)
      		}

   		}
   		else  MAIN_LOOP.cnt_closecb=0;             	//Ö÷¶ÏÂ·Æ÷±ÕºÏÒÑ¾­Íê³É
  
  }	 
	  

//4----------Ö÷¶Ï±ÕºÏºó£¬½ÓÊÕµ½Ö÷¿Ø±äÁ÷Æ÷ÔËÐÐÖ¸Áî---------------------------------------------------------------------------------

  if(M_ChkFlag(SL_CBCLOSED)!=0)   
  {           
  	if((M_ChkFlag(SL_OCS_SYSRUN)!=0)&&(M_ChkFlag(SL_SERIESTOP)==0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SYSSTOPING)==0))
	{
      MAIN_LOOP.cnt_mkoff=0;
      if(M_ChkFlag(SL_CHARGEOK)==0)				//Î´½øÐÐÔ¤³äµç
	  {      	
      	
      	if(M_ChkFlag(SL_SPEED_IN_RANGE)!=0)     //×ªËÙÔÚ·¶Î§ÄÚ
		{
//	        M_ClrFlag(SL_DISPLAY7);    			//Çå±äÁ÷Æ÷ÔËÐÐ´ý»úÖ¸Ê¾     			
			M_SetFlag(SL_FAN_WORK);       		//¿ªÆô¹¦ÂÊ×é¼þ·ç»ú  20090816

      		if((M_ChkFlag(SL_IN1_MIANK)==0)||(M_ChkFlag(SL_IN1_MIANFILTER)==0))        //Ö÷¶ÏÂ·Æ÷ÒÑ±ÕºÏ,ÇÒÖ÷½Ó´¥Æ÷/ÂË²¨Æ÷ÆäÒ»ÓÐ¶Ï¿ª
	  		{	       	     		 		              
	       		M_SetFlag(CL_PRE);          	 //±ÕºÏÔ¤³äµç½Ó´¥Æ÷
		   		if((AD_OUT_UDC>=_SC_VDCON)&&(M_ChkCounter(MAIN_LOOP.cnt_precok,DELAY_PRECOK)>=0))  M_SetFlag(CL_MAINK);  //8s      //Ô¤³äµçOK£¬±ÕºÏÖ÷½Ó´¥Æ÷ 9s   
	  		} 
	  		else
	  		{
		 		M_ClrFlag(CL_PRE);          	      //¶Ï¿ªÔ¤³äµç½Ó´¥Æ÷£¬Ó²¼þÉÏ»áÔÚMF/MC±ÕºÏºóÊ¹Æä¶Ï¿ª£¬µ«Èí¼þÒ²Òªå¿ØÖÆÎ»
		 		M_SetFlag(SL_CHARGEOK);               //ÏµÍ³×¼±¸¾ÍÐ÷         
      		}
		}
		else	MAIN_LOOP.cnt_precok=0;
	  }
	  else 
	  {
	 		if(AD_OUT_UDC >= 870.0)   M_SetFlag(SL_OCS_NPRSTART);	 //ÏµÍ³ÉÏµçÍê³É£¬ÑÓÊ±ºóÆô¶¯NPR 1s   
		 	MAIN_LOOP.cnt_precok=0;
	  }
	}
	else   //SYSRUNÍ£»ú¹ý³Ì
	{
   		M_SetFlag(SL_SYSSTOPING);  //20090817
 
  		M_ClrFlag(SL_OCS_NPRSTART);
		M_ClrFlag(SL_OCS_MPRSTART);
  		if((M_ChkFlag(SL_NPR_PWMOUT)==0)&&(M_ChkFlag(SL_MPR_PWMOUT)==0))
		{
  	  		if((M_ChkFlag(SL_IN1_MIANK)!=0)||(M_ChkFlag(SL_IN1_MIANFILTER)!=0))     //Ö÷½Ó´¥Æ÷±ÕºÏ×´Ì¬
	 		{
         		if(M_ChkCounter(MAIN_LOOP.cnt_mkoff,DELAY_MKOFF)>0)                 //0.2s
	     		{
			  		M_ClrFlag(CL_MAINK);    		  								// ¶ÏÖ÷½Ó´¥Æ÷ºÍÂË²¨Æ÷
	          		M_ClrFlag(CL_PRE);    		  									// ÔÙÇåÒ»´ÎÔ¤³äµçÖ¸Áî	
		 		}	        	    		  	     
	  		}	
	  		else    
	  		{
	       		M_ClrFlag(SL_CHARGEOK);
				M_ClrFlag(SL_SYSSTOPING);  //20090817
				if(AMUX.skiiptempmax<50.0)	 M_ClrFlag(SL_FAN_WORK);     			//¹Ø±Õ¹¦ÂÊ×é¼þ·ç»ú 20090816
//				if(M_ChkFlag(SL_ERRSTOP)==0) M_SetFlag(SL_DISPLAY7);    			//ÖÃ±äÁ÷Æ÷ÔËÐÐ´ý»úÖ¸Ê¾
		  		MAIN_LOOP.cnt_mkoff=0;
      		}
    	}
		else   MAIN_LOOP.cnt_mkoff=0;
	}
  }
  else
  {
		M_ClrFlag(SL_CHARGEOK);
		MAIN_LOOP.cnt_precok=0;
		MAIN_LOOP.cnt_mkoff=0;
//		M_ClrFlag(SL_DISPLAY7);    					//Çå±äÁ÷Æ÷ÔËÐÐ´ý»úÖ¸Ê¾ 
  }

//5----------- Ö÷½Ó´¥Æ÷/ÂË²¨Æ÷ÒÑ±ÕºÏÇÒÎÞ¹ÊÕÏ,µÈ´ýÉÏÎ»»úÍø²àÆô¶¯Ö¸Áî£¬·¢Âöå	  
	  
	if((M_ChkFlag(SL_CBCLOSED)!=0)&&(M_ChkFlag(SL_CHARGEOK)!=0)&&(M_ChkFlag(SL_SERIESTOP)==0)&&(M_ChkFlag(SL_ERRSTOP)==0))   
	{
	   if(M_ChkFlag(SL_OCS_NPRSTART)!=0)
	   {	   
//	       M_ClrFlag(SL_DISPLAY6);            									//ÇåNPRÂö³å´ý»úÖ¸Ê¾
         		 
           if(M_ChkFlag(SL_CONFIGPWM)!=0)    M_SetFlag(SL_NPR_START);		   	//pwmdriveÅäÖÃ¿ª¹ØÆµÂÊÍê³É,Íø²à±äÁ÷Æ÷ÔËÐÐ		  
	       else   	                         M_ClrFlag(SL_NPR_START);		   	//ûÖ¹Íø²à±äÁ÷Æ÷ÔËÐÐ 

           //---------ÅÐ¶ÏVdcÊÇ·ñÎÈ¶¨start	          
	       if((AD_OUT_UDC>(_URF - 20))&&(AD_OUT_UDC<(_URF + 20)))   			//ÅÐ¶ÏÖ±Á÷µçÑ¹ÊÇ·ñÎÈ¶¨ÔÚ_URF(1100V)²¢ÇÒÒÑ­ÁË³ÖÐøÒ»¶ÎÊ±¼ä£¬ÒÔÊ¹ÄÜ»ú²àÂö³å
	   	   {   
	   		  M_SetFlag(SL_DISPLAY2);                							//ÖÃVdcÎÈ¶¨Ö¸Ê¾
	   		  if(M_ChkFlag(SL_MPR_START)==0)         							//Ö»ÔÚ»ú²àÎ´·¢Âö³åÇ°¼ì²â£¬»ú²àÔËÐÐÆÚ¼äVdc²»ÎÈ²»ÔÙÇåSL_STEADYFB  
	   		  {                                                					//ÉÏÏÂÆ«²îÐ¡ÓÚ20V
	     	    if(M_ChkCounter(MAIN_LOOP.cnt_steadyfb,DELAY_STEADYFB)>0)	  M_SetFlag(SL_STEADYFB);  //2s
		      }
		      else   MAIN_LOOP.cnt_steadyfb=0;
		   }
		   else  
		   {
		   	 MAIN_LOOP.cnt_steadyfb=0;
			 M_ClrFlag(SL_DISPLAY2);             								//ÇåVdcÎÈ¶¨Ö¸Ê¾

		   }	        
	       //---------ÅÐ¶ÏVdcÊÇ·ñÎÈ¶¨end
	     	    
		   if(M_ChkFlag(SL_STEADYFB)!=0)
		   {
		      if(M_ChkCounter(MAIN_LOOP.cnt_mprstart,DELAY_MPRSTART)>=0)   M_SetFlag(SL_OCS_MPRSTART);	 //VdcÒÑ¾­ÎÈ¶¨£¬ÑÓÊ±ºóÆô¶¯MPR 1s  
           }
		   else  MAIN_LOOP.cnt_mprstart=0;

	   }
	   else					  													
	   {
	      if(M_ChkFlag(SL_MPR_PWMOUT)==0)   M_ClrFlag(SL_NPR_START);  			//»ú²àÂö³å·âËøÒÔºóÔÙ½«Íø²à±äÁ÷Æ÷ÔËÐÐÖ¸ÁîÇåÁã

          if(M_ChkFlag(SL_NPR_PWMOUT)==0)      
          {
             M_ClrFlag(SL_STEADYFB);
//             M_SetFlag(SL_DISPLAY6);                  							//Íø²àÂö³å·âËø, ÖÃNPRÂö³å´ý»úÖ¸Ê¾
			 M_ClrFlag(SL_DISPLAY2);
		  }
		  MAIN_LOOP.cnt_steadyfb=0;	   
		  MAIN_LOOP.cnt_mprstart=0; 
	   }

    }	   
	else                  
    {   
//	    M_ClrFlag(SL_DISPLAY6);            //ÇåNPRÂö³å´ý»úÖ¸Ê¾
		M_ClrFlag(SL_NPR_START);
		M_ClrFlag(SL_OCS_NPRSTART);	
		M_ClrFlag(SL_STEADYFB);
		M_ClrFlag(SL_DISPLAY2);
		MAIN_LOOP.cnt_steadyfb=0;
		MAIN_LOOP.cnt_mprstart=0;
	}	


//6----------- Ö÷½Ó´¥Æ÷/ÂË²¨Æ÷ÒÑ±ÕºÏÇÒÎÞ¹ÊÕÏ,µÈ´ýÉÏÎ»»ú»ú²àÆô¶¯Ö¸Áî£¬·¢Âö³å	  
	  
	if((M_ChkFlag(SL_CBCLOSED)!=0)&&(M_ChkFlag(SL_CHARGEOK)!=0)&&(M_ChkFlag(SL_SERIESTOP)==0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_NPR_PWMOUT)!=0))   
	{
      if((M_ChkFlag(SL_OCS_MPRSTART)!=0)&&(M_ChkFlag(SL_STEADYFB)!=0))		  								//Íø²àÔËÐÐ,ÇÒVdcÎÈ¶¨
	  {
		M_SetFlag(SL_MPR_START);             								//ÖÃ»ú²à±äÁ÷Æ÷ÔËÐÐÖ¸Áî

		if(M_ChkFlag(SL_IN1_STATORK)==0)      								//¶¨×Ó½Ó´¥Æ÷´¦ÓÚ¶Ï¿ª×´Ì¬
	 	{
		   if((MEAN_DATA.uab_d <= _SC_UDSTAC)&&(MEAN_DATA.ubc_d <= _SC_UDSTAC))          //¶¨×Ó½Ó´¥Æ÷Ç°ºó°ë²¨Æ½¾ùÖµ²îÔÚ40VÒÔÄÚ
		   {
		      M_SetFlag(SL_DISPLAY4);
		      if(_AUTOCSTAC !=0)    M_SetFlag(CL_STATORK);                               //Íâ²¿¿ØÖÆ£¬Îª1²ÅÔÊÐí±ÕºÏSTAC
			  else					M_ClrFlag(CL_STATORK);		        
		   }
		   else   M_ClrFlag(SL_DISPLAY4);      
	 	}
		else	M_SetFlag(SL_MPR_SYNOK);  									//»ú²àÍ¬²½²¢ÍøÍê³É±êÖ¾	
	  }
	  else  
	  {
		 M_ClrFlag(SL_MPR_START);
		 M_ClrFlag(SL_DISPLAY4);											// Í£»ú²à±äÁ÷Æ÷ÔËÐÐÖ¸Áî

		 if(M_ChkFlag(SL_IN1_STATORK)!=0)      								//
	 	 {
         	if(M_ChkFlag(SL_STACTRIPEN)!=0)	  M_ClrFlag(CL_STATORK);        //´ïµ½¶¨×Ó¶Ï¿ªÌõ¼þ, ¶Ï¿ª¶¨×Ó½Ó´¥Æ÷		            
	 	 }
		 else      M_ClrFlag(SL_MPR_SYNOK);
		
	  }
	}
	else
	{
	   M_ClrFlag(SL_MPR_START);                								//Í£Ö¹»ú²à±äÁ÷Æ÷ÔËÐÐÖ¸Áî
	   M_ClrFlag(SL_OCS_MPRSTART);	
	   if(M_ChkFlag(SL_SERIESTOP)==0)	M_ClrFlag(CL_STATORK); 
	   M_ClrFlag(SL_MPR_SYNOK);
	   M_ClrFlag(SL_DISPLAY4);
	}    


//======================ÏµÍ³ÑÏÖØ¹ÊÕÏ·¢Éú²¢ÇÒÏû³ýÒÔºó£¬Íâ²¿·¢³ö¸´Î»ÐÅºÅ,Ö»ÄÜÔÚÏµÍ³Í£»úºó¸´Î»=================//
//======================Ò»°ã¹ÊÕÏ·¢Éú²¢ÇÒÏû³ýÒÔºó£¬Íâ²¿·¢³ö¸´Î»ÐÅºÅ,ÔÚÍø²àºÍ»ú²à¶¼Í£Âö³åºó¼´¿É¸´Î»=================//
    if((M_ChkFlag(SL_OCS_RESET)!=0)&& ((M_ChkFlag(SL_SERIESTOP)!=0)||(M_ChkFlag(SL_ERRSTOP)!=0)) &&(M_ChkFlag(SL_CHARGEOK)==0))  //20090815
	{
	   if(M_ChkFlag(SL_SERIESTOP)!=0)   //20091015 at zy
	   {
	   	 if(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(SL_IN1_MIANK)==0 && M_ChkFlag(SL_IN1_STATORK)==0)   M_SetFlag(SL_RESET);
		 else                            																M_ClrFlag(SL_RESET);
	   }
	   else	   M_SetFlag(SL_RESET); 
	}
	else   M_ClrFlag(SL_RESET);

//====================¹ÊÕÏ·¢Éú²¢ÇÒÏû³ýÒÔºó£¬²Ù×÷Æ÷·¢³ö¸´Î»ÐÅºÅ---½áÊø==============// 
}

/////////////////no more////////////////////
