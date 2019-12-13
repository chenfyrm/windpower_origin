/****************************************Copyright (c)**************************************************
**                       		     北	京	交	通	大	学
**                                         电气工程学院
**                                         604实验室
**
**                              
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: user_main.c
**创   建   人: 
**最后修改日期: 
**描        述: 右玉1.5MW双馈风电机组并网变流器控制软件主程序----201005atcpc-201007atbjtu
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
/*---------------------自带头文件-----------------------------*/
#include "C28x_FPU_FastRTS.h"
#include "DSP2833x_Device.h"     				// DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   				// DSP2833x Examples Include File
/*--------------------自定义头文件----------------------------*/
#include "user_header.h"  						//变量常量定义
#include "user_macro.h"							//宏函数
#include "user_database.h"						//数据库   
#include "user_interface.c"						//接口层
#include "user_work.c"							//工作控制
#include "math.h"

/*-----------------------中断声明-----------------------------*/
#pragma CODE_SECTION(CpuTimer0Isr, "ramfuncs");
interrupt void CpuTimer0Isr(void);				//主定时器中断			
interrupt void EPWM1_TZ1_Isr(void);				//TZ1中断
interrupt void EPWM2_TZ2_Isr(void);				//TZ2中断
/*-----------------------函数声明-----------------------------*/

void Protect(void);

#pragma CODE_SECTION(Scout, "ramfuncs");
void Scout(void);
void et_relay_N(void);
void et_relay_M(void);
void CntCtrl(void);
void SysCtrl(void);
void Bank(void);
void Display(void);
void BANK_Datasave(void);					
void ERROR_Datasave(void);
#pragma CODE_SECTION(ACrowbar, "ramfuncs");					
void ACrowbar(void);					

// These are defined by the linker (see F28335_flash.cmd)
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
/*********************************************************************************************************
** 函数名称: main
** 功能描述: 系统初始化,主循环
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
void main(void)
{
//--------------------------------系统初始化--------------------------------
	InitSysCtrl();
//---------------------------------IO初始化---------------------------------
	InitGpio();	

	DINT;
   	InitPieCtrl();
	IER = 0x0000;
	IFR = 0x0000;
	InitPieVectTable();
//--------------------------------外设初始化--------------------------------
	InitPeripherals(); 	
//---------------------------------写FLASH程序------------------------------
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	InitFlash();
//--------------------------------工作初始化--------------------------------
	InitWork();	
//---------------------------用户自定义程序的初始化-------------------------
	InitEeprom();

//-----MagnetCurve2013-12-13-ZZJ------------------
//-----读取EEROM内部存储的励磁参数----------------------

   	MC_DATA.y[0] = _SC_EXISTY1 * 0.001;
   	MC_DATA.y[1] = _SC_EXISTY1 * 0.001;
	MC_DATA.y[2] = _SC_EXISTY2 * 0.001;
	MC_DATA.y[3] = _SC_EXISTY3 * 0.001;
	MC_DATA.y[4] = _SC_EXISTY4 * 0.001;
	MC_DATA.y[5] = _SC_EXISTY5 * 0.001;
	MC_DATA.y[6] = 1.4088;
	MC_DATA.y[7] = 1.5904;
	MC_DATA.y[8] = 1.8233;
	MC_DATA.y[9] = 2.1123;
	MC_DATA.y[10] = 2.4714;

	MC_DATA.x[0] = 0;
	MC_DATA.x[1] = 50;
	MC_DATA.x[2] = 75;
	MC_DATA.x[3] = 90;
	MC_DATA.x[4] = 100;
	MC_DATA.x[5] = 110;
	MC_DATA.x[6] = 115;
	MC_DATA.x[7] = 120;
	MC_DATA.x[8] = 125;
	MC_DATA.x[9] = 130;
	MC_DATA.x[10] = 135;

	_MC_OK = 1;
//--------2013-12-13--------

//	InitRtimer();										//实时时钟初始化只在芯片需要初始化时才进行
//-------------------------------中断地址初始化-----------------------------
	EALLOW;  
	PieVectTable.TINT0       = &CpuTimer0Isr;		  	//定时器T0周期中断
	PieVectTable.EPWM1_TZINT = &EPWM1_TZ1_Isr;      	//TZ1 功率保护中断 
	PieVectTable.EPWM2_TZINT = &EPWM2_TZ2_Isr;      	//TZ2 功率保护中断 

	EDIS;    
	
//------------------------------加载中断使能寄存器--------------------------
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;		//CPUTIMER0周期中断
    										//INT1(CPU)/INT1.7(PIE)
	PieCtrlRegs.PIEIER2.bit.INTx1 = 1;		//epwm1.tz
											//INT2(CPU)/INT2.1(PIE)
	PieCtrlRegs.PIEIER2.bit.INTx2 = 1;		//epwm2.tz 																	//INT1(CPU)/INT1.1(PIE)
												
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;  	//使能PIE   
    
   	IER |= (M_INT1|M_INT2);					//加载总中断屏蔽寄存器
    
    EINT;   								//开总中断
    
//-------------------------------定时器开始工作-------------------------------
	CpuTimer0Regs.TCR.all = 0x4001; // Use write-only instruction to set TSS bit = 0//	M_StartCpuTimer0();												//主定时器工作

//----------------------------------主循环---------------------------------------------------------
	for(;;) 
	{				

//----------------------------------每次运行----------------------------------
		EeCtrl();							//eeprom控制
		Sci485Ctrl(); 
//		Sci_canopenrx();					//系统输入读取	

//-----------------------------一级循环5ms运行一次----------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt1,DELAY_MAIN1)>=0)		
		{
			MAIN_LOOP.cnt1=0;				//清空计数器
			Input();						//系统输入读取
	        M_NotFlag(SL_WATCHDOG);		    //DSP软件看门狗 20100401at27
			Output();						//系统输出指示
		}

//-----------------------------二级循环10ms运行一次---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt2,DELAY_MAIN2)>=0)
		{
			MAIN_LOOP.cnt2=0;
			if(M_ChkFlag(SL_CODEOK)!=0)		//在eeprom正常的情况下
			{
			     Give();					//运行指令给定
			}
		}

//-----------------------------三级循环20ms运行一次---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt3,DELAY_MAIN3)>=0)
		{
			MAIN_LOOP.cnt3=0;
			SysCtrl();						//主控程序
		}

//-----------------------------四级循环50ms运行一次---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt4,DELAY_MAIN4)>=0)
		{
			MAIN_LOOP.cnt4=0;
		    Bank();							//显示控制
		}

//----------------------------五级循环100ms运行一次---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt5,DELAY_MAIN5)>=0)
		{					
			MAIN_LOOP.cnt5=0;
			if((M_ChkFlag(SL_POWERON)!=0)&&(M_ChkCounter(MAIN_LOOP.cnt_poweron,DELAY_POWERON)>0))
			{
				M_ClrFlag(SL_POWERON);
				ClrPdpint();						//PDPINT中断清空	
				EnPdpint();							//PDPINT使能中断
			}			
//20130801
			if(M_ChkFlag(SL_ENCODEPOS_OK)==0 && M_ChkFlag(SL_ECPOSSEARCH)!=0 && M_ChkFlag(SL_MPR_START)!=0)	//20130604
			{
				*FUNC[20].para_add = - QEPDATA.encodpos * 1000;	//修改RAM和EEROM

				EEPROM.mcode=20;			//写入2字节数据
				M_SetFlag(SL_EEASK_MCODE);			//设EEPROM修改功能码请求标志
			}
		}
//-----------------------------六级循环1000ms运行一次---------------------------
		if(M_ChkCounter(MAIN_LOOP.cnt6,DELAY_MAIN6)>=0)
		{
			MAIN_LOOP.cnt6=0;

			if(M_ChkFlag(SL_CODEOK)!=0)		
				RtRead();							//在eeprom正常的情况下//读取实时时钟,很耗时，要13ms.20090801,CPC				
//20121103										
//			if(M_ChkCounter(MAIN_LOOP.cnt_senszfstdy,DELAY_SENSZFSTDY)>=0)
//				M_SetFlag(SL_SENSZFSTDY);   		//延迟时间到后置零漂滤波稳定标志位
			if(MAIN_LOOP.cnt_gridok_last!=65535)		MAIN_LOOP.cnt_gridok_last++;	
			if(MAIN_LOOP.cnt_hvlv_detect3!=65535)		MAIN_LOOP.cnt_hvlv_detect3++;	
			if(MAIN_LOOP.cnt_uacover!=65535)			MAIN_LOOP.cnt_uacover++;	

		}
	}  					
} 


/*********************************************************************************************************
** 函数名称: CpuTimer0Isr
** 功能描述: 主定时器周期卸?(0.04ms)
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
interrupt void CpuTimer0Isr(void)
{
    Uint16 i,k;
    
	M_SetFlag(SL_PHASEA);            							//测量CPU占有率,测量DSP板上T1端子
	*OUT3_ADDR = _OUT3_DATA;
		
	if(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(SL_POWERON)==0)   //在合主断前且上电瓿珊笸瓜嘈蚣觳庾雍? cap5-ubc   cap6-uab
	{
	  PhaseOrderChk();        
	} 
//	QepEncodPos(); 					
    CapQepCtrl();  		  										//测网压相位，网压频率，坐标正、反变换的角度
   	Ad8364Ctrl();             									//有传感器和互感器AD采样和转换
	ACrowbar();													//LVRT20100112
    PLLCtrl();		//201205LVRTatZB调换了角度积分的位置20120527												//201011PLL
	QepEncodPos();		//根据zb经验调换了位置20121103

    NPR_CONTROL();            									//网侧变流器控制算法
    MPR_CONTROL();            									//机侧变流器控制算法

	et_relay_N();												//网侧过流反时限计算
	et_relay_M();												//机侧过流反时限计算

	Da5344Manage();          	//DA output

//	testtheta = testtheta + 1;									//三角波测试信号
//	if(testtheta>=3721)	testtheta= -1879;

//---------------------------------分时脉冲计算--------------------------------
	MAIN_LOOP.pulse++;
	if(MAIN_LOOP.pulse>=10)		MAIN_LOOP.pulse=0;
	
//--20级分时脉冲
	switch(MAIN_LOOP.pulse)
	{

//--定时器累加,慢速AD,保护值计算,故障
		case 0:
		{
			Protect();			   //protect calculation		
			Scout();			   //故障保护
			BANK_Datasave();		//0.4ms一次
			break;
		}
		case 1:
		{	
			PwmDrive();	 		   //输出脉冲控制  				
		    CntCtrl(); 				//计数1ms计一个数，定时器中断周期为200us
			ERROR_Datasave();		//0.4ms一次
			Sci_canopenrx();		//系统输入读取120507
			break;
		}
		case 2:
		{
			Protect();			   //protect calculation		
			Scout();			   //故障保护
			BANK_Datasave();		//0.4ms一次
			break;
		}
		case 3:
		{	
			PwmDrive();			   //输出脉冲控制				
			RunCtrl();			   //给定积分
			ERROR_Datasave();		//0.4ms一次
			break;
		}
		case 4:
		{	
			Protect();			   //protect calculation	
			Scout();			   //故障保护
			BANK_Datasave();		//0.4ms一次
			break;
		}
		case 5:
		{	
			PwmDrive();			   //输出脉冲控制				
			Display();
			ERROR_Datasave();		//0.4ms一次
			break;
		}
		case 6:
		{	
			CntCtrl(); 	           //计数，1ms计一个数，定时器中断周期为200us
			Protect();			   //protect calculation	
			Scout();			   //故障保护
			BANK_Datasave();		//0.4ms一次
			Sci_canopenrx();		//系统输入读取120507
			break;
		}
		case 7:
		{
			PwmDrive();			   //输出脉冲控制				
			ERROR_Datasave();	   //0.4ms一次
			break;
		}
		case 8:
		{	
			Protect();												//slow AD, protect calculation	
			Scout();												//故障保护
			BANK_Datasave();		//0.4ms一次
			break;
		}
		case 9:
		{	
			PwmDrive();			   //输出脉冲控制				
			RunCtrl();			   //给定积分
			ERROR_Datasave();		//0.4ms一次
			break;
		}
//--------------------------------------------------------------------------------------------------
		default:
			break;
	}

//--应答中断20091109atzy
//	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	

//canopen
//-----------------------------canopen运行一次--120507----------------------------
	if(M_ChkCounter(MAIN_LOOP.canopen_tx,DELAY_CANOPENTX)>=0)    //250  6ms		
	{
		MAIN_LOOP.canopen_tx=0;				//清空计数器
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

			Sci_canopentx();				//12ms执行一次
			M_SetFlag(SL_CANOPENTX);
			M_ClrFlag(SL_CANOPENTX_FLAG);		
		}
		else
		{
			Sci_canopentx_read();			//12ms执行一次
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
		if(SciaRegs.SCIFFTX.bit.TXFFST == 0)						//发送完成?
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
//----------------------------------------------//121014byljd 根据施韵试验，在T0中断加入及时释放总线程序
	if(M_ChkFlag(SL_TX)!=0)						//Sci.txb寄存器数据发送完成后，由于主循环有其他程序忙，
 	{											//无法进入Sci485Ctrl子程序释放通讯总线，导致通讯中断，
		if(M_ChkFlag(SL_TXLONGDATA)!=0)			//将总线释放子程序移至T0中断中及时释放	120625
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
		else									//发送数据完成?
		{
			longtx_num = 15;

			if(ScibRegs.SCIFFTX.bit.TXFFST == 0)						//发送完成?Transmit FIFO is empty.
			{
				SCI.cnt_sciover=0;										//清除发送/接收超时定时器
				if(ScibRegs.SCICTL2.bit.TXEMPTY==1)						//发送寄存器为空?Transmitter buffer and shift registers are both empty
		 		{
					M_ClrFlag(SL_TX);
					Sci485_RxInit();									//485接收初始化
				}
			}

		}
 	}


//------------------------------------------------------------------------

    M_ClrFlag(SL_PHASEA);								//测量CPU占有率,测量DSP板上T1端子
    *OUT3_ADDR = _OUT3_DATA;

//--应答中断20091109atzy
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	
}

/*********************************************************************************************************
** 函数名称: EPWM1_TZ1_Isr
** 功能描述: 功率保护中断
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
interrupt void EPWM1_TZ1_Isr (void)
{
//----------------------------------------------//处理程序

	Disepwmio_NPR();
	Disepwmio_MPR();
    DisPdpint();                
	M_SetFlag(SL_PDPINTA);
//----------------------------------------------//应答中断
	ClrPdpint();
	M_SetFlag(SL_DISPLAY3);    //LED3红灯亮
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}
/*********************************************************************************************************
** 函数名称: EPWM2_TZ2_Isr
** 功能描述: 功率保护中断
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
interrupt void EPWM2_TZ2_Isr (void)
{
//----------------------------------------------//处理程序
	Disepwmio_NPR();
	Disepwmio_MPR();
    DisPdpint();     
	M_SetFlag(SL_PDPINTB);
//----------------------------------------------//应答中断
	ClrPdpint();
	M_SetFlag(SL_DISPLAY3);    //LED3红灯亮
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}


/*********************************************************************************************************
** 函数名称: ACrowbar
** 功能描: ActiveCrowbar控制
** 输　入: 	 
** 输  出:   
** 注  释: 	 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void ACrowbar(void)
{
    float NGS_U_D,NGS_U_Q;
//-------dc-chopper----------------开关量备用7 
//----2016-1-13----增加斩波限制条件------
	if((_COMMAND2&0x0080)==0 && M_ChkFlag(SL_ERRSTOP)==0 && M_ChkFlag(SL_SERIESTOP)==0)
	{
		if(M_ChkFlag(SL_LV_STATE)!=0)
		   M_SetFlag(CL_CHOPEN);//DC-CHOPPER驱动使能信号，拉高使能，开通关断还与底板滞环VDC2结果有关
		else if(M_ChkFlag(SL_HV_STATE)!=0)
		{
		   if(PRO.udc >1300)
		   M_SetFlag(CL_CHOPEN);
		   else if(PRO.udc <1260)
		   {
			 M_ClrFlag(CL_CHOPEN);
		   }		   
		}
		else
		{
			M_ClrFlag(CL_CHOPEN);
		}		
	}	
	else
		M_ClrFlag(CL_CHOPEN);

	if((_COMMAND2&0x0800)!=0)	//115.11备用11-斩波测试 2013-12-6ZZJ
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_chopper,DELAY_CHOPPER)<=0) M_SetFlag(CL_CHOPTEST);
		else 													 M_ClrFlag(CL_CHOPTEST);
	}
	else
	{
		M_ClrFlag(CL_CHOPTEST);		//DC-CHOPPER驱动使能信号，拉低禁止
		MAIN_LOOP.cnt_chopper=0;
	}

	*OUT2_ADDR = _OUT2_DATA; //迅速输出

//-------------------LV 检测电网跌落-----------------
	
//---2015-11-2---低穿检测增加故障、运行等前提条件----
	if(M_ChkFlag(SL_IN1_MIANK)!=0 && M_ChkFlag(SL_SERIESTOPING)==0 && M_ChkFlag(SL_SERIESTOP)==0 && M_ChkFlag(SL_ERRSTOPING)==0 && M_ChkFlag(SL_ERRSTOP)==0 && M_ChkFlag(SL_MPR_SYNOK)!=0)	
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_hvlv_detect,DELAY_HVLV_DETEC)>=0)			//1s
			M_SetFlag(SL_HVLV_DETEC);
	}
	else	
	{
		MAIN_LOOP.cnt_hvlv_detect=0;
		M_ClrFlag(SL_HVLV_DETEC);
	}

//	NGS_Udq    = sqrt(TRS_NGS_U.dpll * TRS_NGS_U.dpll + TRS_NGS_U.qpll * TRS_NGS_U.qpll); //计算网压幅值-仅适用于正序
//	NGS_Udq_lv = sqrt(TRS_NGS_U.dnpr * TRS_NGS_U.dnpr + TRS_NGS_U.qnpr * TRS_NGS_U.qnpr);  //计算网压幅值-仅适用于正序-跌落

	NGS_Udq = sqrt(TRS_NGS_U.dflt * TRS_NGS_U.dflt + TRS_NGS_U.qflt * TRS_NGS_U.qflt);  //20110326计算网压幅值-仅适用于正序-跌落

//  由PLL输出的NGS_Udq_p替代BJTULVRT201204

	NGS_U_D    = 0.3333333 * (2 * AD_OUT_NGS_U.a - AD_OUT_NGS_U.b - AD_OUT_NGS_U.c);	//用静止坐标分量计算网压幅值
	NGS_U_Q    = SQRT3_3 * (AD_OUT_NGS_U.b - AD_OUT_NGS_U.c);
	TRS_NPR_U.amp = sqrt(NGS_U_D * NGS_U_D + NGS_U_Q * NGS_U_Q);   //20110328
	DataFilter(0.443,&TRS_NPR_U.ampflt,TRS_NPR_U.amp); 			//c=0.4,Ts=200us,fh=1.2kHz,滤掉开关频率次

	if(M_ChkFlag(SL_LV_QWORKING)==0||M_ChkFlag(SL_HV_QWORKING)==0)	DataFilter(0.998,&NGS_Udq_pflt,NGS_Udq_p);		//100ms	20130303

//-----计算无功补偿系数kq-----BJTULVRT201204
	if(M_ChkFlag(SL_UNBALANCE)==0)
//		GRD_Ut = NGS_Udq / GRD_UN;
		GRD_Ut = NGS_Udq / NGS_Udq_p_ex;	//20130301
	else
//		GRD_Ut = NGS_Udq_p / GRD_UN;
		GRD_Ut = NGS_Udq_p / NGS_Udq_p_ex;	//20130301
//	if(GRD_Ut <= 0.9)	kq = 1.5 * (1 - NGS_Udq_p / GRD_UN);	//201205LVRTatZB
//	else				kq = 0;		//20120531 201205LVRTatZB
//--------20130225--------
	if(GRD_Ut <= 0.94 )							//20130124	20130303
	{
		kq = 1.5 * (1.05 - NGS_Udq_p / NGS_Udq_p_ex);	//20130301
		MAIN_LOOP.cnt_hvlv_kq = 0;				//20130124
	}
	else if(GRD_Ut >= 1.05 )
	{
		kq = 1.5 * (NGS_Udq_p / NGS_Udq_p_ex-1.05);	
		MAIN_LOOP.cnt_hvlv_kq = 0;				
	}
	else if(M_ChkCounter(MAIN_LOOP.cnt_hvlv_kq,30)>0)
		kq = 0;		//电压恢复正常后无功命令延时30ms清除

	if(kq<0) kq=0;
//------------------------
	if(M_ChkFlag(SL_HVLV_DETEC)!=0)	//20120601night
	{
		if(M_ChkFlag(SL_LV_STATE)!=0||(M_ChkFlag(SL_HV_STATE)!=0))	MAIN_LOOP.cnt_gridok_last=0;//恢复后计时
		else 		MAIN_LOOP.cnt_gridfault_last = 0; //故障后计时

		if(M_ChkCounter(MAIN_LOOP.cnt_gridok_last,DELAY_EQUIP_CD)>0)	//进入低穿状态的判断，低穿状态恢复后必须间隔5min才能第二次进入低穿状态
		{
//			if(M_ChkFlag(SL_LV_STATE)==0 && (TRS_NPR_U.ampflt<(0.91 * GRD_UN)))
//			if(M_ChkFlag(SL_LV_STATE)==0 && (TRS_NPR_U.ampflt<(0.91 * NGS_Udq_p_ex)))		//20130301
//			if(M_ChkFlag(SL_LV_STATE)==0 && (TRS_NPR_U.ampflt<(_stdby01 * NGS_Udq_p_ex)))		//20130309
			//以下根据幅度检测，负序情况，幅度震荡周期10ms，可保证10ms内检测到故障
			if(M_ChkFlag(SL_LV_STATE)==0 && (TRS_NPR_U.ampflt<(0.6 * NGS_Udq_p_ex)))//2013-12-6小于0.6立刻置位
			{
				gridfault_flag_flt1++;
				if(gridfault_flag_flt1>=5)
				{
					M_SetFlag(SL_LV_STATE);
					if((_STDBY9&0x0002)==0)		M_SetFlag(CL_ZKHVLVRT);	
					gridfault_flag_flt1=0;
				}
			}
			else
			{
				gridfault_flag_flt1=0;
			}
			
			if(M_ChkFlag(SL_HV_STATE)==0 && (TRS_NPR_U.ampflt>(1.2 * NGS_Udq_p_ex)))
			{
				gridfault_flag_flt2++;
				if(gridfault_flag_flt2>=5)
				{
					M_SetFlag(SL_HV_STATE);
					gridfault_flag_flt2=0;
				}
			}
			else
			{
				gridfault_flag_flt2=0;
			}
			
			if(M_ChkFlag(SL_LV_STATE)==0 && (TRS_NPR_U.ampflt<(0.9 * NGS_Udq_p_ex)))//2013-12-6小于0.91延时1ms置位
			{
				gridfault_flag_flt3++;
				if(gridfault_flag_flt3>=10)
				{
					M_SetFlag(SL_LV_STATE);
					if((_STDBY9&0x0002)==0)		M_SetFlag(CL_ZKHVLVRT);	
					gridfault_flag_flt3=0;
				}
			}
			else
			{
				gridfault_flag_flt3=0;
			}
			
			if(M_ChkFlag(SL_HV_STATE)==0 && (TRS_NPR_U.ampflt>(1.10* NGS_Udq_p_ex)))//2013-12-6小于0.91延时1ms置位
			{
				gridfault_flag_flt4++;
				if(gridfault_flag_flt4>=10)
				{
					M_SetFlag(SL_HV_STATE);
					gridfault_flag_flt4=0;
				}
			}
			else gridfault_flag_flt4=0;
		}
		else 
		{
			gridfault_flag_flt1=0;
			gridfault_flag_flt2=0;
			gridfault_flag_flt3=0;
			gridfault_flag_flt4=0;	
		}
//		if(M_ChkFlag(SL_LV_STATE)!=0 && (NGS_Udq_p > (0.92 * GRD_UN)))
		if((M_ChkFlag(SL_LV_STATE)!=0)&& (NGS_Udq_p > (0.92 * NGS_Udq_p_ex)))		//恢复正常态的判断
		{		
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_rcv,DELAY_HVLVRCV)>0)			//10ms 负序震荡周期
			{
				M_ClrFlag(SL_LV_STATE);	

				if((_STDBY9&0x0002)==0)		M_ClrFlag(CL_ZKHVLVRT);		//20121107

				if(M_ChkCounter(MAIN_LOOP.cnt_gridfault_last,50)<=0)		//误触发无需延时可立即重新置位20130303
				{
					if(NGS_Udq_n2p < 2.5)	MAIN_LOOP.cnt_gridok_last = 20000;						//应大于DELAY_EQUIP_CD
					M_SetFlag(SL_LV_CLRERRAM);		//清除RAM内波形		20130306		
					M_ClrFlag(SL_LV_QWORKING);
					M_ClrFlag(SL_LV_STRICTLV);
				}
			}
		}
		else	MAIN_LOOP.cnt_lv_rcv = 0;

		if((M_ChkFlag(SL_HV_STATE)!=0)&& (NGS_Udq_p < (1.08 * NGS_Udq_p_ex)))		//恢复正常态的判断
		{		
			if(M_ChkCounter(MAIN_LOOP.cnt_hv_rcv,DELAY_HVLVRCV)>0)			//10ms 负序震荡周期
			{
				M_ClrFlag(SL_HV_STATE);	

				if(M_ChkCounter(MAIN_LOOP.cnt_gridfault_last,50)<=0)		//误触发无需延时可立即重新置位20130303
				{
					if(NGS_Udq_n2p < 2.5)	MAIN_LOOP.cnt_gridok_last = 20000;						//应大于DELAY_EQUIP_CD
					M_SetFlag(SL_LV_CLRERRAM);		//清除RAM内波形		20130306
					M_ClrFlag(SL_HV_QWORKING);			
				}
			}
		}
		else	MAIN_LOOP.cnt_hv_rcv = 0;		
	}
	else	
	{
		M_ClrFlag(SL_LV_STATE);	
		M_ClrFlag(SL_HV_STATE);	
		if((_STDBY9&0x0002)==0)		M_ClrFlag(CL_ZKHVLVRT);		//20121107
		MAIN_LOOP.cnt_lv_rcv = 0;
		MAIN_LOOP.cnt_hv_rcv = 0;
		MAIN_LOOP.cnt_gridok_last=0;
		gridfault_flag_flt1=0;
		gridfault_flag_flt2=0;
		gridfault_flag_flt3=0;
		gridfault_flag_flt4=0;
		MAIN_LOOP.cnt_gridfault_last = 0;
	} 
//--------20130225--------
    if(M_ChkFlag(SL_LV_STATE)!=0)		//20130128
	{
	  	if(M_ChkCounter(MAIN_LOOP.cnt_lv_dlrecmstop,3000)>0)	//3s 每次主动封停脉冲的间隔时间为3s	20130221
	  	{
			if(M_ChkCounter(MAIN_LOOP.cnt_gridfault_last,500)>=0)	//由于是500ms后，避开了跌落瞬间，只考察回复时
			{
//				if(((NGS_Udq_p - NGS_Udq_p_lv) > 55) && (NGS_Udq_p_lv < (0.85 * NGS_Udq_p_ex)))	//3ph-75,2ph-50 20130205
				if(((NGS_Udq_p - NGS_Udq_p_lv) > 55) && ((NGS_Udq_p_lv < (0.82 * NGS_Udq_p_ex))||(NGS_Udq_n2p_lv > 19)))	//3ph/2ph-50及以下 20130301 xly当前19为33%
				{
					M_SetFlag(SL_LV_RECMSTOP);					
//					M_SetFlag(SL_LV_RECNSTOP);		//2013-12-6ZZJ取消恢复瞬间主动封网侧脉冲
				}
			}
		}
	}
	else
	{
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
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scrkeepon,_stdby06)>=0)	
			M_ClrFlag(SL_LV_SCRKEEPON);//与后面切出程序配合，在不对称深度跌落后最多允许SCR持续投入时间为604ms，见后面程序
		}
	}
	else
	{
		M_ClrFlag(SL_LV_SCRKEEPON);
		MAIN_LOOP.cnt_lv_scrkeepon = 0;
	}
//------------------------


/*20120601night
 	if((_COMMAND2&0x0020)!=0)	//开关量备用5  201011LVRT
	{
		LV_A1 = 0.9; 			//E.on2003标准
		LV_A2 = 0.15;
		LV_T1 = 625;
		LV_T2 = 3000;
	}
	else						//默认为电科院的国家标准
	{
		LV_A1 = 0.9; //_stdby02  testing
		LV_A2 = 0.1; //放底些，别轻易保护 
		LV_T1 = 625;
		LV_T2 = 2000;	
//		LV_A2 = 0;			////放宽条件,LV实验不停机 20110326
//		LV_T1 = 1500;
//		LV_T2 = 4000;	
	}

	if(M_ChkFlag(SL_HVLV_DETEC)!=0)
	{
//		if(M_ChkFlag(SL_HVLV_DETEC)!=0 && NGS_Udq_lv < (LV_A1 * GRD_UN))			//0.9 GRD_UN=额定电网相电压峰值 690->563V; 380->310V
		if(M_ChkFlag(SL_HVLV_DETEC)!=0 && TRS_NPR_U.ampflt < (0.9 * GRD_UN))			//GRD_UN=额定电网相电压峰值 690->563V; 380->310V LVRTNR201103
		{																//跌落检测:NGS_Udq_lv延时1ms,NGS_Udq要10ms
				//--------------------LVRT区域判断--------------------------
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
//							M_SetFlag(SL_UACLV2);							//添加200ms延时
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
//							M_SetFlag(SL_UACLV2);							//添加100ms延时
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
				//--------------------LVRT区域判断--------------------------
			MAIN_LOOP.cnt_hvlv_rcv=0;	
		}
		else if(NGS_Udq_p > (0.9 * GRD_UN))									//电网正常或者
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_hvlv_rcv,DELAY_HVLVRCV)>0)			//20ms
			{
			 	M_ClrFlag(SL_LV_STATE);										//恢复！
			 	M_ClrFlag(SL_UACLV2);		
			}
			MAIN_LOOP.cnt_lv_time=0;
			MAIN_LOOP.cnt_lv_fault1=0;	
			MAIN_LOOP.cnt_lv_fault2=0;		
		}		
		else	
		{
			MAIN_LOOP.cnt_hvlv_rcv=0;	
			MAIN_LOOP.cnt_lv_time=0;
			MAIN_LOOP.cnt_lv_fault1=0;	
			MAIN_LOOP.cnt_lv_fault2=0;	
		} 
	}
	else
	{
		M_ClrFlag(SL_LV_STATE);										
		M_ClrFlag(SL_UACLV2);	
		MAIN_LOOP.cnt_hvlv_rcv=0;	
		MAIN_LOOP.cnt_lv_time=0;
		MAIN_LOOP.cnt_lv_fault1=0;	
		MAIN_LOOP.cnt_lv_fault2=0;	
	}
*/
//------------------判断跌落时网压的不平衡度--20110326---------------------------------------------------------------
	if(M_ChkFlag(SL_HVLV_DETEC)!=0 && M_ChkFlag(SL_LV_STATE)!=0)
	{
		if(NGS_Udq_n2p > 10)		//10表示不对称跌落达到约20%		
//		if(NGS_Udq_n2p > _stdby05)	//外部给定201011LVRT LVRTNR201103
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_unbalance,DELAY_LVUBALNCE)>0)	
			M_SetFlag(SL_UNBALANCE);//30ms <50ms 要小于MPR或NPR临时封脉冲时间
		}
		else if(NGS_Udq_n2p < 6)	
//		else if(NGS_Udq_n2p < (_stdby05-5))	
		{
			M_ClrFlag(SL_UNBALANCE);
			MAIN_LOOP.cnt_unbalance=0;
		}
		else	MAIN_LOOP.cnt_unbalance=0;
	}
	else if	(M_ChkFlag(SL_HVLV_DETEC)!=0 && M_ChkFlag(SL_HV_STATE)!=0)
	{
		if(NGS_Udq_n2pex > 7)		//10表示不对称跌落达到约14%	
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_unbalance,3)>0)
			M_SetFlag(SL_UNBALANCE);	
		}
		else if(NGS_Udq_n2pex < 5)	
		{
			M_ClrFlag(SL_UNBALANCE);
			MAIN_LOOP.cnt_unbalance=0;
		}
		else	MAIN_LOOP.cnt_unbalance=0;
	}
	else 	
	{
		M_ClrFlag(SL_UNBALANCE);
		MAIN_LOOP.cnt_unbalance=0;
	}

//开关量备用6
/*	if((_COMMAND2&0x0040)!=0)	
	{
		if(M_ChkFlag(SL_LV_STATE)!=0)	M_SetFlag(SL_UNBALANCE);//=1是认为是不平衡跌落,LV机侧网侧禁止启动！201011LVRT
		else							M_ClrFlag(SL_UNBALANCE);	//20110327
	}
*/
//----------20130225采用电压跌落判断触发CROWBAR，加快投入速度-------------
	if(M_ChkFlag(SL_LV_STATE)!=0)
	{
	 if(M_ChkCounter(MAIN_LOOP.cnt_gridfault_last,30)<=0)		//跌落30ms以内执行判断		20130205
	 {
		if(M_ChkCounter(MAIN_LOOP.cnt_lv_start_scron,DELAY_LV_START_SCRON)<=0)
		{
//			if(NGS_Udq<300)		M_SetFlag(SL_LV_START_SCRON);
//			if(NGS_Udq<340)		//20130202 20%,35%
//			if(NGS_Udq<(NGS_Udq_p_ex * 0.5))		//20130221 20%(2ph/3ph),35%(3ph)
//			if(NGS_Udq<(NGS_Udq_p_ex * 0.41))		//20130223 20%(2ph/3ph),35%(3ph)
//			if((NGS_Udq<(NGS_Udq_p_ex * 0.41)) && (PRO.Pgactive_lv>600000))		//20130228 20%(2ph/3ph),35%(3ph),fullpower
//2013-12-6ZZJ取消投入crowbar及主动封网侧脉冲
/*			if(NGS_Udq<(NGS_Udq_p_ex * 0.65))		//20130228 20%(2ph/3ph),35%(3ph),主动投	20130308
			{
//				M_SetFlag(SL_LV_START_SCRON);
				if((_STDBY9&0x0100)==0)		M_SetFlag(SL_LV_START_SCRON);		//116.8	20130223
			}
//			if(NGS_Udq<(NGS_Udq_p_ex * 0.95))		//20130206 20%,35%,50%
			if((NGS_Udq<(NGS_Udq_p_ex * 0.95))||(PRO.NPR_iac>150.0))		//20130228 20%,35%,50%
			{
				if(M_ChkFlag(SL_NSTOP)==0)	M_SetFlag(SL_LV_ZDNSTOP);		//跌落瞬间主动封网侧脉冲20130202
			}
*/
//			if(NGS_Udq<(NGS_Udq_p_ex * 0.7))		//20130221 50%及以下所有工况
			if((NGS_Udq<(NGS_Udq_p_ex * 0.8))||(PRO.MPR_iac>150.0))		//20130228 80%及以下或1023A过流封停电机侧脉冲
//			if((NGS_Udq<(NGS_Udq_p_ex * 0.9))||(PRO.MPR_iac>1000.0))		//20130228 50%及以下所有工况
			{
				if(M_ChkFlag(SL_MSTOP)==0)	
				M_SetFlag(SL_LV_ZDMSTOP);		//跌落瞬间主动封机侧脉冲20130202
			}
			if(NGS_Udq>520)		
			MAIN_LOOP.cnt_lv_start_scron = 0;		
			//20130205 XLY?重置延时会持续检测，此句话用意是在跌落缓慢时，延时触发封停，但也可能会跳不出去锁前30ms都进此段		
			MAIN_LOOP.cnt_lv_start_scroff=0;
		}
		else if(M_ChkCounter(MAIN_LOOP.cnt_lv_start_scroff,DELAY_VDSCROFF)>=0) //scr至少导通3+7=10ms
		{
			M_ClrFlag(SL_LV_START_SCRON);
		}
	 }
	}
	else
	{
		M_ClrFlag(SL_LV_START_SCRON);
		MAIN_LOOP.cnt_lv_start_scron = 0;
	}

//-------------------绝对延时版本--MPR封脉冲控制 封停10ms 触发源(跌落主动封停、恢复封停、SCR动作封停、过流封停、电压超压封停)--------------------------------
//	if(M_ChkFlag(SL_MSTOP)==0 && M_ChkFlag(SL_LV_STATE)!=0 &&((PRO.MPR_iac>_SC_LVIAC2) || (PRO.udc>_SC_LVUDC1) || (M_ChkFlag(SL_SCRRUNING)!=0) || (M_ChkFlag(SL_LV_START_SCRON)!=0)))   //20110326mpr软件过流或者Vdc过压或电网不平衡 201012
	if(M_ChkFlag(SL_MSTOP)==0 && M_ChkFlag(SL_LV_STATE)!=0 &&((PRO.MPR_iac>_SC_LVIAC2) || (PRO.udc>_SC_LVUDC1) || (M_ChkFlag(SL_SCRRUNING)!=0) || (M_ChkFlag(SL_LV_START_SCRON)!=0) || (M_ChkFlag(SL_LV_RECMSTOP)!=0) || (M_ChkFlag(SL_LV_ZDMSTOP)!=0)))   //20110326mpr软件过流或者Vdc过压或电网不平衡 20130225
	{
		M_SetFlag(SL_MSTOP);	
		M_ClrFlag(SL_LV_ZDMSTOP);		//20130221	20130225
		MAIN_LOOP.cnt_mpwmrestart=0;									//LVRT机侧脉冲停止
		Disepwmio_MPR();       											//201011LVRT
		M_ClrFlag(SL_MPR_PWMOUT);
		M_ClrFlag(SL_HVLV_MPWMOUT);

		if(M_ChkFlag(SL_LV_RECMSTOP)!=0)	MAIN_LOOP.cnt_lv_dlrecmstop=0;	//20130201 恢复时只封一次
		M_ClrFlag(SL_LV_RECMSTOP);		//20130130 脉冲只封一次
	}
	else if(M_ChkFlag(SL_MSTOP)==0 && M_ChkFlag(SL_HV_STATE)!=0 &&((PRO.MPR_iac>_SC_LVIAC2) || (M_ChkFlag(SL_SCRRUNING)!=0)))
	{
		M_SetFlag(SL_MSTOP);	
		MAIN_LOOP.cnt_mpwmrestart=0;									//LVRT机侧脉冲停止
		Disepwmio_MPR();       											//201011LVRT
		M_ClrFlag(SL_MPR_PWMOUT);
		M_ClrFlag(SL_HVLV_MPWMOUT);
	}
	else if(M_ChkFlag(SL_MSTOP)!=0)									//20110829尽快恢复励磁
//	else if(M_ChkFlag(SL_MSTOP)!=0 && M_ChkFlag(SL_UNBALANCE)==0)	//LVRTNR201103
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_mpwmrestart,_stdby07)>=0)					//当前设置 封停10ms
		{
			if((PRO.MPR_iac<(_SC_LVIAC2-20)) && (PRO.udc<(_SC_LVUDC1-5)) && (M_ChkFlag(SL_SCRRUNING)==0))		//20121210	小功率可以重启20130228
//			if(PRO.MPR_iac<(_SC_LVIAC2-20) && PRO.udc<(_SC_LVUDC1-5) && M_ChkFlag(SL_SCRRUNING)==0 && M_ChkFlag(SL_UNBALANCE)==0)
//				M_ClrFlag(SL_MSTOP);				  					//机侧LVRT及其过渡结束且不平衡消失，立即允许重启脉冲20110829
//--------除了大功率2相20、35情况，其余可重启脉冲20130228--------
			{
//				if(M_ChkFlag(SL_UNBALANCE)==0)	M_ClrFlag(SL_MSTOP);
				if((M_ChkFlag(SL_UNBALANCE)==0) && (NGS_Udq_n2p<23))	
				M_ClrFlag(SL_MSTOP);		//20130306对称跌落可重启，23对应不对称跌落40%
//				else if((PRO.Pgactive_lv<600000) || (NGS_Udq_n2p<27))
//				else if(((PRO.Pgactive_lv<600000) || (NGS_Udq_n2p<27)) && ((_STDBY9&0x0080)!=0))	//116.7		20130303
				else if((NGS_Udq_n2p<31) && (NGS_Udq_p>(NGS_Udq_p_ex * 0.70)))	//不对称跌落深度不恶劣于50%的情况可启脉冲
				M_ClrFlag(SL_MSTOP);
			}
//------------------------
		}
	}
	else	MAIN_LOOP.cnt_mpwmrestart=0;

//------------------无功控制--修改，未验证！与转矩清零有关20110327-------------------------
	if(M_ChkFlag(SL_LV_STATE)!=0)//20110829HC
//	if(M_ChkFlag(SL_HVLV_DETEC)!=0 && (NGS_Udq < (0.9 * GRD_UN)))//20111201bjtu交大现行版本
//	if(M_ChkFlag(SL_HVLV_DETEC)!=0 && M_ChkFlag(SL_LV_STATE)!=0 && (NGS_Udq < (0.7 * GRD_UN)))//20110327
//	if(M_ChkFlag(SL_HVLV_DETEC)!=0 && M_ChkFlag(SL_LV_STATE)!=0 &&(NGS_Udq < (0.8 * GRD_UN)) && M_ChkFlag(SL_MSTOP)==0 && M_ChkFlag(SL_NSTOP)==0)//20110327
//	if(M_ChkFlag(SL_HVLV_DETEC)!=0 && M_ChkFlag(SL_LV_STATE)!=0 &&(NGS_Udq < (0.8 * GRD_UN)) && M_ChkFlag(SL_MSTOP)==0 && M_ChkFlag(SL_NSTOP)==0 && M_ChkFlag(SL_UNBALANCE)==0)//LVRTNR201103不平衡条件
	{
		M_SetFlag(SL_TRIG_ERRDSAVE);	 	//LVRT触发故障锁存示波器201205LVRTatZB
		M_SetFlag(SL_LV_QWORKING);												//网侧发无功
		M_ClrFlag(SL_HV_QWORKING);
		M_SetFlag(SL_LV_STRICTLV);			//20130222
		M_ClrFlag(SL_HV_STRICTLV);
		GIVE.lvwtiqrf = kq * 1775.0;	//BJTULVRT201204
		if(GIVE.lvwtiqrf > 1775)	GIVE.lvwtiqrf = 1775; //无功电流限幅限幅
		MAIN_LOOP.cnt_qworking = 0;	

//--------20130222	20130225--------
		if(M_ChkCounter(MAIN_LOOP.cnt_gridfault_last,500)>=0)	//500ms
		{
			if(((NGS_Udq_p - NGS_Udq_p_lv) > 55) || (NGS_Udq_p > (0.95 * NGS_Udq_p_ex)))
				M_ClrFlag(SL_LV_STRICTLV);
		}
//-------------------------
	}
	else if(M_ChkFlag(SL_HV_STATE)!=0)
	{
		M_SetFlag(SL_TRIG_ERRDSAVE);	 			
		M_SetFlag(SL_HV_QWORKING);			//网侧发无功
		M_ClrFlag(SL_LV_QWORKING);
		M_SetFlag(SL_HV_STRICTLV);
		M_ClrFlag(SL_LV_STRICTLV);			
		GIVE.hvwtiqrf = kq * 1775.0;	   
		if(GIVE.hvwtiqrf > 1775)	GIVE.hvwtiqrf = 1775; //无功电流限幅限幅
		MAIN_LOOP.cnt_qworking = 0;
		if(M_ChkCounter(MAIN_LOOP.cnt_gridfault_last,500)>=0)	//hVRT标志位设定后500ms以上
		{
			if(NGS_Udq_p < (1.05 * NGS_Udq_p_ex))    //电压恢复了清除
			M_ClrFlag(SL_HV_STRICTLV);
		}
//-------------------------
	}
//	else if(NGS_Udq > (_stdby01 * GRD_UN))//!!!!!!!!!!!
//	else if(NGS_Udq > (0.9 * GRD_UN))//!!!!!!!!!!!
	else	//20110829HC
	{
//		M_ClrFlag(SL_LV_QWORKING);												//网侧停止发无功,恢复有功
		GIVE.lvwtiqrf = 0;
		GIVE.hvwtiqrf = 0;
		if(M_ChkCounter(MAIN_LOOP.cnt_qworking,DELAY_QWORKING)>=0)			//100ms 201205LVRTatZB
		{
			M_ClrFlag(SL_LV_QWORKING);											//LV标志位恢复100ms后，网侧停止发无功,恢复有功
			M_ClrFlag(SL_HV_QWORKING);
		}
		M_ClrFlag(SL_LV_STRICTLV);
		M_ClrFlag(SL_HV_STRICTLV);
	}

//--------LV NPR封脉冲控制LV NPR封脉冲控制（过流、跌落时需求主动封、恢复时需求主动封，都会封停）--------
//--------2013-12-6由于_SC_LVIAC2从195改为190，所以_SC_LVIAC2-35改为_SC_LVIAC2-40	
	if(M_ChkFlag(SL_NSTOP)==0 && M_ChkFlag(SL_LV_STATE)!=0 && ((PRO.NPR_iac>(_SC_LVIAC2-45)) || M_ChkFlag(SL_LV_ZDNSTOP)!=0 || M_ChkFlag(SL_LV_RECNSTOP)!=0)) 
	{
		M_SetFlag(SL_NSTOP);
		M_ClrFlag(SL_LV_ZDNSTOP);		//20130202
		MAIN_LOOP.cnt_npwmrestart=0;									//LVRT网侧脉冲停止	

//		if(M_ChkFlag(SL_LV_RECNSTOP)!=0) //2013-12-6ZZJ取消网压恢复时封网侧脉冲
//		M_ClrFlag(SL_LV_RECNSTOP);		//20130206 脉冲只封一次
	}
//	else if(M_ChkFlag(SL_NSTOP)!=0 && (PRO.NPR_iac<(_SC_LVIAC2-20)) && M_ChkFlag(SL_UNBALANCE)==0) //LVRTNR201103
	else if(M_ChkFlag(SL_NSTOP)==0 && M_ChkFlag(SL_HV_STATE)!=0 && (PRO.NPR_iac>(_SC_LVIAC2)))	
	{
		M_SetFlag(SL_NSTOP);
		MAIN_LOOP.cnt_npwmrestart=0;									//LVRT网侧脉冲停止	
	}
	else if(M_ChkFlag(SL_NSTOP)!=0)			//20110829
	{
//		if(M_ChkCounter(MAIN_LOOP.cnt_npwmrestart,DELAY_NPWMRESTART)>=0)		//50ms
//		if(M_ChkCounter(MAIN_LOOP.cnt_npwmrestart,_stdby04)>=0)					//BJTULVRT201204
		if(M_ChkCounter(MAIN_LOOP.cnt_npwmrestart,5)>=0)					//20130309atsp封停5ms
		{
//			 if(PRO.NPR_iac<(_SC_LVIAC2-20) && M_ChkFlag(SL_UNBALANCE)==0)
			 if(PRO.NPR_iac<(_SC_LVIAC2-45))				//20121226
				M_ClrFlag(SL_NSTOP);				  							//机侧LVRT及其也黄胶庀В⒓丛市碇仄袈龀?20110829
		}
	}
	else MAIN_LOOP.cnt_npwmrestart=0;

//-------------------LV SCR电流到零判断（意味着SCR已经关断，检测瞬时值）-20110829HC----------------
	if((abs(AD_OUT_SCR_I.a)>35) || (abs(AD_OUT_SCR_I.b)>35))	//SCR电流接近零   						
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_scrizero2,DELAY_SCRIZERO2)>=0)	M_ClrFlag(SL_SCRIZERO);	//1ms
		MAIN_LOOP.cnt_scrizero=0;	
	}
	else if((abs(AD_OUT_SCR_I.a)<25) && (abs(AD_OUT_SCR_I.b)<25))	//201007BJTULVRT?????I-LEM零漂误差多大?
	{	
//		if(M_ChkCounter(MAIN_LOOP.cnt_scrizero,DELAY_SCRIZERO)>=0)	M_SetFlag(SL_LV_SCRIZERO);	  //15ms 通过电髋卸蟂CR是否都关断,DELAY_SCRIZERO延靠实验确定 
//		if(M_ChkCounter(MAIN_LOOP.cnt_scrizero,_stdby06)>=0)	M_SetFlag(SL_LV_SCRIZERO);	  //201007BJTULVRT 通缌髋卸蟂CR是否都关断,DELAY_SCRIZERO延时靠实验确定 
		if(M_ChkCounter(MAIN_LOOP.cnt_scrizero,1)>=0)	M_SetFlag(SL_SCRIZERO);	  //201007BJTULVRT 通缌髋卸蟂CR是否都关断,DELAY_SCRIZERO延时靠实验确定 
//		else	M_ClrFlag(SL_LV_SCRIZERO);								 
		MAIN_LOOP.cnt_scrizero2=0;							 
    }
	else	
	{
		MAIN_LOOP.cnt_scrizero=0;
		MAIN_LOOP.cnt_scrizero2=0;							 
	}

//---------------Crowbar控制改进20110829HC----------------------------------
/*	if(M_ChkFlag(SL_LV_STATE)!=0)	//不对称跌落crowbar开通第3次之后一直开通到LV结束20120602
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
//-------------------LV SCR动作Vdc条件判断-----------------
//	if(((PRO.udc>1400) || (PRO.udc>_SC_LVUDC2 && M_ChkFlag(SL_LV_STATE)!=0))&& M_ChkFlag(SL_SCRON)==0)
/*	if(((PRO.udc>1400) || ((PRO.udc>_SC_LVUDC2 || PRO.MPR_iac>(_SC_LVIAC2+15)) && M_ChkFlag(SL_LV_STATE)!=0) || M_ChkFlag(SL_LV_START_SCRON)!=0)&& M_ChkFlag(SL_SCRON)==0) 
												//20120603night二极管过流也触发crowbar	20121203
	{
		M_SetFlag(SL_SCRON);							//开启SCR动作指令
		MAIN_LOOP.cnt_lv_scron=0;						//重要：保证下次SCR开通依然延时_SC_SCRON2010115修改
		CNT_SCRON++;		//20120602					
	}
	else if(PRO.udc< (_SC_LVUDC2-30))
	{
		M_ClrFlag(SL_SCRON);							//关断SCR指令	
	}
*/
//--------20130225--------------
	if(M_ChkFlag(SL_SCRDONE)==0)		//首次投入或未投入
	{//SCR未启动且（直流电压1400V，低穿下直流电压大于阈值（当前1180V）或机侧过流，跌落瞬间主动启动scr）
		if(((PRO.udc>1400) || ((PRO.udc>_SC_LVUDC2 || PRO.MPR_iac>(_SC_LVIAC2)) && M_ChkFlag(SL_LV_STATE)!=0) || M_ChkFlag(SL_LV_START_SCRON)!=0)&& M_ChkFlag(SL_SCRON)==0)  //2013-12-6 ZZJ首次投入1300A
		{
			M_SetFlag(SL_SCRON);							//开启SCR动作指令
			MAIN_LOOP.cnt_lv_scron=0;						//重要：保证下次SCR开通依然延时_SC_SCRON2010115修改
		}
		else if(PRO.udc< (_SC_LVUDC2-30))
		{
			M_ClrFlag(SL_SCRON);
		}
	}
	else  //已经投入了一次现已切出
	{//SCR未启动且（直流电压1400V，低穿下直流电压大于阈值（当前1180V）或机侧过流）
		if(((PRO.udc>1400) || ((PRO.udc>1250 || PRO.MPR_iac>(_SC_LVIAC2+15)) && M_ChkFlag(SL_LV_STATE)!=0))&& M_ChkFlag(SL_SCRON)==0) 
		{
			M_SetFlag(SL_SCRON);							//开启SCR动作指令
			MAIN_LOOP.cnt_lv_scron=0;						//重要：保证下次SCR开通依然延时_SC_SCRON2010115修改
		}
		else if(PRO.udc< (_SC_LVUDC2-30))
		{
			M_ClrFlag(SL_SCRON);
		}
	}

//-------------------LV SCR控制-采用后须将机侧临时封脉冲的SL_SCRON换成SL_SCRRUNING--------------------------------------
//	if(M_ChkFlag(SL_LV_SCRON)!=0 || M_ChkFlag(SL_LV_SCRKEEPON)!=0) //20120602											//要求Crowbar开通
	if(M_ChkFlag(SL_SCRON)!=0)											//要求Crowbar开通
	{		
		M_ACROWBARON();								 						//SCR开通
		M_SetFlag(SL_SCRRUNING);											//SCR开通标志位
		M_SetFlag(SL_DISPLAY6);													
	}
	else if(M_ChkFlag(SL_SCRRUNING)!=0 && M_ChkFlag(SL_LV_STATE)!=0)//---------分跌落深度和类型切出Crowbar20121210---------20130225
	{
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scroff3,_stdby04)>0)			//投入scr后5ms判断	20130309
			{
//				if(M_ChkFlag(SL_SCRDONE)!=0)
//				if((scroff1==1) || (M_ChkFlag(SL_SCRDONE)!=0) || ((NGS_Udq_epsilon > 18) && (PRO.Pgactive_lv<600000) && (NGS_Udq_p>(0.66 * NGS_Udq_p_ex))))	//小功率35%-2ph及以上只投2-5ms	20130303
/*				if((scroff1==1) || (M_ChkFlag(SL_SCRDONE)!=0) || ((NGS_Udq_epsilon > 18) && (PRO.Pgactive_lv<600000) && (NGS_Udq_p>(0.55 * NGS_Udq_p_ex))))	//小功率20%-2ph及以上只投2-5ms	20130308
				{
					scroff1 = 1;
					M_ACROWBAROFF();
					if(M_ChkFlag(SL_SCRIZERO)!=0)							//检测到SCR电鹘咏?
					{
						M_ClrFlag(SL_SCRRUNING);								//结束SCR动作进程
						MAIN_LOOP.cnt_lv_scroff3=0;								//重要：保证下次SCR开通依然延时_SC_SCRON2010115修改	
						scroff1 = 0;
					}
				}
*///--------20130309小功率20%再次投入--------
				if((M_ChkFlag(SL_SCRDONE)!=0) || (PRO.Pgactive_lv<600000))	//小功率20%-2ph及以上只投2-5ms	20130308
				{
					M_ACROWBAROFF();
					if(M_ChkFlag(SL_SCRIZERO)!=0)							//检测到SCR电鹘咏?
					{
						if((NGS_Udq_p<=(0.327 * NGS_Udq_p_ex)) && (scroff1==0) && (M_ChkFlag(SL_SCRDONE)==0))
						{//116.2	20-2ph小风，首次投入，深度跌落，则再次投入	20130314
							MAIN_LOOP.cnt_lv_scroff3=0;
//							M_ACROWBARON();
							if((_STDBY9&0x0004)!=0)		M_ACROWBARON();		
							scroff1 = 1;
						}
						else
						{
							M_ClrFlag(SL_SCRRUNING);								//结束SCR动作进程
							MAIN_LOOP.cnt_lv_scroff3=0;								//重要：保证下次SCR开通依然延时_SC_SCRON2010115修改	
							scroff1 = 0;
						}
					}
				}
			}
//-------------------------------------------
//------------------------
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scroff2,10)>0)			//投入scr后10ms判断
			{
//				if(((NGS_Udq_p>280) && (NGS_Udq_epsilon < 15)) || (M_ChkFlag(SL_LV_SCROFF2)!=0))				//跌幅深度低于50%	20130116
				if(((NGS_Udq_p>190) && (NGS_Udq_n2p < 15)) || (M_ChkFlag(SL_LV_SCROFF2)!=0))				//35%及以上情况（除35%大风）	20130221
				{
					if((NGS_Udq_p<250) && (PRO.Pgactive_lv>600000))	{}		//剔除35%大风	20130221
					else
					{
						M_ACROWBAROFF();											//SCR关断
						M_SetFlag(SL_LV_SCROFF2);					
			
						if(M_ChkFlag(SL_SCRIZERO)!=0)							//检测到SCR电鹘咏?
						{				
							M_ClrFlag(SL_SCRRUNING);								//结束SCR动作进程
							MAIN_LOOP.cnt_lv_scroff2=0;								//重要：保证下次SCR开通依然延时_SC_SCRON2010115修改	
							M_ClrFlag(SL_LV_SCROFF2);
						}
					}
				}
			}
/*			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scroff1,lv_ctrl3)>0)			//投入scr后22ms判断
			{
//				if((NGS_Udq_p<190 && PRO.Pgactive_lv<600000) && (NGS_Udq_epsilon < 15) || (M_ChkFlag(SL_LV_SCROFF1)!=0))				//至20%及小风 20130221
				if((PRO.Pgactive_lv<600000) || (M_ChkFlag(SL_LV_SCROFF1)!=0))				//所有小风工况不超过该时间 20130303
				{
					M_ACROWBAROFF();											//SCR关断
					M_SetFlag(SL_LV_SCROFF1);

					if(M_ChkFlag(SL_SCRIZERO)!=0)							//检测到SCR电鹘咏?
					{				
						M_ClrFlag(SL_SCRRUNING);								//结束SCR动作进程
						MAIN_LOOP.cnt_lv_scroff1=0;								//重要：保证下次SCR开通依然延时_SC_SCRON2010115修改				
						M_ClrFlag(SL_LV_SCROFF1);
					}
				}
			}
*///20130309取消,恢复时注意恢复定时器
			if(M_ChkCounter(MAIN_LOOP.cnt_lv_scron,_SC_TSCRON)>0)			//SCR开通一个_SC_SCRON固定检测25ms
			{								
				if(((NGS_Udq_n2p > 40) && (PRO.Pgactive_lv>600000) && (NGS_Udq_p<(NGS_Udq_p_ex * 0.56))) || (scrhold==1))		//2013022 大功率不对称跌落
				{
					 scrhold = 1;		//20130223
					 if(M_ChkFlag(SL_LV_SCRKEEPON)==0)
					 {
					 	M_ACROWBAROFF();		//20130223
						scrhold = 0;
					 }
				}
				else if(NGS_Udq_n2p > 26)		//涵盖20%小功率及35%大小功率20130222
				{
//					if(M_ChkCounter(MAIN_LOOP.cnt_lv_unblcscroff2,50)>0)	M_ACROWBAROFF();		//SCR关断	50ms-20130221
					if(M_ChkCounter(MAIN_LOOP.cnt_lv_unblcscroff2,80)>0)	M_ACROWBAROFF();		//SCR关断	50ms-20130221
				}
				else	M_ACROWBAROFF();				//SCR关断

				if(M_ChkFlag(SL_SCRIZERO)!=0)							//检测到SCR电鹘咏?
				{
					M_ClrFlag(SL_SCRRUNING);								//结束SCR动作进程
					MAIN_LOOP.cnt_lv_scron=0;								//重要：保证下次SCR开通依然延时_SC_SCRON2010115修改				
					MAIN_LOOP.cnt_lv_unblcscroff1 = 0;
					MAIN_LOOP.cnt_lv_unblcscroff2 = 0;
					MAIN_LOOP.cnt_lv_unblcscroff3 = 0;			//20130124
					scrhold = 0;								//20130223
				}
			}
	}
	else if(M_ChkFlag(SL_SCRRUNING)!=0) //其余切出逻辑，包括高电压切出
	{
			M_ACROWBAROFF();
			if(M_ChkFlag(SL_SCRIZERO)!=0)							//检测到SCR电鹘咏?
			{
				M_ClrFlag(SL_SCRRUNING);								//结束SCR动作进程

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
//---------------------
//----------第一次投入scr结束判断20130222-----------20130225
	if((M_ChkFlag(SL_LV_STATE)!=0 && M_ChkFlag(SL_SCRON)!=0) || (scrdone == 1))
	{
		scrdone = 1;
		if(M_ChkFlag(SL_SCRRUNING)==0)	M_SetFlag(SL_SCRDONE);
	}
	if(M_ChkFlag(SL_LV_STATE)==0)
	{
		scrdone = 0;
		M_ClrFlag(SL_SCRDONE);
	}
//----------------------------------------------

//----------LV动态定子磁链控制 跌落和恢复后100-200ms内实施 BJTULVRT201204
/*	if(M_ChkFlag(SL_LV_STATE)!=0)	
	{
		M_SetFlag(SL_LV_PHICON);
		MAIN_LOOP.cnt_lv_phicontrol=0;
	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_lv_phicontrol,DELAY_PHICON)>=0)	//恢复后磁链抵消控制时间50ms 201205LVRTatZB
//		if(M_ChkCounter(MAIN_LOOP.cnt_lv_phicontrol,_stdby03)>=0)	//恢复后磁链抵刂剖奔?
			 M_ClrFlag(SL_LV_PHICON);
	}		
*/			

//----------LV动态PI控制 机侧重启后20ms内实施 BJTULVRT201204
	if(M_ChkFlag(SL_MSTOP)!=0)
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
** 函数名?: Protect
** 功能描述: 保护值计算
** 输　入: 	 
** 输  出:   
** 注  释: 	 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:20121103
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Protect(void)
{
/*	float temp;

//----------------------------并网电流瞬时值保护---------------------------
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

	PRO.NPR_iac = temp * 100 / NPR_IACN;			//并网电流瞬时最大值，基准电流的百分值

//----------------------------转子电流瞬时值保护---------------------------
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

	PRO.MPR_iac = temp * 100 / MPR_IACN;		//转子电鞔笾担嫉琪的百分值

//------------------------中间直流电压保护值计算-----------------------------

	PRO.udc  = AD_OUT_UDC;	                                 //单位V
*/
//--------------------------- 中间斩波电流保护值-201105atbjtuLVRT---------------------------
	PRO.CHOP_idc = AD_OUT_DC_I.b * 100 / CHOP_IDCN;

//---------------------------网侧线电压有效值计算----------------------------
    PRO.NPR_uab= MEAN_DATA.uab1 * 1.110721;                      //网侧Uab有效值单位V rms=mean*PAI/(2*sqrt(2)) 
	PRO.NPR_ubc= MEAN_DATA.ubc1 * 1.110721;                      //网侧Ubc有效值单位V 1.110721=PAI * SQRT2 / 4
//	PRO.GID_uab = MEAN_DATA.uab * 1.110721;        //20091026atzy 电网侧Uab有效值单位V 单位V  rms=mean*PAI/(2*sqrt(2)) ； 1.110721=PAI * SQRT2 / 4
//	PRO.GID_ubc = MEAN_DATA.ubc * 1.110721;        //20091026atzy 电网侧Uab有效值单位V 单位V  rms=mean*PAI/(2*sqrt(2)) ； 1.110721=PAI * SQRT2 / 4
//	PRO.GID_uab = MEAN_DATA.uab1 * 1.110721;        //20091026atzy 电网侧Uab有效值单位V 单位V  rms=mean*PAI/(2*sqrt(2)) ； 1.110721=PAI * SQRT2 / 4
//	PRO.GID_ubc = MEAN_DATA.ubc1 * 1.110721;        //20091026atzy 电网侧Uab有效值单位V 单位V  rms=mean*PAI/(2*sqrt(2)) ； 1.110721=PAI * SQRT2 / 4

//---------------------------定子侧线电压有效值计算----------------------------
    PRO.STA_uab= MEAN_DATA.uab2 * 1.110721;                    //电侧Uab有效值单V
	PRO.STA_ubc= MEAN_DATA.ubc2 * 1.110721;                    //电机侧Ubc有效值单位V

//------------------功率保护及显示值计算-----------------------------------------
//------------------定子侧dq变换定相角度值--------------------------------------------------------------------
/*	temp = CAP4.nprtrstheta + 0.5236 - 1.57; //systest 201007BJTULVRT CAP4.nprtrstheta+30-90
//	temp = CAP4.nprtrstheta + (1.047 * 0.5); //systest 2010atcpc
	DIP_STA_I.sintheta = sin(temp);  //systest 2010atcpc
	DIP_STA_I.costheta = cos(temp);  //systest 2010atcpc
//------------------定子侧电流dq值--------------------------------------------------------------------
   	DIP_STA_I.a     = AD_OUT_STA_I.a;
   	DIP_STA_I.b     = AD_OUT_STA_I.b;
   	DIP_STA_I.c     = AD_OUT_STA_I.c;
	Transform_3s_2s_2r(&DIP_STA_I);
	DataFilter(0.99,&DIP_STA_I.dflt,DIP_STA_I.d); 	//定子侧电流反馈值滤波,Ts=200us,滤掉开关频率次
	DataFilter(0.99,&DIP_STA_I.qflt,DIP_STA_I.q); 	//定子侧电流反馈值滤波,Ts=200us,滤掉开关频率次

//------------------定子侧电压dq值--------------------------------------------------------------------
	DIP_STA_U.dflt = 	DIP_NPR_U.dflt;			   //systest 2010atcpc
	DIP_STA_U.qflt = 	DIP_NPR_U.qflt;

//-----------------计算定子磁链--需要转子定子归算！201007BJTULVRT---------------------------------------------------------------------
	PHAI_d = DIP_STA_I.dflt * MPR_Ls + TRS_MPR_I.dflt * MPR_Lm / STAROTRTO;
	PHAI_q = DIP_STA_I.qflt * MPR_Ls + TRS_MPR_I.qflt * MPR_Lm / STAROTRTO;
	PHAI_dq = sqrt(PHAI_d * PHAI_d + PHAI_q * PHAI_q);

//------------------------定子相电压有效值显示值计算(为功率计算)--------------------------------------------------
    PRO.sta_uar = MEAN_DATA.ua1 * 1.110721;         //单位V 采用网压作为定子电压，因为定子V-LEM反〉缪咕舜蟪Ｊ瞬?
	PRO.sta_ubr = MEAN_DATA.ub1 * 1.110721;
	PRO.sta_ucr = MEAN_DATA.uc1 * 1.110721;

//------------------------定子相电流有效值显示值计算(为功率计算)--------------------------------------------------
    PRO.sta_iar = MEAN_DATA.ia3 * 1.110721;			//单位A
	PRO.sta_ibr = MEAN_DATA.ib3 * 1.110721;
	PRO.sta_icr = MEAN_DATA.ic3 * 1.110721;
*///屏蔽BJTULVRT201204

//------------------定子侧有功和无功计算值--------------------------------------------------------------------
//    PRO.Psactive   = 1.5 * SQRT3 * (DIP_STA_U.dflt * DIP_STA_I.dflt + DIP_STA_U.qflt * DIP_STA_I.qflt); 
//    PRO.Psreactive = 1.5 * SQRT3 * (DIP_STA_U.qflt * DIP_STA_I.dflt - DIP_STA_U.dflt * DIP_STA_I.qflt); 
//	PRO.Ps		   = SQRT3 * (PRO.sta_uar * PRO.sta_iar + PRO.sta_ubr * PRO.sta_ibr + PRO.sta_ucr * PRO.sta_icr);//20091007

//------------------------网侧并网电流有效值显示值计算----------------------------------------------
    PRO.npr_iar = MEAN_DATA.ia1 * 1.110721;     			 //1.110721=PAI * SQRT2 / 4  	//单位A
	PRO.npr_ibr = MEAN_DATA.ib1 * 1.110721;
	PRO.npr_icr = MEAN_DATA.ic1 * 1.110721;

//------------------------网嘞嗟缪褂行е迪允局导扑?----------------------------------------------
    PRO.npr_uar = MEAN_DATA.ua1 * 1.110721;      			//1.110721=PAI * SQRT2 / 4  //单位A
	PRO.npr_ubr = MEAN_DATA.ub1 * 1.110721;
	PRO.npr_ucr = MEAN_DATA.uc1 * 1.110721;

//------------------网嘤泄臀薰扑阒?--------------------------------------------------------------------
// 	PRO.Pnactive   = Pnactive;
//	PRO.Pnreactive = Pnreactive;
//	PRO.Pn         = PRO.npr_iar * PRO.npr_uar + PRO.npr_ibr * PRO.npr_ubr + PRO.npr_icr * PRO.npr_ucr;

//------------------------------定子侧和网侧总并网功率显示-----------------------------------------------------
//	PRO.Pgactive   = PRO.Psactive   + PRO.Pnactive;
//	PRO.Pgreactive = PRO.Psreactive + PRO.Pnreactive;

//	if(CAP4.omigaslp >= 0)	 PRO.Pg = PRO.Ps - PRO.Pn;
//	else 					 PRO.Pg = PRO.Ps + PRO.Pn;
//---------------------------网侧和机绺形露戎?----------------------------
    PRO.NPR_TLOV= MEAN_DATA.Lac_temp;                    //网侧电感温度
	PRO.MPR_TLOV= MEAN_DATA.Ldudt_temp;                  //机侧电感温度

//--------------------------- 定子侧线电流保护值----------------------------
	PRO.STA_iac = MEAN_DATA.iac3 * 1.110721  * 100/ STA_IACN;
	PRO.STA_icb = MEAN_DATA.icb3 * 1.110721  * 100/ STA_IACN;

//功率计算201205LVRTatZB
//------------------网侧有功和无功计算值--------------------------------------------------------------------
	if((M_ChkFlag(SL_IN1_CBSTS)!=0) && (M_ChkFlag(SL_IN1_MIANFILTER)!=0))	
		Pcreactive = DIP_NPR_U.dflt * DIP_NPR_U.dflt * 0.236;	//Pc=1.5*ed*ed*ws*Cn 201205LVRTatZB
	else	Pcreactive = 0;

//--------瞬时功率201205LVRTatZB
 	PRO.Pnactive   = AD_OUT_NGS_U.a * AD_OUT_NPR_I.a + AD_OUT_NGS_U.b * AD_OUT_NPR_I.b + AD_OUT_NGS_U.c * AD_OUT_NPR_I.c;
	PRO.Pnreactive = SQRT3_3 *  (AD_OUT_NGS_U.bc * AD_OUT_NPR_I.a + (AD_OUT_NGS_U.c-AD_OUT_NGS_U.a) * AD_OUT_NPR_I.b + AD_OUT_NGS_U.ab * AD_OUT_NPR_I.c);
	PRO.Pnreactive = PRO.Pnreactive + Pcreactive;

    PRO.Psactive   = AD_OUT_NGS_U.a * AD_OUT_STA_I.ac + AD_OUT_NGS_U.b * AD_OUT_STA_I.ba + AD_OUT_NGS_U.c * AD_OUT_STA_I.cb; 
    PRO.Psreactive = SQRT3_3 *  (AD_OUT_NGS_U.bc * AD_OUT_STA_I.ac + (AD_OUT_NGS_U.c-AD_OUT_NGS_U.a) * AD_OUT_STA_I.ba + AD_OUT_NGS_U.ab * AD_OUT_STA_I.cb); 

//---------------------------转速保护值计算----------------------------------
	PRO.speed   = 9.5492966 * QEPDATA.omigamec;		//单位：转/分；n=60*w/2pai=*w
//	PRO.speed   = 1200;		//controltest

	DataFilter(0.45,&PRO.speedflt,PRO.speed); 				//主控转速反馈值滤波， Ts=200us,Tr=5ms 20091021atzy之前123ms滤波太大了
	
	if(PRO.speedflt>700)		M_SetFlag(SL_SPEED_HIGH);	//进入高转速区   20091021atzy 650改为700，加大滞环
    else if(PRO.speedflt<600)	M_ClrFlag(SL_SPEED_HIGH);   //650r/min测周法PRD=845.测频法POSLAT=887.

    if((PRO.speedflt > _SC_MSPEED1) && (PRO.speedflt < _SC_MSPEED2)) 	 
    		M_SetFlag(SL_SPEED_IN_RANGE);  
    else if((PRO.speedflt < (_SC_MSPEED1-SC_MSPD_HW)) || (PRO.speedflt > (_SC_MSPEED2 + SC_MSPD_HW))) 	 
			M_ClrFlag(SL_SPEED_IN_RANGE);					//判断加回差201005atcpc

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

//----------------------------纠正计算并网用功功率,显示---------------------------------------
//------------------------------定子侧和网侧总并网功率显示-----------------------------------------------------
//	PRO.Pgactive = Te_feedback * PRO.speedflt * PAI * 0.03333333;	//P=T*2PIE*n/60 201005atcpc
	PRO.Pgactive2   = PRO.Psactive   + PRO.Pnactive;					
	PRO.Pgreactive2 = PRO.Psreactive + PRO.Pnreactive;
	DataFilter(0.99,&PRO.Pgactive,PRO.Pgactive2); 		//注：改滤波?400us/2.5kHz执行一次。定子侧电流反馈值滤波,Ts=200us,滤掉开关频率次0.97
	DataFilter(0.99,&PRO.Pgreactive,PRO.Pgreactive2); 	//c=0.99->8Hz; c=0.9->88Hz

	if(M_ChkFlag(SL_LV_QWORKING)==0) 	PRO.Pgactive_lv = PRO.Pgactive;		//判断LVRT之前功率20121210

	if((_STDBY9&0x0008)!=0)	//116.3 201205LVRTatZB
    	PRO.Pgactive   = Te_feedback * PRO.speedflt * PAI * 0.03333333;	//P=T*2PIE*n/60 201005atcpc

//----------------------------网侧接地故障-三相电流不平衡保护判断---------------------------20100302
	if((AD_OUT_NPR_I.a+AD_OUT_NPR_I.b+AD_OUT_NPR_I.c)>120)
			M_SetFlag(SL_IUBLNCE1);
	else	M_ClrFlag(SL_IUBLNCE1);

//----------------------------机侧接地故障-三相电流不平衡保护判断---------------------------20100302
	if((AD_OUT_MPR_I.a+AD_OUT_MPR_I.b+AD_OUT_MPR_I.c)>200)
			M_SetFlag(SL_IUBLNCE2);
	else	M_ClrFlag(SL_IUBLNCE2);

}
/*********************************************************************************************************
** 函数名称: Scout
** 功能描述: 系统故障检测及处理程序
** 输入：
** 输出:        
**  释: 
**-------------------------------------------------------------------------------------------------------
** 作者: 
** 日期: 
**-------------------------------------------------------------------------------------------------------
** 修改者：
** 日期:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Scout(void)
{

//  float temp_pgactive,temp_pgreactive;
  Uint16 tempb,tempc;
//------------------根据子程序上报的畔ⅲ卸鲜荰AB_MSG中哪一个-------------------------------------
	if(M_ChkFlag(SL_POWERON)==0)										//若电完成故障则检测故障
	{	
//---------------------------------TZ1中断保护------------------------------------------------------
		if(M_ChkFlag(SL_PDPINTA)!=0)	
		{	
			tempb = *IN2_ADDR;
			tempc = *IN3_ADDR;
			_IN12_DATA = (_IN12_DATA & 0x00FF) | ((tempb<<8) & 0xFF00);
			_IN34_DATA = (tempc & 0x00FF);

			if(M_ChkFlag(SL_IN2_IOVA1)!=0)			M_SetFlag(SL_HIA1);		//网侧A相SKiiP故障
			else if(M_ChkFlag(SL_IN2_IOVB1)!=0)	   	M_SetFlag(SL_HIB1);		//网侧B相SKiiP故障
		    else if(M_ChkFlag(SL_IN2_IOVC1)!=0)	   	M_SetFlag(SL_HIC1);		//网侧C相SKiiP故障
			else if(M_ChkFlag(SL_IN3_VDCOV)!=0)   	M_SetFlag(SL_HUDCOV);	//直流侧过压故障
			else if(M_ChkFlag(SL_IN3_NPRIOV)!=0)  	M_SetFlag(SL_HIACOV1);	//网侧硬件过流故障
//2015-12-7
			else if(M_ChkFlag(SL_IN2_IOVA2)!=0)		M_SetFlag(SL_HIA2);			//电机侧A相SKiiP故障
			else if(M_ChkFlag(SL_IN2_IOVB2)!=0)	    M_SetFlag(SL_HIB2);			//电机侧B相SKiiP故障
		    else if(M_ChkFlag(SL_IN2_IOVC2)!=0)	    M_SetFlag(SL_HIC2);			//电机侧C相SKiiP故障
		    else if(M_ChkFlag(SL_IN3_MPRIOV)!=0)    M_SetFlag(SL_HIACOV2);		//电机侧硬件过流故障
			else if(M_ChkFlag(SL_IN3_IOVDC)!=0)  	M_SetFlag(SL_HIDCOV);		//中间直流斩波IGBT硬件故障20120228atbjtu_chopper	BJTULVRT201204

			else if(M_ChkFlag(SL_PDPASERIES)!=0)	M_ClrFlag(SL_PDPINTA);	//再允许启动变流器	20091107atzy						
			else									M_SetFlag(SL_PDPASERIES);//发生硬件故障,CPLD没有存到故障
		}
		else
		{
			M_ClrFlag(SL_HIA1);   	    									//清故障标志
			M_ClrFlag(SL_HIB1);
			M_ClrFlag(SL_HIC1);
			M_ClrFlag(SL_HUDCOV);
			M_ClrFlag(SL_HIACOV1);  			

			M_ClrFlag(SL_HIA2);    											//清故障标志
			M_ClrFlag(SL_HIB2);	   		
			M_ClrFlag(SL_HIC2);	    
			M_ClrFlag(SL_HIACOV2);
			M_ClrFlag(SL_HIDCOV);  						//20120228	BJTULVRT201204

			M_ClrFlag(SL_PDPASERIES);  			
		}

//---------------------------------TZ2中断保护-------------------------------------------------------
		if(M_ChkFlag(SL_PDPINTB)!=0)	
		{	
			tempb = *IN2_ADDR;
			tempc = *IN3_ADDR;
			_IN12_DATA = (_IN12_DATA & 0x00FF) | ((tempb<<8) & 0xFF00);
			_IN34_DATA = (tempc & 0x00FF);

			if(M_ChkFlag(SL_PDPBSERIES)!=0)	M_ClrFlag(SL_PDPINTB);		//允许启动变流器	20091107atzy						
			else									M_SetFlag(SL_PDPBSERIES);	//发生硬件故障,CPLD没有存到故障
		}
		else
		{
			M_ClrFlag(SL_PDPBSERIES);	
		}

//---------------------------------E-STOP保护-------------------------------------------------------
		if(M_ChkFlag(SL_IN1_EXESTOP)!=0)								//外部急停故障  操作板信号绯∶挥?								
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_estop,DELAY_ESTOP)>=0)   M_SetFlag(SL_ESTOP);		//紧急停止延迟时间到？
			else M_ClrFlag(SL_ESTOP);									//清曛疚?
		}
		else 
		{
			MAIN_LOOP.cnt_estop=0;										//清定时器
			M_ClrFlag(SL_ESTOP);
		}  

//---------------------------------变流器外部硬件故障保护-------------------------------------------------
		if((M_ChkFlag(SL_IN1_EXFAULTOK)==0)||(M_ChkFlag(SL_IN1_MIANFAN)==0))	//外部故障动作或者功率缁收? new							
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_exfault,DELAY_EXFAULT)>=0)   M_SetFlag(SL_EXFAIL);	//外部硬件故障延迟时间到？
			else M_ClrFlag(SL_EXFAIL);							     //清标志位
		}
		else 
		{
			MAIN_LOOP.cnt_exfault=0;                               	 //清定时器
			M_ClrFlag(SL_EXFAIL);
		}	

//-----------------------CANOPENOVER CAN通讯故障-----------------------------------------------------------
	    if(M_ChkFlag(SL_ENPCOPER)==0) 											//上电且选择CAN控制才进行通讯故障
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_canfault,DELAY_CANFAULT)>=0)		//10s  20090816
		  	{
				if(M_ChkCounter(SCI_canopen.cnt_heartbeat,DELAY_CANOPENOVER)>0)
				{                                   						
			   		 M_SetFlag(SL_CANOPENOVER);
//			   		 SCI_canopen.rx_controlword=0;
//					 SCI_canopen.rx_torque=0;
//					 SCI_canopen.rx_angle=0;
				}
				else M_ClrFlag(SL_CANOPENOVER); 
		  	}   
        }
		else  
		{
			MAIN_LOOP.cnt_canfault=0;
			M_ClrFlag(SL_CANOPENOVER);
		}

//-----------------------CBTRIP保护(主断过流脱扣)---------------------------------------

	    if(M_ChkFlag(SL_IN1_CBRESET)==0)							   	//1=主控发来要求主断复位信号
		{		
			if(M_ChkFlag(SL_IN3_CBTRIP)!=0)								//主断过流脱扣故障	090816	
	    	{
	    		if(M_ChkCounter(MAIN_LOOP.cnt_cbtp,DELAY_CBTP)>=0)	   	//主断脱扣最小时间到?
					M_SetFlag(SL_CBTRIP);	
				else 
					M_ClrFlag(SL_CBTRIP);								//清标志位	    
	    	}
			else   	MAIN_LOOP.cnt_cbtp=0;		
		}
		else if(M_ChkFlag(SL_IN3_CBTRIP)==0)
		{
			M_ClrFlag(SL_CBTRIP);								 		//清标志位
			MAIN_LOOP.cnt_cbtp=0;
		}
		else	MAIN_LOOP.cnt_cbtp=0;
	    	    

//-----------------------各开关动作故障110818---------------------------------------
//	if(M_ChkFlag(SL_SWITCHDETECT)==0)
//	{
//----根据胶南风场的bug暂时取消该检测----20121024----
//-----------------------主断合闸/分闸/意外断开故障---------------------------------------
	    if((M_ChkFlag(CL_CBON)!=0 && M_ChkFlag(SL_IN1_CBSTS)==0)||(M_ChkFlag(CL_CBLVC)==0 && M_ChkFlag(SL_IN1_CBSTS)!=0)||(M_ChkFlag(SL_IN1_CBSTS)==0 && M_ChkFlag(CL_CBLVC)!=0 && M_ChkFlag(SL_CBCLOSED)!=0 && M_ChkFlag(SL_IN3_CBTRIP)==0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_cberror,DELAY_CBERROR)>=0)	M_SetFlag(SL_ERROR_CB);	  //2s  
			else M_ClrFlag(SL_ERROR_CB);								 //清标志位
		}
		else 
		{
			MAIN_LOOP.cnt_cberror=0;                                    //清时器
			M_ClrFlag(SL_ERROR_CB);
		} 
		
//-----------------------主接触器闭合/断开故障---------------------------------------
	    if((M_ChkFlag(CL_MAINK)!=0 && M_ChkFlag(SL_IN1_MIANK)==0)||(M_ChkFlag(CL_MAINK)==0 && M_ChkFlag(SL_IN1_MIANK)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_mainkerror,DELAY_MAINKERROR)>=0)	M_SetFlag(SL_ERROR_MAINK);	
			else M_ClrFlag(SL_ERROR_MAINK);								 //清标志位
		}
		else 
		{
			MAIN_LOOP.cnt_mainkerror=0;                                    //清定时器
			M_ClrFlag(SL_ERROR_MAINK);
		}  
		
//-----------------------主滤波器闭合/断开故障---------------------------------------
	    if((M_ChkFlag(CL_MAINK)!=0 && M_ChkFlag(SL_IN1_MIANFILTER)==0)||(M_ChkFlag(CL_MAINK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_mainferror,DELAY_MAINFERROR)>=0)	M_SetFlag(SL_ERROR_MAINF);	
			else M_ClrFlag(SL_ERROR_MAINF);								 //清标志位
		}
		else 
		{
			MAIN_LOOP.cnt_mainferror=0;                                    //清定时器
			M_ClrFlag(SL_ERROR_MAINF);
		}   

//-----------------------定子接触器闭合/断开故障---------------------------------------
	    if((M_ChkFlag(CL_STATORK)!=0 && M_ChkFlag(SL_IN1_STATORK)==0)||(M_ChkFlag(CL_STATORK)==0 && M_ChkFlag(SL_IN1_STATORK)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_stacerror,DELAY_STACERROR)>=0)	M_SetFlag(SL_ERROR_STAC);	
			else M_ClrFlag(SL_ERROR_STAC);								 //清标志位
		}
		else 
		{
			MAIN_LOOP.cnt_stacerror=0;                                    //清定时器
			M_ClrFlag(SL_ERROR_STAC);
		}   
//	}

//-----------------------各开关意外动作故障110818---------------------------------------
/*	else if(M_ChkFlag(SL_SWITCHDETECT)!=0)
	{
//-----------------------主断合闸/分闸故障---------------------------------------
	    if((M_ChkFlag(CL_CBON)!=0 && M_ChkFlag(SL_IN1_CBSTS)==0) || (M_ChkFlag(CL_CBFCON)==0 && M_ChkFlag(SL_IN1_CBSTS)!=0))	     //110723，CL_CBFCON替换CL_CBLVC							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_cberror,DELAY_SWITCHDETECT)>=0)	M_SetFlag(SL_ERROR_CB);	   
			else M_ClrFlag(SL_ERROR_CB);								 //清标志位
		}
		else 
		{
			MAIN_LOOP.cnt_cberror=0;                                    //清定时器
			M_ClrFlag(SL_ERROR_CB);
		} 
		
//-----------------------主接触器闭合/断开故障---------------------------------------
	    if((M_ChkFlag(CL_MAINK)!=0 && M_ChkFlag(SL_IN1_MIANK)==0)||(M_ChkFlag(CL_MAINK)==0 && M_ChkFlag(SL_IN1_MIANK)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_mainkerror,DELAY_SWITCHDETECT)>=0)	M_SetFlag(SL_ERROR_MAINK);	
			else M_ClrFlag(SL_ERROR_MAINK);								 //清标志位
		}
		else 
		{
			MAIN_LOOP.cnt_mainkerror=0;                                    //清定时器
			M_ClrFlag(SL_ERROR_MAINK);
		}  
		
//-----------------------主滤波器闭合/断开故障---------------------------------------
	    if((M_ChkFlag(CL_MAINK)!=0 && M_ChkFlag(SL_IN1_MIANFILTER)==0)||(M_ChkFlag(CL_MAINK)==0 && M_ChkFlag(SL_IN1_MIANFILTER)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_mainferror,DELAY_SWITCHDETECT)>=0)	M_SetFlag(SL_ERROR_MAINF);	
			else M_ClrFlag(SL_ERROR_MAINF);								 //清标志位
		}
		else 
		{
			MAIN_LOOP.cnt_mainferror=0;                                    //清定时器
			M_ClrFlag(SL_ERROR_MAINF);
		}   

//-----------------------定子接触器闭合/断开故障---------------------------------------
	    if((M_ChkFlag(CL_STATORK)!=0 && M_ChkFlag(SL_IN1_STATORK)==0)||(M_ChkFlag(CL_STATORK)==0 && M_ChkFlag(SL_IN1_STATORK)!=0))	     //							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_stacerror,DELAY_SWITCHDETECT)>=0)	M_SetFlag(SL_ERROR_STAC);	
			else M_ClrFlag(SL_ERROR_STAC);								 //清标志位
		}
		else 
		{
			MAIN_LOOP.cnt_stacerror=0;                                    //清定时器
			M_ClrFlag(SL_ERROR_STAC);
		}
	}
*/
//----------------------预充电故障---------------------------------------
	    if((M_ChkFlag(CL_PRE)!=0)&&(M_ChkFlag(CL_MAINK)==0))	     	//							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_preerror,DELAY_PREERROR)>=0)	M_SetFlag(SL_ERROR_PRE);  //15s	
			else M_ClrFlag(SL_ERROR_PRE);								 //清标志位
		}
		else 
		{
			MAIN_LOOP.cnt_preerror=0;                                    //清定时器
			M_ClrFlag(SL_ERROR_PRE);
		}  
 
//-----------------------ENCODFAULT保护(Qep检测,编码器故?)-----------------------------------------
//	    if(((M_ChkFlag(SL_QEPPCO)!=0)||(M_ChkFlag(SL_QCAPDISTURB)!=0)||(M_ChkFlag(SL_QEPZDISTRUB)!=0))&&(M_ChkFlag(SL_OCS_EIN)!=0))	//QEP上溢或者QCAP、QEP Z信号受干扰故障  20090804于cpc							
	    if((M_ChkFlag(SL_QCAPSPDIN)!=0)&&((M_ChkFlag(SL_QEPPCO)!=0)||(M_ChkFlag(SL_QCAPDISTURB)!=0)||(M_ChkFlag(SL_QEPZDISTRUB)!=0)||(M_ChkFlag(SL_QEPPCDE)!=0)))	//QEP上溢或者QCAP、QEP Z/A/B信号受干扰故障  201007BJTULVRT						
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_encofault,DELAY_ENCOFAULT)>=0)	//150ms 2010atcpc									//置鲎俜锭标志位
				M_SetFlag(SL_ENCODFAULT);				     			    //置标志位
		}
		else 
		{
			M_ClrFlag(SL_ENCODFAULT);									//清标志位
			MAIN_LOOP.cnt_encofault=0;
		} 

//-----------------------电机工作转速范围判断-----------------------------------------------------
//	    if(M_ChkFlag(SL_OCS_EIN)!=0)									//bit0才进行转速龇段卸? 201005atcpc
	    if((M_ChkFlag(SL_OCS_EIN)!=0)&&(M_ChkFlag(SL_IN1_CBSTS)!=0))	//bit0才进行转速超出范围判断 20120310
		{
//			if((M_ChkFlag(SL_MSPOUT)==0)&& ((PRO.speedflt<_SC_MSPEED1)||(PRO.speedflt>_SC_MSPEED2)))  //加个延时判断20090817
			if((M_ChkFlag(SL_MSPOUT)==0) && (M_ChkFlag(SL_MPR_PWMOUT)!=0) && ((PRO.speedflt<_SC_MSPEED1)||(PRO.speedflt>_SC_MSPEED2)))  //加个延迸卸?220120310
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_speedout,DELAY_SPEEDOUT)>=0)	M_SetFlag(SL_MSPOUT);	//50ms 20091022atzy									//置鲎俜锭标志位
			}
			else if((PRO.speedflt>(_SC_MSPEED1 + SC_MSPD_HW))&&(PRO.speedflt<(_SC_MSPEED2 - SC_MSPD_HW)))   //判断加回差201005atcpc
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
//-----------------------网侧变流器软件过流判断-----------------------------------------------------
		if((M_ChkFlag(SL_SIAC1)==0)&&(PRO.NPR_iac>_SC_IACOV1))
		{
			M_SetFlag(SL_SIAC1);									//置软件过流标疚?
		}
		else if((M_ChkFlag(SL_SIAC1)!=0)&&(PRO.NPR_iac<(_SC_IACOV1-SC_IAC_HW))) 
		{
			M_ClrFlag(SL_SIAC1);									//清软件过流标志位
		}
 

//------------------------电机侧变流器软件过流判断--------------------------------------------------
		if((M_ChkFlag(SL_SIAC2)==0)&&(PRO.MPR_iac>_SC_IACOV2))
		{
			M_SetFlag(SL_SIAC2);									//置软件过流标志位
		}
		else if((M_ChkFlag(SL_SIAC2)!=0)&&(PRO.MPR_iac<(_SC_IACOV2-SC_IAC_HW))) 
		{
			M_ClrFlag(SL_SIAC2);									//清软件过流标志位
		}
*/
/*
//-------------------------中间直流电压软件欠压判断----有问题！该故障屏蔽---------------------------------------------
	    if((M_ChkFlag(SL_CHARGEOK)!=0)&&(M_ChkFlag(SL_NPR_PWMOUT)!=0))//预充电完成后才进星费古卸?
		{
			if((M_ChkFlag(SL_SUDCLV)==0)&&(PRO.udc<_SC_UDCLV))	
			{
				M_SetFlag(SL_SUDCLV);									//之前没有欠压，此刻检测出欠，置欠压标志
			}	
			else if((M_ChkFlag(SL_SUDCLV)!=0)&&(PRO.udc>=(_SC_UDCLV+SC_UDC_HW)))	
			{
				M_ClrFlag(SL_SUDCLV);									//之前欠压，此刻检测出超过(欠压值加回差)，清欠压标志
			}		
		}
		else	M_ClrFlag(SL_SUDCLV);				
*/
//-----------------------中间直流斩波电流过流判断-BJTULVRT201204----------------------------------------------------------
		//chooper启动中
		if(((_COMMAND2&0x0002)==0)&&(PRO.CHOP_idc>10)&&((M_ChkFlag(SL_LV_STATE)!=0)||(M_ChkFlag(SL_HV_STATE)!=0)))
		{
			if(MAIN_LOOP.cnt_idcov!=65535)		MAIN_LOOP.cnt_idcov++;	
			if (M_ChkCounter(MAIN_LOOP.cnt_idcov,1294)>=0) //1294  对应I2RT=350kJ
			{
				M_SetFlag(SL_IDCOV);
			}		
		}
		else
		{
			MAIN_LOOP.cnt_idcov = 0;
			M_ClrFlag(SL_IDCOV);
		}
		
//------------------------中间直流电压软件过压判断--------------------------------------------------
		if((M_ChkFlag(SL_SUDCOV)==0)&&(PRO.udc>_SC_UDCOV))	                   
		{
			M_SetFlag(SL_SUDCOV);	 	//
		}
		else if((M_ChkFlag(SL_SUDCOV)!=0)&&(PRO.udc<=(_SC_UDCOV-SC_UDC_HW)))   M_ClrFlag(SL_SUDCOV);  //之前过压，此刻检测低于过压值减回差	


//-----------------------网压欠压软件判断(线压有效值)-----------------------------------------------
	   if(M_ChkCounter(MAIN_LOOP.cnt_uaclv1,DELAY_UACLV1)>0)	//10s 上电延时判断网侧软欠压故障 此处估计低穿时低电压都被滤掉了，所以不会报错
	   {
		if(M_ChkFlag(SL_UACLV1)==0)
		{
			if((PRO.NPR_uab<_SC_UACLV1)||(PRO.NPR_ubc<_SC_UACLV1))	M_SetFlag(SL_UACLV1);  //20091026atzy 两线电压任意一个不达要求,置欠曛?
		}
		else 
		{
			if((PRO.NPR_uab>=(_SC_UACLV1+SC_UAC_HW))&&(PRO.NPR_ubc>=(_SC_UACLV1+SC_UAC_HW)))	M_ClrFlag(SL_UACLV1); //20091026atzy 两个线电压馗吹秸：笄迩饭标志	
		}
	   }					
//----------------------网压过压软件判断(线压有效值)------------------------------------------------
		if(M_ChkFlag(SL_HV_STATE)==0)
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_uacover,10)>=0) //高电压恢复10s后判断
			{
			if((PRO.NPR_uab>_SC_UACOV1)||(PRO.NPR_ubc>_SC_UACOV1))	 M_SetFlag(SL_UACOV1);
			else if((PRO.NPR_uab<=(_SC_UACOV1-SC_UAC_HW))&&(PRO.NPR_ubc<=(_SC_UACOV1-SC_UAC_HW)))	M_ClrFlag(SL_UACOV1);
			}
		}
		else
			MAIN_LOOP.cnt_uacover = 0;
		


//------------------------电网频率故障--------------------------------------------------------------
/*		if(M_ChkFlag(SL_IN1_CBSTS)!=0)     									//主断闭合再判断网频故障 20090816
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_cbfreq,DELAY_CBFREQ)>=0)      	//1s
			{
*/				if(M_ChkFlag(SL_GRDFQE)!=0)									//10个网压周期(200ms),则置频率错误标志
				{
					if(M_ChkCounter(MAIN_LOOP.cnt_freq,DELAY_FREQ)>=0)	 M_SetFlag(SL_FE1);	
				}	
				else
				{
					M_ClrFlag(SL_FE1);									   	//否则清频率错误标志
					MAIN_LOOP.cnt_freq=0;                                  	//计时器每1ms加1
				}
/*			}
		}
		else   	
		{
			MAIN_LOOP.cnt_cbfreq=0;	
			M_ClrFlag(SL_FE1);
		}
*/	
//-----------------------定子电流过流判断-----------------------------------------------------------
		if((M_ChkFlag(SL_SIOVST)==0)&&((PRO.STA_iac>_SC_IACOVST)||(PRO.STA_icb>_SC_IACOVST)))
		{
			M_SetFlag(SL_SIOVST);									//置定子电流过流标志位
		}
		else if((M_ChkFlag(SL_SIOVST)!=0)&&(PRO.STA_iac<(_SC_IACOVST-SC_IAC_HW))&&(PRO.STA_icb<(_SC_IACOVST-SC_IAC_HW))) 
		{
			M_ClrFlag(SL_SIOVST);									//清定子电流过流标志位
		}
/*
//-----------------------中间电压不稳定---暂时不用201005atcpc----------------------------------------------------------
        if(M_ChkFlag(SL_NPR_PWMOUT)!=0 && M_ChkFlag(SL_MPR_PWMOUT)!=0)
		{
		   if((AD_OUT_UDC<(_URF - 70))||(AD_OUT_UDC>(_URF + 70)))  //中间电压超出范围后置故障标志位 new
		   {
		       if(M_ChkCounter(MAIN_LOOP.cnt_steadyfb,DELAY_UDCWAVE)>0)
			   {                                   //DELAY_STEADYFB，cnt_steadyfb与sysctrl里面的是同一个
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
//-----------------------SCR 断不开故障LVRT---201007BJTULVRT----------------------------------------------------------
 //       if((M_ChkFlag(SL_SCRON)!=0)&&(M_ChkFlag(SL_LV_SCRIZERO)==0))
        if(M_ChkFlag(SL_SCRIZERO)==0)	//20110829HC
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

//------------------------网侧接地故障-三相电流不平衡-----20100302---------------------------------------------------------
		if(M_ChkFlag(SL_IUBLNCE1)!=0)
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_gfault1,DELAY_GFAULT)>=0)	 M_SetFlag(SL_GFAULT1);	
		}	
		else
		{
			M_ClrFlag(SL_GFAULT1);									  
			MAIN_LOOP.cnt_gfault1=0;                               
		}		

//------------------------机侧接地故障-三相电流不平衡-----20100302---------------------------------------------------------
		if(M_ChkFlag(SL_IUBLNCE2)!=0)
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_gfault2,DELAY_GFAULT)>=0)	 M_SetFlag(SL_GFAULT2);	
		}	
		else
		{
			M_ClrFlag(SL_GFAULT2);									  
			MAIN_LOOP.cnt_gfault2=0;                               
		}		

//-----------------------软件温度反馈SKIIP超温故障判断----------------------------------------------
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
			M_SetFlag(SL_SKTOV);									//置SKIIP超温故障标志位
		}
		else if((M_ChkFlag(SL_SKTOV)!=0)&&(AMUX.skiiptempmax < (_SC_SKPTOV-10))) 
		{
			M_ClrFlag(SL_SKTOV);									//清SKIIP超温故障标志位
		}
        
//----------------------网侧SKIIP超温故障-----201005atcpc-----------------------------------------------------			
		if(M_ChkFlag(SL_IN2_TAOV)!=0)
		{	
			if(M_ChkCounter(MAIN_LOOP.cnt_nskiptov,DELAY_NSKIPTOV)>=0)	M_SetFlag(SL_TAOV); 	//贸温标志位
		}
       	else
		{
			MAIN_LOOP.cnt_nskiptov=0; 								//清延时计数
			M_ClrFlag(SL_TAOV);										//清超温标志位
		}

//--------------------电机侧SKIIP超温故障-----201005atcpc------------------------------------------------------	
		if(M_ChkFlag(SL_IN2_TBOV)!=0)
		{	
			if(M_ChkCounter(MAIN_LOOP.cnt_mskiptov,DELAY_MSKIPTOV)>=0)	M_SetFlag(SL_TBOV); 	//置超温标志位
		}
       	else
		{
			MAIN_LOOP.cnt_mskiptov=0; 								//清延时计数
			M_ClrFlag(SL_TBOV);										//清超温标志位
		}

//----------------------网侧电感超温故障----------------------------------------------------------			
		if((M_ChkFlag(SL_NPR_TLOV)==0)&&(PRO.NPR_TLOV>_SC_NPR_TLOV))
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_nprtlov,DELAY_NPRTLOV)>=0)	M_SetFlag(SL_NPR_TLOV);	//置网侧电感超温标志位
		}
		else if((M_ChkFlag(SL_NPR_TLOV)!=0)&&(PRO.NPR_TLOV<(_SC_NPR_TLOV - 5))) 
		{
			M_ClrFlag(SL_NPR_TLOV);									//逋嗟绺谐卤曛疚?
			MAIN_LOOP.cnt_nprtlov=0;
		} 


//--------------------电电感超温故障-----------------------------------------------------------	
		if((M_ChkFlag(SL_MPR_TLOV)==0)&&(PRO.MPR_TLOV>_SC_MPR_TLOV))
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_mprtlov,DELAY_MPRTLOV)>=0)	M_SetFlag(SL_MPR_TLOV);	//置机侧电感超温标志位
		}
		else if((M_ChkFlag(SL_MPR_TLOV)!=0)&&(PRO.MPR_TLOV<(_SC_MPR_TLOV - 5))) 
		{
			M_ClrFlag(SL_MPR_TLOV);									//清机侧电感超温标志位
			MAIN_LOOP.cnt_mprtlov=0;
		} 


//-----------------------变流器有功功率过载判断-----------------------------------------------------------
/*		temp_pgactive = abs(PRO.Pgactive);							//20090816
		temp_pgactive = temp_pgactive * 0.001;
		if((M_ChkFlag(SL_PGOV)==0)&&(temp_pgactive>_SC_PGOV))
		{
			M_SetFlag(SL_PGOV_COM);
			if(M_ChkCounter(MAIN_LOOP.cnt_pgovload,DELAY_PGOVLOAD)>0)  	//10s
				M_SetFlag(SL_PGOV);										//置变流器有功β使乇曛疚?
		}
		else if((M_ChkFlag(SL_PGOV)!=0)&&(temp_pgactive<(_SC_PGOV-SC_POWOROV_HW))) 
		{
			M_ClrFlag(SL_PGOV);
			M_ClrFlag(SL_PGOV_COM);										//清变流器有功功率过载标志位
			MAIN_LOOP.cnt_pgovload=0;
		}
		else  MAIN_LOOP.cnt_pgovload=0;

//-----------------------变流器无功功率过载判断-----------------------------------------------------------
        temp_pgreactive = abs(PRO.Pgreactive);							//20090816
        temp_pgreactive = temp_pgreactive * 0.001;
		if((M_ChkFlag(SL_QGOV)==0)&&(temp_pgreactive>_SC_QGOV))
		{
			M_SetFlag(SL_QGOV_COM);										//20091007
			if(M_ChkCounter(MAIN_LOOP.cnt_qgovload,DELAY_QGOVLOAD)>0)  	//10s
				M_SetFlag(SL_QGOV);										//置变流器无功功率过载标志位
		}
		else if((M_ChkFlag(SL_QGOV)!=0)&&(temp_pgreactive<(_SC_QGOV-SC_POWOROV_HW))) 
		{
			M_ClrFlag(SL_QGOV);	
			M_ClrFlag(SL_QGOV_COM);										//清变流器无功功率过载标志位
			MAIN_LOOP.cnt_qgovload=0;
		} 
		else MAIN_LOOP.cnt_qgovload=0;
*/
//-------增加低穿超时限制---------用无功过载故障位--------2016-2-17-------------
	if(M_ChkFlag(SL_LV_STATE)!=0)
	{
		if (M_ChkCounter(MAIN_LOOP.cnt_hvlv_detect3,DELAY_LVTEST3)>=0)
		M_SetFlag(SL_QGOV);			
	}
	else if(M_ChkFlag(SL_HV_STATE)!=0)
	{
		if (M_ChkCounter(MAIN_LOOP.cnt_hvlv_detect3,12)>=0)
		M_SetFlag(SL_QGOV);			
	}
	else
	{
		MAIN_LOOP.cnt_hvlv_detect3=0;
		M_ClrFlag(SL_QGOV);
	}

//-----------------------定子同步并网失败故障---------------------------------------
/*	    if(M_ChkFlag(SL_OCS_SYSRUN)!=0 && M_ChkFlag(SL_MPR_SYNOK)==0 && M_ChkFlag(SL_ENPCOPER)==0 )	//PC调试不进行该故障判断							
     	{
			if(M_ChkCounter(MAIN_LOOP.cnt_synfail,120000)>=0)	M_SetFlag(SL_SYNFAIL);	  //2min  201005atcpc
			else M_ClrFlag(SL_SYNFAIL);								 	//清标志位
		}
		else 
		{
			MAIN_LOOP.cnt_synfail=0;                                    //清定时器
			M_ClrFlag(SL_SYNFAIL);
		} 
*/
//--------20130801--------
	    if(M_ChkFlag(SL_OCS_SYSRUN)!=0 && M_ChkFlag(SL_MPR_SYNOK)==0 && M_ChkFlag(SL_ENPCOPER)==0 )	//PC调试不进行该故障判断							
//		if(M_ChkFlag(SL_MPR_START)!=0 && M_ChkFlag(SL_MPR_SYNOK)==0 && M_ChkFlag(SL_ENPCOPER)==0 )	//PC调试不进行该故障判断							
		{
			if(M_ChkCounter(MAIN_LOOP.cnt_synfail,60000)>=0)
			{
				M_SetFlag(SL_SYNFAIL);	  //2min  201005atcpc
				ecpostep1 = 0;
				ecpostep2 = 0;
				ecpostep3 = 0;
				MAIN_LOOP.cnt_synfail = 0;

			}
			else M_ClrFlag(SL_SYNFAIL);								 	//清标志位
		}
		else 
		{
//			if(M_ChkFlag(SL_ENCODEPOS_OK)!=0 && M_ChkFlag(SL_ECPOSSEARCH)==0)	MAIN_LOOP.cnt_synfail=0;		//清定时器
			if(M_ChkFlag(SL_ECPOSSEARCH)==0)	MAIN_LOOP.cnt_synfail=0;		//清定时器
			M_ClrFlag(SL_SYNFAIL);
		}

//--------------------------------得到TAB_MSG中的故闲蚝?-systest------------------------------------------
		_MSG_SCOUT2 = MSG_NONE;												//先将MSG清零
		
		if(M_ChkFlag(SL_CODEOK)==0)				_MSG_SCOUT2=MSG_CODEOK;     //1=功能码未校验完毕
		
		else if(M_ChkFlag(SL_EE_FAIL)!=0) 		_MSG_SCOUT2=MSG_EE_FAIL;    //2=EEPROM故障

		else if(M_ChkFlag(SL_ESTOP)!=0) 	    _MSG_SCOUT2=MSG_ESTOP;      //3=紧急停机故障

		else if(M_ChkFlag(SL_CBTRIP)!=0) 	    _MSG_SCOUT2=MSG_CBTRIP;     //4=主断脱扣故障

		else if(M_ChkFlag(SL_EXFAIL)!=0) 	 	_MSG_SCOUT2=MSG_EXFAULT;    //5=变流器外部硬件故障

		else if(M_ChkFlag(SL_CANOPENOVER)!=0) 	_MSG_SCOUT2=MSG_CAN_FAIL;   //6=CAN通讯故障//controltest

		else if(M_ChkFlag(SL_ENCODFAULT)!=0) 	_MSG_SCOUT2=MSG_ENCODFAULT; //7=编码器故障//controltest

		else if(M_ChkFlag(SL_ERROR_CB)!=0) 	 	_MSG_SCOUT2=MSG_CBERROR;    //8=主断合闸故障

		else if(M_ChkFlag(SL_ERROR_PRE)!=0) 	_MSG_SCOUT2=MSG_PREERROR;   //9=预充电故障

		else if(M_ChkFlag(SL_ERROR_MAINK)!=0) 	_MSG_SCOUT2=MSG_MAINKERROR; //10=主接触器闭合故障

		else if(M_ChkFlag(SL_ERROR_MAINF)!=0) 	_MSG_SCOUT2=MSG_MAINFERROR; //11=主滤波器闭合故障

		else if(M_ChkFlag(SL_ERROR_STAC)!=0) 	_MSG_SCOUT2=MSG_STACERROR;  //12=定子哟テ鞅蘸瞎收?

		else if(M_ChkFlag(SL_HIA1)!=0)			_MSG_SCOUT2=MSG_HIA1;     	//13=网侧变流器A相SKIIP故障

		else if(M_ChkFlag(SL_HIB1)!=0)			_MSG_SCOUT2=MSG_HIB1;     	//14=网侧变流器B相SKIIP故障

		else if(M_ChkFlag(SL_HIC1)!=0)			_MSG_SCOUT2=MSG_HIC1;     	//15=网侧变流器C相SKIIP故障
		
		else if(M_ChkFlag(SL_HIA2)!=0)			_MSG_SCOUT2=MSG_HIA2;     	//16=电机侧变流器A相SKIIP故?

		else if(M_ChkFlag(SL_HIB2)!=0)			_MSG_SCOUT2=MSG_HIB2;     	//17=电机侧变流器B相SKIIP故障

		else if(M_ChkFlag(SL_HIC2)!=0)			_MSG_SCOUT2=MSG_HIC2;     	//18=电机侧变流器C相SKIIP故障

		else if(M_ChkFlag(SL_HUDCOV)!=0)		_MSG_SCOUT2=MSG_UDCOV;    	//19=变流器直流母线硬件过压故障

        else if(M_ChkFlag(SL_HIACOV1)!=0)		_MSG_SCOUT2=MSG_HIAC1;    	//20=网侧硬件过流故障

		else if(M_ChkFlag(SL_HIACOV2)!=0)		_MSG_SCOUT2=MSG_HIAC2;    	//21=电机侧硬件过流故障

        else if(M_ChkFlag(SL_PDPASERIES)!=0)	_MSG_SCOUT2=MSG_PDPASERIES;	//22=网侧严重故障

		else if(M_ChkFlag(SL_PDPBSERIES)!=0)	_MSG_SCOUT2=MSG_PDPBSERIES;	//23=网侧严重故障

		else if(M_ChkFlag(SL_MSPOUT)!=0) 	    _MSG_SCOUT2=MSG_MSPEEDOUT;  //24=转速超出范围故障

		else if(M_ChkFlag(SL_SIAC1)!=0)			_MSG_SCOUT2=MSG_SIAC1;    	//25=网侧软件过流故障

		else if(M_ChkFlag(SL_SIAC2)!=0)			_MSG_SCOUT2=MSG_SIAC2;    	//26=电机侧软件过流故障

		else if(M_ChkFlag(SL_FE1)!=0)			_MSG_SCOUT2=MSG_FE1;      	//27=网侧频率不符故障//controltest

		else if(M_ChkFlag(SL_SUDCOV)!=0)		_MSG_SCOUT2=MSG_SUDCOV;   	//28=软件中间直流电压过压
		
		else if(M_ChkFlag(SL_UACOV1)!=0)		_MSG_SCOUT2=MSG_SUACOV1;  	//29=软件网压交流过压

//		else if(M_ChkFlag(SL_SUDCLV)!=0)		_MSG_SCOUT2=MSG_SUDCLV;   	//30=软件中间绷鞯缪骨费?
		else if((M_ChkFlag(SL_IDCOV)!=0)||(M_ChkFlag(SL_HIDCOV)!=0))	_MSG_SCOUT2=MSG_SIDCOV;   	//30=改为dc-chopper电流保护BJTULVRT201204

//		else if(M_ChkFlag(SL_UACLV1)!=0 || M_ChkFlag(SL_UACLV2)!=0)		_MSG_SCOUT2=MSG_SUACLV1;  	//31=软件网压交流欠压 201007BJTULVRT				
		else if(M_ChkFlag(SL_UACLV1)!=0)		_MSG_SCOUT2=MSG_SUACLV1;  	//31=软件菇涣髑费?
		
		else if(M_ChkFlag(SL_UDCWAVE)!=0)		_MSG_SCOUT2=MSG_UDCWAVE;  	//32=中间电压波动故障

        else if(M_ChkFlag(SL_SIOVST)!=0)		_MSG_SCOUT2=MSG_SIOVST;   	//33=软件检测定子过流故障
		
		else if(M_ChkFlag(SL_GFAULT1)!=0)		_MSG_SCOUT2=MSG_GFAULT1;   	//34=网侧接地故障
		
		else if(M_ChkFlag(SL_GFAULT2)!=0)		_MSG_SCOUT2=MSG_GFAULT2;  	//35=机侧接地故障

		else if(M_ChkFlag(SL_TAOV)!=0)			_MSG_SCOUT2=MSG_TOV1;	  	//36=网侧SKIIP超温//controltest
		
		else if(M_ChkFlag(SL_TBOV)!=0)			_MSG_SCOUT2=MSG_TOV2;  	  	//37=电机侧SKIIP超温//controltest

		else if(M_ChkFlag(SL_SKTOV)!=0)			_MSG_SCOUT2=MSG_SKTOV;     	//38=软件判断SKIIP超温

        else if(M_ChkFlag(SL_NPR_TLOV)!=0)		_MSG_SCOUT2=MSG_TLOV1;     	//39=电网侧电感超温

		else if(M_ChkFlag(SL_MPR_TLOV)!=0)		_MSG_SCOUT2=MSG_TLOV2;     	//40=电机侧电感超温

//		else if(M_ChkFlag(SL_PGOV)!=0)		    _MSG_SCOUT2=MSG_PGOV;      	//41=变流器有功功率过载

		else if(M_ChkFlag(SL_QGOV)!=0)	    	_MSG_SCOUT2=MSG_QGOV;      	//42=变流器无功功率过载

		else if(M_ChkFlag(SL_SYNFAIL)!=0)	    _MSG_SCOUT2=MSG_STASYNFAIL; //43=定子同步并网失败故障

		else if(M_ChkFlag(SL_PHORDE)!=0) 		_MSG_SCOUT2=MSG_PHORDE;     //44=电网相序错误 //controltest

//------------------------PC示波器故障波形存储处理--20100308-------------------------------------------------------

//		if(_MSG_SCOUT2!=MSG_NONE)												//本次有故障发生
		if((_MSG_SCOUT2!=MSG_NONE)&&(_MSG_SCOUT2!=6))		//本次有故障发生 不存储canopen故障20121129
		{
			if(_PCEROTRIG==0)					M_SetFlag(SL_TRIG_ERRDSAVE);	//所有故障触发锁存 201005atcpc
			else if(_MSG_SCOUT2==_PCEROTRIG)	M_SetFlag(SL_TRIG_ERRDSAVE);	//某收洗シ⑺?
			else if((_PCEROTRIG==50)&&(_MSG_SCOUT2==14||_MSG_SCOUT2==15||_MSG_SCOUT2==16||_MSG_SCOUT2==21||_MSG_SCOUT2==23))
				M_SetFlag(SL_TRIG_ERRDSAVE);	 								//所有网侧硬件故障触发锁存
			else if((_PCEROTRIG==51)&&(_MSG_SCOUT2==17||_MSG_SCOUT2==18||_MSG_SCOUT2==19||_MSG_SCOUT2==22||_MSG_SCOUT2==24))
				M_SetFlag(SL_TRIG_ERRDSAVE);	 								//所有机侧硬件故障触发锁存
		}
		
		if((_PCEROTRIG==99)&&(M_ChkFlag(SL_PC_ERRDATASAVE)!=0))
				M_SetFlag(SL_TRIG_ERRDSAVE);	 								//PC触发外部RAM数据锁存 201005atcpc
		if((_PCEROTRIG==0)&&(M_ChkFlag(CL_CHOPTEST)!=0))	M_SetFlag(SL_TRIG_ERRDSAVE);	//斩波触发2013-12-3

//------------------------故障处理--systest-------------------------------------------------------

//NO1-------原来没有故障本次有故障(或)原来有故障但本次级别更高-------------------（故障升级）------
		if(((_MSG_SCOUT2!=MSG_NONE)&&(M_ChkFlag(SL_ERROR)==0))||((_MSG_SCOUT2!=MSG_NONE)&&(M_ChkFlag(SL_ERROR)!=0)&&(TAB_MSG[_MSG_SCOUT2].rank > TAB_MSG[_MSG_SCOUT1].rank)))
		{													
			M_SetFlag(SL_ERROR);							//置故障标志位
			M_SetFlag(SL_DISPLAY5);                         //置系统故障指示

//----------------------------鹿收鲜粜员晔?-------------------------------------------------------												
			if((TAB_MSG[_MSG_SCOUT2].attr & OFFCB)==OFFCB)		M_SetFlag(SL_OFFCB);	//严重急停属性
			else												M_ClrFlag(SL_OFFCB);

			if((TAB_MSG[_MSG_SCOUT2].attr & SHUT)==SHUT)		M_SetFlag(SL_SHUT);		//一般停机属性
			else												M_ClrFlag(SL_SHUT);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & N_RCVR)==N_RCVR)	M_SetFlag(SL_NRCVR);	//不能恢复属性
			else												M_ClrFlag(SL_NRCVR);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & I_RCVR)==I_RCVR)	M_SetFlag(SL_IRCVR);	//立即恢复属性
			else												M_ClrFlag(SL_IRCVR);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & D_RCVR)==D_RCVR)	M_SetFlag(SL_DRCVR);	//延时恢复属性
			else												M_ClrFlag(SL_DRCVR);
						
			if((TAB_MSG[_MSG_SCOUT2].attr & CNT)==CNT)			M_SetFlag(SL_CNT);		//计次数属性
			else												M_ClrFlag(SL_CNT);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & OT_SER)==OT_SER)	M_SetFlag(SL_OTSER);	//超时严重属性
			else												M_ClrFlag(SL_OTSER);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & SAVE)==SAVE)		M_SetFlag(SL_SAVE);		//需要保存故障信息属性
			else												M_ClrFlag(SL_SAVE);		

			if((TAB_MSG[_MSG_SCOUT2].attr & WARNING)==WARNING)	M_SetFlag(SL_WARNING);	//报警属性
			else												M_ClrFlag(SL_WARNING);	

//----------------------------------根据故障属性判断系统动作---------------------------------------
						
			if(M_ChkFlag(SL_NRCVR)!=0)		//不可复位故障
			{
				M_SetFlag(SL_NO_RESET);		//不可复位故障,置 禁止复位 标志
				M_ClrFlag(SL_NRCVR);		//清不可复位故障
			}

			if(M_ChkFlag(SL_DRCVR)!=0)		//延时复位故障
			{
				M_SetFlag(SL_DL_RESET);		//首先置 禁止复位 标志
			}											

/*			if(M_ChkFlag(SL_IRCVR)!=0)		//允许立即复位故障,没有操作等待复位
			{
//				M_ClrFlag(SL_NO_RESET);		//清 禁止复位 标志
				M_ClrFlag(SL_IRCVR);							
			}											
*/
            if(M_ChkFlag(SL_OFFCB)!=0)		//现毓收?
			{
				M_SetFlag(SL_SERIESTOP);	//置 严重故障停机 标志											
				M_ClrFlag(SL_OFFCB);
			}

            if(M_ChkFlag(SL_SHUT)!=0)		//一般故障
			{
				M_SetFlag(SL_ERRSTOP);		//置 一般故障停机 标志												
				M_ClrFlag(SL_SHUT);
			}

    		if(M_ChkFlag(SL_CNT)!=0)
			{
				if(R_SCOUT == _MSG_SCOUT2)	_SY_RTRN++;				//重复发生同一故障
				else
				{
					R_SCOUT = _MSG_SCOUT2;
					_SY_RTRN=0;
				}
				if(_SY_RTRN>=_SC_RTRN)		M_SetFlag(SL_NO_RESET);	//置 禁止复位 标志	
				M_ClrFlag(SL_CNT);				
			}

        	if(M_ChkFlag(SL_WARNING)!=0)   //暂时没有进行何操作,仅通过通讯发给主控
			{
				M_ClrFlag(SL_WARNING);
			} 

			MAIN_LOOP.cnt_rcvr=0;			//清延时恢复计数器	
			_MSG_SCOUT1 = _MSG_SCOUT2;		//本次故障信息转存
								
//----------------------------更新故霞锹疾⑾蛏衔换ü收?------------------------------------------						
			if((M_ChkFlag(SL_SAVE)!=0)&&(M_ChkFlag(SL_EEBUSY_ERRSAVE)==0))			
			{
				MAIN_LOOP.cnt_rcvr=0;									//逖邮被指醇剖?	

				M_SetFlag(SL_EEASK_ERRSAVE);							//EEPROM操作肭?
				M_ClrFlag(SL_SAVE);
				_BA_ERR1 = _BA_ERR2;									//故障信息保存
				_BA_ERR2 = _BA_ERR3;
				_BA_ERR3 = _BA_ERR4;
				_BA_ERR4 = _MSG_SCOUT2;
       			
				_BA_EURF   = (int16)RUN.urf;							//中间直流电压给定
				_BA_EUDC   = (int16)AD_OUT_UDC;							//中间直流电压
				_BA_EMIDRF = (int16)(RUN.mpridrf * 10);					//d轴电流给定
				_BA_ENIQRF = (int16)(RUN.npriqrf * 10);					//q轴电流给定
				_BA_EMIQRF = (int16)(RUN.mpriqrf * 10);					//q轴电流给定
				_BA_ETOQRF = (int16)RUN.toqrf;							//转矩电流给定
                _BA_EAGLRF = (int16)RUN.aglrf;            			 //无功角度指令

				_BA_EIA1  = (int16)(AD_OUT_NPR_I.a * 10);				//网侧变流器,A相电流瞬时值
				_BA_EIB1  = (int16)(AD_OUT_NPR_I.b * 10);				//网侧变流器,B相电流瞬时值
				_BA_EIC1  = (int16)(AD_OUT_NPR_I.c * 10);				//网侧变流器,C相缌魉彩敝?
				_BA_EIA2  = (int16)(AD_OUT_MPR_I.a * 10);				//电机侧变流器,A相电流瞬时值
				_BA_EIB2  = (int16)(AD_OUT_MPR_I.b * 10);	            //电机侧变流?,B相电流瞬时值
				_BA_EIC2  = (int16)(AD_OUT_MPR_I.c * 10);	            //电机侧变流器,C相电流瞬时值
				
				_BA_EUAB1  = (int16)PRO.NPR_uab;						//网侧ab线电压
				_BA_EUBC1  = (int16)PRO.NPR_ubc;						//网侧bc线电压
				_BA_EUAB2  = (int16)PRO.STA_uab;						//定子侧ab线电压
				_BA_EUBC2  = (int16)PRO.STA_ubc;						//定子侧bc线电压

//				_BA_EUAB0    = (int16)DISP.grd_uab;			       		//主断前网侧ab线压
//            	_BA_EUBC0    = (int16)DISP.grd_ubc;			       		//主断前网侧bc线压
				_BA_EUAB0    = (int16)(AD_OUT_DC_I.b);			       	//主断前网侧ab线压BJTULVRT201204
            	_BA_EUBC0    = (int16)GRD_Ut;			       			//主断前网侧bc线压

				_BA_ETLAC  = (int16)PRO.NPR_TLOV;                           //网侧电感温度
//				_BA_ETLAC  = (int16)SCI_canopen.cnt_heartbeat;             //网侧绺形露? 2014-05-06LJD临时修改CANopen
				
				_BA_ETLDUDT= (int16)PRO.MPR_TLOV;                           //机侧电感温度
				_BA_ETSKIIP= (int16)AMUX.skiiptempmax;                      //SKIIP温度	

				_BA_EFREQ  = (int16)(CAP4.freq * 10);						//电网频率
				_BA_ESPEED = (int16)(PRO.speed);							//电机转速 

	            _BA_ENPRUD = (int16)TRS_NGS_U.d;              			//d轴主断后网压反馈20091026atzy
				_BA_ENPRUQ = (int16)TRS_NGS_U.q;          				//q轴主断后网压反馈20091026atzy
	            _BA_ENPRUD2 = (int16)TRS_NGS_U.dflt;          			//d轴主断后网压反馈 滤波后20091026atzy
				_BA_ENPRUQ2 = (int16)TRS_NGS_U.qflt;          			//q轴主断后网压反馈 瞬ê?20091026atzy
	            _BA_ENUDOUT = (int16)TRS_NPR_U.d;          				//网侧d轴电压输出20091026atzy
				_BA_ENUQOUT = (int16)TRS_NPR_U.q;          				//网侧q轴电压输出20091026atzy

				_BA_EPIONU  = (int16)(PI_NPR_U.out * 10);					//单籄
				_BA_EPIONID = (int16)(PI_NPR_Id.out* 10);					//单位V
				_BA_EPIONIQ	= (int16)(PI_NPR_Iq.out* 10);					//单位V

				_BA_EMEXI   = (int16)(RUN_mpridrf_exi * 10);				//单位A  机侧励磁理论计算值显示 cpc
				_BA_EPIOMID = (int16)(PI_MPR_Id.out* 10);					//单籚
				_BA_EPIOMIQ	= (int16)(PI_MPR_Iq.out* 10);					//单位V

//				_BA_ESTAIAC = (int16)(DISP.sta_iac * 10);
//				_BA_ESTAIBA = (int16)(DISP.sta_iba * 10);
//				_BA_ETOQFB  = (int16)DISP.toqfb;				        	//转矩反馈
//				_BA_EPSTA   = (int16)(DISP.Psactive * 0.01);          		//定子侧有功功率显示kW,DOT1 *10/1000=100
//				_BA_EPNPR   = (int16)(DISP.Pnactive * 0.01);         		//网侧有功功率显示
//				_BA_EPGRID  = (int16)(DISP.Pgactive * 0.01);          		//并网总的有功功率显示 zuoyuntest
				_BA_ESTAIAC = (int16)(PRO.STA_iac * 10);				//BJTULVRT201204
				_BA_ESTAIBA = (int16)(PRO.STA_icb * 10);				//BJTULVRT201204
				_BA_ETOQFB  = (int16)Te_feedback;				        	//转矩反馈//BJTULVRT201204
				_BA_EPSTA   = (int16)(PRO.Psactive * 0.01);          		//定子侧有功功率显示kW,DOT1 *10/1000=100
				_BA_EPNPR   = (int16)(PRO.Pnactive * 0.01);         		//网侧有功功率显示BJTULVRT201204
				_BA_EPGRID  = (int16)(PRO.Pgactive * 0.01);          		//并网总的有功功率显示 zuoyuntest

				_BA_TIME1_0=_BA_TIME2_0;									//故障时刻1
				_BA_TIME1_1=_BA_TIME2_1;
				_BA_TIME1_2=_BA_TIME2_2;
				_BA_TIME1_3=_BA_TIME2_3;
				_BA_TIME1_4=_BA_TIME2_4;
				_BA_TIME1_5=_BA_TIME2_5;
				
				_BA_TIME2_0=_BA_TIME3_0;									//故障时刻2
				_BA_TIME2_1=_BA_TIME3_1;
				_BA_TIME2_2=_BA_TIME3_2;
				_BA_TIME2_3=_BA_TIME3_3;
				_BA_TIME2_4=_BA_TIME3_4;
				_BA_TIME2_5=_BA_TIME3_5;
					
				_BA_TIME3_0=_BA_TIME4_0;									//故障时刻3
				_BA_TIME3_1=_BA_TIME4_1;
				_BA_TIME3_2=_BA_TIME4_2;
				_BA_TIME3_3=_BA_TIME4_3;
				_BA_TIME3_4=_BA_TIME4_4;
				_BA_TIME3_5=_BA_TIME4_5;
					
				_BA_TIME4_0=RTIMER.time[0];									//故障时刻4
				_BA_TIME4_1=RTIMER.time[1];                         		//最新故障时间
				_BA_TIME4_2=RTIMER.time[2];
				_BA_TIME4_3=RTIMER.time[3];
				_BA_TIME4_4=RTIMER.time[4];
				_BA_TIME4_5=RTIMER.time[5];
			}
		}

//NO2------原有故障现在也有，且级别相同或者低于之前级别的故障------（故障平级非零、降级非零）----
		else if((_MSG_SCOUT2!=MSG_NONE)&&(M_ChkFlag(SL_ERROR)!=0)&&((TAB_MSG[_MSG_SCOUT2].rank <= TAB_MSG[_MSG_SCOUT1].rank)))					
		{
			MAIN_LOOP.cnt_rcvr=0;
			_MSG_SCOUT1 = _MSG_SCOUT2;									//本次故障信息转存

			if((TAB_MSG[_MSG_SCOUT2].attr & D_RCVR)==D_RCVR)	M_SetFlag(SL_DRCVR);	//高级故障无D_RCVR而低级故障有D_RCVR且前者首先发生时需要
			else												M_ClrFlag(SL_DRCVR);
			
			if((TAB_MSG[_MSG_SCOUT2].attr & OT_SER)==OT_SER)	M_SetFlag(SL_OTSER);	//超时严重属性,高级故障无D_RCVR而低级故障有D_RCVR且前者首先发生时需要
			else												M_ClrFlag(SL_OTSER);

			if(M_ChkFlag(SL_OTSER)!=0)
			{
				if(M_ChkCounter(MAIN_LOOP.cnt_otser,DELAY_OTSER)>=0)	//延时时间到
				{
					M_SetFlag(SL_NO_RESET);								//置 禁止复位 标志	
					M_ClrFlag(SL_OTSER);								//清超时严重标志
				}	
			}
			else	MAIN_LOOP.cnt_otser=0;		
			
			if(M_ChkFlag(SL_DRCVR)!=0)									//延时复位故障
			{
				M_SetFlag(SL_DL_RESET);									//首先置 禁止复位 标志
			}											
		
		}

//NO3---------原来有故障，本次没有故障----------------------------------------（故障降级为零）------			
		else if((_MSG_SCOUT2==MSG_NONE)&&(M_ChkFlag(SL_ERROR)!=0))					
		{				
			MAIN_LOOP.cnt_otser=0;  								//清超时严重延时计
			if(M_ChkFlag(SL_IRCVR)!=0)								//允许立即恢复故障
			{
				M_ClrFlag(SL_IRCVR);								//清立即恢复标志
				M_ClrFlag(SL_ERROR);								//删除故障信号
				M_ClrFlag(SL_ERRSTOP);
				M_ClrFlag(SL_SERIESTOP);							//20110829
//-------20130314--------
					M_ClrFlag(SL_OCS_NPRSTART);	          	//清主控命令	 
					M_ClrFlag(SL_OCS_MPRSTART);				//清主控命令								
					M_ClrFlag(SL_QEPPCO);

					M_ClrFlag(SL_EE_FAIL);					//清除EEROM故障标志

					M_ClrFlag(SL_PDPINTA);					//清除网侧TZ1_PDP故障标志
					M_ClrFlag(SL_PDPINTB);					//清除机侧TZ2_PDP故障标志
					M_ClrFlag(SL_DISPLAY3);             	//灭PDP故障指示灯
					ClrPdpint();							//PDPINT中断清空	
					EnPdpint();								//PDPINT中断使能

					MAIN_LOOP.cnt_rcvr=0;                   //故障延时恢复计时
					MAIN_LOOP.cnt_otser=0;                  //超时严重延时	
//----------------------
			}
			
//			if(M_ChkFlag(SL_DL_RESET)!=0)							//允许延时恢复且本次没有故障
			if((M_ChkFlag(SL_DL_RESET)!=0)&&(M_ChkFlag(SL_NO_RESET)==0))		//允许延时恢复且本次没有故障20121023
			{
				PRO.rcvr = _SC_RTRT * 1000;							//单位变换:s->ms
				if(M_ChkCounter(MAIN_LOOP.cnt_rcvr,PRO.rcvr)>=0)	//延时时间到
				{
					M_ClrFlag(SL_DRCVR);							//清延时恢复标志
					M_ClrFlag(SL_DL_RESET);							//清 禁止复位 标?
					M_ClrFlag(SL_ERROR);							//删除故障信号20120310
					M_ClrFlag(SL_ERRSTOP);							//20120310
					M_ClrFlag(SL_SERIESTOP);						//20120310
//-------20121206--------
					M_ClrFlag(SL_OCS_NPRSTART);	          	//清主控命令	 
					M_ClrFlag(SL_OCS_MPRSTART);				//清主控命令								
					M_ClrFlag(SL_QEPPCO);

					M_ClrFlag(SL_EE_FAIL);					//清除EEROM故障标志

					M_ClrFlag(SL_PDPINTA);					//清除网侧TZ1_PDP故障标志
					M_ClrFlag(SL_PDPINTB);					//清除机侧TZ2_PDP故障标志
					M_ClrFlag(SL_DISPLAY3);             	//灭PDP故障指示灯
					ClrPdpint();							//PDPINT中断清空	
					EnPdpint();								//PDPINT中断使能

					MAIN_LOOP.cnt_rcvr=0;                   //故障延时恢复计时
					MAIN_LOOP.cnt_otser=0;                  //超时严重延时	
//----------------------
				}
			}
			else   MAIN_LOOP.cnt_rcvr=0;						
		}
//NO4---------------------------------原来没有故障本次也没有故障--------------（故障平级为零）------
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
			M_ClrFlag(SL_DISPLAY5);                         //清系统故障指示
		}
	}

//-------------------------故障停机后复位处理-------------------------------------------------------
	if((M_ChkFlag(SL_ERRSTOP)!=0 || M_ChkFlag(SL_SERIESTOP)!=0) &&(M_ChkFlag(SL_OCS_RESET)!=0)&&(M_ChkFlag(SL_NO_RESET)==0)&&(M_ChkFlag(SL_DL_RESET)==0)&&(_MSG_SCOUT2==0))   //没有故障才芨次?
	{	
			M_ClrFlag(SL_SERIESTOP);				//清除严重停机故障标志
			M_ClrFlag(SL_ERRSTOP);					//清除停机故障标志
			M_ClrFlag(SL_ERROR);					//清除故障标志
			
			M_ClrFlag(SL_OCS_NPRSTART);	          	//清主控命令	 
			M_ClrFlag(SL_OCS_MPRSTART);				//清主控命令								
			M_ClrFlag(SL_QEPPCO);

			M_ClrFlag(SL_EE_FAIL);					//清除EEROM故障标志

			M_ClrFlag(SL_PDPINTA);					//清除网侧TZ1_PDP故障标志
			M_ClrFlag(SL_PDPINTB);					//清除机侧TZ2_PDP故障标志
			M_ClrFlag(SL_DISPLAY3);             	//灭PDP故障指示灯
			ClrPdpint();							//PDPINT中断清空	
			EnPdpint();								//PDPINT中断使能
	
			_MSG_SCOUT1=0;							//清故障信息位
			MAIN_LOOP.cnt_rcvr=0;                   //故障延时恢复计时
			MAIN_LOOP.cnt_otser=0;                  //超时严重延时	
//			_SY_RTRN=0;								//故障剖髑辶?

//			M_ClrFlag(SL_QEPPHE);
//			M_ClrFlag(SL_QEPPCDE);
//			M_ClrFlag(SL_CANOPENOVER);               //CAN通讯
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
** 函?: et_relay
** 功能描述: 过载保护计算-NPR
** 输　入: 	 
** 输:   
** ?  释: 	 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void et_relay_N(void)
{
	int32 et1,et2,ev1,ev2,ev,et;
//网侧	
	ev = (int32)PRO.NPR_iac;
	et = ((ev-_SC_IACOV1) * 512) /_SC_IACOV1;

//200us
	if(abs(et)>255)
	{
		if(et>=0)
		{
			et1=et-256;
			if(et1>255) et1=255;   //超出200%的话就算作200%
		
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

	if(ET_SUM1<=0)					//小于额定值，清零
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
//			M_SetFlag(SL_ERRDATASAVE);	//触发外部RAM数据转?20091109atzy
		}
		else M_ClrFlag(SL_SIAC1);
	}
}  
/*********************************************************************************************************
** 函数名称: et_relay
** 功苊述: 过载保护计算--MPR
** 输　入: 	 
** 输:   
** 注  释: 	 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日期:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void et_relay_M(void)
{
	int32 et1,et2,ev1,ev2,ev,et;
//机侧	
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
//			M_SetFlag(SL_ERRDATASAVE);	//触发外部RAM数据转存20091109atzy
		}
		else M_ClrFlag(SL_SIAC2);
	}
}  

/*********************************************************************************************************
** 函数名: CntCtrl
** 功能描述: 计数控制
** 输　入:
** 输　出       
** 注  释: systest
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修人:
** 日　期:
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
					
	if(MAIN_LOOP.cnt_nprlamp!=65535)	MAIN_LOOP.cnt_nprlamp++;  //指示网侧变流器工作灯
	if(MAIN_LOOP.cnt_mprlamp!=65535)	MAIN_LOOP.cnt_mprlamp++;  //指示机侧变流器工作灯
				
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

	if(MAIN_LOOP.cnt_isteady0!=65535)	MAIN_LOOP.cnt_isteady0++;     //RunCtrl网侧停机给定延时
	if(MAIN_LOOP.cnt_mprsteady0!=65535)	MAIN_LOOP.cnt_mprsteady0++;   //RunCtrl机侧停机给定延时

//	if(MAIN_LOOP.cnt_senszfstdy!=60001)	MAIN_LOOP.cnt_senszfstdy++;		//传感器零漂滤波计数器
	if(MAIN_LOOP.cnt_speedout!=65535)	MAIN_LOOP.cnt_speedout++;		//20090817
	if(MAIN_LOOP.cnt_uaclv1!=65535)		MAIN_LOOP.cnt_uaclv1++;		//200901027atzy
	if(MAIN_LOOP.cnt_encofault!=65535)	MAIN_LOOP.cnt_encofault++;		//200905atcpc

//sysctl_zl_start
	if(MAIN_LOOP.cnt_cbon!=65535)			MAIN_LOOP.cnt_cbon++;    		//主断闭合子程序	
	if(MAIN_LOOP.cnt_errstop!=65535)	    MAIN_LOOP.cnt_errstop++;    	//一般故障停机
	if(MAIN_LOOP.cnt_serstop!=65535)	    MAIN_LOOP.cnt_serstop++;        //严重故障停机     
	if(MAIN_LOOP.cnt_norstop!=65535)		MAIN_LOOP.cnt_norstop++;        //正常停机
	if(MAIN_LOOP.cnt_synrun!=65535)      	MAIN_LOOP.cnt_synrun++;         //并网
	if(MAIN_LOOP.cnt_epwmstop!=65535)      	MAIN_LOOP.cnt_epwmstop++;        //一般故障封脉冲后延时断主断20110829
	if(MAIN_LOOP.cnt_sysoff!=65535)			MAIN_LOOP.cnt_sysoff++;			//正常停机延迟，20120321
//sysctl_zl_end	

//ADctl_zl_start
    if(MAIN_LOOP.cnt_AMUX!=65535)      	    MAIN_LOOP.cnt_AMUX++;           //慢速AD采样延时及时
//ADctl_zl_end
    if(MAIN_LOOP.cnt_reset!=65535)      	MAIN_LOOP.cnt_reset++;           //上位机I/O复位，延时2s，防止误僮?
    if(MAIN_LOOP.cnt_clostacmd!=65535)      MAIN_LOOP.cnt_clostacmd++;       //衔薯合定子接触器，延时1s，防止误操
    if(MAIN_LOOP.cnt_nprcmd!=65535)       	MAIN_LOOP.cnt_nprcmd++;         //上位机允许NPR发脉冲邮?1s，防止误操作
    if(MAIN_LOOP.cnt_mprcmd!=65535)         MAIN_LOOP.cnt_mprcmd++;         //上位机允许MPR发脉冲，延时1s，防刮蟛僮?
	if(MAIN_LOOP.cnt_qcapspdin!=65535)		MAIN_LOOP.cnt_qcapspdin++;   
	if(MAIN_LOOP.cnt_qcapdisturb!=65535)	MAIN_LOOP.cnt_qcapdisturb++;		//QEP抗扇?   
	if(MAIN_LOOP.cnt_qepcntok!=65535)		MAIN_LOOP.cnt_qepcntok++;   		//QEP抗干扰   
	if(MAIN_LOOP.cnt_qepzdisturb!=65535)	MAIN_LOOP.cnt_qepzdisturb++;   		//QEP抗干扰   
    if(MAIN_LOOP.cnt_ein!=65535)       		MAIN_LOOP.cnt_ein++;        	 //上换合主断，延时1s，防止误操作
    if(MAIN_LOOP.cnt_sysrun!=65535)       	MAIN_LOOP.cnt_sysrun++;        	 //上位机诵?/电机试验，延时1s，防止误操作
    if(MAIN_LOOP.cnt_prec!=65535)       	MAIN_LOOP.cnt_prec++;         	 //上位机预充电，延时1s，防止误操作
	   
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
    if(MAIN_LOOP.cnt_hv_rcv!=65535)    		MAIN_LOOP.cnt_hv_rcv++; 
	if(MAIN_LOOP.cnt_lv_rcv!=65535)    		MAIN_LOOP.cnt_lv_rcv++;           
    if(MAIN_LOOP.cnt_lv_time!=65535)    	MAIN_LOOP.cnt_lv_time++;       
    if(MAIN_LOOP.cnt_hvlv_detect!=65535)    	MAIN_LOOP.cnt_hvlv_detect++;       
    if(MAIN_LOOP.cnt_scrondelay!=65535)    	MAIN_LOOP.cnt_scrondelay++;       
    if(MAIN_LOOP.cnt_lv_fault1!=65535)    	MAIN_LOOP.cnt_lv_fault1++;       
    if(MAIN_LOOP.cnt_lv_fault2!=65535)    	MAIN_LOOP.cnt_lv_fault2++;       

    if(MAIN_LOOP.cnt_resetrn!=100000000)    MAIN_LOOP.cnt_resetrn++;   
	if(MAIN_LOOP.cnt_phorde!=65535)		    MAIN_LOOP.cnt_phorde++;				//201005atcpc

	if(MAIN_LOOP.cnt_freqrecv!=65535)		MAIN_LOOP.cnt_freqrecv++;			//201011PLL
//	if(MAIN_LOOP.cnt_lv_gving!=65535)		MAIN_LOOP.cnt_lv_gving++;			//201011PLL
	if(MAIN_LOOP.cnt_unbalance!=65535)	MAIN_LOOP.cnt_unbalance++;		//201011LVRT

//---------20121103atsp---------------
//---------BJTULVRT201204
	if(MAIN_LOOP.cnt_qworking!=65535)	MAIN_LOOP.cnt_qworking++;		
	if(MAIN_LOOP.cnt_lv_phicontrol!=65535)	MAIN_LOOP.cnt_lv_phicontrol++;		
	if(MAIN_LOOP.cnt_lv_lowpi!=65535)		MAIN_LOOP.cnt_lv_lowpi++;	
	if(MAIN_LOOP.cnt_lv_mpridvar!=65535)	MAIN_LOOP.cnt_lv_mpridvar++;	
	if(MAIN_LOOP.cnt_lv_utlv!=65535)		MAIN_LOOP.cnt_lv_utlv++;	
//--------20130225--------
	if(MAIN_LOOP.cnt_lv_irqdelay!=65535)	MAIN_LOOP.cnt_lv_irqdelay++;	//20121210
	if(MAIN_LOOP.cnt_lv_start_scron!=65535)		MAIN_LOOP.cnt_lv_start_scron++;	//20121210
	if(MAIN_LOOP.cnt_lv_start_scroff!=65535)		MAIN_LOOP.cnt_lv_start_scroff++;	//20121210
//	if(MAIN_LOOP.cnt_lv_scroff1!=65535)		MAIN_LOOP.cnt_lv_scroff1++;	//20121210
	if(MAIN_LOOP.cnt_lv_scroff2!=65535)		MAIN_LOOP.cnt_lv_scroff2++;	//20121210
	if(MAIN_LOOP.cnt_lv_scroff3!=65535)		MAIN_LOOP.cnt_lv_scroff3++;	//20130221
	if(MAIN_LOOP.cnt_lv_unblcscroff1!=65535)		MAIN_LOOP.cnt_lv_unblcscroff1++;	//20121210
	if(MAIN_LOOP.cnt_lv_unblcscroff2!=65535)		MAIN_LOOP.cnt_lv_unblcscroff2++;	//20121210

	if(MAIN_LOOP.cnt_lv_unblcscroff3!=65535)	MAIN_LOOP.cnt_lv_unblcscroff3++;	//20130124
	if(MAIN_LOOP.cnt_lv_npriqrf!=65535)			MAIN_LOOP.cnt_lv_npriqrf++;		//20130124
	if(MAIN_LOOP.cnt_lv_mpridrf!=65535)			MAIN_LOOP.cnt_lv_mpridrf++;		//20130130
	if(MAIN_LOOP.cnt_hvlv_kq!=65535)			MAIN_LOOP.cnt_hvlv_kq++;			//20130124
	if(MAIN_LOOP.cnt_gridfault_last!=65535)		MAIN_LOOP.cnt_gridfault_last++;	//20130124
	if(MAIN_LOOP.cnt_lv_dlrecmstop!=65535)		MAIN_LOOP.cnt_lv_dlrecmstop++;	//20130130
//	if(MAIN_LOOP.cnt_lv_xnzkkz!=65535)			MAIN_LOOP.cnt_lv_xnzkkz++;		//20130202
	if(MAIN_LOOP.cnt_jgvdscron!=65535)			MAIN_LOOP.cnt_jgvdscron++;		//20130205
	if(MAIN_LOOP.cnt_lv_reciq!=65535)			MAIN_LOOP.cnt_lv_reciq++;		//20130205
	if(MAIN_LOOP.cnt_lv_holdpow!=65535)			MAIN_LOOP.cnt_lv_holdpow++;		//20130205
	if(MAIN_LOOP.cnt_lv_scrkeepon!=65535)		MAIN_LOOP.cnt_lv_scrkeepon++;		//20130221
	
	if(MAIN_LOOP.cnt_mc_ack!=65535)		    MAIN_LOOP.cnt_mc_ack++;				//MagnetCurve2013-12-13
		
//--------20130801--------
	if(M_ChkFlag(SL_ECPOSSEARCH)!=0 && M_ChkFlag(SL_MPR_START)!=0)
	{
		if(MAIN_LOOP.cnt_synfail!=200000)	MAIN_LOOP.cnt_synfail++;   	//20130525
	}
	if(MAIN_LOOP.cnt_ecposcheck!=65535)		MAIN_LOOP.cnt_ecposcheck++;			//20130525
	if(MAIN_LOOP.cnt_ecpostrace!=65535)		MAIN_LOOP.cnt_ecpostrace++;			//20130525

	if(MAIN_LOOP.cnt_chopper!=65535)		MAIN_LOOP.cnt_chopper++;		//201101PLL

}

/*********************************************************************************************************
** 函数名称: Display
** 能描述: 显示值扑?
** 输　入: 	 
** 输出:   
** 注  释: 	 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Display(void)
{
	float temp1,temp2,temp3;

//------------------------中间直流电压及指令值显示值计算-------------------------------------------
	DISP.udc  = PRO.udc;							//单位V
	DISP.urf  = RUN.urf;							//单位V												//单位V
	DISP.mpridrf = RUN.mpridrf;							//单位A
	DISP.npriqrf = RUN.npriqrf;							//单位A
	DISP.mpriqrf = RUN.mpriqrf;							//单位A
	DISP.toqrf   = RUN.toqrf;							//单位NM
	DISP.aglrf   = RUN.aglrf;							//单位 角度
    DISP.toqfb   = Te_feedback;    
//---------------------------线电压有效值显示值计算------------------------------------------------
	DISP.npr_uab = PRO.NPR_uab;						//单位V
	DISP.npr_ubc = PRO.NPR_ubc;						//单位V
	DISP.sta_uab = PRO.STA_uab;						//单位V
	DISP.sta_ubc = PRO.STA_ubc;						//单位V

//---------------------------主断前网侧线电压有效值计算-显示---------------------------
//	DISP.grd_uab = MEAN_DATA.uab * 1.110721;        //单位V  rms=mean*PAI/(2*sqrt(2)) ； 1.110721=PAI * SQRT2 / 4
//	DISP.grd_ubc = MEAN_DATA.ubc * 1.110721;        //单位V  rms=mean*PAI/(2*sqrt(2)) ； 1.110721=PAI * SQRT2 / 4
	DISP.grd_uab = MEAN_DATA.uab1 * 1.110721;        //单位V  rms=mean*PAI/(2*sqrt(2)) ； 1.110721=PAI * SQRT2 / 4
	DISP.grd_ubc = MEAN_DATA.ubc1 * 1.110721;        //单位V  rms=mean*PAI/(2*sqrt(2)) ； 1.110721=PAI * SQRT2 / 4

//---------------------------网侧和机侧电感温度显示----------------------------
    DISP.Lac_temp      = MEAN_DATA.Lac_temp;                  //网侧电感温度
	DISP.Ldudt_temp    = MEAN_DATA.Ldudt_temp;                //机侧电感温度 
	DISP.NPR_skiptemp  = AMUX.NPR_skiiptemp;              //网侧SKIIP温度
	DISP.MPR_skiptemp  = AMUX.MPR_skiiptemp;              //机侧SKIIP温


//---------------------------瞬时电流显示值计算----------------------------------------------------
	DISP.npr_iai = AD_OUT_NPR_I.a;					//单位A
	DISP.npr_ibi = AD_OUT_NPR_I.b;					//单位A
	DISP.npr_ici = AD_OUT_NPR_I.c;					//单位A
	DISP.mpr_iai = AD_OUT_MPR_I.a;					//单位A
	DISP.mpr_ibi = AD_OUT_MPR_I.b;					//单位A
	DISP.mpr_ici = AD_OUT_MPR_I.c;					//单位A

//------------------------网侧并网电流有效值显示值计?----------------------------------------------
	DISP.npr_iar = PRO.npr_iar;							//单位A
	DISP.npr_ibr = PRO.npr_ibr;							//单位A
	DISP.npr_icr = PRO.npr_icr;							//单位A

//------------------------机侧电流有效值显示值计算--------------------------------------------------
    temp1 = MEAN_DATA.ia2 * 1.110721;
	temp2 = MEAN_DATA.ib2 * 1.110721;
	temp3 = MEAN_DATA.ic2 * 1.110721;
	DISP.mpr_iar = temp1;							//单位A
	DISP.mpr_ibr = temp2;							//单位A
	DISP.mpr_icr = temp3;							//单位A

//------------------------定子线电流有效值显示值计算--------------------------------------------------
    temp1 = MEAN_DATA.iac3 * 1.110721;
	temp2 = MEAN_DATA.icb3 * 1.110721;
	DISP.sta_iac = temp1;							//单位A
	DISP.sta_iba = temp2;							//单位A

//------------------------------定子侧和网侧和总并网有功功率显示-----------------------------------------------------
	DISP.Psactive   = PRO.Psactive;
	DISP.Psreactive = PRO.Psreactive;
	DISP.Ps		    = PRO.Ps;

	DISP.Pnactive   = PRO.Pnactive;
	DISP.Pnreactive = PRO.Pnreactive;
    DISP.Pn         = PRO.Pn;

	DISP.Pgactive   = PRO.Pgactive;
	DISP.Pgreactive = PRO.Pgreactive;
	DISP.Pg      	= PRO.Pg;

//------------------------------频率和转速显示-----------------------------------------------------	
	if(M_ChkFlag(SL_IN1_CBSTS)==0) 	DISP.freq=0;            //20090815
	else 							DISP.freq = CAP4.freqtmp;

	DISP.speed= PRO.speedflt;		//20090815

//------------------------------定子同步并网前定子前后电压差值显示-----------------------------------------------------
	DISP.uab23 = MEAN_DATA.uab_d;
	DISP.ubc23 = MEAN_DATA.ubc_d;

//------------------------------PI环输出显示-------------------------------------------------------
	DISP.pionu  = PI_NPR_U.out;						//单位A
	DISP.pionid = PI_NPR_Id.out;					//单位V
	DISP.pioniq	= PI_NPR_Iq.out;					//单位V
	DISP.mexi  = RUN_mpridrf_exi;					//单位A  机侧励磁理论计算值显示
	DISP.piomid = PI_MPR_Id.out;				    //单位V
	DISP.piomiq	= PI_MPR_Iq.out;					//单位V
} 
/*********************************************************************************************************
** 函数名称: Bank()
** 功能描述: 显示控制
** 输　入: 	 
** 输出:   
** 注  释: 	 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改人:
** 日　期:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Bank(void)
{
	
//------------------------------------网侧工作指示灯显示--------------------------------------------
	if(M_ChkFlag(SL_NPR_PWMOUT)!=0)
	{
		M_SetFlag(SL_DISPLAY0);
/*		if(M_ChkCounter(MAIN_LOOP.cnt_nprlamp,DELAY_NPRRUN)>=0)	//0灯快速闪?,指示网侧正在发脉冲运行
		{
			M_NotFlag(SL_DISPLAY0);
			MAIN_LOOP.cnt_nprlamp=0;
		}
*/	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_nprlamp,DELAY_NPRSTDBY)>=0)	//0灯慢速了?
		{
		   M_NotFlag(SL_DISPLAY0);
		   MAIN_LOOP.cnt_nprlamp=0;
		} 
	}
			
//------------------------------------机侧工作甘镜葡允?--------------------------------------------
	if(M_ChkFlag(SL_MPR_PWMOUT)!=0)
	{
		M_SetFlag(SL_DISPLAY1);
/*		if(M_ChkCounter(MAIN_LOOP.cnt_mprlamp,DELAY_MPRRUN)>=0)	//1灯快速闪?,指示机侧正在发脉冲运行
		{
			M_NotFlag(SL_DISPLAY1);
			MAIN_LOOP.cnt_mprlamp=0;
		}
*/	}
	else
	{
		if(M_ChkCounter(MAIN_LOOP.cnt_mprlamp,DELAY_MPRSTDBY)>=0)	//1灯慢速闪烁
		{
			M_NotFlag(SL_DISPLAY1);
			MAIN_LOOP.cnt_mprlamp=0;
		}
	}


	if(M_ChkFlag(SL_STEADYFB)!=0)	M_SetFlag(SL_DISPLAY2);			//Vdc稳定指示
	else							M_ClrFlag(SL_DISPLAY2);

	if(M_ChkFlag(SL_UDSTACOK)!=0 && M_ChkFlag(SL_MPR_SYNOK)!=0)	M_SetFlag(SL_DISPLAY4);			//定子达到并网条件指示
	else if(M_ChkFlag(SL_UDSTACOK)!=0)							M_NotFlag(SL_DISPLAY4);
	else														M_ClrFlag(SL_DISPLAY4);

	if(M_ChkFlag(SL_CBCLOSED)!=0 && M_ChkFlag(SL_OCS_SYSRUN)==0 && M_ChkFlag(SL_SERIESTOPING)==0 && M_ChkFlag(SL_SERIESTOP)==0 && M_ChkFlag(SL_ERRSTOPING)==0 && M_ChkFlag(SL_ERRSTOP)==0 && M_ChkFlag(SL_OCS_EIN)!=0)	
			M_SetFlag(SL_DISPLAY7);									//主断已蘸?,预充电待机指示
	else	M_ClrFlag(SL_DISPLAY7);

//--------20130801--------
	if((M_ChkFlag(SL_POWERON)==0) && (M_ChkFlag(SL_MPR_START)==0))
		QEPDATA.encodpos =  - _encodpos;

//----------------------------------------运行监控--------------------------------------------------
	_BA_URF   = (int16)DISP.urf;				//中间电压给定值
	_BA_UDC   = (int16)DISP.udc;				//中间直鞯缪?
	_BA_MIDRF  = (int16)(DISP.mpridrf * 10);		//d轴电流指令
	_BA_NIQRF  = (int16)(DISP.npriqrf * 10);		//q轴电流指令
	_BA_MIQRF  = (int16)(DISP.mpriqrf * 10);		//q轴电流指令
	_BA_TOQRF  = (int16)(DISP.toqrf);				//转矩指令
    _BA_AGLRF  = (int16)(DISP.aglrf);             //无功角度噶? 20091027atzy

	_BA_IA1   = (int16)(DISP.npr_iar * 10);		//网侧,a相并网电流，改为1位∈?
	_BA_IB1   = (int16)(DISP.npr_ibr * 10);		//网侧,b相并网电流
	_BA_IC1   = (int16)(DISP.npr_icr * 10);		//?,c相并网电流
	_BA_IA2   = (int16)(DISP.mpr_iar * 10);		//电机侧,a嗟缌
	_BA_IB2   = (int16)(DISP.mpr_ibr * 10);		//电机侧,b相电流
	_BA_IC2   = (int16)(DISP.mpr_icr * 10);		//电机侧,c相电流

	_BA_UAB0   = (int16)DISP.grd_uab;			//主断前网侧ab线压
	_BA_UBC0   = (int16)DISP.grd_ubc;			//主断前网侧bc线压				
	_BA_UAB1  = (int16)DISP.npr_uab;			//网侧ab线压
	_BA_UBC1  = (int16)DISP.npr_ubc;			//网侧bc线压
	_BA_UAB2  = (int16)DISP.sta_uab;			//定子侧ab线压,定子侧 20091027atzy
	_BA_UBC2  = (int16)DISP.sta_ubc;			//定子侧bc线压,定子侧 20091027atzy
	_BA_GRDUD = (int16)TRS_NGS_U.dflt;          //d轴主断后网压反馈 滤波后
	_BA_GRDUQ = (int16)TRS_NGS_U.qflt;          //q轴主断后网压反馈 滤波后
	_BA_NPRID = (int16)TRS_NPR_I.dflt;          //d轴网侧电流反馈 滤波后
	_BA_NPRIQ = (int16)TRS_NPR_I.qflt;          //q轴网侧电流反馈 滤波后
//	_BA_EXCID = (int16)(TRS_MPR_I.d * 10);             //d轴机嗟缌鞣蠢? 滤波前
//	_BA_EXCIQ = (int16)(TRS_MPR_I.q * 10);             //q轴机侧电流反馈 滤波前
    _BA_SCRIA = (int16)(AD_OUT_SCR_I.a * 10);             //SCR电流a	20110906
    _BA_SCRIB = (int16)(AD_OUT_SCR_I.b * 10);             //SCR电流b	20110906

//	_BA_STAUD = (int16)TRS_MPR_U.d;		//2013-12-13
//	_BA_STAUQ = (int16)TRS_MPR_U.q;		//2013-12-13

	_BA_MC_IN = (int16)(MC_DATA.in * 100);		//MagnetCurve2013-12-13ZZJ
	_BA_MC_K  = (int16)(MC_DATA.k * 1000);		//MagnetCurve2013-12-13ZZJ

	_BA_TLAC  = (int16)DISP.Lac_temp;		    //网侧电感温度
//	_BA_TLAC  = (int16)SCI_canopen.cnt_heartbeat;	    //网侧电感温度 2014-05-06LJD临时修改为CANopen

	_BA_TLDUDT= (int16)DISP.Ldudt_temp;	        //机侧电感温度
	_BA_TNSKIIP= (int16)DISP.NPR_skiptemp;      //网侧SKIIP温度 摄氏度
	_BA_TMSKIIP= (int16)DISP.MPR_skiptemp;      //机侧SKIIP温度 摄氏度

	_BA_FREQ  = (int16)(DISP.freq * 10);		//电网频率
	_BA_SPEED = (int16)DISP.speed;				//电机转速 

	_BA_PIONU  = (int16)(DISP.pionu  * 10);			    //NPR电压环输出 6.23change_zl改为1位小?
	_BA_PIONID = (int16)(DISP.pionid * 100);			//NPR电流环d输出
	_BA_PIONIQ = (int16)(DISP.pioniq * 100);			//NPR电流环q输出
	_BA_MEXI   = (int16)(DISP.mexi  * 10);			    // 6.23change_zl改为1小数 改为励磁电流理论值显示cpc
	_BA_PIOMID = (int16)(DISP.piomid * 100);			//MPR电流环d输出
	_BA_PIOMIQ = (int16)(DISP.piomiq * 100);			//MPR电流环q输出

//	_BA_STAUABD = (int16)(DISP.uab23 * 10);             //定子同步并网前定忧昂蟮缪共? 20091027atzy//2013-12-13
	_BA_STAUABD = (int16)(MC_U_test * 10);             	//定子励磁电压差--用于校正励磁曲线MagnetCurve2013-12-13ZZJ
	_BA_STAUBCD = (int16)(DISP.ubc23 * 10);				//定子同步并网前定子前后电压差 20091027atzy
	_BA_STAIAC = (int16)(DISP.sta_iac * 10);            //定子线电流有效值显示
	_BA_STAIBA = (int16)(DISP.sta_iba * 10);			//定子线电流有效值显示

	_BA_TOQFB  = (int16)DISP.toqfb;				        //转矩反馈

	_BA_PSTA  = (int16)(DISP.Psactive * 0.01);          //定子侧有功功率显示kW,DOT1 *10/1000=100
	_BA_PNPR  = (int16)(DISP.Pnactive * 0.01);          //网侧有功功率显示
	_BA_PGRID = (int16)(DISP.Pgactive * 0.01);          //并网总的有功功率显示

	_BA_QSTA  = (int16)(DISP.Psreactive * 0.01);        //定子侧无功功率显示kVAR,DOT1 *10/1000=100
	_BA_QNPR  = (int16)(DISP.Pnreactive * 0.01);        //网侧无功功率显示
	_BA_QGRID = (int16)(DISP.Pgreactive * 0.01);        //并网总的无功功率显示

	_BA_SSTA  = (int16)(DISP.Ps * 0.01);              	//定子侧视在功率显示kVA,DOT1 *10/1000=100
	_BA_SNPR  = (int16)(DISP.Pn * 0.01);              	//网侧视在功率显示
	_BA_SGRID = (int16)(DISP.Pg * 0.01);              	//并网总的视在功率显示

//通过CANOPEN通讯反馈给主控的监控变量	

//	DISP.toqfb = 8000;  //cantest
//    DISP.speed = 1800;  //cantest
//	AMUX.skiiptempmax = 65; //cantest

	SCI_canopen.tx_watertempin  = 0;
	SCI_canopen.tx_watertempout = 0;
	SCI_canopen.tx_demand = 0;

}


/*********************************************************************************************************
** 函数名称: ERROR_Datasave
** 功  能: 将数据写入外部RAM
** 输　入:
** 输　出:        
** 注  释: 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 20100203atbjtu PCmodbus
**-------------------------------------------------------------------------------------------------------
** 修改?:
** 日　期:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void ERROR_Datasave(void)
{
//--------------------------------External RAM Data Save-----------------------------------------
	//20121103
//--------------------------------External RAM Data Save-----------------------------------------

	if(M_ChkFlag(SL_PC_ERRDATACLR)!=0)						//PC指令外部RAM数据释放
	{
		M_ClrFlag(SL_RAMERRORSAVE);						
		*(ERROR_RAMEND + 1) = 0x0000;
		M_ClrFlag(SL_TRIG_ERRDSAVE);
		M_ClrFlag(SL_PC_ERRDATACLR);						//释放数据/复位完成,自动清除复位指令
		M_ClrFlag(SL_PC_ERRDATASAVE);						//释放数据/复位完成,自动清除手动触发存储指令 201005atcpc
		M_ClrFlag(SL_DISPLAY6);								//201007BJTULVRT
		*(BANK_RAMEND + 1) = 0x0000;
		M_ClrFlag(SL_RAMBANKSAVE);							//
	} 

  	if(*(ERROR_RAMEND + 1) != 0x55AA)
  	{
		if(M_ChkFlag(SL_TRIG_ERRDSAVE)==0)  								//没有故障	
			MAIN_LOOP.cnt_datasave=0;
		else if(M_ChkCounter(MAIN_LOOP.cnt_datasave,DELAY_DATASAVE)>=0)	//故障发生后延时1.8s禁止画图201205LVRTatZB
		{
			M_SetFlag(SL_RAMERRORSAVE);
//			M_ClrFlag(SL_TRIG_ERRDSAVE);
			*(ERROR_RAMEND + 1) = 0x55AA;			
			*(ERROR_RAMEND + 2) = RTIMER.time[0];				//最新故障时间systest 201005atcpc
			*(ERROR_RAMEND + 3) = RTIMER.time[1];
			*(ERROR_RAMEND + 4) = RTIMER.time[2];
			*(ERROR_RAMEND + 5) = RTIMER.time[3];
			*(ERROR_RAMEND + 6) = RTIMER.time[4];
			*(ERROR_RAMEND + 7) = RTIMER.time[5];				
			*(ERROR_RAMEND + 8) = _MSG_SCOUT2;				

			_BA_TIME5_0 = RTIMER.time[0];						//最新故障时间systest 201005atcpc
			_BA_TIME5_1 = RTIMER.time[1];
			_BA_TIME5_2 = RTIMER.time[2];
			_BA_TIME5_3 = RTIMER.time[3];
			_BA_TIME5_4 = RTIMER.time[4];
			_BA_TIME5_5 = RTIMER.time[5];				
			_BA_ERR5	= _MSG_SCOUT2;				
		}

		if(M_ChkFlag(SL_RAMERRORSAVE)==0)  										//有故障之后延时2s后停止数据存储
		{
//----------LVRT--201205LVRTatZB
			*(ERROR_RAMSTART+ ERROR_RAMDATA_POS) = (int16)AD_OUT_UDC;										//0=中间直流电压
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 1 + ERROR_RAMDATA_POS)) = (int16)(CAP4.nprtrstheta*1000);	//1=NPR定向角度
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 2 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NPR_I.a*10);		//2=NPR-A电流
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 3 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NPR_I.b*10);		//3=NPR-B电流
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 4 + ERROR_RAMDATA_POS)) = (int16)(GRD_Ut*10);				//4=**********电网残压**********
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 5 + ERROR_RAMDATA_POS)) = (int16)(NGS_Udq*10);				//5=*****电网电压******
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 6 + ERROR_RAMDATA_POS)) = (int16)(NGS_Udq_p*10);			//6=********电网电压仅含有正序PLL*******
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 7 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_SCR_I.a);		//7=SCR电流BJTULVRT201204
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 8 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_SCR_I.b);		//8=SCR电流BJTULVRT201204
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 9 + ERROR_RAMDATA_POS)) = (int16)(GIVE.toqrf);			//9=**********主控给定转矩*示波器显示缩小了10倍*************
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 10 + ERROR_RAMDATA_POS)) = (int16)(GIVE.toqrfrun);		//10=*******主控给定转矩大滤波*示波器显示缩小了10倍*********
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 11 + ERROR_RAMDATA_POS)) = (int16)(PI_NPR_Id.reference*10);//11=网侧Id指令
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 12 + ERROR_RAMDATA_POS)) = (int16)(TRS_NPR_I.dflt*10);		//12=网侧Id反馈
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 13 + ERROR_RAMDATA_POS)) = (int16)(RUN.toqrf);			//13=*******运行给定转矩*示波器显示缩小了10倍*********
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 14 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_DC_I.b*10);		//14=中间直流斩波电流BJTULVRT201204
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 15 + ERROR_RAMDATA_POS)) = (int16)(RUN.mpridrf*10);		//15=机侧励磁电流指令
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 16 + ERROR_RAMDATA_POS)) = (int16)(TRS_MPR_I.dflt*10);		//16=机侧励磁电流反馈
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 17 + ERROR_RAMDATA_POS)) = (int16)(RUN.mpriqrf*10);		//17=机侧转矩电流指令
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 18 + ERROR_RAMDATA_POS)) = (int16)(TRS_MPR_I.qflt*10);		//18=机侧转矩电流反馈BJTULVRT201204
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 19 + ERROR_RAMDATA_POS)) = (int16)(NGS_Udq_n2p* 10);	    //19=网压不平衡度
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 20 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_MPR_I.a*10);		//20=MPR-A电流
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 21 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_MPR_I.b*10);		//21=MPR-B电流
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 22 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_MPR_I.c*10);		//22=MPR-C电流
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 23 + ERROR_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_STATE) * 10);//23=低电压状态
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 24 + ERROR_RAMDATA_POS)) = (int16)PRO.speed; 				//24=电机转速

			*(ERROR_RAMEND) = ERROR_RAMDATA_POS;															//

			ERROR_RAMDATA_POS++;
			if(ERROR_RAMDATA_POS >= RAM_BIAS)  ERROR_RAMDATA_POS=0;		
		}
  	}

//--------------------------------External RAM Data Read-----------------------------------------
} 

/*********************************************************************************************************
** 函数名称: BANK_Datasave
** 功  能: 将数据写入外部RAM
** 输　入:
** 输　出:        
** 注  释: 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 20100203atbjtu PCmodbus
**-------------------------------------------------------------------------------------------------------
** 修乃:
** 日　期:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void BANK_Datasave(void)
{
//--------------------------------External RAM Data Save-----------------------------------------
	//20121103
  	if(*(BANK_RAMEND + 1) != 0x55AA)
  	{
		if(M_ChkFlag(SL_TRIG_ERRDSAVE)==0)  								//没有故障	
			MAIN_LOOP.cnt_datasave2=0;
		else if(M_ChkCounter(MAIN_LOOP.cnt_datasave2,DELAY_DATASAVE)>=0)	//故障发生后延时1.8s禁止画图201205LVRTatZB
		{
			M_SetFlag(SL_RAMBANKSAVE);
			*(BANK_RAMEND + 1) = 0x55AA;			
		}

		if(M_ChkFlag(SL_RAMBANKSAVE)==0)  																//PC指令未要求数据锁存，处于刷新状态
		{
			if(_NPR_ID_Kd==0)
			{
			*(BANK_RAMSTART+ BANK_RAMDATA_POS) = (int16)(DIP_STA_I.qflt);							//0=网侧定向角度
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 1 + BANK_RAMDATA_POS)) = (int16)(CAP4.mprtrstheta*1000);	//1=机侧定向嵌?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 2 + BANK_RAMDATA_POS)) = (int16)(GRD_Utlv*10);		//2=转子角?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 3 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.ab* 10);		//3=电网滤波AB线电压
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 4 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.bc* 10);		//4=电网滤波BC线电压
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 5 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_U.ab* 10);		//5=定子滤波AB线电压
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 6 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_U.bc* 10);		//6=定子滤波BC线电压

//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 3 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_STATE) * 10);		//3=电网滤波AB线电压
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 4 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_QWORKING)* 10);		//4=电网滤波BC线缪?
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 5 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_SCRRUNING)* 10);		//5=定子滤波AB线电压
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 6 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_SCRIZERO)* 10);		//6=定子滤波BC线电压
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 7 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_SYNCON)* 10);					//7=中间直流电压
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 8 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_UNBALANCE)* 10);			//8=电网d轴电压分量滤波前
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 9 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_MSTOP)* 10);			//9=电网q轴电压分柯瞬ㄇ?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 10 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_NSTOP)* 10);		//10=网侧A相电流
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 11 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_I.ac);		//11=网侧B相电流
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_PHICON)* 10);		//11=网侧B相电流
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(GIVE.npriqrf_cl);		//12=网侧C相电流
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(NGS_Udq_pflt* 10);		//12=网侧C相电流
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 13 + BANK_RAMDATA_POS)) = (int16)(RUN.npriqrf* 10);		//13=机侧A相电流
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 14 + BANK_RAMDATA_POS)) = (int16)(TRS_NPR_I.qflt* 10);		//14=机侧B相电流
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 15 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NPR_I.c* 10);		//15=机侧C嗟流
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 16 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_STRICTLV) * 10);			//16=电网实时频率
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 17 + BANK_RAMDATA_POS)) = (int16)(kq * 100);					//17=电机转速
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 18 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_I.cb);	//18=网侧电压环输出
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_SCRKEEPON)* 10);		//19=定子缌鰽相BJTULVRT201204
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_SCRDONE)* 10);		//19=定子缌鰽相BJTULVRT20130221
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_SCRKEEPON) * 10);		//20=定子电流C相
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(TRS_NGS_U.d * 10);		//20=定子电流C相
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(PHI_DATA_M.PHId * 10);		//21=中间直流斩波缌鰾JTULVRT201204
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_LV_START_SCRON) * 10);		//21=20121210
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(PHI_DATA_M.PHIq* 10);			//22=机侧励磁电流指令
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_IMUNBALANCE) * 10);		//21=20130116
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(PRO.Pgactive2 * 0.01);		//21=20130116
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(PHI_DATA_M.Id);		//23=SCR支路电流 BJTULVRT201204
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(TRS_NPR_U.d * 10);		//23=SCR支路电流 BJTULVRT201204
//			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(PHI_DATA_M.Iq);		//24=SCR支路电流 BJTULVRT201204
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(TRS_NPR_U.q * 10);		//24=SCR支路电流 BJTULVRT201204
			}
			else if(_NPR_ID_Kd==1000)
			{
			*(BANK_RAMSTART+ BANK_RAMDATA_POS) = (int16)(NGS_Udq_p * 10);							        //0=网压正序分量
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 1 +  BANK_RAMDATA_POS)) = (int16)(CAP4.mprtrstheta*1000);	//1=机侧定向角度
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 2 +  BANK_RAMDATA_POS)) = (int16)(NGS_Udq_n2pex * 10);		//2=网压负序分量与跌落前电压正序分量之比
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 3 +  BANK_RAMDATA_POS)) = (int16)(kq * 100);				//3=无功发生系数
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 4 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_HV_STATE)* 10);	//4=高电压穿越状态
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 5 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_UNBALANCE)* 10);	//5=电网不平衡状态
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 6 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_MSTOP)* 10);		//6=机侧封脉冲
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 7 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_NSTOP)* 10);		//7=网侧封脉冲
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 8 +  BANK_RAMDATA_POS)) = (int16)(DIP_STA_I.qflt * 10);			//8=定子侧q轴无功电流滤波后
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 9 +  BANK_RAMDATA_POS)) = (int16)(PI_NPR_U.reference);			//9=直流电压参考值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 10 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_U.feedback);			//10=直流电压反馈值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 11 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.reference);		 	//11=网侧无功电流参考值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.feedback);			//12=网侧无功电流反馈值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 13 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.reference);			//13=网侧有功电流参考值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 14 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.feedback);			//14=网侧无功电流反馈值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 15 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.out);			//15=网侧id环PI输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 16 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.out);			//16=网侧iq环PI输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 17 + BANK_RAMDATA_POS)) = (int16)(PHAI_d);					//17=定子磁链d轴分量
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 18 + BANK_RAMDATA_POS)) = (int16)(PHAI_q);				    //18=定子磁链q轴分量
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(PI_MPR_Id.out *10);			//19=机侧id电流环输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(PI_MPR_Iq.out *10);			//20=机侧iq电流环输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(TRS_MPR_U.d * 10);		//21=机侧d轴电压输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(TRS_MPR_U.q * 10);		//22=机侧q轴电压输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(TRS_NPR_U.d * 10);		//23=网侧d轴输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(TRS_NPR_U.q * 10);		//24=网侧q轴输出
			*(BANK_RAMEND) = BANK_RAMDATA_POS;																
			}
			else if(_NPR_ID_Kd==2000)
			{
			*(BANK_RAMSTART+ BANK_RAMDATA_POS) = (int16)(NGS_Udq_p * 10);							        //0=网压正序分量
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 1 +  BANK_RAMDATA_POS)) = (int16)(CAP4.mprtrstheta*1000);	//1=机侧定向角度
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 2 +  BANK_RAMDATA_POS)) = (int16)(NGS_Udq_n2pex * 10);		//2=网压负序分量与跌落前电压正序分量之比
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 3 +  BANK_RAMDATA_POS)) = (int16)(kq * 100);				//3=无功发生系数
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 4 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_HV_STATE)* 10);	//4=高电压穿越状态
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 5 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_UNBALANCE)* 10);	//5=电网不平衡状态
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 6 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_MSTOP)* 10);		//6=机侧封脉冲
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 7 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_NSTOP)* 10);		//7=网侧封脉冲
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 8 +  BANK_RAMDATA_POS)) = (int16)(DIP_STA_I.qflt * 10);			//8=定子侧q轴无功电流滤波后
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 9 +  BANK_RAMDATA_POS)) = (int16)(PI_NPR_U.reference);			//9=直流电压参考值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 10 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_U.feedback);			//10=直流电压反馈值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 11 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.reference);		 	//11=网侧无功电流参考值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.feedback);			//12=网侧无功电流反馈值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 13 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.reference);			//13=网侧有功电流参考值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 14 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.feedback);			//14=网侧无功电流反馈值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 15 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.out);			//15=网侧id环PI输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 16 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.out);			//16=网侧iq环PI输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 17 + BANK_RAMDATA_POS)) = (int16)(PHAI_d);					//17=定子磁链d轴分量
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 18 + BANK_RAMDATA_POS)) = (int16)(PHAI_q);				    //18=定子磁链q轴分量
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(PHAI_d*10);			//19=机侧id电流环输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(PHAI_q*10);			//20=机侧iq电流环输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(urdc * 10);		//21=机侧d轴电压输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(urqc * 10);		//22=机侧q轴电压输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(urdc_dynamic * 10);		//23=网侧d轴输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(urqc_dynamic * 10);		//24=网侧q轴输出
			*(BANK_RAMEND) = BANK_RAMDATA_POS;		
			}
			else if(_NPR_ID_Kd==3000)
			{
			*(BANK_RAMSTART+ BANK_RAMDATA_POS) = (int16)(NGS_Udq_p * 10);							        //0=网压正序分量
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 1 +  BANK_RAMDATA_POS)) = (int16)(CAP4.mprtrstheta*1000);	//1=机侧定向角度
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 2 +  BANK_RAMDATA_POS)) = (int16)(NGS_Udq_n2pex * 10);		//2=网压负序分量与跌落前电压正序分量之比
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 3 +  BANK_RAMDATA_POS)) = (int16)(kq * 100);				//3=无功发生系数
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 4 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_HV_STATE)* 10);	//4=高电压穿越状态
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 5 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_UNBALANCE)* 10);	//5=电网不平衡状态
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 6 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_MSTOP)* 10);		//6=机侧封脉冲
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 7 +  BANK_RAMDATA_POS)) = (int16)(M_ChkFlag(SL_NSTOP)* 10);		//7=网侧封脉冲
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 8 +  BANK_RAMDATA_POS)) = (int16)(DIP_STA_I.qflt * 10);			//8=定子侧q轴无功电流滤波后
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 9 +  BANK_RAMDATA_POS)) = (int16)(PI_NPR_U.reference);			//9=直流电压参考值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 10 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_U.feedback);			//10=直流电压反馈值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 11 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.reference);		 	//11=网侧无功电流参考值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.feedback);			//12=网侧无功电流反馈值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 13 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.reference);			//13=网侧有功电流参考值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 14 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.feedback);			//14=网侧无功电流反馈值
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 15 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Id.out);			//15=网侧id环PI输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 16 + BANK_RAMDATA_POS)) = (int16)(PI_NPR_Iq.out);			//16=网侧iq环PI输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 17 + BANK_RAMDATA_POS)) = (int16)(PHAI_d);					//17=定子磁链d轴分量
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 18 + BANK_RAMDATA_POS)) = (int16)(PHAI_q);				    //18=定子磁链q轴分量
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(TRS_STA_U.dflt*10);			//19=机侧id电流环输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(TRS_STA_U.qflt*10);			//20=机侧iq电流环输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(TRS_STA_I.dflt*10);		//21=机侧d轴电压输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(TRS_STA_I.qflt*10);		//22=机侧q轴电压输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(TRS_MPR_I.dflt*10);		//23=网侧d轴输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(TRS_MPR_I.qflt*10);		//24=网侧q轴输出
			*(BANK_RAMEND) = BANK_RAMDATA_POS;		
			}
			BANK_RAMDATA_POS++;
			if(BANK_RAMDATA_POS >= RAM_BIAS)  BANK_RAMDATA_POS=0;	
			
		}
	}


//--------------------------------External RAM Data Read-----------------------------------------
}  

/*********************************************************************************************************
** 函数名称: BANK_Datasave
** 功  能: 将数据写入外部RAM
** 输　入:
** 输　出:        
** 注  释: 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 20100203atbjtu 
**-------------------------------------------------------------------------------------------------------
** 修改?:
** 日　期:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
/*void BANK_Datasave(void)	//testPCmodbus
{
//--------------------------------External RAM Data Save-----------------------------------------

		if(M_ChkFlag(SL_PC_BANKDATASAVE)==0)  																//PC指令未要求数据锁存，处于刷新状态
		{
			*(BANK_RAMSTART+ BANK_RAMDATA_POS) = (int16)(testtheta* 1000);									//0=网侧定向角度
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 1 + BANK_RAMDATA_POS)) = (int16)(testtheta_120* 1000);		//1=机侧定向角度
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 2 + BANK_RAMDATA_POS)) = (int16)(testtheta_sta* 1000);		//2=咏嵌?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 3 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.ab* 10);		//3=电网滤波AB线电压
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 4 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.bc* 10);		//4=电网滤波BC线电压
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 5 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_U.ab* 10);		//5=定子滤波AB线缪?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 6 + BANK_RAMDATA_POS)) = (int16)(AD_OUT_STA_U.bc* 10);		//6=定子滤波BC线电压
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 7 + BANK_RAMDATA_POS)) = 1100;								//7=中间直流电压
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 8 + BANK_RAMDATA_POS)) = (int16)(testtheta+400);			//8=电网d轴电压分量滤波前
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 9 + BANK_RAMDATA_POS)) = (int16)(testtheta+450);			//9=电网q轴电压分量滤波前
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 10 + BANK_RAMDATA_POS)) = (int16)(testtheta+500);			//10=网侧A相电流
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 11 + BANK_RAMDATA_POS)) = (int16)(testtheta+550);			//11=网侧B嗟缌?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 12 + BANK_RAMDATA_POS)) = (int16)(testtheta+600);			//12=网侧C相电流
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 13 + BANK_RAMDATA_POS)) = (int16)(testtheta+650);			//13=机侧A相电流
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 14 + BANK_RAMDATA_POS)) = (int16)(testtheta+700);			//14=机侧B相电流
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 15 + BANK_RAMDATA_POS)) = (int16)(testtheta+750);			//15=机侧C相电流
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 16 + BANK_RAMDATA_POS)) = (int16)(testtheta+800);			//16=电网实时频率
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 17 + BANK_RAMDATA_POS)) = (int16)(testtheta+850);			//17=电机转速
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 18 + BANK_RAMDATA_POS)) = (int16)(testtheta+900);			//18=网侧电压环输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 19 + BANK_RAMDATA_POS)) = (int16)(testtheta+950);			//19=网侧d轴电流环输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 20 + BANK_RAMDATA_POS)) = (int16)(testtheta+1000);			//20=网侧q轴缌骰肥涑?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 21 + BANK_RAMDATA_POS)) = (int16)(testtheta+1050);			//21=机侧广义励磁电流
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 22 + BANK_RAMDATA_POS)) = (int16)(testtheta+1100);			//22=机侧励磁缌髦噶?
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 23 + BANK_RAMDATA_POS)) = (int16)(testtheta+1150);			//23=机侧d轴电流环输出
			*(BANK_RAMSTART+((Uint32)RAM_BIAS * 24 + BANK_RAMDATA_POS)) = (int16)(testtheta+1200);			//24=机侧q轴电流环输出

			*(BANK_RAMEND) = BANK_RAMDATA_POS;																//当前数据存储位置转存

			BANK_RAMDATA_POS++;
			if(BANK_RAMDATA_POS >= RAM_BIAS)  BANK_RAMDATA_POS=0;		
			
			M_ClrFlag(SL_RAMBANKSAVE);												//显示波形数据处于刷新标志
		}
		else	M_SetFlag(SL_RAMBANKSAVE);											//显示波形数据处于锁存标志

//--------------------------------External RAM Data Read-----------------------------------------
}  

void ERROR_Datasave(void)	//testPCmodbus
{
//--------------------------------External RAM Data Save-----------------------------------------
	if(M_ChkFlag(SL_PC_ERRDATACLR)!=0)						//PC指令外部RAM数据释放
	{
		M_ClrFlag(SL_RAMERRORSAVE);						
		*(ERROR_RAMEND + 1) = 0x0000;
		M_ClrFlag(SL_TRIG_ERRDSAVE);
		M_ClrFlag(SL_PC_ERRDATACLR);						//释放数据/复位完成,自动清除复位指令
	} 

  	if(*(ERROR_RAMEND + 1) != 0x55AA)
  	{
		if(M_ChkFlag(SL_TRIG_ERRDSAVE)==0)  								//没有故障	
			MAIN_LOOP.cnt_datasave=0;
		else if(M_ChkCounter(MAIN_LOOP.cnt_datasave,DELAY_DATASAVE)>=0)	//故障发生后延时0.5s禁止画图
		{
			M_SetFlag(SL_RAMERRORSAVE);
			M_ClrFlag(SL_TRIG_ERRDSAVE);
			*(ERROR_RAMEND + 1) = 0x55AA;			
		}

		if(M_ChkFlag(SL_RAMERRORSAVE)==0)  										//有故障之后延时2s后停止数据存储
		{
			*(ERROR_RAMSTART+ ERROR_RAMDATA_POS) = 1100;														//0=test
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 1 + ERROR_RAMDATA_POS)) = (int16)(testtheta* 1000);			//1=NPR定向角度
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 2 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.ab* 10);		//2=NPR-A电流
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 3 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.bc* 10);		//3=NPR-B电流
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 4 + ERROR_RAMDATA_POS)) = (int16)(testtheta+400);				//4=NPR-C电
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 5 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.ab* 10);		//5=电网电压Vab
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 6 + ERROR_RAMDATA_POS)) = (int16)(AD_OUT_NGS_U.bc* 10);		//6=电网电压Vbc
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 7 + ERROR_RAMDATA_POS)) = (int16)(testtheta+700);				//7=网压d轴分量ed滤波前
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 8 + ERROR_RAMDATA_POS)) = (int16)(testtheta+800);				//8=网压q轴分量eq滤波前
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 9 + ERROR_RAMDATA_POS)) = (int16)(testtheta+900);				//9=网压d轴分量ed滤波后
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 10 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1000);			//10=网压q轴分量eq滤波后
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 11 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1100);			//11=网侧Id指令
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 12 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1200);			//12=网侧Id反馈
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 13 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1300);			//13=网侧IdPI输出
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 14 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1400);			//14=网侧Ud输出
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 15 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1500);			//15=网侧Iq指令
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 16 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1600);			//16=网侧Iq反馈
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 17 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1700);			//17=网侧IqPI输出
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 18 + ERROR_RAMDATA_POS)) = (int16)(testtheta+1800);			//18=网侧Uq输出
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 19 + ERROR_RAMDATA_POS)) = (int16)(testtheta_sta* 1000);		//19=MPR定向角度
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 20 + ERROR_RAMDATA_POS)) = (int16)(testtheta+2000);			//20=MPR-A电流
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 21 + ERROR_RAMDATA_POS)) = (int16)(testtheta+2100);			//21=MPR-B电流
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 22 + ERROR_RAMDATA_POS)) = (int16)(testtheta+2200);			//22=MPR-C电流
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 23 + ERROR_RAMDATA_POS)) = (int16)(testtheta+2300);			//23=实际实时网频
			*(ERROR_RAMSTART+((Uint32)RAM_BIAS * 24 + ERROR_RAMDATA_POS)) = (int16)(testtheta+2400);			//24=电机转速

			*(ERROR_RAMEND) = ERROR_RAMDATA_POS;																//当前数据存储位置转存

			ERROR_RAMDATA_POS++;
			if(ERROR_RAMDATA_POS >= RAM_BIAS)  ERROR_RAMDATA_POS=0;		
		}
  	}

//--------------------------------External RAM Data Read-----------------------------------------
}  

*/
/*********************************************************************************************************
** 函数名称: Draw
** 功苊枋?: 绘仆夹?
** 输　入:
** 输　出:        
** 注  释: 
**-------------------------------------------------------------------------------------------------------
** 作　者: 
** 日　期: 
**-------------------------------------------------------------------------------------------------------
** 修改?:
** 日　期:
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
