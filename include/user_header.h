/****************************************Copyright (c)**************************************************
**                       		     北	京	交	通	大	学
**                                        电气工程学院
**                                         604实验室
**
**
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: user_header.h
**创   建   人: 
**最后修改日期: 
**描        述: 右玉1.5MW双馈风电机组并网变流器控制软件头文件----201005atcpc-201007atbjtu
				包括版本信息,宏定义,变量声明
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
#ifndef DCDC_header_H
#define DCDC_header_H

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------宏定义-----------------------------------------------
				
//-----------------------参数相关宏定义------------------------------------------
//--------------------------版本信息-----------------------------------------------
#define MC_TYPE 			2010			//机型参数
#define MC_REV 				210				//软件版本号（dot2）
#define MC_DATE 			2010			//出厂日期
#define MC_ID				1				//产品编号
#define MC_WRKEY			999				//功能码修改密码
#define MC_MYKEY			1021			//制造商使用的密码

//------------------------调试状态软开关-----------------------------------------------
//#define	ON					1				          //开
//#define	OFF					0				          //关
//#define	DEBUG_OPER			OFF			              //1=不带操作器2009.6.24
Uint16 DEBUG_OPER;                                        //20100401at27

//------------------------电机转子反序控制选择-201005atcpc-----------------------------------------
#define	R_PHORDE			1				          //开

//********************************************************************************************//
//===================额定电流基准值峰值（每次实验根据需要更改）=====================================
//#define NPR_IACN  473        //网侧额定电流基准值 额定有效值为334A
//#define MPR_IACN  545        //机侧额定电流基准值 额定有效值为385A
#define NPR_IACN  682        //网侧额定电流基准值 201205LVRTatZB LVRT实验改大 20120601night
#define MPR_IACN  682        //机侧额定电流基准值 额定有效值为482A 201107HC
#define STA_IACN  1000       //定子额定电流基准值 
#define GRD_UN    563        //额定电网相电压峰值690V系统
#define CHOP_IDCN 650        //dc-chopper电流保护额定值BJTULVRT201204

//********************************************************************************************//
 
//----------------------------系统工作常数------------------------------------------------

#define NPR_L       0.5e-3     //网侧滤波电感值（H）
#define MPR_L	    0.071e-3   //机侧dv/dt电感值(H)
#define	PLSPRVL     2048        //编码器每圈光栅数 注:该值改变，2.617994e-3要改,初始化EQep2Regs.QPOSMAX要改！encoder pulses per revolution cpc
/*
//电机参数----额定状态
#define MPR_Ls   	24.494882e-3	 	//定子自感	--单位H			（实际相值）
#define MPR_Lr   	15.889556e-3	 	//转子自感	--单位H			（实际相值）
#define MPR_Lm   	19.02e-3	 	    //定转子互感--单位H
#define MPR_Rs   	0.106	     		//定子电阻	--单位ohm		（实际相值）
#define MPR_Rr   	0.14995	    		//转子电阻	--单位ohm		（实际相值）
#define SIGMA	 	0.07053314			//(1-MPR_Lm*MPR_Lm/(MPR_Ls*MPR_Lr))	
#define	STAROTRTO	1.246504       		//定转子变比
#define	POLEPAIRES	2       			//电机极对数 
*/
/*
//CPC电机参数----额定状态归算到定子侧参数
#define MPR_Ls   	18.154e-3	 	    //定子自感	--单位H
#define MPR_Lr   	18.3258e-3	 	    //转子自感	--单位H	
#define MPR_Lm   	17.87e-3	 	    //定转子互感--单位H
#define MPR_Rs   	0.007747	     	//定子电阻	--单位ohm
#define MPR_Rr   	0.004175	    	//转子电阻	--单位ohm
#define SIGMA	 	0.040127			//(1-MPR_Lm*MPR_Lm/(MPR_Ls*MPR_Lr))	        
#define	STAROTRTO	0.5922       		//定转子变比
#define	STAROTRTO2	0.35070084       	//定转子变比的平方
#define	POLEPAIRES	2       			//电机极对数  
*/
//电机参数设定2010atcpc
float  	MPR_Ls;	 	    				//定子自感	--单位H 
float	MPR_Lr;	 	    				//转子自感	--单位H	
float 	MPR_Lm;	 	    				//定转子互感--单位H
float 	MPR_Rs;	     					//定子电阻	--单位ohm
float 	MPR_Rr;	    					//转子电阻	--单位ohm
float 	SIGMA;							//(1-MPR_Lm*MPR_Lm/(MPR_Ls*MPR_Lr))	        
float	STAROTRTO;       				//定转子变比
float	STAROTRTO2;       				//定转子变比的平方
float	POLEPAIRES;       				//电机极对数  
///-------------常用常量宏定义-------------------------//
#define	SQRT3		1.73205081			//sqrt(3)
#define	SQRT3_2		0.8660254           //sqrt(3)/2
#define	SQRT3_3		0.57735027          //sqrt(3)/3
#define	SQRT2		1.41421356			//sqrt(2)
#define	PAI			3.1415926           //圆周率
#define TWOPAI      6.2831853           //2*PAI
#define PAI_2       1.5707963			//PAI/2
#define PAI_3       1.0471975			//PAI/3
#define TWOPAI_3    2.0943951			//2*PAI/3
#define TWOPAI_12   0.523598766			//2*PAI/12
#define IRQTOTE     27.51385e-3			//转换系数：-Irq*乘该系数再乘网压ed=转矩Te

//------------------------------延时时间-------------------------------------------------
#define	DELAY_MAIN1			5										//一级循环：5ms
#define	DELAY_MAIN2			10										//二级循环：10ms
#define	DELAY_MAIN3			20										//三级循环：20ms
#define	DELAY_MAIN4			50										//四级循环：50ms
#define	DELAY_MAIN5			100										//五级循环：100ms
#define	DELAY_MAIN6			1000									//六级循环：1000ms

//canopen
//#define	DELAY_CANOPENTX		13										//CANOPENTX：7ms
#define	DELAY_CANOPENTX		6										//CANOPENTX：6ms	120507
#define	DELAY_CANOPENOVER	1000									//CANOPENOVER：500ms,HEARTBEAT 2014-05-06LJD临时修改CANopen
#define	DELAY_SCICANOPENOVER 250									//20090817
#define	DELAY_CANFAULT       10000									//CANOPENOVER：上电后判断CAN通讯故障

#define DELAY_AMUX          5      	//慢速AD
#define DELAY_UDCWAVE		1500	//直流电压波动超时时间

#define	DELAY_POWERON		5000  	//上电时间
#define	DELAY_PWMOUT		1000   	//系统从启动标志置位到脉冲发生允许标志置位延时(ms)
#define	DELAY_NPRSTDBY		1000   	//NPR待机---板子工作灯慢速闪烁(ms)
#define	DELAY_NPRRUN		100	   	//NPR工作---板子工作灯快速闪烁(ms)
#define	DELAY_MPRSTDBY		1000   	//MPR待机---板子工作灯慢速闪烁(ms)
#define	DELAY_MPRRUN		100	   	//MPR工作---板子工作灯快速闪烁(ms) 
#define	DELAY_FREQ			2000	   	//频率失败允许延时
#define	DELAY_CBFREQ		1000	//从主断反馈闭合到判断网频故障的延时（ms）

#define	DELAY_FREQ2			200	   	//频率失败允许延时
#define	DELAY_OTSER			60000  	//超时严重延时(ms)
#define	DELAY_SCISPACE		10	   	//通讯接收到发送的间隔时间(ms)
#define	DELAY_SCIOVER		5000	//通讯接收/发送超时时(ms)!!!!调试界面后修改20100125//have corected
//-----Protection-------
#define	DELAY_ESTOP  		6					//
#define	DELAY_CBTP		    6					//
#define DELAY_SWITCHDETECT	50		//开关意外动作故障检测延时10ms	110818 
#define DELAY_DETECT		2000	//开关意外动作检测切换延时1s	110818
#define DELAY_EXFAULT       500 
#define DELAY_CBERROR       1000     //0.5s		110818 20110829HC
#define DELAY_MAINKERROR    1000     //0.5s		110818
#define DELAY_MAINFERROR    1000     //0.5s		110818
#define DELAY_STACERROR     1000     //0.5s		110818
#define DELAY_PREERROR      15000    //15s
#define DELAY_PGOVLOAD      10000    //10s
#define DELAY_QGOVLOAD      10000    //10s
#define DELAY_CBRESET       500    	 //0.5s
#define DELAY_SPEEDOUT      50       //50ms 20091022atzy 主控fastshutdown
#define DELAY_UACLV1        10000     //10s   20091027atzy
#define	DELAY_GFAULT		3000	//三相电流不平衡到接地故障延时(ms)
#define DELAY_PHORDE        12000     //12s   201005atcpc
#define DELAY_ENCOFAULT     150     //150ms   201005atcpc

#define	DELAY_NPRTLOV		60000	//网侧电感超温
#define DELAY_MPRTLOV		60000	//机侧电感超温
#define	DELAY_NSKIPTOV		10		//网侧SKIIP超温 systest
#define DELAY_MSKIPTOV		10		//机侧SKIIP超温 systest

#define	DELAY_EE		    20					//eeprom延迟时间

#define	DELAY_ISTEADY0	    1000	 //RunCtrl网侧停机给定延时1s
#define	DELAY_MPRSTEADY0    50     //RunCtrl网侧停机给定延时0.05s 
#define	DELAY_SENSZFSTDY    60000    //RunCtrl网侧停机给时60s

//sysctl_start
#define	DELAY_STEADYFB	    2000	 //正常启动，检测到Vdc稳定在1100V到允许机侧脉冲的延时 2s
#define	DELAY_PRECOK        8000	 //正常启动，预充电时间 8s
#define	DELAY_SERSTOP		2000	 //严重故障下，发出断主断指令后到发出断定子接触器和主接触器的延时 2s 
#define	DELAY_ERRSTOP	    2000	 //接收到系统停机指令后，延时断主接触器 2s
#define	DELAY_EPWMSTOP	    500	 //正常启动，一般故障封脉冲后延时断主断再断接触器 500ms  20110829
#define	DELAY_CBON	    	1000	 //正常启动，闭合主断欠压链到主断合闸的延时计数 1s 
#define	DELAY_SYSOFF	    2000	 //风机停机，无视转矩指令，自行停机后延时2s置停机结束标志	20120321
//sysctl_end  

#define	DELAY_RESET	        1000  	 //系统复位延时，防止误操作 2s
#define	DELAY_CLOSTACMD	    1000  	 //允许合定子接触器，防止误操作 1s
#define	DELAY_NPRCMD	    1000  	 //上位机允许NPR发脉冲，防止误操作 1s
#define	DELAY_MPRCMD	    1000  	 //上位机允许MPR发脉冲，防止误操作 1s
#define	DELAY_EIN	    	1000  	 //衔换刂浦鞫希乐刮蟛僮? 1s
#define	DELAY_SYSRUN	    1000  	 //上位机控制系统运行/电机试验，防止误操? 1s
#define	DELAY_PREC	     	1000  	 //上位机控制预充电，防止误操作 1s

#define	DELAY_QCAPSPDIN     120    	//转子速度测正常120ms   20090817
#define	DELAY_QCAPDISTURB   100    	//连续10次检测转子速度不正常报故障 100ms  20090817
#define	DELAY_QEPCNTOK      1000    //连续1s未发生上溢错误 1s
#define	DELAY_QEPZDISTURB   1000    //连续1s发生Z信号提前到来被干扰错误  1s

#define	DELAY_OCSEIN1     	250    	//20090817
#define	DELAY_OCSEIN2   	250    	//20090817
#define	DELAY_OCSSYSRUN1    250    	//20090817
#define	DELAY_OCSSYSRUN2   	250    	//20090817
#define	DELAY_MC_ACK		500  	 //500ms MagnetCurve2013-12-13

#define	DELAY_DATASAVE   	1800     //故障发生后延时0.5s后停止存储20100203atbjtu
//LVRT
#define	DELAY_SCRIZERO	    15  	 //检测延时0.5个最大转差周期 100ms/10Hz//201007BJTULVRT
#define	DELAY_MPWMSTOP	    0  	 	 //0ms
#define	DELAY_SCRIZERO2	    2  	 	 //2ms 20121103
#define	DELAY_MPWMRESTART	50  	 //50ms//201007BJTULVRT
#define	DELAYMPWM			0  	 	 //5ms
#define	DELAY_NPWMRESTART	50  	 //50ms//201007BJTULVRT
#define	DELAY_LVSTATE		2  	 	 //2ms
#define	DELAY_HVLVRCV			10  	 //40ms
#define	DELAY_LV_T1			625  	 //625ms
#define	DELAY_LV_T2			3000  	 //3000ms
#define	DELAY_HVLV_DETEC		1000  	 //1000ms
#define	DELAY_SCRONDELAY	2000  	 //2s
#define	DELAY_LVFAULT1		200  	 //200ms
#define	DELAY_LVFAULT2		100  	 //100ms



//---------20121103atsp-----------
#define	DELAY_QWORKING	100  	 //100ms 20121103
#define	DELAY_PHICON		50  	 //50ms 
#define	DELAY_EQUIP_CD		60  	 //5min 2016-1-13
#define	DELAY_LVTEST3		3  		 //3s 2016-3-31

#define	DELAY_LVUBALNCE		30  	 //30ms 201011LVRT
#define	DELAY_LVUTLV		500  	 //500ms 
#define	DELAY_LVMIDVAR		20  	 //20ms 

#define	DELAY_FREQRECV		10  	 //10ms 201011PLL
#define DELAY_IDCOV			300		//延迟10ms检测chopper电流	201205LVRTatZB20120531
#define DELAY_IDCOVDELAY	1000	//延迟1s检测chopper电流	BJTULVRT201204

#define	DELAY_LV_START_SCRON		3		//3ms
#define	DELAY_VDSCROFF		2		//7ms

#define	DELAY_CHOPPER		100  	 //100ms 2013-12-6ZZJ  

//---------------------------故障保护回差值-----------------------------------------------
#define	SC_IAC_HW			2			//输入电流保护回差(基准电流的百分值)
#define	SC_UDC_HW			20			//电压保护回差(V)
#define	SC_UAC_HW			20			//电压保护回差(V)
#define	SC_MSPD_HW			30			//转速保护回差(V) r/min
#define	SC_POWOROV_HW	    0	    	//功率过载保护回差(W) 0kW
#define	et_gate	    		50000	    //过载保护计算

//-----------------------------CANOPEN通信宏定义----------------------------------------------
//#define	SCICANOPENRXNUM		10										//CANOPENRX数据包大小
//#define	SCICANOPENTXNUM		18										//CANOPENTX数据包大小
#define CANOPENCOM_HEAD		0x7E									//CANOPEN报头
#define	SCICANOPENRXNUM		14										//CANOPENRX数据包大小120507
#define	SCICANOPENTXNUM		22										//CANOPENTX数据包大小120507
#define	SCICANOPENREADNUM	7										//CANOPENRD读取指令数据包大小120507
//-----------------------------CANOPEN通信宏定义NORMAL----------------------------------------------
#define CAN_TEN  	  10000     //主控指令额定转矩基准值 (常牵:1800r/min时P=1560kW,Te=P*60/(n*2*pie)=8276 NM )
#define CAN_ANGLEN     90       //主控指令额定无功因数角度基准值 
#define CAN_SPEED     1500      //主控指令额定转速基准值 
#define CAN_TEMP      100       //主控指令额定温度基准值 

//-----------------------------485通讯----------------------------------------------
#define	SCI485NUM_RX		8										//485数据包大小modbus

//#define UPSTART			    0x0064									//恢复出厂参数的校验码
//#define SYSRESUME			0x0067									//恢复出厂参数的校验码

//上位机发送命令
#define COM_RDDO			0x0001									//读取功能码(读取下位机EEPROM中的功能码值写入到上位机EEPROM中)
#define COM_RDAO			0x0003									//读取功能码(读取下位机EEPROM中的功能码值写入到上位机EEPROM中)
#define COM_WRDO			0x0005									//修改功能码(修改下位机RAM中的功能码值)
#define COM_WRAO			0x0006									//修改功能码(修改下位机RAM中的功能码值)
#define COM_RDRAM			0x0046									//读取exRAM功能码(20100203atbjtu)

//------------------------------CANOPEN-------------------------------------------------------------
//CANOPEN上位机状态字
#define COM_EIN			    0x0001									//BIT0
#define COM_NPREIN			0x0002									//BIT1
#define COM_SYSRUN			0x0004									//BIT2
#define COM_PGMODE			0x0010									//BIT4
#define COM_QGMODE			0x0020									//BIT5
#define COM_OCSRESET		0x0080									//BIT7
#define COM_HEARTBEAT1		0x8000									//BIT15
//CANOPEN反馈上位机的变流器状态字1
#define COM_NPRREADY	    0x0001									//BIT0
#define COM_NPRON			0x0002									//BIT1
#define COM_READYGENERATION	0x0004									//BIT2
#define COM_FAILURE			0x0008									//BIT3
#define COM_SPEEDINRANGE	0x0010									//BIT4
#define COM_FAILUREA		0x0020									//BIT5
#define COM_FAILUREB		0x0040									//BIT6
#define COM_TOOCOLD		    0x0080									//BIT7
#define COM_PLIM	        0x0100									//BIT8
#define COM_QLIM			0x0200									//BIT9
#define COM_GRIDFAILURE 	0x0400									//BIT10
#define COM_CROWBAR			0x0800									//BIT11
#define COM_VLIM        	0x1000									//BIT12
#define COM_HEARTBEAT2		0x8000									//BIT15

#define COM_NPRREADY_NOT	    0xFFFE								//BIT0
#define COM_NPRON_NOT			0xFFFD								//BIT1
#define COM_READYGENERATION_NOT	0xFFFB								//BIT2
#define COM_FAILURE_NOT			0xFFF7								//BIT3
#define COM_SPEEDINRANGE_NOT	0xFFEF								//BIT4
#define COM_FAILUREA_NOT		0xFFDF								//BIT5
#define COM_FAILUREB_NOT		0xFFBF								//BIT6
#define COM_TOOCOLD_NOT		    0xFF7F								//BIT7
#define COM_PLIM_NOT	        0xFEFF								//BIT8
#define COM_QLIM_NOT			0xFDFF								//BIT9
#define COM_GRIDFAILURE_NOT 	0xFBFF								//BIT10
#define COM_CROWBAR_NOT			0xF7FF								//BIT11
#define COM_VLIM_NOT        	0xEFFF								//BIT12
#define COM_HEARTBEAT2_NOT		0x7FFF								//BIT15
//CANOPEN反馈上位机的变流器状态字2
#define COM_ILIM			0x0001									//BIT0
#define COM_HEATSINKT		0x0002									//BIT1
#define COM_ESTOP			0x0004									//BIT2
#define COM_CBTRIP			0x0010									//BIT4
#define COM_POWLIM			0x0040									//BIT6
#define COM_VOLDIP			0x0100									//BIT8
#define COM_CBCLOSED		0x8000									//BIT15	 20090815

#define COM_ILIM_NOT		0xFFFE									//BIT0
#define COM_HEATSINKT_NOT	0xFFFD									//BIT1
#define COM_ESTOP_NOT		0xFFFB									//BIT2
#define COM_CBTRIP_NOT		0xFFEF									//BIT4
#define COM_POWLIM_NOT		0xFFBF									//BIT6
#define COM_VOLDIP_NOT	    0xFEFF									//BIT8
#define COM_CBCLOSED_NOT	0x7FFF									//BIT15 20090815

//-------------------------AD、DA转换及数据读取的控制-----------------------------------------
#define	AD8364_CONVERT		1										//8364开始转换
//#define	AD1544_CONVERT		2										//1544开始转换
//#define	DA7614_CONVERT		4										//7614开始转换
//ad
#define AD_ASTART		    ((Uint16 *)0x100000)						//片外AD的数荻寥∈椎刂?
#define AD_BSTART		    ((Uint16 *)0x110000)						//片外AD的数据读取首地址
#define AD_CSTART		    ((Uint16 *)0x120000)						//片外AD的数据读取首地址
#define AD_DA_CTRL		    ((Uint16 *)0x130000)						//片外AD,DA的控制字节地址
//da
#define DA_ADD0				((Uint16 *)0x1B0000)						//输出地址0
#define DA_ADD1				((Uint16 *)0x1B0001)						//输出地址1
#define DA_ADD2				((Uint16 *)0x1B0002)						//输出地址2
#define DA_ADD3				((Uint16 *)0x1B0003)						//输出地址3

//External RAM 20091109atzy
#define BANK_RAMSTART		((Uint16 *)0x200000)						//片外RAM首地址XINTF zone 7
#define BANK_RAMEND			((Uint16 *)0x21E848)						//片外RAM尾地址XINTF zone 7 end at 125000
#define ERROR_RAMSTART		((Uint16 *)0x21FBD0)						//片外RAM首地址XINTF zone 7 start at 130000
#define ERROR_RAMEND		((Uint16 *)0x23E418)						//片外RAM尾地址XINTF zone 7 end at 255000
#define RAM_BIAS			5000										//=2s/(0.4ms/point)=5000points/variable
Uint16  BANK_RAMDATA_POS;						    					//存储变量的存储点位置[0,4999]
Uint16  ERROR_RAMDATA_POS;						    					//存储变量的存储点位置[0,4999]
#define RAM_SEGMENT			1000											//每个波形每次发送分段打包长度


//----------------------------输入输出地址定义--------------------------------------------
#define IN1_ADDR			((Uint16 *)0x140000)	//输入地址1
#define IN2_ADDR			((Uint16 *)0x150000)	//输入地址2
#define IN3_ADDR			((Uint16 *)0x160000)	//输入地址3
#define IN4_ADDR			((Uint16 *)0x170000)	//输入地址4

#define OUT1_ADDR			((Uint16 *)0x180000)	//输出地址2
#define OUT2_ADDR			((Uint16 *)0x190000)	//输出地址3
#define OUT3_ADDR			((Uint16 *)0x1A0000)	//输出地址3
#define OUT4_ADDR			((Uint16 *)0x1C0000)	//输出地址4,与慢速AD有关

//--------------------------------SPI通讯宏定义-------------------------------------------
#define	SPINUM				4										//SPI口发送/接收数据量

//-----------------------------------时钟--------------------------------------------------
#define	HSPCLK				75      		//高速基准时钟频率(Hz)
#define	LSPCLK				37500000		//低速基准时钟频率(Hz)

//--------------------------------标志位定-----------------------------------------------
//flag[0]显示及设定标
#define	SL_DISPLAY0			0x00			//LED显示0
#define	SL_DISPLAY1			0x01			//LED显示1
#define	SL_DISPLAY2			0x02			//LED显示2
#define	SL_DISPLAY3			0x03			//LED显示3
#define	SL_DISPLAY4			0x04			//LED显示4
#define	SL_DISPLAY5			0x05			//LED显示5
#define	SL_DISPLAY6			0x06			//LED显示6
#define	SL_DISPLAY7			0x07			//LED显示7
#define SL_RAMERRORSAVE     0x08            //=1外部RAM故障部分锁存标志
#define SL_RAMBANKSAVE      0x09            //=1外部RAM故障部分锁存标志
#define	SL_TRIG_ERRDSAVE    0x0A      		//触发外部RAM数据转存20091109atzy
#define	SL_PC_ERRDATACLR    0x0B      		//触发外部RAM数据释放 PCmodbus20100305
#define	SL_PC_ERRDATASAVE   0x0C      		//PC触发外部RAM数据锁存 PCmodbus20100308
#define SL_PC_BANKDATASAVE  0x0D            //PC指令外部RAM显示数据锁存 PCmodbus20100308
#define	SL_MC_OK   			0x0E      		//励磁曲线MagnetCurve2013-12-13
#define SL_MC_FAIL          0x0F    		//励磁曲线MagnetCurve2013-12-13

//#define	SL_STA_PHORDE   	0x0E      		//TEST
//#define SL_ROT_PHORDE  		0x0F            //
  
//flag[1]串行通信相关标;eeprom相关标
#define SL_TX		    	0x10			//1=表示sci正在发送
#define SL_RX		    	0x11			//1=表示sci接收完成延时中
#define SL_RESUME    		0x12			//1=恢复出厂参数进行中
#define SL_HEADOK			0x13			//1=本帧数据报头正确
#define SL_SETRTIMER		0x14			//1=需要校正实时时钟
#define SL_EE_NOACK			0x15
#define	SL_EE_FAIL			0x16			//=1eeprom写入错误
#define SL_CODEOK			0x17			//=1功能码校验完毕
#define SL_EEASK_INI		0x18			//=上位机要求eeprom恢复出厂参数
#define SL_EEASK_MCODE		0x19			//=1存在修改功能码请求
#define SL_EEASK_ERRSAVE	0x1A			//=1存在保存故障信息请求
#define SL_EEBUSY_INI		0x1B			//=1恢复出厂参数进行中
#define SL_EEBUSY_MCODE		0x1C			//=1修改功能码进行中
#define SL_EEBUSY_ERRSAVE	0x1D			//=1保存故障信息进行中 
#define SL_TXLONGDATA		0x1E			//=1发送长数据进行中 

//#define SL_PHO_FH			0x1F			//=1TEST
#define SL_CANOPENTX_FLAG	0x1F			//120507
	
//flag[2]QEP相关标志位 及故障标志位
#define SL_QEPPCO			0x20			//=1 QEP计数上溢故障
#define SL_QCAPDISTURB		0x21			//=1 QEP脉冲受到干扰故障 连续10次读取均不在正确范围内 20090804cpc
#define	SL_QEPZDISTRUB	    0x22     	    //=1 QEP Z信号受到干扰，连续1s内错误就报故障   20090804cpc
#define SL_ENCODFAULT		0x23			//=1 QEP故障(编码器故障)
#define	SL_QEPPCDE          0x24      		//编码器转向改变错误,A或B信号消失
#define	SL_QCAPSPDIN        0x25      		//QEP的CAP电路能够正确检测 qcap speed in 
#define	SL_SPEED_HIGH       0x26      		//高转速区采用QCLK128分频，低转速采用8分频 
//flag[2]接触器故障标志位
#define SL_ERROR_CB        	0x27        	//=1主断有故障 
#define SL_ERROR_MAINF   	0x28         	//=1主滤波器故障
#define SL_ERROR_MAINK     	0x29         	//=1主接触器故障
#define SL_ERROR_STAC		0x2A         	//=1定子接触器故障
#define SL_ERROR_PRE       	0x2B         	//=1预充电收? 
#define SL_MSPOUT        	0x2C         	//=1转速超出范围故障 
#define SL_SYNFAIL        	0x2D         	//=1定子同步并网失败故障 
#define SL_STEADYGV			0x2E			//=1Vdc给定稳定
#define SL_STEADYFB			0x2F			//=1Vdc反馈电压稳定

//flag[3]状态标
#define SL_POWERON			0x30			//=1系统上电完成
#define SL_WARNING			0x31			//=1报警
#define SL_OFFCB			0x32			//=1断主断
//#define SL_SOFT				0x33			//=1缓慢停止
#define SL_DL_RESET			0x33			//=1延时允许复位
#define SL_SHUT				0x34			//=1紧急停止
#define SL_NRCVR			0x35			//=1故障不能恢复
#define SL_DRCVR			0x36			//=1故障延时恢复
#define SL_IRCVR			0x37			//=1故障立即恢复
#define SL_CNT				0x38			//=1计入故障次数
#define SL_OTSER			0x39			//=1故障超时严重
#define SL_SAVE				0x3A			//=1故障需要存储
#define	SL_NO_RESET        	0x3B      		//=1禁止复位标志位 
#define SL_IUBLNCE1     	0x3C			//1=电网侧漏电流超限
#define SL_IUBLNCE2      	0x3D			//1=机侧漏电流超限
#define SL_GFAULT1			0x3E			//1=电网侧接地故障	
#define SL_GFAULT2		    0x3F			//1=机侧接地故障	

//flag[4]故障标1
#define SL_ERROR			0x40			//=1系统故障
#define SL_ERRSTOP			0x41			//=1系统一般故障
#define	SL_SERIESTOP        0x42      		//=1系统严重故障 
#define	SL_PDPINTA			0x43			//=1功率驱动保护A
#define	SL_PDPINTB			0x44			//=1功率驱动保护B
#define	SL_UDCWAVE			0x45			//=1中间电压不稳定，超出580V和620V
#define SL_HIA1			    0x46			//=1网侧变流器A相硬件过流
#define SL_HIB1			    0x47			//=1网侧变流器B相硬件过流
#define SL_HIC1			    0x48			//=1网侧变流器C相硬件过流
#define SL_HIA2		 	   	0x49			//=1电机侧变流器A相硬件过流
#define SL_HIB2		   	 	0x4A			//=1电机侧变流器B相硬件过流
#define SL_HIC2		   	 	0x4B			//=1电机侧变流器C相硬件过流
#define SL_SIAC1		    0x4C			//=1网侧变流器软件过流
#define SL_SIAC2		    0x4D			//=1电机侧变流器软件过流
#define SL_SUDCOV			0x4E			//=1中间直流正过压
#define SL_UACLV2			0x4F			//=1网压瞬间跌落故障LVRT 201007BJTULVRT

//flag[5]故障标2
#define SL_UACOV1			0x50			//=1网压软件过压
#define SL_UACLV1			0x51			//=1网压软件欠压
#define SL_PGOV				0x52			//=1变流器有功功率过载
#define SL_QGOV				0x53			//=1变流器无功功率过载
#define	SL_GRDFQE			0x54			//=1网侧频率检测有误
#define SL_FE1			    0x55			//=1网侧频率故障
#define	SL_PDPASERIES		0x56			//=1网侧严重故障
#define SL_PDPBSERIES		0x57			//=1电机侧严重故障
#define SL_HUDCOV			0x58			//=1硬件过压故障
#define SL_HIACOV1			0x59			//=1网侧交流硬件保护过流故障
#define SL_HIACOV2			0x5A			//=1电机侧交流硬件保护过流故障
#define SL_CBTRIP 			0x5B			//=1主断脱扣或者主控断开主断故障
#define	SL_EXFAIL		    0x5C			//=1变流器外部硬件故障
#define	SL_ESTOP			0x5D			//=1外部紧急停机故障
#define SL_PHORDE		    0x5F            //=1相序正常

//flag[6]运行方式
#define SL_ENPCOPER			0x60			//控制模式：1=PC控制；0=CAN控制			modbus2010
#define SL_OPERATION		0x61			//运行模式：1=网侧单独运行 0=背靠背运行 modbus2010
#define	SL_NPR_START		0x62			//网侧变流器运行指令 1=运行
#define	SL_MPR_START		0x63			//机侧变流器运行指令 1=运行
#define	SL_NPR_RUN			0x64			//=1网侧变流器工作允许
#define	SL_MPR_RUN			0x65			//=1机侧变流器工作市? 
#define	SL_NPR_RUNING		0x66			//=1网侧变流器正在工?
#define	SL_MPR_RUNING		0x67			//=1机侧变流器正在工作
#define SL_NPR_PWMOUT       0x68            //=1网侧脉冲已经使能标志位,并且保证Enepwmio_NPR()只能运行一次
#define SL_MPR_PWMOUT       0x69            //=1机侧脉冲已经使能标志位,并且保证Enepwmio_MPR()只能运行一次
#define SL_RUN				0x6A			//=1系统诵?
#define SL_CONFIGPWM		0x6B			//=1PWM脉冲配置完成标志位,onfigPwm()只能运行一次
#define SL_FAN_WORK         0x6C			//=1要求功率风扇开始工作
#define SL_CHARGEOK			0x6D			//=1预充电完成
//#define SL_SENSZFSTDY       0x6E            //=1传感器零漂滤波结束 
#define SL_UNBALANCE       	0x6E            //=1电网不平衡标志位 20121103
#define SL_SPEED_IN_RANGE	0x6F			//=1转速在范围以内
								
//flag[7]输出标1,IO输出   0x180000          //电器输出，=1 输出DC24V
#define _OUT1_DATA			flag[7]	
#define CL_CBLVC            0x70         	//=0主断断开指令P5.5
#define CL_CBON   			0x71			//=1要求合主断P5.6
#define CL_PRE   			0x72			//=1要求合预充电P5.7
#define	CL_MAINK    		0x73			//=1要求合控制主接触器和滤波器器P5.8
#define CL_STATORK		    0x74			//=1要求合控制定子接触鱌5.9
#define CL_CBFCON           0x75			//=1要求主断欠压链闭合，0=主断欠压链断开P5.10
#define CL_FANWORK	        0x76			//=1要求功率风扇运行P5.11
//#define CL_SAVEC	        0x77			//=1正常,安全链闭合P5.12 20110829
#define CL_ZKHVLVRT	        0x77			//=1发生网压跌落，发给主控24V信号20121107
//高八位 HVRT标志位
#define SL_HV_STATE	        0x78			//=1高电压穿越标志
#define SL_HV_QWORKING 	    0x78			//=1高压发无功
#define SL_HV_STRICTLV      0x78			//=1严格HV区间
//#define SL_HV_STATE	    0x78	        //=1高电压

//flag[8]输出标2         0x190000           //继电器输出，=1 输出AC220V
#define _OUT2_DATA			flag[8]				
#define CL_RELAY8      		0x80			//备用
#define CL_CHOPTEST        	0x81			//斩波测试 2013-12-6ZZJ 
#define SL_WATCHDOG			0x82            //DSP软件看门狗 20100401at27
//#define SL_SWITCHDETECT    	0x84        //=1检测意舛? 		110818
#define CL_CHOPEN			0x83			//enable chopper

//--------以下标疚环鞘涑觯诓勘曛?--------------------------------------------
#define SL_ERRSTOP_S1		0x84            //一般故障步骤标志位20110829
#define SL_SWITCHDETECT    	0x85         	//=1检测意舛? 		110818
#define SL_QEPPHE    		0x86         	//=1电机反转或AB反接故障 20110829HC
#define SL_SYSOFF    		0x87         	//=1风机正常停机时转速过低，不再接收主控转矩指令，自行停机	20120321
#define SL_IDCOV    		0x88         	//=1中间直流斩波软件电流过流判断-BJTULVRT201204
#define SL_LV_SYNCON    	0x89         	//=1机侧动态PI控制标志位-BJTULVRT201204
#define SL_LV_PHICON    	0x8A         	//=1机侧有源磁链衰减控制标志位-BJTULVRT201204
#define SL_CHOPPEREN		0x8B			//=1斩波器使能		20120228	BJTULVRT201204
#define SL_LV_SCRKEEPON		0x8C			//=1crowbar一直开通标志20120602
#define SL_LV_START_SCRON		0x8D			//=1电压跌落触发SCR 20121203
#define SL_LV_SCROFF1		0x8E			//=1 35%不对称关SCR 20121210
#define SL_LV_SCROFF2		0x8F			//=1 50%不对称关SCR 20121210

//flag[15]输出标4         0x1A0000          //慢速AD地址输出
#define _OUT3_DATA			flag[15]									
#define SL_PHASEA			0xF0		     //					
#define SL_PHASEB			0xF1             //
 
Uint16   _OUT4_DATA;		//该标志位无用改为宏定义20130222

//flag[11]输出标4         0x1C0000          //AD址输出
/*#define _OUT4_DATA			flag[11]									
#define SL_LSADA0			0xB0		     //A0						
#define SL_LSADA1			0xB1             //A1
#define SL_LSADA2			0xB2             //A2
#define SL_LSADA3			0xB3	         //A3							
*/
//flag[11]LVRT相关标志位	20130222
#define SL_LV_RECMSTOP			0xB0			//=1恢复时刻主动封脉冲	20130124
#define SL_LV_ZDNSTOP			0xB1			//=1跌落时刻网侧主动封脉冲	20130202
//#define SL_LV_XNZKKZ			0xB2			//=1虚拟阻抗控制期间	20130202
#define SL_LV_STRICTLV			0xB2			//=1严格LV区间	20130222
#define SL_LV_RECNSTOP			0xB3			//=1恢复时刻主动封脉冲	20130206
#define SL_LV_ZDMSTOP			0xB4			//=1跌落时刻机侧主动封脉冲	20130221
#define SL_SCRDONE			0xB5			//=1 投入scr一次20130221
#define SL_LV_CLRERRAM			0xB6			//=1 清除LV误触发波形20130306
#define SL_CANOPENHEAD			0xB7			//=1 收到报头	20130801
#define SL_ENCODEPOS_OK			0xB8			//=1编码器位置检测完成	20130801
#define SL_ECPOSSEARCH			0xB9			//=1编码器位置校验过 	20130801


//flag[9]输入标12
#define _IN12_DATA			flag[9]			//变流器布刺蠢?
//---------------------0x140000----------------------------------------------------------------------------	
#define SL_IN1_CBSTS 			0x90			//=1 主断处于闭合状态P4.1
#define SL_IN1_CBRESET			0x91			//=1 检测到骺匾笾鞫蟁ESET指令(需要延时一段时间后才能合主断)，P4.2
#define SL_IN1_STATORK			0x92			//=1 定子接触器处于合闸态，P4.3  
#define SL_IN1_MIANK   			0x93			//=1 主接触器处于合闸状琍4.4
#define SL_IN1_MIANFILTER		0x94			//=1 主滤波器处于合闸状态，P4.5
#define SL_IN1_EXFAULTOK	   	0x95			//=1 外部故障链正常，0=故障发生	P4.6		
#define SL_IN1_MIANFAN	    	0x96	        //=1 功缁＃?0=超温障   P4.7 
#define SL_IN1_EXESTOP	        0x97			//=1 外部急停故障  变流器柜上紧急停磁グ聪?=1 P4.8	090815				
//---------------------0x150000---------------------------------------------------------------------------	
#define SL_IN2_IOVA1			0x98			//=1 网侧A相 SKiiP故障(过流或者超温)
#define SL_IN2_IOVB1			0x99			//=1 网侧B相 SKiiP故障(过流或者超温)
#define SL_IN2_IOVC1			0x9A			//=1 网侧C SKiiP故障(过流或者超温)					
#define SL_IN2_TAOV			    0x9B			//=1 网侧SKiiP超温故障 
#define SL_IN2_IOVA2			0x9C			//=1 电机侧A相SKiiP故障(过流或者超温）
#define SL_IN2_IOVB2			0x9D			//=1 电机侧B相 SKiiP故障(过流或者超温)
#define SL_IN2_IOVC2			0x9E			//=1 电机侧C相 SKiiP故障(过流者超温)
#define SL_IN2_TBOV				0x9F			//=1 电机侧SKiiP超温故障


//flag[10]输入标34      0x160000
#define _IN34_DATA			flag[10]			//控制板硬件保护故障反馈
//-----------------------------------------------------------------------------------------------				
#define SL_IN3_VDCOV			0xA0			//=1 直流电压过压
#define SL_IN3_NPRIOV			0xA1			//=1 网侧交流硬件保护过流故障
#define SL_IN3_MPRIOV			0xA2			//=1 电机侧交流硬件保护过流故障

#define SL_IN3_CBTRIP		    0xA3			//=1 主断过流脱扣故障  P4.9   20090816
//#define SL_IN3_OCS_NPRSTART		0xA4			//=1 上位机允许发网侧脉冲  操作板信号，风场没用
//#define SL_IN3_OCS_MPRSTART		0xA5			//=1 上位机允许发机侧脉冲  操作板信号，风场没用
#define SL_IN3_IOVDC			0xA6			//=1 dc-link chopper errror 20120228atbjtu	BJTULVRT201204
//注意：用高8位时，输入赋值会清零20130222atzb

//flag[12]主控通讯及PC指令标志位
#define SL_OCS_NPREIN       0xC0            //=1主控要求变流器网侧单独无功并网运行				
#define SL_OCS_EIN          0xC1            //=1主控发过来的系统启动信号  bit0 合主断路器
#define SL_OCS_SYSRUN    	0xC2			//=1主控发过来的变流器启动信号  bit2 开始预充电直至定子接触器闭合
#define SL_CBCLOSED    	    0xC3			//=1反馈主控，主断已经闭合
#define SL_MPR_SYNOK        0xC4            //=1机侧同步并网完成标志位				
#define SL_PGOV_COM         0xC5            //=1有功功率超出限制，通知主控 持续10s后变流器会报故障			
#define SL_QGOV_COM         0xC6            //=1无功功率超出限制，通知主控 持续10s后变流器会报故障				
#define SL_OCS_PREC			0xC7			//=1PC控制预充电指令
#define SL_OCS_NPRSTART     0xC8            //=1PC控制要求NPR发脉冲        
#define SL_OCS_MPRSTART     0xC9            //=1PC控制要求MPR发脉冲
#define SL_OCS_RESET        0xCA            //=1主控发过来的故障复位信号  
#define SL_OCS_OPENSTAC		0xCB			//=1禁止闭合定子接触器 1=禁止闭合
#define SL_PCOCS_CBOFF		0xCC			//=1PC控制手动断开CB 1=断开 2010atcpc
//canopen
#define SL_CANOPENOVER		0xCD			//=1 CANOPEN FAULT
#define SL_CANOPENHEADOK	0xCE			//=1 SL_CANOPENHEADOK
#define SL_CANOPENTX		0xCF			//=1 SL_CANOPENOVER 


//flag[13]
//LVRT标志位
#define SL_LV_QWORKING      0xD0       		    
#define SL_MSTOP         0xD1       		    
#define SL_NSTOP         0xD2
#define SL_SCRON         0xD3
#define SL_SCRRUNING     0xD4
#define SL_SCRIZERO      0xD5
#define SL_HVLV_MPWMOUT		0xD6		   //=1 机侧低穿封脉冲
#define SL_LV_STATE		    0xD7		  
#define SL_HVLV_DETEC       0xD8		
//系统逻辑控制 标志位
#define SL_SYN_S3         	0xD9			//SYN步骤标志位
#define SL_SYN_S2         	0xDA			//SYN步骤标志位
#define SL_SYN_S1     		0xDB			//SYN步骤标志位	
#define SL_NORSTOP_FH      	0xDC			//NORSTOP步骤标志位	
#define SL_NORSTOP_S2		0xDD			//NORSTOP步骤标志位	
#define SL_NORSTOP_S1		0xDE			//NORSTOP步骤标志位	
#define SL_ERRSTOP_FH		0xDF			//ERRSTOP步骤标志位		

//flag[14]
#define SL_RESET            0xE0
#define SL_SIOVST           0xE1
#define SL_NPR_TLOV         0xE2
#define SL_MPR_TLOV         0xE3
#define SL_STACKEEPON		0xE4			//=1定子并网电流较大，定子接触器禁止断开 20100510
#define	SL_TAOV				0xE5			//=1网侧超温
#define SL_TBOV			    0xE6			//=1机侧超温
#define SL_SKTOV            0xE7            //=1软件判断SKIIP超温
#define SL_SERIESTOPING    	0xE8			//=1正在严重故障停机过程 
#define	SL_ERRSTOPING       0xE9      		//=1正在一般故障停机过程
#define SL_NORSTOPING       0xEA            //=1正在正常正常停机过程
#define	SL_UDSTACOK       	0xEB      		//=1定子压差达到并网跫
#define SL_CBON_FH       	0xEC            //=1CBON步骤标志位
#define SL_SERSTOP_FH       0xED            //=1SERSTOP步骤标志位
#define SL_HIDCOV         	0xEE			//=1中间直流斩波IGBT硬件故障20120228atbjtu_chopper	BJTULVRT201204
#define SL_SYN_FH         	0xEF   			//SYN步骤标志位    		    


//------------------------------------const定义--------------------------------------------

//eeprom位操作
const  Uint16 ONEBYTE[8]={0x0080,0x0040,0x0020,0x0010,0x0008,0x0004,0x0002,0x0001};

//AD1544通道选择
const  Uint16 AD1544[4]={0x0000,0x2000,0x4000,0x6000};		
//--------------------------------------变量定义-------------------------------------------
Uint16	flag[16];													//标志位变量
//Uint16	disp[15];													//标志变量

//---2013-12-13--ZZJ增加电机励磁
//------------------------电机励磁参数识别MagnetCurve200909------------------------------------
struct	MC_DATA{
	float   x[11];
	float   y[11];
	float   temp[11];
	float   k;
	float   in;
}MC_DATA;
float   MC_y_test;
float   MC_U_test; 
float	Uns_ft;
Uint16	MC_N_test;
Uint32	MC_F_cnt;
//---2013-12-13--ZZJ增加电机励磁

Uint32	QEP_CNT1;
Uint32	QEP_CNT2;
int16	QEPPHE_N;


float   pll_theta; 				//201011PLL
float   ptheta;
float   ntheta; 
float temp_pd;
float temp_pq;
float temp_nd;
float temp_nq; 
float   NGS_Udq_lv;

float   NGS_Udq_p;
float   NGS_Udq_n;
float   NGS_Udq_n2p;
float   NGS_Udq_n2pex;
float   NGS_Udq_p_ex;		//20130206
float   NGS_Udq_n2p_lv;	    //20130301
float   NGS_Udq_pflt;		//20130301
float   NGS_Udq_p_longflt;	//20130301

float   LV_A1;				//201011LVRT
float   LV_A2;
float   LV_T1;
float   LV_T2;

float   gridfault_flag_flt1;		//2013-12-6 LVRT标志位延时计数器
float   gridfault_flag_flt2;		//2013-12-6 LVRT标志位延时计数器
float   gridfault_flag_flt3;		//2013-12-6 LVRT标志位延时计数器
float   gridfault_flag_flt4;		//2013-12-6 LVRT标志位延时计数器

float   urdc;		//
float   urqc;		//
float   urdc_steady;		//
float   urqc_steady;		//
float   urdc_dynamic;		//
float   urqc_dynamic;		//

//ModbusRS485
Uint16  length485;													//modbus
Uint16  longtx_num;													//modbus

float   RUN_mpridrf_exi;
Uint16  RUN_mpriq;
Uint16  _OUT3_TEST;
Uint16  _NQEPPCO;
Uint16  R_SCOUT;
float   Ud_lv;
float   temp_ud;

//float   NGS_Udq_p;
//float   NGS_Udq_n2p;
float   NGS_Udq;
float   PHAI_d;
float   PHAI_q;
float   PHAI_dq;

int32   ET_SUM1;
int32   ET_SUM2;

/*下面的用作SVPWM计算*/
Uint16 SW_NPR,SW_MPR; 
float   vdc;	 
float   m=0.0;
float   ua_m=0,ub_m=0,uc_m=0;  //调制波
float   u_alfa_NPR=0,u_beta_NPR=0;
float   u_alfa_MPR=0,u_beta_MPR=0;


//给定值寄存器
struct	GIVE{
	float   urf;					   //网侧中间直流电压给ㄖ?
	float   npriqrf;                   //网侧无功电流给定值(仅网侧无功运行时使用)
	float   toqrf;                      //机侧转矩给定值(有功)
	float   anglerf;                    //机侧功率因数角度给定值(无功) 角度
	float   lvwtiqrf;                   //低电压穿越期间机组总无功电流指令20121103
	float   hvwtiqrf;                   //低电压穿越期间机组总无功电流指令20121103	
	float   toqrfrun;                  //机侧转矩给定值(有功)201205LVRTatZB
	float   toqrfflt;                   //机侧转矩给定值(1s大滤波平均值)201205LVRTatZB
	float   npriqrf_cl;                   //网侧无功电流给定值(仅网侧无功运行时使用)201205LVRTatZB
	float   toqrf_lv;                      //机侧转矩给定值(有功)	20130128
}GIVE;
float OCS_Trf;                          //主控给过来的转矩指令

//给定积分相关寄存器
struct	RUN{									
	float   urf;						//电压给定积分值
	float   ustep;						//电压给定积分步长
	float   mpridrf;						//有功电流给定积分值
	float   mpridstep;						//有功电流给定积分步长
	float   npriqrf;						//无功电流给ɑ种?
 	float   npriqstep;						//无功电流给定积分步长
	float   mpriqrf;						//无功电流给ɑ种?
 	float   mpriqstep;						//无功电流给定积分步?
	float   toqrf;						//转矩给定积分值
	float   toqstep;					//转矩给定积分步长
	float   aglrf;						//功率因数角给定积
	float   aglstep;					//功率因数角给定积分步长

	float   staidrf;					//解列定子d轴电流指令
	float   staiqrf;					//解列定子q轴电流指令
	float   staidstep;					//解列定子d轴电流步长
	float   staiqstep;					//解列定子q轴电流步长  

	float   mpridrf_g;						
	float   mpriqrf_g;					
 	float   mpriqrf_g_exlv;					//20130221
    
    float   radianrf;
	float   mpridrf_exi;
	float   mpridrf_var;
}RUN; 
//坐标变换相关寄存器
struct	TRANS_DATA{
    float a;           	//a相
	float b;           	//b相
	float c;           	//c相
	float alfa;	       	//alfa轴
	float beta;	       	//beta轴
	float d;		   	//d轴
	float q;           	//q轴 
	float sintheta;    	//正弦值
	float costheta;	   	//余弦值
	float dflt;	   		//滤波后d值
	float qflt;	   		//滤波后q值
	float amp;			//幅值
	float ampflt;		//滤波后幅值
	float dflt2;	   	//小滤波后d值20091027atzy为了检测电网跌落
//--------20121103--------
	float dpll;	   		//201011PLL
	float qpll;	   		//201011PLL
	float palfa;	    
	float pbeta;	 
	float pd;		   	
	float pq;       
	float nalfa; 	
	float nbeta;
	float nd;		   
	float nq;
    float de_pd;
    float de_pq;
    float de_nd;
    float de_nq;
    float de_fltpd;
    float de_fltpq;
    float de_fltnd;
    float de_fltnq;
	float de_psintheta;
	float de_pcostheta;	 
	float de_nsintheta;
	float de_ncostheta;	 //201011PLL 
	float dbsf;		//BJTULVRT201204
	float qbsf;	
	float dnbsf;		//BJTULVRT201204
	float qnbsf;	
    float ab;          //ab BJTULVRT201204
	float bc;           //bc

};																 
struct	TRANS_DATA TRS_NPR_I;   //网侧变流器的电流（这里NPR作为后缀，不用grd，以区分电网电流还是网侧变流器电流）
struct	TRANS_DATA TRS_MPR_I;   //机侧变流器的电流
struct	TRANS_DATA TRS_MPR_U;   //机侧变流器算脉冲用
struct	TRANS_DATA TRS_NPR_U;   //网侧变流器算脉冲
struct	TRANS_DATA TRS_NGS_U;   //嗟缤缪?
struct	TRANS_DATA TRS_STA_I;   //定子电流
struct	TRANS_DATA TRS_STA_U;   //定子电压
struct	TRANS_DATA TRS_GSTA_U;  //目标定子电压
struct	TRANS_DATA TRS_PSTA_U;  //用于编码器位置检测的定子电压
struct	TRANS_DATA TRS_PMPR_I;  //用于编码器位置检测的转子电流   
struct	TRANS_DATA DIP_NPR_U;  //用于功率计算  
struct	TRANS_DATA DIP_NPR_I;  //用于功率计算 
struct	TRANS_DATA DIP_STA_U;  //用于功率计算 
struct	TRANS_DATA DIP_STA_I;  //用于功率计算 
struct	TRANS_DATA DIP_MPR_I;  //用于功率计算  
struct	TRANS_DATA TRS_GAC_U;  //用于201011PLL	
struct	TRANS_DATA TRS_SCR_I;   //机侧crowbar电流BJTULVRT201204

struct I_DATA_PLL				//201011PLL
{
   	float last;
   	float out0;
   	float out1;
	float out;
}I_DATA_PLL; 

//PI环相关寄存器
struct PI_DATA
{
	float reference;							//给定值
	float feedback;								//反馈值
	float errorp;								//拔差
	float error;								//误差
	float integrator;							//前次积纸峁?
	float out;
	float last;							//201011PLL
	float outflt;						//201011PLL 
};  
struct PI_DATA PI_NPR_Id;				//网侧变流器d轴电流闭环
struct PI_DATA PI_NPR_Iq;				//网侧变流器q轴电流闭环
struct PI_DATA PI_MPR_Id;				//机侧变髌鱠轴电流闭环
struct PI_DATA PI_MPR_Iq;				//机侧变流器q轴电流闭环
struct PI_DATA PI_NPR_U;				//网侧变流器电压环
struct PI_DATA PI_MPR_U;				//软并网闭环
struct PI_DATA PI_STA_Id;				//励磁控制d
struct PI_DATA PI_STA_Iq;				//励磁控制q 
struct PI_DATA PI_DATA_PLL;				//201011PLL
struct PI_PARA PI_PARA_DYMI;			//动态宽带宽PI控制BJTULVRT201204

struct PI_PARA
{
   float kp;
   float ki;
   float kd;
   float errmax;
   float errmin;
   float incrementmax;
   float outmax;
};
struct PI_PARA PI_PARA_NPRU;
struct PI_PARA PI_PARA_NPRID;
struct PI_PARA PI_PARA_MPRU;
struct PI_PARA PI_PARA_MPRID;
struct PI_PARA PI_PARA_NPRIQ;
struct PI_PARA PI_PARA_MPRIQ;
struct PI_PARA PI_PARA_DYNU;
struct PI_PARA PI_PARA_DYNID;
struct PI_PARA PI_PARA_DYNIQ;
struct PI_PARA PI_PARA_MPRI;
struct PI_PARA PI_PARA_STAI;
struct PI_PARA PI_PARA_PLL;				//201011PLL


//餮范ㄊ逼骷把邮奔剖?
struct	MAIN_LOOP{
	int32 cnt1;					//主循范ㄊ奔逆器5ms
	int32 cnt2;					//主循环定时寄存器10ms
	int32 cnt3;					//主循环定时寄存器20ms
	int32 cnt4;					//主循环ㄊ奔拇嫫?50ms
	int32 cnt5;					//餮范ㄊ奔拇嫫?100ms
	int32 cnt6;					//主循环定时寄存?1000ms
	int32 pulse;				//主循环分时脉冲寄存器
    int32 cnt_AMUX;            	//慢速AD
	
	int32 cnt_poweron;			//preparation time delay
	int32 cnt_pwmout;			//PWM输出延时
	int32 cnt_nprlamp;			//网侧脉冲发出指示灯闪烁延时计数
	int32 cnt_mprlamp;			//机侧脉冲发出指示灯闪烁延时计数 
	int32 cnt_freq;			   	//频率偏移计?
	int32 cnt_cbfreq;
	int32 cnt_estop;
	int32 cnt_rcvr;				//故障延时恢复计时
	int32 cnt_otser;			//严重故障延时
	int32 cnt_datasave;
	int32 cnt_datasave2;

	int32 cnt_detectdelay;		//开关意外动作切换延?	110818
	int32 cnt_exfault;
	int32 cnt_cbtp;
	int32 cnt_cberror;
	int32 cnt_mainkerror;
	int32 cnt_mainferror;
	int32 cnt_preerror;
	int32 cnt_stacerror;
	int32 cnt_pgovload;
	int32 cnt_qgovload;
	int32 cnt_canfault;
	int32 cnt_speedout;
	int32 cnt_uaclv1;   		//20091027atzy		
	int32 cnt_gfault1;			//嘟拥毓收?
	int32 cnt_gfault2;			//机侧接地故障
	int32 cnt_synfail;			//同步并网失故障
	int32 cnt_nprtlov;			//网侧电感超温
	int32 cnt_mprtlov;			//机侧电感超温
	int32 cnt_nskiptov;			//网侧SKIIP超温
	int32 cnt_mskiptov;			//机侧SKIIP超温
	int32 cnt_steadyfb;			

	int32 cnt_isteady0;
    int32 cnt_mprsteady0;
	int32 cnt_senszfstdy;
	int32 cnt_qcapspdin;
	int32 cnt_qepzdisturb;
	int32 cnt_qepcntok;
	int32 cnt_qcapdisturb;

	int32 cnt_ecposcheck;		//机侧启动延迟检测 20130801
	int32 cnt_ecpostrace;		//每次角度变化，延迟判断 20130801
//sysctl_zl_start
    int32 cnt_cbon;      		//主断闭合子程序
	int32 cnt_errstop;          //一般故障停机
	int32 cnt_serstop;          //严重故障停机
	int32 cnt_norstop;         	//正常停机
	int32 cnt_epwmstop;         //一般故障封脉冲后延时断主断20110829
	int32 cnt_synrun;          	//并网     
	int32 cnt_sysoff;         	//正常停机延迟，20120321
//sysctl_zl_end 
	int32 cnt_reset;
    int32 cnt_clostacmd;
	int32 cnt_nprcmd;
	int32 cnt_mprcmd;
	int32 cnt_ein;
	int32 cnt_sysrun;
	int32 cnt_prec;	

	//canopen
	int32 canopen_rx;
	int32 canopen_tx;
	int32 cnt_cbreset;

	int32 et_relay1;     
	int32 et_relay2;

	int32 cnt_ocsein1;     
	int32 cnt_ocsein2;
	int32 cnt_ocssysrun1;     
	int32 cnt_ocssysrun2;

//LVRT
	int32 cnt_scrizero;
	int32 cnt_lv_scron;
	int32 cnt_lv_mpwmstart;
	int32 cnt_lv_mpwmstop;
	int32 cnt_mpwmrestart;
	int32 cnt_npwmrestart;
	int32 cnt_lv_state;
	int32 cnt_lv_rcv;
	int32 cnt_hv_rcv;
	int32 cnt_lv_time;
	int32 cnt_hvlv_detect;
	int32 cnt_hvlv_detect3;//2016-1-13

	int32 cnt_scrondelay;
	int32 cnt_lv_fault1;
	int32 cnt_lv_fault2;
	int32 cnt_mc_ack;   		//MagnetCurve2013-12-13	
	
	int32 cnt_resetrn;
	int32 cnt_phorde;   		//201005atcpc	
	int32 cnt_encofault;   		//201005atcpc	
//------20121103atsp-------
	int32 cnt_gridok_last;
	int32 cnt_unbalance;		
	int32 cnt_qworking;	
	int32 cnt_scrizero2;
	int32 cnt_lv_phicontrol;	
	int32 cnt_lv_lowpi;	
	int32 cnt_lv_mpridvar;	
	int32 cnt_lv_utlv;
	int32 cnt_idcov;
	int32 cnt_freqrecv;			//201011PLL
	int32 cnt_lv_irqdelay;		//20121202
	int32 cnt_lv_start_scron;			//20121203
	int32 cnt_lv_start_scroff;			//20121203

//--------20130225atsp--------
	int32 cnt_lv_scroff1;		//20121210
	int32 cnt_lv_scroff2;		//20121210
	int32 cnt_lv_scroff3;		//20130221
	int32 cnt_lv_unblcscroff1;		//20121210
	int32 cnt_lv_unblcscroff2;		//20121210

	int32 cnt_lv_npriqrf;			//20130124
	int32 cnt_lv_mpridrf;			//20130130
	int32 cnt_hvlv_kq;				//20130124
	int32 cnt_lv_unblcscroff3;		//20130124
	int32 cnt_gridfault_last;			//20130124
	int32 cnt_lv_dlrecmstop;		//20130130
	int32 cnt_lv_xnzkkz;			//20130202
	int32 cnt_jgvdscron;			//20130205
	int32 cnt_lv_reciq;				//20130205
	int32 cnt_lv_holdpow;			//20130205
	int32 cnt_lv_scrkeepon;			//20130221
	int32 cnt_chopper;				//2013-12-6	ZZJ斩波测试
	int32 cnt_uacover;              //交流过压检测

}MAIN_LOOP;  

//慢速AD处理寄存器
Uint16 _OUT_AMUX1=0;  //用于慢速AD选通信号，每次要选通下一个AD通道，_OUT_AMUX加1，然后送到CPLD去处理，选通16选1芯片
Uint16 _OUT_AMUX2;    //用于慢速AD选通信号，每次要选通下一个AD通道，_OUT_AMUX加1，然后送到CPLD去处理，选通16选1芯片
struct  AMUX{
    float NPR_tempa;  //NPR的A相半导体温度，Skiip传回来的
    float NPR_tempb;  //NPR的B相半导体温度，Skiip传回来的
    float NPR_tempc;  //NPR的C相半导体温度，Skiip传回来的
    float MPR_tempa;  //MPR的A相半导体温度，Skiip传回来的
    float MPR_tempb;  //MPR的B相半导体温度，Skiip传回来的
    float MPR_tempc;  //MPR的C相半导体温度，Skiip传回来的
    float Lac_temp;   //交流测电感温度
	float Ldudt_temp; //du/dt的温度
	float skiiptempmax;    //SKIIP模块的温度最大值
	float NPR_skiiptemp;    //网侧SKIIP模块的温度最大值
	float MPR_skiiptemp;    //机侧SKIIP模块的温度最大值
	float Lac_R;   			//交流电感pt100
	float Ldudt_R; 			//du/dt的pt100

}AMUX;

//AD模块处理寄存器
struct	AD{
	int16  dat[18];													//AD当前转换结果存放
	Uint32 cputime;
	Uint32 cputime_last;
}AD;

//AD转换结果寄存器
struct	ADFINAL{
	int16  ia1;				//网侧A相电流
	int16  ib1;				//网侧B相电流
	int16  ic1;				//网侧B相电流
	int16  ia2;				//电机侧A相电流
	int16  ib2;				//电机侧B相电流
	int16  ic2;				//电机侧B相电流
	int16  isa;              //定子电流A相 
    int16  isc;              //定子电流C相
    int16  idc;              //chopper电流
    int16  freq;            //VVVF调试时用做电位器给定频率
	int16  uab1;			//网侧ab线压
	int16  ubc1;			//网侧bc线压
	int16  uab2;			//电机定子侧ab线压
	int16  ubc2;			//电机定子侧bc线压
	int16  uab3;			//电网电压ab（主断后）大滤波通道
	int16  ubc3;			//电网电压bc（主断后）大滤波通道
    int16  udc;             //直流电压
    int16  ubc;			    //主断前bc线压
	int16  uab;			    //主断前ab线压
    int16  AMUX;            //慢速AD信号



}ADFINAL;
   

//网压矢量
struct AD_DATA{
	  float  a;
	  float  b;
	  float  c;
	  float  ab;
	  float  bc;
	  float  ac;
	  float  cb;
	  float  ba;
	  float  a_temp;
	  float  b_temp;
	  float  c_temp;
	  float  ab_temp;
	  float  bc_temp;
	  float  a_dc;
	  float  b_dc;
	  float  c_dc;
	  float  ps;
};
struct AD_DATA AD_OUT_GRD_U;  //电网电压（主断前）
struct AD_DATA AD_OUT_STA_U;  //电机定子侧电压 大滤波通道
struct AD_DATA AD_OUT_NGS_U;  //电网电压（主断后）
struct AD_DATA AD_OUT_NPR_I;  //网侧变流器电流
struct AD_DATA AD_OUT_MPR_I;  //机侧变流器电流
struct AD_DATA AD_OUT_STA_I;  //定子侧电流  
struct AD_DATA AD_OUT_NGF_U;  //电网电压（主断后）大滤波通道
struct AD_DATA AD_OUT_STAD_U;  //差值
struct AD_DATA AD_OUT_SCR_I;  //ACTIVECROWBAR电流  
struct AD_DATA AD_OUT_DC_I;  //CHOPPER电流  

float  AD_OUT_UDC; //AD计算后输出的直流电梗╒）

//求平均值
struct MEAN_DATA{
       float ia1,ib1,ic1;  			//网侧电流平均值滤波
	   float ia2,ib2,ic2;  			//机侧电流平均值滤波
	   float ia3,ib3,ic3;  			//定子侧电流平均值滤波
	   float uab1,ubc1;	   			//网侧电压平均值滤波
	   float uab2,ubc2;	   			//定子侧电压平均值滤波
	   float uab3,ubc3;	   			//主断后电网电压,大滤波通道magnetCurve2013-12-13
	   float ua1,ub1,uc1;	   		//网侧电压平均值滤波
	   float uab_d,ubc_d;	   		//网侧与定子侧电压平均值差 大滤波通道
	   float zfia1,zfib1,zfic1; 	//网侧电流传感器零漂滤波
	   float zfia3,zfib3,zfic3; 	//定子侧电流传感器零漂滤波
	   float zfuab1,zfubc1; 		//网侧电压传感器零漂滤波
	   float zfuab2,zfubc2; 		//定子电压传感器零漂滤波
	   float zfuab3,zfubc3; 		//网侧电压 大滤波通道	零漂滤?
	   float nid,niq,nud,nuq;    	//对NPR量进行滤波
	   float uab,ubc;	   			//主断前网侧电压平均值滤波
	   float zfuab,zfubc; 		    //主断前电压传感器零漂滤波
	   float udc;
	   float iac3,iba3,icb3;				//定子侧电流平均值滤波
	   float psactive;				//定子侧电流平均值滤?
	   float Lac_temp,Ldudt_temp;   //电感温度滤波
	   float ubc_t;					//20130801
	   	   
}MEAN_DATA;  

//保护模块结果变量
struct	PRO{									
	         	float	NPR_iac;								//并网电流峰值,基准电流的百分值
				float	MPR_iac;								//定子电流峰值,基准电流的百分值
				float	udc;									//中间直流电压正实际值(V)
				float	NPR_uab;								//网侧 ab线电压实际值(V)
				float	NPR_ubc;								//网侧 bc线电压实际值(V)
				float	STA_uab;								//定子侧 ab线电压实际值(V)
				float	STA_ubc;								//定子侧 bc线电压实际值(V)
				float	GID_uab;								//电网侧 ab线电压实际值(V) 20091026atzy
				float	GID_ubc;								//电网侧 bc线电压实际值(V) 20091026atzy
				float	rcvr;									//延迟恢复时间
				float   STA_iac;                            	//定子交流电流iac
				float   STA_icb;                            	//定子交流电流iba
   				float   NPR_TLOV;                            	//网侧电感温度
				float   MPR_TLOV;                            	//机侧电感温度 
				float   speed;                               	//电机转速 r/min
				float   speedflt;                               //20090815
				float   sta_iar;	  							//定子侧a相电流A RMS
				float   sta_ibr;	  							//定子侧b相电流A RMS
				float   sta_icr;	  							//定子郼电流A RMS
				float   sta_uar;	  							//定子侧a相电压V RMS
				float   sta_ubr;	  							//定子侧b相电压V RMS
				float   sta_ucr;	 							//定子郼嗟缪筕 RMS
				float   npr_uar;	  							//网侧a相电压V RMS
				float   npr_ubr;	  							//网侧b相电压V RMS
				float   npr_ucr;	  							//网侧c相电压V RMS
				float   npr_iar;   								//网侧,A相并网电流有效值,单位A
				float   npr_ibr;	  							//网侧,B相并网电流行е?,单位A
				float   npr_icr;	  							//?,C相并缌饔行е?,单位A
				float   Psactive; 								//定子侧泄β?
				float   Psreactive; 							//定子侧无功功率
				float   Ps;          							//定子侧视在功率
				float   Pnactive;    							//网侧有功功率
				float   Pnreactive;  							//网侧无功功率
				float   Pn;          							//网侧视在功率
				float   Pgactive;    							//并网总的有功功率	
				float   Pgreactive;  							//网侧总霓功功率				
				float   Pg;       								//⑼艿氖釉诠β?
				float	CHOP_idc;								//chopper电流正实际值(A)20120228	BJTULVRT201204
				float   Psactive2; 								//定子侧有功功率
				float   Psreactive2; 							//ㄗ硬噢功功率
				float   Pnactive2;    							//网侧有功功率
				float   Pnreactive2;  							//网侧无功功率
				float   Pgactive2;    							//并网总的有功功率	
				float   Pgreactive2;  							//网侧总霓功功率				

				float   Pgactive_lv;   							//LVRT前的功率20121210	

		   }PRO; 
 
//显示嗫乇淞?
struct	DISP{										
    			float  npr_iai;   //网侧,A相并网电流瞬时值,单位A
				float  npr_ibi;	  //网侧,B相并网电流瞬时值,单位A
				float  npr_ici;	  //网?,C相并网缌魉彩敝?,ノ籄
				float  mpr_iai;	  //电机侧,A相电流瞬时值,单位A
				float  mpr_ibi;	  //电机侧,B相电流敝?,单位A
				float  mpr_ici;   //电机侧,C相电流时值,单位A
				float  npr_iar;   								//网侧,A相并网电流有效值,单位A
				float  npr_ibr;	  							//网侧,B相并网电餍效值,单位A
				float  npr_icr;	  							//网侧,C相并网电流有效值,单位A
				float  mpr_iar;	  //琥侧,A相电流有效值,单位A
				float  mpr_ibr;	  //电机侧,B相电流有效值,单位A
				float  mpr_icr;   //电机侧,C嗟缌饔行е?,ノ籄 
    			float  npr_uab;	  //网侧ab相电压V RMS
				float  npr_ubc;	  //网侧bc相电压V RMS

				float  sta_uab;	  //定子侧ab相电压V RMS
				float  sta_ubc;	  //定子侧bc相电压V RMS
				float  sta_iac;	  //定子侧线电流A RMS
				float  sta_iba;	  //定子侧线电流A RMS

				float  grd_uab;   //主断前网侧ab电压V RMS
				float  grd_ubc;   //主断前网侧bc电压V RMS
				float  uab23;     //定子同步并网前定子前后ab电压差 V
				float  ubc23;     //定子同步并网前定子前后bc电压差 V			
				float  Lac_temp;  //网侧绺形露? 摄氏度
				float  Ldudt_temp;  //机侧电感温度 摄氏度
				float  NPR_skiptemp;  //SKIIP温度 摄氏度
				float  MPR_skiptemp;  //SKIIP温度 摄氏度
				float  urf;
				float  udc;
				float  mpridrf;
				float  npriqrf;
				float  mpriqrf;
				float  toqrf;
				float  toqfb;
				float  aglrf;
				float  freq;
				float  speed;
				float  pionu;   	//网嗟缪够肥出
				float  pionid;		//网侧电流d环输出
				float  pioniq;		//网侧电流q环输出
				float  mexi;		//
				float  piomid; 		//机侧电流d环输出
				float  piomiq; 		//机侧电流q环输出
				float  Psactive; 	//定子侧有功功率
				float  Psreactive; 	//定子侧无功功?
				float  Ps;          //定子侧视在功率
				float  Pnactive;    //网侧有功功率
				float  Pnreactive;  //网侧无功功率
				float  Pn;          //网侧视在功率
				float  Pgactive;    //并网总的有功功率	
				float  Pgreactive;  //网侧总的无功功率				
				float  Pg;       	//并网总的视在功率			
			}DISP;
 
//痘模块变量
struct CAP_DATA	{
					float		prd;				//Cap4事件周期
					Uint32		adsmptsctr; 		//AD采样时刻的Cap4计数器值(本次AD在CAP4之后运行)
					float 		phase;	    		//事件当前相位//用于相序检查
					float 		freqtmp;			//敬问录德?
					float 		freq;	    		//确认后的频率
					float	    omigasyn;			//同步角频率－电频率
					float 		omigaslp;	    	//转差角频率
					float	    radpertb;			//计数器计单位时间矢量位置增量 rad per timebase
					float	    nprtrstheta;		//网侧算法正变换角度
					float	    npratitheta;		//网侧算法反变换角度
					float	    mprtrstheta;		//机嗨惴ㄕ浠唤嵌?
					float	    mpratitheta;		//机侧算法反变换角度
					float	    stavectheta;	    //定子磁链角度	
					float	    nprtrstheta_lv;		//网侧算法正变换角度 201011LVRT 201011PLL
				};
struct CAP_DATA CAP4,CAP5;

struct QEPDATA{
//----------------------转子位置检测变量-----------------------------------------------------------
					Uint16		qcapprd;		//QCAP单元周期寄存器值，用于测周法算转速
					float 		omigamec;		//机械角速度
					float 		omigarote;	    //电气角俣? rotor electric
					float 		omigaroteflt;	//
					float 		rotpos;			//转子当前位置rad（电弧度）
					Uint32   	adsmposcnt;		//采样时刻编码器读数  
					Uint32 		posilat;        //记录Z信号上升沿时CNT计数值
					Uint32 		qposlat;        //记录Unit time内QEPCNT的计数值，用于测频法算转速
					Uint32 		qposlat1;        //记录Unit time内QEPCNT的计数值，用于测频法算转速
					Uint32 		qposlat2;        //记录Unit time内QEPCNT的计数值，用于测频法算转速
					float 		rotposdisp;		//DA显示
					float 		rotradpret0;	//用于QEP Z信号抗干扰
//----------------------编码器位置检测变量--------------------------------------------------------	
					float 		encodpos;		//编码器位置rad（电弧度）
					float 		u_stapos;		//2S参考系下定子电压矢量的位置rad（电弧度）
					float 		i_rotpos;		//2S参考系下转子电流矢量的位置rad（电弧度）
			  }QEPDATA; 

//SCI模块变量
struct	SCI{
					Uint16 txb[2100];				//SCIB发送数据//modbus
					Uint16 rxb[SCI485NUM_RX];		//SCIB接收数据//modbus
					int32 cnt_sciover;				//SCI发送/接收超时定时器
					int32 cnt_scispace;				//SCI接收到发送的间隔定时器
		   }SCI;

//SCICANOPEN模块变量
struct	SCI_CANOPEN{
					Uint16 txb[SCICANOPENTXNUM];			//SCIB发送数据
					Uint16 rxb[SCICANOPENRXNUM];			//SCIB接收数据
					Uint16 heartbeat;
					int32  cnt_heartbeat;
					int32  cnt_sciover;
					Uint16 rx_controlword;
					Uint16 rx_torque;
					Uint16 rx_angle;
					Uint16 tx_torque;
					Uint16 tx_speed;
					Uint16 tx_state1;
					Uint16 tx_state2;	
					Uint16 tx_watertempin;
					Uint16 tx_watertempout;
					Uint16 tx_skiiptempmax;
					Uint16 tx_demand;
		   }SCI_canopen;

//SPI模块变量
struct	SPI{
					Uint16 da7614_txb[SPINUM];		//SPI,DA发送数据
					Uint16 da7614point;				//
		   }SPI;

//EEPROM模块变量
struct	EEPROM{
				Uint16 data[11];					//读出或写入的数据
				Uint16 point;						//EEPROM当前作变量序号
				Uint16 mcode;						//需要修改的功能码名称
			  }EEPROM;

//RTIMER模块变量
struct	RTIMER{
				Uint16 time[6];				//当前时间[5~0]-->年 月 日 时 分 秒（均为8位数据）
			  }RTIMER;

//磁链观测BJTULVRT201204
struct PHI_DATA
{  
   float PHId;							//d轴磁链
   float PHIq;							//q轴磁链
   float PHIsd;         				//定子磁链d轴正序分量
   float PHIsq;        				//定子磁链q轴正序分量  
   float PHIdflt;						//d轴电流补偿指令
   float PHIqflt;						//q轴电流补偿指令 
   float PHIdflt2;						//d轴电流补偿指令
   float PHIqflt2;						//q轴电流补偿指令 
   float PHIMd;							//d轴磁链 用于ims动态补偿
   float PHIMq;							//q轴磁链 用于ims动态补偿
   float PHIMdflt;						//d轴磁链 用于ims动态补偿
   float PHIMqflt;						//q轴磁链 用于ims动态补偿 
   float Ird;							//d轴转子电流
   float Irq;							//q轴转子电流
   float Isd;							//d轴定子电流
   float Isq;							//q轴定子电流
   float Isdflt;							//d轴定子电流
   float Isqflt;							//q轴定子电流
   float ISD;							//d轴定子电流
   float ISQ;							//q轴定子电流
   float Id;							//d轴电流补偿指令
   float Iq;							//q轴电流补偿指令
   float Id_max;						//d轴电流补偿指令限幅
   float Iq_max;						//q轴电流补偿指令限幅
   float Id_min;						//d轴电流补偿指令限幅
   float Iq_min;						//q轴电流补偿指令限幅
   float theta;							//磁链变换角度
   float k;								//励磁Lm校正系数
   float PHId0;							//d轴磁链稳态值 201203
   float PHIq0;							//q轴磁链稳态值
   float IMD;							//转子电流
   float IMQ;							//转子电流
   float Imd;							//d轴转子电流
   float Imq;							//q轴转子电流 
   float Imdflt;							//d轴转子电流
   float Imqflt;							//q轴转子电流 
};
struct PHI_DATA PHI_DATA_M; 

//201110bs
struct BS_DATA
{
	float in;					//in(k)
	float inp;					//in(k-1)
	float infp;					//in(k-2)
	float out;					//out(k)
	float outp;					//out(k-1)
	float outfp;				//out(k-2)
};  
struct BS_DATA BS_MPR_Id;				//转子侧电流d轴
struct BS_DATA BS_MPR_Iq;				//转子侧电流q轴
struct BS_DATA BS_STA_Id;				//定子侧电流d轴
struct BS_DATA BS_STA_Iq;				//定子侧电流q轴
struct BS_DATA BS_NPR_Ud;				//网侧变流器d轴
struct BS_DATA BS_NPR_Uq;				//网侧变流器d轴
struct BS_DATA BS_NPR_Udn;				//网侧变流器d轴
struct BS_DATA BS_NPR_Uqn;				//网侧变流器d轴

struct BS_PARA
{
   float f0;
   float fb;
   float outmax;
};
struct BS_PARA BS_PARA_MPRI;
struct BS_PARA BS_PARA_STAI;
struct BS_PARA BS_PARA_NPRU;




//-----------------------------内部变量----------------------------------------------------------
Uint16  _NULL_VAL=0;
Uint16  _MSG_SCOUT1;												//SCOUT信息级别(前次)
Uint16  _MSG_SCOUT2;												//SCOUT信息级别(本次)
Uint16  _SY_RTRN;
Uint32   POSCNT_TEST;
float   DM_imrd;   						//转子励磁电流
float   DM_imrq;   						//转子励磁电流
float   DM_Lr;							//当前重试次数
float   Te_feedback;                    //实际转矩值反馈
float   Pnactive;    					//网侧有功功率
float   Pnreactive;    					//网侧无功功率
float   Pcreactive;    					//网侧电容无功功率201205LVRTatZB
//----------------------------参数变量------------------------------------------------------------
Uint16 _MC_TYPE;						//机型参数
Uint16 _MC_REV;							//软件版本
Uint16 _MC_DATE;						//出厂日期
Uint16 _MC_ID;							//产品编号
Uint16 _SCIB_ADDR;						//485节点地址
Uint16 _PROSTDBY1;						//
Uint16 _PROSTDBY2;						//

Uint16 _SW_FR;							//开关频率(kHz)
Uint16 _DEADTIME;						//脉冲的最小导通时间(us)
Uint16 _COMPTIME;						//脉冲的最小导通时间(ms)
Uint16 _MINONTIME;						//脉冲的最小导通时间(us)

Uint16 _RU_UDCT;						//电压指令积分时间(ms)
Uint16 _RU_IDT;							//电流指令积分时间(ms)
Uint16 _RU_IQT;							//电流指令积分时间(ms)
Uint16 _RU_TOQT;						//转矩指令积分变(ms)
Uint16 _RU_AGLT;                        //功率因数角指令积分时间(ms)
Uint16 _URF;							//直流电压给定值
Uint16 _MIDRF;							//d轴电流给定值
Uint16 _NIQRF;							//q轴电流给定值
Uint16 _MIQRF;							//q轴电流给定值
Uint16 _TOQRF;							//转矩指令
Uint16 _AGLRF;							//机侧功率因数角
Uint16 _EIDCO;							//手动 机侧励磁电流调整系数
Uint16 _ENCODPOS;						//手动 编码器初始位置角度

Uint16 _STDBY1;							//备用1
Uint16 _STDBY2;							//备用2
Uint16 _STDBY3;							//备用3
Uint16 _STDBY4;							//备用4
Uint16 _STDBY5;							//备用5
Uint16 _STDBY6;							//备用6
Uint16 _STDBY7;							//备用7
//Uint16 _STDBY8;							//备用8
Uint16 _MC_OK;							//备用8	 MagnetCurve2013-12-13
Uint16 _STDBY9;							//备用9
Uint16 _STDBY10;						//备用10
Uint16 _STDBY11;						//备用11
Uint16 _STDBY12;						//备用12
Uint16 _STDBY13;						//备用13
Uint16 _STDBY14;						//备用14
float _eidco;
float _encodpos;
float _sc_freq1;
float _sc_freq2;

//----'数字量变量'PC------
Uint16 _COMMAND1;
Uint16 _COMMAND2;
Uint16 _PCEROTRIG;					    //20100308
Uint16 _DISPLAY;
Uint16 _STATE1;
Uint16 _STATE2;
Uint16 _INPUT;
Uint16 _OUTPUT;
Uint16 _LVRT;

float _stdby01;							//备用经过小数点处理后值
float _stdby02;							//备用经过小数点处理后值
float _stdby03;							//赣经过小数点处砗笾?
float _stdby04;							//备用∈愦砗笾?
float _stdby05;							//备用经过小数处理后值
float _stdby06;							//备用经过小数点处值
float _stdby07;							//备用经过小数点处理后值
float _stdby08;							//备用经过小数点处理后值
//------------------------运行模式变量-------------------------------------------------------------
Uint16 _ENSCR;							//

//-----------------------PI环参数变量--------------------------------------------------------------
Uint16	_NPR_U_Kp;		  		//网侧变流器电压闭环比例系数
Uint16	_NPR_U_Ki;		   		//网侧淞髌鞯缪贡栈坊窒凳?
Uint16	_NPR_U_Kd;		   		//网侧澉器电压闭肺⒎窒凳?
Uint16	_NPR_U_outmax;			//网侧变流器缪贡栈肥涑鱿薹?
Uint16	_NPR_U_errmax;		 	//网侧变流器电压闭环误差限幅
Uint16	_NPR_U_errmin;		   	//网侧变流鞯缪贡栈肺蟛铑兄?
Uint16	_NPR_U_incrementmax;	//网侧变流器电压闭环增量限幅

Uint16	_NPR_ID_Kp;				//网侧变流器电流闭环比例系数
Uint16	_NPR_ID_Ki;	   			//网侧变流器电流闭环积分系数
Uint16	_NPR_ID_Kd;	   			//网侧变流器电流闭环微分系数
Uint16	_NPR_ID_outmax;			//网侧变流器电流闭环输鱿薹?
Uint16	_NPR_ID_errmax;	 		//网侧变流器电流闭环误差限幅
Uint16	_NPR_ID_errmin;	  		//网侧变流器电流闭环误差钚≈?
Uint16	_NPR_ID_incrementmax; 	//网侧变流器电流闭环隽肯薹?

Uint16	_NPR_IQ_Kp;				//网侧变流器电流闭环比例系数
Uint16	_NPR_IQ_Ki;	   			//网侧变髌鞯缌鞅栈坊窒凳?
Uint16	_NPR_IQ_Kd;	   			//网啾淞髌鞯缌鞅栈肺⒎窒凳?
Uint16	_NPR_IQ_outmax;			//网侧变流器电流闭环输出限幅
Uint16	_NPR_IQ_errmax;	 		//网侧变流器电流闭环误差限幅
Uint16	_NPR_IQ_errmin;	  		//网啾淞髌鞯缌鞅环误差最小值
Uint16	_NPR_IQ_incrementmax; 	//网侧变流器电流闭环增量限幅

Uint16	_MPR_U_Kp;	   			//机侧变流器电压闭环比例系数
Uint16	_MPR_U_Ki;	  			//机侧变流器电压闭环积分系数
Uint16	_MPR_U_Kd;	  			//机侧变流器电压闭环微分系数
Uint16	_MPR_U_outmax;	 		//机侧变流器电压闭环输出限幅
Uint16	_MPR_U_errmax;	  		//机侧变流器电压闭环误差限?
Uint16	_MPR_U_errmin;		 	//机侧变流器电压闭环误差最小值
Uint16	_MPR_U_incrementmax; 	//机侧变流器电压闭环增量限幅

Uint16 _MPR_ID_Kp;				//机侧变流器电流闭环比例系数
Uint16 _MPR_ID_Ki;				//机侧变流器电流闭环积分系数
Uint16 _MPR_ID_Kd;				//机侧变流器电流闭环微分系数
Uint16 _MPR_ID_outmax;			//机侧变流器电流闭环输出限幅
Uint16 _MPR_ID_errmax;			//机侧淞髌鞯缌鞅栈肺蟛钕薹?
Uint16 _MPR_ID_errmin;			//机侧变流器电流闭环误差最小值
Uint16 _MPR_ID_incrementmax;		//机侧变流器电流闭环增量限幅

Uint16 _MPR_IQ_Kp;				//机侧变流器电流闭环比例系数
Uint16 _MPR_IQ_Ki;				//机侧变流器电流闭环积分系数
Uint16 _MPR_IQ_Kd;				//机侧变流器电流闭环微分系数
Uint16 _MPR_IQ_outmax;			//机侧变流器电流闭环输出限幅
Uint16 _MPR_IQ_errmax;			//机侧变流器电流闭环误差限幅
Uint16 _MPR_IQ_errmin;			//机侧变流器电流闭环误差最小值
Uint16 _MPR_IQ_incrementmax;	//机侧变流器电流闭环增量限幅


Uint16	_DYN_U_Kp;	   			//机侧变流器电压闭环比例系数
Uint16	_DYN_U_Ki;	  			//机侧变流器电压闭环积分系数
Uint16	_DYN_U_Kd;	  			//机侧变流器电压闭环微分系数
Uint16	_DYN_U_outmax;	 		//机侧变流器电压闭环输出限幅
Uint16	_DYN_U_errmax;	  		//机侧变流器电贡环误差限幅
Uint16	_DYN_U_errmin;		 	//机侧变流器电压闭环误差最小值
Uint16	_DYN_U_incrementmax; 	//机侧变流器电贡栈吩隽肯薹?


Uint16 _SC_RTRN;													//故障最大重试次数
Uint16 _SC_RTRT;													//两次故障重试间隔时间(ms)

Uint16 _SC_IACOV1;													//基准电流的百分值)
Uint16 _SC_UDCOV;													//中间直流电压过压值(V)
Uint16 _SC_UDCOVH;													//中间直流电压过压值（网压跌落）(V)
Uint16 _SC_IDCOV;													//改为dc-chopper电流保护BJTULVRT201204
//Uint16 _SC_UDCLV;													//中渲绷鞯缪骨费怪?(V)
Uint16 _SC_VDCON;													//上电电压
Uint16 _SC_UACOV1;													//网压交流电压过压动作值(V)
Uint16 _SC_UACLV1;													//交流电压缺相动作值(V)
Uint16 _SC_IACOV2;													//基准电流的百分值)
Uint16 _SC_PGOV;													//
Uint16 _SC_QGOV;													//
Uint16 _SC_IACOVST;                                                 //定子电流过流保护 

Uint16 _SC_Rs;														//定子电阻(mohm)
Uint16 _SC_Lls;														//定子漏抗(mH)
Uint16 _SC_Rr;														//转子电阻(mohm)
Uint16 _SC_Llr;														//转子漏抗(mH)
Uint16 _SC_Lm;														//励磁电?(mH)
Uint16 _SC_POLES;													//极对数
Uint16 _SC_Usn;														//定子额定电压(V)
Uint16 _SC_Uro;                                                		//转子开路电压(V)	
Uint16 _SC_PLSPRVL;													//编码器每圈光栅数量
Uint16 _SC_MSTDBY;                                               	//电机参数备用
Uint16 _SC_EXISTY1;                                               	//电机励磁调节备用1
Uint16 _SC_EXISTY2;                                               	//电机励磁调节备用2
Uint16 _SC_EXISTY3;                                               	//电机励磁调节备用3
Uint16 _SC_EXISTY4;                                               	//电机励磁调节备用4
Uint16 _SC_EXISTY5;                                               	//电机励磁调节备用5

Uint16 _BA_ERR4;													//第四次故障信息
Uint16 _BA_ERR3;													//第三次故障信息
Uint16 _BA_ERR2;													//第二次故障信息
Uint16 _BA_ERR1;													//第一次故障信息
Uint16 _BA_ERR5;													//示波器故障信息
//--------------------------------报警信息变?-----------------------------------------------------
Uint16 _BA_EMIDRF;													//最新故障时刻 d轴电流指令(A)
Uint16 _BA_ENIQRF;													//最新故障时刻 q轴电流指令(A)
Uint16 _BA_EMIQRF;													//最鹿障时刻 q轴电流指令(A)
Uint16 _BA_ETOQRF;													//最新故障时刻 转矩指令(NM)
Uint16 _BA_EURF;													//最新故障时刻 中间直流电压给?(V)
Uint16 _BA_EUDC;													//最新故障时刻 中间直流电压(V)
Uint16 _BA_EAGLRF;													//最新故障时刻 无功角度指令()
Uint16 _BA_EIA1;													//最新故障时刻 网侧A相电流有效值(A)
Uint16 _BA_EIB1;													//最新故障时刻 网侧B相电流有效值(A)
Uint16 _BA_EIC1;													//最新故障时刻 网侧C相电流有效值(A)
Uint16 _BA_EIA2;													//最新故障时刻 缁郃相电流有效值(A)
Uint16 _BA_EIB2;													//最新故障时刻 电机侧B相电流有效值(A)
Uint16 _BA_EIC2;													//最新故障时刻 电机侧C相电流有效值(A)
Uint16 _BA_EUAB1;													//最新故障时刻 网侧A相电压有效值(V)
Uint16 _BA_EUBC1;													//最新故障时刻 网侧B相电压有效值(V)
Uint16 _BA_EUAB2;													//最新故障时刻 网侧相电压有效值(V)
Uint16 _BA_EUBC2;													//最新故障时刻 电机侧A相电流有效值(V)
Uint16 _BA_EUAB0;													//最新故障时刻 主断前AB线电压有效值(V)
Uint16 _BA_EUBC0;													//最新故障时刻 主锨癇C线电压有效值(V)
Uint16 _BA_EFREQ;													//最新故障时刻 电网频率(hz)
Uint16 _BA_ESPEED;													//最新故障时刻 电机转速(rpm)
Uint16 _BA_ENPRUD;              			//d轴主断后网压反馈20091026atzy
Uint16 _BA_ENPRUQ;          				//q轴主断后网压反馈20091026atzy
Uint16 _BA_ENPRUD2;          				//d轴主断后网压反馈 滤波后20091026atzy
Uint16 _BA_ENPRUQ2;          				//q轴主断后网压反馈 滤波后20091026atzy
Uint16 _BA_ENUDOUT;          				//郿岬缪故涑?20091026atzy
Uint16 _BA_ENUQOUT;          				//网侧q轴电压输出20091026atzy
Uint16 _BA_ETA1;													//最新故障时刻 网侧变流器A桥臂温度
Uint16 _BA_ETB1;													//最新故障时刻 网侧变流器B桥臂温度
Uint16 _BA_ETC1;													//最新故障时刻 网侧变流器C桥臂温度
Uint16 _BA_ETA2;													//最新故障时刻 电机侧变流器A疟畚露?
Uint16 _BA_ETB2;													//最新故障时刻 电机侧淞髌鰾桥臂温?
Uint16 _BA_ETC2;													//最鹿收鲜笨? 电机侧变流器C桥臂温度
Uint16 _BA_EPIONU;													//最新故障时刻 网侧电压环输出
Uint16 _BA_EPIONID;													//最新故障时刻 网侧d轴电流环输出
Uint16 _BA_EPIONIQ;													//最新故障时刻 网侧q轴电流环输出
Uint16 _BA_EMEXI;													//最新故障时刻 机侧励磁理论计算值显示
Uint16 _BA_EPIOMID;													//最新故障时刻 机侧d轴电流环输?
Uint16 _BA_EPIOMIQ;													//最新故障时刻 机侧q轴电流环输出
Uint16 _BA_ETLAC;													//最新故障时刻 网侧电感温度
Uint16 _BA_ETLDUDT;													//最新故障时刻 机侧电感温度
Uint16 _BA_ETSKIIP;													//最新故障时刻 SKIIP温?
Uint16 _BA_ESTAIAC;	
Uint16 _BA_ESTAIBA;	
Uint16 _BA_ETOQFB;	
Uint16 _BA_EPSTA;	
Uint16 _BA_EPNPR;	
Uint16 _BA_EPGRID;	

Uint16 _BA_TIME4_0;													//最新故障时间(秒)
Uint16 _BA_TIME4_1;													//最新故障时间(?)
Uint16 _BA_TIME4_2;													//最新故障时间(时)
Uint16 _BA_TIME4_3;													//最新故障时间(日)
Uint16 _BA_TIME4_4;													//最新故鲜奔?(月)
Uint16 _BA_TIME4_5;													//最新故障时间(年)

Uint16 _BA_TIME3_0;													//第三次故障时间(秒)
Uint16 _BA_TIME3_1;													//第三次故障时间(分)
Uint16 _BA_TIME3_2;													//第三次故障时间(时)
Uint16 _BA_TIME3_3;													//第三次故障时间(日)
Uint16 _BA_TIME3_4;													//第三次故障时间(月)
Uint16 _BA_TIME3_5;													//第三次故鲜奔?(年)

Uint16 _BA_TIME2_0;													//第二问障时间(秒)
Uint16 _BA_TIME2_1;													//第二次故障时间(分)
Uint16 _BA_TIME2_2;													//第二次故障奔?时)
Uint16 _BA_TIME2_3;													//第二次故障时间(日)
Uint16 _BA_TIME2_4;													//第二次故障时间(月)
Uint16 _BA_TIME2_5;													//第二次故障时?年)

Uint16 _BA_TIME1_0;													//第一次故障时间(秒)
Uint16 _BA_TIME1_1;													//第一次故障时间(分)
Uint16 _BA_TIME1_2;													//第一次故障时间(时)
Uint16 _BA_TIME1_3;													//第一次故障时间(日)
Uint16 _BA_TIME1_4;													//第一次故障时间(月)
Uint16 _BA_TIME1_5;													//第一次故障时间(年)

Uint16 _BA_TIME5_0;													//示波器故障时间(秒)
Uint16 _BA_TIME5_1;													//示波器故障时间(分)
Uint16 _BA_TIME5_2;													//示波器故障时间(时)
Uint16 _BA_TIME5_3;													//示波器故障时间(日)
Uint16 _BA_TIME5_4;													//示波器故障时间(月)
Uint16 _BA_TIME5_5;													//示波器故障时间(年)

//-------------------------------------运行监控变量-----------------------------------------------
Uint16 _BA_MIDRF;													//d轴电流指令(A)
Uint16 _BA_NIQRF;													//q轴电流指令(A)
Uint16 _BA_MIQRF;													//q轴电流指令(A)
Uint16 _BA_TOQRF;													//转矩指令(NM)
Uint16 _BA_AGLRF;													//无功角度指令(NM)
Uint16 _BA_TOQFB;													//转矩反馈(NM)
Uint16 _BA_URF;														//中间直流电压给定(V)
Uint16 _BA_UDC;														//中间直流电压(V)
Uint16 _BA_IA1;														//网郃相电流有效值(A)
Uint16 _BA_IB1;														//网侧B相电流有е?A)
Uint16 _BA_IC1;														//网侧C相电流有效值(A)
Uint16 _BA_IA2;														//电机侧A相电流有效值(A)
Uint16 _BA_IB2;														//电机侧B相电饔行е?(A)
Uint16 _BA_IC2;														//电机侧C相电流有效值(A)
Uint16 _BA_UAB1;													//网侧AB线电压有效值(V)
Uint16 _BA_UBC1;													//网侧BC线电压行е?(V)
Uint16 _BA_UAB2;													//电机侧AB线电压有效值(V)
Uint16 _BA_UBC2;													//电机侧BC线电流有效值(V)
Uint16 _BA_UAB0;												    	//主断前AB线电压有效值(V)
Uint16 _BA_UBC0;												    	//主断前BC线电流有效值(V) 
Uint16 _BA_FREQ;													//电网频?(hz)
Uint16 _BA_SPEED;													//电机转速(rpm)
Uint16 _BA_TA1;														//网侧变流器A桥畚露
Uint16 _BA_TB1;														//网侧变流鰾桥臂温度
Uint16 _BA_TC1;														//网侧变流器C桥臂温度
Uint16 _BA_TA2;														//电机侧变流器A桥臂温度
Uint16 _BA_TB2;														//电机侧变流器B桥臂温度
Uint16 _BA_TC2;														//电机侧变流器C桥臂温度
Uint16 _BA_PIONU;													//网侧电压环输出
Uint16 _BA_PIONID;													//网侧d轴电流环输出
Uint16 _BA_PIONIQ;													//网侧q轴电流环输出
Uint16 _BA_MEXI;													//
Uint16 _BA_PIOMID;													//机侧d轴电流环输出
Uint16 _BA_PIOMIQ;													//机侧q轴电流环输出
Uint16 _BA_GRDUD;													//传感器测量网压d
Uint16 _BA_GRDUQ;													//传感器测量网压q

//Uint16 _BA_STAUD;													//定子电压d MagnetCurve2013-12-13
Uint16 _BA_MC_IN;													//励磁校正输入 MagnetCurve2013-12-13
Uint16 _BA_MC_K;													//励磁校正输出 MagnetCurve2013-12-13
//Uint16 _BA_STAUQ;													//定子电压q MagnetCurve2013-12-13

Uint16 _BA_NPRID;													//传感器测量网压d
Uint16 _BA_NPRIQ;													//传感器测量网压q
//Uint16 _BA_EXCID;													//传感器测量网压d
//Uint16 _BA_EXCIQ;
Uint16 _BA_SCRIA;													//SCR电流a	20110906
Uint16 _BA_SCRIB;													//SCR电流b	20110906
Uint16 _BA_TLAC;
Uint16 _BA_TLDUDT;
Uint16 _BA_TNSKIIP;
Uint16 _BA_TMSKIIP;
Uint16 _BA_STAUABD;													//定子同步⑼岸ㄗ忧昂蟮缪共?
Uint16 _BA_STAUBCD;													//定子同步并网前定子前后电压差
Uint16 _BA_STAIAC;													//定子线电流有效值显示
Uint16 _BA_STAIBA;													//定子线电流有效值显示
Uint16 _BA_PSTA;
Uint16 _BA_PNPR;
Uint16 _BA_PGRID;
Uint16 _BA_QSTA;
Uint16 _BA_QNPR;
Uint16 _BA_QGRID;
Uint16 _BA_SSTA;
Uint16 _BA_SNPR;
Uint16 _BA_SGRID;

Uint16 _SC_NPR_TLOV;
Uint16 _SC_MPR_TLOV;
Uint16 _SC_LVUDC1;
Uint16 _SC_LVUDC2;
Uint16 _SC_LVIAC2;
Uint16 _SC_TSCRON;
Uint16 _SC_MPR_UD;
Uint16 _SC_MPR_UQ;
Uint16 _SC_MSPEED1;
Uint16 _SC_MSPEED2;
Uint16 _SC_UDSTAC;
Uint16 _SCIB_BAUD;													//485波特率
Uint16 _SC_SKPTOV;
Uint16 _SC_FREQ1;
Uint16 _SC_FREQ2;
 
//-----------------------测试用变量-----------------------------------------------------------
int16 	zys[4];			//DA变量
Uint16 	daindex[4]; 	//BJTULVRT201204


//int32 	zys = 0;
//int16 	zys1,zys2,zys3,zys4;
Uint32 	i_cnt1=0;  //用于CCS波形显示
Uint32 	i_cnt2=0;  //用于CCS波形显示
Uint32 	i_cnt3=0;  //用于CCS波形显示
float   draw1[150]={0.0},draw2[150]={0.0},draw3[150]={0.0},draw4[150]={0.0},draw5[150]={0.0};
float   draw6[150]={0.0};//,draw7[100]={0.0},draw8[100]={0.0},draw9[100]={0.0},draw10[100]={0.0};
Uint16 zsector = 0;
float testtheta;

Uint16 temp_flag3;		//120507
Uint16 TX_SPEED;		//120507

Uint16	state_n=0;		//110818
Uint16	state_m=0;		//110818
Uint16	action1=0;		//110818
Uint16	action2=0;		//110818

//----------20121103新添加变量--------------
float   GRD_Ut;
float   GRD_Utlv;		//201205LVRTatZB
float   kq;
Uint16	CNT_SCRON;		//20120602
float   Te_instant;
float   Te_instant2;
float   Te_instant3;

//---------20121210----------
float   lv_ctrl1=0;
float   lv_ctrl2=0;
float   lv_ctrl3=0;
float   lv_ctrl4=0;
float   lv_ctrl5=0;
float   lv_ctrl6=0;

//----20130225atsp----
float   kq_lv;
float   NGS_Udq_p_lv;
float   Te_feedback_lv;                    //实际转矩值反馈
Uint16	scrdone=0;		//20130222
Uint16	scrhold=0;		//20130223
Uint16	scroff1=0;		//20130303
Uint16	cnt_lv_clrerram=0;		//20130303

//--------------------
//---------20130801---------
Uint16	ecpostep1 = 0;
Uint16	ecpostep2 = 0;
Uint16	ecpostep3 = 0;

#ifdef __cplusplus
}
#endif /* extern "C" */


#endif

//===========================================================================
// No more.
//===========================================================================
