//-------------右玉1.5MW双馈风电机组并网变流器控制软件主程序----201005atcpc-201007atbjtu--------------------------------
#ifndef DCDC_database_H
#define DCDC_database_H

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------//参数属性定义
#define	WR0		1						//write only
#define	RDO		2						//read only
#define WR1		4						//write enable when stop
#define	WR2		8						//write enable with password
#define	WR3		16						//write on RAM without eerom
#define	MAX_PT	32						//最大值为指针
#define	MIN_PT	64						//最小值为指针
#define	SIG		128						//有符号数
#define	VA0		256						//表示包含数值0(相当于禁止相应的功能)
#define	UNS		512						//禁止存储
#define	KEY		1024						//查看和修改需要制造商密码
#define	DOT1	2048					//操作器带1位小数点
#define	DOT2	4096					//操作器带2位小数点
#define	DOT3	8192					//操作器带3位小数点

//--------------------------------------//显示信息属性定义
#define	NONE	0						//无
#define	SOFT	1						//软停止
#define	SHUT	2						//急停
#define	N_RCVR	4						//不能恢复
#define	D_RCVR	8						//延时恢复
#define	I_RCVR	16						//立即恢复
#define	CNT		32						//计入故障次数
#define	OT_SER	64						//超时1min后变成严重故障
#define	SAVE	128						//信息需要保存
#define	OFFCB	256						//断主断
#define	WARNING	512						//报警

//--------------------------宏定义-----------------------------------------------
				
//-----------------------参数相关宏定义------------------------------------------
//功能码相关数量定义(增减功能码后只需要增减数据库内的数据和此处的相应数量定义即可)
#define NUM_GROUP1			29				//首组功能码的个数（'制造商用'）
#define	NUM_GROUP2			42				//第二组功能码的个数（'PI参数'）
#define NUM_GROUP3			43				//第三组功能码的个数（'保护阈值'）201005atcpc
#define NUM_GROUP4			14				//第四组功能码的个数（'运行模式')
#define NUM_TIME			6				//时间设定功能码的个数
#define NUM_WATCHLCD		53				//运行监控功能码的个数
#define NUM_ERRINF			42				//报警信息功能码的个数
#define NUM_ERROR			35				//历史最新故障信息

//操作器功能码数量
#define PARA_NUM			NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4+NUM_TIME+NUM_WATCHLCD+NUM_ERRINF+NUM_ERROR

//故障参数总数量
#define ERRO_NUM			NUM_ERROR+NUM_ERRINF

//故障参数表第一个参数的序号		
#define TAB_ERR_FIRST		NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4+NUM_TIME+NUM_WATCHLCD

//故障参数表最后参数的序号		
#define TAB_ERR_END			NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4+NUM_TIME+NUM_WATCHLCD+NUM_ERRINF+NUM_ERROR-1

//监控参数表第一个参数的序号(包括时间参数)
#define BANK_FIRST			NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4+NUM_TIME				

//监控参数表最后一个参数的序号
#define BANK_END			NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4+NUM_TIME+NUM_WATCHLCD-1				

//时间参数表第一个参数的序号
#define TIME_FIRST			NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4				

//时间参数表最后一个参数的序号
#define TIME_END			NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4+NUM_TIME-1	

//数字写指令参数表第一个参数的序号		
#define TAB_COMSTATE_FIRST		NUM_GROUP1+NUM_GROUP2+NUM_GROUP3

//数字写指令参数表最后参数的序号		
#define TAB_COMSTATE_END			NUM_GROUP1+NUM_GROUP2+NUM_GROUP3+NUM_GROUP4-1


//运行给定的位置//
#define NO_URF				NUM_GROUP1-16	
#define NO_MIDRF			NUM_GROUP1-15
#define NO_NIQRF			NUM_GROUP1-14
#define NO_MIQRF			NUM_GROUP1-13
#define NO_TOQRF            NUM_GROUP1-12
#define NO_AGLRF			NUM_GROUP1-11

//--------------------------------------//定义参数型结构体
struct PARA
{
 Uint16 *para_add;						//变量地址指针
 Uint16 init;							//初始值
 Uint16	min,max;						//最小值 最大值
 Uint16	*min_add,*max_add;				//最小值 最大值指针
 Uint16 attr;							//属性
};

struct MESSAGE
{
 Uint16	name[3];						//名称
 Uint16 rank;							//信息级别
 Uint16 attr;							//属性
};

//---------------------------故障信息序号-systest----------------------------------------------
#define MSG_NUM				45				//故障信息数量

#define	MSG_NONE			0				//空信息
#define	MSG_CODEOK			1				//EEPROM正常
#define	MSG_EE_FAIL			2				//EEPROM故障
#define	MSG_ESTOP			3				//急停故障
#define	MSG_CBTRIP			4				//主断脱扣故障
#define	MSG_EXFAULT 		5				//变流器外部硬件故障
#define	MSG_CAN_FAIL		6				//CAN通讯故障
#define	MSG_ENCODFAULT		7				//编码器故障
#define	MSG_CBERROR 		8				//主断合闸故障
#define	MSG_PREERROR 		9				//预充电故障
#define	MSG_MAINKERROR 		10				//主接触器闭合故障
#define	MSG_MAINFERROR 		11				//主滤波器闭合故障
#define	MSG_STACERROR 		12				//定子接触器闭合故障
#define	MSG_HIA1			13				//网侧,A相硬件过流
#define	MSG_HIB1			14				//网侧,B相硬件过流
#define	MSG_HIC1			15				//网侧,C相硬件过流
#define	MSG_HIA2			16				//电机侧,A相硬件过流
#define	MSG_HIB2			17  			//电机侧,B相硬件过流
#define	MSG_HIC2			18				//电机侧,C相硬件过流
#define	MSG_UDCOV           19	            //硬件检测中间直流电压过压
#define	MSG_HIAC1			20				//网侧硬件过流
#define	MSG_HIAC2			21				//电机侧硬件过流
#define MSG_PDPASERIES      22            	//网侧严重故障
#define MSG_PDPBSERIES      23            	//电机侧严重故障
#define	MSG_MSPEEDOUT 		24				//转速超出范围故障
#define	MSG_SIAC1			25				//网侧软件过流
#define	MSG_SIAC2			26				//电机侧软件过流
#define	MSG_FE1				27				//电网频率错误
#define	MSG_SUDCOV			28				//软件中间直流电压过压
#define	MSG_SUACOV1			29				//软件网压交流过压
#define	MSG_SIDCOV			30				//改为dc-chopper电流保护BJTULVRT201204
//#define	MSG_SUDCLV			30				//软件中间直流电压欠压
#define	MSG_SUACLV1			31				//软件网压交流欠压或失电
#define	MSG_UDCWAVE			32				//中间电压波动故障
#define	MSG_SIOVST	    	33				//定子电流过流故障
#define	MSG_GFAULT1			34				//网侧接地故障
#define	MSG_GFAULT2		   	35				//机侧接地故障
#define	MSG_TOV1			36				//网侧SKIIP超温
#define	MSG_TOV2			37  			//电机侧SKIIP超温
#define	MSG_SKTOV			38  			//软件判断SKIIP超温
#define	MSG_TLOV1			39				//网侧电感超温
#define MSG_TLOV2           40            	//机侧电感超温
#define	MSG_PGOV			41				//并网有功超限
#define MSG_QGOV            42            	//并网无功超限
#define	MSG_STASYNFAIL 		43				//定子同步并网失败故障
#define	MSG_PHORDE			44				//电网相序错误

//--------------------------------------//数据总表
const struct PARA FUNC[PARA_NUM]={
//		名称   		初值		最小值		最大值		最小值指针	最大值指针	属性		说明
//-------------------------------'制造商用'-------------------------------------------------------
//--29/0-28
	{&_MC_TYPE,		MC_TYPE,	MC_TYPE,	MC_TYPE,	&_NULL_VAL,	&_NULL_VAL,	RDO},		//机型参 
	{&_MC_REV, 		MC_REV,		MC_REV,		MC_REV,		&_NULL_VAL,	&_NULL_VAL,	KEY},		//软件版本号
	{&_MC_DATE,		MC_DATE,	0,	    	65535,		&_NULL_VAL,	&_NULL_VAL,	KEY},		//出厂日期 
	{&_MC_ID,		MC_ID,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	KEY},		//产品编号 
	{&_SW_FR,		210,		200,		1800,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT2},	//开关频率(0.01kHz)
	
	{&_DEADTIME,	8,			0,			20,			&_NULL_VAL,	&_NULL_VAL,	WR1},		//死区时间(us)
	{&_COMPTIME,	45,			0,			50,			&_NULL_VAL,	&_NULL_VAL,	WR1+DOT1},	//死区补偿时间(ms)
	{&_MINONTIME,	4,			0,			20,			&_NULL_VAL,	&_NULL_VAL,	WR1},		//最小导通时间(us)
	{&_RU_UDCT,		10000,		1,			20000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//电压指令给定积分上升/下降时间(ms)
	{&_RU_IDT,		2,			1,			20000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//电流指令给定积分上升/下降时间(ms)
	
	{&_RU_IQT,		10,			1,			20000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//电流指令给定积分上升/下降时间(ms)
	{&_RU_TOQT,		500,		1,			20000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//转矩指令给定积分上升/下降时间(ms)
	{&_RU_AGLT,		500,		1,			20000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//功率因数角指令给定积分上升/下降时间(ms)
	{&_URF,			1100,		0,			1200,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//中间直流电压给定值
	{&_MIDRF,		0,			0,		    550,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//机侧无功电流给定值
	
	{&_NIQRF,		65,			-200,		200,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//并网无功电流给定值 20140503LJD电能质量
	{&_MIQRF,		0,			0,			700,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//机网有功电流给定值
    {&_TOQRF,		0,			0,			9000,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},   //机侧转矩给定值
    {&_AGLRF,		0,			-36,		36,	   		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},   //机侧功率因数角给定值
	{&_EIDCO,		1000,		0,			10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//机侧励磁?

	{&_ENCODPOS,	4569,		0,			6283,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//肫鞒跏嘉恢媒嵌? 0--2PIE 201005atcpc 20140503LJD电能质量	
	{&_STDBY1,		5000,  	    0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//备用1
	{&_STDBY2,		12,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	//备用2 20140503LJD电能质量 20150204XD
	{&_STDBY3,		5500,	    0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//备用3
	{&_STDBY4,		5,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//备用4

	{&_STDBY5,		10,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//备用5
	{&_STDBY6,		604,		-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//备用6
	{&_STDBY7,		10,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//备用7
//	{&_STDBY8,		0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//备用8
	{&_MC_OK,		1,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	WR0+SIG},	//备用8励磁完成标志MagnetCurve2013-12-13--ZZJ
	
//-------------------------------'PI参数'---------------------------------------------------------
//--42/29-70
	{&_NPR_U_Kp,			40,		1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	//网侧电压环比例系数
	{&_NPR_U_Ki,			1000,	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1+VA0},//网侧电压环积分系数
	{&_NPR_U_Kd,			0,		0,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3+VA0},//网侧电压环积分系数
	{&_NPR_U_outmax,		600,	1,		1000,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//网侧电压闭环输出限幅
	{&_NPR_U_errmax,		1000,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//网侧电压闭环误差上限限幅
	{&_NPR_U_errmin,		0,		0,		2000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//网侧电压环下限误差
	{&_NPR_U_incrementmax,  1000,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//网侧电压环增量限幅

	{&_NPR_ID_Kp,			7000, 	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	//网侧电流闭环比例系数
	{&_NPR_ID_Ki,			1000,	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1+VA0},//网侧电流闭环积分系数
	{&_NPR_ID_Kd,			0,		0,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3+VA0},//网侧电流闭环积分系数
	{&_NPR_ID_outmax,		10,		1,		500,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//网侧电流闭环输出限幅
	{&_NPR_ID_errmax,		6200,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//网侧电流闭环误差上限限幅
	{&_NPR_ID_errmin,		1200,	0,		2000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//网侧电流闭环误差下限限幅
	{&_NPR_ID_incrementmax,	4000, 	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//网侧电流闭环增量限幅

	{&_NPR_IQ_Kp,			40, 	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	//网侧电流闭环比例系数 201005atcpc
	{&_NPR_IQ_Ki,			1670,	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1+VA0},//网侧电流闭环积分系数
	{&_NPR_IQ_Kd,			0,		0,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3+VA0},//网侧电流闭环积分系数
	{&_NPR_IQ_outmax,		200,	1,		500,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//网侧电流闭环输出限幅
	{&_NPR_IQ_errmax,		500,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//网侧电流闭环误差上限限幅
	{&_NPR_IQ_errmin,		0,		0,		2000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//网侧电流闭环误差下限限幅
	{&_NPR_IQ_incrementmax,	550, 	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//网侧电流闭环增量限幅

	{&_MPR_ID_Kp,			40, 	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	//机侧变流器电流闭环比例系数
	{&_MPR_ID_Ki,			1600,	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1+VA0},//机侧变流器电流闭环积分系数
	{&_MPR_ID_Kd,			0,		0,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3+VA0},//机侧变流器电流闭环积分系数
	{&_MPR_ID_outmax,		200,	1,		500,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//机侧变流器电流闭环输出限幅
	{&_MPR_ID_errmax,		500,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//机侧变流器电流闭环误差限幅
	{&_MPR_ID_errmin,		0,		0,		2000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//机侧变流器电流闭环误差限幅
	{&_MPR_ID_incrementmax,	550, 	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//机侧变流器电流闭环增量限幅

	{&_MPR_IQ_Kp,			15, 	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	//机侧变流器电流闭环比例系数
	{&_MPR_IQ_Ki,			2000,	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1+VA0},//机侧变流器电流闭环积分系数
	{&_MPR_IQ_Kd,			0,		0,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3+VA0},//机侧变流器电流闭环积分系数
	{&_MPR_IQ_outmax,		200,	1,		500,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//机侧变流器电流闭环输出限幅
	{&_MPR_IQ_errmax,		500,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//机侧变流器电流闭环误差限幅
	{&_MPR_IQ_errmin,		0,		0,		2000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	//机侧变流器电流闭环误差限幅
	{&_MPR_IQ_incrementmax,	550, 	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	//机侧变流器电流闭环增量限幅
/*
	{&_DYN_U_Kp,			40,	    1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	  //动态电压闭环比例系数
	{&_DYN_U_Ki,			50,		1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1+VA0},//动态电压闭环积分系数//201007BJTULVRT
	{&_DYN_U_Kd,			0,		0,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3+VA0},//动态电压闭环积分系数
	{&_DYN_U_outmax,		600,	1,		1000,		&_NULL_VAL,	&_NULL_VAL,	WR0},		  //动态电压闭环输出限幅 
	{&_DYN_U_errmax,		1000,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	  //动态电压闭环误差限幅 
	{&_DYN_U_errmin,		0,		0,		2000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	  //动态电压闭环误钭钚〉 
	{&_DYN_U_incrementmax,	1000, 	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	  //动态电压闭环增量限幅
*/
	{&_DYN_U_Kp,			40,		1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT2},	  //动态电压闭环比例系数
	{&_DYN_U_Ki,			1000,	1,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1+VA0},//动态电压闭环积分系数//201007BJTULVRT
	{&_DYN_U_Kd,			0,		0,		20000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3+VA0},//动态电压闭环积分系数
	{&_DYN_U_outmax,		600,	1,		1000,		&_NULL_VAL,	&_NULL_VAL,	WR0},		  //动态电压闭环输出限幅 
	{&_DYN_U_errmax,		1000,	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	  //动态电压闭环误差限幅 
	{&_DYN_U_errmin,		0,		0,		2000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},	  //动态电压闭环误钭钚〉 
	{&_DYN_U_incrementmax,	1000, 	1,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT1},	  //动态电压闭环增量限幅

//-------------------------------'保护阈值'---201007BJTULVRT------------------------------------------------------
//--35/71-113		
	{&_SC_RTRN,		5,			0,			20,			&_NULL_VAL,	&_NULL_VAL,	WR1},		//故障最大重试次数
	{&_SC_RTRT,		15,			10,			600,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//两次故障重试间隔时间(s)，延时恢复时间
	{&_SC_UDCOV,	1350,		1130,       1350,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//中间直流电压 过压动作值(V) 
	{&_SC_IDCOV,	650,    	0,       	1100,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//中间直流电压 欠压动作值(V) 
	{&_SC_VDCON,	750,		700,       	900,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//中间直流电压 上电完成比较阈值(V) 	

	{&_SC_IACOV1,	180, 		20,			200,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//网侧交流电流 过流保护值(基准电流的百分值)
	{&_SC_UACOV1,	770,		690,		828,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//网侧电压 过压动作值(V) 20140503LJD电能质量
	{&_SC_UACLV1,	100,	   	0,			690,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//网侧电压 欠压动作值(V) 20140503LJD电能质量 20150204XD
//	{&_SC_UACLV1,	100,	   	0,			690,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//网侧电压 欠压动作值(V) 201005atcpc
	{&_SC_IACOV2,	180, 		20,			200,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//电机侧交流电流 过流保护值(基准电流的百分值)
	{&_SC_PGOV,		1800,		0,			3000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//变流器有功功率过载阈值(kW) 201005atcpc	

	{&_SC_QGOV,		1000,		0,			3000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//变流器无功功率过载阈值(kW)
	{&_SC_IACOVST,	150, 		20,			200,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//定子侧交流电流 过流保护值(基准电流的百分值)
	{&_SC_NPR_TLOV,	160, 		20,	    	200,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//网侧电感PT100超温保护值，单位摄氏度
	{&_SC_MPR_TLOV,	160, 		20,	    	200,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//电机侧电感PT100超温保护值，单位摄氏度
	{&_SC_LVIAC2,	190, 		20,			240,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//SCR电机侧交流电流 过流保护值(基准电流的百分值) 201005atcpc

	{&_SC_LVUDC1,	1180,		1100,       1350,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//机侧封脉冲 中间直流电压1 过压动作值(V)
	{&_SC_LVUDC2,	1200,		1100,       1350,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//SCR动作 中间直流电压2 过压动作值(V)
	{&_SC_TSCRON,	25,			10,         1000,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//SCR导通时间（ms）
	{&_SC_MSPEED1,	950, 		800,	    1500,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//转速保护 欠速保护值
	{&_SC_MSPEED2,	2030, 		1800,	    2200,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//转速保护 超速保护值

	{&_SC_UDSTAC,	50,		    0,			150,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//定子接触器合闸电压差阈值
	{&_SC_SKPTOV,	100,		20,			110,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//IGBT超温
	{&_SC_FREQ1,	515,		500,		550,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT1},	//频率范围上限
	{&_SC_FREQ2,	475,		450,		500,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT1},	//频率范围下限	
	{&_SCIB_BAUD,	19200,		1200,		57600,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//485波特率

	{&_SCIB_ADDR,	1,			1,			31,			&_NULL_VAL,	&_NULL_VAL,	WR1},		//485节点地址	
	{&_PROSTDBY1,	500,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//备用
	{&_PROSTDBY2,	665,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//备用

//201005atcpc
	{&_SC_Rs,		7747,		0,			15000,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT3},	//定子电阻(mohm)
	{&_SC_Lls,		284,		1,			1000,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT3},	//定子漏抗(mH)
	{&_SC_Rr,		4175,		0,			15000,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT3},	//转子电阻(mohm)	
	{&_SC_Llr,		456,		1,			1000,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT3},	//转子漏抗(mH)
	{&_SC_Lm,		17870,		1,			50000,		&_NULL_VAL,	&_NULL_VAL,	WR1+DOT3},	//励磁电抗(mH)

	{&_SC_POLES,	2,		    1,			8,			&_NULL_VAL,	&_NULL_VAL,	WR1},		//极对数
	{&_SC_Usn,		690,		380,		690,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//定子额定电压(V)
	{&_SC_Uro,		2018,		380,		10000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//转子开路电压(V)	
	{&_SC_PLSPRVL,	2048,		1,			10000,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//编码器每圈光栅数量
	{&_SC_MSTDBY,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//电机参数备用 MagnetCurve2013-12-13--ZZJ

	{&_SC_EXISTY1,	920,		100,		3000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},		//电机励磁调节备用1 MagnetCurve2013-12-13--ZZJ
	{&_SC_EXISTY2,	950,		100,		3000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},		//电机励磁调节备用2 MagnetCurve2013-12-13--ZZJ
	{&_SC_EXISTY3,	1010,		100,		3000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},		//电机励磁调节备用3	MagnetCurve2013-12-13--ZZJ
	{&_SC_EXISTY4,	1100,		100,		3000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},		//电机励磁调节备用4 MagnetCurve2013-12-13--ZZJ
	{&_SC_EXISTY5,	1280,		100,		3000,		&_NULL_VAL,	&_NULL_VAL,	WR0+DOT3},		//电机励磁调节备用5 MagnetCurve2013-12-13--ZZJ

//-------------------------------'模拟数字量变量'-----------------------------------------------------
//--14/114-127
	{&_COMMAND1,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR3},		//运行指令第1组
	{&_COMMAND2,	0,			0,		    65535,		&_NULL_VAL,	&_NULL_VAL,	WR3},	    //运行指令第2组 2048--0x0800 2013-12-9 ZZJ
	{&_STDBY9,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR3},		//备用9	20141004 LJD
	{&_PCEROTRIG,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR0},		//PC故障示波器触发条件选择 20100308
	{&_STATE1, 		0,	  	    0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//运行状态1

	{&_STATE2,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},	    //运行状态2
	{&_DISPLAY,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//显示
	{&_INPUT,		0,		    0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},   	//IO输入状态
	{&_OUTPUT,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//IO输出状态
	{&_LVRT,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//LVRT状态相关位

	{&_STDBY11,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//备用11
	{&_STDBY12,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//备用12
	{&_STDBY13,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//备用13
	{&_STDBY14,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//备用14

//-------------------------------时间设定-----------------------------------------------------------
//--6/128-133
	{&RTIMER.time[0],	0,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//最新故障时间(秒)
	{&RTIMER.time[1],	0,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//最新故障时间(分)
	{&RTIMER.time[2],	0,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//最新故障时间(时)
	{&RTIMER.time[3],	1,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//最新故障时间(日)
	{&RTIMER.time[4],	1,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//最新故障时间(月)
	{&RTIMER.time[5],	10,		0,			65535,		&_NULL_VAL,	&_NULL_VAL,	WR1},		//最新故障时间(年)


//-------------------------------运行监控----------------------------------------------------------
//--53/134-186
	{&_BA_MIDRF,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//d轴电流指令(A)
	{&_BA_NIQRF,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//q轴电流指令(A)
	{&_BA_MIQRF,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//q轴电流指令(A)
	{&_BA_TOQRF,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},	//转矩指令(NM)
	{&_BA_AGLRF,	0,			-90,		90,	    	&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},	//无功角度指令()

	{&_BA_URF,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//中间直流电压给定值(V)
	{&_BA_UDC,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//中间直流电压(V)	
	{&_BA_IA1,		0,			0,    		65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},	//网侧变流器A相并网电流有效值(A)
	{&_BA_IB1,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},	//网侧变流器B相并网电流行е?A)
	{&_BA_IC1,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},	//网侧变流器C相并网电流有效值(A)

	{&_BA_IA2,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},	//电机侧变流器A嗖⑼缌饔行е?(A)
	{&_BA_IB2,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},	//电机侧变流器B相并缌饔行е?A)	
	{&_BA_IC2,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},	//电机侧变流器C相并网电流有效值(A)
	{&_BA_UAB1,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//网侧AB线电压有效值(V)
	{&_BA_UBC1,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//网侧BC线电压有效值(V)

	{&_BA_UAB2,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//20091027atzy挪用作网压跌落检测 电机侧AB线电压有效值(V)
	{&_BA_UBC2,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//20091027atzy挪用作网压跌落检测 电机侧BC线电压有效值(V)
	{&_BA_FREQ,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//电网频率(hz)
	{&_BA_UAB0,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//主断前AB线电压有效值(V)
	{&_BA_UBC0,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//主断前BC线电压有效值(V)

	{&_BA_TLAC,		0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},	//网侧电感温度(摄氏度)20091019atzy
	{&_BA_TLDUDT,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},	//机侧电感温度(摄氏度)20091019atzy
	{&_BA_TNSKIIP,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//网侧SKIIP温度(摄氏度) CPC
	{&_BA_TMSKIIP,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//机侧SKIIP温度(摄氏度) cpc
	{&_BA_SPEED,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//电?(rpm)

	{&_BA_PIONU,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//网侧缪够肥涑?
	{&_BA_PIONID,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//网侧d轴电流环输出
	{&_BA_PIONIQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//网侧q轴电流环输出
	{&_BA_MEXI,	    0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//
	{&_BA_PIOMID,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//机侧d轴电流环输出

	{&_BA_PIOMIQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//机侧q轴电流环输出
	{&_BA_GRDUD,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//d轴主断后网压反馈 滤波后
	{&_BA_MC_IN,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT2},		//励磁校正输入MagnetCurve2013-12-13--ZZJ
//	{&_BA_STAUD,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//定子电压d
	{&_BA_GRDUQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//q轴主断后网压反馈 滤波后
	{&_BA_MC_K,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT3},		//励磁校正输出MagnetCurve2013-12-13--ZZJ
//	{&_BA_STAUQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//定子电压q


	{&_BA_NPRID,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//d轴电流反馈 滤波后 201005atcpc
	{&_BA_NPRIQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//q轴电流反馈 滤波后 201005atcpc
//	{&_BA_EXCID,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//机侧励磁电流d轴
//	{&_BA_EXCIQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	 //机侧励磁电流q轴
	{&_BA_SCRIA,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//SCR-A	20110906
	{&_BA_SCRIB,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//SCR-B	20110906
//	{&_BA_STAUABD,	0,			0,    		65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},		//20091027atzy挪用作网压跌落检测 定子同步并网前定子前后电压差
	{&_BA_STAUABD,	0,			-10000,		10000,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//定子励磁电压差--用于校正励磁曲线MagnetCurve2013-12-13--ZZJ

	{&_BA_STAUBCD,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},	    //20091027atzy挪用作网压跌落检测 定子同步并网前定子前后电压差
	{&_BA_STAIAC,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},		//定子线电流有效值显示 cpc
	{&_BA_STAIBA,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},		//定子线电流有效值显示cpc
	{&_BA_TOQFB,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//矩反馈(NM)
	{&_BA_PSTA,		0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//定子侧有功功率显示kW

	{&_BA_PNPR,		0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//网侧有功功率显示
	{&_BA_PGRID,	0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//并网总的有功功率显示 
	{&_BA_QSTA,		0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//定子侧无功功率显示kVAR
	{&_BA_QNPR,		0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//网侧无功功率显示
	{&_BA_QGRID,	0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//并网总的无功功率显示

	{&_BA_SSTA,		0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//定子侧视在功率显示
	{&_BA_SNPR,		0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//网侧视在功率显示
	{&_BA_SGRID,	0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//并网总的有功功率显示 


//-------------------------------'报警信息'--------------------------------------------------------
//--42/187-228
	{&_BA_EMIDRF,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 d轴电髦噶?(A)
	{&_BA_ENIQRF,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 q轴电流(A)
	{&_BA_EMIQRF,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 q轴电流指令(A)
	{&_BA_ETOQRF,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//最新故障时刻 转矩指令(A)
	{&_BA_EURF,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//最新故障时刻 中间直流电压给定值(V)

	{&_BA_EAGLRF,	0,			-90,		90,	    	&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//最新故障时刻 无功角度指令()   07.27
	{&_BA_EUDC,		0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//最新故障时刻 中间直流缪(V)	
	{&_BA_EIA1,		0,			-32767,     32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 网侧变髌鰽相并网电流有效值(A)
	{&_BA_EIB1,		0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 网侧变流器B相并网电流有效值(A)
	{&_BA_EIC1,		0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 网侧变流器C相并网电流有效值(A)

	{&_BA_EIA2,		0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 电机侧变髌鰽相并网电流有效值(A)
	{&_BA_EIB2,		0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 电机侧变流器B相并网电流有效值(A)	
	{&_BA_EIC2,		0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 电机侧变流器C相并网电流有效值(A)
	{&_BA_EUAB1,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//最新故障时刻 网侧AB线电压有值(V)
	{&_BA_EUBC1,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//最新故障笨? 网侧BC线电压行е?(V)

	{&_BA_EUAB2,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//最新故障时刻 电机侧AB线电压有效值(V)
	{&_BA_EUBC2,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//最新故障时刻 电机侧BC线电压有效值(V)	
    {&_BA_EFREQ,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//最新故障时刻 电网频率(hz)
	{&_BA_EUAB0,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//最新故障时刻 主断前AB线电压有效值(V)
	{&_BA_EUBC0,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//最新故障时刻 主断前BC线电压有效值(V)

	{&_BA_ETLAC,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//最新故障时刻 网侧电感温度(摄氏度) 20091019atzy
	{&_BA_ETLDUDT,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//最新故障时刻 机侧电感温度(摄氏度)	20091019atzy
	{&_BA_ETSKIIP,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//最新故障时刻 SKIPP温度(摄氏度)
	{&_BA_ESPEED,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},			//最新故障时刻 电机转速(rpm)
	{&_BA_ENPRUD,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//最新故障时刻 d轴主断后网压反馈	20091019atzy

	{&_BA_ENPRUQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//最新故障时刻 q轴主断后网压反馈	20091019atzy
	{&_BA_ENPRUD2,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//最新故障时刻 d轴主断后网压反馈 滤波后	20091019atzy
	{&_BA_ENPRUQ2,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//最新故障时刻 q轴主断后网压反馈 滤波后	20091019atzy
	{&_BA_ENUDOUT,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//最新故障时刻 网侧d轴电压输出	20091019atzy
	{&_BA_ENUQOUT,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//最新故障时刻 网侧q轴电压输出	20091019atzy

	{&_BA_EPIONU,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 网侧电压环输出
	{&_BA_EPIONID,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 网侧d轴电流环输
	{&_BA_EPIONIQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 网侧q轴电流环输出
	{&_BA_EMEXI,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 机侧励磁理论计算值显示 07.27
	{&_BA_EPIOMID,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 机侧d轴电流环输出

	{&_BA_EPIOMIQ,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 机侧q轴电流环输出
	{&_BA_ESTAIAC,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},		//最新故障时刻 定子线电流有效值显示 cpc 07.27
	{&_BA_ESTAIBA,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO+DOT1},		//最新故障时刻 定子线电流有效值显示cpc 07.27
	{&_BA_ETOQFB,	0,			-32767,		32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG},		//最新故障时刻 转矩反馈(NM) 07.27
	{&_BA_EPSTA,	0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 定子侧有功功率显示kW 07.27

	{&_BA_EPNPR,	0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 网侧有功功率显示 07.27
	{&_BA_EPGRID,	0,			-32767,	    32767,		&_NULL_VAL,	&_NULL_VAL,	RDO+SIG+DOT1},	//最新故障时刻 并网总的有功功率显示  07.27

	

//-------------------------------'报警原因/时间'--------------------------------------------------------
//--28/229-256	
	{&_BA_ERR4,		0,			0,			100,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//最新故障信息
	{&_BA_TIME4_0,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//最新故障时间(秒)
	{&_BA_TIME4_1,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//新故障时间(分)
	{&_BA_TIME4_2,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//最新故障时间(时)
	{&_BA_TIME4_3,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//最新故障时间(日)
	{&_BA_TIME4_4,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//最新故障时间(月)	
	{&_BA_TIME4_5,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//最新故障时间(年)

	{&_BA_ERR3,		0,			0,			100,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//第三次故障信息 
	{&_BA_TIME3_0,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//第三次故障时间(秒)
	{&_BA_TIME3_1,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//第三次故障时间(分)
	{&_BA_TIME3_2,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//第三次故障时间(时)
	{&_BA_TIME3_3,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//第三次故障时间(日)	
	{&_BA_TIME3_4,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//第三次故障时间(月)
	{&_BA_TIME3_5,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//第三次故障时间(年)

	{&_BA_ERR2,		0,			0,			100,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//第二次故障信息 
	{&_BA_TIME2_0,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//第二次故障时间(秒)
	{&_BA_TIME2_1,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//第二次故障时间(分)
	{&_BA_TIME2_2,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//第二次故障时间(时)	
	{&_BA_TIME2_3,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//第二次故障时间(日)
	{&_BA_TIME2_4,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//第二次故障时间(月)
	{&_BA_TIME2_5,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//第二喂收鲜奔?年)

	{&_BA_ERR1,		0,			0,			100,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//第一次故障信息 
	{&_BA_TIME1_0,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//第一次故障时间(秒)
	{&_BA_TIME1_1,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//第一次故障时间(分)
	{&_BA_TIME1_2,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//第一次故障时间(时)
	{&_BA_TIME1_3,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//第一次故障时间(日)
	{&_BA_TIME1_4,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//诖问?(月)
	{&_BA_TIME1_5,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//第一次故障时间(年)	                                    	

	{&_BA_ERR5,		0,			0,			100,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//示波器故障信息 201005atcpc
	{&_BA_TIME5_0,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//示波器故障时间(秒)
	{&_BA_TIME5_1,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//示波器故障时间(分)
	{&_BA_TIME5_2,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//示波器故障时间(时)
	{&_BA_TIME5_3,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//示波器故障时间(日)
	{&_BA_TIME5_4,	0,			0,			65535,	    &_NULL_VAL,	&_NULL_VAL,	RDO},		//示波器故障时间(月)
	{&_BA_TIME5_5,	0,			0,			65535,		&_NULL_VAL,	&_NULL_VAL,	RDO},		//示波器故障时间(年)	                                    	

//-------------------------------------------------------------------------------------------------
};

//--------------------------------------//故障信息子表(４媸莸男蚝?--70)20091026atzy
const Uint16 TAB_ERR[ERRO_NUM]={
	TAB_ERR_FIRST   ,TAB_ERR_FIRST+ 1,TAB_ERR_FIRST+ 2,TAB_ERR_FIRST+ 3,
	TAB_ERR_FIRST+ 4,TAB_ERR_FIRST+ 5,TAB_ERR_FIRST+ 6,TAB_ERR_FIRST+ 7,
	TAB_ERR_FIRST+ 8,TAB_ERR_FIRST+ 9,TAB_ERR_FIRST+10,TAB_ERR_FIRST+11,
	TAB_ERR_FIRST+12,TAB_ERR_FIRST+13,TAB_ERR_FIRST+14,TAB_ERR_FIRST+15,
	TAB_ERR_FIRST+16,TAB_ERR_FIRST+17,TAB_ERR_FIRST+18,TAB_ERR_FIRST+19,
	TAB_ERR_FIRST+20,TAB_ERR_FIRST+21,TAB_ERR_FIRST+22,TAB_ERR_FIRST+23,
	TAB_ERR_FIRST+24,TAB_ERR_FIRST+25,TAB_ERR_FIRST+26,TAB_ERR_FIRST+27,
	TAB_ERR_FIRST+28,TAB_ERR_FIRST+29,TAB_ERR_FIRST+30,TAB_ERR_FIRST+31,
	TAB_ERR_FIRST+32,TAB_ERR_FIRST+33,TAB_ERR_FIRST+34,TAB_ERR_FIRST+35,
	TAB_ERR_FIRST+36,TAB_ERR_FIRST+37,TAB_ERR_FIRST+38,TAB_ERR_FIRST+39,
	TAB_ERR_FIRST+40,TAB_ERR_FIRST+41,TAB_ERR_FIRST+42,TAB_ERR_FIRST+43,
	TAB_ERR_FIRST+44,TAB_ERR_FIRST+45,TAB_ERR_FIRST+46,TAB_ERR_FIRST+47,
	TAB_ERR_FIRST+48,TAB_ERR_FIRST+49,TAB_ERR_FIRST+50,TAB_ERR_FIRST+51,
	TAB_ERR_FIRST+52,TAB_ERR_FIRST+53,TAB_ERR_FIRST+54,TAB_ERR_FIRST+55,
	TAB_ERR_FIRST+56,TAB_ERR_FIRST+57,TAB_ERR_FIRST+58,TAB_ERR_FIRST+59,
    TAB_ERR_FIRST+60,TAB_ERR_FIRST+61,TAB_ERR_FIRST+62,TAB_ERR_FIRST+63,
    TAB_ERR_FIRST+64,TAB_ERR_FIRST+65,TAB_ERR_FIRST+66,TAB_ERR_FIRST+67,
    TAB_ERR_FIRST+68,TAB_ERR_FIRST+69,TAB_ERR_FIRST+70,TAB_ERR_FIRST+71,
    TAB_ERR_FIRST+72,TAB_ERR_FIRST+73,TAB_ERR_FIRST+74,TAB_ERR_FIRST+75,
    TAB_ERR_FIRST+76
};

//--------------------------------------//电机励磁系数信息子表(80%到115%疃ㄍ故?(540V--800V)，实蔵rd*/理论计算ird*)
const float TAB_EXI[128]={
 0.9743,    0.9750,    0.9758,    0.9766,    0.9774,    0.9783,    0.9792,
 0.9801,    0.9811,    0.9820,    0.9830,    0.9841,    0.9851,    0.9862,
 0.9873,    0.9884,    0.9896,    0.9907,    0.9919,    0.9932,    0.9944,
 0.9957,    0.9970,    0.9983,    0.9997,    1.0010,    1.0024,    1.0039,
 1.0053,    1.0068,    1.0083,    1.0098,    1.0114,    1.0129,    1.0146,

 1.0162,    1.0178,    1.0195,    1.0213,    1.0230,    1.0248,    1.0266,
 1.0284,    1.0303,    1.0322,    1.0342,    1.0361,    1.0381,    1.0402,
 1.0423,    1.0444,    1.0465,    1.0487,    1.0510,    1.0532,    1.0556,
 1.0579,    1.0603,    1.0628,    1.0653,    1.0678,    1.0704,    1.0731,
 1.0758,    1.0785,    1.0813,    1.0842,    1.0871,    1.0901,    1.0931,

 1.0962,    1.0994,    1.1026,    1.1059,    1.1092,    1.1127,    1.1162,
 1.1197,    1.1234,    1.1271,    1.1309,    1.1348,    1.1388,    1.1428,
 1.1470,    1.1512,    1.1555,    1.1599,    1.1644,    1.1690,    1.1737,
 1.1785,    1.1834,    1.1884,    1.1935,    1.1987,    1.2040,    1.2095,
 1.2150,    1.2207,    1.2265,    1.2324,    1.2384,    1.2446,    1.2509,

 1.2574,    1.2639,    1.2707,    1.2775,    1.2845,    1.2917,    1.2990,
 1.3064,    1.3140,    1.3218,    1.3297,    1.3378,    1.3460,    1.3545,
 1.3631,    1.3718,    1.3808,    1.3899,    1.3992,    1.4088,    1.4185,
 1.4284,    1.4384 
}; 

//--------------------------------------//电机励磁系数子表(补偿负载电流)
const float TAB_IQEXI[128]={
 1.0000,   1.0239,    1.0319,    1.0398,    1.0476,    1.0554,    1.0631,    1.0707,    1.0783,    1.0858,    1.0932,    1.1006,
 1.1079,   1.1151,    1.1222,    1.1293,    1.1363,    1.1432,    1.1501,    1.1569,    1.1636,    1.1702,    1.1768,    1.1833,
 1.1898,   1.1961,    1.2024,    1.2087,    1.2148,    1.2209,    1.2269,    1.2329,    1.2387,    1.2445,    1.2503,    1.2559,
 1.2615,   1.2671,    1.2725,    1.2779,    1.2832,    1.2884,    1.2936,    1.2987,    1.3037,    1.3087,    1.3136,    1.3184,
 1.3232,   1.3278,    1.3324,    1.3370,    1.3414,    1.3458,    1.3502,    1.3544,    1.3586,    1.3627,    1.3668,    1.3707,
 1.3746,   1.3785,    1.3822,    1.3859,    1.3896,    1.3931,    1.3966,    1.4000,    1.4033,    1.4066,    1.4098,    1.4129,
 1.4160,   1.4190,    1.4219,    1.4248,    1.4275,    1.4302,    1.4329,    1.4354,    1.4379,    1.4404,    1.4427,    1.4450,
 1.4472,   1.4494,    1.4514,    1.4534,    1.4554,    1.4572,    1.4590,    1.4607,    1.4624,    1.4640,    1.4655,    1.4669,
 1.4683,   1.4696,    1.4708,    1.4720,    1.4731,    1.4741,    1.4750,    1.4759,    1.4767,    1.4774,    1.4781,    1.4787,
 1.4792,   1.4797,    1.4801,    1.4804,    1.4806,    1.4808,    1.4809,    1.4809,    1.4809,    1.4808,    1.4806,    1.4804,
 1.4800,   1.4796,    1.4792,    1.4787,    1.4780,    1.4774,    1.4766,    1.4758                                           
}; 


//过载温度表格
//要求：过载能力：               105％/1h，110％/3min，150％/10s，200%/0s

//tab1//1s t1(j)=10^(0.36095/(i^2-0.97538) +0.7169);
const Uint16 TAB_ET2[256]={										//1s
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	
	0,	0,	0,	0,	0,	0,	1,	1,	2,	2,	
	3,	4,	6,	8,	10,	12,	15,	18,	22,	27,	
	31,	37,	43,	49,	56,	64,	73,	82,	91,	102,	
	113,	124,	136,	149,	163,	177,	192,	207,	223,	239,	
	257,	274,	292,	311,	330,	350,	370,	391,	412,	434,	
	456,	478,	501,	524,	548,	572,	596,	620,	645,	670,	
	696,	722,	748,	774,	800,	827,	854,	881,	908,	936,	
	963,	991,	1019,	1047,	1075,	1103,	1131,	1160,	1188,	1217,	
	1246,	1274,	1303,	1332,	1361,	1390,	1419,	1448,	1477,	1506,	
	1534,	1563,	1592,	1621,	1650,	1679,	1708,	1737,	1766,	1794,	
	1823,	1852,	1881,	1909,	1938,	1966,	1995,	2023,	2051,	2079,	
	2107,	2136,	2164,	2191,	2219,	2247,	2275,	2302,	2330,	2357,	
	2384,	2412,	2439,	2466,	2493,	2519,	2546,	2573,	2599,	2626,	
	2652,	2678,	2704,	2730,	2756,	2782,	2807,	2833,	2858,	2884,	
	2909,	2934,	2959,	2984,	3009,	3034,	3058,	3083,	3107,	3131,	
	3155,	3179,	3203,	3227,	3251,	3274,	3298,	3321,	3344,	3368,	
	3391,	3414,	3436,	3459,	3482,	3504,	3526,	3549,	3571,	3593,	
	3615,	3637,	3658,	3680,	3702,	3723,	3744,	3765,	3787,	3808,	
	3828,	3849,	3870,	3890,	3911,	3931,	3952,	3972,	3992,	4012,	
	4032,	4051,	4071,	4091,	4110,	4129,	4149,	4168,	4187,	4206,	
	4225,	4244,	4262,	4281,	4299,	4318,	4336,	4354,	4373,	4391,	
	4409,	4426,	4444,	4462,	4480,	4497,	4515,	4532,	4549,	4566,	
	4583,	4600,	4617,	4634,	4651,	4668,	4684,	4701,	4717,	4733,	
	4750,	4766,	4782,	4798,	4814,	4830,	4845,	4861,	4877,	4892,	
	4908,	4923,	4939,	4954,	4969,	4984 
};

//tab2//200us   t2(j)=10^(24.1717/(i^2) -9.7430);
const Uint16 TAB_ET1[256]={										//200us
	1,	1,	1,	1,	1,	1,	1,	2,	2,	2,	
	2,	2,	2,	2,	2,	3,	3,	3,	3,	3,	
	3,	4,	4,	4,	4,	5,	5,	5,	6,	6,	
	6,	7,	7,	7,	8,	8,	9,	9,	10,	10,	
	11,	12,	12,	13,	14,	14,	15,	16,	17,	18,	
	19,	20,	21,	22,	23,	24,	26,	27,	29,	30,	
	32,	33,	35,	37,	39,	41,	43,	45,	47,	50,	
	52,	55,	58,	61,	64,	67,	70,	74,	78,	81,	
	85,	90,	94,	98,	103,	108,	113,	119,	125,	130,	
	137,	143,	150,	157,	164,	172,	180,	188,	197,	206,	
	215,	225,	235,	246,	257,	268,	280,	293,	306,	320,	
	334,	348,	364,	379,	396,	413,	431,	450,	469,	489,	
	510,	531,	554,	577,	602,	627,	653,	680,	708,	738,	
	768,	799,	832,	866,	901,	938,	976,	1015,	1056,	1098,	
	1141,	1187,	1234,	1282,	1333,	1385,	1439,	1495,	1553,	1613,	
	1675,	1739,	1806,	1874,	1946,	2019,	2096,	2174,	2256,	2340,	
	2427,	2518,	2611,	2707,	2807,	2909,	3016,	3125,	3239,	3356,	
	3477,	3602,	3731,	3864,	4002,	4143,	4290,	4441,	4597,	4758,	
	4924,	5095,	5272,	5454,	5642,	5836,	6036,	6242,	6454,	6673,	
	6899,	7131,	7370,	7617,	7871,	8133,	8403,	8681,	8967,	9261,	
	9565,	9877,	10198,	10529,	10870,	11220,	11580,	11951,	12333,	12726,	
	13130,	13545,	13972,	14411,	14863,	15328,	15805,	16296,	16800,	17319,	
	17852,	18399,	18962,	19540,	20134,	20744,	21370,	22014,	22674,	23353,	
	24050,	24765,	25500,	26254,	27027,	27822,	28637,	29473,	30331,	31212,	
	32115,	33042,	33993,	34968,	35968,	36993,	38045,	39123,	40229,	41362,	
	42524,	43715,	44935,	46186,	47468,	48781 
};
 
//--------------------------------------//掉电信息子表(保存数据的序号)

//--------------------------------------//显示信息子表Systest 2014-05-06LJD参照华创程序修改故障属性
const struct MESSAGE TAB_MSG[MSG_NUM]={
	{{'-',	'-',	'-'},	0, 			NONE},						//0=MSG_NONE

	{{'D',	'-',	'-'},	20, 		SHUT+D_RCVR+SAVE},			//1=MSG_CODEOK
	{{'E',	'E',	'E'},	20, 		SHUT+D_RCVR+SAVE},			//2=MSG_EE_FAIL
	{{'E',	'S',	'P'},	19, 		SHUT+I_RCVR+SAVE},			//3=MSG_ESTOP
	{{'C',	'B',	'F'},	18, 		OFFCB+D_RCVR+SAVE},			//4=MSG_CBTRIP
	{{'E',	'F',	'T'},	17, 		OFFCB+CNT+SAVE},			//5=MSG_EXFAULT 
	{{'C',	'A',	'N'},	16, 		SHUT+I_RCVR+SAVE},			//6=MSG_CAN_FAIL
	{{'E',	'C',	'F'},	15, 		SHUT+D_RCVR+CNT+SAVE}, 		//7=MSG_ENCODFAULT 
	{{'C',	'B',	'F'},	14, 		OFFCB+D_RCVR+CNT+SAVE},		//8=MSG_CBERROR 
	{{'P',	'R',	'F'},	14, 		OFFCB+D_RCVR+CNT+SAVE},		//9=MSG_PREERROR 
	{{'M',	'K',	'F'},	14, 		OFFCB+D_RCVR+CNT+SAVE},		//10=MSG_MAINKERROR 
	{{'M',	'F',	'F'},	14, 		OFFCB+D_RCVR+CNT+SAVE},		//11=MSG_MAINFERROR 2014-05-06没有参照华创，增加了CNT属性
	{{'S',	'C',	'F'},	14, 		OFFCB+D_RCVR+CNT+SAVE},		//12=MSG_STACERROR
	{{'O',	'C',	'1'},	13, 		OFFCB+D_RCVR+CNT+SAVE},     //13=MSG_HIA1
	{{'O',	'C',	'2'},	13, 		OFFCB+D_RCVR+CNT+SAVE},		//14=MSG_HIB1
	{{'O',	'C',	'3'},	13, 		OFFCB+D_RCVR+CNT+SAVE},		//15=MSG_HIC1
	{{'O',	'C',	'4'},	13, 		OFFCB+D_RCVR+CNT+SAVE},		//16=MSG_HIA2
	{{'O',	'C',	'5'},	13, 		OFFCB+D_RCVR+CNT+SAVE},		//17=MSG_HIB2	
	{{'O',	'C',	'6'},	13, 		OFFCB+D_RCVR+CNT+SAVE},		//18=MSG_HIC2
    {{'U',	'F',	'1'},	12, 		OFFCB+D_RCVR+CNT+SAVE},		//19=MSG_UDCOV	
	{{'O',	'C',	'7'},	12, 		OFFCB+D_RCVR+CNT+SAVE},		//20=MSG_HIAC1 
 	{{'O',	'C',	'8'},	12, 		OFFCB+D_RCVR+CNT+SAVE},		//21=MSG_HIAC2 
	{{'P',	'D',	'A'},	11, 		OFFCB+D_RCVR+CNT+SAVE},		//22=MSG_PDPASERIES 	
 	{{'P',	'D',	'B'},	11, 		OFFCB+D_RCVR+CNT+SAVE},		//23=MSG_PDPBSERIES 
	{{'S',	'P',	'O'},	10, 		SHUT+D_RCVR+SAVE},			//24=MSG_MSPEEDOUT
	{{'O',	'C',	'9'},	9, 			SHUT+D_RCVR+CNT+SAVE},		//25=MSG_SIAC1 
	{{'O',	'C',	'A'},	9, 			SHUT+D_RCVR+CNT+SAVE},		//26=MSG_SIAC2 	
	{{'F',	'E',	'1'},	8, 			SHUT+I_RCVR+SAVE},			//27=MSG_FE1 201005atcpc	
	{{'U',	'F',	'2'},	7, 			SHUT+D_RCVR+CNT+SAVE},		//28=MSG_SUDCOV
	{{'U',	'F',	'3'},	7, 			SHUT+I_RCVR+CNT+SAVE},  	//29=MSG_SUACOV1
	{{'U',	'F',	'5'},	6, 			SHUT+I_RCVR+CNT+SAVE},		//30=MSG_SUDCLV
	{{'U',	'F',	'6'},	6, 			SHUT+I_RCVR+CNT+SAVE},		//31=MSG_SUACLV1 201005atcpc
	{{'D',	'C',	'E'},	5, 			SHUT+D_RCVR+CNT+SAVE},		//32=MSG_UDCWAVE 中间电压波动故障
	{{'O',	'C',	'S'},	5, 			SHUT+D_RCVR+CNT+SAVE},		//33=MSG_SIOVST 软件定子电流过流
	{{'G',	'F',	'1'},	4, 			SHUT+I_RCVR+CNT+SAVE},		//34=MSG_GFAULT1 网侧接地故障
	{{'G',	'F',	'2'},	4, 			SHUT+I_RCVR+CNT+SAVE},		//35=MSG_GFAULT2 机侧接地故障
	{{'O',	'H',	'1'},	3, 			SHUT+I_RCVR+CNT+SAVE},		//36=MSG_TOV1超温
	{{'O',	'H',	'2'},	3, 			SHUT+I_RCVR+CNT+SAVE},		//37=MSG_TOV2超温
	{{'O',	'S',	'K'},	3, 			SHUT+I_RCVR+CNT+SAVE},		//38=MSG_SKTOV超温
	{{'O',	'L',	'1'},	2, 			SHUT+I_RCVR+CNT+SAVE},		//39=MSG_TLOV1网侧电感超温
	{{'O',	'L',	'2'},	2, 			SHUT+I_RCVR+CNT+SAVE},		//40=MSG_TLOV2机侧电感超温
	{{'P',	'O',	'V'},	1, 			SHUT+CNT+SAVE},				//41=MSG_PGOV变流器有功功率过载
	{{'Q',	'O',	'V'},	1, 			SHUT+D_RCVR+CNT+SAVE},		//42=MSG_QGOV变流器无功功率过载
	{{'S',	'Y',	'F'},	1, 			SHUT+SAVE},					//43=MSG_STASYNFAIL定子同步并网失败
	{{'P',	'O',	'E'},	1, 			SHUT+I_RCVR+CNT+SAVE},		//44=MSG_PHORDE 
};


#ifdef __cplusplus
}
#endif /* extern "C" */


#endif  // end

//===========================================================================
// No more.
//===========================================================================
