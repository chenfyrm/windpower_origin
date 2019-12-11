   /****************************************Copyright (c)**************************************************
**                       		     北	京	交	通	大	学
**                                        电气工程学院
**                                         604实验室
** 
**                              
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: user_work.c
**创   建   人: 
**最后修改日期: 
**描        述: 右玉1.5MW双馈风电机组并网变流器控制软件核心控制程序----201005atcpc-201007atbjtu
**              
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 
** 版  本: 
** 日　期: 
** 描　述: 
**
**--------------当前版本修订------------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "DSP2833x_Device.h"     // Headerfile Include File
#include "DSP2833x_Examples.h"   // Examples Include File
#include "math.h"

//函数声明systest201005atcpc
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
** 函数名称: InitWork
** 功能描述: 初始化WORK部分，包括GIVE，RunCtrl，WAVE，SCOUT
** 输　入: 
** 输　出:   
** 注  释: 	 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void InitWork(void)
{
	M_SetFlag(SL_POWERON);		//置初次上电标志
	_MSG_SCOUT1=MSG_NONE;		//故障信息清除
	_MSG_SCOUT2=MSG_NONE;		//故障信息清除
	MAIN_LOOP.pulse = 0;		//分时脉冲赋初值
	M_ClrFlag(SL_OCS_NPRSTART);
    M_ClrFlag(SL_OCS_MPRSTART);
	M_ACROWBAROFF();			//ACTIVECROWBAR关断
	if(GpioDataRegs.GPBDAT.bit.GPIO50==0)	DEBUG_OPER=1;  //mode0使能，变量初始化初值
	else									DEBUG_OPER=0;  //20100401at27

}


/*********************************************************************************************************
** 函数名称: PwmDrive
** 功能描述: 
** 输　入: 
** 输　出:        
** 注  释: 	 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void PwmDrive(void)
{
    if((M_ChkFlag(SL_POWERON)==0)&&(M_ChkFlag(SL_CHARGEOK)!=0))		//DSP不是正在上电，已上完电,系统预充电完成
 	{ 	
		    //--下面执行的是开关频率配置，在停机状态下允许操作器设置开关频率，每次启动时更新一次开关频率----
			if(M_ChkFlag(SL_OCS_NPRSTART)!=0)      					//主控要求变流器启动
			{
			   if(M_ChkFlag(SL_CONFIGPWM)==0 && M_ChkFlag(SL_NPR_PWMOUT)==0 && M_ChkFlag(SL_MPR_PWMOUT)==0)  //只有停脉冲了才允许再配置PWM						//没有配置开关频率
		       {								  
				   ConfigPwm();	                					//配置开关频率				
				   if(M_ChkCounter(MAIN_LOOP.cnt_pwmout,DELAY_PWMOUT)>=0)	M_SetFlag(SL_CONFIGPWM);  //脉冲发生允许延时到	20091015 at zy						//脉冲允许,保证EnPwm()只能运行一次       
		       }
			   else	 MAIN_LOOP.cnt_pwmout=0;						//清除系统工作允许延时	
			}
			else   
			{
			   M_ClrFlag(SL_CONFIGPWM);
			   MAIN_LOOP.cnt_pwmout=0;								//清除系统工作允许延时
			}

//----------------下面是真正的脉冲使能最后一关----------------
	        
	 //--------网侧变流器脉冲允许,且没有故障--------------
//	        if((M_ChkFlag(SL_NPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0)) 
	        if((M_ChkFlag(SL_NPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0)&&(M_ChkFlag(SL_NSTOP)==0)) //201007BJTULVRT
	        {
		       if(M_ChkFlag(SL_NPR_PWMOUT)==0)  					//保证Enepwmio_NPR()只能运行一次
		       {
			      Enepwmio_NPR();
			          	
				  M_SetFlag(SL_RUN);
			      M_SetFlag(SL_NPR_PWMOUT);			        
		       }
	        }
			else       												//发生故障就立即封脉冲
	        {
		       Disepwmio_NPR(); 
		              
			   M_ClrFlag(SL_NPR_PWMOUT);
			   M_ClrFlag(SL_RUN);
	        }
     //------机侧变流器脉冲使能,且没有故障---------------
//            if((M_ChkFlag(SL_MPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0))
            if((M_ChkFlag(SL_MPR_RUNING)!=0)&&(M_ChkFlag(SL_ERRSTOP)==0)&&(M_ChkFlag(SL_SERIESTOP)==0)&&(M_ChkFlag(SL_MSTOP)==0)) //201007BJTULVRT
	        {
		       if(M_ChkFlag(SL_MPR_PWMOUT)==0)	//保证Enepwmio_MPR()只能运行一次
		       {
			      Enepwmio_MPR();      
			      M_SetFlag(SL_MPR_PWMOUT);
			      M_SetFlag(SL_HVLV_MPWMOUT);		// LVRT	         
		       }
			   MAIN_LOOP.cnt_lv_mpwmstop=0;
	        }
	        else                  //发生故障就立即封脉冲
	        {		       
			   Disepwmio_MPR();       
			   M_ClrFlag(SL_MPR_PWMOUT); 
			   if(M_ChkCounter(MAIN_LOOP.cnt_lv_mpwmstop,DELAY_MPWMSTOP)>0)	//0ms LVRT 20100125cpc试验确定
			   		M_ClrFlag(SL_HVLV_MPWMOUT);	      						//脉冲封掉后延时0ms允许开通SCR
	        } 
	}
    else   //系统正在上电,或预充电闸没有切除
	{      
		DisPwm();
		M_ClrFlag(SL_RUN);											//2010
		M_ClrFlag(SL_NPR_PWMOUT);
		M_ClrFlag(SL_MPR_PWMOUT);
		M_ClrFlag(SL_HVLV_MPWMOUT);	      							//脉冲封掉后延时1ms允许开通SCR
		M_ClrFlag(SL_CONFIGPWM);			        				//脉冲封锁
		MAIN_LOOP.cnt_pwmout=0;										//清除系统工作允许延时
		MAIN_LOOP.cnt_lv_mpwmstop=0;
	}
} 
//****************************************************************************
//编号：
//名称：SVPWM
//功能：脉冲调制输出
//输入：

//输出：
//注释：
//修改说明:
//****************************************************************************
//------------------网侧变流器SVPWM调制------------------------------------//	
void SVPWM_NPR(float alfa, float beta)
{      
  float tempmin,tempmax,temp1,temp2,temp3;
  float t0,t1,t2;    						 					//空间基本矢量的作用时间
  Uint16 sector;      											//扇区

  vdc = AD_OUT_UDC;  											//vdc为实际直流电压值,cdc取实际直流电压值用于调制
//-----------------------SVPWM 扇区判断和矢量作用时间计算------------------------------------------
  if(beta>=0)
  {
	   if(SQRT3 * alfa >= beta) 
	   {
	      sector=1;
	      t1=SQRT3 * (alfa * SQRT3_2-beta * 0.5)/vdc;   		//SQRT3=sqrt(3)=1.73205081，在宏定义里面实现,0.8660254=SQRT3/2
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

//--------------------过调制处理------------------------------------------------------------------ 
  	if(t1+t2>=1.0)           									//此时Ts=1，故判断时以1作为比较
  	{
	  t1=t1/(t1+t2);
	  t2=1.0-t1;
  	} 

  	  t0 = 1.0 - t1 - t2;	
  	  t0 = t0 * 0.5;											//零矢量作用时间
//------------------------ 比较寄存器值计算---------------------------------------------------------
  switch(sector)
    {
      	case 1:    temp1=(Uint16) (t0 * SW_NPR);				//SW_NPR为PWM周期寄存器?,为开关周期的一半
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

   tempmin = 0.5 * 75 * (_MINONTIME + _DEADTIME);   //_MINONTIME对应最小脉宽的时间，单位us
                                           	  		//_DEADTIME对应死区的时间，单位us
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
//编号：
//名称：SVPWM_MPR
//功能：脉冲调制输出
//输入：

//输出：
//注释： 机侧变流器SVPWM调制
//修改说明:
//****************************************************************************
void SVPWM_MPR( float alfa, float beta)
{
  
   float tempmin,tempmax,temp1,temp2,temp3;
   float t0,t1,t2;
   Uint16 sector;

   vdc = AD_OUT_UDC;
//-----------------------SVPWM 扇区判断和矢量作用时间计算-------------------------------------------	       
   if(beta>=0)
	{
	   if(SQRT3*alfa >= beta) 
	   	{
	      sector=1;
	      t1=SQRT3 * (alfa * SQRT3_2-beta * 0.5)/vdc;   		//SQRT3=sqrt(3)=1.73205081，在宏定义里面实现,0.8660254=SQRT3/2
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
  t0 = t0 * 0.5;	    										//零矢量作用时间
//------------------------ 比较寄存器值计算---------------------------------------------------------
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

   tempmin = 0.5 * 75 * (_MINONTIME + _DEADTIME);   //_MINONTIME对应最小脉宽的时间，单位us
                                           	  		//_DEADTIME对应死区的时间，单位us
   tempmax = SW_MPR - tempmin;  

   if	  (temp1<tempmin) 	temp1=tempmin;
   else if(temp1>tempmax)	temp1=tempmax;
   if	  (temp2<tempmin) 	temp2=tempmin;
   else if(temp2>tempmax)	temp2=tempmax;
   if	  (temp3<tempmin) 	temp3=tempmin;
   else if(temp3>tempmax)	temp3=tempmax;

   if(R_PHORDE==1)		//转子反相序控制201005atcpc
   {
 	  EPwm4Regs.CMPA.half.CMPA = temp2;	//转子反相序控制
 	  EPwm5Regs.CMPA.half.CMPA = temp1;
 	  EPwm6Regs.CMPA.half.CMPA = temp3; 
   }
   else
   {
	  EPwm4Regs.CMPA.half.CMPA = temp1;	//转子正相序控制
	  EPwm5Regs.CMPA.half.CMPA = temp2;
 	  EPwm6Regs.CMPA.half.CMPA = temp3;    
   }


}  

/*********************************************************************************************************
** 函数名称: 坐标变换子函数
** 功能描述: 3s/2s变换，2s/2r变换
** 输　入: 
** 输　出:   
** 注  释: 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日期:20090331
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Transform_3s_2s_2r(struct TRANS_DATA *var1)
{   
    var1->alfa = var1->a;												//采用等幅变换 ualfa=2/3(ua-0.5ub-0.5uc)   
	var1->beta = (var1->b - var1->c) * SQRT3_3;							//ubeta=2/3*sqrt(3)/2 (ub-uc)  SQRT3_3=sqrt(3)/3=0.57735026918962576450914878050196

    var1->d =  var1->alfa * var1->costheta + var1->beta * var1->sintheta;//ud=ualfa*cos(th)+ubeta*sin(th)
    var1->q = -var1->alfa * var1->sintheta + var1->beta * var1->costheta;//uq=-ualfa*sin(th)+ubeta*cos(th)
}  
/*********************************************************************************************************
** 函数名称: 坐标变换子函数
** 功能描述: 2r/2s变换，2s/3s变换
** 输　入: 
** 输　出: 
** 注  释: 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日期:20090331
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Anti_Transform_2r_2s(struct TRANS_DATA *var2)
{
    var2->alfa = var2->d * var2->costheta - var2->q * var2->sintheta;//ualfa=ud*cos(th)-uq*sin(th)   
	var2->beta = var2->d * var2->sintheta + var2->q * var2->costheta;//*ubeta=ud*sin(th)+uq*cos(th)     			
}

/*********************************************************************************************************
** 函数名称: PI_Loop
** 功能描述: PI调?
** 输　入:   kp 比例放大系数
             ki 积分系数
			 outmax 输出限幅
			 errmax 误差最大值限幅
             errmin 误差最小值限幅
             incrementmax 输出增量限幅

** 输　出:   
** 注  释: 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日期:20090331
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void PI_Loop(struct PI_DATA *var,struct PI_PARA PI_var)
{
	float temp,ptemp,itemp,detemp,outtemp,du;

//----------------------计算本次误差---------------------------------------------------------------
	temp = var->reference - var->feedback;  			

//----------------------次误差正最大值限幅、最小值限幅-------------------------------------------
    if		(temp  	 	>  PI_var.errmax)  temp =  PI_var.errmax; 
    else if	(temp  	 	< -PI_var.errmax)  temp = -PI_var.errmax; 
	else 	 temp = temp;

	var->error = temp;													//修正后误差，抗干扰
    detemp = var->error -  var->errorp;    

//----------------------本次增量、限幅-------------------------------------------------------------
	ptemp = PI_var.kp *  detemp;										//比例项
	itemp = PI_var.kp * PI_var.ki *  var->error * 2.0e-4;				//积分项增量,09.6.17

	du = ptemp + itemp;                                             	//本次增量
	if     (du >  PI_var.incrementmax)    du =  PI_var.incrementmax;	//本次增量限幅
	else if(du < -PI_var.incrementmax)    du = -PI_var.incrementmax;	//本次增量限幅

//----------------------本次输出、限幅------------------------------------------------------------
	outtemp = var->out + du;

	if     (outtemp >  PI_var.outmax)  outtemp =  PI_var.outmax;		//输出限幅
	else if(outtemp < -PI_var.outmax)  outtemp = -PI_var.outmax;		//输出限幅

//---------------------输出赋值、刷新上次误差值--------------------------------------------------	
	var->out    = outtemp;		
	var->errorp = var->error;												
} 

//--------20121103--------
/*********************************************************************************************************
** 函数名: PLLCtrl
** 功能描述: 计数控制
** 输　入:
** 输　出       
** 注  释: 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
#pragma CODE_SECTION(PLLCtrl, "ramfuncs");	
void PLLCtrl(void)
{  
	float n,p;
 
//    I_Loop(&PI_DATA_PLL);		//201205LVRTatZB
//    pll_theta=I_DATA_PLL.last;
 
    TRS_GAC_U.a = AD_OUT_NGS_U.a;					//获取电网电压
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

	PTransform_3s_2s_2r(&TRS_GAC_U);					//坐标变换
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
	NGS_Udq_n2p = 100 * NGS_Udq_n / NGS_Udq_p;
	NGS_Udq_n2pex = 100 * NGS_Udq_n / NGS_Udq_p_ex;

} 

/*********************************************************************************************************
** 函数名称: PLL坐标变换子函数
** 功能描述: 3s/2s变换，2s/2r变换
** 输　入: 
** 输　出:   
** 注  释: 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日期:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void PTransform_3s_2s_2r(struct TRANS_DATA *var2)
{   
    var2->palfa = var2->a;													//采用等幅变换 ualfa=2/3(ua-0.5ub-0.5uc)   
	var2->pbeta = (var2->b - var2->c) * SQRT3_3;							//ubeta=2/3*sqrt(3)/2 (ub-uc)  SQRT3_3=sqrt(3)/3=0.57735026918962576450914878050196

    var2->pd =  var2->palfa * var2->costheta + var2->pbeta * var2->sintheta;//ud=ualfa*cos(th)+ubeta*sin(th)
    var2->pq = -var2->palfa * var2->sintheta + var2->pbeta * var2->costheta;//uq=-ualfa*sin(th)+ubeta*cos(th)
}  
/*********************************************************************************************************
** 函数名称: PLL坐标变换子函数
** 功能描述: 3s/2s变换，2s/2r变换
** 输　入: 
** 输　出:   
** 注  释: 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日期:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void NTransform_3s_2s_2r(struct TRANS_DATA *var3)
{   
    var3->nalfa = var3->a;													//采用等幅变换 ualfa=2/3(ua-0.5ub-0.5uc)   
	var3->nbeta = (var3->b - var3->c) * SQRT3_3;							//ubeta=2/3*sqrt(3)/2 (ub-uc)  SQRT3_3=sqrt(3)/3=0.57735026918962576450914878050196

    var3->nd =  var3->nalfa * var3->costheta - var3->nbeta * var3->sintheta;//ud=ualfa*cos(th)+ubeta*sin(th)
    var3->nq = var3->nalfa * var3->sintheta + var3->nbeta * var3->costheta;//uq=-ualfa*sin(th)+ubeta*cos(th)
} 
/*********************************************************************************************************
** 函数名称:PLL正负序解耦函数
** 功能描述: 
** 输　入: 
** 输　出:   
** 注  释: 
**-------------------------------------------------------------------------------------------------------
** : 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日期:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void De_ONE_P_2r(struct TRANS_DATA *var4)
{
  var4->de_pd=var4->pd - var4->de_fltnd*var4->de_pcostheta-var4->de_fltnq*var4->de_psintheta;
  var4->de_pq=var4->pq +var4->de_fltnd*var4->de_psintheta- var4->de_fltnq*var4->de_pcostheta;
}

/*********************************************************************************************************
** 函数名称:PLL正负序解耦函数
** 功能描述: 
** 输　入: 
** 输　出:   
** 注  释: 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日期:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void De_ONE_N_2r(struct TRANS_DATA *var5)
{
  var5->de_nd=var5->nd - var5->de_fltpd*var5->de_ncostheta - var5->de_fltpq*var5->de_nsintheta;
  var5->de_nq=var5->nq + var5->de_fltpd*var5->de_nsintheta - var5->de_fltpq*var5->de_ncostheta;
}

/*********************************************************************************************************
** 函数名称: PLL-PI_Loop
** 功能描述: PI调节器
** 输　入:   kp 比例放大系数
             ki 积分系数
			 outmax 输出限幅
			 errmax 误差最大值限幅
             errmin 误差最小值限幅
             incrementmax 输出增量限幅

** 输　出:   
** 注  释: 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日期:201011PLL
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void PLL_PI_Loop(struct PI_DATA *var,struct PI_PARA PI_var,struct TRANS_DATA *var1)
{
	float ptemp,itemp,detemp,outtemp,du;
													
    detemp = var1->de_pq - var->last;    

//----------------------本次增量、限幅-------------------------------------------------------------
	ptemp = PI_var.kp * detemp  ;										//比例项
	itemp = PI_var.kp * PI_var.ki * var1->de_pq * 2.0e-4;				//积分项增量,09.6.17

	du = ptemp + itemp;                                             	//本次增量
	if     (du >  PI_var.incrementmax)    du =  PI_var.incrementmax;	//本次增量限幅
	else if(du < -PI_var.incrementmax)    du = -PI_var.incrementmax;	//本次增量限幅

//----------------------本次输觥⑾薹?-----------------------------------------------------------
	outtemp = var->out + du;

	if     (outtemp >  PI_var.outmax)  outtemp =  PI_var.outmax;		//输出限幅
	else if(outtemp < -PI_var.outmax)  outtemp = -PI_var.outmax;		//输出限幅

//---------------------输出赋值、刷新上次误差值--------------------------------------------------	
	var->out    = outtemp;		
	var->last = var1->de_pq;												
} 
/*********************************************************************************************************
** 函数名称: I_Loop
** 功能描述: Integrator调节器
** 输　入:   
** 输　出:   
** 注  释: 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日期:201011PLL
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
** 函数名称: Magnet_Curve
** 功能描述: 电机励磁曲线函数(分段线性插值)
** 输　入:   插值自变量x[n],插值函数值y[n],计算函数点in
** 输　出:   励磁电流校正系数out  
** 注  释: 	 
**-------------------------------------------------------------------------------------------------------
** 作　者: MagnetCurve2013-12-13
** 日　期: 2013-12-13
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Magnet_Curve(struct MC_DATA *var)	//分段线性插值
{
	float temp_m,temp_k;
	Uint16 i,j,s;

//---------------插值点限幅------------------------------------------------    
	if(var->in < var->x[0])	var->in= var->x[0];
	else if(var->in > var->x[10])	var->in= var->x[10];

//---------------判断插值点区间[0,4]------------------------------------------------    
	if(var->in <= var->x[1])	s=0;
	else if((var->in > var->x[1])&&(var->in <= var->x[2]))	s=1;
	else if((var->in > var->x[2])&&(var->in <= var->x[3]))	s=2;
	else if((var->in > var->x[3])&&(var->in <= var->x[4]))	s=3;
	else if((var->in > var->x[4])&&(var->in <= var->x[5]))	s=4;
	else if((var->in > var->x[5])&&(var->in <= var->x[6]))	s=5;
	else if((var->in > var->x[6])&&(var->in <= var->x[7]))	s=6;
	else if((var->in > var->x[7])&&(var->in <= var->x[8]))	s=7;
	else if((var->in > var->x[8])&&(var->in <= var->x[9]))	s=8;
	else if((var->in > var->x[9])&&(var->in <= var->x[10]))	s=9;

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
** 函数名称: Give_Integral
** 功能描述: 给定积分子函数
** 输　入:   积分步长step，给定指令值give
** 输　出:   给定积分后的指令值out  
** 注  释: 	 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 20090604
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:20090604
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
** 函数名称: Give
** 功能描述: 确定能否工作 cpc修改
** 输　入:   
** 输　出:   
** 注  释: 	 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　? 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:20090801修改
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Give(void)
{	
	float temp_toqrf,temp_aglrf;
    
	if(M_ChkFlag(SL_NPR_START)==0 && M_ChkFlag(SL_MPR_START)==0)
	{
		M_ClrFlag(SL_NPR_RUN);			    						//网侧工作允许标志清0
		M_ClrFlag(SL_MPR_RUN);			    						//机工作曛厩?0
		GIVE.urf      = _URF;										//网侧给定电压为当前Vdc值
		GIVE.npriqrf  = 0;											//网侧给定电流为0
        GIVE.toqrf    = 0;                  						//机侧给定转矩为0
        GIVE.anglerf  = 0;		            						//机侧给定功率因数角度为0
		GIVE.toqrfflt = 0;		//20121103
	}
	else
	{
//		if(M_ChkFlag(SL_LV_QWORKING)==0) 	NGS_Udq_p_ex = NGS_Udq_p;		//判断LVRT之前网压 20130206
		if(M_ChkFlag(SL_LV_QWORKING)==0||M_ChkFlag(SL_HV_QWORKING)==0) 	NGS_Udq_p_ex = NGS_Udq_pflt;		//判断LVRT之前网压 20130301

/*		if(M_ChkFlag(SL_OCS_NPREIN)!=0)					           //网侧无功并网,网侧给定直流电压和无功电流值
		{
		    M_ClrFlag(SL_MPR_RUN);

			if(M_ChkFlag(SL_NPR_START)!=0)  
			{
				M_SetFlag(SL_NPR_RUN); 								//网侧变流器运行
		    	GIVE.urf      = (int16) _URF;                       //给网侧中间直流电压指令赋值
		    	GIVE.npriqrf  = (int16) _NIQRF;						//给机侧无功电流指令赋值
			}
	        else  M_ClrFlag(SL_NPR_RUN);
		}	
			
		else 		                       							//背靠背工作时网侧直流电压,无功电流给定和机侧转矩和功率因数角给定
		{		
*/			if(M_ChkFlag(SL_NPR_START)!=0)  		   				//网侧运行判断
			{
			   M_SetFlag(SL_NPR_RUN);                  				//网侧脉冲允许		       
		       GIVE.urf       = (int16) _URF;          				//给中间直流电压指令赋值
			   GIVE.npriqrf   = (int16) _NIQRF;        				//网侧给定感性无功保证并网功率因数为1
//			   GIVE.npriqrf   = 70.0;                  				//网侧给定感性无功保证并网功率因数为1，互馈实验验证需要补70A

/*20121103 在Runclt里给定
			   if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   			//LVRT网发无功功率
			   {
					GIVE.npriqrf   = -(int16)_PROSTDBY1;   			//201007BJTULVRT
			   }
*/
			}
		    else     M_ClrFlag(SL_NPR_RUN);


	        if(M_ChkFlag(SL_MPR_START)!=0)  						//机侧运行判断
	        {
	           M_SetFlag(SL_MPR_RUN);                   			//机侧脉冲允许
 
               if(M_ChkFlag(SL_ENPCOPER)!=0)	  
               {
					if(M_ChkFlag(SL_IN1_STATORK)!=0)				//定子接触器闭合以后再接收转矩和角度指令
					{ 
               			GIVE.toqrf  =  (int16) _TOQRF;              //正值为发电
						GIVE.anglerf=  (int16) _AGLRF;  			//给功率因数角指令赋值,-360 -- 360

					    if(M_ChkFlag(SL_LV_QWORKING)!=0||M_ChkFlag(SL_HV_QWORKING)!=0)  		   	//HVRT LVRT机侧发有功或者无功
			            {
//					    	GIVE.toqrf  = (int16)_PROSTDBY2;   		//201007BJTULVRT//转矩不修改就是任然接受转矩指令
							GIVE.anglerf= 0;						//20110829
							GIVE.toqrf  = Te_feedback_lv;
			   			}
						else if(M_ChkFlag(SL_HV_QWORKING)!=0)
						{
							GIVE.anglerf= 0;						//20110829
						}
					}
					else
      		   		{
      		   			GIVE.toqrf    = 0;                  		//机侧给定转矩为0
        				GIVE.anglerf  = 0;		            		//机侧给定功率因数角度为0
						GIVE.toqrfflt = 0;
     		   		} 
			   }
               else   
               {           
					if(M_ChkFlag(SL_IN1_STATORK)!=0)				//定子接触器闭合以后再接收转矩和角度指令
					{
               			temp_toqrf     =  (int16)SCI_canopen.rx_torque;               //正值为发电
      		   			GIVE.toqrf     =  temp_toqrf * CAN_TEN / 16383.0;             //机侧转矩指令实际值  _TOQRF == -200%-200%

						if(GIVE.toqrf<0)	GIVE.toqrf=0;    //变流器对主控的负转矩指令不予响应 20090815
						if(GIVE.toqrf>9500)	GIVE.toqrf=9500;    //变流器对主控的超限转矩指令不予响应 20130801

               			temp_aglrf     =  (int16)SCI_canopen.rx_angle;               
      		   			GIVE.anglerf   =  temp_aglrf * CAN_ANGLEN / 16383.0;  

					    if(M_ChkFlag(SL_LV_QWORKING)!=0||M_ChkFlag(SL_HV_QWORKING)!=0)  		   	//LVRT机侧发有功或者无功
			            {
//					    	GIVE.toqrf  = (int16)_PROSTDBY2;   		//201007BJTULVRT
							GIVE.anglerf= 0;						//20110829
			   			}
      		   		}
      		   		else
      		   		{
      		   			GIVE.toqrf    = 0;                  		//机侧给定转矩为0
        				GIVE.anglerf  = 0;		            		//机侧给定功率因数角度为0
						GIVE.toqrfflt = 0;
     		   		}             				
			   
			   }

				if     (GIVE.anglerf > 0)
					GIVE.anglerf = GIVE.anglerf * 1.35;		//20140503LJD电能质量 20150204XD
				else if     (GIVE.anglerf < 0)
					GIVE.anglerf = GIVE.anglerf * 1.1;			//20140503LJD电能质量

			   if     (GIVE.anglerf <-30.0)                      GIVE.anglerf     = -30.0;
			   else if(GIVE.anglerf > 30.0)                      GIVE.anglerf     =  30.0;
			   else  											 GIVE.anglerf     = GIVE.anglerf;
                                         
	        }
            else    M_ClrFlag(SL_MPR_RUN);
//		}
	}
}
/*********************************************************************************************************
** 函数名称: RunCtrl
** 功能描述: 工作控制  cpc修改
** 输　入:   
** 输　出:   
** 注  释: 	 
**--------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**--------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:20090721修改
**--------------------------------------------------------------------------------------------------
***********************************************************************************************/
void RunCtrl(void)
{
    float udc_max,id_max,iq_max,toq_max,agl_max,temp,temp_var;//2013-12-9ZZJ增加temp_var
//	Uint16 temp_n,temp_ud,temp_iqexi,temp_number;
//	float  temp_exi,temp_iqk;
//	float  temp_Qc,temp_Qg,temp_tan;
//	float  temp_exi,temp_iqk,temp_npriqrf,temp_idrfmax,temp_iqrfmax;
	float  temp_exi,temp_npriqrf,temp_idrfmax;

//----------------------------嘣诵懈ǹ刂?---------------------------------------------------------

//-----网侧正在运行且要求正常停机
    if((M_ChkFlag(SL_NPR_RUNING)!=0)&&(M_ChkFlag(SL_NPR_RUN)==0))  				//网侧无功电流将为0后再停脉冲
    {
		RUN.npriqrf = Give_Integral(0,RUN.npriqstep,RUN.npriqrf);  				//指令减小直至等于0
            
        if(RUN.npriqrf==0)   
        {   
        	if(M_ChkCounter(MAIN_LOOP.cnt_isteady0,DELAY_ISTEADY0)>0)  M_ClrFlag(SL_NPR_RUNING);
		}
		else  MAIN_LOOP.cnt_isteady0= 0;
    }

//-----要求网侧运行
    else if(M_ChkFlag(SL_NPR_RUN)!= 0) 
    {  
       M_SetFlag(SL_NPR_RUNING);                 								//网侧正诵校挤⑼嗦龀?
       MAIN_LOOP.cnt_isteady0= 0;
	                

	   if(M_ChkFlag(SL_HV_QWORKING)!= 0)	    
	   {
		RUN.urf = (NGS_Udq_p - NGS_Udq_p_ex)/SQRT3 + GIVE.urf;
		if(RUN.urf <= GIVE.urf)	    RUN.urf=GIVE.urf;        			   
	   	else if(RUN.urf >= 1200)	RUN.urf=1200;	   		   
	   }
	   else
	   {
		RUN.urf = Give_Integral(GIVE.urf,RUN.ustep,RUN.urf);     				//电压给定积分
       	if(RUN.urf == GIVE.urf)	    M_SetFlag(SL_STEADYGV);        			   //给定积分完成,置稳定标志位
	   	else						M_ClrFlag(SL_STEADYGV);
	   }
	   
       	if(M_ChkFlag(SL_NSTOP)!=0)	RUN.urf=AD_OUT_UDC; 					   //20121103		

//-------网侧无功电流控制BJTULVRT20121103--------
		if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   			//LVRT网发无功功率
		{				
		 if(M_ChkCounter(MAIN_LOOP.cnt_gridfault_last,20)>=0)		//20130225  20ms后才开始发无功
		 {
			temp_npriqrf   = -(int16)_PROSTDBY1;   			//网侧无功指令计算20111103
			if((_COMMAND2&0x0400)!=0)	//备用10 20111106
			{
				if(M_ChkFlag(SL_LV_STATE)==0)
					temp_npriqrf   = -450; 	//对应后面的iq限幅，额定网压下网侧无功电流峰值须小于460A，对应的是故障恢复后的一段时间
			}

			GIVE.npriqrf_cl   = -GIVE.lvwtiqrf - DIP_STA_I.qflt;   		//Itq=100A Isq201205LVRTatZB

//			if((_COMMAND2&0x0200)!=0)	//备用9 201011BJTULVRT
//				GIVE.npriqrf   =  GIVE.npriqrf_cl;   		//Itq=100A Isq
//			else	
				GIVE.npriqrf = - kq * 400; //90kW-DFIG网侧30kW额定电流峰值65A 1.5MW是kq*296A 加大到400A 20120601night

			if(GIVE.npriqrf < temp_npriqrf)	GIVE.npriqrf=temp_npriqrf;
			else if(GIVE.npriqrf > 0)		GIVE.npriqrf=0;
//--------20130124----网侧发无功填补跌落初期无功的不足，1.2倍来发无功----
			if(M_ChkCounter(MAIN_LOOP.cnt_gridfault_last,120)<=0)     //前120ms
			{
//				GIVE.npriqrf = - kq * lv_ctrl5 * 400;
				if((NGS_Udq_n2p < 30) && (NGS_Udq_p < (0.43 * NGS_Udq_p_ex)))		//只3ph-20及35情况下才发	20130308
					GIVE.npriqrf = - kq * lv_ctrl5 * 400;	//20130308 lv_ctrl5 默认1.2
			}
//--------20130124--------
//--------不对称跌落网侧无功控制20130221--------
//			if((NGS_Udq_n2p > 27) && (PRO.Pgactive_lv>600000) && (NGS_Udq_p < (0.71 * NGS_Udq_p_ex)) && (M_ChkFlag(SL_SCRRUNING)!=0))
//			if((NGS_Udq_n2p > 27) && (NGS_Udq_p < (0.71 * NGS_Udq_p_ex)) && (M_ChkFlag(SL_SCRRUNING)!=0))	//20%/35%所有工况	20130222
			if((NGS_Udq_n2p > 38) && (PRO.Pgactive_lv>600000) && (NGS_Udq_p < (0.61 * NGS_Udq_p_ex)) && (M_ChkFlag(SL_SCRRUNING)!=0))	//20%/35%所有工况	20130306
			{
				GIVE.npriqrf = - 10 * lv_ctrl1;		//初值设定为600A
			}
			else if(NGS_Udq_n2p > 15)
			{
				GIVE.npriqrf = - _stdby03;			//初值设定为300A
//				GIVE.npriqrf = - 300;			//初值设定为300A
			}
//----------------------------------------------
//			if(M_ChkFlag(SL_LV_STRICTLV)==0 && (_STDBY9&0x0040 == 0))		//116.6	20130301
//				GIVE.npriqrf = - kq_lv * _stdby08;	//网压恢复后网侧填补无功400A 20130228

			if(GIVE.npriqrf < -850)	GIVE.npriqrf = -850;
			else if(GIVE.npriqrf >0)		GIVE.npriqrf=0;
		 }
		}
		else if (M_ChkFlag(SL_HV_QWORKING)!=0)
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_gridfault_last,100)<=0)
			GIVE.npriqrf = (NGS_Udq_p - NGS_Udq_p_ex) /(CAP4.omigasyn * NPR_L); 
			else
			{
				GIVE.npriqrf = kq * 1776;
			}
			if(GIVE.npriqrf > 850)	    GIVE.npriqrf = 850;
			else if(GIVE.npriqrf <0)		GIVE.npriqrf=0;
		}

		if(M_ChkFlag(SL_HV_QWORKING)!=0)
		   RUN.npriqrf = Give_Integral(GIVE.npriqrf,100,RUN.npriqrf); //每ms 100A
 	   	else if(M_ChkFlag(SL_STEADYGV)!=0)											//待电压给定完成后再给定无功电流
	 	   RUN.npriqrf = Give_Integral(GIVE.npriqrf,RUN.npriqstep,RUN.npriqrf); //网侧无功电流给定积分	         
						
	   if(M_ChkFlag(SL_NSTOP)!=0)  
	   		RUN.npriqrf=0;		//20121103		网侧临时封脉冲时将无功指令清零					

//	   if(M_ChkFlag(SL_UNBALANCE)!=0 && (NGS_Udq_n2p > 35))	//20121103
//	   		RUN.npriqrf=0; //不对称跌落很大时封脉冲时将无功指令清零					
//	   		RUN.npriqrf = - (int16)_PROSTDBY1 * lv_ctrl1;		//不对称跌落发少量无功以抵消电网无功吸收20121210					
	}
//-----待机    
    else									//待机
    {
	   M_ClrFlag(SL_NPR_RUNING);
	   M_ClrFlag(SL_STEADYGV);
	   MAIN_LOOP.cnt_isteady0= 0;

	   RUN.urf   = AD_OUT_UDC;													//给定积分初始值为当前Vdc值
       RUN.npriqrf  = 0;														//给定积分初始值为0
        
       udc_max    =  (float)FUNC[NO_URF].max;									//读取最大值
       iq_max     =  (float)FUNC[NO_NIQRF].max;
       temp       =  (float)_RU_UDCT;
	   RUN.ustep   = abs(udc_max)/temp;											//计算直流电压给定步长
	   temp       =  (float)_RU_IQT;
	   RUN.npriqstep  = abs(iq_max)/temp;										//计算无功电流给定步长

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

//----------------------------机侧运行有功电流值给定积分-------------------------------------

//-----机侧正在运行并网要求停机
    if((M_ChkFlag(SL_MPR_RUNING)!=0)&&(M_ChkFlag(SL_MPR_RUN)==0)) 
    {      
		if(M_ChkFlag(SL_MPR_SYNOK)!=0)												//systest201005atcpc
		{
		    RUN.toqrf = Give_Integral(0,RUN.toqstep,RUN.toqrf);  					//机侧转矩给定积分
		    RUN.aglrf = Give_Integral(0,RUN.aglstep,RUN.aglrf);  			    	//机侧功率因数角给定积分


//		    RUN.mpriqrf_g  =  - RUN.toqrf  / (IRQTOTE * TRS_NGS_U.dflt);   			//给定q轴电流指令计算
	    	RUN.mpriqrf_g  =  - RUN.toqrf * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * TRS_NGS_U.dflt / (314.15926 *  MPR_Ls));   //给定q轴电流指令计算201005atcpc
//----MagnetCurve2013-12-13--ZZJ--以下两句取消，每次停机按照运行时id指令值 MagnetCurve200909- 
//			RUN_mpridrf_exi  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));  //负号:网压ed与机侧d轴相差180度             
//          RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          						//乘调整系数,由操作器给定 20100622
/*--MagnetCurve2013-12-13---ZZJ
        	temp_ud = (Uint16)(TRS_NGS_U.dflt * SQRT3 / SQRT2);
        	if     (temp_ud < 540) 		temp_ud = 540;
        	else if(temp_ud > 800) 	    temp_ud = 800;
	    	temp_n = (temp_ud - 540) * 127 / 260;
        	temp_exi = TAB_EXI[temp_n];
        	RUN.mpridrf_exi =  RUN.mpridrf_exi * temp_exi;          				//查表调整系数,与空载感应电势有关
//      	    RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          					//乘调整系数,由操作器给定		20100622		
///20091022atzy
			RUN_mpriq = (Uint16)(- RUN.mpriqrf_g);
			if     (RUN_mpriq < 290)   temp_iqexi = 290;
			else if(RUN_mpriq > 600)   temp_iqexi = 600;
			temp_iqexi = RUN_mpriq;  //20091019atzy
			temp_number = (temp_iqexi - 290) * 127 / 310;
			temp_iqk = TAB_IQEXI[temp_number];
        	RUN.mpridrf_exi = RUN.mpridrf_exi * temp_iqk;          				//查表调整系数,由负载电流Irq给定确定 
*/ //20091022atzy
	 	    RUN.radianrf    = RUN.aglrf * 0.017453292;							//角度转弧度360->2PAI	 0.01745329=2 * PAI / 360
			   	    
//		    temp_Qc = TRS_NGS_U.dflt * 314.15926 * 501.0e-6 / SQRT2;
//		    temp_Qg = PRO.Pgactive * sin(RUN.radianrf) / cos(RUN.radianrf);
//			temp_tan    = (temp_Qg - temp_Qc) / PRO.Psactive;
//		    RUN.mpridrf_var = RUN.mpriqrf_g * temp_tan;

	    	RUN.mpridrf_var = RUN.mpriqrf_g * sin(RUN.radianrf) / cos(RUN.radianrf); //停机时，保证定子电流为0，即功率因数为1
        
 	        RUN.mpridrf_g   = RUN.mpridrf_exi + RUN.mpridrf_var;                                           

			RUN.mpridrf = Give_Integral(RUN.mpridrf_g,RUN.mpridstep,RUN.mpridrf); //机侧并网前d轴给定就为-90A
			RUN.mpriqrf = Give_Integral(RUN.mpriqrf_g,RUN.mpriqstep,RUN.mpriqrf); //指令减小直至等于0	  
		    if(RUN.mpriqrf>50)        M_SetFlag(SL_STACKEEPON);					  //绝对的=0是锌赡艽锊坏降膕ystest20100510
			else    			      M_ClrFlag(SL_STACKEEPON);
		    MAIN_LOOP.cnt_mprsteady0= 0; 		
		}
		else
		{
			RUN.mpriqrf = Give_Integral(0,RUN.mpriqstep,RUN.mpriqrf);  			   	//指令减小直至等于0
			RUN.mpridrf = Give_Integral(0,RUN.mpridstep,RUN.mpridrf);  				//指令减小直至等于0
		    if((RUN.mpridrf==0)&&(RUN.mpriqrf==0))  								//机侧有功和无功电流给定都降零 
		    { 
		        if(M_ChkCounter(MAIN_LOOP.cnt_mprsteady0,DELAY_MPRSTEADY0)>0)   M_ClrFlag(SL_MPR_RUNING); //0.05s
		    }
		    else   MAIN_LOOP.cnt_mprsteady0= 0; 		
		}
		
    }
   	 	   	
//-----机侧要求运行且Vdc稳定
//    else if((M_ChkFlag(SL_MPR_RUN)!=0) && (M_ChkFlag(SL_STEADYFB)!=0))  
    else if(M_ChkFlag(SL_MPR_RUN)!=0)  //2010-4-27atBJTU for new Sysctrl systest
    {  
        M_SetFlag(SL_MPR_RUNING);
        M_SetFlag(SL_STACKEEPON);                 									//机侧正在运行，开始发机侧脉冲
//        M_ClrFlag(SL_STACTRIPEN);                 								//systest 201005atcpc
        MAIN_LOOP.cnt_mprsteady0= 0;
//----20121103----
		if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   	//LVRT机侧发有功或者无功
		{
//			GIVE.toqrf  = GRD_Ut * GIVE.toqrf;   	//201205LVRTatZB
			RUN.toqrf = GIVE.toqrf;					//20130228 //低穿状态直接给定转矩，不需要斜坡积分 xly？
//			if(M_ChkFlag(SL_UNBALANCE)!=0)
//				GIVE.toqrfrun  = 0;   					//20121013LVRTatBJTU 不对称跌落下转矩不保持
		}
		else
		{
//			DataFilter(0.994,&GIVE.toqrfflt,GIVE.toqrf); //主控给定转矩平均值1s 每1ms执行一次201205LVRTatZB
 			GIVE.toqrfrun = GIVE.toqrf;  //用于保存故障前的转矩指令
 		}	   
//----------------
		RUN.toqrf = Give_Integral(GIVE.toqrf,RUN.toqstep,RUN.toqrf);  				//机侧转矩给定积分
		RUN.aglrf = Give_Integral(GIVE.anglerf,RUN.aglstep,RUN.aglrf);  			//机侧功率因数角给定积分
//-----20121103-------
		if(M_ChkFlag(SL_MSTOP)!=0 || (M_ChkFlag(SL_LV_STATE)!=0 && M_ChkFlag(SL_UNBALANCE)!=0))												//201007BJTULVRT
		{				
			RUN.toqrf = 0;  		//201205LVRTatZB 不平衡下的转矩运行指令要清零		
			RUN.aglrf = 0;  			
		}
//-------------------
 
//	    RUN.mpriqrf_g  =  - RUN.toqrf  / (IRQTOTE * TRS_NGS_U.dflt);   				//给定q轴电流指令计算
	    RUN.mpriqrf_g  =  - RUN.toqrf * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * TRS_NGS_U.dflt / (314.15926 *  MPR_Ls));   //给定q轴电流指令计算201005atcpc
/*---MagnetCurve2013-12-13---ZZJ
		RUN_mpridrf_exi  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));  //负号:网压ed与机侧d轴相差180度             
        RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          						//乘调整系数,由操作器给定 20100622
        temp_ud = (Uint16)(TRS_NGS_U.dflt * SQRT3 / SQRT2);							//网压线电压有效值 690V
        if     (temp_ud < 540) 		temp_ud = 540;
        else if(temp_ud > 800) 	    temp_ud = 800;
	    temp_n = (temp_ud - 540) * 127 / 260;
        temp_exi = TAB_EXI[temp_n];
        RUN.mpridrf_exi =  RUN.mpridrf_exi * temp_exi;          					//查表调整系数,由操作器给定 
//      	RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;          					//乘调整系数,由操作器给定	20100622			
*///---MagnetCurve2013-12-13---ZZJ
/*//20091022atzy 
		RUN_mpriq = (Uint16)(- RUN.mpriqrf_g);
		if     (RUN_mpriq < 290)   temp_iqexi = 290;
		else if(RUN_mpriq > 600)   temp_iqexi = 600;
        temp_iqexi = RUN_mpriq;  //20091019atzy
		temp_number = (temp_iqexi - 290) * 127 / 310;
		temp_iqk = TAB_IQEXI[temp_number];
        RUN.mpridrf_exi = RUN.mpridrf_exi * temp_iqk;          						//查表调整系数,由负载电流Irq给定确定 
*///20091022atzy 
//----MagnetCurve2013-12-13 ZZJ增加励磁调整内容---
//----MagnetCurve200909 test_start-----------------------

   		if(_SC_MSTDBY==0)
   		{
			RUN_mpridrf_exi  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));
        	RUN.mpridrf_exi  =  RUN_mpridrf_exi * _eidco;

 			if(_MC_OK==1)				//电机励磁曲线绘制完成
			{								
 				MC_DATA.in = 100 * TRS_NGS_U.dflt * SQRT3 / ((float)_SC_Usn * SQRT2);
				if(MC_DATA.in < 50)			MC_DATA.in = 50;		//输入限幅[50,110]
				else if(MC_DATA.in > 135)	MC_DATA.in = 135;		//输入限幅
 				Magnet_Curve(&MC_DATA);
        		RUN.mpridrf_exi =  RUN.mpridrf_exi * MC_DATA.k;
        	}
		}
		else
		{
			temp_exi  = - ((float)_SC_Usn * SQRT2 * STAROTRTO / (MPR_Lm * 314.15926));  //计算理论额定网压ed=563时机侧d轴励磁电流值            
        	if(_SC_MSTDBY==1)		RUN_mpridrf_exi = 0.5  * _SC_EXISTY1 * 0.001 * temp_exi;
			else if(_SC_MSTDBY==2)	RUN_mpridrf_exi = 0.75 * _SC_EXISTY2 * 0.001 * temp_exi;
			else if(_SC_MSTDBY==3)	RUN_mpridrf_exi = 0.9  * _SC_EXISTY3 * 0.001 * temp_exi;
			else if(_SC_MSTDBY==4)	RUN_mpridrf_exi = 1.0  * _SC_EXISTY4 * 0.001 * temp_exi;
			else if(_SC_MSTDBY==5)	RUN_mpridrf_exi = 1.1  * _SC_EXISTY5 * 0.001 * temp_exi;
			else if(_SC_MSTDBY==6)	RUN_mpridrf_exi = 1.15  * 1.4088 * temp_exi;
			else if(_SC_MSTDBY==7)	RUN_mpridrf_exi = 1.2   * 1.5904 * temp_exi;
			else if(_SC_MSTDBY==8)	RUN_mpridrf_exi = 1.25  * 1.8233 * temp_exi;
			else if(_SC_MSTDBY==9)	RUN_mpridrf_exi = 1.3   * 2.1123 * temp_exi;
			else if(_SC_MSTDBY==10)	RUN_mpridrf_exi = 1.35  * 2.4714 * temp_exi;
//--------------------校验环节------------------------------------------------------------
			else if(_SC_MSTDBY==99)
			{
				MC_DATA.in = 50 + MC_N_test * 5;	//[MC_test_n , Us]=[0;1;6 , 50;10;110]
				Magnet_Curve(&MC_DATA);
        		RUN_mpridrf_exi =  (0.5 + MC_N_test * 0.05) * temp_exi * MC_DATA.k;
			}

//----------------------------------------------------------------------------------------
			RUN.mpridrf_exi =  RUN_mpridrf_exi;     
		}		
//----MagnetCurve200909 test_end-----------------------


//----MagnetCurve2013-12-13 ZZJ增加励磁调整内容---end


 	    RUN.radianrf    = RUN.aglrf * 0.017453292;									//角度转弧度360->2PAI	 0.01745329=2 * PAI / 360
	    RUN.mpridrf_var = RUN.mpriqrf_g * sin(RUN.radianrf) / cos(RUN.radianrf); //停机时，保证定子电流为0，即功率因数为1
			   	    																//RUN.aglrf>0,转子励磁增加,发出感性无功;反之,容性
//	    temp_Qc = TRS_NGS_U.dflt * 314.15926 * 501.0e-6 / SQRT2;					//运行时，已总并网功率因数1为目标
//	    temp_Qg = PRO.Pgactive * sin(RUN.radianrf) / cos(RUN.radianrf);
//		temp_tan    = (temp_Qg - temp_Qc) / PRO.Psactive;
//	    RUN.mpridrf_var = RUN.mpriqrf_g * temp_tan;
//----MagnetCurve2013-12-13 ZZJ增加励磁调整内容---
		if(_MC_OK==1)				//电机励磁曲线绘制完成BJTULVRT201204
		{
	 		RUN.mpridrf_var = 	RUN.mpridrf_var	* MC_DATA.k;
		}
//----MagnetCurve2013-12-13 ZZJ增加励磁调整内容---end

		temp_var = RUN.mpridrf_var;//2013-12-9ZZJ修改无功指令响应问题  先保存正常情况的无功id，电压故障恢复后迅速恢复
//----20121103-----
//-------机侧无功电流控制BJTULVRT201204------------------
//进入电网故障程序之前，已经计算好了主控要求的无功功率电流RUN.mpridrf_var 基本励磁电流RUN.mpridrf_exi 转矩电流RUN.mpriqrf_g 
//		if(M_ChkFlag(SL_LV_STATE)!=0)  		   			//!!!!!!20120601night 201205LVRTatZB
		if(M_ChkFlag(SL_LV_QWORKING)!=0)  		   			//LVRT机侧发无功电流 20120603night改回延时响应主控转矩指令
		{
//			RUN.mpriqrf_g  = - GIVE.toqrfrun * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * GRD_UN / (314.15926 *  MPR_Ls));   //201205LVRTatZB
			RUN.mpriqrf_g  = - GIVE.toqrfrun * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * NGS_Udq_p_ex / (314.15926 *  MPR_Ls));   //20130301LVRTatZB
//注：保持跌落前转矩不变，认为网压额定，则跌落期间有功电流维持不变，避免Irq*突增//20120531
			
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_utlv,DELAY_LVUTLV)<=0)	//500ms
//				GRD_Utlv = GRD_Ut;
			{
				GRD_Utlv = GRD_Ut;
				NGS_Udq_p_lv = NGS_Udq_p; 		//20130124
				kq_lv = kq;				//网侧控制用 20130124
				NGS_Udq_n2p_lv = NGS_Udq_n2p;	//20130301
//				Te_feedback_lv=Te_feedback; 	//20130124
//--------分情况返回主控转矩值20130221--------
//				if(PRO.Pgactive_lv<600000)
				if((PRO.Pgactive_lv<600000) || (NGS_Udq_p > (NGS_Udq_p_ex * 0.83)))		//加入90%和两相75%的情况	20130303
					Te_feedback_lv = GIVE.toqrf_lv;		//反馈主控转矩维持20130128  故障前的转矩
				else
					Te_feedback_lv = GIVE.toqrf_lv * GRD_Utlv;
//--------分情况返回主控转矩值20130221--------
			}
			else
			{
//				RUN.mpriqrf_g  = - (GRD_Utlv/GRD_Ut) * GIVE.toqrfrun * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * GRD_UN / (314.15926 *  MPR_Ls));   //201205LVRTatZB
				RUN.mpriqrf_g  = - (GRD_Utlv/GRD_Ut) * GIVE.toqrfrun * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * NGS_Udq_p_ex / (314.15926 *  MPR_Ls));   //20130301LVRTatZB
													//GRD_Ut在电压恢复的时候会变大，转矩电流会变小，这样可以在电压恢复时保持转矩不变
//				if(PRO.Pgactive_lv<600000)		RUN.mpriqrf_g = RUN.mpriqrf_g_exlv;
				if((PRO.Pgactive_lv<600000) || (NGS_Udq_p_lv > (NGS_Udq_p_ex * 0.83)))	//加入90%和两相75%的情况	20130303
				RUN.mpriqrf_g = RUN.mpriqrf_g_exlv;   //小幅跌落，恢复跌落前的转矩
			}
//--------90%维持功率不变20130205--------2013-12-6 取消两相75%和90%有功保持功能
/*			if(M_ChkCounter(MAIN_LOOP.cnt_lv_holdpow,10)>=0)
			{
//				if((NGS_Udq_p>(NGS_Udq_p_ex * 0.85)) && M_ChkFlag(SL_LV_STRICTLV)!=0)		//20130222
//				if((NGS_Udq_p>(NGS_Udq_p_ex * 0.85)) && (NGS_Udq_n2p<9) && M_ChkFlag(SL_LV_STRICTLV)!=0)		//2ph-75排除在外 20130309
//					RUN.mpriqrf_g = RUN.mpriqrf_g * 1.14;

				if((_STDBY9&0x0008)==0 && (PRO.Pgactive_lv>600000) && (NGS_Udq_p>(NGS_Udq_p_ex * 0.83)) && (NGS_Udq_n2p>8) && M_ChkFlag(SL_LV_STRICTLV)!=0)  //116.3	20130314
					RUN.mpriqrf_g = - 400;		//2ph-75大风有功控制	20130314
				else if((NGS_Udq_p>(NGS_Udq_p_ex * 0.85)) && (NGS_Udq_n2p<9) && M_ChkFlag(SL_LV_STRICTLV)!=0)		//2ph-75排除在外 20130309
//					RUN.mpriqrf_g = RUN.mpriqrf_g * 1.14;
					RUN.mpriqrf_g = RUN.mpriqrf_g * 1.02;
			}
*/
//--------20130205--------

			if(M_ChkCounter(MAIN_LOOP.cnt_lv_mpridvar,DELAY_LVMIDVAR)>=0)	//延时给定无功电流201201
			{				
				if(M_ChkFlag(SL_LV_SYNCON)==0)	//201201  机侧没有封锁脉冲
				{
//					RUN.mpridrf_var	= - kq * 605.0;				//20111201 Ir_varn=116A 1.5MW是Irn=kq*505A
					RUN.mpridrf_var	= - kq * 645.0;				//20111201 Ir_varn=116A 1.5MW是Irn=kq*505A	20130309XLY?经过计算为615A，机侧部分为512A，这里给大了
//					RUN.mpridrf_var	= - kq * 116.0;				//20111201 Ir_varn=116A
//					temp_idrfmax = -(int16)_PROSTDBY2;
//					if(RUN.mpridrf_var < temp_idrfmax)	RUN.mpridrf_var=temp_idrfmax;	//机侧无功指令限幅
//					RUN.mpridrf_var  = -(int16)_PROSTDBY2;   		//发有功201011BJTULVRT

//				    RUN.mpriqrf_g  = (GRD_Utlv/GRD_Ut) * RUN.toqrf * STAROTRTO / (1.5 * SQRT3 * POLEPAIRES * MPR_Lm * GRD_UN / (314.15926 *  MPR_Ls));   //201205LVRTatZB
				}
				else	RUN.mpridrf_var = 0;
			}
			temp_idrfmax = - (int16)_PROSTDBY2;			//无功电流限幅665A
			if(RUN.mpridrf_var < temp_idrfmax)	RUN.mpridrf_var=temp_idrfmax;	//机侧无功指令限幅

//--------20130130----机侧发无功填补跌落初期无功的不足----
/*			if(M_ChkCounter(MAIN_LOOP.cnt_lv_mpridrf,100)<=0)
			{
				RUN.mpridrf_var = - kq * lv_ctrl4;
			}
			if(RUN.mpridrf_var < -900)	RUN.mpridrf_var = -900;	//机侧无功指令限幅
*///--------20130130--------
	
//			temp_iqrfmax = sqrt(302500.0 - ((RUN.mpridrf_exi + RUN.mpridrf_var) * (RUN.mpridrf_exi + RUN.mpridrf_var)));//Irmax=550A	
//			temp_iqrfmax = 	- _stdby03; 					//BJTULVRT201204有功电流限幅 给正数

//--------20121202--------
//			if(M_ChkFlag(SL_LV_SYNCON)!=0)
			if(M_ChkFlag(SL_LV_SYNCON)!=0 && M_ChkFlag(SL_SCRRUNING)!=0)		//20130202
			{
				RUN.mpriqrf_g = 0;
				MAIN_LOOP.cnt_lv_irqdelay=0;
			}
//			else
			else if(M_ChkFlag(SL_LV_STRICTLV)!=0)				//scr运行后60ms不发有功，网压恢复后立即发有功 20130228
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_lv_irqdelay,60)<=0)		
				RUN.mpriqrf_g = 0;
			}
//--------20121202--------

			if(RUN.mpriqrf_g < -680)	RUN.mpriqrf_g = -680;	//有功电流限幅
			else if(RUN.mpriqrf_g > 0)			RUN.mpriqrf_g = 0;
		}
		else
		{
			GRD_Utlv = GRD_Ut;
			MAIN_LOOP.cnt_lv_mpridvar = 0;
			MAIN_LOOP.cnt_lv_utlv = 0;	//201205LVRTatZB
			MAIN_LOOP.cnt_lv_mpridrf = 0;	//20130130
			RUN.mpridrf_var	= temp_var;		//2013-12-9ZZJ修改无功指令响应问题
			NGS_Udq_p_lv = NGS_Udq_p; 		//20130124
//			Te_feedback_lv=Te_feedback; 	//20130124
			GIVE.toqrf_lv=GIVE.toqrf; 	//20130128
			kq_lv = kq;				//网侧控制用 20130124
			RUN.mpriqrf_g_exlv = RUN.mpriqrf_g;		//20130221

			MAIN_LOOP.cnt_lv_holdpow = 0;	//20130205
		}
//			temp_iqrfmax = 	_stdby03; 					//BJTULVRT201204有功电流限幅 给正数
//			if(RUN.mpriqrf_g > temp_iqrfmax)	RUN.mpriqrf_g = temp_iqrfmax;	//有功电流限幅
//			else if(RUN.mpriqrf_g < 0)			RUN.mpriqrf_g = 0;


//-----------------------------------------------------------------xly 当前配置，不平衡下发少量有功，不发无功
		if(M_ChkFlag(SL_LV_STATE)!=0 && M_ChkFlag(SL_UNBALANCE)!=0)//201205LVRTatZB
		{
//			RUN.mpriqrf_g   = 0;	//检测到不平衡后不发有功和无功
			if(_STDBY9&0x0010!=0)	RUN.mpriqrf_g   = 0;	//116.4	20130228
			else	RUN.mpriqrf_g   = - kq * lv_ctrl2;		//20130228目前kq*100
			RUN.mpridrf_var = 0;
//			RUN.mpridrf_var = - (int16)_PROSTDBY2 * lv_ctrl5;		//不对称跌落发少量无功以抵消电网无功吸收20121210
		}
//-----------
        RUN.mpridrf_g   = RUN.mpridrf_exi + RUN.mpridrf_var;


		RUN.mpridrf = Give_Integral(RUN.mpridrf_g,RUN.mpridstep,RUN.mpridrf);  		//机侧有功电流给定积分
		RUN.mpriqrf = Give_Integral(RUN.mpriqrf_g,RUN.mpriqstep,RUN.mpriqrf);  		//机侧无功电流给定积分

		if(M_ChkFlag(SL_MSTOP)!=0)	 											    //201007BJTULVRT							
		{
			RUN.mpridrf=0;				
			RUN.mpriqrf=0;
		}			
//------允硎时鸥ɑ諦JTULVRT20121103
		id_max     =  (float)FUNC[NO_MIDRF].max;
	    temp       =  (float)_RU_IDT;
	    RUN.mpridstep  = abs(id_max)/temp;											//计算有Φ缌鞲ú匠? 

    } 
//-----待机
    else                                    										//待机
    {
	   	M_ClrFlag(SL_MPR_RUNING);
		M_ClrFlag(SL_STACKEEPON);	//systest										//201005atcpc
	    MAIN_LOOP.cnt_mprsteady0= 0;
        
        RUN.mpridrf= 0;															    //给定积分初始值为0
        RUN.mpriqrf= 0;															    //给定积分初始值为0
        RUN.mpridrf_g= 0;															//给定积分初始值为0
        RUN.mpriqrf_g= 0;	
        id_max     =  (float)FUNC[NO_MIDRF].max;
	    temp       =  (float)_RU_IDT;
	    RUN.mpridstep  = abs(id_max)/temp;											//计算有功电流给定步长 
        iq_max     =  (float)FUNC[NO_MIQRF].max;
	    temp       =  (float)_RU_IQT;
	    RUN.mpriqstep  = abs(iq_max)/temp;											//计算无功电流给定步长  
 
        RUN.toqrf  = 0;																//给定积分初始值为0
        RUN.aglrf  = 0;																//给定积分初始值为0
        toq_max    =  (float)FUNC[NO_TOQRF].max;
	    temp       =  (float)_RU_TOQT;		//单位ms
	    RUN.toqstep=  abs(toq_max)/temp;											//计算转矩给定步长 
        agl_max    =  (float)FUNC[NO_AGLRF].max;
	    temp       =  (float)_RU_AGLT;
//		temp       =  temp * 1000.0;		//单位s									//为CPC中心实验台降低有功给定速度201005atcpc
	    RUN.aglstep=  abs(agl_max)/temp; 											//计算功率因数角给定步长 
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

	if(M_ChkFlag(SL_MSTOP)!=0)	//LVRT
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

	if(M_ChkFlag(SL_NSTOP)!=0)	//LVRT
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
** 函数:  QepEncodPos
** 功能描述:  调整位置计数器为增计数模式，编码器位置检测
** 输　入:    稳态下的转子电流，定子压
** 输　?:    编码器位置rad  
** 注  释:    定子接触器合闸前运行，合闸后禁止运行。每个采样周期运行一次。非特别指明的角度均为电角度
**--------------------------------------------------------------------------------------------------
** 作　者: 	 
** 日　期: 	 20090330
**--------------------------------------------------------------------------------------------------
** 修改人:
** 日  期:  20090409
**--------------------------------------------------------------------------------------------------
***********************************************************************************************/
#pragma CODE_SECTION(QepEncodPos, "ramfuncs");
 void QepEncodPos(void)
{
//	Uint16 temp1;
	float temp_pos;
/*//20091021atzy经检测判断，计数AB信号不必对调，原先就是增计数，现场将对调程序屏蔽了
//----------------------判断POSCNT计数较颍繁Ｔ黾剖Ｊ?------------------------------------------
	if(M_ChkFlag(SL_QCAPSPDIN)!=0 && M_ChkFlag(SL_SPEED_HIGH)!=0)	//20091020atzy风机待机时电机轴会左右?//转速检测正常后，调整编码器工作模式
	{
		temp1 = EQep2Regs.QEPSTS.bit.QDF;   				//计数方向  0-逆时针-减   1-顺时针-增
		if (temp1 == 0 && M_ChkFlag(SL_QEPSWP)==0) 			//如果当前处于减计数模式，并且未对调过AB
		{
			temp1 = EQep2Regs.QDECCTL.bit.SWAP;				//不论AB是否对调过，对调当前的AB信号
		
			if (temp1 == 0) 
			{
				EQep2Regs.QPOSCNT = 1000;					//防止对调后立马报上溢故障			
				EQep2Regs.QDECCTL.bit.SWAP  = 1;			//交换A、B输入信号			
				EQep2Regs.QPOSCNT = 0; 					
			}
			else 
			{ 
				EQep2Regs.QPOSCNT = 1000;				 	//防止对调后立马报上溢故障
				EQep2Regs.QDECCTL.bit.SWAP  = 0;			//还原A、B输入信号
				EQep2Regs.QPOSCNT = 0; 						
			} 
		
			M_SetFlag(SL_QEPSWP);					    	//置AB对调完成标志位，防止多次对调AB
		}
//		else if (temp1==0 && M_ChkFlag(SL_QEPSWP)!=0)
//			M_SetFlag(SL_QEPPCDE);  						//置位置计数器方向错误标志位,为提高抗干扰性能,该故障不做判断 20090804于cpc

	}
*/

//--------20121103--------
	DIP_STA_I.sintheta = TRS_NGS_U.sintheta; 
	DIP_STA_I.costheta = TRS_NGS_U.costheta; 
//------------------定子侧电流dq值--------------------------------------------------------------------
   	DIP_STA_I.a     = AD_OUT_STA_I.ac;
   	DIP_STA_I.b     = AD_OUT_STA_I.ba;
   	DIP_STA_I.c     = AD_OUT_STA_I.cb;
	Transform_3s_2s_2r(&DIP_STA_I);
//	DataFilter(0.99,&DIP_STA_I.dflt,DIP_STA_I.d); 	//注：改滤波是400us/2.5kHz执行一次。定子侧电流反馈值滤波,Ts=200us,滤掉开关频率次0.97
	DataFilter(0.99,&DIP_STA_I.qbsf,DIP_STA_I.q); 	//c=0.99->8Hz; c=0.9->88Hz

//	BS_STA_Id.in = DIP_STA_I.dbsf;					//50Hz notch 将Is中dc量滤掉
	BS_STA_Iq.in = DIP_STA_I.qbsf;
//	BS_Loop_S(&BS_STA_Id,BS_PARA_STAI);	//陷波器使用需2e-4执行一次，这里是200us执行一次201205LVRTatZB
	BS_Loop_S(&BS_STA_Iq,BS_PARA_STAI);
//	DIP_STA_I.dflt = BS_STA_Id.out;
	DIP_STA_I.qflt = BS_STA_Iq.out;

//   	TRS_MPR_I.a     = AD_OUT_MPR_I.a + 0;	//验证计算磁链和转子控制都进行初始零漂BJTULVRT201204
//   	TRS_MPR_I.b     = AD_OUT_MPR_I.b + 0;
//   	TRS_MPR_I.c     = AD_OUT_MPR_I.c + 0;
   	TRS_MPR_I.a     = AD_OUT_MPR_I.a;	//不进行初始零漂BJTULVRT201204
   	TRS_MPR_I.b     = AD_OUT_MPR_I.b;
  	TRS_MPR_I.c     = AD_OUT_MPR_I.c;
	Transform_3s_2s_2r(&TRS_MPR_I);
	DataFilter(0.4,&TRS_MPR_I.dflt,TRS_MPR_I.d); 			//机侧电流反馈值瞬ǎ? Ts=200us,fh=1.2kHz,滤掉开关频率次
	DataFilter(0.4,&TRS_MPR_I.qflt,TRS_MPR_I.q); 			//机侧电流反馈值滤波， Ts=200us,fh=1.2kHz,滤掉开关频率次

	TRS_STA_I.a = AD_OUT_STA_I.a;
	TRS_STA_I.b = AD_OUT_STA_I.b;
	TRS_STA_I.c = AD_OUT_STA_I.c;
	Transform_3s_2s_2r(&TRS_STA_I);
	DataFilter(0.4,&TRS_STA_I.dflt,TRS_STA_I.d); 			//机侧电流反馈值瞬ǎ? Ts=200us,fh=1.2kHz,滤掉开关频率次
	DataFilter(0.4,&TRS_STA_I.qflt,TRS_STA_I.q); 			//机侧电流反馈值滤波， Ts=200us,fh=1.2kHz,滤掉开关频率次

//--------20121103--------

//--------QCAP正常工作后计数编码器位置--------------------------------------------------------------
	if(M_ChkFlag(SL_QCAPSPDIN)!=0 && M_ChkFlag(SL_MPR_SYNOK)==0)//转速检测正常且定子并网前，计算编码器位置 201005atcpc
	{
//		temp_pos = _encodpos;
//		QEPDATA.encodpos =  - temp_pos;   	//由操作器直接给定初始位置角度										
//--------20130801--------
	   if((_STDBY9&0x0040)!=0)	//116.6开关量备用
	   {
		temp_pos = _encodpos;
		QEPDATA.encodpos =  - temp_pos;   	//由操作器直接给定初始位置角度	
	   }
	   else
	   {
		if(M_ChkFlag(SL_MPR_START)==0)
		{
			M_SetFlag(SL_ENCODEPOS_OK);		//20130525
			MAIN_LOOP.cnt_ecposcheck = 0;
			MAIN_LOOP.cnt_ecpostrace = 0;
		}
		else if(M_ChkCounter(MAIN_LOOP.cnt_ecposcheck,1500)>0)		//机侧工作1.5s后开始判断压差
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
//						QEPDATA.encodpos = QEPDATA.encodpos - 0.5235988;		//30度
						QEPDATA.encodpos = QEPDATA.encodpos - 0.5061455;		//29度	20130604
						if (QEPDATA.encodpos < - TWOPAI)  	QEPDATA.encodpos = QEPDATA.encodpos + TWOPAI;
						ecpostep2 = 1;
						ecpostep3 = 1;
						MAIN_LOOP.cnt_ecpostrace = 0;
					}
					else if((abs(MEAN_DATA.ubc_t)>90) && (ecpostep2 == 0))
					{
//						QEPDATA.encodpos = QEPDATA.encodpos + 0.5235988;		//30度
						QEPDATA.encodpos = QEPDATA.encodpos + 0.5410521;		//31度	20130604
						if (QEPDATA.encodpos > 0)  	QEPDATA.encodpos = QEPDATA.encodpos - TWOPAI;
						ecpostep1 = 1;
						MAIN_LOOP.cnt_ecpostrace = 0;
					}
					else if(((abs(MEAN_DATA.ubc_t)<=90) && (ecpostep2 == 0)) || (ecpostep1 == 1))
					{
//						QEPDATA.encodpos = QEPDATA.encodpos - 0.3490659;		//20度
						QEPDATA.encodpos = QEPDATA.encodpos - 0.3839724;		//22度	20130604
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
							QEPDATA.encodpos = QEPDATA.encodpos + 0.08727;		//5度=0.08727
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
** 函数名称: PhaseOrderChk
** 功能描述: 检查相序是否正确。
** 输　入:
** 输　出:   
** 注  释: 在合主断前运行，合主断后不再调用此子函数 cap5-ubc   cap6-uab
**--------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 	20090627
**--------------------------------------------------------------------------------------------------
** 修改人:	
** 日  期:	
**--------------------------------------------------------------------------------------------------
***********************************************************************************************/
 void PhaseOrderChk(void)
{
//	float temp,phaseshift;
	float temp;

//------------计算网压频率、角频率、CAP单位时间内网压相位增量--------------------------------------
	temp=(float)ECap5Regs.CAP1;									//记录捕获值

	if(temp>0)
	{
		CAP5.prd = temp;										
		CAP5.freqtmp   = 150.0e6/CAP5.prd;						//计算本次频率
	}

//	if (abs(CAP5.freqtmp-50)<1)									//如果频率正常则更新频率
	if ((CAP5.freqtmp>_sc_freq2) && (CAP5.freqtmp<_sc_freq1))	//如果频率正常则更新频率201005atcpc
	{
		CAP5.freq   = CAP5.freqtmp;    							//更新
		CAP5.omigasyn  = TWOPAI * CAP5.freq;						
		CAP5.radpertb  = TWOPAI/CAP5.prd;						//计算一个计数周期角度增量
//		M_ClrFlag(SL_GRDFQE);									//清频率失败标志
	}
//	else	M_SetFlag(SL_GRDFQE);								//否则置频率有误标志位	
		
/*
//----------------计算Ubc琔ab相?-----------------------------------------------------------------
	temp = (float)ECap5Regs.TSCTR;
	CAP5.phase = temp * CAP5.radpertb;							//主断前网压Ubc相位 
	temp = (float)ECap6Regs.TSCTR;
	temp = temp * CAP5.radpertb;								//主断前网压Uab相位
	
//---------------计算相位差------------------------------------------------------------------------
	phaseshift = temp - CAP5.phase;								//计算主断前网压Uab与Ubc的相位差
	if (phaseshift<0)   phaseshift = phaseshift + TWOPAI;		//相差限制在0到2 * PAI之间

//---------------检查相序是否正确------------------------------------------------------------------
	if(abs(phaseshift - TWOPAI_3) >  PAI_3)  					//当相差误差超过PAI/3时报错
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_phorde,DELAY_PHORDE)>0)	//12s 上电延时判断网侧相序故障 201005atcpc
			M_SetFlag(SL_PHORDE);								//DSP上电后	相序有误标志位置1 20090817
	}
	else 
	{
		M_ClrFlag(SL_PHORDE);									// 清零相序有误标志位
		MAIN_LOOP.cnt_phorde=0;
	}
*/
//--------更改相序检测方法20120228	BJTULVRT201204--------
//----------------计算Ubc，Uab相位-----------------------------------------------------------------
	temp = (float)ECap5Regs.TSCTR;
	CAP5.phase = temp * CAP5.radpertb;							//主断前网压Ubc相位 
//---------------检查相序是否正确------------------------------------------------------------------
	if((CAP5.phase < TWOPAI_12 ))  					//当相差误差超过PAI/3时报错
	{
		if(AD_OUT_NGS_U.ab < 0)
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_phorde,DELAY_PHORDE)>0)	//12s 上电延时判断网侧相序故障 201005atcpc
			M_SetFlag(SL_PHORDE);			//DSP上电后	相序有蟊曛疚恢?1 20090817
		}
		else
		{
			M_ClrFlag(SL_PHORDE);									// 清零相序有误标志位
			MAIN_LOOP.cnt_phorde=0;
		}
	}
}

/**************************************************************************************************
** 函数名称: CapQepCtrl
** 功能描述: compute vector position and frequency
** 输  入:	  
** 输　出:    
** 注  释:    
			//Cap4.adsmptsctr=ECap4Regs.TSCTR; 		//加在AD采样启动语句后
			//QEPDATA.adsmposcnt=EQep2Regs.QPOSCNT;	//加在AD采样启动语句后
**--------------------------------------------------------------------------------------------------
** 作　者: 
** 日　:	20090409 
**--------------------------------------------------------------------------------------------------
** 修改人:
** 日  期:	20090812 at cpc
**--------------------------------------------------------------------------------------------------
**************************************************************************************************/
#pragma CODE_SECTION(CapQepCtrl, "ramfuncs");
 void CapQepCtrl(void)
{
	float	temp,temp1,radpert0;
	Uint32  temp2,temp3;

 if((_COMMAND2&0x0001)!=0)	//开关量备用0  201011PLL
 {

//------------计算网侧网压率、髦角频率、CAPノ皇奔淠谕瓜辔辉隽?-----------------------------------
	temp2 = ECap4Regs.CAP1;
	temp  =	(float)temp2;									//捕获事件周期值
	
	if(temp>0)
	{
		CAP4.prd = temp;									
		CAP4.freqtmp   = 150.0e6/CAP4.prd;					//计算本次网压频率
	}

//	if (abs(CAP4.freqtmp - 50)<1)							//如果频率正常则更新网压频率
	if ((CAP4.freqtmp>_sc_freq2) && (CAP4.freqtmp<_sc_freq1))	//如果频率正常则更新频率201005atcpc
	{
		CAP4.freq      = CAP4.freqtmp;				    	//更新网压频率 CAP4.freqtmp是实际实时网频
		CAP4.omigasyn  = TWOPAI * CAP4.freq;						
		CAP4.radpertb  = TWOPAI * CAP4.freq / 150.0e6;		//计算一个计数周期角度增量
		M_ClrFlag(SL_GRDFQE);					   	   	 	//清频率失败标志
	}
	else	M_SetFlag(SL_GRDFQE);							//频率有误标志位置1	
		


//-----------计算网侧算法正变换角度----------------------------------------------------------------                         
	
	radpert0       = TWOPAI * CAP4.freq * 0.2e-3;   		//计算一个T0中现芷诮嵌仍隽? T0=0.2ms
  
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
 else	//向角度采用PLL
 {
//-----------PLL计算网侧角频率和角度-201011PLL--------------------------------------------------------------
	DataFilter(0.999,&PI_DATA_PLL.outflt,PI_DATA_PLL.out); 	//PLL输出角频率在调节期间会振荡,加入适当滤波
	temp    = PI_DATA_PLL.outflt;
	
	if(temp>0)	CAP4.freqtmp   = temp / TWOPAI;				//计算本次网压频率

//	if (abs(CAP4.freqtmp - 50)<1)							//如果频率正常则更新网压频率
	if ((CAP4.freqtmp>_sc_freq2) && (CAP4.freqtmp<_sc_freq1))//如果频率正常且保持10ms恢复频率刷新201011PLL
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_freqrecv,DELAY_FREQRECV)>=0)
		{
			CAP4.freq      = CAP4.freqtmp;				    	//更新网压频率 CAP4.freqtmp是实际实时网频
			CAP4.omigasyn  = temp;			
			CAP4.radpertb  = CAP4.omigasyn / 150.0e6;			//计算一个计数周期角度增量			
			M_ClrFlag(SL_GRDFQE);					   	   	 	//清频率О鼙曛?	
		}
	}
	else	
	{
		M_SetFlag(SL_GRDFQE);								//频率有误标志位置1	
		MAIN_LOOP.cnt_freqrecv=0;
	}

//-----------计算网侧算法正变换角度-201011PLL----------------------------------------------------------------                         
	
	radpert0 = CAP4.omigasyn * 0.2e-3;   					//计算一个T0中断周期角度增量 T0=0.2ms
  
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

//-------------LV期间机侧定向角度抗干扰控制20121103LVRT-------------------------
	if(M_ChkFlag(SL_LV_STATE)!=0)
	{
	   CAP4.nprtrstheta_lv =CAP4.nprtrstheta_lv + radpert0;              
       if(CAP4.nprtrstheta_lv >= TWOPAI)	CAP4.nprtrstheta_lv = CAP4.nprtrstheta_lv - TWOPAI; 		
	}
	else
	{
		CAP4.nprtrstheta_lv =CAP4.nprtrstheta;              
	}
//开关量备用2  
	if((_COMMAND2&0x0004)==0)	CAP4.nprtrstheta_lv =CAP4.nprtrstheta;	//=1是LV期间机侧角度抗干扰递增,认为网压相位不变

/*
//-------------转子机械角频率和电角频率------------------------------------------------------------	

	if(M_ChkFlag(SL_SPEED_HIGH)!=0)
	{
		QEPDATA.qposlat2 = EQep2Regs.QPOSLAT;                 		//采用SYSCLK=150M，QUPRD=1.5M,单位频率为100Hz
		if(QEPDATA.qposlat2 < QEPDATA.qposlat1)
			QEPDATA.qposlat  = QEPDATA.qposlat2 + _SC_PLSPRVL * 4 - QEPDATA.qposlat1;  //20090817
		else
			QEPDATA.qposlat  = QEPDATA.qposlat2 - QEPDATA.qposlat1;
		
		QEPDATA.qposlat1  = QEPDATA.qposlat2;
		
		temp = 50.0 *  PAI * (float)QEPDATA.qposlat / _SC_PLSPRVL; 		//转子机械角频率rad/s  QEPDATA.omigamec = 2 * PAI * (150M/1.5M) *  QEPDATA.qposlat/(PLSPRVL * 4 )  20090817	 						                            		
	}
	else
	{
		QEPDATA.qcapprd = EQep2Regs.QCPRD;	                		//对eQEP模块的QCLK进行32分频，QCAP时钟采用SYSCLKOUT/128  20090817
		temp = 37.5e6 *  PAI/(_SC_PLSPRVL  * (float)QEPDATA.qcapprd); //转子机械角频率rad/s  QEPDATA.omigamec = 2 * PAI * (150M/128) *  64/(QCPRD * PLSPRVL * 4 )  20090817	 						                            		
	}
*/
//--------20121103--------
//-------------转子机械角频率和电角频率------------------------------------------------------------	

	if(M_ChkFlag(SL_SPEED_HIGH)!=0)
	{
		QEPDATA.qposlat2 = EQep2Regs.QPOSLAT;                 		//采用SYSCLK=150M，QUPRD=1.5M,单位频率为100Hz
//====================重要修正 201011BJTULVRT======================================================================
		if(QEPDATA.qposlat2 < QEPDATA.qposlat1)				//201011LVRT DSP内部的EQep2Regs.QPOSLAT更新速度没有T0这么快！刷新100Hz和采样为5kHz
		{
			QEPDATA.qposlat  = QEPDATA.qposlat2 + _SC_PLSPRVL * 4 - QEPDATA.qposlat1;  //20090817
			temp = 50.0 *  PAI * (float)QEPDATA.qposlat / _SC_PLSPRVL; 		//转子机械角频率rad/s  QEPDATA.omigamec = 2 * PAI * (150M/1.5M) *  QEPDATA.qposlat/(PLSPRVL * 4 )  20090817	 						                            		
		}			
		else if(QEPDATA.qposlat2 == QEPDATA.qposlat1)
		{
			temp = QEPDATA.omigamec;
		}
		else
		{
			QEPDATA.qposlat  = QEPDATA.qposlat2 - QEPDATA.qposlat1;
			temp = 50.0 *  PAI * (float)QEPDATA.qposlat / _SC_PLSPRVL; 		//转子机械角频率rad/s  QEPDATA.omigamec = 2 * PAI * (150M/1.5M) *  QEPDATA.qposlat/(PLSPRVL * 4 )  20090817	 						                            		
		}
			
		QEPDATA.qposlat1  = QEPDATA.qposlat2;
	}
	else
	{
		QEPDATA.qcapprd = EQep2Regs.QCPRD;	                		//对eQEP模块的QCLK进行32分频，QCAP时钟采用SYSCLKOUT/128  20090817
		temp = 37.5e6 *  PAI/(_SC_PLSPRVL  * (float)QEPDATA.qcapprd); //转子机械角频率rad/s  QEPDATA.omigamec = 2 * PAI * (150M/128) *  64/(QCPRD * PLSPRVL * 4 )  20090817	 						                            		
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
   		QEPDATA.omigamec = 0;  				//检测范围之外 
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
		else if(temp >= (0.75 * QEPDATA.omigamec) && temp <= (1.25 * QEPDATA.omigamec))	//8分频时丢失1个A或B信号测速将是上一次的1.25倍，多出1个则是0.75倍
		{
			QEPDATA.omigamec = temp;												//如果转速在正确范围内，更新;否则保持上一次的值 20090804于cpc
			MAIN_LOOP.cnt_qcapdisturb=0;
			M_ClrFlag(SL_QCAPDISTURB);		
		}
		else
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_qcapdisturb,DELAY_QCAPDISTURB)>=0)		//延时100ms,连续10次均被干扰,报故障 20090817cpc
			{
				M_SetFlag(SL_QCAPDISTURB);
				QEPDATA.omigamec = temp;    //20090817
			}
		}
	}
	 
   		  		
	QEPDATA.omigarote = POLEPAIRES * QEPDATA.omigamec;								//转子电角频率rad/s
//	DataFilter(0.985,&QEPDATA.omigaroteflt,QEPDATA.omigarote); 						//转速反馈值滤波， Ts=200us,Tr=100ms 20090815
	QEPDATA.rotradpret0 = QEPDATA.omigarote * 0.2e-3;								//每次T0周期转拥缃嵌仍隽浚ノ换《? 20090815

//-----------转差角频率----------------------------------------------------------------------------
	CAP4.omigaslp = CAP4.omigasyn - QEPDATA.omigarote;  	//转差角频率  20090815


//------------检测编码器是否有故障------------------------------------------------------------------	
	temp = EQep2Regs.QFLG.bit.PCO;
	if((temp!=0) && M_ChkFlag(SL_QCAPSPDIN)!=0)		
	{
	   _NQEPPCO++;
//	   if(_NQEPPCO >10)		   M_SetFlag(SL_QEPPCO);    	//置计数器上溢标志位
	   if(_NQEPPCO >1000)		   M_SetFlag(SL_QEPPCO);    	//置计数器上溢标志位	滤波时间2s,20120310
	   EQep2Regs.QCLR.bit.PCO = 1;
	   MAIN_LOOP.cnt_qepcntok=0;
	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_qepcntok,DELAY_QEPCNTOK)>=0) 				//1s 连续1s未发生上溢错误
		{
			_NQEPPCO=0;
			M_ClrFlag(SL_QEPPCO);
		}
	}

//--------20121103--------
//-----电机反转/AB信号反接故障判断-20110829HC---------------------	
	if(M_ChkFlag(SL_QCAPSPDIN)!=0)							//转速在范围内
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
	    M_SetFlag(SL_QEPPHE);								//QEP的AB信号相位故障,相差非90度,故障程序中不再检测该故障
	    EQep2Regs.QCLR.bit.PHE = 1; 
	}
*/
//---------判别A/B信号消失-201007BJTULVRT----------------------------------------------------------------------
	if(M_ChkFlag(SL_QCAPSPDIN)!=0)							//转速在范围内
	{
		temp = EQep2Regs.QFLG.bit.QDC;						//QPOSCNT计数方向改变
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

//----------机侧算法正变换角度--------------------------------------------------------------------- 
    QEPDATA.posilat = EQep2Regs.QPOSILAT;								//Z信号到来时POSCNT的计数值
//	if(QEPDATA.posilat < (_SC_PLSPRVL * 4 -20))
//	if((QEPDATA.posilat < (_SC_PLSPRVL * 4 -20)) || M_ChkFlag(SL_QEPPCDE)!=0)	//201007BJTULVRT
	if(((QEPDATA.posilat < (_SC_PLSPRVL * 4 -20)) || M_ChkFlag(SL_QEPPCDE)!=0)&&(M_ChkFlag(SL_SPEED_HIGH)!=0)) //高转速区再检测,防治误报20110829HC
	{	
		QEPDATA.rotpos = QEPDATA.rotpos + QEPDATA.rotradpret0;
		if(QEPDATA.rotpos > 2* TWOPAI) QEPDATA.rotpos = QEPDATA.rotpos - 2* TWOPAI;
//		if((M_ChkCounter(MAIN_LOOP.cnt_qepzdisturb,DELAY_QEPZDISTURB)>=0)&&(M_ChkFlag(SL_QCAPSPDIN)!=0))    //1s 转速检测正常后，计算编码器位置201007BJTULVRT
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

	temp1 = QEPDATA.rotpos - QEPDATA.encodpos;				//AD采样时刻转子位置 - 初始位置角度
                                                            // -2PIE -- 6PIE
	if 		(temp1 < 0) 	  	temp1 = temp1 + TWOPAI;	
	else if (temp1 > TWOPAI)  	temp1 = temp1 - TWOPAI;		//AD采样时刻转子位置取模(0-TWOPAI)
	if 		(temp1 < 0) 	  	temp1 = temp1 + TWOPAI;	
	else if (temp1 > TWOPAI)  	temp1 = temp1 - TWOPAI;		//AD采样时刻转子位置取模(0-TWOPAI)
							
//	temp = TWOPAI_3 + CAP4.nprtrstheta - temp1;				//正变换角度，未取模，（-1.3PAI---2.7PAI） 
	temp = TWOPAI_3 + CAP4.nprtrstheta_lv - temp1;			//正变换角度（-1.3PAI---2.7PAI） 20121103LVRT
															//定子绕组角接,B相电压过零时，电压矢量为30degree
	if		(temp > TWOPAI)	 	temp = temp - TWOPAI;	
	else if	(temp < 0)	  		temp = temp + TWOPAI;		//取模 (0-TWOPAI)
//	if		(temp > TWOPAI)	 	temp = temp - TWOPAI;	
//	else if	(temp < 0)	  		temp = temp + TWOPAI;		//取模 (0-TWOPAI)
		
	CAP4.mprtrstheta = temp;								//机侧正变换角度

	QEPDATA.rotposdisp = QEPDATA.rotpos;	//BJTULVRT201204
//	if 		(QEPDATA.rotpos < 0) 	  	QEPDATA.rotposdisp = QEPDATA.rotpos + TWOPAI;	
//	else if (QEPDATA.rotpos > TWOPAI)  	QEPDATA.rotposdisp = QEPDATA.rotpos - TWOPAI;	//QEPDATA.rotposdisp用于DA显示
	if 		(QEPDATA.rotposdisp < 0) 	  	QEPDATA.rotposdisp = QEPDATA.rotposdisp + TWOPAI;	
	else if (QEPDATA.rotposdisp > TWOPAI)  	QEPDATA.rotposdisp = QEPDATA.rotposdisp - TWOPAI;	//QEPDATA.rotposdisp用于DA显示 201005atcpc


//-----------计算网侧反变换角度--------------------------------------------------------------------
	if(M_ChkT1Direction() == 0)  							//CTR减计数 
    	temp3 = EPwm1Regs.TBPRD + EPwm1Regs.TBCTR;
	else 													//CTR黾剖?
		temp3 = 3 * EPwm1Regs.TBPRD - EPwm1Regs.TBCTR;

	temp3 = temp3 * 2; 										//变换到CAP4的时钟频率：150M

//	temp2 = ECap4Regs.TSCTR;     //problem:不能有效抗网频干扰
//	temp2 = temp2 + temp3;									//估计下次脉冲发时ECap4Regs.TSCTR的值		
//	temp  = temp2 * CAP4.radpertb;							//网侧幢浠唤嵌龋慈∧＃迹?0---2PAI）
	temp  = CAP4.nprtrstheta + (float)temp3 * CAP4.radpertb;//采用抗干扰后角度计算反变换角度201005atcpc		
	
	if (temp > TWOPAI)	temp = temp - TWOPAI;				//取模2 * PAI
			
	CAP4.npratitheta = temp;								//网侧反变换角度
	
//----------计算机侧反变换角度---------------------------------------------------------------------
//	temp2 = ECap4Regs.TSCTR;           //problem:不能有效抗网频干扰
//	temp  = (float)temp2;
//	temp  = temp * CAP4.radpertb + TWOPAI_3;  				//鼻岸ㄗ哟帕次恢胷ad PAI_2=PAI/2=1.5707963	定子绕组角接 zlquestion
/*	temp  = CAP4.nprtrstheta + TWOPAI_3;					//采用抗干扰后角度计算反变换角度201005atcpc		
  													
//	temp2 = EQep2Regs.QPOSCNT;
//	temp1 = (float)temp2;									//QEP模块位置计数器的值

//	temp1 = 1.53398e-3 * temp1;								//转子相对于周期信号的恢rad(电角度) //temp1 = POLEPAIRES  * PAI * temp1/(PLSPRVL * 2.0);
	temp1 = 1.5708 * POLEPAIRES * QEPDATA.adsmposcnt / ((float)_SC_PLSPRVL); 			//201005atcpc							

	temp1 = temp1 - QEPDATA.encodpos;						//鼻白邮导饰恢胷ad	

	if 		(temp1 < 0) 	   	temp1 = temp1 + TWOPAI;
	else if (temp1 > TWOPAI) 	temp1 = temp1 - TWOPAI;		//当前转子位置取模
		
	temp = temp - temp1; 	 								//当前反变换角度
*/	
	temp = CAP4.mprtrstheta; 	 							//当前反变换角度 201011BJTULVRT

	if(M_ChkT3Direction() == 0) temp1 = EPwm4Regs.TBPRD + EPwm4Regs.TBCTR;		//CTR减计数  	
	else 						temp1 = 3 * EPwm4Regs.TBPRD - EPwm4Regs.TBCTR;	//CTR增计数
		
	temp1 = temp1 * CAP4.omigaslp * 1.333333e-8; 			//剩余时间对应的角度增量PWMclk:75M, 1/75.0e6=1.3333e-8
															//补偿角度temp1符号随着CAP4.omigaslp符号变化
	temp = temp + temp1; 									//反变换角度，未取模，约（-1.5PAI---2.5PAI）		

	if		(temp > TWOPAI)		temp = temp - TWOPAI;		
	else if	(temp < 0)		  	temp = temp + TWOPAI;		//取模2 * PAI
	if		(temp > TWOPAI)		temp = temp - TWOPAI;		
	else if	(temp < 0)		  	temp = temp + TWOPAI;		//取模2 * PAI 201005atcpc

	CAP4.mpratitheta = temp; 								//机侧反变换角度

//--------------------赋值给3/2变换结构体变量-------------------------------------------------------

	TRS_NPR_I.sintheta = sin(CAP4.nprtrstheta);	
	TRS_NPR_I.costheta = cos(CAP4.nprtrstheta);				//网侧定向角度
	TRS_NGS_U.costheta = TRS_NPR_I.costheta;	
	TRS_NGS_U.sintheta = TRS_NPR_I.sintheta;				//网侧定向角度
	TRS_NPR_U.sintheta = sin(CAP4.npratitheta);	
	TRS_NPR_U.costheta = cos(CAP4.npratitheta);				//网侧控制电压角度 

	TRS_MPR_I.sintheta = sin(CAP4.mprtrstheta);				//机侧控制用
	TRS_MPR_I.costheta = cos(CAP4.mprtrstheta);		
	TRS_MPR_U.sintheta = sin(CAP4.mpratitheta);				//转子控制电压角度 
	TRS_MPR_U.costheta = cos(CAP4.mpratitheta);		

	TRS_STA_I.sintheta = -TRS_NPR_I.costheta;				//转子控制电压角度 
	TRS_STA_I.costheta =  TRS_NPR_I.sintheta;		

}

/**************************************************************************************************
** 函数名称: 	MPR_CONTROL
** 功能描述:  	机侧变流器控制算法
** 输　入:   	
** 输　出: 		 
** 注  释: 		鞒鲎游?	
**-------------------------------------------------------------------------------------------------
** 作　者: 		
** 日　期: 	
**-------------------------------------------------------------------------------------------------
** 修改人:
** 日期:	 
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/
#pragma CODE_SECTION(MPR_CONTROL, "ramfuncs");
void MPR_CONTROL(void)
{
   float temp_d,temp_q,temp_ud;
//   float temp_d,temp_q,temp_exi;
//	Uint16 temp_n,temp_ud;

//------------------机侧电流dq值--------------------------------------------------------------------
/*   	TRS_MPR_I.a     = AD_OUT_MPR_I.a;
   	TRS_MPR_I.b     = AD_OUT_MPR_I.b;
   	TRS_MPR_I.c     = AD_OUT_MPR_I.c;
	Transform_3s_2s_2r(&TRS_MPR_I);
	DataFilter(0.4,&TRS_MPR_I.dflt,TRS_MPR_I.d); 			//机侧电流反馈值滤波， Ts=200us,fh=1.2kHz,滤掉开关频率次
	DataFilter(0.4,&TRS_MPR_I.qflt,TRS_MPR_I.q); 			//机侧电流反馈值滤波， Ts=200us,fh=1.2kHz,滤掉开关频率次
*///不转移到QepEncodPos里面计算，得到快读磁链值BJTULVRT201204

//----------------运行机侧电流环-------------------------------------------------------------------
	if(M_ChkFlag(SL_MPR_RUNING)!=0)							//机侧调节需要运行
	{	
/*
		PI_MPR_Id.reference      = RUN.mpridrf;
		PI_MPR_Id.feedback       = TRS_MPR_I.dflt; 			//机侧d轴电流环
		PI_Loop(&PI_MPR_Id,PI_PARA_MPRID);   

		PI_MPR_Iq.reference      = RUN.mpriqrf;		  	  
		PI_MPR_Iq.feedback       = TRS_MPR_I.qflt;			//机侧q轴电流环
  		PI_Loop(&PI_MPR_Iq,PI_PARA_MPRIQ);
*/
//-----------应用LV磁链衰减控制-BJTULVRT201204 
//			if((_COMMAND2&0x0008)!=0)		//备用3 201201
//			{
//				PI_MPR_Id.reference    = RUN.mpridrf;
//				PR_M_Id.reference      = RUN.mpridrf;
//			}
//			else
//			{
//				if(M_ChkFlag(SL_LV_PHICON)!=0)
//				{
//					 PI_MPR_Id.reference    = RUN.mpridrf + PHI_DATA_M.Id;		//动态帕吹窒?
//					 PR_M_Id.reference      = RUN.mpridrf + PHI_DATA_M.Id;		//动态磁链抵消PR				
//				}
//				else 
//				{
					PI_MPR_Id.reference    = RUN.mpridrf;
//					PR_M_Id.reference      = RUN.mpridrf;
//				}
//			}
		
			PI_MPR_Id.feedback     = TRS_MPR_I.dflt; 			//机侧d轴电流环
//			PR_M_Id.feedback       = TRS_MPR_I.dflt; 			//机侧d轴电流环

//			PI_Loop(&PI_MPR_Id,PI_PARA_MPRID);

//			if(M_ChkFlag(SL_LV_SYNCON)!=0 && ((_STDBY9&0x0001)!=0)) //备用0 动态PI参数调节
//			{
//				PI_Loop(&PI_MPR_Id,PI_PARA_DYMI);   		
//			}
//			else
//			{
				PI_Loop(&PI_MPR_Id,PI_PARA_MPRID);   
//			}

		
//			if((_COMMAND2&0x0010)!=0)	//备用4 201201
//			{
//				PI_MPR_Iq.reference    = RUN.mpriqrf;	
//				PR_M_Iq.reference      = RUN.mpriqrf;	
//			}
//			else
//			{
//				if(M_ChkFlag(SL_LV_PHICON)!=0)
//				{
//						PI_MPR_Iq.reference    = RUN.mpriqrf + PHI_DATA_M.Iq;		 //动态磁链抵消 
//						PR_M_Iq.reference      = RUN.mpriqrf + PHI_DATA_M.Iq;		 //动态磁链抵消 
//				}
//				else	
//				{
					PI_MPR_Iq.reference    = RUN.mpriqrf;		  
//					PR_M_Iq.reference      = RUN.mpriqrf;		  
//				}
//			}	  	  
		
			PI_MPR_Iq.feedback     = TRS_MPR_I.qflt;			//机侧q轴电流环
//			PR_M_Iq.feedback       = TRS_MPR_I.qflt;			//机侧q轴电流环

//  			PI_Loop(&PI_MPR_Iq,PI_PARA_MPRIQ);	  	  	  
		

//			if(M_ChkFlag(SL_LV_SYNCON)!=0 && ((_STDBY9&0x0001)!=0)) //动态PI参数调节
//			{
//  				PI_Loop(&PI_MPR_Iq,PI_PARA_DYMI);	  	  	  	
//			}
//			else
//			{
  				PI_Loop(&PI_MPR_Iq,PI_PARA_MPRIQ);	  	  	  
//			} 
   
/*			if((_STDBY9&0x0008)!=0)  //116备用3 应用PR输出结果20120315
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
   
//--------------MPR输出电压算?---------------------------------------------------------------------

//   DM_imrd = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * CAP4.omigasyn));	//20100121
//   DM_imrq = - (TRS_NGS_U.qflt * SQRT3 * STAROTRTO / (MPR_Lm * CAP4.omigasyn));  //网压q轴前馈解耦项
/*	DM_imrd  = - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / (MPR_Lm * 314.15926));  		//负号:网压ed与机侧d轴相差180度             
    temp_ud = (Uint16)(TRS_NGS_U.dflt * SQRT3 / SQRT2);								//网压线电压有效值 690V
    if     (temp_ud < 540) 		temp_ud = 540;
    else if(temp_ud > 800) 	    temp_ud = 800;
	temp_n = (temp_ud - 540) * 127 / 260;
    temp_exi = TAB_EXI[temp_n];
    DM_imrd =  DM_imrd * temp_exi;          										//查表调整系数,由操作器给定 

*/

//磁链计算
	PHAI_d = TRS_STA_I.dflt * MPR_Ls + TRS_MPR_I.dflt * MPR_Lm / STAROTRTO;
	PHAI_q = TRS_STA_I.qflt * MPR_Ls + TRS_MPR_I.qflt * MPR_Lm / STAROTRTO;


//--------------MPR输出电压计算BJTULVRT20121103--------------------------------------------------------------------
//	if(M_ChkFlag(SL_MPR_SYNOK)!=0 || (_SC_MSTDBY!=0))  //201201	
	if(M_ChkFlag(SL_MPR_SYNOK)!=0)  //20121103			
	{
		PHI_DATA_M.PHIsd =  - (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / CAP4.omigasyn);	////201112fluxobs
		temp_ud = TRS_NGS_U.dflt;
	}
	else
	{
		PHI_DATA_M.PHIsd =  - RUN.mpridrf / RUN.mpridrf_g * (TRS_NGS_U.dflt * SQRT3 * STAROTRTO / CAP4.omigasyn);	//慢慢上升
		temp_ud = RUN.mpridrf / RUN.mpridrf_g * TRS_NGS_U.dflt;
	}

	if((M_ChkFlag(SL_UNBALANCE)==0) && (M_ChkFlag(SL_LV_STATE)!=0) && (NGS_Udq_p_lv < (0.8 * NGS_Udq_p_ex)))		  //20130223
	{
		temp_d = - PI_MPR_Id.out + SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Iq.feedback + TRS_MPR_I.dflt * _stdby02;	//解耦项计算

	  	temp_q = - PI_MPR_Iq.out - SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Id.feedback;	//解耦项计算
		temp_q = temp_q	- CAP4.omigaslp * MPR_Lm * PHI_DATA_M.PHIsd / MPR_Ls + TRS_MPR_I.qflt * _stdby02;

	}
	else
	{
		temp_d = - PI_MPR_Id.out + SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Iq.feedback;	//解耦项计算
	 	temp_q = - PI_MPR_Iq.out - SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Id.feedback;	//解耦项计算
		temp_q = temp_q	- CAP4.omigaslp * MPR_Lm * PHI_DATA_M.PHIsd / MPR_Ls;

//		temp_d = - PI_MPR_Id.out + SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Iq.feedback+MPR_Lm/MPR_Ls*(MPR_Rs*PHAI_d/MPR_Ls-QEPDATA.omigarote*PHAI_q-TRS_NGS_U.qflt);
//	 	temp_q = - PI_MPR_Iq.out - SIGMA * CAP4.omigaslp * MPR_Lr * PI_MPR_Id.feedback+MPR_Lm/MPR_Ls*(MPR_Rs*PHAI_q/MPR_Ls+QEPDATA.omigarote*PHAI_d+temp_ud);

	}

//	temp_d = temp_d - MPR_Rr * PI_MPR_Id.feedback;
//	temp_q = temp_q - MPR_Rr * PI_MPR_Iq.feedback;									//转子电阻压降
  
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
	Anti_Transform_2r_2s(&TRS_MPR_U);							//坐标反变换到两相静止系

//------------SVM脉宽计算和脉冲发生-----------------------------------------------------------------	
	SVPWM_MPR(TRS_MPR_U.alfa,TRS_MPR_U.beta);					//脉冲发生

//------------Te实际转矩反馈值计算-----------------------------------------------------------------	
//	DataFilter(0.8,&DIP_MPR_I.qflt,TRS_MPR_I.q); 					//定子侧功率计算,机侧q电流， Ts=200us,fh=Hz
	DataFilter(0.8,&DIP_MPR_I.qflt,TRS_MPR_I.qflt); 					//定子侧功率计算,机侧q电流， Ts=200us,fh=Hz
	Te_feedback = - (1.5 * SQRT3 * POLEPAIRES * MPR_Lm  / (314.15926 * STAROTRTO * MPR_Ls)) * DIP_NPR_U.dflt *  DIP_MPR_I.qflt;     // Te=-1.5*np*(Us/w1)*(Irq/K)*(Lm/Ls),MPR_i为负值
//	if(Te_feedback<=0)	Te_feedback=0;   							//20090817	20121129取消atsp
//-----BJTULVRT20121103
	Te_instant  = - (1.5 * SQRT3 * POLEPAIRES * MPR_Lm  / (314.15926 * STAROTRTO * MPR_Ls)) * TRS_NGS_U.dflt *  TRS_MPR_I.qflt;     // Te=-1.5*np*(Us/w1)*(Irq/K)*(Lm/Ls),MPR_i为负值

}  


/**************************************************************************************************
** 函数名称: 	NPR_CONTROL
** 功能描述:  	网侧变流器控制算法
** 输　入:   	
** 输　出: 		 
** 注  释: 		电流流向电网为正	
**-------------------------------------------------------------------------------------------------------
** 作　者: 		
** 日　期: 		
**-------------------------------------------------------------------------------------------------------
** 修改者:
** 日期:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
#pragma CODE_SECTION(NPR_CONTROL, "ramfuncs");
void NPR_CONTROL(void)
{ 
//	float temp_d,temp_q;

//-----------计算网侧变流器电流dq值-----------------------------------------------------------------
	TRS_NPR_I.a = AD_OUT_NPR_I.a;  							//获取网侧变流器电流
	TRS_NPR_I.b = AD_OUT_NPR_I.b;
	TRS_NPR_I.c = AD_OUT_NPR_I.c;
	Transform_3s_2s_2r(&TRS_NPR_I);							//坐标变换
	DataFilter(0.4,&TRS_NPR_I.dflt,TRS_NPR_I.d); 			//网侧电流反馈值滤波， Ts=200us,fh=1.2kHz,滤掉开关频率次
	DataFilter(0.4,&TRS_NPR_I.qflt,TRS_NPR_I.q); 			//网侧电流蠢≈德瞬ǎ? Ts=200us,fh=1.2kHz，20090615改

//----------计算网压dq值----------------------------------------------------------------------------
	TRS_NGS_U.a = AD_OUT_NGS_U.a;							//获取电网电压
	TRS_NGS_U.b = AD_OUT_NGS_U.b;
	TRS_NGS_U.c = AD_OUT_NGS_U.c;
	Transform_3s_2s_2r(&TRS_NGS_U);							//坐标变换

	if(TRS_NGS_U.q > 200)			TRS_NGS_U.q = 200;		//20130228
	else if(TRS_NGS_U.q < -200)		TRS_NGS_U.q = -200;		//20130228

	DataFilter(0.4,&TRS_NGS_U.dflt,TRS_NGS_U.d); 			//网压反值滤波，Ts=200us,fh=1.2kHz 20090608change to ok
	DataFilter(0.4,&TRS_NGS_U.qflt,TRS_NGS_U.q); 			//网压反馈德瞬ǎ琓s=200us,fh=1.2kHz 20090608change to ok
//	DataFilter(0.1,&TRS_NGS_U.dflt2,TRS_NGS_U.d); 			//网压反馈值滤波，Ts=200us,fh=7.9kHz,126us,为监测网跌，20091026


	if(M_ChkFlag(SL_NPR_RUNING)!=0)							//网侧PI运行控制条件
	{

//----------运行电压外环----------------------------------------------------------------------------
	  if(M_ChkFlag(SL_HV_QWORKING)==0)  						//Vdc没有稳定，且采用稳态PI参数
	  {

		PI_NPR_U.reference     = - RUN.urf;   				//获取中间电压指令
		PI_NPR_U.feedback      = - AD_OUT_UDC;				//获取中涞缪狗蠢≈?
    	PI_Loop(&PI_NPR_U,PI_PARA_DYNU);
      }	
	  else                                                   //采用稳态PI参数
	  {
		PI_NPR_U.reference     = - RUN.urf;   				//获取中间电压指令
		PI_NPR_U.feedback      = - AD_OUT_UDC;				//获取中间电压反馈值
    	PI_Loop(&PI_NPR_U,PI_PARA_NPRU); 
      }
//--------对35%-2ph工况做电压环限幅20130222--------
		if(M_ChkFlag(SL_LV_STATE)!=0)
		{
			if(M_ChkFlag(SL_UNBALANCE)!=0 && (NGS_Udq_n2p < 50) && M_ChkFlag(SL_SCRRUNING)==0)	//除20%-2ph，其余2ph都限值
			{
				if(PI_NPR_U.out > lv_ctrl6)				PI_NPR_U.out = lv_ctrl6;	//20130223
				else if(PI_NPR_U.out < -lv_ctrl6)		PI_NPR_U.out = -lv_ctrl6;	//20130223
			}
		}

//---------运行d轴电流环----------------------------------------------------------------------------
   		PI_NPR_Id.reference      = PI_NPR_U.out; 			//获取d轴电流噶睿⒁庹汉?
		PI_NPR_Id.feedback       = TRS_NPR_I.dflt;			//获取d轴电流反馈滤波值
		PI_Loop(&PI_NPR_Id,PI_PARA_NPRID);

//---------运行q轴电流环----------------------------------------------------------------------------
		PI_NPR_Iq.reference      = RUN.npriqrf; 			//q轴电流指令
		PI_NPR_Iq.feedback       = TRS_NPR_I.qflt;	  		//获取q岬缌鞣蠢÷瞬ㄖ?
		PI_Loop(&PI_NPR_Iq,PI_PARA_NPRIQ);	  	  
	}

//---------扑阈迪翹PR控制电压------------------------------------------------------------------

//	TRS_NPR_U.d = PI_NPR_Id.out  + TRS_NGS_U.dflt - CAP4.omigasyn * NPR_L * PI_NPR_Iq.feedback;
//	TRS_NPR_U.q = PI_NPR_Iq.out  + TRS_NGS_U.qflt + CAP4.omigasyn * NPR_L * PI_NPR_Id.feedback;
//	if((_STDBY9&0x0020)==0)		  //116.5 取消前馈滤波2013022
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

//---------SVM脉宽计算-----------------------------------------------------------------------------
	SVPWM_NPR(TRS_NPR_U.alfa,TRS_NPR_U.beta);				//脉冲发生

//--20121103---网侧有功功率计算(3相)-----------------------------------------------------------------	
	DataFilter(0.99,&DIP_NPR_U.dflt,TRS_NGS_U.dpll); 			//定子侧功率计算,网压ed， Ts=200us,Tr=250ms

//------------网侧有功功率计算(3相)-----------------------------------------------------------------	
/*	DataFilter(0.99,&DIP_NPR_I.dflt,TRS_NPR_I.d); 			//定子侧功率计算,网压ed， Ts=200us,fh=88Hz
	DataFilter(0.99,&DIP_NPR_I.qflt,TRS_NPR_I.q); 			//定子侧功率计算,机侧q电流， Ts=200us,fh=88Hz
	DataFilter(0.99,&DIP_NPR_U.qflt,TRS_NGS_U.q); 			//定子侧功率计算,机侧q电流， Ts=200us,fh=88Hz
	DataFilter(0.995,&DIP_NPR_U.dflt,TRS_NGS_U.d); 			//定子侧功率计算,网压ed， Ts=200us,Tr=250ms

	Pnactive   = 1.5 * (DIP_NPR_U.dflt * DIP_NPR_I.dflt + DIP_NPR_U.qflt * DIP_NPR_I.qflt);   	// Pnactive=3*Ud*Id/(squt2*squt2)
	Pnreactive = 1.5 * (DIP_NPR_U.qflt * DIP_NPR_I.dflt - DIP_NPR_U.dflt * DIP_NPR_I.qflt);   	// Pnactive=3*Ud*Id/(squt2*squt2)
*///20121103
}


/**************************************************************************************************
** 名称: SysCtrl
** 功能描述: 1.5MW变流器系统逻辑控制-主程序 	
** 输　入:   	
** 输　出: 		 
** 注  释: 	systest		
**-------------------------------------------------------------------------------------------------
** 作　者: 	 
** 日　期: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** 修改人:
** 日期:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void SysCtrl(void)         
{
	if(M_ChkFlag(SL_SERIESTOPING)!=0)							//正在严重故障停机中
	{
		SER_STOP();												//执行严重故障停机,先断主断
	}
	else if(M_ChkFlag(SL_SERIESTOP)!=0)							//存在严重故障
	{
		M_SetFlag(SL_SERIESTOPING);								//置 正在严重故障停机中 标志
	}	
	else if(M_ChkFlag(SL_ERRSTOPING)!=0)						//正在一般故障停机中
	{
		ERR_STOP();												//执行一般故障停机,增加手动断开CB控制201005atcpc
	}
	else if(M_ChkFlag(SL_ERRSTOP)!=0)							//判断是否存在一般故障
	{
		M_SetFlag(SL_ERRSTOPING);								//置 正在一般故障停机中 标志
	}
	else if(M_ChkFlag(SL_NORSTOPING)!=0)						//正在正常停机中
	{
		NOR_STOP();												//执行正常停机,增加手动断开CB控制201005atcpc
	}
	else if(M_ChkFlag(SL_OCS_EIN)==0)							//是否接收到主控BIT0闭合主断指令
	{
		M_SetFlag(SL_NORSTOPING);								//置 正在正常停机中 标志
	}
	else if(M_ChkFlag(SL_CBCLOSED)==0)							//主断闭合是否完成
	{
		CB_ON();												//执行主断闭合操作
	}
	else if(M_ChkFlag(SL_OCS_SYSRUN)==0)						//是否接收到主控BIT2同步并网指令
	{
		M_SetFlag(SL_NORSTOPING);								//置 正在正常停机中 标志
	}
	else if(M_ChkFlag(SL_MPR_SYNOK)==0)							//定子并网是否已完成
	{
		RUN_SYN();												//执行预涞绲酵讲⑼僮?
	}
	else														//执行定子并网后运行控制
	{
		if((AD_OUT_UDC>(_URF - 20))&&(AD_OUT_UDC<(_URF + 20))) 	//运行中判断直流电压是否稳定+-20V  
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
** 名称: RUN_SYN
** 功能描?: 1.5MW变流器系统逻辑控制-同步并网	
** 输　入:   	
** 输　出: 		 
** 注  释: 			
**-------------------------------------------------------------------------------------------------
** 作　者: 	 
** 日　期: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** 修改人:
** 日期:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void RUN_SYN(void)         
{ 
	if(M_ChkFlag(SL_SYN_FH)!=0)										//FH=并网完成
	{
		if(M_ChkFlag(SL_IN1_STATORK)!=0)							//判断定子接触器是否已闭合
		{		
			M_SetFlag(SL_MPR_SYNOK);								//置定子并网完成
			M_ClrFlag(SL_SYN_FH);									//清步骤标志位
			M_ClrFlag(SL_SYN_S3);									//清步骤标志位
			M_ClrFlag(SL_SYN_S2);									//清步骤标志位
			M_ClrFlag(SL_SYN_S1);									//清步骤标志位
		}	
	}
	else if(M_ChkFlag(SL_SYN_S3)!=0)								//S3=启动机侧定子并网
	{
		M_SetFlag(SL_MPR_START);									//机侧变流器运行

		if(MEAN_DATA.uab_d <= _SC_UDSTAC && MEAN_DATA.ubc_d <= _SC_UDSTAC)    //定子符合并网条件    					//定子接触器前后半波平均值差在40V以内
		{
			M_SetFlag(SL_UDSTACOK);									//置 定子达到并网条件 标志

		    if(M_ChkFlag(SL_OCS_OPENSTAC)==0)	
		    {														//外部控制，为0才允许闭合STAC
		      	M_SetFlag(CL_STATORK);  							//上位机允许并网,闭合定子接触器
				M_SetFlag(SL_SYN_FH);								//置步骤标志位
				MAIN_LOOP.cnt_synrun=0;           					//清计数器		
		    }                             					
			else	M_ClrFlag(CL_STATORK);							//上位机禁止并网,断开定子接触器	        
		}
		else	M_ClrFlag(SL_UDSTACOK);								//清 定子达到并网条件 标志
	}
	else if(M_ChkFlag(SL_SYN_S2)!=0)								//S2=启动网侧
	{
		M_SetFlag(SL_OCS_NPRSTART);									//启疍SP_PWM配置

		if(M_ChkFlag(SL_CONFIGPWM)!=0)   M_SetFlag(SL_NPR_START);	//pwmdrive配置PWM完成,啾流器运行		  
		else   	                         M_ClrFlag(SL_NPR_START); 

		if(AD_OUT_UDC>(_URF - 20) && AD_OUT_UDC<(_URF + 20) && M_ChkFlag(SL_NPR_PWMOUT)!=0) //网侧脉冲已发出且Vdc稳定
   		{
   			if(M_ChkCounter(MAIN_LOOP.cnt_synrun,DELAY_STEADYFB)>=0)
			{
				M_SetFlag(SL_STEADYFB);          					//置Vdc稳定
				M_SetFlag(SL_SYN_S3);								//置步骤标志位
				MAIN_LOOP.cnt_synrun=0;           					//清计数器		
			}
   		}
		else
		{
			M_ClrFlag(SL_STEADYFB);          						//清Vdc稳定
			MAIN_LOOP.cnt_synrun=0;           						//寮剖?		
		}	
	}
	else if(M_ChkFlag(SL_SYN_S1)!=0)								//S1=预充电
	{
		if( AD_OUT_UDC>=_SC_VDCON && M_ChkCounter(MAIN_LOOP.cnt_synrun,DELAY_PRECOK)>=0) //8s且Vdc>700V 
		{
			M_SetFlag(CL_MAINK);        							//预充电OK，闭合主接触器 
      	
	      	if(M_ChkFlag(SL_IN1_MIANK)!=0 && M_ChkFlag(SL_IN1_MIANFILTER)!=0 && AD_OUT_UDC>=870)  //主接触器和滤波器均闭合
			{
				M_ClrFlag(CL_PRE);          	 					//断开预充电接触器
		 		M_SetFlag(SL_CHARGEOK);               				//预充电完成         
				M_SetFlag(SL_FAN_WORK);								//风机运行
				M_SetFlag(SL_SYN_S2);								//置步骤标志位
				MAIN_LOOP.cnt_synrun=0;           					//清计数器	
			}
		}
	}
	else															//S0=合预充电接触器
	{
      	if(M_ChkFlag(SL_SPEED_IN_RANGE)!=0)     					//转速在范围内
		{
			M_SetFlag(CL_PRE);          	 						//闭合こ涞缃哟テ?
			M_SetFlag(SL_SYN_S1);									//置步骤标志位
			MAIN_LOOP.cnt_synrun=0;           						//清计数器	
		}
	}
}

/**************************************************************************************************
** 名称: NOR_STOP
** 功能描述: 1.5MW变流器系统逻辑控制-正常停机
** 输　入:   	
** 输　出: 		 
** 注  释: 			
**-------------------------------------------------------------------------------------------------
** 作　者: 	 
** 日　期: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** 修改人:
** 日期:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void NOR_STOP(void)         
{ 
	if(M_ChkFlag(SL_NORSTOP_FH)!=0)									//FH=断开主接触器
	{
		M_ClrFlag(CL_MAINK);    		  							//断主接触器和滤波器
		M_ClrFlag(CL_PRE); 											//再断预充电接触器
	   	M_ClrFlag(SL_CHARGEOK);          							//清预充电完成

		if(M_ChkFlag(SL_IN1_MIANK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)==0)
		{
			M_ClrFlag(SL_FAN_WORK);									//停止风机运行
			if(M_ChkFlag(SL_PCOCS_CBOFF)!=0 && M_ChkFlag(SL_OCS_EIN)==0)//PC手动控制断开CB 201005atcpc
			{
			   	M_ClrFlag(CL_CBON);    		     					//清主断合闸指令
			   	M_ClrFlag(CL_CBLVC);         						//清欠压链断开，主断断开 
			   	M_ClrFlag(CL_CBFCON);            					//断开故障链
				M_ClrFlag(SL_CBCLOSED);			 					//清主断闭合完成
				M_ClrFlag(SL_PCOCS_CBOFF);            				//清断开指令
				M_ClrFlag(SL_CBON_FH);								//清CBON步骤标志位
			} 
/*		   	M_ClrFlag(SL_NORSTOPING);          						//清 正在正常停机中 标志
			M_ClrFlag(SL_NORSTOP_FH);								//清步骤标志位
			M_ClrFlag(SL_NORSTOP_S2);								//清步骤标志位
			M_ClrFlag(SL_NORSTOP_S1);								//清步骤标志位
*/
//--------20120321--------
			if(M_ChkFlag(SL_SYSOFF)==0)
			{
			   	M_ClrFlag(SL_NORSTOPING);          						//清 正在正常停机中 标志
				M_ClrFlag(SL_NORSTOP_FH);								//清步骤标志位
				M_ClrFlag(SL_NORSTOP_S2);								//清步骤标志位
				M_ClrFlag(SL_NORSTOP_S1);								//清步骤标志位
			}
//------------------------

		}	
	}
	else if(M_ChkFlag(SL_NORSTOP_S2)!=0)							//S2=停网侧
	{
		M_ClrFlag(SL_NPR_START);									//清网侧脉冲指令
		M_ClrFlag(SL_OCS_NPRSTART);									//清网侧运行指令

		if(M_ChkFlag(SL_NPR_PWMOUT)==0)								//网侧脉冲已停止
		{
			M_ClrFlag(SL_STEADYFB);          						//清Vdc稳定
			M_SetFlag(SL_NORSTOP_FH);								//置步骤标志位
			MAIN_LOOP.cnt_norstop=0;           						//清计数器	
		}
	}
	else if(M_ChkFlag(SL_NORSTOP_S1)!=0)							//S1=断开定咏哟テ魍；?
	{        
		if(M_ChkFlag(SL_IN1_STATORK)==0)							//定子接触器断开
		{
			M_ClrFlag(SL_MPR_SYNOK);								//清定子并网完成,与RUNCtrl有关,不要轻易修改		
			M_ClrFlag(SL_UDSTACOK);									//清 定子达到并网条件 标志
			
			if(M_ChkFlag(SL_MPR_PWMOUT)==0)							//机侧脉冲停止
			{
				M_SetFlag(SL_NORSTOP_S2);							//置步骤标志位
				MAIN_LOOP.cnt_norstop=0;           					//清计数器	
			}
		}      								
	}
	else															//S0=停机侧脉冲
	{
		M_ClrFlag(SL_MPR_START);									//清机侧脉冲指令,机侧有功降低

        if(M_ChkFlag(SL_STACKEEPON)==0)	  
        {
        	M_ClrFlag(CL_STATORK);    								//达到定子断开条件, 断开定子接触器
			M_SetFlag(SL_NORSTOP_S1);								//置步骤标志位
			MAIN_LOOP.cnt_norstop=0;           						//清计数器	
        }	

		M_ClrFlag(SL_SYN_FH);										//清SYN步骤标志位
		M_ClrFlag(SL_SYN_S3);										//清SYN步骤标志位
		M_ClrFlag(SL_SYN_S2);										//清SYN步骤标志位
		M_ClrFlag(SL_SYN_S1);										//清SYN步骤标志位	
	}
}

/**************************************************************************************************
** 名称: SER_STOP
** 功能描述: 1.5MW变流器系统逻辑控制-严重故障停机 	
** 输　入:   	
** 输　出: 		 
** 注  释: 			
**-------------------------------------------------------------------------------------------------
** 作　者: 	 
** 日　期: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** 修改人:
** 日期:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void SER_STOP(void)         
{ 
	if(M_ChkFlag(SL_SERSTOP_FH)!=0)
	{
//------2015-11-3----------
//		if(M_ChkCounter(MAIN_LOOP.cnt_serstop,DELAY_SERSTOP)>0)		//2s 主断断开延时后再断定子接触器和主接触器
//		{
			M_ClrFlag(CL_MAINK);    		  						//断主接触器和滤波?
		   	M_ClrFlag(CL_PRE); 										//俣显こ涞缃哟テ?
		   	M_ClrFlag(CL_STATORK);    	  							//断定子接触器	
		   	M_ClrFlag(SL_CHARGEOK);          						//清预充电完成
			M_ClrFlag(SL_FAN_WORK);									//停止风机运行
		   	M_ClrFlag(SL_MPR_SYNOK);          						//清定子并网完成
		   	M_ClrFlag(SL_UDSTACOK);          						//清定子满足并网条件
		
			if(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(SL_IN1_MIANK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)==0 && M_ChkFlag(SL_IN1_STATORK)==0)
			{
				M_ClrFlag(SL_SERSTOP_FH);							//清步骤标志位
				M_ClrFlag(SL_SERIESTOPING);							//清 正在严重故障停机中 标志
			}
//		}
	}
	else
	{
	   	M_ClrFlag(CL_CBON);    		     							//清主断合闸指令
	   	M_ClrFlag(CL_CBLVC);         								//清欠压链断开，主断断开 
	   	M_ClrFlag(CL_CBFCON);            							//断开故障链
	  	M_ClrFlag(SL_OCS_NPRSTART);         						//清网侧运行指令
	  	M_ClrFlag(SL_OCS_EIN);         								//清主断闭合指令 201005atcpc
	  	M_ClrFlag(SL_OCS_SYSRUN);         							//清预充电指令	201005atcpc
		M_ClrFlag(SL_NPR_START);									//清网侧脉冲指令
		M_ClrFlag(SL_MPR_START);									//清机侧脉冲指令
	   	M_ClrFlag(SL_CBCLOSED);			 							//清主断闭合完成
	   	M_ClrFlag(SL_STEADYFB);          							//清Vdc稳定
		M_SetFlag(SL_SERSTOP_FH);									//置步骤标志位
		MAIN_LOOP.cnt_serstop=0;           							//清计数器	

		M_ClrFlag(SL_SYN_FH);										//清SYN步骤标志位
		M_ClrFlag(SL_SYN_S3);										//清SYN步骤标志位
		M_ClrFlag(SL_SYN_S2);										//清SYN步骤标志位
		M_ClrFlag(SL_SYN_S1);										//清SYN步骤标志位
		M_ClrFlag(SL_NORSTOPING);          							//清 谡Ｍ；? 标志
		M_ClrFlag(SL_NORSTOP_FH);									//清NORSTOP步骤标志位
		M_ClrFlag(SL_NORSTOP_S2);									//清NORSTOP步骤标志位
		M_ClrFlag(SL_NORSTOP_S1);									//清NORSTOP步骤标志位
		M_ClrFlag(SL_ERRSTOPING);									//清 正在一般故障停机中 标志
		M_ClrFlag(SL_ERRSTOP_FH);									//清ERRSTOP步骤标志位
		M_ClrFlag(SL_CBON_FH);										//清CBON步骤标志位
	}
}


/**************************************************************************************************
** 名称: ERR_STOP
** 功能描述: 1.5MW变流器系统逻辑控制-话愎收贤；? 	
** 输　入:   	
** 输　出: 		 
** 注  释: 			
**-------------------------------------------------------------------------------------------------
** 作　者: 	 
** 日　期: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** 修改人:
** 日期:	20121103改成与华创一榷现鞫显俣隙ㄗ咏哟テ?
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void ERR_STOP(void)         
{
	if(M_ChkFlag(SL_ERRSTOP_FH)!=0)
	{
//----------2013-11-3------------
//		if(M_ChkCounter(MAIN_LOOP.cnt_errstop,DELAY_ERRSTOP)>0)		//2s 主断断开延时后再断定子接触器和主接触器
//		{
			M_ClrFlag(CL_MAINK);    		  						//断主接触器和滤波器
		   	M_ClrFlag(CL_PRE); 										//再断预充电接触器
		   	M_ClrFlag(CL_STATORK);    	  							//断定子接触器	
		   	M_ClrFlag(SL_CHARGEOK);          						//清预充电完成
			M_ClrFlag(SL_FAN_WORK);									//停止风诵?
		   	M_ClrFlag(SL_MPR_SYNOK);          						//清定子并网完成
		   	M_ClrFlag(SL_UDSTACOK);          						//清定子满足并网条件
		
			if(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(SL_IN1_MIANK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)==0 && M_ChkFlag(SL_IN1_STATORK)==0)
			{
				M_ClrFlag(SL_ERRSTOP_FH);							//清步骤标志位
				M_ClrFlag(SL_ERRSTOP_S1);							//清步骤标志位
				M_ClrFlag(SL_ERRSTOPING);							//清 正在严重故障停机中 标?
			}
//		}
	}
	else if(M_ChkFlag(SL_ERRSTOP_S1)!=0)
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_epwmstop,DELAY_EPWMSTOP)>0)		//500ms
		{
			M_ClrFlag(CL_CBON);    		     							//清主断合闸指令
	   		M_ClrFlag(CL_CBLVC);         								//清欠压链断开，主断断开 
	   		M_ClrFlag(CL_CBFCON);            							//断开故障链
	   		M_ClrFlag(SL_CBCLOSED);			 							//清主断闭合完成			
			M_SetFlag(SL_ERRSTOP_FH);									//置步骤标志位		
		}		
		MAIN_LOOP.cnt_errstop=0;           							//清计数器	
	}
	else
	{
	  	M_ClrFlag(SL_OCS_NPRSTART);         						//清网侧运行指令
	  	M_ClrFlag(SL_OCS_EIN);         								//清主断闭合指令 201005atcpc
	  	M_ClrFlag(SL_OCS_SYSRUN);         							//清预充电指令	201005atcpc
		M_ClrFlag(SL_NPR_START);									//清网侧脉冲指令
		M_ClrFlag(SL_MPR_START);									//清机侧脉冲指令
	   	M_ClrFlag(SL_STEADYFB);          							//清Vdc稳定
		MAIN_LOOP.cnt_errstop=0;           							//清计数器	
		MAIN_LOOP.cnt_epwmstop=0;
		M_SetFlag(SL_ERRSTOP_S1);									//置步骤标志位			

		M_ClrFlag(SL_SYN_FH);										//清SYN步骤标志位
		M_ClrFlag(SL_SYN_S3);										//清SYN步骤标志位
		M_ClrFlag(SL_SYN_S2);										//清SYN步骤标志位
		M_ClrFlag(SL_SYN_S1);										//清SYN步骤标志位
		M_ClrFlag(SL_NORSTOPING);          							//清 正在正常停机中 标志
		M_ClrFlag(SL_NORSTOP_FH);									//清NORSTOP步骤标志位
		M_ClrFlag(SL_NORSTOP_S2);									//清NORSTOP步骤标志位
		M_ClrFlag(SL_NORSTOP_S1);									//清NORSTOP步骤标志位
	} 
/*	if(M_ChkFlag(SL_ERRSTOP_FH)!=0)
	{
		M_ClrFlag(CL_PRE); 											//断预充电接触器
		M_ClrFlag(CL_MAINK);    		  							//断主接触器和滤波器
		M_ClrFlag(SL_CHARGEOK);          							//清预充电完成

		if(M_ChkFlag(SL_IN1_MIANK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)==0)
		{
			M_ClrFlag(SL_FAN_WORK);									//停止风机运行
			if(M_ChkFlag(SL_PCOCS_CBOFF)!=0 && M_ChkFlag(SL_OCS_EIN)==0)//PC手动控制断开CB 201005atcpc
			{
			   	M_ClrFlag(CL_CBON);    		     					//清主断合闸指令
			   	M_ClrFlag(CL_CBLVC);         						//清欠压链断开，主断断开 
			   	M_ClrFlag(CL_CBFCON);            					//断开故障链
				M_ClrFlag(SL_CBCLOSED);			 					//清主断闭合完成
				M_ClrFlag(SL_PCOCS_CBOFF);            				//清断开指令
				M_ClrFlag(SL_CBON_FH);								//清CBON步骤标志位
			} 
			M_ClrFlag(SL_ERRSTOP_FH);								//清步骤标志位
			M_ClrFlag(SL_ERRSTOPING);								//清 正在一般故障停机中 标志
		}	
	}
	else
	{
	  	M_ClrFlag(SL_OCS_EIN);         								//清主断闭合指令
	  	M_ClrFlag(SL_OCS_SYSRUN);         							//清预充电指令 201005atcpc
	  	M_ClrFlag(SL_OCS_NPRSTART);         						//清网侧运行指令
		M_ClrFlag(SL_NPR_START);									//清网侧脉冲指令
		M_ClrFlag(SL_MPR_START);									//清机侧脉冲指令
		M_ClrFlag(CL_STATORK);    	  								//断定子接触器	
		M_ClrFlag(SL_UDSTACOK);          							//清定子满悴⑼跫?
		M_ClrFlag(SL_MPR_SYNOK);          							//清定子并网完成

//		if(M_ChkFlag(SL_NPR_PWMOUT)==0 && M_ChkFlag(SL_MPR_PWMOUT)==0 && M_ChkFlag(SL_IN1_STATORK)==0)
		if(M_ChkFlag(SL_NPR_PWMOUT)==0 && M_ChkFlag(SL_MPR_PWMOUT)==0)//20100510
		{
	   		M_ClrFlag(SL_STEADYFB);          							//清Vdc稳定
			M_SetFlag(SL_ERRSTOP_FH);								//置步骤标志位
			MAIN_LOOP.cnt_errstop=0;           						//清计数器	
		}

		M_ClrFlag(SL_SYN_FH);										//清SYN步骤标志位
		M_ClrFlag(SL_SYN_S3);										//清SYN步骤标志位
		M_ClrFlag(SL_SYN_S2);										//清SYN步骤标志位
		M_ClrFlag(SL_SYN_S1);										//清SYN步骤标志位
		M_ClrFlag(SL_NORSTOPING);          							//清 正在正常停机中 标志
		M_ClrFlag(SL_NORSTOP_FH);									//清NORSTOP步骤标志位
		M_ClrFlag(SL_NORSTOP_S2);									//清NORSTOP步骤标志位
		M_ClrFlag(SL_NORSTOP_S1);									//清NORSTOP步骤标志位
	}
*/
}

/**************************************************************************************************
** 名称: CB_ON
** 功能描述: 1.5MW变流器系统逻辑控制-主断闭合 	
** 输　入:   	
** 输　出: 		 
** 注  释: 			
**-------------------------------------------------------------------------------------------------
** 作　者: 	 
** 日　期: 2010-4-27		
**-------------------------------------------------------------------------------------------------
** 修改人:
** 日期:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
void CB_ON(void)         
{ 
	if(M_ChkFlag(SL_CBON_FH)!=0)
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_cbon,DELAY_CBON)>0)		//1s 延时发主断合闸指令
		{
		   	M_SetFlag(CL_CBON);    		     					//发主断合闸指令
		}

		if(M_ChkFlag(SL_IN1_CBSTS)!=0)
		{
	   		M_SetFlag(SL_CBCLOSED);			 					//置主断闭合完成
		   	M_ClrFlag(CL_CBON);    		     					//清主断合闸指令
			M_ClrFlag(SL_CBON_FH);								//清步骤标志位
		}	
	}
	else
	{
	   	M_SetFlag(CL_CBLVC);         							//闭合主断欠压?
	   	M_SetFlag(CL_CBFCON);            						//闭合故障链
		M_SetFlag(SL_CBON_FH);									//置步骤标疚?
		MAIN_LOOP.cnt_cbon=0;           						//清计数器	
	}
}


/**************************************************************************************************
** 名称: CONTACTOR_TEST
** 功能描述: 1.5MW变流器系统逻辑控制-接触器测试 	
** 输　入:   	
** 输　出: 		 
** 注  释: 			
**-------------------------------------------------------------------------------------------------
** 作　者: 	 
** 日　期: 2010-5-22		
**-------------------------------------------------------------------------------------------------
** 修改?:
** 日期:	
**-------------------------------------------------------------------------------------------------
***********************************************************************************************/ 
/*	else if(_STDBY8==7 && M_ChkFlag(SL_RUN)==0 && M_ChkFlag(SL_IN1_CBSTS)==0 && AD_OUT_UDC<50) //不发脉冲,CB断开,Vdc没电情况下允许测试							//
	{
		CONTACTOR_TEST();										//执行接触器测试操作 201005atcpc
	}


void CONTACTOR_TEST(void)         								//systest 201005atcpc
{ 
	M_ClrFlag(CL_CBON);    		     							//清主断合闸指令
   	M_ClrFlag(CL_CBLVC);         								//清欠压链峡鞫隙峡? 
   	M_ClrFlag(CL_CBFCON);            							//断开故障链
  	M_ClrFlag(SL_OCS_NPRSTART);         						//清网嘣诵兄噶?
	M_ClrFlag(SL_NPR_START);									//清网侧脉冲指令
	M_ClrFlag(SL_MPR_START);									//清机侧脉冲指令
   	M_ClrFlag(SL_CBCLOSED);			 							//清主断闭合完成
   	M_ClrFlag(SL_CHARGEOK);          							//清预充电完成
	M_ClrFlag(SL_FAN_WORK);										//停止风机运行
  	M_ClrFlag(SL_MPR_SYNOK);          							//清定子并网完成
   	M_ClrFlag(SL_UDSTACOK);          							//清定子满足并网条件

   	if(M_ChkFlag(SL_OCS_SYSRUN)!=0)								//外部控制，为1才闭合PREC/MC/MF
   	{
   		M_SetFlag(CL_MAINK);    		  						//断主接触器和滤波器
  		M_SetFlag(CL_PRE); 										//再断预充电接触器
  	}
	else
	{
   		M_ClrFlag(CL_MAINK);    		  						//断主接触器和滤波器
  		M_ClrFlag(CL_PRE); 										//俣显こ涞缃哟テ?	
	}

    if(M_ChkFlag(SL_OCS_OPENSTAC)==0)							//外部控制，为0才允许闭合STAC
     		M_SetFlag(CL_STATORK);  							//上位机允许并网,闭合定子接触器                            					
	else	M_ClrFlag(CL_STATORK);								//上位机禁止并网,断开定子接触器	        

}

*/
/////////////////no more///////////////////
