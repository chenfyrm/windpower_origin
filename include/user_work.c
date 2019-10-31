   /****************************************Copyright (c)**************************************************
**                       		     ��	��	��	ͨ	��	ѧ
**                                        ��������ѧԺ
**                                         614ʵ����
** 
**                              
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: user_work.c
**��   ��   ��: 
**����޸�����: 
**��        ��: 1.5MW˫�����������Ŀ��Ƴ���----���Ʒ糡
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

//��������
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
			   if(M_ChkFlag(SL_CONFIGPWM)==0 && M_ChkFlag(SL_NPR_PWMOUT)==0 && M_ChkFlag(SL_MPR_PWMOUT)==0) //û�����ÿ���Ƶ����������� 20100507atzuoyun
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
	        if((M_ChkFlag(SL_NPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0))
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
            if((M_ChkFlag(SL_MPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0))
	        {
		       if(M_ChkFlag(SL_MPR_PWMOUT)==0)						//��֤Enepwmio_MPR()ֻ������һ��
		       {
			      Enepwmio_MPR();      
			      M_SetFlag(SL_MPR_PWMOUT);			         
		       }
	        }
	        else                  									//�������Ͼ�����������
	        {		       
			   Disepwmio_MPR();       
			   M_ClrFlag(SL_MPR_PWMOUT);        
	        }
	}
    else   //ϵͳ�����ϵ�,��Ԥ���բû���г�
	{      
		DisPwm();
		M_ClrFlag(SL_NPR_PWMOUT);
		M_ClrFlag(SL_MPR_PWMOUT);
		M_ClrFlag(SL_CONFIGPWM);			        				//�������
		MAIN_LOOP.cnt_pwmout=0;										//���ϵͳ����������ʱ
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
      	case 1:    temp1=(Uint16) (t0 * SW_NPR);				//SW_NPRΪPWM���ڼĴ�����ֵ,Ϊ�������ڵ�һ��
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
   EPwm4Regs.CMPA.half.CMPA = temp1;
   EPwm5Regs.CMPA.half.CMPA = temp2;
   EPwm6Regs.CMPA.half.CMPA = temp3; 
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
		M_ClrFlag(SL_MPR_RUN);			    						//�����������־��0
		GIVE.urf      = _URF;										//���������ѹΪ��ǰVdcֵ
		GIVE.npriqrf  = 0;											//�����������Ϊ0
        GIVE.toqrf    = 0;                  						//�������ת��Ϊ0
        GIVE.anglerf  = 0;		            						//����������������Ƕ�Ϊ0
	}
	else
	{

		if(M_ChkFlag(SL_OCS_NPREIN)!=0)					           //�����޹�����,�������ֱ����ѹ���޹�����ֵ
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
			if(M_ChkFlag(SL_NPR_START)!=0)  		   				//���������ж�
			{
			   M_SetFlag(SL_NPR_RUN);                  				//������������		       
		       GIVE.urf       = (int16) _URF;          				//���м�ֱ����ѹָ�ֵ
			   GIVE.npriqrf   = (int16) _NIQRF;        				//������������޹���֤������������Ϊ1
//			   GIVE.npriqrf   = 70.0;                  				//������������޹���֤������������Ϊ1������ʵ����֤��Ҫ��70A
			}
		    else     M_ClrFlag(SL_NPR_RUN);


	        if(M_ChkFlag(SL_MPR_START)!=0)  						//���������ж�
	        {
	           M_SetFlag(SL_MPR_RUN);                   			//������������
 
               if(_CANOPER==0)	  
               {
					if(M_ChkFlag(SL_IN1_STATORK)!=0)				//���ӽӴ����պ��Ժ��ٽ���ת�غͽǶ�ָ��
					{ 
               			GIVE.toqrf  =  (int16) _TOQRF;              //��ֵΪ����
						GIVE.anglerf=  (int16) _AGLRF;  			//������������ָ�ֵ,-360 -- 360
					}
					else
      		   		{
      		   			GIVE.toqrf    = 0;                  		//�������ת��Ϊ0
        				GIVE.anglerf  = 0;		            		//����������������Ƕ�Ϊ0
      		   		} 
			   }
               else   
               {           
					if(M_ChkFlag(SL_IN1_STATORK)!=0)				//���ӽӴ����պ��Ժ��ٽ���ת�غͽǶ�ָ��
					{
               			temp_toqrf     =  (int16)SCI_canopen.rx_torque;               //��ֵΪ����
      		   			GIVE.toqrf     =  temp_toqrf * CAN_TEN / 16383.0;             //����ת��ָ��ʵ��ֵ  _TOQRF == -200%-200%

						if(GIVE.toqrf<0)	GIVE.toqrf=0;    //�����������صĸ�ת��ָ�����Ӧ 20090815
			   
               			temp_aglrf     =  (int16)SCI_canopen.rx_angle;               
      		   			GIVE.anglerf   =  temp_aglrf * CAN_ANGLEN / 16383.0;  
      		   		}
      		   		else
      		   		{
      		   			GIVE.toqrf    = 0;                  		//�������ת��Ϊ0
        				GIVE.anglerf  = 0;		            		//����������������Ƕ�Ϊ0
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
    float udc_max,id_max,iq_max,toq_max,agl_max,temp;
	Uint16 temp_n,temp_ud,temp_iqexi,temp_number;
	float  temp_exi,temp_iqk;
	float  temp_Qc,temp_Qg,temp_tan;

//----------------------------�������и�������---------------------------------------------------------

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
       M_SetFlag(SL_NPR_RUNING);                 								//�����������У���ʼ����������
       MAIN_LOOP.cnt_isteady0= 0;
	                
	   RUN.urf = Give_Integral(GIVE.urf,RUN.ustep,RUN.urf);     				//��ѹ��������
       if(RUN.urf == GIVE.urf)	    M_SetFlag(SL_STEADYGV);        				//�����������,���ȶ���־λ
	   else							M_ClrFlag(SL_STEADYGV);

 	   if(M_ChkFlag(SL_STEADYGV)!=0)											//����ѹ������ɺ��ٸ����޹�����
	 	   RUN.npriqrf = Give_Integral(GIVE.npriqrf,RUN.npriqstep,RUN.npriqrf); //���๦������������	         
    }
//-----����    
    else                                   										//����
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

//-----�����������в�Ҫ��ͣ��
    if((M_ChkFlag(SL_MPR_RUNING)!=0)&&(M_ChkFlag(SL_MPR_RUN)==0)) 
    {      
		if(M_ChkFlag(SL_IN1_STATORK)!=0)
		{
		    RUN.toqrf = Give_Integral(0,RUN.toqstep,RUN.toqrf);  				//����ת�ظ�������
		    RUN.aglrf = Give_Integral(0,RUN.aglstep,RUN.aglrf);  			    //���๦�������Ǹ�������


		    RUN.mpriqrf_g  =  - RUN.toqrf  / (IRQTOTE * TRS_NGS_U.dflt);   		//����q�����ָ�����
//	    	RUN.mpriqrf_g  =  RUN.toqrf * STAROTRTO / (1.5 * POLEPAIRES * MPR_Lm * TRS_NGS_U.dflt / (314.15926 *  MPR_Ls));   //����q�����ָ�����
 
			RUN_mpridrf_exi  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));  //����:��ѹed�����d�����180��             
      	    RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          				//�˵���ϵ��,�ɲ��������� 
        	temp_ud = (Uint16)(TRS_NGS_U.dflt * SQRT3 / SQRT2);
        	if     (temp_ud < 540) 		temp_ud = 540;
        	else if(temp_ud > 800) 	    temp_ud = 800;
	    	temp_n = (temp_ud - 540) * 127 / 260;
        	temp_exi = TAB_EXI[temp_n];
        	RUN.mpridrf_exi =  RUN.mpridrf_exi * temp_exi;          			//������ϵ��,����ظ�Ӧ�����й�
/*//20091022atzy
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
		    if(RUN.mpriqrf==0)        M_SetFlag(SL_STACTRIPEN);
			else    			      M_ClrFlag(SL_STACTRIPEN);
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
    else if((M_ChkFlag(SL_MPR_RUN)!=0) && (M_ChkFlag(SL_STEADYFB)!=0))  
    {  
        M_SetFlag(SL_MPR_RUNING);
        M_ClrFlag(SL_STACTRIPEN);                 									//�����������У���ʼ����������
        MAIN_LOOP.cnt_mprsteady0= 0;
 			   
		RUN.toqrf = Give_Integral(GIVE.toqrf,RUN.toqstep,RUN.toqrf);  				//����ת�ظ�������
		RUN.aglrf = Give_Integral(GIVE.anglerf,RUN.aglstep,RUN.aglrf);  			//���๦�������Ǹ�������
 
	    RUN.mpriqrf_g  =  - RUN.toqrf  / (IRQTOTE * TRS_NGS_U.dflt);   				//����q�����ָ�����
//	    RUN.mpriqrf  =  GIVE.toqrf * STAROTRTO / (1.5 * POLEPAIRES * MPR_Lm * TRS_NGS_U.dflt / (314.15926 *  MPR_Ls));   //����q�����ָ�����
 
		RUN_mpridrf_exi  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));  //����:��ѹed�����d�����180��             
        RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          						//�˵���ϵ��,�ɲ��������� 
        temp_ud = (Uint16)(TRS_NGS_U.dflt * SQRT3 / SQRT2);							//��ѹ�ߵ�ѹ��Чֵ 690V
        if     (temp_ud < 540) 		temp_ud = 540;
        else if(temp_ud > 800) 	    temp_ud = 800;
	    temp_n = (temp_ud - 540) * 127 / 260;
        temp_exi = TAB_EXI[temp_n];
        RUN.mpridrf_exi =  RUN.mpridrf_exi * temp_exi;          					//������ϵ��,�ɲ��������� 
/*//20091022atzy 
		RUN_mpriq = (Uint16)(- RUN.mpriqrf_g);
		if     (RUN_mpriq < 290)   temp_iqexi = 290;
		else if(RUN_mpriq > 600)   temp_iqexi = 600;
        temp_iqexi = RUN_mpriq;  //20091019atzy
		temp_number = (temp_iqexi - 290) * 127 / 310;
		temp_iqk = TAB_IQEXI[temp_number];
        RUN.mpridrf_exi = RUN.mpridrf_exi * temp_iqk;          						//������ϵ��,�ɸ��ص���Irq����ȷ�� 
*///20091022atzy 
   
 	    RUN.radianrf    = RUN.aglrf * 0.017453292;									//�Ƕ�ת����360->2PAI	 0.01745329=2 * PAI / 360
	    RUN.mpridrf_var = RUN.mpriqrf_g * sin(RUN.radianrf) / cos(RUN.radianrf); //ͣ��ʱ����֤���ӵ���Ϊ0������������Ϊ1
			   	    																//RUN.aglrf>0,ת����������,���������޹�;��֮,����
//	    temp_Qc = TRS_NGS_U.dflt * 314.15926 * 501.0e-6 / SQRT2;					//����ʱ�����ܲ�����������1ΪĿ��
//	    temp_Qg = PRO.Pgactive * sin(RUN.radianrf) / cos(RUN.radianrf);
//		temp_tan    = (temp_Qg - temp_Qc) / PRO.Psactive;
//	    RUN.mpridrf_var = RUN.mpriqrf_g * temp_tan;
        
        RUN.mpridrf_g   = RUN.mpridrf_exi + RUN.mpridrf_var;                                           


		RUN.mpridrf = Give_Integral(RUN.mpridrf_g,RUN.mpridstep,RUN.mpridrf);  		//�����й�������������
		RUN.mpriqrf = Give_Integral(RUN.mpriqrf_g,RUN.mpriqstep,RUN.mpriqrf);  		//�����޹�������������

    } 
//-----����
    else                                    										//����
    {
	   	M_ClrFlag(SL_MPR_RUNING);
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
//	    temp       =  temp * 1000.0;        //ΪCPC����ʵ��̨����й������ٶ� cpc 20090815 ��ת�ظ�����msΪ��λ
	    RUN.toqstep=  abs(toq_max)/temp;											//����ת�ظ������� 
        agl_max    =  (float)FUNC[NO_AGLRF].max;
	    temp       =  (float)_RU_AGLT;
		temp       =  temp * 1000.0;		//��λs									//ΪCPC����ʵ��̨�����й������ٶ� cpc test
	    RUN.aglstep=  abs(agl_max)/temp; 											//���㹦�������Ǹ������� 

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
** ��������:  QepEncodPos
** ��������:  ����λ�ü�����Ϊ������ģʽ��������λ�ü��
** �䡡��:    ��̬�µ�ת�ӵ��������ӵ�ѹ
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
//----------------------�ж�POSCNT��������ȷ��������ģʽ------------------------------------------
	if(M_ChkFlag(SL_QCAPSPDIN)!=0 && M_ChkFlag(SL_SPEED_HIGH)!=0)	//20091020atzy�������ʱ����������ת��//ת�ټ�������󣬵�������������ģʽ
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
//--------QCAP�������������������λ��--------------------------------------------------------------
	if(M_ChkFlag(SL_QCAPSPDIN)!=0)			//ת�ټ�������󣬼��������λ��
	{
  		temp_pos = _encodpos;
		QEPDATA.encodpos =  - temp_pos;   	//�ɲ�����ֱ�Ӹ�����ʼλ�ýǶ�										
	}
	
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
	float temp,phaseshift;

//------------������ѹƵ�ʡ���Ƶ�ʡ�CAP��λʱ������ѹ��λ����--------------------------------------
	temp=(float)ECap5Regs.CAP1;									//��¼����ֵ

	if(temp>0)
	{
		CAP5.prd = temp;										
		CAP5.freqtmp   = 150.0e6/CAP5.prd;						//���㱾��Ƶ��
	}

	if (abs(CAP5.freqtmp-50)<1)									//���Ƶ�����������Ƶ��
	{
		CAP5.freq   = CAP5.freqtmp;    							//����
		CAP5.omigasyn  = TWOPAI * CAP5.freq;						
		CAP5.radpertb  = TWOPAI/CAP5.prd;						//����һ���������ڽǶ�����
		M_ClrFlag(SL_GRDFQE);									//��Ƶ��ʧ�ܱ�־
	}
	else	M_SetFlag(SL_GRDFQE);								//������Ƶ�������־λ	
		

//----------------����Ubc��Uab��λ-----------------------------------------------------------------
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
		if(M_ChkFlag(SL_POWERON)==0)	M_SetFlag(SL_PHORDE);	//DSP�ϵ��	���������־λ��1 20090817
	}
	else 
		M_ClrFlag(SL_PHORDE);									// �������������־λ 
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
** �ա���:	20090409 
**--------------------------------------------------------------------------------------------------
** �޸���:
** ��  ��:	20090812 at cpc
**--------------------------------------------------------------------------------------------------
**************************************************************************************************/
 void CapQepCtrl(void)
{
	float	temp,temp1,radpert0;
	Uint32  temp2,temp3;

//------------����������ѹ�ʡ��ֽ�Ƶ�ʡ�CAP��λʱ������ѹ��λ����-----------------------------------
	temp2 = ECap4Regs.CAP1;
	temp  =	(float)temp2;									//�����¼�����ֵ
	
	if(temp>0)
	{
		CAP4.prd = temp;									
		CAP4.freqtmp   = 150.0e6/CAP4.prd;					//���㱾����ѹƵ��
	}

	if (abs(CAP4.freqtmp - 50)<1)							//���Ƶ�������������ѹƵ��
	{
		CAP4.freq      = CAP4.freqtmp;				    	//������ѹƵ�� CAP4.freqtmp��ʵ��ʵʱ��Ƶ
		CAP4.omigasyn  = TWOPAI * CAP4.freq;						
		CAP4.radpertb  = TWOPAI * CAP4.freq / 150.0e6;		//����һ���������ڽǶ�����
		M_ClrFlag(SL_GRDFQE);					   	   	 	//��Ƶ��ʧ�ܱ�־
	}
	else	M_SetFlag(SL_GRDFQE);							//Ƶ�������־λ��1	
		


//-----------���������㷨���任�Ƕ�----------------------------------------------------------------                         
	
	radpert0       = TWOPAI * CAP4.freq * 0.2e-3;   		//����һ��T0�ж����ڽǶ����� T0=0.2ms
  
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
//-------------���Ӵ���λ��-------------------------------------------------------------------------
//	CAP4.stavectheta = 	CAP4.nprtrstheta + TWOPAI_3;			//���Ӵ��������A�����ߵ�λ��


//-------------ת�ӻ�е��Ƶ�ʺ͵��Ƶ��------------------------------------------------------------	
//	QEPDATA.qcapprd = EQep2Regs.QCPRD;	                		//��eQEPģ���QCLK����512��Ƶ��QCAPʱ�Ӳ���SYSCLKOUT/128  20090817
//	temp = 300.0e6 *  PAI/(PLSPRVL  * (float)QEPDATA.qcapprd); 	//ת�ӻ�е��Ƶ��rad/s  QEPDATA.omigamec = 2 * PAI * (150M/128) *  512/(QCPRD * PLSPRVL * 4 )  20090817	 						                            

//    if(M_ChkFlag(SL_SPEED_HIGH)!=0)		temp = 2400.0e6 *  PAI/(PLSPRVL  * (float)QEPDATA.qcapprd);  //ת�ӻ�е��Ƶ��rad/s  QEPDATA.omigamec = 2 * PAI * 37.5M * 128/(QCPRD * PLSPRVL * 4 )  20090815atcpc
//    else  								temp = 150.0e6 *  PAI/(PLSPRVL  * (float)QEPDATA.qcapprd);   //ת�ӻ�е��Ƶ��rad/s  QEPDATA.omigamec = 2 * PAI * 37.5M * 8/(QCPRD * PLSPRVL * 4 )  20090815atcpc	

	if(M_ChkFlag(SL_SPEED_HIGH)!=0)
	{
		QEPDATA.qposlat2 = EQep2Regs.QPOSLAT;                 		//����SYSCLK=150M��QUPRD=1.5M,��λƵ��Ϊ100Hz
/*
		if(QEPDATA.qposlat2 < QEPDATA.qposlat1)
			QEPDATA.qposlat  = QEPDATA.qposlat2 + PLSPRVL * 4 - QEPDATA.qposlat1;  //20090817
		else
			QEPDATA.qposlat  = QEPDATA.qposlat2 - QEPDATA.qposlat1;
		
		QEPDATA.qposlat1  = QEPDATA.qposlat2;
		
		temp = 50.0 *  PAI * (float)QEPDATA.qposlat / PLSPRVL; 		//ת�ӻ�е��Ƶ��rad/s  QEPDATA.omigamec = 2 * PAI * (150M/1.5M) *  QEPDATA.qposlat/(PLSPRVL * 4 )  20090817	 						                            		
	}
	else
	{
		QEPDATA.qcapprd = EQep2Regs.QCPRD;	                		//��eQEPģ���QCLK����32��Ƶ��QCAPʱ�Ӳ���SYSCLKOUT/128  20090817
		temp = 37.5e6 *  PAI/(PLSPRVL  * (float)QEPDATA.qcapprd); //ת�ӻ�е��Ƶ��rad/s  QEPDATA.omigamec = 2 * PAI * (150M/128) *  64/(QCPRD * PLSPRVL * 4 )  20090817	 						                            		
	}
*/

//====================��Ҫ���� 201105atzuoyun======================================================================
		if(QEPDATA.qposlat2 < QEPDATA.qposlat1)				//201011LVRT DSP�ڲ���EQep2Regs.QPOSLAT�����ٶ�û��T0��ô�죡ˢ��100Hz�Ͳ���Ϊ5kHz
		{
			QEPDATA.qposlat  = QEPDATA.qposlat2 + PLSPRVL * 4 - QEPDATA.qposlat1;  //20090817
			temp = 50.0 *  PAI * (float)QEPDATA.qposlat / PLSPRVL; 		//ת�ӻ�е��Ƶ��rad/s  QEPDATA.omigamec = 2 * PAI * (150M/1.5M) *  QEPDATA.qposlat/(PLSPRVL * 4 )  20090817	 						                            		
		}			
		else if(QEPDATA.qposlat2 == QEPDATA.qposlat1)
		{
			temp = QEPDATA.omigamec;
		}
		else
		{
			QEPDATA.qposlat  = QEPDATA.qposlat2 - QEPDATA.qposlat1;
			temp = 50.0 *  PAI * (float)QEPDATA.qposlat / PLSPRVL; 		//ת�ӻ�е��Ƶ��rad/s  QEPDATA.omigamec = 2 * PAI * (150M/1.5M) *  QEPDATA.qposlat/(PLSPRVL * 4 )  20090817	 						                            		
		}
			
		QEPDATA.qposlat1  = QEPDATA.qposlat2;
	}
	else
	{
		QEPDATA.qcapprd = EQep2Regs.QCPRD;	                		//��eQEPģ���QCLK����32��Ƶ��QCAPʱ�Ӳ���SYSCLKOUT/128  20090817
		temp = 37.5e6 *  PAI/(PLSPRVL  * (float)QEPDATA.qcapprd); //ת�ӻ�е��Ƶ��rad/s  QEPDATA.omigamec = 2 * PAI * (150M/128) *  64/(QCPRD * PLSPRVL * 4 )  20090817	 						                            		
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
//	if(temp!=0)		
	if((temp!=0) && M_ChkFlag(SL_QCAPSPDIN)!=0)		//201105atzuoyun
	{
	   _NQEPPCO++;
	   if(_NQEPPCO >10)		   M_SetFlag(SL_QEPPCO);    	//�ü����������־λ
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
/*						
	temp = EQep2Regs.QFLG.bit.PHE;
	if(temp!=0)		
	{
	    M_SetFlag(SL_QEPPHE);								//QEP��AB�ź���λ����,����90��,���ϳ����в��ټ��ù���
	    EQep2Regs.QCLR.bit.PHE = 1; 
	}
*/
//----------�����㷨���任�Ƕ�--------------------------------------------------------------------- 
    QEPDATA.posilat = EQep2Regs.QPOSILAT;								//Z�źŵ���ʱPOSCNT�ļ���ֵ
	if(QEPDATA.posilat < (PLSPRVL * 4 -20))
	{	
		QEPDATA.rotpos = QEPDATA.rotpos + QEPDATA.rotradpret0;
		if(QEPDATA.rotpos > 2* TWOPAI) QEPDATA.rotpos = QEPDATA.rotpos - 2* TWOPAI;
//		if((M_ChkCounter(MAIN_LOOP.cnt_qepzdisturb,DELAY_QEPZDISTURB)>=0)&&(M_ChkFlag(SL_QCAPSPDIN)!=0))    //1s //ת�ټ�������󣬼��������λ��
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

	temp1 = QEPDATA.rotpos - QEPDATA.encodpos;				//AD����ʱ��ת��λ�� - ��ʼλ�ýǶ�
                                                            // -2PIE -- 6PIE
	if 		(temp1 < 0) 	  	temp1 = temp1 + TWOPAI;	
	else if (temp1 > TWOPAI)  	temp1 = temp1 - TWOPAI;		//AD����ʱ��ת��λ��ȡģ(0-TWOPAI)
	if 		(temp1 < 0) 	  	temp1 = temp1 + TWOPAI;	
	else if (temp1 > TWOPAI)  	temp1 = temp1 - TWOPAI;		//AD����ʱ��ת��λ��ȡģ(0-TWOPAI)
							
	temp = TWOPAI_3 + CAP4.nprtrstheta - temp1;				//���任�Ƕȣ�δȡģ����-1.3PAI---2.7PAI�� 
															//��������ǽ�,B���ѹ����ʱ����ѹʸ��Ϊ30degree
	if		(temp > TWOPAI)	 	temp = temp - TWOPAI;	
	else if	(temp < 0)	  		temp = temp + TWOPAI;		//ȡģ (0-TWOPAI)
//	if		(temp > TWOPAI)	 	temp = temp - TWOPAI;	
//	else if	(temp < 0)	  		temp = temp + TWOPAI;		//ȡģ (0-TWOPAI)
		
	CAP4.mprtrstheta = temp;								//�������任�Ƕ�

	if 		(QEPDATA.rotpos < 0) 	  	QEPDATA.rotposdisp = QEPDATA.rotpos + TWOPAI;	
	else if (QEPDATA.rotpos > TWOPAI)  	QEPDATA.rotposdisp = QEPDATA.rotpos - TWOPAI;	//QEPDATA.rotposdisp����DA��ʾ


//-----------�������෴�任�Ƕ�----201105atzuoyun----------------------------------------------------------------
	if(M_ChkT1Direction() == 0)  							//CTR������ 
    	temp3 = EPwm1Regs.TBPRD + EPwm1Regs.TBCTR;
	else 													//CTR������
		temp3 = 3 * EPwm1Regs.TBPRD - EPwm1Regs.TBCTR;

	temp3 = temp3 * 2; 										//�任��CAP4��ʱ��Ƶ�ʣ�150M
//	temp2 = ECap4Regs.TSCTR;     //problem:������Ч����Ƶ����
//	temp2 = temp2 + temp3;									//�����´����巢ʱECap4Regs.TSCTR��ֵ		
//	temp  = temp2 * CAP4.radpertb;							//���෴�任�Ƕȣ�δȡģ��Լ��0---2PAI��
	temp  = CAP4.nprtrstheta + (float)temp3 * CAP4.radpertb;//���ÿ����ź�Ƕȼ��㷴�任�Ƕ�201105atzuoyun		

//	temp2 =  temp3 * CAP4.radpertb;									
//	temp  = CAP4.nprtrstheta + temp2;						//���෴�任�Ƕȣ�δȡģ��Լ��0---2PAI��
	
	if (temp > TWOPAI)	temp = temp - TWOPAI;				//ȡģ2 * PAI
			
	CAP4.npratitheta = temp;								//���෴�任�Ƕ�
	
//----------������෴�任�Ƕ�---201105atzuoyun------------------------------------------------------------------
/*	temp2 = ECap4Regs.TSCTR;           //problem:������Ч����Ƶ����
	temp  = (float)temp2;
	temp  = temp * CAP4.radpertb + TWOPAI_3;  				//��ǰ���Ӵ���λ��rad PAI_2=PAI/2=1.5707963	��������ǽ� zlquestion
    													
	temp2 = EQep2Regs.QPOSCNT;
	temp1 = (float)temp2;									//QEPģ��λ�ü�������ֵ

	temp1 = 1.53398e-3 * temp1;								//ת������������źŵ�λ��rad(��Ƕ�) //temp1 = POLEPAIRES  * PAI * temp1/(PLSPRVL * 2.0);
              												//0.002618=POLEPAIRES  * PAI/(PLSPRVL * 2.0); POLEPAIRES=2, PLSPRVL=2048cpc                          						
	temp1 = temp1 - QEPDATA.encodpos;						//��ǰת��ʵ��λ��rad	

	if 		(temp1 < 0) 	   	temp1 = temp1 + TWOPAI;
	else if (temp1 > TWOPAI) 	temp1 = temp1 - TWOPAI;		//��ǰת��λ��ȡģ
		
	temp = temp - temp1; 	 								//��ǰ���任�Ƕ�
*/	
	temp = CAP4.mprtrstheta; 	 							//��ǰ���任�Ƕ� 201105atzuoyun

	if(M_ChkT3Direction() == 0) temp1 = EPwm4Regs.TBPRD + EPwm4Regs.TBCTR;		//CTR������  	
	else 						temp1 = 3 * EPwm4Regs.TBPRD - EPwm4Regs.TBCTR;	//CTR������
		
	temp1 = temp1 * CAP4.omigaslp * 1.333333e-8; 			//ʣ��ʱ���Ӧ�ĽǶ�����PWMclk:75M, 1/75.0e6=1.3333e-8

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
   float temp_d,temp_q,temp1,temp2;

//------------------�������dqֵ--------------------------------------------------------------------
   	TRS_MPR_I.a     = AD_OUT_MPR_I.a;
   	TRS_MPR_I.b     = AD_OUT_MPR_I.b;
   	TRS_MPR_I.c     = AD_OUT_MPR_I.c;
	Transform_3s_2s_2r(&TRS_MPR_I);
	DataFilter(0.4,&TRS_MPR_I.dflt,TRS_MPR_I.d); 			//�����������ֵ�˲��� Ts=200us,fh=1.2kHz,�˵�����Ƶ�ʴ�
	DataFilter(0.4,&TRS_MPR_I.qflt,TRS_MPR_I.q); 			//�����������ֵ�˲��� Ts=200us,fh=1.2kHz,�˵�����Ƶ�ʴ�


//----------------���л��������-------------------------------------------------------------------
	if(M_ChkFlag(SL_MPR_RUNING)!=0)							//���������Ҫ����
	{	

		PI_MPR_Id.reference      = RUN.mpridrf;
		PI_MPR_Id.feedback       = TRS_MPR_I.dflt; 			//����d�������
		PI_Loop(&PI_MPR_Id,PI_PARA_MPRID);   

		PI_MPR_Iq.reference      = RUN.mpriqrf;		  	  
		PI_MPR_Iq.feedback       = TRS_MPR_I.qflt;			//����q�������
  		PI_Loop(&PI_MPR_Iq,PI_PARA_MPRIQ);
    }
   
//--------------MPR�����ѹ��---------------------------------------------------------------------

    DM_imrd = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * CAP4.omigasyn));
//    DM_imrq = - (TRS_NGS_U.qflt * SQRT3 * STAROTRTO / (MPR_Lm * CAP4.omigasyn));  //��ѹq��ǰ��������

	temp_d = - PI_MPR_Id.out + SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Iq.feedback;	//���������

//	temp_d = temp_d	- CAP4.omigaslp * MPR_Lr * DM_imrq;								//��ѹq��ǰ��������

  	temp_q = - PI_MPR_Iq.out - SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Id.feedback;	//���������
	temp_q = temp_q	- CAP4.omigaslp * MPR_Lr * DM_imrd;                               


	temp_d = temp_d - MPR_Rr * PI_MPR_Id.feedback;
	temp_q = temp_q - MPR_Rr * PI_MPR_Iq.feedback;									//ת�ӵ���ѹ��
  
   	TRS_MPR_U.d = temp_d / STAROTRTO2;
	TRS_MPR_U.q = temp_q / STAROTRTO2;
    
    temp1 = (float)_SC_MPR_UD;
	temp2 = (float)_SC_MPR_UQ;

	if     (TRS_MPR_U.d >  temp1)     TRS_MPR_U.d =  temp1;	    //jutsttest
	else if(TRS_MPR_U.d < -temp1)     TRS_MPR_U.d = -temp1;	    //jutsttest
	if     (TRS_MPR_U.q >  temp2)     TRS_MPR_U.q =  temp2;	    //jutsttest
	else if(TRS_MPR_U.q < -temp2)     TRS_MPR_U.q = -temp2;	    //jutsttest

	Anti_Transform_2r_2s(&TRS_MPR_U);							//���귴�任�����ྲֹϵ

//------------SVM�����������巢��-----------------------------------------------------------------	
	SVPWM_MPR(TRS_MPR_U.alfa,TRS_MPR_U.beta);					//���巢��

//------------Teʵ��ת�ط���ֵ����-----------------------------------------------------------------	
//	Te_feedback = 1.5 * POLEPAIRES * MPR_Lm * TRS_NGS_U.dflt * SQRT3 * TRS_MPR_I.qflt / (314.15926 * STAROTRTO * MPR_Ls);   // Te=1.5*np*(Us/w1)*(Irq/K)*(Lm/Ls)
//	Te_feedback = IRQTOTE * TRS_NGS_U.dflt *  PI_MPR_Iq.feedback;   // Te=1.5*np*(Us/w1)*(Irq/K)*(Lm/Ls)
//	DataFilter(0.999,&DIP_MPR_I.qflt,TRS_MPR_I.q); 					//���Ӳ๦�ʼ���,����q������ Ts=200us,fh=Hz
	DataFilter(0.8,&DIP_MPR_I.qflt,TRS_MPR_I.q); 					//20111116 ��Сת���˲�ʱ�䳣�� ��1s��Ϊ4ms
	Te_feedback = - IRQTOTE * DIP_NPR_U.dflt *  DIP_MPR_I.qflt;     // Te=-1.5*np*(Us/w1)*(Irq/K)*(Lm/Ls),MPR_iΪ��ֵ
	if(Te_feedback<=0)	Te_feedback=0;   							//20090817
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
	float temp_d,temp_q;

//-----------�����������������dqֵ-----------------------------------------------------------------
	TRS_NPR_I.a = AD_OUT_NPR_I.a;  							//��ȡ�������������
	TRS_NPR_I.b = AD_OUT_NPR_I.b;
	TRS_NPR_I.c = AD_OUT_NPR_I.c;		
	Transform_3s_2s_2r(&TRS_NPR_I);							//����任
	DataFilter(0.4,&TRS_NPR_I.dflt,TRS_NPR_I.d); 			//�����������ֵ�˲��� Ts=200us,fh=1.2kHz,�˵�����Ƶ�ʴ�
	DataFilter(0.4,&TRS_NPR_I.qflt,TRS_NPR_I.q); 			//�����������ֵ�˲��� Ts=200us,fh=1.2kHz��20090615��

//----------������ѹdqֵ----------------------------------------------------------------------------
	TRS_NGS_U.a = AD_OUT_NGS_U.a;							//��ȡ������ѹ
	TRS_NGS_U.b = AD_OUT_NGS_U.b;
	TRS_NGS_U.c = AD_OUT_NGS_U.c;
	Transform_3s_2s_2r(&TRS_NGS_U);							//����任

	DataFilter(0.4,&TRS_NGS_U.dflt,TRS_NGS_U.d); 			//��ѹ����ֵ�˲���Ts=200us,fh=1.2kHz t=132us 20090608change to ok
	DataFilter(0.4,&TRS_NGS_U.qflt,TRS_NGS_U.q); 			//��ѹ����ֵ�˲���Ts=200us,fh=1.2kHz t=132us 20090608change to ok
//	DataFilter(0.062,&TRS_NGS_U.dflt,TRS_NGS_U.d); 			//��ѹ����ֵ�˲���Ts=200us,fh=12kHz 20100507change at zuoyun
//	DataFilter(0.062,&TRS_NGS_U.qflt,TRS_NGS_U.q); 			//��ѹ����ֵ�˲���Ts=200us,fh=12kHz 20100507change at zuoyun

	DataFilter(0.1,&TRS_NGS_U.dflt2,TRS_NGS_U.d); 			//��ѹ����ֵ�˲���Ts=200us,fh=7.9kHz,126us,Ϊ���������20091026


	if(M_ChkFlag(SL_NPR_RUNING)!=0)							//����PI���п�������
	{

//----------���е�ѹ�⻷----------------------------------------------------------------------------
	  if(M_ChkFlag(SL_STEADYGV)==0)  						//Vdcû���ȶ����Ҳ�����̬PI����
	  {

		PI_NPR_U.reference     = - RUN.urf;   				//��ȡ�м��ѹָ��
		PI_NPR_U.feedback      = - AD_OUT_UDC;				//��ȡ�м��ѹ����ֵ
    	PI_Loop(&PI_NPR_U,PI_PARA_DYNU);
      }	
	  else                                                   //������̬PI����
	  {
		PI_NPR_U.reference     = - RUN.urf;   				//��ȡ�м��ѹָ��
		PI_NPR_U.feedback      = - AD_OUT_UDC;				//��ȡ�м��ѹ����ֵ
    	PI_Loop(&PI_NPR_U,PI_PARA_NPRU); 
      }
    	
//---------����d�������----------------------------------------------------------------------------
   		PI_NPR_Id.reference      = PI_NPR_U.out; 			//��ȡd�������ע��������
		PI_NPR_Id.feedback       = TRS_NPR_I.dflt;			//��ȡd����������˲�ֵ
		PI_Loop(&PI_NPR_Id,PI_PARA_NPRID);

//---------����q�������----------------------------------------------------------------------------
		PI_NPR_Iq.reference      = RUN.npriqrf; 			//q�����ָ��
		PI_NPR_Iq.feedback       = TRS_NPR_I.qflt;	  		//��ȡq����������˲�ֵ
		PI_Loop(&PI_NPR_Iq,PI_PARA_NPRIQ);	  	  
	}

//---------�����תϵ��NPR���Ƶ�ѹ------------------------------------------------------------------

	temp_d = PI_NPR_Id.out  + TRS_NGS_U.dflt - CAP4.omigasyn * NPR_L * PI_NPR_Iq.feedback;
	temp_q = PI_NPR_Iq.out  + TRS_NGS_U.qflt + CAP4.omigasyn * NPR_L * PI_NPR_Id.feedback;
	
  	TRS_NPR_U.d = temp_d;
  	TRS_NPR_U.q = temp_q;

	Anti_Transform_2r_2s(&TRS_NPR_U); 						//antitransform to static axis

//---------SVM�������-----------------------------------------------------------------------------
	SVPWM_NPR(TRS_NPR_U.alfa,TRS_NPR_U.beta);				//���巢��

//------------�����й����ʼ���(3��)-----------------------------------------------------------------	
	DataFilter(0.99,&DIP_NPR_I.dflt,TRS_NPR_I.d); 			//���Ӳ๦�ʼ���,��ѹed�� Ts=200us,fh=88Hz
	DataFilter(0.99,&DIP_NPR_I.qflt,TRS_NPR_I.q); 			//���Ӳ๦�ʼ���,����q������ Ts=200us,fh=88Hz
	DataFilter(0.99,&DIP_NPR_U.qflt,TRS_NGS_U.q); 			//���Ӳ๦�ʼ���,����q������ Ts=200us,fh=88Hz
	DataFilter(0.995,&DIP_NPR_U.dflt,TRS_NGS_U.d); 			//���Ӳ๦�ʼ���,��ѹed�� Ts=200us,Tr=250ms

	Pnactive   = 1.5 * (DIP_NPR_U.dflt * DIP_NPR_I.dflt + DIP_NPR_U.qflt * DIP_NPR_I.qflt);   	// Pnactive=3*Ud*Id/(squt2*squt2)
	Pnreactive = 1.5 * (DIP_NPR_U.qflt * DIP_NPR_I.dflt - DIP_NPR_U.dflt * DIP_NPR_I.qflt);   	// Pnactive=3*Ud*Id/(squt2*squt2)

}


/**************************************************************************************************
** ����: SysCtrl
** ��������: 1.5MW������ϵͳ�߼����Ƴ���--CPC���ʵ�� 	
** �䡡��:   	
** �䡡��: 		 
** ע  ��: 			
**-------------------------------------------------------------------------------------------------
** ������: 	 
** �ա���: 		
**-------------------------------------------------------------------------------------------------
** �޸���:
** ����:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void SysCtrl(void)         
{    

//1-----����PDP�������ز��ɻָ�����-----------------------//     
 if(M_ChkFlag(SL_SERIESTOP)!=0 || M_ChkFlag(SL_SERIESTOPING)!=0)   //ϵͳ�������ع��ϻ�������ͣ�� 20091015 at zy
 {

		M_SetFlag(SL_SERIESTOPING);         //�����ع�������ͣ����־ 20091015 at zy
//		M_ClrFlag(SL_DISPLAY6);          	//��NPR�������ָʾ
//    	M_ClrFlag(SL_DISPLAY7);          	//����������д���ָʾ
		MAIN_LOOP.cnt_cboff=0;           	//if.2
          MAIN_LOOP.cnt_closecb=0;       	//if.3
	  
	   	M_ClrFlag(CL_CB);    		     	//�����Ϻ�բָ��
	   	M_ClrFlag(CL_CBENGSTRG);         	//�����ϴ���ָ�ͬʱǷѹ���Ͽ������϶Ͽ� 
	   	M_ClrFlag(CL_CBFCON);            	//�������Ͽ�

	  	M_ClrFlag(SL_OCS_NPRSTART);         //����������ָ�� 20090815
//	   	M_ClrFlag(SL_STEADYFB);          	//��Vdc�ȶ���־  20091015atzy
	   	M_ClrFlag(SL_OCS_MPRSTART);	   	 	//���������ָ�� 20090815
//      M_ClrFlag(SL_CHARGEOK);          	//��Ԥ������   20091015atzy
	   	M_ClrFlag(SL_CBCLOSED);			 	//�����ϱպ���� 20091022atzy
	   
		 
	   if(M_ChkCounter(MAIN_LOOP.cnt_opencontac,DELAY_OPENCONTAC)>0)	//2s ���϶Ͽ���ʱ���ٶ϶��ӽӴ��������Ӵ���
	   {
		   M_ClrFlag(CL_MAINK);    		  	//�����Ӵ������˲���
		   M_ClrFlag(CL_PRE); 				//��Ԥ���Ӵ���
		   M_ClrFlag(CL_STATORK);    	  	//�϶��ӽӴ���	
		   M_ClrFlag(SL_CHARGEOK);          //��Ԥ������    20091015atzy
	   }

	   if(AMUX.skiiptempmax<50.0)	 M_ClrFlag(SL_FAN_WORK);          	//�رչ���������  20090816

	   if(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(SL_IN1_MIANK)==0 && M_ChkFlag(SL_IN1_STATORK)==0) M_ClrFlag(SL_SERIESTOPING); //20091015 at zy

 }	//end of NO.1 if

  

//2----���յ�SL_OCS_EIN/BIT0=0ʱ���϶��ӽӴ�����ͣ���壬�����Ӵ����˲���������------------------------- 

  else if((M_ChkFlag(SL_OCS_EIN)==0) || (M_ChkFlag(SL_EINSTOPING)!=0))
  {
	   	MAIN_LOOP.cnt_opencontac=0;        			//if.1
         MAIN_LOOP.cnt_closecb=0;         			//if.3

		M_SetFlag(SL_EINSTOPING);    				//20090817
		M_ClrFlag(SL_OCS_SYSRUN); 					//��ϵͳ���кͷ�����ָ��ȴ�����ֹͣ��MC�Ͽ�
    	M_ClrFlag(SL_OCS_NPRSTART);
		M_ClrFlag(SL_OCS_MPRSTART);
  
		if((M_ChkFlag(SL_NPR_PWMOUT)==0)&&(M_ChkFlag(SL_MPR_PWMOUT)==0)&&(M_ChkFlag(SL_CHARGEOK)==0))
		{
	  		if(M_ChkFlag(SL_IN1_CBSTS)!=0)    		//���Ӵ���\���˲����Ѿ��Ͽ� ������·���պ�״̬
	  		{
	    		if(M_ChkCounter(MAIN_LOOP.cnt_cboff,DELAY_CBOFF)>0)  //1s
				{
			  		M_ClrFlag(CL_CB);    		    //�����Ϻ�բָ��
			  		M_ClrFlag(CL_CBENGSTRG);      	//�����ϴ���ָ�ͬʱǷѹ���Ͽ������϶Ͽ�
			 		M_ClrFlag(CL_CBFCON);
				}	     
	 
	  		}
	  		else   
	  		{
//	        	M_ClrFlag(SL_DISPLAY6);           	//��NPR�������ָʾ
//				M_ClrFlag(SL_DISPLAY7);           	//����������д���ָʾ
				M_ClrFlag(CL_CB);    		      	//�����Ϻ�բָ��
		    	M_ClrFlag(CL_CBENGSTRG);          	//�����ϴ���ָ��
		    	M_ClrFlag(CL_CBFCON);	
		    	M_ClrFlag(CL_PRE);                	//��Ԥ��Ӵ���
		    	M_ClrFlag(SL_CHARGEOK);           	//��Ԥ������
            	M_ClrFlag(SL_OCS_NPRSTART);        	//����λ��NPR������ָ��
	        	M_ClrFlag(SL_OCS_MPRSTART);        	//����λ��MPR������ָ��
				M_ClrFlag(CL_MAINK);    		  	//�����Ӵ������˲���
				M_ClrFlag(SL_CBCLOSED);				//�����ϱպ���� 
				if(AMUX.skiiptempmax<50.0)	 M_ClrFlag(SL_FAN_WORK);     //�رչ��������� 20090816
				M_ClrFlag(SL_EINSTOPING);    		//20090817    		
	  		}
		}
		else    MAIN_LOOP.cnt_cboff=0;
	 	
   } //end of NO.2 if


//3----------- ���յ�SL_OCS_EIN/BIT0=1��ϵͳ����ָ������ϣ�Ԥ��磬�����Ӵ���/�˲�����������

  else                                         		//ϵͳ�޹��ϣ����յ�BIT0=1��ִ��ϵͳ����ָ��ֱ�����ϱպ� 
  {
      	MAIN_LOOP.cnt_opencontac=0;        			//if.1
	   	 MAIN_LOOP.cnt_cboff=0;            			//if.2
   		if(M_ChkFlag(SL_CBCLOSED)==0) 				//���ϻ�û�бպϣ�ִ�����ϱպϳ���
   		{
      		if(M_ChkFlag(SL_IN1_CBSTS)==0)      	//����·���Ͽ�
	  		{
	     	 	M_SetFlag(CL_CBENGSTRG);     		//���յ��պ�CB��ʱ��,��ʼ����
			 	M_SetFlag(CL_CBFCON);

			 	if(M_ChkCounter(MAIN_LOOP.cnt_closecb,DELAY_CLOSE_CB)>0)  M_SetFlag(CL_CB);   //����CB��բָ��(���弴��) 5s
	  		}      
      		else	 
      		{
      			M_SetFlag(SL_CBCLOSED);             //ϵͳ���Ϻ�բ���
				M_ClrFlag(CL_CB);  					//���ϱպϺ󣬾����CB��բ(��Ϊ��բ����Ҫһ�����弴��)
      		}

   		}
   		else  MAIN_LOOP.cnt_closecb=0;             	//����·���պ��Ѿ����
  
  }	 
	  

//4----------���ϱպϺ󣬽��յ����ر���������ָ��---------------------------------------------------------------------------------

  if(M_ChkFlag(SL_CBCLOSED)!=0)   
  {           
  	if((M_ChkFlag(SL_OCS_SYSRUN)!=0)&&(M_ChkFlag(SL_SERIESTOP)==0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SYSSTOPING)==0))
	{
      MAIN_LOOP.cnt_mkoff=0;
      if(M_ChkFlag(SL_CHARGEOK)==0)				//δ����Ԥ���
	  {      	
      	
      	if(M_ChkFlag(SL_SPEED_IN_RANGE)!=0)     //ת���ڷ�Χ��
		{
//	        M_ClrFlag(SL_DISPLAY7);    			//����������д���ָʾ     			
			M_SetFlag(SL_FAN_WORK);       		//��������������  20090816

      		if((M_ChkFlag(SL_IN1_MIANK)==0)||(M_ChkFlag(SL_IN1_MIANFILTER)==0))        //����·���ѱպ�,�����Ӵ���/�˲�����һ�жϿ�
	  		{	       	     		 		              
	       		M_SetFlag(CL_PRE);          	 //�պ�Ԥ���Ӵ���
		   		if((AD_OUT_UDC>=_SC_VDCON)&&(M_ChkCounter(MAIN_LOOP.cnt_precok,DELAY_PRECOK)>=0))  M_SetFlag(CL_MAINK);  //8s      //Ԥ���OK���պ����Ӵ��� 9s   
	  		} 
	  		else
	  		{
		 		M_ClrFlag(CL_PRE);          	      //�Ͽ�Ԥ���Ӵ�����Ӳ���ϻ���MF/MC�պϺ�ʹ��Ͽ��������ҲҪ����λ
		 		M_SetFlag(SL_CHARGEOK);               //ϵͳ׼������         
      		}
		}
		else	MAIN_LOOP.cnt_precok=0;
	  }
	  else 
	  {
	 		if(AD_OUT_UDC >= 870.0)   M_SetFlag(SL_OCS_NPRSTART);	 //ϵͳ�ϵ���ɣ���ʱ������NPR 1s   
		 	MAIN_LOOP.cnt_precok=0;
	  }
	}
	else   //SYSRUNͣ������
	{
   		M_SetFlag(SL_SYSSTOPING);  //20090817
 
  		M_ClrFlag(SL_OCS_NPRSTART);
		M_ClrFlag(SL_OCS_MPRSTART);
  		if((M_ChkFlag(SL_NPR_PWMOUT)==0)&&(M_ChkFlag(SL_MPR_PWMOUT)==0))
		{
  	  		if((M_ChkFlag(SL_IN1_MIANK)!=0)||(M_ChkFlag(SL_IN1_MIANFILTER)!=0))     //���Ӵ����պ�״̬
	 		{
         		if(M_ChkCounter(MAIN_LOOP.cnt_mkoff,DELAY_MKOFF)>0)                 //0.2s
	     		{
			  		M_ClrFlag(CL_MAINK);    		  								// �����Ӵ������˲���
	          		M_ClrFlag(CL_PRE);    		  									// ����һ��Ԥ���ָ��	
		 		}	        	    		  	     
	  		}	
	  		else    
	  		{
	       		M_ClrFlag(SL_CHARGEOK);
				M_ClrFlag(SL_SYSSTOPING);  //20090817
				if(AMUX.skiiptempmax<50.0)	 M_ClrFlag(SL_FAN_WORK);     			//�رչ��������� 20090816
//				if(M_ChkFlag(SL_ERRSTOP)==0) M_SetFlag(SL_DISPLAY7);    			//�ñ��������д���ָʾ
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
//		M_ClrFlag(SL_DISPLAY7);    					//����������д���ָʾ 
  }

//5----------- ���Ӵ���/�˲����ѱպ����޹���,�ȴ���λ����������ָ������	  
	  
	if((M_ChkFlag(SL_CBCLOSED)!=0)&&(M_ChkFlag(SL_CHARGEOK)!=0)&&(M_ChkFlag(SL_SERIESTOP)==0)&&(M_ChkFlag(SL_ERRSTOP)==0))   
	{
	   if(M_ChkFlag(SL_OCS_NPRSTART)!=0)
	   {	   
//	       M_ClrFlag(SL_DISPLAY6);            									//��NPR�������ָʾ
         		 
           if(M_ChkFlag(SL_CONFIGPWM)!=0)    M_SetFlag(SL_NPR_START);		   	//pwmdrive���ÿ���Ƶ�����,�������������		  
	       else   	                         M_ClrFlag(SL_NPR_START);		   	//�ֹ������������� 

           //---------�ж�Vdc�Ƿ��ȶ�start	          
	       if((AD_OUT_UDC>(_URF - 20))&&(AD_OUT_UDC<(_URF + 20)))   			//�ж�ֱ����ѹ�Ƿ��ȶ���_URF(1100V)�����ѭ�˳���һ��ʱ�䣬��ʹ�ܻ�������
	   	   {   
	   		  M_SetFlag(SL_DISPLAY2);                							//��Vdc�ȶ�ָʾ
	   		  if(M_ChkFlag(SL_MPR_START)==0)         							//ֻ�ڻ���δ������ǰ��⣬���������ڼ�Vdc���Ȳ�����SL_STEADYFB  
	   		  {                                                					//����ƫ��С��20V
	     	    if(M_ChkCounter(MAIN_LOOP.cnt_steadyfb,DELAY_STEADYFB)>0)	  M_SetFlag(SL_STEADYFB);  //2s
		      }
		      else   MAIN_LOOP.cnt_steadyfb=0;
		   }
		   else  
		   {
		   	 MAIN_LOOP.cnt_steadyfb=0;
			 M_ClrFlag(SL_DISPLAY2);             								//��Vdc�ȶ�ָʾ

		   }	        
	       //---------�ж�Vdc�Ƿ��ȶ�end
	     	    
		   if(M_ChkFlag(SL_STEADYFB)!=0)
		   {
		      if(M_ChkCounter(MAIN_LOOP.cnt_mprstart,DELAY_MPRSTART)>=0)   M_SetFlag(SL_OCS_MPRSTART);	 //Vdc�Ѿ��ȶ�����ʱ������MPR 1s  
           }
		   else  MAIN_LOOP.cnt_mprstart=0;

	   }
	   else					  													
	   {
	      if(M_ChkFlag(SL_MPR_PWMOUT)==0)   M_ClrFlag(SL_NPR_START);  			//������������Ժ��ٽ��������������ָ������

          if(M_ChkFlag(SL_NPR_PWMOUT)==0)      
          {
             M_ClrFlag(SL_STEADYFB);
//             M_SetFlag(SL_DISPLAY6);                  							//�����������, ��NPR�������ָʾ
			 M_ClrFlag(SL_DISPLAY2);
		  }
		  MAIN_LOOP.cnt_steadyfb=0;	   
		  MAIN_LOOP.cnt_mprstart=0; 
	   }

    }	   
	else                  
    {   
//	    M_ClrFlag(SL_DISPLAY6);            //��NPR�������ָʾ
		M_ClrFlag(SL_NPR_START);
		M_ClrFlag(SL_OCS_NPRSTART);	
		M_ClrFlag(SL_STEADYFB);
		M_ClrFlag(SL_DISPLAY2);
		MAIN_LOOP.cnt_steadyfb=0;
		MAIN_LOOP.cnt_mprstart=0;
	}	


//6----------- ���Ӵ���/�˲����ѱպ����޹���,�ȴ���λ����������ָ�������	  
	  
	if((M_ChkFlag(SL_CBCLOSED)!=0)&&(M_ChkFlag(SL_CHARGEOK)!=0)&&(M_ChkFlag(SL_SERIESTOP)==0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_NPR_PWMOUT)!=0))   
	{
      if((M_ChkFlag(SL_OCS_MPRSTART)!=0)&&(M_ChkFlag(SL_STEADYFB)!=0))		  								//��������,��Vdc�ȶ�
	  {
		M_SetFlag(SL_MPR_START);             								//�û������������ָ��

		if(M_ChkFlag(SL_IN1_STATORK)==0)      								//���ӽӴ������ڶϿ�״̬
	 	{
		   if((MEAN_DATA.uab_d <= _SC_UDSTAC)&&(MEAN_DATA.ubc_d <= _SC_UDSTAC))          //���ӽӴ���ǰ��벨ƽ��ֵ����40V����
		   {
		      M_SetFlag(SL_DISPLAY4);
		      if(_AUTOCSTAC !=0)    M_SetFlag(CL_STATORK);                               //�ⲿ���ƣ�Ϊ1������պ�STAC
			  else					M_ClrFlag(CL_STATORK);		        
		   }
		   else   M_ClrFlag(SL_DISPLAY4);      
	 	}
		else	M_SetFlag(SL_MPR_SYNOK);  									//����ͬ��������ɱ�־	
	  }
	  else  
	  {
		 M_ClrFlag(SL_MPR_START);
		 M_ClrFlag(SL_DISPLAY4);											// ͣ�������������ָ��

		 if(M_ChkFlag(SL_IN1_STATORK)!=0)      								//
	 	 {
         	if(M_ChkFlag(SL_STACTRIPEN)!=0)	  M_ClrFlag(CL_STATORK);        //�ﵽ���ӶϿ�����, �Ͽ����ӽӴ���		            
	 	 }
		 else      M_ClrFlag(SL_MPR_SYNOK);
		
	  }
	}
	else
	{
	   M_ClrFlag(SL_MPR_START);                								//ֹͣ�������������ָ��
	   M_ClrFlag(SL_OCS_MPRSTART);	
	   if(M_ChkFlag(SL_SERIESTOP)==0)	M_ClrFlag(CL_STATORK); 
	   M_ClrFlag(SL_MPR_SYNOK);
	   M_ClrFlag(SL_DISPLAY4);
	}    


//======================ϵͳ���ع��Ϸ������������Ժ��ⲿ������λ�ź�,ֻ����ϵͳͣ����λ=================//
//======================һ����Ϸ������������Ժ��ⲿ������λ�ź�,������ͻ��඼ͣ����󼴿ɸ�λ=================//
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

//====================���Ϸ������������Ժ󣬲�����������λ�ź�---����==============// 
}

/////////////////no more////////////////////
