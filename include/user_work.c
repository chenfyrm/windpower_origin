   /****************************************Copyright (c)**************************************************
**                       		     ��	��	��	ͨ	��	ѧ
**                                        ��������ѧԺ
**                                         604ʵ����
** 
**                              
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: user_work.c
**��   ��   ��: 
**����޸�����: 
**��        ��: ����1.5MW˫�������鲢������������������Ŀ��Ƴ���----201005atcpc-201007atbjtu
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
#include "DSP2833x_Device.h"     // Headerfile Include File
#include "DSP2833x_Examples.h"   // Examples Include File
#include "math.h"

//��������systest201005atcpc
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
** ��������: InitWork
** ��������: ��ʼ��WORK���֣�����GIVE��RunCtrl��WAVE��SCOUT
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
void InitWork(void)
{
	M_SetFlag(SL_POWERON);		//�ó����ϵ��־
	_MSG_SCOUT1=MSG_NONE;		//������Ϣ���
	_MSG_SCOUT2=MSG_NONE;		//������Ϣ���
	MAIN_LOOP.pulse = 0;		//��ʱ���帳��ֵ
	M_ClrFlag(SL_OCS_NPRSTART);
    M_ClrFlag(SL_OCS_MPRSTART);
	M_ACROWBAROFF();			//ACTIVECROWBAR�ض�
	if(GpioDataRegs.GPBDAT.bit.GPIO50==0)	DEBUG_OPER=1;  //mode0ʹ�ܣ�������ʼ����ֵ
	else									DEBUG_OPER=0;  //20100401at27

}


/*********************************************************************************************************
** ��������: PwmDrive
** ��������: 
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
void PwmDrive(void)
{
    if((M_ChkFlag(SL_POWERON)==0)&&(M_ChkFlag(SL_CHARGEOK)!=0))		//DSP���������ϵ磬�������,ϵͳԤ������
 	{ 	
		    //--����ִ�е��ǿ���Ƶ�����ã���ͣ��״̬��������������ÿ���Ƶ�ʣ�ÿ������ʱ����һ�ο���Ƶ��----
			if(M_ChkFlag(SL_OCS_NPRSTART)!=0)      					//����Ҫ�����������
			{
			   if(M_ChkFlag(SL_CONFIGPWM)==0 && M_ChkFlag(SL_NPR_PWMOUT)==0 && M_ChkFlag(SL_MPR_PWMOUT)==0)  //ֻ��ͣ�����˲�����������PWM						//û�����ÿ���Ƶ��
		       {								  
				   ConfigPwm();	                					//���ÿ���Ƶ��				
				   if(M_ChkCounter(MAIN_LOOP.cnt_pwmout,DELAY_PWMOUT)>=0)	M_SetFlag(SL_CONFIGPWM);  //���巢��������ʱ��	20091015 at zy						//��������,��֤EnPwm()ֻ������һ��       
		       }
			   else	 MAIN_LOOP.cnt_pwmout=0;						//���ϵͳ����������ʱ	
			}
			else   
			{
			   M_ClrFlag(SL_CONFIGPWM);
			   MAIN_LOOP.cnt_pwmout=0;								//���ϵͳ����������ʱ
			}

//----------------����������������ʹ�����һ��----------------
	        
	 //--------�����������������,��û�й���--------------
//	        if((M_ChkFlag(SL_NPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0)) 
	        if((M_ChkFlag(SL_NPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0)&&(M_ChkFlag(SL_LV_NSTOP)==0)) //201007BJTULVRT
	        {
		       if(M_ChkFlag(SL_NPR_PWMOUT)==0)  					//��֤Enepwmio_NPR()ֻ������һ��
		       {
			      Enepwmio_NPR();
			          	
				  M_SetFlag(SL_RUN);
			      M_SetFlag(SL_NPR_PWMOUT);			        
		       }
	        }
			else       												//�������Ͼ�����������
	        {
		       Disepwmio_NPR(); 
		              
			   M_ClrFlag(SL_NPR_PWMOUT);
			   M_ClrFlag(SL_RUN);
	        }
     //------�������������ʹ��,��û�й���---------------
//            if((M_ChkFlag(SL_MPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0))
            if((M_ChkFlag(SL_MPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0)&&(M_ChkFlag(SL_LV_MSTOP)==0)) //201007BJTULVRT
	        {
		       if(M_ChkFlag(SL_MPR_PWMOUT)==0)	//��֤Enepwmio_MPR()ֻ������һ��
		       {
			      Enepwmio_MPR();      
			      M_SetFlag(SL_MPR_PWMOUT);
			      M_SetFlag(SL_LV_MPWMOUT);		// LVRT	         
		       }
			   MAIN_LOOP.cnt_lv_mpwmstop=0;
	        }
	        else                  //�������Ͼ�����������
	        {		       
			   Disepwmio_MPR();       
			   M_ClrFlag(SL_MPR_PWMOUT); 
			   if(M_ChkCounter(MAIN_LOOP.cnt_lv_mpwmstop,DELAY_MPWMSTOP)>0)	//0ms LVRT 20100125cpc����ȷ��
			   		M_ClrFlag(SL_LV_MPWMOUT);	      						//����������ʱ0ms����ͨSCR
	        } 
	}
    else   //ϵͳ�����ϵ�,��Ԥ���բû���г�
	{      
		DisPwm();
		M_ClrFlag(SL_RUN);											//2010
		M_ClrFlag(SL_NPR_PWMOUT);
		M_ClrFlag(SL_MPR_PWMOUT);
		M_ClrFlag(SL_LV_MPWMOUT);	      							//����������ʱ1ms����ͨSCR
		M_ClrFlag(SL_CONFIGPWM);			        				//�������
		MAIN_LOOP.cnt_pwmout=0;										//���ϵͳ����������ʱ
		MAIN_LOOP.cnt_lv_mpwmstop=0;
	}
} 
//****************************************************************************
//��ţ�
//���ƣ�SVPWM
//���ܣ�����������
//���룺

//�����
//ע�ͣ�
//�޸�˵��:
//****************************************************************************
//------------------���������SVPWM����------------------------------------//	
void SVPWM_NPR(float alfa, float beta)
{      
  float tempmin,tempmax,temp1,temp2,temp3;
  float t0,t1,t2;    						 					//�ռ����ʸ��������ʱ��
  Uint16 sector;      											//����

  vdc = AD_OUT_UDC;  											//vdcΪʵ��ֱ����ѹֵ,cdcȡʵ��ֱ����ѹֵ���ڵ���
//-----------------------SVPWM �����жϺ�ʸ������ʱ�����------------------------------------------
  if(beta>=0)
  {
	   if(SQRT3 * alfa >= beta) 
	   {
	      sector=1;
	      t1=SQRT3 * (alfa * SQRT3_2-beta * 0.5)/vdc;   		//SQRT3=sqrt(3)=1.73205081���ں궨������ʵ��,0.8660254=SQRT3/2
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

//--------------------�����ƴ���------------------------------------------------------------------ 
  	if(t1+t2>=1.0)           									//��ʱTs=1�����ж�ʱ��1��Ϊ�Ƚ�
  	{
	  t1=t1/(t1+t2);
	  t2=1.0-t1;
  	} 

  	  t0 = 1.0 - t1 - t2;	
  	  t0 = t0 * 0.5;											//��ʸ������ʱ��
//------------------------ �ȽϼĴ���ֵ����---------------------------------------------------------
  switch(sector)
    {
      	case 1:    temp1=(Uint16) (t0 * SW_NPR);				//SW_NPRΪPWM���ڼĴ����,Ϊ�������ڵ�һ��
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

   tempmin = 0.5 * 75 * (_MINONTIME + _DEADTIME);   //_MINONTIME��Ӧ��С�����ʱ�䣬��λus
                                           	  		//_DEADTIME��Ӧ������ʱ�䣬��λus
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
//��ţ�
//���ƣ�SVPWM_MPR
//���ܣ�����������
//���룺

//�����
//ע�ͣ� ���������SVPWM����
//�޸�˵��:
//****************************************************************************
void SVPWM_MPR( float alfa, float beta)
{
  
   float tempmin,tempmax,temp1,temp2,temp3;
   float t0,t1,t2;
   Uint16 sector;

   vdc = AD_OUT_UDC;
//-----------------------SVPWM �����жϺ�ʸ������ʱ�����-------------------------------------------	       
   if(beta>=0)
	{
	   if(SQRT3*alfa >= beta) 
	   	{
	      sector=1;
	      t1=SQRT3 * (alfa * SQRT3_2-beta * 0.5)/vdc;   		//SQRT3=sqrt(3)=1.73205081���ں궨������ʵ��,0.8660254=SQRT3/2
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
  t0 = t0 * 0.5;	    										//��ʸ������ʱ��
//------------------------ �ȽϼĴ���ֵ����---------------------------------------------------------
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

   tempmin = 0.5 * 75 * (_MINONTIME + _DEADTIME);   //_MINONTIME��Ӧ��С�����ʱ�䣬��λus
                                           	  		//_DEADTIME��Ӧ������ʱ�䣬��λus
   tempmax = SW_MPR - tempmin;  

   if	  (temp1<tempmin) 	temp1=tempmin;
   else if(temp1>tempmax)	temp1=tempmax;
   if	  (temp2<tempmin) 	temp2=tempmin;
   else if(temp2>tempmax)	temp2=tempmax;
   if	  (temp3<tempmin) 	temp3=tempmin;
   else if(temp3>tempmax)	temp3=tempmax;

   if(R_PHORDE==1)		//ת�ӷ��������201005atcpc
   {
 	  EPwm4Regs.CMPA.half.CMPA = temp2;	//ת�ӷ��������
 	  EPwm5Regs.CMPA.half.CMPA = temp1;
 	  EPwm6Regs.CMPA.half.CMPA = temp3; 
   }
   else
   {
	  EPwm4Regs.CMPA.half.CMPA = temp1;	//ת�����������
	  EPwm5Regs.CMPA.half.CMPA = temp2;
 	  EPwm6Regs.CMPA.half.CMPA = temp3;    
   }


}  

/*********************************************************************************************************
** ��������: ����任�Ӻ���
** ��������: 3s/2s�任��2s/2r�任
** �䡡��: 
** �䡡��:   
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** ����:20090331
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Transform_3s_2s_2r(struct TRANS_DATA *var1)
{   
    var1->alfa = var1->a;												//���õȷ��任 ualfa=2/3(ua-0.5ub-0.5uc)   
	var1->beta = (var1->b - var1->c) * SQRT3_3;							//ubeta=2/3*sqrt(3)/2 (ub-uc)  SQRT3_3=sqrt(3)/3=0.57735026918962576450914878050196

    var1->d =  var1->alfa * var1->costheta + var1->beta * var1->sintheta;//ud=ualfa*cos(th)+ubeta*sin(th)
    var1->q = -var1->alfa * var1->sintheta + var1->beta * var1->costheta;//uq=-ualfa*sin(th)+ubeta*cos(th)
}  
/*********************************************************************************************************
** ��������: ����任�Ӻ���
** ��������: 2r/2s�任��2s/3s�任
** �䡡��: 
** �䡡��: 
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** ����:20090331
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Anti_Transform_2r_2s(struct TRANS_DATA *var2)
{
    var2->alfa = var2->d * var2->costheta - var2->q * var2->sintheta;//ualfa=ud*cos(th)-uq*sin(th)   
	var2->beta = var2->d * var2->sintheta + var2->q * var2->costheta;//*ubeta=ud*sin(th)+uq*cos(th)     			
}

/*********************************************************************************************************
** ��������: PI_Loop
** ��������: PI���
** �䡡��:   kp �����Ŵ�ϵ��
             ki ����ϵ��
			 outmax ����޷�
			 errmax ������ֵ�޷�
             errmin �����Сֵ�޷�
             incrementmax ��������޷�

** �䡡��:   
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** ����:20090331
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void PI_Loop(struct PI_DATA *var,struct PI_PARA PI_var)
{
	float temp,ptemp,itemp,detemp,outtemp,du;

//----------------------���㱾�����---------------------------------------------------------------
	temp = var->reference - var->feedback;  			

//----------------------����������ֵ�޷�����Сֵ�޷�-------------------------------------------
    if		(temp  	 	>  PI_var.errmax)  temp =  PI_var.errmax; 
    else if	(temp  	 	< -PI_var.errmax)  temp = -PI_var.errmax; 
	else 	 temp = temp;

	var->error = temp;													//��������������
    detemp = var->error -  var->errorp;    

//----------------------�����������޷�-------------------------------------------------------------
	ptemp = PI_var.kp *  detemp;										//������
	itemp = PI_var.kp * PI_var.ki *  var->error * 2.0e-4;				//����������,09.6.17

	du = ptemp + itemp;                                             	//��������
	if     (du >  PI_var.incrementmax)    du =  PI_var.incrementmax;	//���������޷�
	else if(du < -PI_var.incrementmax)    du = -PI_var.incrementmax;	//���������޷�

//----------------------����������޷�------------------------------------------------------------
	outtemp = var->out + du;

	if     (outtemp >  PI_var.outmax)  outtemp =  PI_var.outmax;		//����޷�
	else if(outtemp < -PI_var.outmax)  outtemp = -PI_var.outmax;		//����޷�

//---------------------�����ֵ��ˢ���ϴ����ֵ--------------------------------------------------	
	var->out    = outtemp;		
	var->errorp = var->error;												
} 

//--------20121103--------
/*********************************************************************************************************
** ������: PLLCtrl
** ��������: ��������
** �䡡��:
** �䡡��       
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void PLLCtrl(void)
{  
	float n,p;
 
//    I_Loop(&PI_DATA_PLL);		//201205LVRTatZB
//    pll_theta=I_DATA_PLL.last;
 
    TRS_GAC_U.a = AD_OUT_NGS_U.a;					//��ȡ������ѹ
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

	PTransform_3s_2s_2r(&TRS_GAC_U);					//����任
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
** ��������: PLL����任�Ӻ���
** ��������: 3s/2s�任��2s/2r�任
** �䡡��: 
** �䡡��:   
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** ����:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void PTransform_3s_2s_2r(struct TRANS_DATA *var2)
{   
    var2->palfa = var2->a;													//���õȷ��任 ualfa=2/3(ua-0.5ub-0.5uc)   
	var2->pbeta = (var2->b - var2->c) * SQRT3_3;							//ubeta=2/3*sqrt(3)/2 (ub-uc)  SQRT3_3=sqrt(3)/3=0.57735026918962576450914878050196

    var2->pd =  var2->palfa * var2->costheta + var2->pbeta * var2->sintheta;//ud=ualfa*cos(th)+ubeta*sin(th)
    var2->pq = -var2->palfa * var2->sintheta + var2->pbeta * var2->costheta;//uq=-ualfa*sin(th)+ubeta*cos(th)
}  
/*********************************************************************************************************
** ��������: PLL����任�Ӻ���
** ��������: 3s/2s�任��2s/2r�任
** �䡡��: 
** �䡡��:   
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** ����:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void NTransform_3s_2s_2r(struct TRANS_DATA *var3)
{   
    var3->nalfa = var3->a;													//���õȷ��任 ualfa=2/3(ua-0.5ub-0.5uc)   
	var3->nbeta = (var3->b - var3->c) * SQRT3_3;							//ubeta=2/3*sqrt(3)/2 (ub-uc)  SQRT3_3=sqrt(3)/3=0.57735026918962576450914878050196

    var3->nd =  var3->nalfa * var3->costheta - var3->nbeta * var3->sintheta;//ud=ualfa*cos(th)+ubeta*sin(th)
    var3->nq = var3->nalfa * var3->sintheta + var3->nbeta * var3->costheta;//uq=-ualfa*sin(th)+ubeta*cos(th)
} 
/*********************************************************************************************************
** ��������:PLL����������
** ��������: 
** �䡡��: 
** �䡡��:   
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** : 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** ����:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void De_ONE_P_2r(struct TRANS_DATA *var4)
{
  var4->de_pd=var4->pd - var4->de_fltnd*var4->de_pcostheta-var4->de_fltnq*var4->de_psintheta;
  var4->de_pq=var4->pq +var4->de_fltnd*var4->de_psintheta- var4->de_fltnq*var4->de_pcostheta;
}

/*********************************************************************************************************
** ��������:PLL����������
** ��������: 
** �䡡��: 
** �䡡��:   
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** ����:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void De_ONE_N_2r(struct TRANS_DATA *var5)
{
  var5->de_nd=var5->nd - var5->de_fltpd*var5->de_ncostheta - var5->de_fltpq*var5->de_nsintheta;
  var5->de_nq=var5->nq + var5->de_fltpd*var5->de_nsintheta - var5->de_fltpq*var5->de_ncostheta;
}

/*********************************************************************************************************
** ��������: PLL-PI_Loop
** ��������: PI������
** �䡡��:   kp �����Ŵ�ϵ��
             ki ����ϵ��
			 outmax ����޷�
			 errmax ������ֵ�޷�
             errmin �����Сֵ�޷�
             incrementmax ��������޷�

** �䡡��:   
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** ����:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void PLL_PI_Loop(struct PI_DATA *var,struct PI_PARA PI_var,struct TRANS_DATA *var1)
{
	float ptemp,itemp,detemp,outtemp,du;
													
    detemp = var1->de_pq - var->last;    

//----------------------�����������޷�-------------------------------------------------------------
	ptemp = PI_var.kp * detemp  ;										//������
	itemp = PI_var.kp * PI_var.ki * var1->de_pq * 2.0e-4;				//����������,09.6.17

	du = ptemp + itemp;                                             	//��������
	if     (du >  PI_var.incrementmax)    du =  PI_var.incrementmax;	//���������޷�
	else if(du < -PI_var.incrementmax)    du = -PI_var.incrementmax;	//���������޷�

//----------------------����������޷?-----------------------------------------------------------
	outtemp = var->out + du;

	if     (outtemp >  PI_var.outmax)  outtemp =  PI_var.outmax;		//����޷�
	else if(outtemp < -PI_var.outmax)  outtemp = -PI_var.outmax;		//����޷�

//---------------------�����ֵ��ˢ���ϴ����ֵ--------------------------------------------------	
	var->out    = outtemp;		
	var->last = var1->de_pq;												
} 
/*********************************************************************************************************
** ��������: I_Loop
** ��������: Integrator������
** �䡡��:   
** �䡡��:   
** ע  ��: 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** ����:201011PLL
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
** ��������: Magnet_Curve
** ��������: ����������ߺ���(�ֶ����Բ�ֵ)
** �䡡��:   ��ֵ�Ա���x[n],��ֵ����ֵy[n],���㺯����in
** �䡡��:   ���ŵ���У��ϵ��out  
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: MagnetCurve2013-12-13
** �ա���: 2013-12-13
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Magnet_Curve(struct MC_DATA *var)	//�ֶ����Բ�ֵ
{
	float temp_m,temp_k;
	Uint16 i,j,s;

//---------------��ֵ���޷�------------------------------------------------    
	if(var->in < var->x[0])	var->in= var->x[0];
	else if(var->in > var->x[5])	var->in= var->x[5];

//---------------�жϲ�ֵ������[0,4]------------------------------------------------    
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
** ��������: Give_Integral
** ��������: ���������Ӻ���
** �䡡��:   ���ֲ���step������ָ��ֵgive
** �䡡��:   �������ֺ��ָ��ֵout  
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 20090604
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:20090604
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
** ��������: Give
** ��������: ȷ���ܷ��� cpc�޸�
** �䡡��:   
** �䡡��:   
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա�? 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:20090801�޸�
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Give(void)
{	
	float temp_toqrf,temp_aglrf;
    
	if(M_ChkFlag(SL_NPR_START)==0 && M_ChkFlag(SL_MPR_START)==0)
	{
		M_ClrFlag(SL_NPR_RUN);			    						//���๤�������־��0
		M_ClrFlag(SL_MPR_RUN);			    						//���������־��0
		GIVE.urf      = _URF;										//���������ѹΪ��ǰVdcֵ
		GIVE.npriqrf  = 0;											//�����������Ϊ0
        GIVE.toqrf    = 0;                  						//�������ת��Ϊ0
        GIVE.anglerf  = 0;		            						//����������������Ƕ�Ϊ0
		GIVE.toqrfflt = 0;		//20121103
	}
	else
	{
//		if(M_ChkFlag(SL_LV_QWORKING)==0) 	NGS_Udq_p_ex = NGS_Udq_p;		//�ж�LVRT֮ǰ��ѹ 20130206
		if(M_ChkFlag(SL_LV_QWORKING)==0) 	NGS_Udq_p_ex = NGS_Udq_pflt;		//�ж�LVRT֮ǰ��ѹ 20130301

/*		if(M_ChkFlag(SL_OCS_NPREIN)!=0)					           //�����޹�����,�������ֱ����ѹ���޹�����ֵ
		{
		    M_ClrFlag(SL_MPR_RUN);

			if(M_ChkFlag(SL_NPR_START)!=0)  
			{
				M_SetFlag(SL_NPR_RUN); 								//�������������
		    	GIVE.urf      = (int16) _URF;                       //�������м�ֱ����ѹָ�ֵ
		    	GIVE.npriqrf  = (int16) _NIQRF;						//�������޹�����ָ�ֵ
			}
	        else  M_ClrFlag(SL_NPR_RUN);
		}	
			
		else 		                       							//����������ʱ����ֱ����ѹ,�޹����������ͻ���ת�غ͹��������Ǹ���
		{		
*/			if(M_ChkFlag(SL_NPR_START)!=0)  		   				//���������ж�
			{
			   M_SetFlag(SL_NPR_RUN);                  				//������������		       
		       GIVE.urf       = (int16) _URF;          				//���м�ֱ����ѹָ�ֵ
			   GIVE.npriqrf   = (int16) _NIQRF;        				//������������޹���֤������������Ϊ1
//			   GIVE.npriqrf   = 70.0;                  				//������������޹���֤������������Ϊ1������ʵ����֤��Ҫ��70A

/*20121103 ��Runclt�����
			   if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   			//LVRT�����޹�����
			   {
					GIVE.npriqrf   = -(int16)_PROSTDBY1;   			//201007BJTULVRT
			   }
*/
			}
		    else     M_ClrFlag(SL_NPR_RUN);


	        if(M_ChkFlag(SL_MPR_START)!=0)  						//���������ж�
	        {
	           M_SetFlag(SL_MPR_RUN);                   			//������������
 
               if(M_ChkFlag(SL_ENPCOPER)!=0)	  
               {
					if(M_ChkFlag(SL_IN1_STATORK)!=0)				//���ӽӴ����պ��Ժ��ٽ���ת�غͽǶ�ָ��
					{ 
               			GIVE.toqrf  =  (int16) _TOQRF;              //��ֵΪ����
						GIVE.anglerf=  (int16) _AGLRF;  			//������������ָ�ֵ,-360 -- 360

					    if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   	//LVRT���෢�й������޹�
			            {
//					    	GIVE.toqrf  = (int16)_PROSTDBY2;   		//201007BJTULVRT
							GIVE.anglerf= 0;						//20110829
			   			}
					}
					else
      		   		{
      		   			GIVE.toqrf    = 0;                  		//�������ת��Ϊ0
        				GIVE.anglerf  = 0;		            		//����������������Ƕ�Ϊ0
						GIVE.toqrfflt = 0;
     		   		} 
			   }
               else   
               {           
					if(M_ChkFlag(SL_IN1_STATORK)!=0)				//���ӽӴ����պ��Ժ��ٽ���ת�غͽǶ�ָ��
					{
               			temp_toqrf     =  (int16)SCI_canopen.rx_torque;               //��ֵΪ����
      		   			GIVE.toqrf     =  temp_toqrf * CAN_TEN / 16383.0;             //����ת��ָ��ʵ��ֵ  _TOQRF == -200%-200%

						if(GIVE.toqrf<0)	GIVE.toqrf=0;    //�����������صĸ�ת��ָ�����Ӧ 20090815
						if(GIVE.toqrf>9500)	GIVE.toqrf=9500;    //�����������صĳ���ת��ָ�����Ӧ 20130801

               			temp_aglrf     =  (int16)SCI_canopen.rx_angle;               
      		   			GIVE.anglerf   =  temp_aglrf * CAN_ANGLEN / 16383.0;  

					    if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   	//LVRT���෢�й������޹�
			            {
//					    	GIVE.toqrf  = (int16)_PROSTDBY2;   		//201007BJTULVRT
							GIVE.anglerf= 0;						//20110829
			   			}
      		   		}
      		   		else
      		   		{
      		   			GIVE.toqrf    = 0;                  		//�������ת��Ϊ0
        				GIVE.anglerf  = 0;		            		//����������������Ƕ�Ϊ0
						GIVE.toqrfflt = 0;
     		   		}             				
			   
			   }

				if     (GIVE.anglerf > 0)
					GIVE.anglerf = GIVE.anglerf * _stdby02;		//20140503LJD��������
				else if     (GIVE.anglerf < 0)
					GIVE.anglerf = GIVE.anglerf * 1.1;			//20140503LJD��������

			   if     (GIVE.anglerf <-30.0)                      GIVE.anglerf     = -30.0;
			   else if(GIVE.anglerf > 30.0)                      GIVE.anglerf     =  30.0;
			   else  											 GIVE.anglerf     = GIVE.anglerf;
                                         
	        }
            else    M_ClrFlag(SL_MPR_RUN);
//		}
	}
}
/*********************************************************************************************************
** ��������: RunCtrl
** ��������: ��������  cpc�޸�
** �䡡��:   
** �䡡��:   
** ע  ��: 	 
**--------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**--------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:20090721�޸�
**--------------------------------------------------------------------------------------------------
***********************************************************************************************/
void RunCtrl(void)
{
    float udc_max,id_max,iq_max,toq_max,agl_max,temp,temp_var;//2013-12-9ZZJ����temp_var
	Uint16 temp_n,temp_ud,temp_iqexi,temp_number;
//	float  temp_exi,temp_iqk;
//	float  temp_Qc,temp_Qg,temp_tan;
	float  temp_exi,temp_iqk,temp_npriqrf,temp_idrfmax,temp_iqrfmax;

//----------------------------������и�������---------------------------------------------------------

//-----��������������Ҫ������ͣ��
    if((M_ChkFlag(SL_NPR_RUNING)!=0)&&(M_ChkFlag(SL_NPR_RUN)==0))  				//�����޹�������Ϊ0����ͣ����
    {
		RUN.npriqrf = Give_Integral(0,RUN.npriqstep,RUN.npriqrf);  				//ָ���Сֱ������0
            
        if(RUN.npriqrf==0)   
        {   
        	if(M_ChkCounter(MAIN_LOOP.cnt_isteady0,DELAY_ISTEADY0)>0)  M_ClrFlag(SL_NPR_RUNING);
		}
		else  MAIN_LOOP.cnt_isteady0= 0;
    }

//-----Ҫ����������
    else if(M_ChkFlag(SL_NPR_RUN)!= 0) 
    {  
       M_SetFlag(SL_NPR_RUNING);                 								//��������У���ʼ����������
       MAIN_LOOP.cnt_isteady0= 0;
	                
	   RUN.urf = Give_Integral(GIVE.urf,RUN.ustep,RUN.urf);     				//��ѹ��������

//       if(M_ChkFlag(SL_LV_NSTOP)!=0)	RUN.urf=AD_OUT_UDC; 					//20121103							

       if(RUN.urf == GIVE.urf)	    M_SetFlag(SL_STEADYGV);        				//�����������,���ȶ���־λ
	   else							M_ClrFlag(SL_STEADYGV);

//-------�����޹���������BJTULVRT20121103--------
		if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   			//LVRT�����޹�����
		{				
		 if(M_ChkCounter(MAIN_LOOP.cnt_lv_nprqworking,20)>=0)		//20130225
		 {
			temp_npriqrf   = -(int16)_PROSTDBY1;   			//�����޹�ָ�����20111103
			if((_COMMAND2&0x0400)!=0)	//����10 20111106
			{
				if(M_ChkFlag(SL_LV_STATE)==0)
					temp_npriqrf   = -450; 	//1100V���ѹ�������޹�������ֵ��С��460A
			}

			GIVE.npriqrf_cl   = -GIVE.lvwtiqrf - DIP_STA_I.qflt;   		//Itq=100A Isq201205LVRTatZB

//			if((_COMMAND2&0x0200)!=0)	//����9 201011BJTULVRT
//				GIVE.npriqrf   =  GIVE.npriqrf_cl;   		//Itq=100A Isq
//			else	
				GIVE.npriqrf = - kq * 400; //90kW-DFIG����30kW�������ֵ65A 1.5MW��kq*296A �Ӵ�400A 20120601night

			if(GIVE.npriqrf < temp_npriqrf)	GIVE.npriqrf=temp_npriqrf;
			else if(GIVE.npriqrf > 0)		GIVE.npriqrf=0;
//--------20130124----���෢�޹����������޹��Ĳ���----
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_npriqrf,120)<=0)
			{
//				GIVE.npriqrf = - kq * lv_ctrl5 * 400;
				if((NGS_Udq_epsilon < 30) && (NGS_Udq_p < (0.43 * NGS_Udq_p_ex)))		//ֻ3ph-20��35����²ŷ�	20130308
					GIVE.npriqrf = - kq * lv_ctrl5 * 400;	//20130308
			}
//--------20130124--------
//--------���ԳƵ��������޹�����20130221--------
//			if((NGS_Udq_epsilon > 27) && (PRO.Pgactive_lv>600000) && (NGS_Udq_p < (0.71 * NGS_Udq_p_ex)) && (M_ChkFlag(SL_LV_SCRRUNING)!=0))
//			if((NGS_Udq_epsilon > 27) && (NGS_Udq_p < (0.71 * NGS_Udq_p_ex)) && (M_ChkFlag(SL_LV_SCRRUNING)!=0))	//20%/35%���й���	20130222
			if((NGS_Udq_epsilon > 38) && (PRO.Pgactive_lv>600000) && (NGS_Udq_p < (0.61 * NGS_Udq_p_ex)) && (M_ChkFlag(SL_LV_SCRRUNING)!=0))	//20%/35%���й���	20130306
			{
				GIVE.npriqrf = - 10 * lv_ctrl1;		//��ֵ�趨Ϊ600A
			}
			else if(NGS_Udq_epsilon > 15)
			{
				GIVE.npriqrf = - _stdby03;			//��ֵ�趨Ϊ300A
//				GIVE.npriqrf = - 300;			//��ֵ�趨Ϊ300A
			}
//----------------------------------------------
//			if(M_ChkFlag(SL_LV_STRICTLV)==0 && (_STDBY9&0x0040 == 0))		//116.6	20130301
//				GIVE.npriqrf = - kq_lv * _stdby08;	//��ѹ�ָ���������޹�400A 20130228

			if(GIVE.npriqrf < -850)	GIVE.npriqrf = -850;
			else if(GIVE.npriqrf >0)		GIVE.npriqrf=0;
		 }
		}
		else		//20130124
		{
			MAIN_LOOP.cnt_lv_npriqrf = 0;
			MAIN_LOOP.cnt_lv_nprqworking = 0;		//20130124���෢�޹��ܹ�20ms
		}


 	   if(M_ChkFlag(SL_STEADYGV)!=0)											//����ѹ������ɺ��ٸ����޹�����
	 	   RUN.npriqrf = Give_Integral(GIVE.npriqrf,RUN.npriqstep,RUN.npriqrf); //���๦������������	         

//	   if(M_ChkFlag(SL_LV_NSTOP)!=0)  	RUN.npriqrf=0; 							//201007BJTULVRT							
	   if((M_ChkFlag(SL_LV_NSTOP)!=0) || (M_ChkFlag(SL_LV_STATE)!=0 && M_ChkFlag(SL_LV_QWORKING)==0))  
	   		RUN.npriqrf=0;		//20121103		������ʱ������ʱ���޹�ָ������					

//	   if(M_ChkFlag(SL_UNBALANCE)!=0 && (NGS_Udq_epsilon > 35))	//20121103
//	   		RUN.npriqrf=0; //���ԳƵ���ܴ�ʱ������ʱ���޹�ָ������					
//	   		RUN.npriqrf = - (int16)_PROSTDBY1 * lv_ctrl1;		//���ԳƵ��䷢�����޹��Ե��������޹�����20121210					
	}
//-----����    
    else									//����
    {
	   M_ClrFlag(SL_NPR_RUNING);
	   M_ClrFlag(SL_STEADYGV);
	   MAIN_LOOP.cnt_isteady0= 0;

	   RUN.urf   = AD_OUT_UDC;													//�������ֳ�ʼֵΪ��ǰVdcֵ
       RUN.npriqrf  = 0;														//�������ֳ�ʼֵΪ0
        
       udc_max    =  (float)FUNC[NO_URF].max;									//��ȡ���ֵ
       iq_max     =  (float)FUNC[NO_NIQRF].max;
       temp       =  (float)_RU_UDCT;
	   RUN.ustep   = abs(udc_max)/temp;											//����ֱ����ѹ��������
	   temp       =  (float)_RU_IQT;
	   RUN.npriqstep  = abs(iq_max)/temp;										//�����޹�������������

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

//----------------------------���������й�����ֵ��������-------------------------------------

//-----�����������в���Ҫ��ͣ��
    if((M_ChkFlag(SL_MPR_RUNING)!=0)&&(M_ChkFlag(SL_MPR_RUN)==0)) 
    {      
		if(M_ChkFlag(SL_MPR_SYNOK)!=0)												//systest201005atcpc
		{
		    RUN.toqrf = Give_Integral(0,RUN.toqstep,RUN.toqrf);  					//����ת�ظ�������
		    RUN.aglrf = Give_Integral(0,RUN.aglstep,RUN.aglrf);  			    	//���๦�������Ǹ�������


//		    RUN.mpriqrf_g  =  - RUN.toqrf  / (IRQTOTE * TRS_NGS_U.dflt);   			//����q�����ָ�����
	    	RUN.mpriqrf_g  =  - RUN.toqrf * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * TRS_NGS_U.dflt / (314.15926 *  MPR_Ls));   //����q�����ָ�����201005atcpc
//----MagnetCurve2013-12-13--ZZJ--��������ȡ����ÿ��ͣ����������ʱidָ��ֵ MagnetCurve200909- 
//			RUN_mpridrf_exi  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));  //����:��ѹed�����d�����180��             
//          RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          						//�˵���ϵ��,�ɲ��������� 20100622
/*--MagnetCurve2013-12-13---ZZJ
        	temp_ud = (Uint16)(TRS_NGS_U.dflt * SQRT3 / SQRT2);
        	if     (temp_ud < 540) 		temp_ud = 540;
        	else if(temp_ud > 800) 	    temp_ud = 800;
	    	temp_n = (temp_ud - 540) * 127 / 260;
        	temp_exi = TAB_EXI[temp_n];
        	RUN.mpridrf_exi =  RUN.mpridrf_exi * temp_exi;          				//������ϵ��,����ظ�Ӧ�����й�
//      	    RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          					//�˵���ϵ��,�ɲ���������		20100622		
///20091022atzy
			RUN_mpriq = (Uint16)(- RUN.mpriqrf_g);
			if     (RUN_mpriq < 290)   temp_iqexi = 290;
			else if(RUN_mpriq > 600)   temp_iqexi = 600;
			temp_iqexi = RUN_mpriq;  //20091019atzy
			temp_number = (temp_iqexi - 290) * 127 / 310;
			temp_iqk = TAB_IQEXI[temp_number];
        	RUN.mpridrf_exi = RUN.mpridrf_exi * temp_iqk;          				//������ϵ��,�ɸ��ص���Irq����ȷ�� 
*/ //20091022atzy
	 	    RUN.radianrf    = RUN.aglrf * 0.017453292;							//�Ƕ�ת����360->2PAI	 0.01745329=2 * PAI / 360
			   	    
//		    temp_Qc = TRS_NGS_U.dflt * 314.15926 * 501.0e-6 / SQRT2;
//		    temp_Qg = PRO.Pgactive * sin(RUN.radianrf) / cos(RUN.radianrf);
//			temp_tan    = (temp_Qg - temp_Qc) / PRO.Psactive;
//		    RUN.mpridrf_var = RUN.mpriqrf_g * temp_tan;

	    	RUN.mpridrf_var = RUN.mpriqrf_g * sin(RUN.radianrf) / cos(RUN.radianrf); //ͣ��ʱ����֤���ӵ���Ϊ0������������Ϊ1
        
 	        RUN.mpridrf_g   = RUN.mpridrf_exi + RUN.mpridrf_var;                                           

			RUN.mpridrf = Give_Integral(RUN.mpridrf_g,RUN.mpridstep,RUN.mpridrf); //���ಢ��ǰd�������Ϊ-90A
			RUN.mpriqrf = Give_Integral(RUN.mpriqrf_g,RUN.mpriqstep,RUN.mpriqrf); //ָ���Сֱ������0	  
		    if(RUN.mpriqrf>50)        M_SetFlag(SL_STACKEEPON);					  //���Ե�=0��п��ܴﲻ����systest20100510
			else    			      M_ClrFlag(SL_STACKEEPON);
		    MAIN_LOOP.cnt_mprsteady0= 0; 		
		}
		else
		{
			RUN.mpriqrf = Give_Integral(0,RUN.mpriqstep,RUN.mpriqrf);  			   	//ָ���Сֱ������0
			RUN.mpridrf = Give_Integral(0,RUN.mpridstep,RUN.mpridrf);  				//ָ���Сֱ������0
		    if((RUN.mpridrf==0)&&(RUN.mpriqrf==0))  								//�����й����޹��������������� 
		    { 
		        if(M_ChkCounter(MAIN_LOOP.cnt_mprsteady0,DELAY_MPRSTEADY0)>0)   M_ClrFlag(SL_MPR_RUNING); //0.05s
		    }
		    else   MAIN_LOOP.cnt_mprsteady0= 0; 		
		}
		
    }
   	 	   	
//-----����Ҫ��������Vdc�ȶ�
//    else if((M_ChkFlag(SL_MPR_RUN)!=0) && (M_ChkFlag(SL_STEADYFB)!=0))  
    else if(M_ChkFlag(SL_MPR_RUN)!=0)  //2010-4-27atBJTU for new Sysctrl systest
    {  
        M_SetFlag(SL_MPR_RUNING);
        M_SetFlag(SL_STACKEEPON);                 									//�����������У���ʼ����������
//        M_ClrFlag(SL_STACTRIPEN);                 								//systest 201005atcpc
        MAIN_LOOP.cnt_mprsteady0= 0;
//----20121103----
		if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   	//LVRT���෢�й������޹�
		{
//			GIVE.toqrf  = GRD_Ut * GIVE.toqrf;   	//201205LVRTatZB
			GIVE.anglerf= 0;						//20110829
			RUN.toqrf = GIVE.toqrf;					//20130228
//			if(M_ChkFlag(SL_UNBALANCE)!=0)
//				GIVE.toqrfrun  = 0;   					//20121013LVRTatBJTU ���ԳƵ�����ת�ز�����
		}
		else
		{
//			DataFilter(0.994,&GIVE.toqrfflt,GIVE.toqrf); //���ظ���ת��ƽ��ֵ1s ÿ1msִ��һ��201205LVRTatZB
 			GIVE.toqrfrun = GIVE.toqrf;
 		}	   
//----------------
		RUN.toqrf = Give_Integral(GIVE.toqrf,RUN.toqstep,RUN.toqrf);  				//����ת�ظ�������
		RUN.aglrf = Give_Integral(GIVE.anglerf,RUN.aglstep,RUN.aglrf);  			//���๦�������Ǹ�������
//-----20121103-------
		if(M_ChkFlag(SL_LV_MSTOP)!=0 || (M_ChkFlag(SL_LV_STATE)!=0 && M_ChkFlag(SL_UNBALANCE)!=0))												//201007BJTULVRT
		{				
			RUN.toqrf = 0;  		//201205LVRTatZB ��ƽ���µ�ת������ָ��Ҫ����		
			RUN.aglrf = 0;  			
		}
//-------------------
 
//	    RUN.mpriqrf_g  =  - RUN.toqrf  / (IRQTOTE * TRS_NGS_U.dflt);   				//����q�����ָ�����
	    RUN.mpriqrf_g  =  - RUN.toqrf * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * TRS_NGS_U.dflt / (314.15926 *  MPR_Ls));   //����q�����ָ�����201005atcpc
/*---MagnetCurve2013-12-13---ZZJ
		RUN_mpridrf_exi  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));  //����:��ѹed�����d�����180��             
        RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          						//�˵���ϵ��,�ɲ��������� 20100622
        temp_ud = (Uint16)(TRS_NGS_U.dflt * SQRT3 / SQRT2);							//��ѹ�ߵ�ѹ��Чֵ 690V
        if     (temp_ud < 540) 		temp_ud = 540;
        else if(temp_ud > 800) 	    temp_ud = 800;
	    temp_n = (temp_ud - 540) * 127 / 260;
        temp_exi = TAB_EXI[temp_n];
        RUN.mpridrf_exi =  RUN.mpridrf_exi * temp_exi;          					//������ϵ��,�ɲ��������� 
//      	RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          					//�˵���ϵ��,�ɲ���������	20100622			
*///---MagnetCurve2013-12-13---ZZJ
/*//20091022atzy 
		RUN_mpriq = (Uint16)(- RUN.mpriqrf_g);
		if     (RUN_mpriq < 290)   temp_iqexi = 290;
		else if(RUN_mpriq > 600)   temp_iqexi = 600;
        temp_iqexi = RUN_mpriq;  //20091019atzy
		temp_number = (temp_iqexi - 290) * 127 / 310;
		temp_iqk = TAB_IQEXI[temp_number];
        RUN.mpridrf_exi = RUN.mpridrf_exi * temp_iqk;          						//������ϵ��,�ɸ��ص���Irq����ȷ�� 
*///20091022atzy 
//----MagnetCurve2013-12-13 ZZJ�������ŵ�������---
//----MagnetCurve200909 test_start-----------------------

   		if(_SC_MSTDBY==0)
   		{
			RUN_mpridrf_exi  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));
        	RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;

 			if(_MC_OK==1)				//����������߻������
			{								
 				MC_DATA.in = 100 * TRS_NGS_U.dflt * SQRT3 / ((float)_SC_Usn * SQRT2);
				if(MC_DATA.in < 50)			MC_DATA.in = 50;		//�����޷�[50,110]
				else if(MC_DATA.in > 110)	MC_DATA.in = 110;		//�����޷�
 				Magnet_Curve(&MC_DATA);
        		RUN.mpridrf_exi =  RUN.mpridrf_exi * MC_DATA.k;
        	}
		}
		else
		{
			temp_exi  = - ((float)_SC_Usn * SQRT2 * STAROTRTO / (MPR_Lm * 314.15926));  //�������۶��ѹed=563ʱ����d�����ŵ���ֵ            
        	if(_SC_MSTDBY==1)		RUN_mpridrf_exi = 0.5  * _SC_EXISTY1 * 0.001 * temp_exi;
			else if(_SC_MSTDBY==2)	RUN_mpridrf_exi = 0.75 * _SC_EXISTY2 * 0.001 * temp_exi;
			else if(_SC_MSTDBY==3)	RUN_mpridrf_exi = 0.9  * _SC_EXISTY3 * 0.001 * temp_exi;
			else if(_SC_MSTDBY==4)	RUN_mpridrf_exi = 1.0  * _SC_EXISTY4 * 0.001 * temp_exi;
			else if(_SC_MSTDBY==5)	RUN_mpridrf_exi = 1.1  * _SC_EXISTY5 * 0.001 * temp_exi;
//--------------------У�黷��------------------------------------------------------------
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


//----MagnetCurve2013-12-13 ZZJ�������ŵ�������---end


 	    RUN.radianrf    = RUN.aglrf * 0.017453292;									//�Ƕ�ת����360->2PAI	 0.01745329=2 * PAI / 360
	    RUN.mpridrf_var = RUN.mpriqrf_g * sin(RUN.radianrf) / cos(RUN.radianrf); //ͣ��ʱ����֤���ӵ���Ϊ0������������Ϊ1
			   	    																//RUN.aglrf>0,ת����������,���������޹�;��֮,����
//	    temp_Qc = TRS_NGS_U.dflt * 314.15926 * 501.0e-6 / SQRT2;					//����ʱ�����ܲ�����������1ΪĿ��
//	    temp_Qg = PRO.Pgactive * sin(RUN.radianrf) / cos(RUN.radianrf);
//		temp_tan    = (temp_Qg - temp_Qc) / PRO.Psactive;
//	    RUN.mpridrf_var = RUN.mpriqrf_g * temp_tan;
//----MagnetCurve2013-12-13 ZZJ�������ŵ�������---
		if(_MC_OK==1)				//����������߻������BJTULVRT201204
		{
	 		RUN.mpridrf_var = 	RUN.mpridrf_var	* MC_DATA.k;
		}
//----MagnetCurve2013-12-13 ZZJ�������ŵ�������---end

		temp_var = RUN.mpridrf_var;//2013-12-9ZZJ�޸��޹�ָ����Ӧ����
//----20121103-----
//-------�����޹���������BJTULVRT201204-------����20121129-----------
//		if(M_ChkFlag(SL_LV_STATE)!=0)  		   			//!!!!!!20120601night 201205LVRTatZB
		if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   			//LVRT���෢�޹����� 20120603night�Ļ���ʱ��Ӧ����ת��ָ��
		{
//			RUN.mpriqrf_g  = - GIVE.toqrfrun * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * GRD_UN / (314.15926 *  MPR_Ls));   //201205LVRTatZB
			RUN.mpriqrf_g  = - GIVE.toqrfrun * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * NGS_Udq_p_ex / (314.15926 *  MPR_Ls));   //20130301LVRTatZB
//ע�����ֵ���ǰת�ز��䣬��Ϊ��ѹ���������ڼ��й�����ά�ֲ��䣬����Irq*ͻ��//20120531
			
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_utlv,DELAY_LVUTLV)<=0)	//500ms
//				GRD_Utlv = GRD_Ut;
			{
				GRD_Utlv = GRD_Ut;
				NGS_Udq_p_lv = NGS_Udq_p; 		//20130124
				kq_lv = kq;				//��������� 20130124
				NGS_Udq_epsilon_lv = NGS_Udq_epsilon;	//20130301
//				Te_feedback_lv=Te_feedback; 	//20130124
//--------�������������ת��ֵ20130221--------
//				if(PRO.Pgactive_lv<600000)
				if((PRO.Pgactive_lv<600000) || (NGS_Udq_p > (NGS_Udq_p_ex * 0.83)))		//����90%������75%�����	20130303
					Te_feedback_lv = GIVE.toqrf_lv;		//��������ת��ά��20130128
				else
					Te_feedback_lv = GIVE.toqrf_lv * GRD_Utlv;
//--------�������������ת��ֵ20130221--------
			}
			else
			{
//				RUN.mpriqrf_g  = - (GRD_Utlv/GRD_Ut) * GIVE.toqrfrun * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * GRD_UN / (314.15926 *  MPR_Ls));   //201205LVRTatZB
				RUN.mpriqrf_g  = - (GRD_Utlv/GRD_Ut) * GIVE.toqrfrun * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * NGS_Udq_p_ex / (314.15926 *  MPR_Ls));   //20130301LVRTatZB
													//20120531
//				if(PRO.Pgactive_lv<600000)		RUN.mpriqrf_g = RUN.mpriqrf_g_exlv;
				if((PRO.Pgactive_lv<600000) || (NGS_Udq_p_lv > (NGS_Udq_p_ex * 0.83)))	//����90%������75%�����	20130303
					RUN.mpriqrf_g = RUN.mpriqrf_g_exlv;
			}
//--------90%ά�ֹ��ʲ���20130205--------2013-12-6 ȡ������75%��90%�й����ֹ���
/*			if(M_ChkCounter(MAIN_LOOP.cnt_lv_holdpow,10)>=0)
			{
//				if((NGS_Udq_p>(NGS_Udq_p_ex * 0.85)) && M_ChkFlag(SL_LV_STRICTLV)!=0)		//20130222
//				if((NGS_Udq_p>(NGS_Udq_p_ex * 0.85)) && (NGS_Udq_epsilon<9) && M_ChkFlag(SL_LV_STRICTLV)!=0)		//2ph-75�ų����� 20130309
//					RUN.mpriqrf_g = RUN.mpriqrf_g * 1.14;

				if((_STDBY9&0x0008)==0 && (PRO.Pgactive_lv>600000) && (NGS_Udq_p>(NGS_Udq_p_ex * 0.83)) && (NGS_Udq_epsilon>8) && M_ChkFlag(SL_LV_STRICTLV)!=0)  //116.3	20130314
					RUN.mpriqrf_g = - 400;		//2ph-75����й�����	20130314
				else if((NGS_Udq_p>(NGS_Udq_p_ex * 0.85)) && (NGS_Udq_epsilon<9) && M_ChkFlag(SL_LV_STRICTLV)!=0)		//2ph-75�ų����� 20130309
//					RUN.mpriqrf_g = RUN.mpriqrf_g * 1.14;
					RUN.mpriqrf_g = RUN.mpriqrf_g * 1.02;
			}
*/
//--------20130205--------

			if(M_ChkCounter(MAIN_LOOP.cnt_lv_mpridvar,DELAY_LVMIDVAR)>=0)	//��ʱ�����޹�����201201
			{				
				if(M_ChkFlag(SL_LV_SYNCON)==0)	//201201
				{
//					RUN.mpridrf_var	= - kq * 605.0;				//20111201 Ir_varn=116A 1.5MW��Irn=kq*505A
					RUN.mpridrf_var	= - kq * 645.0;				//20111201 Ir_varn=116A 1.5MW��Irn=kq*505A	20130309
//					RUN.mpridrf_var	= - kq * 116.0;				//20111201 Ir_varn=116A
//					temp_idrfmax = -(int16)_PROSTDBY2;
//					if(RUN.mpridrf_var < temp_idrfmax)	RUN.mpridrf_var=temp_idrfmax;	//�����޹�ָ���޷�
//					RUN.mpridrf_var  = -(int16)_PROSTDBY2;   		//���й�201011BJTULVRT

//				    RUN.mpriqrf_g  = (GRD_Utlv/GRD_Ut) * RUN.toqrf * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * GRD_UN / (314.15926 *  MPR_Ls));   //201205LVRTatZB
				}
				else	RUN.mpridrf_var = 0;
			}

			temp_idrfmax = - (int16)_PROSTDBY2;			//�޹������޷�
			if(RUN.mpridrf_var < temp_idrfmax)	RUN.mpridrf_var=temp_idrfmax;	//�����޹�ָ���޷�

//--------20130130----���෢�޹����������޹��Ĳ���----
/*			if(M_ChkCounter(MAIN_LOOP.cnt_lv_mpridrf,100)<=0)
			{
				RUN.mpridrf_var = - kq * lv_ctrl4;
			}
			if(RUN.mpridrf_var < -900)	RUN.mpridrf_var = -900;	//�����޹�ָ���޷�
*///--------20130130--------
	
//			temp_iqrfmax = sqrt(302500.0 - ((RUN.mpridrf_exi + RUN.mpridrf_var) * (RUN.mpridrf_exi + RUN.mpridrf_var)));//Irmax=550A	
//			temp_iqrfmax = 	- _stdby03; 					//BJTULVRT201204�й������޷� ������

//--------20121202--------
//			if(M_ChkFlag(SL_LV_SYNCON)!=0)
			if(M_ChkFlag(SL_LV_SYNCON)!=0 && M_ChkFlag(SL_LV_SCRRUNING)!=0)		//20130202
			{
				RUN.mpriqrf_g = 0;
				MAIN_LOOP.cnt_lv_irqdelay=0;
			}
//			else
			else if(M_ChkFlag(SL_LV_STRICTLV)!=0)				//��ѹ�ָ����ӳٷ��й� 20130228
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_lv_irqdelay,60)<=0)		RUN.mpriqrf_g = 0;
			}
//--------20121202--------

			if(RUN.mpriqrf_g < -680)	RUN.mpriqrf_g = -680;	//�й������޷�
			else if(RUN.mpriqrf_g > 0)			RUN.mpriqrf_g = 0;
		}
		else
		{
			GRD_Utlv = GRD_Ut;
			MAIN_LOOP.cnt_lv_mpridvar = 0;
			MAIN_LOOP.cnt_lv_utlv = 0;	//201205LVRTatZB
			MAIN_LOOP.cnt_lv_mpridrf = 0;	//20130130
			RUN.mpridrf_var	= temp_var;		//2013-12-9ZZJ�޸��޹�ָ����Ӧ����
			NGS_Udq_p_lv = NGS_Udq_p; 		//20130124
//			Te_feedback_lv=Te_feedback; 	//20130124
			GIVE.toqrf_lv=GIVE.toqrf; 	//20130128
			kq_lv = kq;				//��������� 20130124
			RUN.mpriqrf_g_exlv = RUN.mpriqrf_g;		//20130221

			MAIN_LOOP.cnt_lv_holdpow = 0;	//20130205
		}
//			temp_iqrfmax = 	_stdby03; 					//BJTULVRT201204�й������޷� ������
//			if(RUN.mpriqrf_g > temp_iqrfmax)	RUN.mpriqrf_g = temp_iqrfmax;	//�й������޷�
//			else if(RUN.mpriqrf_g < 0)			RUN.mpriqrf_g = 0;


//-----------------------------------------------------------------
		if(M_ChkFlag(SL_LV_STATE)!=0 && M_ChkFlag(SL_UNBALANCE)!=0)//201205LVRTatZB
		{
//			RUN.mpriqrf_g   = 0;	//��⵽��ƽ��󲻷��й����޹�
			if(_STDBY9&0x0010!=0)	RUN.mpriqrf_g   = 0;	//116.4	20130228
			else	RUN.mpriqrf_g   = - kq * lv_ctrl2;		//20130228
			RUN.mpridrf_var = 0;
//			RUN.mpridrf_var = - (int16)_PROSTDBY2 * lv_ctrl5;		//���ԳƵ��䷢�����޹��Ե��������޹�����20121210
		}
//-----------
        RUN.mpridrf_g   = RUN.mpridrf_exi + RUN.mpridrf_var;


		RUN.mpridrf = Give_Integral(RUN.mpridrf_g,RUN.mpridstep,RUN.mpridrf);  		//�����й�������������
		RUN.mpriqrf = Give_Integral(RUN.mpriqrf_g,RUN.mpriqstep,RUN.mpriqrf);  		//�����޹�������������

		if(M_ChkFlag(SL_LV_MSTOP)!=0)	 											//201007BJTULVRT							
		{
			RUN.mpridrf=0;				
			RUN.mpriqrf=0;
		}			
//------����ʱ����Ÿ�������BJTULVRT20121103
		id_max     =  (float)FUNC[NO_MIDRF].max;
	    temp       =  (float)_RU_IDT;
	    RUN.mpridstep  = abs(id_max)/temp;											//�����Ц������������ 

    } 
//-----����
    else                                    										//����
    {
	   	M_ClrFlag(SL_MPR_RUNING);
		M_ClrFlag(SL_STACKEEPON);	//systest										//201005atcpc
	    MAIN_LOOP.cnt_mprsteady0= 0;
        
        RUN.mpridrf= 0;															    //�������ֳ�ʼֵΪ0
        RUN.mpriqrf= 0;															    //�������ֳ�ʼֵΪ0
        RUN.mpridrf_g= 0;															//�������ֳ�ʼֵΪ0
        RUN.mpriqrf_g= 0;	
        id_max     =  (float)FUNC[NO_MIDRF].max;
	    temp       =  (float)_RU_IDT;
	    RUN.mpridstep  = abs(id_max)/temp;											//�����й������������� 
        iq_max     =  (float)FUNC[NO_MIQRF].max;
	    temp       =  (float)_RU_IQT;
	    RUN.mpriqstep  = abs(iq_max)/temp;											//�����޹�������������  
 
        RUN.toqrf  = 0;																//�������ֳ�ʼֵΪ0
        RUN.aglrf  = 0;																//�������ֳ�ʼֵΪ0
        toq_max    =  (float)FUNC[NO_TOQRF].max;
	    temp       =  (float)_RU_TOQT;		//��λms
	    RUN.toqstep=  abs(toq_max)/temp;											//����ת�ظ������� 
        agl_max    =  (float)FUNC[NO_AGLRF].max;
	    temp       =  (float)_RU_AGLT;
//		temp       =  temp * 1000.0;		//��λs									//ΪCPC����ʵ��̨�����й������ٶ�201005atcpc
	    RUN.aglstep=  abs(agl_max)/temp; 											//���㹦�������Ǹ������� 
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
** ������:  QepEncodPos
** ��������:  ����λ�ü�����Ϊ������ģʽ��������λ�ü��
** �䡡��:    ��̬�µ�ת�ӵ���������ѹ
** �䡡�:    ������λ��rad  
** ע  ��:    ���ӽӴ�����բǰ���У���բ���ֹ���С�ÿ��������������һ�Ρ����ر�ָ���ĽǶȾ�Ϊ��Ƕ�
**--------------------------------------------------------------------------------------------------
** ������: 	 
** �ա���: 	 20090330
**--------------------------------------------------------------------------------------------------
** �޸���:
** ��  ��:  20090409
**--------------------------------------------------------------------------------------------------
***********************************************************************************************/
 void QepEncodPos(void)
{
//	Uint16 temp1;
	float temp_pos;
/*//20091021atzy������жϣ�����AB�źŲ��ضԵ���ԭ�Ⱦ������������ֳ����Ե�����������
//----------------------�ж�POSCNT�������ȷ��������ģʽ------------------------------------------
	if(M_ChkFlag(SL_QCAPSPDIN)!=0 && M_ChkFlag(SL_SPEED_HIGH)!=0)	//20091020atzy�������ʱ���������Ҫ��//ת�ټ�������󣬵�������������ģʽ
	{
		temp1 = EQep2Regs.QEPSTS.bit.QDF;   				//��������  0-��ʱ��-��   1-˳ʱ��-��
		if (temp1 == 0 && M_ChkFlag(SL_QEPSWP)==0) 			//�����ǰ���ڼ�����ģʽ������δ�Ե���AB
		{
			temp1 = EQep2Regs.QDECCTL.bit.SWAP;				//����AB�Ƿ�Ե������Ե���ǰ��AB�ź�
		
			if (temp1 == 0) 
			{
				EQep2Regs.QPOSCNT = 1000;					//��ֹ�Ե��������������			
				EQep2Regs.QDECCTL.bit.SWAP  = 1;			//����A��B�����ź�			
				EQep2Regs.QPOSCNT = 0; 					
			}
			else 
			{ 
				EQep2Regs.QPOSCNT = 1000;				 	//��ֹ�Ե��������������
				EQep2Regs.QDECCTL.bit.SWAP  = 0;			//��ԭA��B�����ź�
				EQep2Regs.QPOSCNT = 0; 						
			} 
		
			M_SetFlag(SL_QEPSWP);					    	//��AB�Ե���ɱ�־λ����ֹ��ζԵ�AB
		}
//		else if (temp1==0 && M_ChkFlag(SL_QEPSWP)!=0)
//			M_SetFlag(SL_QEPPCDE);  						//��λ�ü�������������־λ,Ϊ��߿���������,�ù��ϲ����ж� 20090804��cpc

	}
*/

//--------20121103--------
	DIP_STA_I.sintheta = TRS_NGS_U.sintheta; 
	DIP_STA_I.costheta = TRS_NGS_U.costheta; 
//------------------���Ӳ����dqֵ--------------------------------------------------------------------
   	DIP_STA_I.a     = AD_OUT_STA_I.ac;
   	DIP_STA_I.b     = AD_OUT_STA_I.ba;
   	DIP_STA_I.c     = AD_OUT_STA_I.cb;
	Transform_3s_2s_2r(&DIP_STA_I);
//	DataFilter(0.99,&DIP_STA_I.dflt,DIP_STA_I.d); 	//ע�����˲���400us/2.5kHzִ��һ�Ρ����Ӳ��������ֵ�˲�,Ts=200us,�˵�����Ƶ�ʴ�0.97
	DataFilter(0.99,&DIP_STA_I.qbsf,DIP_STA_I.q); 	//c=0.99->8Hz; c=0.9->88Hz

//	BS_STA_Id.in = DIP_STA_I.dbsf;					//50Hz notch ��Is��dc���˵�
	BS_STA_Iq.in = DIP_STA_I.qbsf;
//	BS_Loop_S(&BS_STA_Id,BS_PARA_STAI);	//�ݲ���ʹ����2e-4ִ��һ�Σ�������200usִ��һ��201205LVRTatZB
	BS_Loop_S(&BS_STA_Iq,BS_PARA_STAI);
//	DIP_STA_I.dflt = BS_STA_Id.out;
	DIP_STA_I.qflt = BS_STA_Iq.out;

//   	TRS_MPR_I.a     = AD_OUT_MPR_I.a + 0;	//��֤���������ת�ӿ��ƶ����г�ʼ��ƯBJTULVRT201204
//   	TRS_MPR_I.b     = AD_OUT_MPR_I.b + 0;
//   	TRS_MPR_I.c     = AD_OUT_MPR_I.c + 0;
   	TRS_MPR_I.a     = AD_OUT_MPR_I.a;	//�����г�ʼ��ƯBJTULVRT201204
   	TRS_MPR_I.b     = AD_OUT_MPR_I.b;
  	TRS_MPR_I.c     = AD_OUT_MPR_I.c;
	Transform_3s_2s_2r(&TRS_MPR_I);
	DataFilter(0.4,&TRS_MPR_I.dflt,TRS_MPR_I.d); 			//�����������ֵ˲��� Ts=200us,fh=1.2kHz,�˵�����Ƶ�ʴ�
	DataFilter(0.4,&TRS_MPR_I.qflt,TRS_MPR_I.q); 			//�����������ֵ�˲��� Ts=200us,fh=1.2kHz,�˵�����Ƶ�ʴ�

//--------20121103--------

//--------QCAP�������������������λ��--------------------------------------------------------------
	if(M_ChkFlag(SL_QCAPSPDIN)!=0 && M_ChkFlag(SL_MPR_SYNOK)==0)//ת�ټ�������Ҷ��Ӳ���ǰ�����������λ�� 201005atcpc
	{
//		temp_pos = _encodpos;
//		QEPDATA.encodpos =  - temp_pos;   	//�ɲ�����ֱ�Ӹ�����ʼλ�ýǶ�										
//--------20130801--------
	   if((_STDBY9&0x0040)!=0)	//116.6����������
	   {
		temp_pos = _encodpos;
		QEPDATA.encodpos =  - temp_pos;   	//�ɲ�����ֱ�Ӹ�����ʼλ�ýǶ�	
	   }
	   else
	   {
		if(M_ChkFlag(SL_MPR_START)==0)
		{
			M_SetFlag(SL_ENCODEPOS_OK);		//20130525
			MAIN_LOOP.cnt_ecposcheck = 0;
			MAIN_LOOP.cnt_ecpostrace = 0;
		}
		else if(M_ChkCounter(MAIN_LOOP.cnt_ecposcheck,1500)>0)		//���๤��1.5s��ʼ�ж�ѹ��
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
//						QEPDATA.encodpos = QEPDATA.encodpos - 0.5235988;		//30��
						QEPDATA.encodpos = QEPDATA.encodpos - 0.5061455;		//29��	20130604
						if (QEPDATA.encodpos < - TWOPAI)  	QEPDATA.encodpos = QEPDATA.encodpos + TWOPAI;
						ecpostep2 = 1;
						ecpostep3 = 1;
						MAIN_LOOP.cnt_ecpostrace = 0;
					}
					else if((abs(MEAN_DATA.ubc_t)>90) && (ecpostep2 == 0))
					{
//						QEPDATA.encodpos = QEPDATA.encodpos + 0.5235988;		//30��
						QEPDATA.encodpos = QEPDATA.encodpos + 0.5410521;		//31��	20130604
						if (QEPDATA.encodpos > 0)  	QEPDATA.encodpos = QEPDATA.encodpos - TWOPAI;
						ecpostep1 = 1;
						MAIN_LOOP.cnt_ecpostrace = 0;
					}
					else if(((abs(MEAN_DATA.ubc_t)<=90) && (ecpostep2 == 0)) || (ecpostep1 == 1))
					{
//						QEPDATA.encodpos = QEPDATA.encodpos - 0.3490659;		//20��
						QEPDATA.encodpos = QEPDATA.encodpos - 0.3839724;		//22��	20130604
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
							QEPDATA.encodpos = QEPDATA.encodpos + 0.08727;		//5��=0.08727
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
** ��������: PhaseOrderChk
** ��������: ��������Ƿ���ȷ��
** �䡡��:
** �䡡��:   
** ע  ��: �ں�����ǰ���У������Ϻ��ٵ��ô��Ӻ��� cap5-ubc   cap6-uab
**--------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 	20090627
**--------------------------------------------------------------------------------------------------
** �޸���:	
** ��  ��:	
**--------------------------------------------------------------------------------------------------
***********************************************************************************************/
 void PhaseOrderChk(void)
{
//	float temp,phaseshift;
	float temp;

//------------������ѹƵ�ʡ���Ƶ�ʡ�CAP��λʱ������ѹ��λ����--------------------------------------
	temp=(float)ECap5Regs.CAP1;									//��¼����ֵ

	if(temp>0)
	{
		CAP5.prd = temp;										
		CAP5.freqtmp   = 150.0e6/CAP5.prd;						//���㱾��Ƶ��
	}

//	if (abs(CAP5.freqtmp-50)<1)									//���Ƶ�����������Ƶ��
	if ((CAP5.freqtmp>_sc_freq2) && (CAP5.freqtmp<_sc_freq1))	//���Ƶ�����������Ƶ��201005atcpc
	{
		CAP5.freq   = CAP5.freqtmp;    							//����
		CAP5.omigasyn  = TWOPAI * CAP5.freq;						
		CAP5.radpertb  = TWOPAI/CAP5.prd;						//����һ���������ڽǶ�����
//		M_ClrFlag(SL_GRDFQE);									//��Ƶ��ʧ�ܱ�־
	}
//	else	M_SetFlag(SL_GRDFQE);								//������Ƶ�������־λ	
		
/*
//----------------����Ubc�Uab��-----------------------------------------------------------------
	temp = (float)ECap5Regs.TSCTR;
	CAP5.phase = temp * CAP5.radpertb;							//����ǰ��ѹUbc��λ 
	temp = (float)ECap6Regs.TSCTR;
	temp = temp * CAP5.radpertb;								//����ǰ��ѹUab��λ
	
//---------------������λ��------------------------------------------------------------------------
	phaseshift = temp - CAP5.phase;								//��������ǰ��ѹUab��Ubc����λ��
	if (phaseshift<0)   phaseshift = phaseshift + TWOPAI;		//���������0��2 * PAI֮��

//---------------��������Ƿ���ȷ------------------------------------------------------------------
	if(abs(phaseshift - TWOPAI_3) >  PAI_3)  					//���������PAI/3ʱ����
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_phorde,DELAY_PHORDE)>0)	//12s �ϵ���ʱ�ж������������ 201005atcpc
			M_SetFlag(SL_PHORDE);								//DSP�ϵ��	���������־λ��1 20090817
	}
	else 
	{
		M_ClrFlag(SL_PHORDE);									// �������������־λ
		MAIN_LOOP.cnt_phorde=0;
	}
*/
//--------���������ⷽ��20120228	BJTULVRT201204--------
//----------------����Ubc��Uab��λ-----------------------------------------------------------------
	temp = (float)ECap5Regs.TSCTR;
	CAP5.phase = temp * CAP5.radpertb;							//����ǰ��ѹUbc��λ 
//---------------��������Ƿ���ȷ------------------------------------------------------------------
	if((CAP5.phase < TWOPAI_12 ))  					//���������PAI/3ʱ����
	{
		if(AD_OUT_NGS_U.ab < 0)
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_phorde,DELAY_PHORDE)>0)	//12s �ϵ���ʱ�ж������������ 201005atcpc
			M_SetFlag(SL_PHORDE);			//DSP�ϵ��	��������־λ��1 20090817
		}
		else
		{
			M_ClrFlag(SL_PHORDE);									// �������������־λ
			MAIN_LOOP.cnt_phorde=0;
		}
	}
}

/**************************************************************************************************
** ��������: CapQepCtrl
** ��������: compute vector position and frequency
** ��  ��:	  
** �䡡��:    
** ע  ��:    
			//Cap4.adsmptsctr=ECap4Regs.TSCTR; 		//����AD������������
			//QEPDATA.adsmposcnt=EQep2Regs.QPOSCNT;	//����AD������������
**--------------------------------------------------------------------------------------------------
** ������: 
** �ա�:	20090409 
**--------------------------------------------------------------------------------------------------
** �޸���:
** ��  ��:	20090812 at cpc
**--------------------------------------------------------------------------------------------------
**************************************************************************************************/
 void CapQepCtrl(void)
{
	float	temp,temp1,radpert0;
	Uint32  temp2,temp3;

 if((_COMMAND2&0x0001)!=0)	//����������0  201011PLL
 {

//------------����������ѹ�ʡ��ֽ�Ƶ�ʡ�CAP�λʱ������ѹ��λ����-----------------------------------
	temp2 = ECap4Regs.CAP1;
	temp  =	(float)temp2;									//�����¼�����ֵ
	
	if(temp>0)
	{
		CAP4.prd = temp;									
		CAP4.freqtmp   = 150.0e6/CAP4.prd;					//���㱾����ѹƵ��
	}

//	if (abs(CAP4.freqtmp - 50)<1)							//���Ƶ�������������ѹƵ��
	if ((CAP4.freqtmp>_sc_freq2) && (CAP4.freqtmp<_sc_freq1))	//���Ƶ�����������Ƶ��201005atcpc
	{
		CAP4.freq      = CAP4.freqtmp;				    	//������ѹƵ�� CAP4.freqtmp��ʵ��ʵʱ��Ƶ
		CAP4.omigasyn  = TWOPAI * CAP4.freq;						
		CAP4.radpertb  = TWOPAI * CAP4.freq / 150.0e6;		//����һ���������ڽǶ�����
		M_ClrFlag(SL_GRDFQE);					   	   	 	//��Ƶ��ʧ�ܱ�־
	}
	else	M_SetFlag(SL_GRDFQE);							//Ƶ�������־λ��1	
		


//-----------���������㷨���任�Ƕ�----------------------------------------------------------------                         
	
	radpert0       = TWOPAI * CAP4.freq * 0.2e-3;   		//����һ��T0������ڽǶ����� T0=0.2ms
  
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
 else	//��ǶȲ���PLL
 {
//-----------PLL���������Ƶ�ʺͽǶ�-201011PLL--------------------------------------------------------------
	DataFilter(0.999,&PI_DATA_PLL.outflt,PI_DATA_PLL.out); 	//PLL�����Ƶ���ڵ����ڼ����,�����ʵ��˲�
	temp    = PI_DATA_PLL.outflt;
	
	if(temp>0)	CAP4.freqtmp   = temp / TWOPAI;				//���㱾����ѹƵ��

//	if (abs(CAP4.freqtmp - 50)<1)							//���Ƶ�������������ѹƵ��
	if ((CAP4.freqtmp>_sc_freq2) && (CAP4.freqtmp<_sc_freq1))//���Ƶ�������ұ���10ms�ָ�Ƶ��ˢ��201011PLL
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_freqrecv,DELAY_FREQRECV)>=0)
		{
			CAP4.freq      = CAP4.freqtmp;				    	//������ѹƵ�� CAP4.freqtmp��ʵ��ʵʱ��Ƶ
			CAP4.omigasyn  = temp;			
			CAP4.radpertb  = CAP4.omigasyn / 150.0e6;			//����һ���������ڽǶ�����			
			M_ClrFlag(SL_GRDFQE);					   	   	 	//��Ƶ�ʧ�ܱ�־	
		}
	}
	else	
	{
		M_SetFlag(SL_GRDFQE);								//Ƶ�������־λ��1	
		MAIN_LOOP.cnt_freqrecv=0;
	}

//-----------���������㷨���任�Ƕ�-201011PLL----------------------------------------------------------------                         
	
	radpert0 = CAP4.omigasyn * 0.2e-3;   					//����һ��T0�ж����ڽǶ����� T0=0.2ms
  
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

//-------------LV�ڼ���ඨ��Ƕȿ����ſ���20121103LVRT-------------------------
	if(M_ChkFlag(SL_LV_STATE)!=0)
	{
	   CAP4.nprtrstheta_lv =CAP4.nprtrstheta_lv + radpert0;              
       if(CAP4.nprtrstheta_lv >= TWOPAI)	CAP4.nprtrstheta_lv = CAP4.nprtrstheta_lv - TWOPAI; 		
	}
	else
	{
		CAP4.nprtrstheta_lv =CAP4.nprtrstheta;              
	}
//����������2  
	if((_COMMAND2&0x0004)==0)	CAP4.nprtrstheta_lv =CAP4.nprtrstheta;	//=1��LV�ڼ����Ƕȿ����ŵ���,��Ϊ��ѹ��λ����

/*
//-------------ת�ӻ�е��Ƶ�ʺ͵��Ƶ��------------------------------------------------------------	

	if(M_ChkFlag(SL_SPEED_HIGH)!=0)
	{
		QEPDATA.qposlat2 = EQep2Regs.QPOSLAT;                 		//����SYSCLK=150M��QUPRD=1.5M,��λƵ��Ϊ100Hz
		if(QEPDATA.qposlat2 < QEPDATA.qposlat1)
			QEPDATA.qposlat  = QEPDATA.qposlat2 + _SC_PLSPRVL * 4 - QEPDATA.qposlat1;  //20090817
		else
			QEPDATA.qposlat  = QEPDATA.qposlat2 - QEPDATA.qposlat1;
		
		QEPDATA.qposlat1  = QEPDATA.qposlat2;
		
		temp = 50.0 *  PAI * (float)QEPDATA.qposlat / _SC_PLSPRVL; 		//ת�ӻ�е��Ƶ��rad/s  QEPDATA.omigamec = 2 * PAI * (150M/1.5M) *  QEPDATA.qposlat/(PLSPRVL * 4 )  20090817	 						                            		
	}
	else
	{
		QEPDATA.qcapprd = EQep2Regs.QCPRD;	                		//��eQEPģ���QCLK����32��Ƶ��QCAPʱ�Ӳ���SYSCLKOUT/128  20090817
		temp = 37.5e6 *  PAI/(_SC_PLSPRVL  * (float)QEPDATA.qcapprd); //ת�ӻ�е��Ƶ��rad/s  QEPDATA.omigamec = 2 * PAI * (150M/128) *  64/(QCPRD * PLSPRVL * 4 )  20090817	 						                            		
	}
*/
//--------20121103--------
//-------------ת�ӻ�е��Ƶ�ʺ͵��Ƶ��------------------------------------------------------------	

	if(M_ChkFlag(SL_SPEED_HIGH)!=0)
	{
		QEPDATA.qposlat2 = EQep2Regs.QPOSLAT;                 		//����SYSCLK=150M��QUPRD=1.5M,��λƵ��Ϊ100Hz
//====================��Ҫ���� 201011BJTULVRT======================================================================
		if(QEPDATA.qposlat2 < QEPDATA.qposlat1)				//201011LVRT DSP�ڲ���EQep2Regs.QPOSLAT�����ٶ�û��T0��ô�죡ˢ��100Hz�Ͳ���Ϊ5kHz
		{
			QEPDATA.qposlat  = QEPDATA.qposlat2 + _SC_PLSPRVL * 4 - QEPDATA.qposlat1;  //20090817
			temp = 50.0 *  PAI * (float)QEPDATA.qposlat / _SC_PLSPRVL; 		//ת�ӻ�е��Ƶ��rad/s  QEPDATA.omigamec = 2 * PAI * (150M/1.5M) *  QEPDATA.qposlat/(PLSPRVL * 4 )  20090817	 						                            		
		}			
		else if(QEPDATA.qposlat2 == QEPDATA.qposlat1)
		{
			temp = QEPDATA.omigamec;
		}
		else
		{
			QEPDATA.qposlat  = QEPDATA.qposlat2 - QEPDATA.qposlat1;
			temp = 50.0 *  PAI * (float)QEPDATA.qposlat / _SC_PLSPRVL; 		//ת�ӻ�е��Ƶ��rad/s  QEPDATA.omigamec = 2 * PAI * (150M/1.5M) *  QEPDATA.qposlat/(PLSPRVL * 4 )  20090817	 						                            		
		}
			
		QEPDATA.qposlat1  = QEPDATA.qposlat2;
	}
	else
	{
		QEPDATA.qcapprd = EQep2Regs.QCPRD;	                		//��eQEPģ���QCLK����32��Ƶ��QCAPʱ�Ӳ���SYSCLKOUT/128  20090817
		temp = 37.5e6 *  PAI/(_SC_PLSPRVL  * (float)QEPDATA.qcapprd); //ת�ӻ�е��Ƶ��rad/s  QEPDATA.omigamec = 2 * PAI * (150M/128) *  64/(QCPRD * PLSPRVL * 4 )  20090817	 						                            		
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
   		QEPDATA.omigamec = 0;  				//��ⷶΧ֮�� 
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
		else if(temp >= (0.75 * QEPDATA.omigamec) && temp <= (1.25 * QEPDATA.omigamec))	//8��Ƶʱ��ʧ1��A��B�źŲ��ٽ�����һ�ε�1.25�������1������0.75��
		{
			QEPDATA.omigamec = temp;												//���ת������ȷ��Χ�ڣ�����;���򱣳���һ�ε�ֵ 20090804��cpc
			MAIN_LOOP.cnt_qcapdisturb=0;
			M_ClrFlag(SL_QCAPDISTURB);		
		}
		else
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_qcapdisturb,DELAY_QCAPDISTURB)>=0)		//��ʱ100ms,����10�ξ�������,������ 20090817cpc
			{
				M_SetFlag(SL_QCAPDISTURB);
				QEPDATA.omigamec = temp;    //20090817
			}
		}
	}
	 
   		  		
	QEPDATA.omigarote = POLEPAIRES * QEPDATA.omigamec;								//ת�ӵ��Ƶ��rad/s
//	DataFilter(0.985,&QEPDATA.omigaroteflt,QEPDATA.omigarote); 						//ת�ٷ���ֵ�˲��� Ts=200us,Tr=100ms 20090815
	QEPDATA.rotradpret0 = QEPDATA.omigarote * 0.2e-3;								//ÿ��T0����ת�ӵ�Ƕ���������λ���� 20090815

//-----------ת���Ƶ��----------------------------------------------------------------------------
	CAP4.omigaslp = CAP4.omigasyn - QEPDATA.omigarote;  	//ת���Ƶ��  20090815


//------------���������Ƿ��й���------------------------------------------------------------------	
	temp = EQep2Regs.QFLG.bit.PCO;
	if((temp!=0) && M_ChkFlag(SL_QCAPSPDIN)!=0)		
	{
	   _NQEPPCO++;
//	   if(_NQEPPCO >10)		   M_SetFlag(SL_QEPPCO);    	//�ü����������־λ
	   if(_NQEPPCO >1000)		   M_SetFlag(SL_QEPPCO);    	//�ü����������־λ	�˲�ʱ��2s,20120310
	   EQep2Regs.QCLR.bit.PCO = 1;
	   MAIN_LOOP.cnt_qepcntok=0;
	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_qepcntok,DELAY_QEPCNTOK)>=0) 				//1s ����1sδ�����������
		{
			_NQEPPCO=0;
			M_ClrFlag(SL_QEPPCO);
		}
	}

//--------20121103--------
//-----�����ת/AB�źŷ��ӹ����ж�-20110829HC---------------------	
	if(M_ChkFlag(SL_QCAPSPDIN)!=0)							//ת���ڷ�Χ��
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
	    M_SetFlag(SL_QEPPHE);								//QEP��AB�ź���λ����,����90��,���ϳ����в��ټ��ù���
	    EQep2Regs.QCLR.bit.PHE = 1; 
	}
*/
//---------�б�A/B�ź���ʧ-201007BJTULVRT----------------------------------------------------------------------
	if(M_ChkFlag(SL_QCAPSPDIN)!=0)							//ת���ڷ�Χ��
	{
		temp = EQep2Regs.QFLG.bit.QDC;						//QPOSCNT��������ı�
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

//----------�����㷨���任�Ƕ�--------------------------------------------------------------------- 
    QEPDATA.posilat = EQep2Regs.QPOSILAT;								//Z�źŵ���ʱPOSCNT�ļ���ֵ
//	if(QEPDATA.posilat < (_SC_PLSPRVL * 4 -20))
//	if((QEPDATA.posilat < (_SC_PLSPRVL * 4 -20)) || M_ChkFlag(SL_QEPPCDE)!=0)	//201007BJTULVRT
	if(((QEPDATA.posilat < (_SC_PLSPRVL * 4 -20)) || M_ChkFlag(SL_QEPPCDE)!=0)&&(M_ChkFlag(SL_SPEED_HIGH)!=0)) //��ת�����ټ��,������20110829HC
	{	
		QEPDATA.rotpos = QEPDATA.rotpos + QEPDATA.rotradpret0;
		if(QEPDATA.rotpos > 2* TWOPAI) QEPDATA.rotpos = QEPDATA.rotpos - 2* TWOPAI;
//		if((M_ChkCounter(MAIN_LOOP.cnt_qepzdisturb,DELAY_QEPZDISTURB)>=0)&&(M_ChkFlag(SL_QCAPSPDIN)!=0))    //1s ת�ټ�������󣬼��������λ��201007BJTULVRT
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

	temp1 = QEPDATA.rotpos - QEPDATA.encodpos;				//AD����ʱ��ת��λ�� - ��ʼλ�ýǶ�
                                                            // -2PIE -- 6PIE
	if 		(temp1 < 0) 	  	temp1 = temp1 + TWOPAI;	
	else if (temp1 > TWOPAI)  	temp1 = temp1 - TWOPAI;		//AD����ʱ��ת��λ��ȡģ(0-TWOPAI)
	if 		(temp1 < 0) 	  	temp1 = temp1 + TWOPAI;	
	else if (temp1 > TWOPAI)  	temp1 = temp1 - TWOPAI;		//AD����ʱ��ת��λ��ȡģ(0-TWOPAI)
							
//	temp = TWOPAI_3 + CAP4.nprtrstheta - temp1;				//���任�Ƕȣ�δȡģ����-1.3PAI---2.7PAI�� 
	temp = TWOPAI_3 + CAP4.nprtrstheta_lv - temp1;			//���任�Ƕȣ�-1.3PAI---2.7PAI�� 20121103LVRT
															//��������ǽ�,B���ѹ����ʱ����ѹʸ��Ϊ30degree
	if		(temp > TWOPAI)	 	temp = temp - TWOPAI;	
	else if	(temp < 0)	  		temp = temp + TWOPAI;		//ȡģ (0-TWOPAI)
//	if		(temp > TWOPAI)	 	temp = temp - TWOPAI;	
//	else if	(temp < 0)	  		temp = temp + TWOPAI;		//ȡģ (0-TWOPAI)
		
	CAP4.mprtrstheta = temp;								//�������任�Ƕ�

	QEPDATA.rotposdisp = QEPDATA.rotpos;	//BJTULVRT201204
//	if 		(QEPDATA.rotpos < 0) 	  	QEPDATA.rotposdisp = QEPDATA.rotpos + TWOPAI;	
//	else if (QEPDATA.rotpos > TWOPAI)  	QEPDATA.rotposdisp = QEPDATA.rotpos - TWOPAI;	//QEPDATA.rotposdisp����DA��ʾ
	if 		(QEPDATA.rotposdisp < 0) 	  	QEPDATA.rotposdisp = QEPDATA.rotposdisp + TWOPAI;	
	else if (QEPDATA.rotposdisp > TWOPAI)  	QEPDATA.rotposdisp = QEPDATA.rotposdisp - TWOPAI;	//QEPDATA.rotposdisp����DA��ʾ 201005atcpc


//-----------�������෴�任�Ƕ�--------------------------------------------------------------------
	if(M_ChkT1Direction() == 0)  							//CTR������ 
    	temp3 = EPwm1Regs.TBPRD + EPwm1Regs.TBCTR;
	else 													//CTR�����
		temp3 = 3 * EPwm1Regs.TBPRD - EPwm1Regs.TBCTR;

	temp3 = temp3 * 2; 										//�任��CAP4��ʱ��Ƶ�ʣ�150M

//	temp2 = ECap4Regs.TSCTR;     //problem:������Ч����Ƶ����
//	temp2 = temp2 + temp3;									//�����´����巢ʱECap4Regs.TSCTR��ֵ		
//	temp  = temp2 * CAP4.radpertb;							//���റ任�Ƕȣ�δȡģ��Լ��0---2PAI��
	temp  = CAP4.nprtrstheta + (float)temp3 * CAP4.radpertb;//���ÿ����ź�Ƕȼ��㷴�任�Ƕ�201005atcpc		
	
	if (temp > TWOPAI)	temp = temp - TWOPAI;				//ȡģ2 * PAI
			
	CAP4.npratitheta = temp;								//���෴�任�Ƕ�
	
//----------������෴�任�Ƕ�---------------------------------------------------------------------
//	temp2 = ECap4Regs.TSCTR;           //problem:������Ч����Ƶ����
//	temp  = (float)temp2;
//	temp  = temp * CAP4.radpertb + TWOPAI_3;  				//�ǰ���Ӵ���λ��rad PAI_2=PAI/2=1.5707963	��������ǽ� zlquestion
/*	temp  = CAP4.nprtrstheta + TWOPAI_3;					//���ÿ����ź�Ƕȼ��㷴�任�Ƕ�201005atcpc		
  													
//	temp2 = EQep2Regs.QPOSCNT;
//	temp1 = (float)temp2;									//QEPģ��λ�ü�������ֵ

//	temp1 = 1.53398e-3 * temp1;								//ת������������źŵĻ�rad(��Ƕ�) //temp1 = POLEPAIRES  * PAI * temp1/(PLSPRVL * 2.0);
	temp1 = 1.5708 * POLEPAIRES * QEPDATA.adsmposcnt / ((float)_SC_PLSPRVL); 			//201005atcpc							

	temp1 = temp1 - QEPDATA.encodpos;						//�ǰת��ʵ��λ��rad	

	if 		(temp1 < 0) 	   	temp1 = temp1 + TWOPAI;
	else if (temp1 > TWOPAI) 	temp1 = temp1 - TWOPAI;		//��ǰת��λ��ȡģ
		
	temp = temp - temp1; 	 								//��ǰ���任�Ƕ�
*/	
	temp = CAP4.mprtrstheta; 	 							//��ǰ���任�Ƕ� 201011BJTULVRT

	if(M_ChkT3Direction() == 0) temp1 = EPwm4Regs.TBPRD + EPwm4Regs.TBCTR;		//CTR������  	
	else 						temp1 = 3 * EPwm4Regs.TBPRD - EPwm4Regs.TBCTR;	//CTR������
		
	temp1 = temp1 * CAP4.omigaslp * 1.333333e-8; 			//ʣ��ʱ���Ӧ�ĽǶ�����PWMclk:75M, 1/75.0e6=1.3333e-8
															//�����Ƕ�temp1��������CAP4.omigaslp���ű仯
	temp = temp + temp1; 									//���任�Ƕȣ�δȡģ��Լ��-1.5PAI---2.5PAI��		

	if		(temp > TWOPAI)		temp = temp - TWOPAI;		
	else if	(temp < 0)		  	temp = temp + TWOPAI;		//ȡģ2 * PAI
	if		(temp > TWOPAI)		temp = temp - TWOPAI;		
	else if	(temp < 0)		  	temp = temp + TWOPAI;		//ȡģ2 * PAI 201005atcpc

	CAP4.mpratitheta = temp; 								//���෴�任�Ƕ�

//--------------------��ֵ��3/2�任�ṹ�����-------------------------------------------------------

	TRS_NPR_I.sintheta = sin(CAP4.nprtrstheta);	
	TRS_NPR_I.costheta = cos(CAP4.nprtrstheta);				//���ඨ��Ƕ�
	TRS_NGS_U.costheta = TRS_NPR_I.costheta;	
	TRS_NGS_U.sintheta = TRS_NPR_I.sintheta;				//���ඨ��Ƕ�
	TRS_NPR_U.sintheta = sin(CAP4.npratitheta);	
	TRS_NPR_U.costheta = cos(CAP4.npratitheta);				//������Ƶ�ѹ�Ƕ� 

	TRS_MPR_I.sintheta = sin(CAP4.mprtrstheta);				//���������
	TRS_MPR_I.costheta = cos(CAP4.mprtrstheta);		
	TRS_MPR_U.sintheta = sin(CAP4.mpratitheta);				//ת�ӿ��Ƶ�ѹ�Ƕ� 
	TRS_MPR_U.costheta = cos(CAP4.mpratitheta);		

}

/**************************************************************************************************
** ��������: 	MPR_CONTROL
** ��������:  	��������������㷨
** �䡡��:   	
** �䡡��: 		 
** ע  ��: 		���ת��Ϊ��	
**-------------------------------------------------------------------------------------------------
** ������: 		
** �ա���: 	
**-------------------------------------------------------------------------------------------------
** �޸���:
** ����:	 
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/
void MPR_CONTROL(void)
{
   float temp_d,temp_q,temp_exi;
//	Uint16 temp_n,temp_ud;

//------------------�������dqֵ--------------------------------------------------------------------
/*   	TRS_MPR_I.a     = AD_OUT_MPR_I.a;
   	TRS_MPR_I.b     = AD_OUT_MPR_I.b;
   	TRS_MPR_I.c     = AD_OUT_MPR_I.c;
	Transform_3s_2s_2r(&TRS_MPR_I);
	DataFilter(0.4,&TRS_MPR_I.dflt,TRS_MPR_I.d); 			//�����������ֵ�˲��� Ts=200us,fh=1.2kHz,�˵�����Ƶ�ʴ�
	DataFilter(0.4,&TRS_MPR_I.qflt,TRS_MPR_I.q); 			//�����������ֵ�˲��� Ts=200us,fh=1.2kHz,�˵�����Ƶ�ʴ�
*///��ת�Ƶ�QepEncodPos������㣬�õ��������ֵBJTULVRT201204

//----------------���л��������-------------------------------------------------------------------
	if(M_ChkFlag(SL_MPR_RUNING)!=0)							//���������Ҫ����
	{	
/*
		PI_MPR_Id.reference      = RUN.mpridrf;
		PI_MPR_Id.feedback       = TRS_MPR_I.dflt; 			//����d�������
		PI_Loop(&PI_MPR_Id,PI_PARA_MPRID);   

		PI_MPR_Iq.reference      = RUN.mpriqrf;		  	  
		PI_MPR_Iq.feedback       = TRS_MPR_I.qflt;			//����q�������
  		PI_Loop(&PI_MPR_Iq,PI_PARA_MPRIQ);
*/
//-----------Ӧ��LV����˥������-BJTULVRT201204 
//			if((_COMMAND2&0x0008)!=0)		//����3 201201
//			{
//				PI_MPR_Id.reference    = RUN.mpridrf;
//				PR_M_Id.reference      = RUN.mpridrf;
//			}
//			else
//			{
//				if(M_ChkFlag(SL_LV_PHICON)!=0)
//				{
//					 PI_MPR_Id.reference    = RUN.mpridrf + PHI_DATA_M.Id;		//��̬�������
//					 PR_M_Id.reference      = RUN.mpridrf + PHI_DATA_M.Id;		//��̬��������PR				
//				}
//				else 
//				{
					PI_MPR_Id.reference    = RUN.mpridrf;
//					PR_M_Id.reference      = RUN.mpridrf;
//				}
//			}
		
			PI_MPR_Id.feedback     = TRS_MPR_I.dflt; 			//����d�������
//			PR_M_Id.feedback       = TRS_MPR_I.dflt; 			//����d�������

//			PI_Loop(&PI_MPR_Id,PI_PARA_MPRID);

//			if(M_ChkFlag(SL_LV_SYNCON)!=0 && ((_STDBY9&0x0001)!=0)) //����0 ��̬PI��������
//			{
//				PI_Loop(&PI_MPR_Id,PI_PARA_DYMI);   		
//			}
//			else
//			{
				PI_Loop(&PI_MPR_Id,PI_PARA_MPRID);   
//			}

		
//			if((_COMMAND2&0x0010)!=0)	//����4 201201
//			{
//				PI_MPR_Iq.reference    = RUN.mpriqrf;	
//				PR_M_Iq.reference      = RUN.mpriqrf;	
//			}
//			else
//			{
//				if(M_ChkFlag(SL_LV_PHICON)!=0)
//				{
						PI_MPR_Iq.reference    = RUN.mpriqrf + PHI_DATA_M.Iq;		 //��̬�������� 
//						PR_M_Iq.reference      = RUN.mpriqrf + PHI_DATA_M.Iq;		 //��̬�������� 
//				}
//				else	
//				{
					PI_MPR_Iq.reference    = RUN.mpriqrf;		  
//					PR_M_Iq.reference      = RUN.mpriqrf;		  
//				}
//			}	  	  
		
			PI_MPR_Iq.feedback     = TRS_MPR_I.qflt;			//����q�������
//			PR_M_Iq.feedback       = TRS_MPR_I.qflt;			//����q�������

//  			PI_Loop(&PI_MPR_Iq,PI_PARA_MPRIQ);	  	  	  
		

//			if(M_ChkFlag(SL_LV_SYNCON)!=0 && ((_STDBY9&0x0001)!=0)) //��̬PI��������
//			{
//  				PI_Loop(&PI_MPR_Iq,PI_PARA_DYMI);	  	  	  	
//			}
//			else
//			{
  				PI_Loop(&PI_MPR_Iq,PI_PARA_MPRIQ);	  	  	  
//			} 
   
/*			if((_STDBY9&0x0008)!=0)  //116����3 Ӧ��PR������20120315
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
   
//--------------MPR�����ѹ��---------------------------------------------------------------------

//   DM_imrd = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * CAP4.omigasyn));	//20100121
//   DM_imrq = - (TRS_NGS_U.qflt * SQRT3 * STAROTRTO / (MPR_Lm * CAP4.omigasyn));  //��ѹq��ǰ��������
/*	DM_imrd  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));  		//����:��ѹed�����d�����180��             
    temp_ud = (Uint16)(TRS_NGS_U.dflt * SQRT3 / SQRT2);								//��ѹ�ߵ�ѹ��Чֵ 690V
    if     (temp_ud < 540) 		temp_ud = 540;
    else if(temp_ud > 800) 	    temp_ud = 800;
	temp_n = (temp_ud - 540) * 127 / 260;
    temp_exi = TAB_EXI[temp_n];
    DM_imrd =  DM_imrd * temp_exi;          										//������ϵ��,�ɲ��������� 

*/
//--------------MPR�����ѹ����BJTULVRT20121103--------------------------------------------------------------------
//	if(M_ChkFlag(SL_MPR_SYNOK)!=0 || (_SC_MSTDBY!=0))  //201201	
	if(M_ChkFlag(SL_MPR_SYNOK)!=0)  //20121103	
		DM_imrd =  - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * CAP4.omigasyn));	////201112fluxobs
	else
		DM_imrd =  - RUN.mpridrf / RUN.mpridrf_g * (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * CAP4.omigasyn));	//201112fluxobs
/*
	temp_d = - PI_MPR_Id.out + SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Iq.feedback;	//���������

//	temp_d = temp_d	- CAP4.omigaslp * MPR_Lr * DM_imrq;								//��ѹq��ǰ��������

  	temp_q = - PI_MPR_Iq.out - SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Id.feedback;	//���������
	temp_q = temp_q	- CAP4.omigaslp * MPR_Lr * DM_imrd;                               
*/
	if((M_ChkFlag(SL_UNBALANCE)==0) && (M_ChkFlag(SL_LV_STATE)!=0) && (NGS_Udq_p_lv < (0.8 * NGS_Udq_p_ex)))		  //20130223
	{
		temp_d = - PI_MPR_Id.out + SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Iq.feedback + TRS_MPR_I.dflt * _stdby02;	//���������

	  	temp_q = - PI_MPR_Iq.out - SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Id.feedback;	//���������
		temp_q = temp_q	- CAP4.omigaslp * MPR_Lr * DM_imrd + TRS_MPR_I.qflt * _stdby02;

	}
	else
	{
		temp_d = - PI_MPR_Id.out + SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Iq.feedback;	//���������

	 	temp_q = - PI_MPR_Iq.out - SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Id.feedback;	//���������
		temp_q = temp_q	- CAP4.omigaslp * MPR_Lr * DM_imrd;

	}


//	temp_d = temp_d - MPR_Rr * PI_MPR_Id.feedback;
//	temp_q = temp_q - MPR_Rr * PI_MPR_Iq.feedback;									//ת�ӵ���ѹ��
  
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
	Anti_Transform_2r_2s(&TRS_MPR_U);							//���귴�任�����ྲֹϵ

//------------SVM�����������巢��-----------------------------------------------------------------	
	SVPWM_MPR(TRS_MPR_U.alfa,TRS_MPR_U.beta);					//���巢��

//------------Teʵ��ת�ط���ֵ����-----------------------------------------------------------------	
//	DataFilter(0.8,&DIP_MPR_I.qflt,TRS_MPR_I.q); 					//���Ӳ๦�ʼ���,����q������ Ts=200us,fh=Hz
	DataFilter(0.8,&DIP_MPR_I.qflt,TRS_MPR_I.qflt); 					//���Ӳ๦�ʼ���,����q������ Ts=200us,fh=Hz
	Te_feedback = - (1.5 * SQRT3 * POLEPAIRES * MPR_Lm  / (314.15926 * STAROTRTO * MPR_Ls)) * DIP_NPR_U.dflt *  DIP_MPR_I.qflt;     // Te=-1.5*np*(Us/w1)*(Irq/K)*(Lm/Ls),MPR_iΪ��ֵ
//	if(Te_feedback<=0)	Te_feedback=0;   							//20090817	20121129ȡ��atsp
//-----BJTULVRT20121103
	Te_instant  = - (1.5 * SQRT3 * POLEPAIRES * MPR_Lm  / (314.15926 * STAROTRTO * MPR_Ls)) * TRS_NGS_U.dflt *  TRS_MPR_I.qflt;     // Te=-1.5*np*(Us/w1)*(Irq/K)*(Lm/Ls),MPR_iΪ��ֵ

}  


/**************************************************************************************************
** ��������: 	NPR_CONTROL
** ��������:  	��������������㷨
** �䡡��:   	
** �䡡��: 		 
** ע  ��: 		�����������Ϊ��	
**-------------------------------------------------------------------------------------------------------
** ������: 		
** �ա���: 		
**-------------------------------------------------------------------------------------------------------
** �޸���:
** ����:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void NPR_CONTROL(void)
{ 
//	float temp_d,temp_q;

//-----------�����������������dqֵ-----------------------------------------------------------------
	TRS_NPR_I.a = AD_OUT_NPR_I.a;  							//��ȡ�������������
	TRS_NPR_I.b = AD_OUT_NPR_I.b;
	TRS_NPR_I.c = AD_OUT_NPR_I.c;
	Transform_3s_2s_2r(&TRS_NPR_I);							//����任
	DataFilter(0.4,&TRS_NPR_I.dflt,TRS_NPR_I.d); 			//�����������ֵ�˲��� Ts=200us,fh=1.2kHz,�˵�����Ƶ�ʴ�
	DataFilter(0.4,&TRS_NPR_I.qflt,TRS_NPR_I.q); 			//����������ֵ�˲��� Ts=200us,fh=1.2kHz��20090615��

//----------������ѹdqֵ----------------------------------------------------------------------------
	TRS_NGS_U.a = AD_OUT_NGS_U.a;							//��ȡ������ѹ
	TRS_NGS_U.b = AD_OUT_NGS_U.b;
	TRS_NGS_U.c = AD_OUT_NGS_U.c;
	Transform_3s_2s_2r(&TRS_NGS_U);							//����任

	if(TRS_NGS_U.q > 200)			TRS_NGS_U.q = 200;		//20130228
	else if(TRS_NGS_U.q < -200)		TRS_NGS_U.q = -200;		//20130228

	DataFilter(0.4,&TRS_NGS_U.dflt,TRS_NGS_U.d); 			//��ѹ��ֵ�˲���Ts=200us,fh=1.2kHz 20090608change to ok
	DataFilter(0.4,&TRS_NGS_U.qflt,TRS_NGS_U.q); 			//��ѹ������˲���Ts=200us,fh=1.2kHz 20090608change to ok
//	DataFilter(0.1,&TRS_NGS_U.dflt2,TRS_NGS_U.d); 			//��ѹ����ֵ�˲���Ts=200us,fh=7.9kHz,126us,Ϊ���������20091026


	if(M_ChkFlag(SL_NPR_RUNING)!=0)							//����PI���п�������
	{

//----------���е�ѹ�⻷----------------------------------------------------------------------------
	  if(M_ChkFlag(SL_STEADYGV)==0)  						//Vdcû���ȶ����Ҳ�����̬PI����
	  {

		PI_NPR_U.reference     = - RUN.urf;   				//��ȡ�м��ѹָ��
		PI_NPR_U.feedback      = - AD_OUT_UDC;				//��ȡ����ѹ����ֵ
    	PI_Loop(&PI_NPR_U,PI_PARA_DYNU);
      }	
	  else                                                   //������̬PI����
	  {
		PI_NPR_U.reference     = - RUN.urf;   				//��ȡ�м��ѹָ��
		PI_NPR_U.feedback      = - AD_OUT_UDC;				//��ȡ�м��ѹ����ֵ
    	PI_Loop(&PI_NPR_U,PI_PARA_NPRU); 
      }
//--------��35%-2ph��������ѹ���޷�20130222--------
		if(M_ChkFlag(SL_LV_STATE)!=0)
		{
			if(M_ChkFlag(SL_UNBALANCE)!=0 && (NGS_Udq_epsilon < 50) && M_ChkFlag(SL_LV_SCRRUNING)==0)	//��20%-2ph������2ph����ֵ
			{
				if(PI_NPR_U.out > lv_ctrl6)				PI_NPR_U.out = lv_ctrl6;	//20130223
				else if(PI_NPR_U.out < -lv_ctrl6)		PI_NPR_U.out = -lv_ctrl6;	//20130223
			}
		}

//---------����d�������----------------------------------------------------------------------------
   		PI_NPR_Id.reference      = PI_NPR_U.out; 			//��ȡd�������ע��������
		PI_NPR_Id.feedback       = TRS_NPR_I.dflt;			//��ȡd����������˲�ֵ
		PI_Loop(&PI_NPR_Id,PI_PARA_NPRID);

//---------����q�������----------------------------------------------------------------------------
		PI_NPR_Iq.reference      = RUN.npriqrf; 			//q�����ָ��
		PI_NPR_Iq.feedback       = TRS_NPR_I.qflt;	  		//��ȡq���������˲�ֵ
		PI_Loop(&PI_NPR_Iq,PI_PARA_NPRIQ);	  	  
	}

//---------�����תϵ��NPR���Ƶ�ѹ------------------------------------------------------------------

//	TRS_NPR_U.d = PI_NPR_Id.out  + TRS_NGS_U.dflt - CAP4.omigasyn * NPR_L * PI_NPR_Iq.feedback;
//	TRS_NPR_U.q = PI_NPR_Iq.out  + TRS_NGS_U.qflt + CAP4.omigasyn * NPR_L * PI_NPR_Id.feedback;
//	if((_STDBY9&0x0020)==0)		  //116.5 ȡ��ǰ���˲�2013022
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

//---------SVM�������-----------------------------------------------------------------------------
	SVPWM_NPR(TRS_NPR_U.alfa,TRS_NPR_U.beta);				//���巢��

//--20121103---�����й����ʼ���(3��)-----------------------------------------------------------------	
	DataFilter(0.99,&DIP_NPR_U.dflt,TRS_NGS_U.dpll); 			//���Ӳ๦�ʼ���,��ѹed�� Ts=200us,Tr=250ms

//------------�����й����ʼ���(3��)-----------------------------------------------------------------	
/*	DataFilter(0.99,&DIP_NPR_I.dflt,TRS_NPR_I.d); 			//���Ӳ๦�ʼ���,��ѹed�� Ts=200us,fh=88Hz
	DataFilter(0.99,&DIP_NPR_I.qflt,TRS_NPR_I.q); 			//���Ӳ๦�ʼ���,����q������ Ts=200us,fh=88Hz
	DataFilter(0.99,&DIP_NPR_U.qflt,TRS_NGS_U.q); 			//���Ӳ๦�ʼ���,����q������ Ts=200us,fh=88Hz
	DataFilter(0.995,&DIP_NPR_U.dflt,TRS_NGS_U.d); 			//���Ӳ๦�ʼ���,��ѹed�� Ts=200us,Tr=250ms

	Pnactive   = 1.5 * (DIP_NPR_U.dflt * DIP_NPR_I.dflt + DIP_NPR_U.qflt * DIP_NPR_I.qflt);   	// Pnactive=3*Ud*Id/(squt2*squt2)
	Pnreactive = 1.5 * (DIP_NPR_U.qflt * DIP_NPR_I.dflt - DIP_NPR_U.dflt * DIP_NPR_I.qflt);   	// Pnactive=3*Ud*Id/(squt2*squt2)
*///20121103
}


/**************************************************************************************************
** ����: SysCtrl
** ��������: 1.5MW������ϵͳ�߼�����-������ 	
** �䡡��:   	
** �䡡��: 		 
** ע  ��: 	systest		
**-------------------------------------------------------------------------------------------------
** ������: 	 
** �ա���: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** �޸���:
** ����:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void SysCtrl(void)         
{
	if(M_ChkFlag(SL_SERIESTOPING)!=0)							//�������ع���ͣ����
	{
		SER_STOP();												//ִ�����ع���ͣ��,�ȶ�����
	}
	else if(M_ChkFlag(SL_SERIESTOP)!=0)							//�������ع���
	{
		M_SetFlag(SL_SERIESTOPING);								//�� �������ع���ͣ���� ��־
	}	
	else if(M_ChkFlag(SL_ERRSTOPING)!=0)						//����һ�����ͣ����
	{
		ERR_STOP();												//ִ��һ�����ͣ��,�����ֶ��Ͽ�CB����201005atcpc
	}
	else if(M_ChkFlag(SL_ERRSTOP)!=0)							//�ж��Ƿ����һ�����
	{
		M_SetFlag(SL_ERRSTOPING);								//�� ����һ�����ͣ���� ��־
	}
	else if(M_ChkFlag(SL_NORSTOPING)!=0)						//��������ͣ����
	{
		NOR_STOP();												//ִ������ͣ��,�����ֶ��Ͽ�CB����201005atcpc
	}
	else if(M_ChkFlag(SL_OCS_EIN)==0)							//�Ƿ���յ�����BIT0�պ�����ָ��
	{
		M_SetFlag(SL_NORSTOPING);								//�� ��������ͣ���� ��־
	}
	else if(M_ChkFlag(SL_CBCLOSED)==0)							//���ϱպ��Ƿ����
	{
		CB_ON();												//ִ�����ϱպϲ���
	}
	else if(M_ChkFlag(SL_OCS_SYSRUN)==0)						//�Ƿ���յ�����BIT2ͬ������ָ��
	{
		M_SetFlag(SL_NORSTOPING);								//�� ��������ͣ���� ��־
	}
	else if(M_ChkFlag(SL_MPR_SYNOK)==0)							//���Ӳ����Ƿ������
	{
		RUN_SYN();												//ִ��Ԥ�絽ͬ����������
	}
	else														//ִ�ж��Ӳ��������п���
	{
		if((AD_OUT_UDC>(_URF - 20))&&(AD_OUT_UDC<(_URF + 20))) 	//�������ж�ֱ����ѹ�Ƿ��ȶ�+-20V  
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
** ����: RUN_SYN
** �������: 1.5MW������ϵͳ�߼�����-ͬ������	
** �䡡��:   	
** �䡡��: 		 
** ע  ��: 			
**-------------------------------------------------------------------------------------------------
** ������: 	 
** �ա���: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** �޸���:
** ����:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void RUN_SYN(void)         
{ 
	if(M_ChkFlag(SL_SYN_FH)!=0)										//FH=�������
	{
		if(M_ChkFlag(SL_IN1_STATORK)!=0)							//�ж϶��ӽӴ����Ƿ��ѱպ�
		{		
			M_SetFlag(SL_MPR_SYNOK);								//�ö��Ӳ������
			M_ClrFlag(SL_SYN_FH);									//�岽���־λ
			M_ClrFlag(SL_SYN_S3);									//�岽���־λ
			M_ClrFlag(SL_SYN_S2);									//�岽���־λ
			M_ClrFlag(SL_SYN_S1);									//�岽���־λ
		}	
	}
	else if(M_ChkFlag(SL_SYN_S3)!=0)								//S3=�������ඨ�Ӳ���
	{
		M_SetFlag(SL_MPR_START);									//�������������

		if(MEAN_DATA.uab_d <= _SC_UDSTAC && MEAN_DATA.ubc_d <= _SC_UDSTAC)    //���ӷ��ϲ�������    					//���ӽӴ���ǰ��벨ƽ��ֵ����40V����
		{
			M_SetFlag(SL_UDSTACOK);									//�� ���Ӵﵽ�������� ��־

		    if(M_ChkFlag(SL_OCS_OPENSTAC)==0)	
		    {														//�ⲿ���ƣ�Ϊ0������պ�STAC
		      	M_SetFlag(CL_STATORK);  							//��λ��������,�պ϶��ӽӴ���
				M_SetFlag(SL_SYN_FH);								//�ò����־λ
				MAIN_LOOP.cnt_synrun=0;           					//�������		
		    }                             					
			else	M_ClrFlag(CL_STATORK);							//��λ����ֹ����,�Ͽ����ӽӴ���	        
		}
		else	M_ClrFlag(SL_UDSTACOK);								//�� ���Ӵﵽ�������� ��־
	}
	else if(M_ChkFlag(SL_SYN_S2)!=0)								//S2=��������
	{
		M_SetFlag(SL_OCS_NPRSTART);									//���DSP_PWM����

		if(M_ChkFlag(SL_CONFIGPWM)!=0)   M_SetFlag(SL_NPR_START);	//pwmdrive����PWM���,�����������		  
		else   	                         M_ClrFlag(SL_NPR_START); 

		if(AD_OUT_UDC>(_URF - 20) && AD_OUT_UDC<(_URF + 20) && M_ChkFlag(SL_NPR_PWMOUT)!=0) //���������ѷ�����Vdc�ȶ�
   		{
   			if(M_ChkCounter(MAIN_LOOP.cnt_synrun,DELAY_STEADYFB)>=0)
			{
				M_SetFlag(SL_STEADYFB);          					//��Vdc�ȶ�
				M_SetFlag(SL_SYN_S3);								//�ò����־λ
				MAIN_LOOP.cnt_synrun=0;           					//�������		
			}
   		}
		else
		{
			M_ClrFlag(SL_STEADYFB);          						//��Vdc�ȶ�
			MAIN_LOOP.cnt_synrun=0;           						//������		
		}	
	}
	else if(M_ChkFlag(SL_SYN_S1)!=0)								//S1=Ԥ���
	{
		if( AD_OUT_UDC>=_SC_VDCON && M_ChkCounter(MAIN_LOOP.cnt_synrun,DELAY_PRECOK)>=0) //8s��Vdc>700V 
		{
			M_SetFlag(CL_MAINK);        							//Ԥ���OK���պ����Ӵ��� 
      	
	      	if(M_ChkFlag(SL_IN1_MIANK)!=0 && M_ChkFlag(SL_IN1_MIANFILTER)!=0 && AD_OUT_UDC>=870)  //���Ӵ������˲������պ�
			{
				M_ClrFlag(CL_PRE);          	 					//�Ͽ�Ԥ���Ӵ���
		 		M_SetFlag(SL_CHARGEOK);               				//Ԥ������         
				M_SetFlag(SL_FAN_WORK);								//�������
				M_SetFlag(SL_SYN_S2);								//�ò����־λ
				MAIN_LOOP.cnt_synrun=0;           					//�������	
			}
		}
	}
	else															//S0=��Ԥ���Ӵ���
	{
      	if(M_ChkFlag(SL_SPEED_IN_RANGE)!=0)     					//ת���ڷ�Χ��
		{
			M_SetFlag(CL_PRE);          	 						//�պϤ���Ӵ���
			M_SetFlag(SL_SYN_S1);									//�ò����־λ
			MAIN_LOOP.cnt_synrun=0;           						//�������	
		}
	}
}

/**************************************************************************************************
** ����: NOR_STOP
** ��������: 1.5MW������ϵͳ�߼�����-����ͣ��
** �䡡��:   	
** �䡡��: 		 
** ע  ��: 			
**-------------------------------------------------------------------------------------------------
** ������: 	 
** �ա���: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** �޸���:
** ����:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void NOR_STOP(void)         
{ 
	if(M_ChkFlag(SL_NORSTOP_FH)!=0)									//FH=�Ͽ����Ӵ���
	{
		M_ClrFlag(CL_MAINK);    		  							//�����Ӵ������˲���
		M_ClrFlag(CL_PRE); 											//�ٶ�Ԥ���Ӵ���
	   	M_ClrFlag(SL_CHARGEOK);          							//��Ԥ������

		if(M_ChkFlag(SL_IN1_MIANK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)==0)
		{
			M_ClrFlag(SL_FAN_WORK);									//ֹͣ�������
			if(M_ChkFlag(SL_PCOCS_CBOFF)!=0 && M_ChkFlag(SL_OCS_EIN)==0)//PC�ֶ����ƶϿ�CB 201005atcpc
			{
			   	M_ClrFlag(CL_CBON);    		     					//�����Ϻ�բָ��
			   	M_ClrFlag(CL_CBLVC);         						//��Ƿѹ���Ͽ������϶Ͽ� 
			   	M_ClrFlag(CL_CBFCON);            					//�Ͽ�������
				M_ClrFlag(SL_CBCLOSED);			 					//�����ϱպ����
				M_ClrFlag(SL_PCOCS_CBOFF);            				//��Ͽ�ָ��
				M_ClrFlag(SL_CBON_FH);								//��CBON�����־λ
			} 
/*		   	M_ClrFlag(SL_NORSTOPING);          						//�� ��������ͣ���� ��־
			M_ClrFlag(SL_NORSTOP_FH);								//�岽���־λ
			M_ClrFlag(SL_NORSTOP_S2);								//�岽���־λ
			M_ClrFlag(SL_NORSTOP_S1);								//�岽���־λ
*/
//--------20120321--------
			if(M_ChkFlag(SL_SYSOFF)==0)
			{
			   	M_ClrFlag(SL_NORSTOPING);          						//�� ��������ͣ���� ��־
				M_ClrFlag(SL_NORSTOP_FH);								//�岽���־λ
				M_ClrFlag(SL_NORSTOP_S2);								//�岽���־λ
				M_ClrFlag(SL_NORSTOP_S1);								//�岽���־λ
			}
//------------------------

		}	
	}
	else if(M_ChkFlag(SL_NORSTOP_S2)!=0)							//S2=ͣ����
	{
		M_ClrFlag(SL_NPR_START);									//����������ָ��
		M_ClrFlag(SL_OCS_NPRSTART);									//����������ָ��

		if(M_ChkFlag(SL_NPR_PWMOUT)==0)								//����������ֹͣ
		{
			M_ClrFlag(SL_STEADYFB);          						//��Vdc�ȶ�
			M_SetFlag(SL_NORSTOP_FH);								//�ò����־λ
			MAIN_LOOP.cnt_norstop=0;           						//�������	
		}
	}
	else if(M_ChkFlag(SL_NORSTOP_S1)!=0)							//S1=�Ͽ���ӽӴ���ͣ����
	{        
		if(M_ChkFlag(SL_IN1_STATORK)==0)							//���ӽӴ����Ͽ�
		{
			M_ClrFlag(SL_MPR_SYNOK);								//�嶨�Ӳ������,��RUNCtrl�й�,��Ҫ�����޸�		
			M_ClrFlag(SL_UDSTACOK);									//�� ���Ӵﵽ�������� ��־
			
			if(M_ChkFlag(SL_MPR_PWMOUT)==0)							//��������ֹͣ
			{
				M_SetFlag(SL_NORSTOP_S2);							//�ò����־λ
				MAIN_LOOP.cnt_norstop=0;           					//�������	
			}
		}      								
	}
	else															//S0=ͣ��������
	{
		M_ClrFlag(SL_MPR_START);									//���������ָ��,�����й�����

        if(M_ChkFlag(SL_STACKEEPON)==0)	  
        {
        	M_ClrFlag(CL_STATORK);    								//�ﵽ���ӶϿ�����, �Ͽ����ӽӴ���
			M_SetFlag(SL_NORSTOP_S1);								//�ò����־λ
			MAIN_LOOP.cnt_norstop=0;           						//�������	
        }	

		M_ClrFlag(SL_SYN_FH);										//��SYN�����־λ
		M_ClrFlag(SL_SYN_S3);										//��SYN�����־λ
		M_ClrFlag(SL_SYN_S2);										//��SYN�����־λ
		M_ClrFlag(SL_SYN_S1);										//��SYN�����־λ	
	}
}

/**************************************************************************************************
** ����: SER_STOP
** ��������: 1.5MW������ϵͳ�߼�����-���ع���ͣ�� 	
** �䡡��:   	
** �䡡��: 		 
** ע  ��: 			
**-------------------------------------------------------------------------------------------------
** ������: 	 
** �ա���: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** �޸���:
** ����:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void SER_STOP(void)         
{ 
	if(M_ChkFlag(SL_SERSTOP_FH)!=0)
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_serstop,DELAY_SERSTOP)>0)		//2s ���϶Ͽ���ʱ���ٶ϶��ӽӴ��������Ӵ���
		{
			M_ClrFlag(CL_MAINK);    		  						//�����Ӵ������˲��
		   	M_ClrFlag(CL_PRE); 										//ٶ�Ԥ���Ӵ���
		   	M_ClrFlag(CL_STATORK);    	  							//�϶��ӽӴ���	
		   	M_ClrFlag(SL_CHARGEOK);          						//��Ԥ������
			M_ClrFlag(SL_FAN_WORK);									//ֹͣ�������
		   	M_ClrFlag(SL_MPR_SYNOK);          						//�嶨�Ӳ������
		   	M_ClrFlag(SL_UDSTACOK);          						//�嶨�����㲢������
		
			if(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(SL_IN1_MIANK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)==0 && M_ChkFlag(SL_IN1_STATORK)==0)
			{
				M_ClrFlag(SL_SERSTOP_FH);							//�岽���־λ
				M_ClrFlag(SL_SERIESTOPING);							//�� �������ع���ͣ���� ��־
			}
		}
	}
	else
	{
	   	M_ClrFlag(CL_CBON);    		     							//�����Ϻ�բָ��
	   	M_ClrFlag(CL_CBLVC);         								//��Ƿѹ���Ͽ������϶Ͽ� 
	   	M_ClrFlag(CL_CBFCON);            							//�Ͽ�������
	  	M_ClrFlag(SL_OCS_NPRSTART);         						//����������ָ��
	  	M_ClrFlag(SL_OCS_EIN);         								//�����ϱպ�ָ�� 201005atcpc
	  	M_ClrFlag(SL_OCS_SYSRUN);         							//��Ԥ���ָ��	201005atcpc
		M_ClrFlag(SL_NPR_START);									//����������ָ��
		M_ClrFlag(SL_MPR_START);									//���������ָ��
	   	M_ClrFlag(SL_CBCLOSED);			 							//�����ϱպ����
	   	M_ClrFlag(SL_STEADYFB);          							//��Vdc�ȶ�
		M_SetFlag(SL_SERSTOP_FH);									//�ò����־λ
		MAIN_LOOP.cnt_serstop=0;           							//�������	

		M_ClrFlag(SL_SYN_FH);										//��SYN�����־λ
		M_ClrFlag(SL_SYN_S3);										//��SYN�����־λ
		M_ClrFlag(SL_SYN_S2);										//��SYN�����־λ
		M_ClrFlag(SL_SYN_S1);										//��SYN�����־λ
		M_ClrFlag(SL_NORSTOPING);          							//�� �������ͣ���� ��־
		M_ClrFlag(SL_NORSTOP_FH);									//��NORSTOP�����־λ
		M_ClrFlag(SL_NORSTOP_S2);									//��NORSTOP�����־λ
		M_ClrFlag(SL_NORSTOP_S1);									//��NORSTOP�����־λ
		M_ClrFlag(SL_ERRSTOPING);									//�� ����һ�����ͣ���� ��־
		M_ClrFlag(SL_ERRSTOP_FH);									//��ERRSTOP�����־λ
		M_ClrFlag(SL_CBON_FH);										//��CBON�����־λ
	}
}


/**************************************************************************************************
** ����: ERR_STOP
** ��������: 1.5MW������ϵͳ�߼�����-������ͣ�� 	
** �䡡��:   	
** �䡡��: 		 
** ע  ��: 			
**-------------------------------------------------------------------------------------------------
** ������: 	 
** �ա���: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** �޸���:
** ����:	20121103�ĳ��뻪��һ����ȶ������ٶ϶��ӽӴ���
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void ERR_STOP(void)         
{
	if(M_ChkFlag(SL_ERRSTOP_FH)!=0)
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_errstop,DELAY_ERRSTOP)>0)		//2s ���϶Ͽ���ʱ���ٶ϶��ӽӴ��������Ӵ���
		{
			M_ClrFlag(CL_MAINK);    		  						//�����Ӵ������˲���
		   	M_ClrFlag(CL_PRE); 										//�ٶ�Ԥ���Ӵ���
		   	M_ClrFlag(CL_STATORK);    	  							//�϶��ӽӴ���	
		   	M_ClrFlag(SL_CHARGEOK);          						//��Ԥ������
			M_ClrFlag(SL_FAN_WORK);									//ֹͣ�������
		   	M_ClrFlag(SL_MPR_SYNOK);          						//�嶨�Ӳ������
		   	M_ClrFlag(SL_UDSTACOK);          						//�嶨�����㲢������
		
			if(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(SL_IN1_MIANK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)==0 && M_ChkFlag(SL_IN1_STATORK)==0)
			{
				M_ClrFlag(SL_ERRSTOP_FH);							//�岽���־λ
				M_ClrFlag(SL_ERRSTOP_S1);							//�岽���־λ
				M_ClrFlag(SL_ERRSTOPING);							//�� �������ع���ͣ���� ��
			}
		}
	}
	else if(M_ChkFlag(SL_ERRSTOP_S1)!=0)
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_epwmstop,DELAY_EPWMSTOP)>0)		//500ms
		{
			M_ClrFlag(CL_CBON);    		     							//�����Ϻ�բָ��
	   		M_ClrFlag(CL_CBLVC);         								//��Ƿѹ���Ͽ������϶Ͽ� 
	   		M_ClrFlag(CL_CBFCON);            							//�Ͽ�������
	   		M_ClrFlag(SL_CBCLOSED);			 							//�����ϱպ����			
			M_SetFlag(SL_ERRSTOP_FH);									//�ò����־λ		
		}		
		MAIN_LOOP.cnt_errstop=0;           							//�������	
	}
	else
	{
	  	M_ClrFlag(SL_OCS_NPRSTART);         						//����������ָ��
	  	M_ClrFlag(SL_OCS_EIN);         								//�����ϱպ�ָ�� 201005atcpc
	  	M_ClrFlag(SL_OCS_SYSRUN);         							//��Ԥ���ָ��	201005atcpc
		M_ClrFlag(SL_NPR_START);									//����������ָ��
		M_ClrFlag(SL_MPR_START);									//���������ָ��
	   	M_ClrFlag(SL_STEADYFB);          							//��Vdc�ȶ�
		MAIN_LOOP.cnt_errstop=0;           							//�������	
		MAIN_LOOP.cnt_epwmstop=0;
		M_SetFlag(SL_ERRSTOP_S1);									//�ò����־λ			

		M_ClrFlag(SL_SYN_FH);										//��SYN�����־λ
		M_ClrFlag(SL_SYN_S3);										//��SYN�����־λ
		M_ClrFlag(SL_SYN_S2);										//��SYN�����־λ
		M_ClrFlag(SL_SYN_S1);										//��SYN�����־λ
		M_ClrFlag(SL_NORSTOPING);          							//�� ��������ͣ���� ��־
		M_ClrFlag(SL_NORSTOP_FH);									//��NORSTOP�����־λ
		M_ClrFlag(SL_NORSTOP_S2);									//��NORSTOP�����־λ
		M_ClrFlag(SL_NORSTOP_S1);									//��NORSTOP�����־λ
	} 
/*	if(M_ChkFlag(SL_ERRSTOP_FH)!=0)
	{
		M_ClrFlag(CL_PRE); 											//��Ԥ���Ӵ���
		M_ClrFlag(CL_MAINK);    		  							//�����Ӵ������˲���
		M_ClrFlag(SL_CHARGEOK);          							//��Ԥ������

		if(M_ChkFlag(SL_IN1_MIANK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)==0)
		{
			M_ClrFlag(SL_FAN_WORK);									//ֹͣ�������
			if(M_ChkFlag(SL_PCOCS_CBOFF)!=0 && M_ChkFlag(SL_OCS_EIN)==0)//PC�ֶ����ƶϿ�CB 201005atcpc
			{
			   	M_ClrFlag(CL_CBON);    		     					//�����Ϻ�բָ��
			   	M_ClrFlag(CL_CBLVC);         						//��Ƿѹ���Ͽ������϶Ͽ� 
			   	M_ClrFlag(CL_CBFCON);            					//�Ͽ�������
				M_ClrFlag(SL_CBCLOSED);			 					//�����ϱպ����
				M_ClrFlag(SL_PCOCS_CBOFF);            				//��Ͽ�ָ��
				M_ClrFlag(SL_CBON_FH);								//��CBON�����־λ
			} 
			M_ClrFlag(SL_ERRSTOP_FH);								//�岽���־λ
			M_ClrFlag(SL_ERRSTOPING);								//�� ����һ�����ͣ���� ��־
		}	
	}
	else
	{
	  	M_ClrFlag(SL_OCS_EIN);         								//�����ϱպ�ָ��
	  	M_ClrFlag(SL_OCS_SYSRUN);         							//��Ԥ���ָ�� 201005atcpc
	  	M_ClrFlag(SL_OCS_NPRSTART);         						//����������ָ��
		M_ClrFlag(SL_NPR_START);									//����������ָ��
		M_ClrFlag(SL_MPR_START);									//���������ָ��
		M_ClrFlag(CL_STATORK);    	  								//�϶��ӽӴ���	
		M_ClrFlag(SL_UDSTACOK);          							//�嶨����㲢������
		M_ClrFlag(SL_MPR_SYNOK);          							//�嶨�Ӳ������

//		if(M_ChkFlag(SL_NPR_PWMOUT)==0 && M_ChkFlag(SL_MPR_PWMOUT)==0 && M_ChkFlag(SL_IN1_STATORK)==0)
		if(M_ChkFlag(SL_NPR_PWMOUT)==0 && M_ChkFlag(SL_MPR_PWMOUT)==0)//20100510
		{
	   		M_ClrFlag(SL_STEADYFB);          							//��Vdc�ȶ�
			M_SetFlag(SL_ERRSTOP_FH);								//�ò����־λ
			MAIN_LOOP.cnt_errstop=0;           						//�������	
		}

		M_ClrFlag(SL_SYN_FH);										//��SYN�����־λ
		M_ClrFlag(SL_SYN_S3);										//��SYN�����־λ
		M_ClrFlag(SL_SYN_S2);										//��SYN�����־λ
		M_ClrFlag(SL_SYN_S1);										//��SYN�����־λ
		M_ClrFlag(SL_NORSTOPING);          							//�� ��������ͣ���� ��־
		M_ClrFlag(SL_NORSTOP_FH);									//��NORSTOP�����־λ
		M_ClrFlag(SL_NORSTOP_S2);									//��NORSTOP�����־λ
		M_ClrFlag(SL_NORSTOP_S1);									//��NORSTOP�����־λ
	}
*/
}

/**************************************************************************************************
** ����: CB_ON
** ��������: 1.5MW������ϵͳ�߼�����-���ϱպ� 	
** �䡡��:   	
** �䡡��: 		 
** ע  ��: 			
**-------------------------------------------------------------------------------------------------
** ������: 	 
** �ա���: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** �޸���:
** ����:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void CB_ON(void)         
{ 
	if(M_ChkFlag(SL_CBON_FH)!=0)
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_cbon,DELAY_CBON)>0)		//1s ��ʱ�����Ϻ�բָ��
		{
		   	M_SetFlag(CL_CBON);    		     					//�����Ϻ�բָ��
		}

		if(M_ChkFlag(SL_IN1_CBSTS)!=0)
		{
	   		M_SetFlag(SL_CBCLOSED);			 					//�����ϱպ����
		   	M_ClrFlag(CL_CBON);    		     					//�����Ϻ�բָ��
			M_ClrFlag(SL_CBON_FH);								//�岽���־λ
		}	
	}
	else
	{
	   	M_SetFlag(CL_CBLVC);         							//�պ�����Ƿѹ�
	   	M_SetFlag(CL_CBFCON);            						//�պϹ�����
		M_SetFlag(SL_CBON_FH);									//�ò����λ
		MAIN_LOOP.cnt_cbon=0;           						//�������	
	}
}


/**************************************************************************************************
** ����: CONTACTOR_TEST
** ��������: 1.5MW������ϵͳ�߼�����-�Ӵ������� 	
** �䡡��:   	
** �䡡��: 		 
** ע  ��: 			
**-------------------------------------------------------------------------------------------------
** ������: 	 
** �ա���: 2010-5-22		
**-------------------------------------------------------------------------------------------------
** �޸��:
** ����:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
/*	else if(_STDBY8==7 && M_ChkFlag(SL_RUN)==0 && M_ChkFlag(SL_IN1_CBSTS)==0 && AD_OUT_UDC<50) //��������,CB�Ͽ�,Vdcû��������������							//
	{
		CONTACTOR_TEST();										//ִ�нӴ������Բ��� 201005atcpc
	}


void CONTACTOR_TEST(void)         								//systest 201005atcpc
{ 
	M_ClrFlag(CL_CBON);    		     							//�����Ϻ�բָ��
   	M_ClrFlag(CL_CBLVC);         								//��Ƿѹ��Ͽ������϶Ͽ� 
   	M_ClrFlag(CL_CBFCON);            							//�Ͽ�������
  	M_ClrFlag(SL_OCS_NPRSTART);         						//���������ָ��
	M_ClrFlag(SL_NPR_START);									//����������ָ��
	M_ClrFlag(SL_MPR_START);									//���������ָ��
   	M_ClrFlag(SL_CBCLOSED);			 							//�����ϱպ����
   	M_ClrFlag(SL_CHARGEOK);          							//��Ԥ������
	M_ClrFlag(SL_FAN_WORK);										//ֹͣ�������
  	M_ClrFlag(SL_MPR_SYNOK);          							//�嶨�Ӳ������
   	M_ClrFlag(SL_UDSTACOK);          							//�嶨�����㲢������

   	if(M_ChkFlag(SL_OCS_SYSRUN)!=0)								//�ⲿ���ƣ�Ϊ1�űպ�PREC/MC/MF
   	{
   		M_SetFlag(CL_MAINK);    		  						//�����Ӵ������˲���
  		M_SetFlag(CL_PRE); 										//�ٶ�Ԥ���Ӵ���
  	}
	else
	{
   		M_ClrFlag(CL_MAINK);    		  						//�����Ӵ������˲���
  		M_ClrFlag(CL_PRE); 										//ٶ�Ԥ���Ӵ���	
	}

    if(M_ChkFlag(SL_OCS_OPENSTAC)==0)							//�ⲿ���ƣ�Ϊ0������պ�STAC
     		M_SetFlag(CL_STATORK);  							//��λ��������,�պ϶��ӽӴ���                            					
	else	M_ClrFlag(CL_STATORK);								//��λ����ֹ����,�Ͽ����ӽӴ���	        

}

*/
/////////////////no more///////////////////
