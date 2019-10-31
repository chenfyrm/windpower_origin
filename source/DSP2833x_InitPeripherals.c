//###########################################################################
//
// FILE:	DSP281x_InitPeripherals.c
//
// TITLE:	DSP281x Device Initialization To Default State.
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  1.00| 11 Sep 2003 | L.H. | No change since previous version (v.58 Alpha)
//###########################################################################

#include "DSP2833x_Device.h"      // DSP2833x Headerfile Include File
#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File

void InitPeripherals(void)
{
	Uint16 i;
	Uint16 *p;
//----------------------Initial RAM-------------------------------------
	p=(Uint16 *)0x00C000;					//RAM数据初始化清0
	for(i=0;i<0x2000;i++)
	{
		*p=0x0000;
		p++;
	}
	
//----------------------Initial External RAM 20091110atzy-------------------------------------
/*	p=(Uint16 *)0x200000;			//外部RAM数据初始化清0,为了保存上次数据不能清零，不执行！
	for(i=0;i<0x40000;i++)
	{
		*p=0x0000;
		p++;
	}
*/		
	EALLOW;
//-----------------------Initial XINF-----------------------------------
#if DSP28_28335
// All Zones	
	XintfRegs.XINTCNF2.bit.XTIMCLK = 1; 	// XTIMCLK = 1/2 SYSCLKOUT
	XintfRegs.XINTCNF2.bit.WRBUFF = 0;		// No write buffering
	XintfRegs.XINTCNF2.bit.CLKOFF = 0;		// XCLKOUT is enabled
	XintfRegs.XINTCNF2.bit.CLKMODE = 1;		// XCLKOUT = XTIMCLK/2
// Zone 0
	XintfRegs.XTIMING0.bit.XWRLEAD = 3;		// Zone write timing
    XintfRegs.XTIMING0.bit.XWRACTIVE = 7;
    XintfRegs.XTIMING0.bit.XWRTRAIL = 3;
    
    XintfRegs.XTIMING0.bit.XRDLEAD = 3;		// Zone read timing
    XintfRegs.XTIMING0.bit.XRDACTIVE = 7;
    XintfRegs.XTIMING0.bit.XRDTRAIL = 3;
    
    XintfRegs.XTIMING0.bit.X2TIMING = 1;	// double read/write lead/active/trail timing	
    
    XintfRegs.XTIMING0.bit.USEREADY = 0;	// not sample XREADY signal
    XintfRegs.XTIMING0.bit.READYMODE = 1;	// sample asynchronous
    
    XintfRegs.XTIMING0.bit.XSIZE = 3;		// Size must be 1,1

// Zone 6------------------------------------
    XintfRegs.XTIMING6.bit.XWRLEAD = 3;
    XintfRegs.XTIMING6.bit.XWRACTIVE = 7;
    XintfRegs.XTIMING6.bit.XWRTRAIL = 3;
    
    XintfRegs.XTIMING6.bit.XRDLEAD = 3;
    XintfRegs.XTIMING6.bit.XRDACTIVE = 7;
    XintfRegs.XTIMING6.bit.XRDTRAIL = 3;
    
    XintfRegs.XTIMING6.bit.X2TIMING = 1;

    XintfRegs.XTIMING6.bit.USEREADY = 0;
    XintfRegs.XTIMING6.bit.READYMODE = 1;

    XintfRegs.XTIMING6.bit.XSIZE = 3;


// Zone 7------------------------------------
    XintfRegs.XTIMING7.bit.XWRLEAD = 1;
    XintfRegs.XTIMING7.bit.XWRACTIVE = 1;
    XintfRegs.XTIMING7.bit.XWRTRAIL = 0;

    XintfRegs.XTIMING7.bit.XRDLEAD = 1;
    XintfRegs.XTIMING7.bit.XRDACTIVE = 1;
    XintfRegs.XTIMING7.bit.XRDTRAIL = 0;
    
    XintfRegs.XTIMING7.bit.X2TIMING = 0;

    XintfRegs.XTIMING7.bit.USEREADY = 0;
    XintfRegs.XTIMING7.bit.READYMODE = 1;

    XintfRegs.XTIMING7.bit.XSIZE = 3;

    // Bank switching
    // Assume Zone 7 is slow, so add additional BCYC cycles 
    // when ever switching from Zone 7 to another Zone.  
    // This will help avoid bus contention.
    //XintfRegs.XBANK.bit.BANK = 7;
    //XintfRegs.XBANK.bit.BCYC = 7;

   //Force a pipeline flush to ensure that the write to 
   //the last register configured occurs before returning.  

   asm(" RPT #7 || NOP"); 
   
#endif

// --------------------Initialize CPU Timer0:---------------------
// 主频150M,中断频率5K
	CpuTimer0Regs.PRD.all  = 30000;      // T0=200us


// Initialize pre-scale counter to divide by 1 (SYSCLKOUT):	
	CpuTimer0Regs.TPR.all  = 0;
	CpuTimer0Regs.TPRH.all = 0;

// Initialize timer control register:
	CpuTimer0Regs.TCR.bit.TSS = 1;		// 1->stop;0->start
	CpuTimer0Regs.TCR.bit.TRB = 1;		// 1->reload
	CpuTimer0Regs.TCR.bit.SOFT = 0;		// 00->stop when emulation suspend
	CpuTimer0Regs.TCR.bit.FREE = 0;     
	CpuTimer0Regs.TCR.bit.TIE = 1;      // 0-> Disable;1->Enable Timer Interrupt

// -------------------End Of Initialize Cpu Timer0------------------	

// -------------------Initialize external interrupt1----------------
	XIntruptRegs.XINT1CR.bit.POLARITY = 0;	//falling edge interrupt
	XIntruptRegs.XINT1CR.bit.ENABLE   = 0;	//DISABLE XINT1 interrupt
// -------------------End of Initialize external interrupt1---------

// -------------------------Initialize Epwm---------------------------	
//PWM1A,PWM1B
  // Setup TBCLK
   EPwm1Regs.TBPRD = 0xFFFF;             // Set timer period 801 TBCLKs
   EPwm1Regs.TBPHS.half.TBPHS = 0x0000;  // Phase is 0
   EPwm1Regs.TBCTR = 0x0000;             // Clear counter
// Set Compare values
   EPwm1Regs.CMPA.half.CMPA = 0xFFFF;    // Set compare A value
   EPwm1Regs.CMPB = 0xFFFF;              // Set compare A value
// Setup counter mode
   EPwm1Regs.TBCTL.bit.CTRMODE = 2;      // Count up and down
   EPwm1Regs.TBCTL.bit.PHSEN = 0;        // Disable phase loading
   EPwm1Regs.TBCTL.bit.PRDLD = 0;		 // shadow
   EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;     // CTR=0
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = 1;    // Clock ratio to SYSCLKOUT/2
   EPwm1Regs.TBCTL.bit.CLKDIV = 0;
// Setup shadowing
   EPwm1Regs.CMPCTL.bit.SHDWAMODE = 0;	 // Shadow mode
   EPwm1Regs.CMPCTL.bit.SHDWBMODE = 0;	 // Shadow mode
   EPwm1Regs.CMPCTL.bit.LOADAMODE = 0;   // Load on  zero
   EPwm1Regs.CMPCTL.bit.LOADBMODE = 0;   // Load on  zero
// Active high complementary PWMs - Setup the deadband
    EPwm1Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm1Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
    EPwm1Regs.DBCTL.bit.IN_MODE = DBA_RED_DBB_FED;

//PWM2A,PWM2B
// Setup TBCLK
   EPwm2Regs.TBPRD = 0xFFFF;             // Set timer period 801 TBCLKs
   EPwm2Regs.TBPHS.half.TBPHS = 0x0000;  // Phase is 0
   EPwm2Regs.TBCTR = 0x0000;             // Clear counter
// Set Compare values
   EPwm2Regs.CMPA.half.CMPA = 0xFFFF;    // Set compare A value
   EPwm2Regs.CMPB = 0xFFFF;              // Set compare A value
// Setup counter mode
   EPwm2Regs.TBCTL.bit.CTRMODE = 2;      // Count up and down
   EPwm2Regs.TBCTL.bit.PHSEN = 1;        // enable phase loading
   EPwm2Regs.TBCTL.bit.PRDLD = 0;		 // shadow
   EPwm2Regs.TBCTL.bit.SYNCOSEL = 0;     // SYNCI
   EPwm2Regs.TBCTL.bit.HSPCLKDIV = 1;    // Clock ratio to SYSCLKOUT/2
   EPwm2Regs.TBCTL.bit.CLKDIV = 0;
// Setup shadowing
   EPwm2Regs.CMPCTL.bit.SHDWAMODE = 0;   // Shadow mode
   EPwm2Regs.CMPCTL.bit.SHDWBMODE = 0;
   EPwm2Regs.CMPCTL.bit.LOADAMODE = 0;   // Load on period
   EPwm2Regs.CMPCTL.bit.LOADBMODE = 0;   
// Active high complementary PWMs - Setup the deadband
    EPwm2Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm2Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
    EPwm2Regs.DBCTL.bit.IN_MODE = DBA_RED_DBB_FED;

//PWM3A,PWM3B
// Setup TBCLK
   EPwm3Regs.TBPRD = 0xFFFF;             // Set timer period 801 TBCLKs
   EPwm3Regs.TBPHS.half.TBPHS = 0x0000;  // Phase is 0
   EPwm3Regs.TBCTR = 0x0000;             // Clear counter
// Set Compare values
   EPwm3Regs.CMPA.half.CMPA = 0xFFFF;    // Set compare A value
   EPwm3Regs.CMPB = 0xFFFF;              // Set compare A value
// Setup counter mode
   EPwm3Regs.TBCTL.bit.CTRMODE = 2;      // Count up and down
   EPwm3Regs.TBCTL.bit.PHSEN = 1;        // enable phase loading
   EPwm3Regs.TBCTL.bit.PRDLD = 0;		 // shadow
   EPwm3Regs.TBCTL.bit.SYNCOSEL = 0;     // SYNCI
   EPwm3Regs.TBCTL.bit.HSPCLKDIV = 1;    // Clock ratio to SYSCLKOUT/2
   EPwm3Regs.TBCTL.bit.CLKDIV = 0;
// Setup shadowing
   EPwm3Regs.CMPCTL.bit.SHDWAMODE = 0;   // Shadow mode
   EPwm3Regs.CMPCTL.bit.SHDWBMODE = 0;  
   EPwm3Regs.CMPCTL.bit.LOADAMODE = 0;   // Load on  period
   EPwm3Regs.CMPCTL.bit.LOADBMODE = 0;   

// Active high complementary PWMs - Setup the deadband
    EPwm3Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm3Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
    EPwm3Regs.DBCTL.bit.IN_MODE = DBA_RED_DBB_FED;

       
// Set actions
// when the counter equals the active CMPA register,
//force EPWMxA output low  when the counter is decrementing.
//force EPWMxA output high when the counter is incrementing.   
   	EPwm1Regs.AQCTLA.all = 0x0060;      
   	EPwm1Regs.AQCTLB.all = 0x0060;         
   	EPwm2Regs.AQCTLA.all = 0x0060;         
   	EPwm2Regs.AQCTLB.all = 0x0060;        
   	EPwm3Regs.AQCTLA.all = 0x0060;         
   	EPwm3Regs.AQCTLB.all = 0x0060;     
   
   	EPwm1Regs.AQSFRC.bit.RLDCSF=3;		//load immediately
   	EPwm2Regs.AQSFRC.bit.RLDCSF=3;		//load immediately
   	EPwm3Regs.AQSFRC.bit.RLDCSF=3;		//load immediately   

	EPwm1Regs.AQCSFRC.bit.CSFA = 1;		//force low
	EPwm1Regs.AQCSFRC.bit.CSFB = 2;		//force high
	EPwm2Regs.AQCSFRC.bit.CSFA = 1;		//force low
	EPwm2Regs.AQCSFRC.bit.CSFB = 2;		//force high
	EPwm3Regs.AQCSFRC.bit.CSFA = 1;		//force low
	EPwm3Regs.AQCSFRC.bit.CSFB = 2;		//force high  

//PWM4A,PWM4B
  // Setup TBCLK
   EPwm4Regs.TBPRD = 0xFFFF;             // Set timer period 801 TBCLKs
   EPwm4Regs.TBPHS.half.TBPHS = 0x0000;  // Phase is 0
   EPwm4Regs.TBCTR = 0x0000;             // Clear counter
// Set Compare values
   EPwm4Regs.CMPA.half.CMPA = 0xFFFF;    // Set compare A value
   EPwm4Regs.CMPB = 0xFFFF;              // Set compare A value
// Setup counter mode
   EPwm4Regs.TBCTL.bit.CTRMODE = 2;      // Count up and down
   EPwm4Regs.TBCTL.bit.PHSEN = 0;        // Disable phase loading//10.15
   EPwm4Regs.TBCTL.bit.PRDLD = 0;		 // shadow
   EPwm4Regs.TBCTL.bit.SYNCOSEL = 1;     // CTR=0
   EPwm4Regs.TBCTL.bit.HSPCLKDIV = 1;    // Clock ratio to SYSCLKOUT/2
   EPwm4Regs.TBCTL.bit.CLKDIV = 0;
// Setup shadowing
   EPwm4Regs.CMPCTL.bit.SHDWAMODE = 0;   // Shadow mode
   EPwm4Regs.CMPCTL.bit.SHDWBMODE = 0;
   EPwm4Regs.CMPCTL.bit.LOADAMODE = 0;   // Load on period
   EPwm4Regs.CMPCTL.bit.LOADBMODE = 0;   
// Active high complementary PWMs - Setup the deadband
	EPwm4Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
    EPwm4Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
    EPwm4Regs.DBCTL.bit.IN_MODE = DBA_RED_DBB_FED;

//PWM5A,PWM5B
// Setup TBCLK
   EPwm5Regs.TBPRD = 0xFFFF;             // Set timer period 801 TBCLKs
   EPwm5Regs.TBPHS.half.TBPHS = 0x0000;  // Phase is 0
   EPwm5Regs.TBCTR = 0x0000;             // Clear counter
// Set Compare values
   EPwm5Regs.CMPA.half.CMPA = 0xFFFF;    // Set compare A value
   EPwm5Regs.CMPB = 0xFFFF;              // Set compare A value
// Setup counter mode
   EPwm5Regs.TBCTL.bit.CTRMODE = 2;      // Count up and down
   EPwm5Regs.TBCTL.bit.PHSEN = 1;        // enable phase loading
   EPwm5Regs.TBCTL.bit.PRDLD = 0;		 // shadow
   EPwm5Regs.TBCTL.bit.SYNCOSEL = 0;     // SYNCI
   EPwm5Regs.TBCTL.bit.HSPCLKDIV = 1;    // Clock ratio to SYSCLKOUT/2
   EPwm5Regs.TBCTL.bit.CLKDIV = 0;
// Setup shadowing
   EPwm5Regs.CMPCTL.bit.SHDWAMODE = 0;   // Shadow mode
   EPwm5Regs.CMPCTL.bit.SHDWBMODE = 0;
   EPwm5Regs.CMPCTL.bit.LOADAMODE = 0;   // Load on period
   EPwm5Regs.CMPCTL.bit.LOADBMODE = 0;   
// Active high complementary PWMs - Setup the deadband
   EPwm5Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
   EPwm5Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
   EPwm5Regs.DBCTL.bit.IN_MODE = DBA_RED_DBB_FED;
 
//PWM6A,PWM6B
// Setup TBCLK
   EPwm6Regs.TBPRD = 0xFFFF;             // Set timer period 801 TBCLKs
   EPwm6Regs.TBPHS.half.TBPHS = 0x0000;  // Phase is 0
   EPwm6Regs.TBCTR = 0x0000;             // Clear counter 
// Set Compare values
   EPwm6Regs.CMPA.half.CMPA = 0xFFFF;    // Set compare A value
   EPwm6Regs.CMPB = 0xFFFF;              // Set compare A value
// Setup counter mode
   EPwm6Regs.TBCTL.bit.CTRMODE = 2;      // Count up and down
   EPwm6Regs.TBCTL.bit.PHSEN = 1;        // enable phase loading
   EPwm6Regs.TBCTL.bit.PRDLD = 0;		 // shadow
   EPwm6Regs.TBCTL.bit.SYNCOSEL = 0;     // SYNCI
   EPwm6Regs.TBCTL.bit.HSPCLKDIV = 1;    // Clock ratio to SYSCLKOUT/2
   EPwm6Regs.TBCTL.bit.CLKDIV = 0;
// Setup shadowing
   EPwm6Regs.CMPCTL.bit.SHDWAMODE = 0;   // Shadow mode
   EPwm6Regs.CMPCTL.bit.SHDWBMODE = 0;
   EPwm6Regs.CMPCTL.bit.LOADAMODE = 0;   // Load on period
   EPwm6Regs.CMPCTL.bit.LOADBMODE = 0;   

// Active high complementary PWMs - Setup the deadband
   EPwm6Regs.DBCTL.bit.OUT_MODE = DB_FULL_ENABLE;
   EPwm6Regs.DBCTL.bit.POLSEL = DB_ACTV_HIC;
   EPwm6Regs.DBCTL.bit.IN_MODE = DBA_RED_DBB_FED;
       
// Set actions
// when the counter equals the active CMPA register,
//force EPWMxA output low  when the counter is decrementing.
//force EPWMxA output high when the counter is incrementing.   
   	EPwm4Regs.AQCTLA.all = 0x0060;              
   	EPwm4Regs.AQCTLB.all = 0x0060;      
   	EPwm5Regs.AQCTLA.all = 0x0060;         
   	EPwm5Regs.AQCTLB.all = 0x0060;        
   	EPwm6Regs.AQCTLA.all = 0x0060;        
   	EPwm6Regs.AQCTLB.all = 0x0060;          

   	EPwm4Regs.AQSFRC.bit.RLDCSF=3;		//load immediately
   	EPwm5Regs.AQSFRC.bit.RLDCSF=3;		//load immediately
   	EPwm6Regs.AQSFRC.bit.RLDCSF=3;		//load immediately  

	EPwm4Regs.AQCSFRC.bit.CSFA = 1;		//force low
	EPwm4Regs.AQCSFRC.bit.CSFB = 2;		//force high
	EPwm5Regs.AQCSFRC.bit.CSFA = 1;		//force low
	EPwm5Regs.AQCSFRC.bit.CSFB = 2;		//force high
	EPwm6Regs.AQCSFRC.bit.CSFA = 1;		//force low
	EPwm6Regs.AQCSFRC.bit.CSFB = 2;		//force high  

//set TZ 
   	EPwm1Regs.TZSEL.bit.OSHT1 = 1;//enable TZ1 as a one-shot trip source for this ePWM module
   	EPwm1Regs.TZSEL.bit.OSHT2 = 1;//-------TZ2-----------------------------------------------
	EPwm2Regs.TZSEL.bit.OSHT1 = 1;//enable TZ1 as a one-shot trip source for this ePWM module
   	EPwm2Regs.TZSEL.bit.OSHT2 = 1;//-------TZ2-----------------------------------------------
	EPwm3Regs.TZSEL.bit.OSHT1 = 1;//enable TZ1 as a one-shot trip source for this ePWM module
   	EPwm3Regs.TZSEL.bit.OSHT2 = 1;//-------TZ2-----------------------------------------------
	EPwm4Regs.TZSEL.bit.OSHT1 = 1;//enable TZ1 as a one-shot trip source for this ePWM module
   	EPwm4Regs.TZSEL.bit.OSHT2 = 1;//-------TZ2-----------------------------------------------
	EPwm5Regs.TZSEL.bit.OSHT1 = 1;//enable TZ1 as a one-shot trip source for this ePWM module
   	EPwm5Regs.TZSEL.bit.OSHT2 = 1;//-------TZ2-----------------------------------------------
	EPwm6Regs.TZSEL.bit.OSHT1 = 1;//enable TZ1 as a one-shot trip source for this ePWM module
   	EPwm6Regs.TZSEL.bit.OSHT2 = 1;//-------TZ2-----------------------------------------------

    EPwm1Regs.TZCTL.bit.TZA = 2;  //when a trip event occurs,force output ePWM A to a low state
    EPwm1Regs.TZCTL.bit.TZB = 2;  //-------------------------------------------B---------------
    EPwm2Regs.TZCTL.bit.TZA = 2;  //when a trip event occurs,force output ePWM A to a low state	
    EPwm2Regs.TZCTL.bit.TZB = 2;  //-------------------------------------------B---------------
	EPwm3Regs.TZCTL.bit.TZA = 2;  //when a trip event occurs,force output ePWM A to a low state
    EPwm3Regs.TZCTL.bit.TZB = 2;  //-------------------------------------------B---------------
	EPwm4Regs.TZCTL.bit.TZA = 2;  //when a trip event occurs,force output ePWM A to a low state
    EPwm4Regs.TZCTL.bit.TZB = 2;  //-------------------------------------------B---------------
	EPwm5Regs.TZCTL.bit.TZA = 2;  //when a trip event occurs,force output ePWM A to a low state
    EPwm5Regs.TZCTL.bit.TZB = 2;  //-------------------------------------------B---------------
	EPwm6Regs.TZCTL.bit.TZA = 2;  //when a trip event occurs,force output ePWM A to a low state
    EPwm6Regs.TZCTL.bit.TZB = 2;  //-------------------------------------------B---------------
// ------------------end of Initialize Epwm---------------------------------------------------------

// ------------------- Initialize ECAP--------------------------
/*
//cap1
	ECap1Regs.ECCTL1.bit.CAP1POL  = 0;	//DETECT RISING EDGE    0--rising 1--falling
//	ECap1Regs.ECCTL1.bit.CAP2POL  = 0;	//DETECT RISING EDGE
//	ECap1Regs.ECCTL1.bit.CAP3POL  = 0;	//reset    0-- 1--diffent mode
//	ECap1Regs.ECCTL1.bit.CAP4POL  = 0;	//DETECT RISING EDGE    0--rising 1--falling
	ECap1Regs.ECCTL1.bit.CTRRST1  = 1;	//DETECT RISING EDGE
//	ECap1Regs.ECCTL1.bit.CTRRST2  = 1;	//reset    0-- 1--diffent mode
//	ECap1Regs.ECCTL1.bit.CTRRST3  = 1;	//DETECT RISING EDGE
//	ECap1Regs.ECCTL1.bit.CTRRST4  = 1;	//reset    0-- 1--diffent mode
	ECap1Regs.ECCTL1.bit.CAPLDEN  = 1;	//enable load
	ECap1Regs.ECCTL1.bit.PRESCALE  = 0;	//0-- cet

	ECap1Regs.ECCTL2.bit.CAP_APWM  = 0;	//0-- cap mode
	ECap1Regs.ECCTL2.bit.SYNCI_EN = 0;	//dis sysc
	ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0;	//continous
	ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1;	//free run
	ECap1Regs.ECCTL2.bit.STOP_WRAP = 0;	//0-- wrap after cet1
//cap2
 	ECap2Regs.ECCLR.all = 0xFFFF;              // Clear all CAP interrupt flags
  	ECap2Regs.ECCTL1.bit.CAP1POL  = 0;	//DETECT RISING EDGE    0--rising 1--falling
	ECap2Regs.ECCTL1.bit.CAP2POL  = 0;	//DETECT RISING EDGE
//	ECap2Regs.ECCTL1.bit.CAP3POL  = 0;	//reset    0-- 1--diffent mode
//	ECap2Regs.ECCTL1.bit.CAP4POL  = 0;	//DETECT RISING EDGE    0--rising 1--falling
	ECap2Regs.ECCTL1.bit.CTRRST1  = 1;	//DETECT RISING EDGE
	ECap2Regs.ECCTL1.bit.CTRRST2  = 1;	//reset    0-- 1--diffent mode
//	ECap2Regs.ECCTL1.bit.CTRRST3  = 1;	//DETECT RISING EDGE
//	ECap2Regs.ECCTL1.bit.CTRRST4  = 1;	//reset    0-- 1--diffent mode
	ECap2Regs.ECCTL1.bit.CAPLDEN  = 1;	//enable load
	ECap2Regs.ECCTL1.bit.PRESCALE  = 0;	//0-- cet

	ECap2Regs.ECCTL2.bit.CAP_APWM  = 0;	//0-- cap mode
	ECap2Regs.ECCTL2.bit.SYNCI_EN = 0;	//dis sysc
	ECap2Regs.ECCTL2.bit.CONT_ONESHT = 0;	//continous
	ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;	//free run
	ECap2Regs.ECCTL2.bit.STOP_WRAP = 1;	//1-- wrap after cet2

	ECap3Regs.ECCTL1.bit.CAP1POL  = 0;	//DETECT RISING EDGE    0--rising 1--falling
//	ECap3Regs.ECCTL1.bit.CAP2POL  = 0;	//DETECT RISING EDGE
//	ECap3Regs.ECCTL1.bit.CAP3POL  = 0;	//reset    0-- 1--diffent mode
//	ECap3Regs.ECCTL1.bit.CAP4POL  = 0;	//DETECT RISING EDGE    0--rising 1--falling
	ECap3Regs.ECCTL1.bit.CTRRST1  = 1;	//DETECT RISING EDGE
//	ECap3Regs.ECCTL1.bit.CTRRST2  = 1;	//reset    0-- 1--diffent mode
//	ECap3Regs.ECCTL1.bit.CTRRST3  = 1;	//DETECT RISING EDGE
//	ECap3Regs.ECCTL1.bit.CTRRST4  = 1;	//reset    0-- 1--diffent mode
	ECap3Regs.ECCTL1.bit.CAPLDEN  = 1;	//enable load
	ECap3Regs.ECCTL1.bit.PRESCALE  = 0;	//0-- cet

	ECap3Regs.ECCTL2.bit.CAP_APWM  = 0;	//0-- cap mode
	ECap3Regs.ECCTL2.bit.SYNCI_EN = 0;	//dis sysc
	ECap3Regs.ECCTL2.bit.CONT_ONESHT = 0;	//continous
	ECap3Regs.ECCTL2.bit.TSCTRSTOP = 1;	//free run
	ECap3Regs.ECCTL2.bit.STOP_WRAP = 0;	//0-- wrap after cet1
*/
//cap4 --用于电压矢量定向
	ECap4Regs.ECCTL1.bit.CAPLDEN   = 1;		//enable load
	ECap4Regs.ECCTL1.bit.PRESCALE  = 0;		//0-cet prescale
	ECap4Regs.ECCTL1.bit.CAP1POL   = 0;		//DETECT RISING EDGE    0--rising 1--falling
	ECap4Regs.ECCTL1.bit.CTRRST1   = 1;		//reset counter after caupured 

	ECap4Regs.ECCTL2.bit.CAP_APWM    = 0;	//0-- cap mode
	ECap4Regs.ECCTL2.bit.SYNCI_EN    = 0;	//dis sysc
	ECap4Regs.ECCTL2.bit.CONT_ONESHT = 0;	//continous
	ECap4Regs.ECCTL2.bit.TSCTRSTOP   = 1;	//free running
	ECap4Regs.ECCTL2.bit.STOP_WRAP   = 0;	//0-- wrap after detect 

//CAP5 --用于相序检测
	ECap5Regs.ECCTL1.bit.CAPLDEN   = 1;		//enable load
	ECap5Regs.ECCTL1.bit.PRESCALE  = 0;		//0-cet prescale
	ECap5Regs.ECCTL1.bit.CAP1POL   = 0;		//DETECT RISING EDGE    0--rising 1--falling
	ECap5Regs.ECCTL1.bit.CTRRST1   = 1;		//reset counter after caupured 

	ECap5Regs.ECCTL2.bit.CAP_APWM    = 0;	//0-- cap mode
	ECap5Regs.ECCTL2.bit.SYNCI_EN    = 0;	//dis sysc
	ECap5Regs.ECCTL2.bit.CONT_ONESHT = 0;	//continous
	ECap5Regs.ECCTL2.bit.TSCTRSTOP   = 1;	//free running
	ECap5Regs.ECCTL2.bit.STOP_WRAP   = 0;	//0-- wrap after detect 

//CAP6 --用于相序检测
	ECap6Regs.ECCTL1.bit.CAPLDEN   = 1;		//enable load
	ECap6Regs.ECCTL1.bit.PRESCALE  = 0;		//0-cet prescale
	ECap6Regs.ECCTL1.bit.CAP1POL   = 0;		//DETECT RISING EDGE    0--rising 1--falling
	ECap6Regs.ECCTL1.bit.CTRRST1   = 1;		//reset counter after caupured 

	ECap6Regs.ECCTL2.bit.CAP_APWM    = 0;	//0-- cap mode
	ECap6Regs.ECCTL2.bit.SYNCI_EN    = 0;	//dis sysc
	ECap6Regs.ECCTL2.bit.CONT_ONESHT = 0;	//continous
	ECap6Regs.ECCTL2.bit.TSCTRSTOP   = 1;	//free running
	ECap6Regs.ECCTL2.bit.STOP_WRAP   = 0;	//0-- wrap after detect 
// -------------------End Of Initialize ECAP--------------------------

//------正交编码QEP设置----------------------------------------
//设置QEP模块工作模式
//设置解码计数控制寄存器
	EQep2Regs.QPOSMAX           = 4 * 2048;		// 位置计数器最大值
//	EQep2Regs.QPOSMAX           = 4 * _SC_PLSPRVL;	// 位置计数器最大值 ???201005atcpc
	EQep2Regs.QDECCTL.bit.QSRC  = 0;			//正交计数模式
	EQep2Regs.QDECCTL.bit.SOEN  = 0;			//禁止比较输出
	EQep2Regs.QDECCTL.bit.SWAP  = 0;            //不交换A、B输入信号，交换的话可以改变计数方向，相当于改变转向
	EQep2Regs.QDECCTL.bit.IGATE = 0;			//禁止周期信号对齐

//设置QEP控制寄存器
	EQep2Regs.QEPCTL.bit.PCRM = 0;		//位置计数器在周期信号到来后复位
	EQep2Regs.QEPCTL.bit.QPEN = 1;		//使能位置计数器
	EQep2Regs.QEPCTL.bit.WDE  = 0;		//禁止qep看门狗计数器
	//EQep2Regs.QEPCTL.bit.IEL  = 1;	//Index event latch; Latches position counter on rising edge of the index signal
										//20090804CPC.该bit配置将被忽略，由于EQep2Regs.QEPCTL.bit.PCRM = 0.手册是这样陈述的
//-----测周与测频法共用20090817------------
	EQep2Regs.QEPCTL.bit.UTE  = 1;		//使能单位时间计数器，时钟为sysclkout，
	EQep2Regs.QUPRD		= 1500000;      //单位时间计数周期1.5M
	EQep2Regs.QEPCTL.bit.QCLM = 1;      //QEP CAP 加载设置 Latch on unit time out	

//设置QCAP控制寄存器
//	EQep2Regs.QCAPCTL.bit.CEN  = 0;		//禁止QCAP单元 为修改switching CAPCLK prescaling mode
//	EQep2Regs.QCAPCTL.bit.CCPS = 7;		//系统时钟预分频设置，采用128分频系统时钟 /128  20090817 注:修改SYSCLK分频要修改DELAY_QCAPSPDIN
//	EQep2Regs.QCAPCTL.bit.UPPS = 9;		//对QCLK进行512分频  20090817
//	EQep2Regs.QCAPCTL.bit.CEN  = 1;		//使能QCAP单元

//-----测周与测频法共用20090817------------
	EQep2Regs.QCAPCTL.bit.CEN  = 0;		//禁止QCAP单元 为修改switching CAPCLK prescaling mode
	EQep2Regs.QCAPCTL.bit.CCPS = 7;		//系统时钟预分频设置，采用128分频系统时钟 /128  20090817 注:修改SYSCLK分频要修改DELAY_QCAPSPDIN
	EQep2Regs.QCAPCTL.bit.UPPS = 6;		//对QCLK进行64分频  20090817newchange
	EQep2Regs.QCAPCTL.bit.CEN  = 1;		//使能QCAP单元

// -------------------------Initialize SPI---------------------------
	SpiaRegs.SPIBRR=0x0009;           		// Baud rate 3.75M
	SpiaRegs.SPIPRI.all=0x0000;				// stop immediately
											//--DA7614工作时SPI的设置: 无延迟的下降沿方式(时钟的初始态为高),16 bit data
	SpiaRegs.SPICCR.all=0x004F;       		// initialize spi,output at falling edge, 16 bit data
	SpiaRegs.SPICTL.all=0x0006;       		// Master,normal phase, enable transmission
    SpiaRegs.SPIFFTX.bit.SPIFFENA=1;
    SpiaRegs.SPIFFTX.bit.SPIRST=1;
	SpiaRegs.SPIFFCT.all=2;
    SpiaRegs.SPICCR.all=0x00CF;       		// enable spi,output at falling edge, 16 bit  data

    		
// -------------------End Of Initialize SPI--------------------------	
	
// -------------------INITIAL SCI------------------------------------
/*
// scib_fifo_init_CANOPEN
	ScibRegs.SCICCR.all =0x0007;   			// 1 stop bit,No loopback 
                                  			// no parity,8 char bits
	ScibRegs.SCICTL1.all =0x0003;  			// Disable TX, RX, internal SCICLK, 
                                  			// Disable RX_ERR, SLEEP, TXWAKE
	ScibRegs.SCICTL2.all =0;		 		// fifo mode,they are ignored 

	ScibRegs.SCIHBAUD = 0x00;				// 19200bps
   	ScibRegs.SCILBAUD = 0xF3;
   	
   	ScibRegs.SCIFFTX.all=0xC000;			// disable tx232_int,reset scia_fifo 
    ScibRegs.SCIFFRX.all=0x000A;			// disable rx232_int 
    ScibRegs.SCIFFCT.all=0x00;
    
	ScibRegs.SCICTL1.all =0x0023;     		// Relinquish SCI from Reset 

	ScibRegs.SCIFFTX.bit.TXFIFOXRESET=1;
	ScibRegs.SCIFFRX.bit.RXFIFORESET=1; 


// scia_fifo_init	
	SciaRegs.SCICCR.all =0x0007;    		// 1 stop bit, No loopback 
                                   			// no parity,8 char bits
   	SciaRegs.SCICTL1.all =0x0001;   		// Disable TX, enable RX, internal SCICLK, 
                                   			// Disable RX ERR, SLEEP, TXWAKE
  	SciaRegs.SCICTL2.all =0;				// fifo mode,they are ignored
   	SciaRegs.SCIHBAUD =0x01;				// 9600 bps
   	SciaRegs.SCILBAUD =0xE7;
   	
   	SciaRegs.SCIFFTX.all=0xC000;			// disable tx485_int
   	SciaRegs.SCIFFRX.all=0x000A;			// disable rx485_int
   	SciaRegs.SCIFFCT.all=0x00;
	
	
  	SciaRegs.SCICTL1.all =0x0021;     		// Relinquish SCI from Reset
  	SciaRegs.SCIFFTX.bit.TXFIFOXRESET=1;
	SciaRegs.SCIFFRX.bit.RXFIFORESET=1;
*/
// scib_fifo_init RS485-PC	//20100329at27
	ScibRegs.SCICCR.all =0x0007;   			// 1 stop bit,No loopback 
                                  			// no parity,8 char bits
	ScibRegs.SCICTL1.all =0x0001;   		// Disable TX, enable RX, internal SCICLK, 
                                  			// Disable RX_ERR, SLEEP, TXWAKE
	ScibRegs.SCICTL2.all =0;		 		// fifo mode,they are ignored 

	ScibRegs.SCIHBAUD = 0x00;				// 19200 bps //20100727
   	ScibRegs.SCILBAUD = 0xF3;
   	
   	ScibRegs.SCIFFTX.all=0xC000;			// disable tx232_int,reset scia_fifo 
    ScibRegs.SCIFFRX.all=0x000A;			// disable rx232_int 
    ScibRegs.SCIFFCT.all=0x00;
    
	ScibRegs.SCICTL1.all =0x0021;     		// Relinquish SCI from Reset 

	ScibRegs.SCIFFTX.bit.TXFIFOXRESET=1;
	ScibRegs.SCIFFRX.bit.RXFIFORESET=1;
	
// scia_fifo_init RS232-CANOPEN	//20100329at27
	SciaRegs.SCICCR.all =0x0007;    		// 1 stop bit, No loopback 
                                   			// no parity,8 char bits
   	SciaRegs.SCICTL1.all =0x0003;  			// Enable TX, RX, internal SCICLK, 
                                   			// Disable RX ERR, SLEEP, TXWAKE
  	SciaRegs.SCICTL2.all =0;				// fifo mode,they are ignored
   	SciaRegs.SCIHBAUD =0x00;				// 57600 bps 20120507CPC	原19200 bps
//	SciaRegs.SCILBAUD =0xF3;
	SciaRegs.SCILBAUD =0x4F;
//	SciaRegs.SCILBAUD =0x79;				//38400bps 20121024sp
   	
   	SciaRegs.SCIFFTX.all=0xC000;			// disable tx485_int
   	SciaRegs.SCIFFRX.all=0x000A;			// disable rx485_int
   	SciaRegs.SCIFFCT.all=0x00;
	
	
  	SciaRegs.SCICTL1.all =0x0023;     		// Relinquish SCI from Reset
  	SciaRegs.SCIFFTX.bit.TXFIFOXRESET=1;
	SciaRegs.SCIFFRX.bit.RXFIFORESET=1; 
//-------------------------------------------------------------------	
	EDIS;

}




//===========================================================================
// No more.
//===========================================================================
