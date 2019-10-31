/****************************************Copyright (c)**************************************************
**                       		     ��	��	��	ͨ	��	ѧ
**                                        ��������ѧԺ
**                                         604ʵ����
**
**                              
** 
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: user_interface.c
**��   ��   ��: 
**����޸�����: 
**��        ��: ����1.5MW˫�������鲢���������������������Ƴ���--201005atcpc-201007atbjtu
				������eeprom,sci,spi,ad,da�Ŀ��Ƴ���
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
#include "math.h"				//20121103
//��������
Uint16 		CheckCode(Uint16 index);
Uint16 		SciDatpro(void);
/*********************************************************************************************************
** ��������: EeStart
** ��������: ��ʼ��eeprom�Ĳ���
** �䡡��: 
** �䡡��:        
** ע  ��: 	 ʱ���߸�ʱ�������½���Ϊ��ʼ
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeStart(void)
{
	EALLOW;
    GpioDataRegs.GPBSET.bit.GPIO32 = 1;     	 //���ݸ�
	GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;  		//�����߱�Ϊ�����
	EDIS;
	DELAY_US(DELAY_EE);
    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//ʱ�Ӹ�
	DELAY_US(DELAY_EE);
	GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;		 	//���ݵ�
	DELAY_US(DELAY_EE);
   	GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//ʱ�ӵ�
	DELAY_US(DELAY_EE);
}
/*********************************************************************************************************
** ��������: EeStop
** ��������: ������eeprom�Ĳ���
** �䡡��: 
** �䡡��:        
** ע  ��: 	 ʱ���߸�ʱ������������Ϊ����
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeStop(void)
{
	EALLOW;
    GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;     	 //���ݵ�
	GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;  		//�����߱�Ϊ�����
	EDIS;
	DELAY_US(DELAY_EE);
    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//ʱ�Ӹ�
	DELAY_US(DELAY_EE);
    GpioDataRegs.GPBSET.bit.GPIO32 = 1;     	 //���ݸ�
	DELAY_US(DELAY_EE);
   	GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//ʱ�ӵ�
	EALLOW;
	GpioCtrlRegs.GPBDIR.bit.GPIO32= 0;			//�����߱�Ϊ�����
	EDIS;
	DELAY_US(DELAY_EE);
}

/*********************************************************************************************************
** ��������: EeWrite
** ��������: ��number���ֽ����ݣ���������ַ������д�뵽eeprom��һ��Ҫ����ͬһҳ�棩
** �䡡��: 	 number,��ʾҪд���ֽ���
** �䡡��:        
** ע  ��: 	 EEPROM.data[0]:д������;
**			 EEPROM.data[1-2]:��д���ݵ�ַ; 
**			 EEPROM.data[3-x]:��д����;
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeWrite(unsigned char number)
{
	int16 i,j;
//----------------------------------------------//������ʼλ
	EeStart();
//----------------------------------------------//��ʼ��������
	for(j=0;j<number+3;j++)						//�ȷ����ֽ�
	{
		for(i=0;i<8;i++)						//ÿ���ֽ��ȷ�����λ
		{
			if((EEPROM.data[j] & ONEBYTE[i])==0)	//Ҫ��0
			{
				GpioDataRegs.GPBCLEAR.bit.GPIO32 =1;	//���ݵ�
				DELAY_US(DELAY_EE);					
    			GpioDataRegs.GPBSET.bit.GPIO33 = 1; //ʱ�Ӹ�
				DELAY_US(DELAY_EE);				
   	   		    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;//ʱ�ӵ�
				DELAY_US(DELAY_EE);				
			}
			else									//Ҫ��1
			{
				GpioDataRegs.GPBSET.bit.GPIO32 =1;	//���ݸ�
				DELAY_US(DELAY_EE);				
   				GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//ʱ�Ӹ�
				DELAY_US(DELAY_EE);				
   	   		    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//ʱ�ӵ�
				DELAY_US(DELAY_EE);				
			}
		}
//----------------------------------------------//����һ���ֽ�		
		EALLOW;
		GpioCtrlRegs.GPBDIR.bit.GPIO32= 0;		//�����߱�Ϊ�����
		EDIS;
    	GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//ʱ�Ӹ�
		DELAY_US(DELAY_EE);
		
		if(GpioDataRegs.GPBDAT.bit.GPIO32==1)	//��������߶���1��ʾû��Ӧ��
		{
			M_SetFlag(SL_EE_NOACK);				//����Ӧ���־
		}
		
   	    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//ʱ�ӵ�

		EALLOW;
		GpioDataRegs.GPBCLEAR.bit.GPIO32= 1;		//���ݵ�
		GpioCtrlRegs.GPBDIR.bit.GPIO32= 1;		//�����߱�Ϊ�����
		EDIS;
		DELAY_US(DELAY_EE);
	}
//----------------------------------------------//����ֹͣλ
	EeStop();
}

/*********************************************************************************************************
** ��������: EeRead
** ��������: ��eeprom��������number���ֽ�����
** �䡡��: 	 number,��ʾҪ�����ֽ���
** �䡡��:   EEPROM.data[0-1]:����������     
** ע  ��: 	 ��дд������,��д�������ݵ�ַ,��д��������,�ڶ�������
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeRead(unsigned char number)
{
	int16 i,j;
	
//----------------------------------------------//������ʼλ
	EeStart();
//----------------------------------------------//��ʼ���͵�ַ����
	for(j=0;j<3;j++)							//�ȷ����ֽ�
	{
		for(i=0;i<8;i++)						//ÿ���ֽ��ȷ�����λ
		{
			if((EEPROM.data[j] & ONEBYTE[i])==0)	//Ҫ��0
			{
				GpioDataRegs.GPBCLEAR.bit.GPIO32 =1;	//���ݵ�
				DELAY_US(DELAY_EE);				
   				GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//ʱ�Ӹ�
				DELAY_US(DELAY_EE);				
   	    		GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//ʱ�ӵ�
				DELAY_US(DELAY_EE);				
			}
			else									//Ҫ��1
			{
				GpioDataRegs.GPBSET.bit.GPIO32 =1;	//���ݸ�
				DELAY_US(DELAY_EE);				
    			GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//ʱ�Ӹ�
				DELAY_US(DELAY_EE);				
   	   		    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//ʱ�ӵ�
				DELAY_US(DELAY_EE);				
			}
		}
//----------------------------------------------//����һ���ֽ�		
		EALLOW;
		GpioCtrlRegs.GPBDIR.bit.GPIO32= 0;		//�����߱�Ϊ�����
		EDIS;
   	    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//ʱ�Ӹ�
		DELAY_US(DELAY_EE);
		
		if(GpioDataRegs.GPBDAT.bit.GPIO32==1)	//��������߶���1��ʾû��Ӧ��
		{
			M_SetFlag(SL_EE_NOACK);				//����Ӧ���־
		}
		
   	    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//ʱ
		
		EALLOW;
		GpioDataRegs.GPBCLEAR.bit.GPIO32 =1;		//���ݵ�
		GpioCtrlRegs.GPBDIR.bit.GPIO32= 1;		//�����߱�Ϊ�����
		EDIS;
		
		DELAY_US(DELAY_EE);
	}
	
//----------------------------------------------//�ٴη�����ʼλ
	EeStart();
//----------------------------------------------//�ٴη�����ʼλ
	EEPROM.data[0] |= 0x01;						//��Ϊ��ָ��
	for(i=0;i<8;i++)							//�ȷ�����λ
	{
		if((EEPROM.data[0] & ONEBYTE[i])==0)		//Ҫ��0
		{
			GpioDataRegs.GPBCLEAR.bit.GPIO32 =1;		//���ݵ�
			DELAY_US(DELAY_EE);					
   		    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//ʱ�Ӹ�
			DELAY_US(DELAY_EE);					
   	  	    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//ʱ�ӵ�
			DELAY_US(DELAY_EE);					
		}
		else										//Ҫ��1
		{
			GpioDataRegs.GPBSET.bit.GPIO32 =1;		//���ݸ�	
			DELAY_US(DELAY_EE);					
   		    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//ʱ�Ӹ�
			DELAY_US(DELAY_EE);					
   	        GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//ʱ�ӵ�
			DELAY_US(DELAY_EE);					
		}
	}
//----------------------------------------------//����һ���ֽ�
	EALLOW;
	GpioCtrlRegs.GPBDIR.bit.GPIO32 =0;			//�����߱�Ϊ�����
	EDIS;
    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//ʱ�Ӹ�
	DELAY_US(DELAY_EE);
	
	if(GpioDataRegs.GPBDAT.bit.GPIO32==1)		//��������߶���1��ʾû��Ӧ��
	{
		M_SetFlag(SL_EE_NOACK);					//����Ӧ���־
	}
	
   	GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//ʱ�ӵ�
	DELAY_US(DELAY_EE);
//----------------------------------------------//��ʼ������
	for(j=0;j<number;j++)						//�ȶ����ֽ�
	{
		EEPROM.data[j]=0;
		for(i=0;i<8;i++)						//ÿ���ֽ��ȶ���λ
		{
   		    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//ʱ�Ӹ�
			DELAY_US(DELAY_EE);	
			if(GpioDataRegs.GPBDAT.bit.GPIO32==1)//����Ϊ1
			{
				EEPROM.data[j] |= ONEBYTE[i];	//Ϊ���򲻱�
			}
   	   	    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//ʱ�ӵ�
			DELAY_US(DELAY_EE);
		}
//----------------------------------------------//����һ���ֽ�
		if(j!=number-1)							//���һ���ֽڲ�����Ӧ��
		{
			EALLOW;
			GpioDataRegs.GPBCLEAR.bit.GPIO32 =1;	//�������Ӧ��
			GpioCtrlRegs.GPBDIR.bit.GPIO32= 1;	//������Ϊ�����
			EDIS;
			DELAY_US(DELAY_EE);
		}
   	    GpioDataRegs.GPBSET.bit.GPIO33 = 1;     	//ʱ�Ӹ�
		DELAY_US(DELAY_EE);
   	    GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1;     	//ʱ�ӵ�
		EALLOW;
		GpioCtrlRegs.GPBDIR.bit.GPIO32= 0;		//������Ϊ�����
		EDIS;
		DELAY_US(DELAY_EE);		
	}
//----------------------------------------------//�������,����ֹͣλ
	EeStop();
}
/*********************************************************************************************************
** ��������: EeWpre
** ��������: ׼��д��EEPROM�ĵ�ַ������
** �䡡��: 	 index:��д����������
** �䡡��:   EEPROM.data[0-4]:д�����֡���д���ݵĵ�ַ�ʹ�д����    
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeWpre(unsigned char index)
{
	EEPROM.data[0]=0xA0;									//Slave ADdress
	EEPROM.data[1]=((index<<1)&0xFF00)>>8;					//MSB ADdress
	EEPROM.data[2]=(index<<1)&0xFF;							//LSB ADdress
	EEPROM.data[3]=*(FUNC[index].para_add) & 0x00ff;		//��8λ����
	EEPROM.data[4]=(*(FUNC[index].para_add) & 0xff00)>>8;	//��8λ����
}
/*********************************************************************************************************
** ��������: EeRpre
** ��������: ׼����ȡEEPROM���ݵĵ�ַ
** �䡡��: 	 index:����ȡ���������
** �䡡��:   EEPROM.data[0-2]:�������ֺʹ���ȡ���ݵĵ�ַ    
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeRpre(unsigned char index)
{
	EEPROM.data[0]=0xA0;							//Slave ADdress
	EEPROM.data[1]=((index<<1)&0xFF00)>>8;			//MSB ADdress
	EEPROM.data[2]=(index<<1)&0xFF;					//LSB ADdress
}
/*********************************************************************************************************
** ��������: EeWrword
** ��������: дһ���ֵ����ݵ�EEPROM��������У��
** �䡡��: 	 index:��д���������
** �䡡��:   
** ע  ��: 	 �Ƚ���д����д��eeprom�ٶ���������
**			 ������鲻�����ñ�־λSL_EE_FAIL
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeWrword(unsigned char index)
{
	Uint16 data_rd;
	
	EeWpre(index);									
	EeWrite(2);											//д��2�ֽ�����
	EeRead(2);											//��2�ֽ����ݶ���
	
	data_rd=(EEPROM.data[1]<<8) | EEPROM.data[0];		//data_rdΪ����������
	
	if(data_rd!=*(FUNC[index].para_add))
		M_SetFlag(SL_EE_FAIL);
}
/*********************************************************************************************************
** ��������: InitEeprom
** ��������: ��ʼ��eeprom
** �䡡��: 	 
** �䡡��:   
** ע  ��: 	 �Ƚ�eeprom�е�ԭ�����ݶ��������м���
**			 ����������ݴ�����eeprom������ȫ����ʼ��
**			 ����ڳ�ʼ���������Ƿ����eeprom��������
**			 ���û�д�������SL_CODEOK��ʾeeprom��������,������SL_CODEOK��ʾeeprom��������
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void InitEeprom(void)
{
	Uint16 i,j;
//	Uint16 j;
	if(DEBUG_OPER==0)
	{
//----------------------------------------------//��eeprom������ȫ������ram
		for(i=0;i<PARA_NUM;i++)					
		{
			EeRpre(i);								//�����ֺ͵�ַ����EEPROM.data[]
			EeRead(2);
												//���������ݷ���RAM
			*FUNC[i].para_add=(EEPROM.data[1]<<8) | EEPROM.data[0];	
		
			if(CheckCode(i)==1)						//������ķ���ֵ=1���ʾ�д���
			{
				for(j=0;j<PARA_NUM;j++)
				{
					*FUNC[j].para_add=FUNC[j].init;	//RAM���ݻָ���ֵ
					EeWrword(j);					//д��2�ֽ�����
				}
				break;								//����У���д�������
			}
		}
	}
	else
	{
//-----------------------------------------//����ʱֱ�ӽ�EEPROM��ʼ��!!!
		for(j=0;j<PARA_NUM;j++)
		{
			*FUNC[j].para_add=FUNC[j].init;	//RAM���ݻָ���ֵ
			EeWrword(j);					//д��2�ֽ�����
		}
	}
//-----------------------------------------	
	if(M_ChkFlag(SL_EE_FAIL)==0)					
		M_SetFlag(SL_CODEOK);					//EEPROM����
	else
		M_ClrFlag(SL_CODEOK);					//EEPROM����
}
/*********************************************************************************************************
** ��������: CheckCode
** ��������: ����eeprom�������Ƿ���ȷ
** �䡡��: 	 index,���������ݵ����
** �䡡��:   j,j=1��ʾ���ݴ���;j=0��ʾ������ȷ
** ע  ��: 	 ���������Ƿ���ָ���ķ�Χ֮��
**			 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
Uint16 CheckCode(Uint16 index)						
{
	Uint16 j,data,max,min;
	int16  temp,max_temp,min_temp;
	
	data=*FUNC[index].para_add;
//----------------------------------------------//�жϸ����ݵ�����	
	if((FUNC[index].attr & MAX_PT)==1)			//�������ֵָ������ 
		max=*FUNC[index].max_add;
	else										//���������ֵָ������ 
		max=FUNC[index].max;					//��������������ֵ
//----------------------------------------------//������Сֵָ������			
	if((FUNC[index].attr & MIN_PT)==1)
		min=*FUNC[index].min_add;
	else 										//��������Сֵָ������
		min=FUNC[index].min;					//�������������Сֵ
//----------------------------------------------//��ʼУ��
	j=0;										//���޹��ϱ�־
	if((FUNC[index].attr & SIG)==0)				//�޷�����
	{
		if(data>max)				
			j=1;								//�������ֵ���д�
		else if(data<min)						//С����Сֵ
		{
			if((FUNC[index].attr & VA0)==0)		
				j=1;							//�ֲ�����VA0���ԣ��д�
			else if(data!=0)					//����VA0���Ե��ֲ�Ϊ�㣬�д�
				j=1;
		}
	}					
	else										//�з�����
	{
		temp=(int)data;							//ȫ��תΪ�з�����
		max_temp=(int)max;
		min_temp=(int)min;
		if(temp>max_temp)						//�������ֵ���д�
			j=1;
		else if(temp<min_temp)					//С����Сֵ���д�
			j=1;								//�з�����û��VA0����
	}
//----------------------------------------------//����ֵΪj	
	return j;
}
/*********************************************************************************************************
** ��������: EeCtrl
** ��������: �������й����ж�eeprom���ݵĲ���
** �䡡��: 	 
** �䡡��:   
** ע  ��: 	 ��eeprom��ȷ��ǰ���£�����־λ
**			 SL_INIEE:		�Ƿ���Ҫ���г�ʼ��eeprom����;
							���������SL_EEBUSY_INIEE,ȫ����ʼ�����Ժ�����SL_INIEE��SL_EEBUSY_INIEE
**			 SL_MCODE:		�Ƿ���Ҫ�޸Ĺ�����ֵ
							���������SL_EEBUSY_MCODE,��EEPROM.mcode��ָ��������д��eeprom
							������ɺ�����SL_MCODE��SL_EEBUSY_MCODE
**			 SL_ERRSAVE:	�Ƿ���Ҫ���������Ϣ
							���������SL_EEBUSY_ERRSAVE,��TAB_ERR�е�����д��eeprom
							������ɺ�����SL_ERRSAVE��SL_EEBUSY_ERRSAVE
**			 SL_POFSAVE:	�Ƿ���Ҫ���������Ϣ
							���������SL_EEBUSY_POFSAVE,��TAB_POF�е�����д��eeprom
							������ɺ�����SL_POFSAVE��SL_EEBUSY_POFSAVE
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void EeCtrl(void)
{
	if(M_ChkFlag(SL_EE_FAIL)==0)					//EEPROM�޹���?
	{
//----------------------------------------------//�ָ���������������
		if(M_ChkFlag(SL_EEBUSY_INI)!=0)			//RAM���ݻָ���ֵ
		{
			*FUNC[EEPROM.point].para_add=FUNC[EEPROM.point].init;	
			EeWrword(EEPROM.point);				//д��2�ֽ�����
			EEPROM.point++;
			
			if(EEPROM.point>=PARA_NUM)			//�������?
			{
				EEPROM.point=0;
				M_ClrFlag(SL_EEBUSY_INI);
				M_ClrFlag(SL_EEASK_INI);
			}
		}
//----------------------------------------------//�޸Ĺ����������
		else if(M_ChkFlag(SL_EEBUSY_MCODE)!=0)
		{
			EeWrword(EEPROM.mcode);				//д��2�ֽ�����
			M_ClrFlag(SL_EEBUSY_MCODE);
			M_ClrFlag(SL_EEASK_MCODE);
		}
//----------------------------------------------//���������Ϣ������
		else if(M_ChkFlag(SL_EEBUSY_ERRSAVE)!=0)
		{
			EeWrword(TAB_ERR[EEPROM.point]);	//д��2�ֽ�����
			EEPROM.point++;
			
			if(EEPROM.point>=ERRO_NUM)			//�������?
			{
				EEPROM.point=0;
				M_ClrFlag(SL_EEBUSY_ERRSAVE);
				M_ClrFlag(SL_EEASK_ERRSAVE);
			}
		}
//----------------------------------------------//�Ƿ���ڱ��������Ϣ����
/*
		else if(M_ChkFlag(SL_EEBUSY_POFSAVE)!=0)
		{
			EeWrword(TAB_POF[EEPROM.point]);	//д��2�ֽ�����
			EEPROM.point++;
			
			if(EEPROM.point>=POFF_NUM)		//�������?
			{
				EEPROM.point=0;
				M_ClrFlag(SL_EEBUSY_POFSAVE);
				M_ClrFlag(SL_EEASK_POFSAVE);
			}
		}		
*/
//----------------------------------------------//EEPROMû�в���������
		else
		{
			EEPROM.point=0;
			if(M_ChkFlag(SL_EEASK_INI)!=0)			//�Ƿ���ڻָ�������������
				M_SetFlag(SL_EEBUSY_INI);
			else if(M_ChkFlag(SL_EEASK_MCODE)!=0)	//�Ƿ�����޸Ĺ���������
				M_SetFlag(SL_EEBUSY_MCODE);
			else if(M_ChkFlag(SL_EEASK_ERRSAVE)!=0)	//�Ƿ���ڱ��������Ϣ����
				{M_SetFlag(SL_EEBUSY_ERRSAVE);
//				     M_SetFlag(SL_PHASEA);          //����Saveռ��ʱ��,����DSP����T1���� 20090803
//    				*OUT3_ADDR = _OUT3_DATA;		//����Saveռ��ʱ��,����DSP����T1���� 20090803
				}
//			M_ClrFlag(SL_PHASEA);           //����CPUռ����,����DSP����T1����
//    		*OUT3_ADDR = _OUT3_DATA;		//����Saveռ��ʱ��,����DSP����T1���� 20090803



//			else if(M_ChkFlag(SL_EEASK_POFSAVE)!=0)	//�Ƿ���ڱ��������Ϣ����
//				M_SetFlag(SL_EEBUSY_POFSAVE);
		}
	}
}
/*********************************************************************************************************
** ��������: SetRtimer
** ��������: ʵʱʱ���趨
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
void SetRtimer(void)
{
	Uint16 temp;
	
//----------------------------------------------//д��ʱ��ֵ	
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x02;						//STATUS register
	EeWrite(1);									//д����
	
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x06;						//STATUS register
	EeWrite(1);									//д�Ĵ�������
	
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x30;						//LSB ADdress
	
	temp=RTIMER.time[0]/10;
	EEPROM.data[3]=(RTIMER.time[0]-temp*10)|(temp<<4);	//��
	
	
	temp=RTIMER.time[1]/10;
	EEPROM.data[4]=(RTIMER.time[1]-temp*10)|(temp<<4);	//��
	
	temp=RTIMER.time[2]/10;
	EEPROM.data[5]=0x80 | ((RTIMER.time[2]-temp*10)|(temp<<4));	//ʱ
	
	temp=RTIMER.time[3]/10;
	EEPROM.data[6]=(RTIMER.time[3]-temp*10)|(temp<<4);	//��
	
	temp=RTIMER.time[4]/10;
	EEPROM.data[7]=(RTIMER.time[4]-temp*10)|(temp<<4);	//��
	
	temp=RTIMER.time[5]/10;
	EEPROM.data[8]=(RTIMER.time[5]-temp*10)|(temp<<4);	//��
	
	EEPROM.data[9]=0x05;						//����
	EEPROM.data[10]=0x20;						//19/20
	EeWrite(8);									//д��ʱ��ֵ

	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x0;							//STATUS register
	EeWrite(1);									//��ֹд��
}
/*********************************************************************************************************
** ��������: RtRead
** ��������: ʵʱʱ�Ӷ�ȡ
** �䡡��: 	 
** �䡡��:   RTIMER.time[0~5]--[�� �� ʱ �� �� ��]
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void RtRead(void)
{
	Uint16 i;

	if(M_ChkFlag(SL_SETRTIMER)!=0)				//��ҪУ��ʵʱʱ��?
	{
		SetRtimer();
		M_ClrFlag(SL_SETRTIMER);
	}
	else
	{
		//----------------------------------------------//��ȡʱ��ֵ��data[0~7]
		EEPROM.data[0]=0xDE;						//Slave ADdress
		EEPROM.data[1]=0;							//MSB ADdress
		EEPROM.data[2]=0x30;						//LSB ADdress
		EeRead(8);
		//----------------------------------------------
		M_ClrBit(EEPROM.data[2],0x80);				//���24Сʱ����λ
	
		for(i=0;i<6;i++)
		{
			RTIMER.time[i]=((EEPROM.data[i] & 0xF0)>>4)*10+(EEPROM.data[i] & 0x0F);
		}
	}
}
/*********************************************************************************************************
** ��������: InitRtimer
** ��������: ʵʱʱ�ӳ�ʼ��
** �䡡��: 	 
** �䡡��:   
** ע  ��: 	 ֻ��ʵʱʱ�ӳ�ʼ����ʱ����Ҫ�趨��ʱ��ֵ�趨����Ӧλ��
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void InitRtimer(void)
{
//----------------------------------------------//д�������
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x0;							//STATUS register
	EeWrite(1);									//��ֹд��

	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x02;						//STATUS register
	EeWrite(1);									//д����

	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x06;						//STATUS register
	EeWrite(1);									//д�Ĵ�������

	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x10;						//LSB ADdress
	EEPROM.data[3]=0x0;							//Control register 0
	EEPROM.data[4]=0x0;							//Control register 1
	EEPROM.data[5]=0x0;							//Control register 2
	EEPROM.data[6]=0x0;							//Control register 3
	EeWrite(4);									//д��4�ֽڿ�����
	
	DELAY_US(10000L);							//��ʱ10ms

//----------------------------------------------//д��ʱ��ֵ	
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x0;							//STATUS register
	EeWrite(1);									//��ֹд��

	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x02;						//STATUS register
	EeWrite(1);									//д����
	
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x06;						//STATUS register
	EeWrite(1);									//д�Ĵ�������
	
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x30;						//LSB ADdress
	EEPROM.data[3]=0;							//��
	EEPROM.data[4]=0x00;						//��
	EEPROM.data[5]=0x80 | 0x00;					//ʱ(0x80ָ����24Сʱģʽ)
	EEPROM.data[6]=0x15;						//��
	EEPROM.data[7]=0x04;						//��
	EEPROM.data[8]=0x09;						//��
	EEPROM.data[9]=0x05;						//����
	EEPROM.data[10]=0x20;						//19/20
	EeWrite(8);									//д��ʱ��ֵ

	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x3F;						//LSB ADdress
	EEPROM.data[3]=0x0;							//STATUS register
	EeWrite(1);									//��ֹд��
	
//----------------------------------------------//��ȡʱ��ֵ��data[0~7]
	EEPROM.data[0]=0xDE;						//Slave ADdress
	EEPROM.data[1]=0;							//MSB ADdress
	EEPROM.data[2]=0x30;						//LSB ADdress
	EeRead(8);
}

/*********************************************************************************************************
** ��������: CRC-16
** ��������:����������* pData��16λCRCУ���룬����������ΪnLength 
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
** ��������: CRC-16
** ��������:����������* pData��16λCRCУ���룬����������ΪnLength 
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
** ��������: Sci485_TxInit
** ��������: 485���ͳ�ʼ��
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
** ��������: Sci485_RxInit
** ��������: 485���ճ�ʼ��
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
void Sci485_RxInit(void)
{
	Uint16 temp16;			//20100727���벨�����趨
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
** ��������: READSTATE
** ��������: ��ȡ������״̬-������
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
void READSTATE(void)
{
//----------------------------------------------------------------------------
//----COMMAND1------------------------------------------------------------------
	if(M_ChkFlag(SL_ENPCOPER)!=0)	  	_COMMAND1 |= 0x0001;	//0 PC����ʹ��   0-CAN����
	else								_COMMAND1 &= 0xFFFE;
	if(M_ChkFlag(SL_OPERATION)!=0)	  	_COMMAND1 |= 0x0002;	//1 ����ģʽѡ�� 0-����������
	else								_COMMAND1 &= 0xFFFD;
	if(M_ChkFlag(SL_OCS_EIN)!=0)	  	_COMMAND1 |= 0x0004;	//2 ����·������ָ��CANģʽ��ʹ��
	else								_COMMAND1 &= 0xFFFB;
	if(M_ChkFlag(SL_OCS_SYSRUN)!=0)	  	_COMMAND1 |= 0x0008;	//3 ͬ������ָ��CANģʽ��ʹ��
	else								_COMMAND1 &= 0xFFF7;
	if(M_ChkFlag(SL_OCS_PREC)!=0)	  	_COMMAND1 |= 0x0010;	//4 Ԥ���ָ��PCģʽ��ʹ��
	else								_COMMAND1 &= 0xFFEF;
	if(M_ChkFlag(SL_OCS_NPRSTART)!=0)	_COMMAND1 |= 0x0020;	//5 �������������PCģʽ��ʹ��
	else								_COMMAND1 &= 0xFFDF;
	if(M_ChkFlag(SL_OCS_MPRSTART)!=0)	_COMMAND1 |= 0x0040;	//6 �������������PCģʽ��ʹ��
	else								_COMMAND1 &= 0xFFBF;
	if(M_ChkFlag(SL_OCS_OPENSTAC)!=0)	_COMMAND1 |= 0x0080;	//7 ���ӽӴ�������ָ��PCģʽ��ʹ��
	else								_COMMAND1 &= 0xFF7F;
	if(M_ChkFlag(SL_OCS_RESET)!=0)		_COMMAND1 |= 0x0100;	//8 ���������ϸ�λָ��CAN/PCģʽ��ʹ��
	else								_COMMAND1 &= 0xFEFF;
	if(M_ChkFlag(SL_PC_ERRDATASAVE)!=0)	_COMMAND1 |= 0x0200;	//9 �ⲿRAM�����������ָ��
	else								_COMMAND1 &= 0xFDFF;
	if(M_ChkFlag(SL_PC_ERRDATACLR)!=0)	_COMMAND1 |= 0x0400;	//10 �ⲿRAM�ͷŹ�������ָ��
	else								_COMMAND1 &= 0xFBFF;
	if(M_ChkFlag(SL_PC_BANKDATASAVE)!=0)_COMMAND1 |= 0x0800;	//11 �ⲿRAM������ʾ����ָ��
	else								_COMMAND1 &= 0xF7FF;
	if(M_ChkFlag(SL_PCOCS_CBOFF)!=0)	_COMMAND1 |= 0x1000;	//12 ָ��PCģʽ��ʹ�� PC�����ֶ��Ͽ�CB 1=�Ͽ� 2010atcpc
	else								_COMMAND1 &= 0xEFFF;

	
//----STATE1------------------------------------------------------------------
	if(M_ChkFlag(SL_RUN)!=0)	  		_STATE1 |= 0x0001;	//����������������
	else								_STATE1 &= 0xFFFE;
	if(M_ChkFlag(SL_ERROR)!=0)	  		_STATE1 |= 0x0002;	//���������ڹ���
	else								_STATE1 &= 0xFFFD;
	if(M_ChkFlag(SL_CBCLOSED)!=0)	  	_STATE1 |= 0x0004;	//����·���պ����
	else								_STATE1 &= 0xFFFB;
	if(M_ChkFlag(SL_CHARGEOK)!=0)	  	_STATE1 |= 0x0008;	//Ԥ������
	else								_STATE1 &= 0xFFF7;
	if(M_ChkFlag(SL_NPR_PWMOUT)!=0)	  	_STATE1 |= 0x0010;	//�������������
	else								_STATE1 &= 0xFFEF;
	if(M_ChkFlag(SL_STEADYFB)!=0)	  	_STATE1 |= 0x0020;	//�м�ֱ����ѹ�ȶ�
	else								_STATE1 &= 0xFFDF;
	if(M_ChkFlag(SL_MPR_PWMOUT)!=0)	  	_STATE1 |= 0x0040;	//�������������
	else								_STATE1 &= 0xFFBF;
	if(M_ChkFlag(SL_MPR_SYNOK)!=0)	  	_STATE1 |= 0x0080;	//���Ӳ�ͬ���������
	else								_STATE1 &= 0xFF7F;
	if(M_ChkFlag(SL_SPEED_IN_RANGE)!=0)	_STATE1 |= 0x0100;	//���ת�������з�Χ��
	else								_STATE1 &= 0xFEFF;
	if(M_ChkFlag(SL_RAMERRORSAVE)!=0)	_STATE1 |= 0x0200;	//�ⲿRAM������Ϣ��ɴ洢��־
	else								_STATE1 &= 0xFDFF;
	if(M_ChkFlag(SL_RAMBANKSAVE)!=0)	_STATE1 |= 0x0400;	//�ⲿRAM��ʾ��Ϣ�����־
	else								_STATE1 &= 0xFBFF;
//----MagnetCurve2013-12-13
	if(M_ChkFlag(SL_MC_OK)!=0)			_STATE1 |= 0x0800;	//��������MagnetCurve200909
	else								_STATE1 &= 0xF7FF;
	if(M_ChkFlag(SL_MC_FAIL)!=0)		_STATE1 |= 0x1000;	//��������MagnetCurve200909
	else								_STATE1 &= 0xEFFF;

//----DISPLAY------------------------------------------------------------------
	if(M_ChkFlag(SL_DISPLAY0)!=0)	  	_DISPLAY |= 0x0001;	//�������������
	else								_DISPLAY &= 0xFFFE;
	if(M_ChkFlag(SL_DISPLAY1)!=0)	  	_DISPLAY |= 0x0002;	//�������������
	else								_DISPLAY &= 0xFFFD;
	if(M_ChkFlag(SL_DISPLAY2)!=0)	  	_DISPLAY |= 0x0004;	//�м�ֱ����ѹ�ȶ�
	else								_DISPLAY &= 0xFFFB;
	if(M_ChkFlag(SL_DISPLAY3)!=0)	  	_DISPLAY |= 0x0008;	//���ع���
	else								_DISPLAY &= 0xFFF7;
	if(M_ChkFlag(SL_DISPLAY4)!=0)	  	_DISPLAY |= 0x0010;	//���Ӳ�ﵽͬ����������
	else								_DISPLAY &= 0xFFEF;
	if(M_ChkFlag(SL_DISPLAY5)!=0)	  	_DISPLAY |= 0x0020;	//����
	else								_DISPLAY &= 0xFFDF;
	if(M_ChkFlag(SL_DISPLAY6)!=0)	  	_DISPLAY |= 0x0040;	//LVRT��բ�ܿ�ָͨʾ
	else								_DISPLAY &= 0xFFBF;
	if(M_ChkFlag(SL_DISPLAY7)!=0)	  	_DISPLAY |= 0x0080;	//�ȴ�Ԥ���ָ��ָʾ
	else								_DISPLAY &= 0xFF7F;
			
//----IO/INPUT------------------------------------------------------------------
	if(M_ChkFlag(SL_IN1_CBSTS)!=0)	  	_INPUT |= 0x0001;	//P4.1����·��״̬������	1=�պϣ�0=�Ͽ�
	else								_INPUT &= 0xFFFE;
	if(M_ChkFlag(SL_IN1_CBRESET)!=0)	_INPUT |= 0x0002;	//P4.2�������ڸ�λ����·����1=��λ��0=δ��λ
	else								_INPUT &= 0xFFFD;
	if(M_ChkFlag(SL_IN1_STATORK)!=0)	_INPUT |= 0x0004;	//P4.3���ӽӴ���״̬������	1=�պϣ�0=�Ͽ�
	else								_INPUT &= 0xFFFB;
	if(M_ChkFlag(SL_IN1_MIANK)!=0)	  	_INPUT |= 0x0008;	//P4.4���Ӵ���״̬������	1=�պϣ�0=�Ͽ�
	else								_INPUT &= 0xFFF7;
	if(M_ChkFlag(SL_IN1_MIANFILTER)!=0)	_INPUT |= 0x0010;	//P4.5���˲���״̬������	1=�պϣ�0=�Ͽ�
	else								_INPUT &= 0xFFEF;
	if(M_ChkFlag(SL_IN1_EXFAULTOK)!=0)	_INPUT |= 0x0020;	//P4.6�ⲿ������״̬������	1=������0=����	
	else								_INPUT &= 0xFFDF;
	if(M_ChkFlag(SL_IN1_MIANFAN)!=0)	_INPUT |= 0x0040;	//P4.7���ʷ��״̬������	1=������0=���� 
	else								_INPUT &= 0xFFBF;
	if(M_ChkFlag(SL_IN1_EXESTOP)!=0)	_INPUT |= 0x0080;	//P4.8�ⲿ��ͣ���Ϸ����� 	1=��ͣ��ť���£�0=��ͣ��ť����
	else								_INPUT &= 0xFF7F;
	if(M_ChkFlag(SL_IN3_CBTRIP)!=0)		_INPUT |= 0x0100;	//P4.9���Ϲ����ѿ۹��Ϸ�����1=���ϣ�0=����
	else								_INPUT &= 0xFEFF;

//----IO/OUTPUT------------------------------------------------------------------
	if(M_ChkFlag(CL_CBLVC)!=0)	  		_OUTPUT |= 0x0001;	//P5.5����·��Ƿѹ������ָ�	1=�պϣ�0=�Ͽ�
	else								_OUTPUT &= 0xFFFE;
	if(M_ChkFlag(CL_CBON)!=0)	  		_OUTPUT |= 0x0002;	//P5.6����·����բָ�		1=��բ
	else								_OUTPUT &= 0xFFFD;
	if(M_ChkFlag(CL_PRE)!=0)	  		_OUTPUT |= 0x0004;	//P5.7Ԥ���ָ�				1=Ԥ��磻0=��ֹԤ���
	else								_OUTPUT &= 0xFFFB;
	if(M_ChkFlag(CL_MAINK)!=0)	  		_OUTPUT |= 0x0008;	//P5.8���Ӵ������˲�����ָ�	1=�պϣ�0=�Ͽ�
	else								_OUTPUT &= 0xFFF7;
	if(M_ChkFlag(CL_STATORK)!=0)	  	_OUTPUT |= 0x0010;	//P5.9���ӽӴ���ָ�			1=�պϣ�0=�Ͽ�
	else								_OUTPUT &= 0xFFEF;
	if(M_ChkFlag(CL_CBFCON)!=0)	  		_OUTPUT |= 0x0020;	//P5.10����·��Ƿѹ�����ƣ�		1=�պϣ�0=�Ͽ�
	else								_OUTPUT &= 0xFFDF;
	if(M_ChkFlag(CL_FANWORK)!=0)	  	_OUTPUT |= 0x0040;	//P5.11���ʷ��ȿ��ƣ�			1=���У�0=ֹͣ
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
** ��������: WRCOMMAND
** ��������: д�������ָ��-������
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
void WRCOMMAND(void)
{
//----------------------------------------------------------------------------
 	if(M_ChkFlag(SL_ENPCOPER)!=0)		
 	{
		if((_COMMAND1&0x0002)!=0)	M_SetFlag(SL_OPERATION);
		else						M_ClrFlag(SL_OPERATION);
		if(((_COMMAND1&0x0004)!=0) && (M_ChkFlag(SL_ERROR)==0))	//201005atcpc
		{
	 		if(M_ChkCounter(MAIN_LOOP.cnt_ein,DELAY_EIN)>0)	M_SetFlag(SL_OCS_EIN);     		//��ʱ1s����ֹ�����		
		}
		else 	
		{
			M_ClrFlag(SL_OCS_EIN);              
			MAIN_LOOP.cnt_ein=0;		
    	}
     
		if(((_COMMAND1&0x0008)!=0) && (M_ChkFlag(SL_ERROR)==0))		//201005atcpc					
		{ 
	  		if(M_ChkCounter(MAIN_LOOP.cnt_sysrun,DELAY_SYSRUN)>0)	 M_SetFlag(SL_OCS_SYSRUN); //��ʱ1s����ֹ�����
		}				
		else 	
		{
			M_ClrFlag(SL_OCS_SYSRUN);
			MAIN_LOOP.cnt_sysrun=0;		
    	}
	
		if(((_COMMAND1&0x0010)!=0) && (M_ChkFlag(SL_ERROR)==0))		//201005atcpc					
		{ 
	  		if(M_ChkCounter(MAIN_LOOP.cnt_prec,DELAY_PREC)>0)	 M_SetFlag(SL_OCS_PREC); //��ʱ1s����ֹ�����
		}				
		else 	
		{
			M_ClrFlag(SL_OCS_PREC);
			MAIN_LOOP.cnt_prec=0;		
   		} 

		if(((_COMMAND1&0x0020)!=0) && (M_ChkFlag(SL_ERROR)==0))		//201005atcpc					
		{ 
	  		if(M_ChkCounter(MAIN_LOOP.cnt_nprcmd,DELAY_NPRCMD)>0)	 M_SetFlag(SL_OCS_NPRSTART); //��ʱ1s����ֹ�����
		}				
		else 	
		{
			M_ClrFlag(SL_OCS_NPRSTART);
			MAIN_LOOP.cnt_nprcmd=0;		
    	} 

		if(((_COMMAND1&0x0040)!=0) && (M_ChkFlag(SL_ERROR)==0))		//201005atcpc					
		{ 
	  		if(M_ChkCounter(MAIN_LOOP.cnt_mprcmd,DELAY_MPRCMD)>0)	 M_SetFlag(SL_OCS_MPRSTART); //��ʱ1s����ֹ�����
		}				
		else 	
		{
			M_ClrFlag(SL_OCS_MPRSTART);
			MAIN_LOOP.cnt_mprcmd=0;		
    	} 

		if((_COMMAND1&0x0080)!=0)							
		{ 
	  		if(M_ChkCounter(MAIN_LOOP.cnt_clostacmd,DELAY_CLOSTACMD)>0)	 M_SetFlag(SL_OCS_OPENSTAC); //��ʱ1s����ֹ�����
		}				
		else 	
		{
			M_ClrFlag(SL_OCS_OPENSTAC);
			MAIN_LOOP.cnt_clostacmd=0;		
    	} 

		if((_COMMAND1&0x0100)!=0)																	//��λ��Ҫ��λ����,���ⲿI/O����
		{ 
	  		if(M_ChkCounter(MAIN_LOOP.cnt_reset,DELAY_RESET)>0)	 M_SetFlag(SL_OCS_RESET);     		//��ʱ2s����ֹ�����
		}				
		else 	
		{
			M_ClrFlag(SL_OCS_RESET);
			MAIN_LOOP.cnt_reset=0;		
    	}	
    	
		if((_COMMAND1&0x1000)!=0)	M_SetFlag(SL_PCOCS_CBOFF);	 //12 ָ��PCģʽ��ʹ�� PC�����ֶ��Ͽ�CB 1=�Ͽ� 2010atcpc
		else						M_ClrFlag(SL_PCOCS_CBOFF);
   	}

	if((_COMMAND1&0x0200)!=0)	M_SetFlag(SL_PC_ERRDATASAVE);	 //9 �ⲿRAM�����������ָ��PC/CANģʽ�¶���ʹ��
	else						M_ClrFlag(SL_PC_ERRDATASAVE);	
	if((_COMMAND1&0x0400)!=0)	M_SetFlag(SL_PC_ERRDATACLR);	 //10 �ⲿRAM�ͷŹ�������ָ��PC/CANģʽ�¶���ʹ��
//	else						M_ClrFlag(SL_PC_ERRDATACLR);
	else if(M_ChkFlag(SL_LV_CLRERRAM)==0)			M_ClrFlag(SL_PC_ERRDATACLR);		//20130306
//	if((_COMMAND1&0x0800)!=0)	M_SetFlag(SL_PC_BANKDATASAVE);	 //11 �ⲿRAM������ʾ����ָ��PC/CANģʽ�¶���ʹ��
//	else						M_ClrFlag(SL_PC_BANKDATASAVE);

//20130724
	if((_COMMAND1&0x0800)!=0)
	{
		M_SetFlag(SL_PC_BANKDATASAVE);
		M_SetFlag(SL_TRIG_ERRDSAVE);	 //11 �ⲿRAM������ʾ����ָ��PC/CANģʽ�¶���ʹ��
	}
	else						M_ClrFlag(SL_PC_BANKDATASAVE);

//----------------------------------------------------------------------------
}

/*********************************************************************************************************
** ��������: ScibDatpro
** ��������: ��sci���յ������ݽ��н���У��
** �䡡��: 	 
** �䡡��:   response=0��ʾ��Ҫ����Ҫ�ظ�;response=1��ʾ��Ҫ�����ظ�;response=2��ʾ�������ظ�,��������ٻظ�(��ָ���������)
** ע  ��: 	 ͨ��Э��˵������
				SCI.rxb[0]:��ͷ(0x7E)
				SCI.rxb[1]:��λ����ַ
				SCI.rxb[2]:������(���ֽ�)
				SCI.rxb[3]:������(���ֽ�)
				SCI.rxb[4]:���������
				SCI.rxb[5]:��������ֵ(���ֽ�)
				SCI.rxb[6]:��������ֵ(���ֽ�)
				SCI.rxb[7]:״̬��(���ֽ�)
				SCI.rxb[8]:״̬��(���ֽ�)
				SCI.rxb[9]:���У��
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
Uint16 ScibDatpro(void)
{
	Uint16 response,i,j;
	Uint16 rx_command,rx_startnum,rx_opernum,rx_crc,cRc_16;
	Uint16 rx_ramwavenum,rx_ramsegstart,error_pos,bank_pos;
	
	
//------------------------------------------------------------------------------------------------		
		if(SCI.rxb[0]!=_SCIB_ADDR)	//��λ��&�ڵ��ַ����,���ǲ�������������
			response=0;				//�����д���ظ�,�ȴ���һ֡����
		
		else							//���������ƻ���λ�������ҽڵ��ַ���
		{
			SCI.txb[0] = SCI.rxb[0];								//��λ����ַ�ֽ�
			SCI.txb[1] = SCI.rxb[1];
			
			rx_crc=SCI.rxb[7];										//����У���1����16λ
			rx_crc=((rx_crc<<8)&0xFF00)|SCI.rxb[6];					//[6][7]two words

			length485=SCI485NUM_RX-2;
            cRc_16=GetCrc_16(&SCI.rxb[0],length485);				//CRCУ��
           
            if(rx_crc!=cRc_16)
              response=0;	
           
            else
            {
                response=1;
				
				//��������ת��
				rx_command=SCI.rxb[1];							//����������
				
				rx_startnum=SCI.rxb[2];							//���湦�������
				rx_startnum=((rx_startnum<<8)&0xFF00)|SCI.rxb[3];
			    
				rx_opernum=SCI.rxb[4];							//���湦����ֵ
				rx_opernum=((rx_opernum<<8)&0xFF00)|SCI.rxb[5];
				
				//�����֦��
				
				switch (rx_command&0xFFFF)					//�������???��FFFF��������û�����ð�������
				{
//--------------------NO.1--READ------
					case COM_RDAO:		//��ģ����
					{
						 if((rx_startnum+rx_opernum)>PARA_NUM)
                              response=0;
						 
						 else
						 {
						    
						    j=0; 
							READSTATE();							    
							for(i=rx_startnum;i<(rx_startnum+rx_opernum);i++)
                            {
								SCI.txb[3+j]=(*FUNC[i].para_add&0xFF00)>>8; //201005atcpc ��֤����CANͬʱͨѶ
								SCI.txb[4+j]=(*FUNC[i].para_add)&0x00FF;	//ֱ�ӽ�RAM�е����ݷ���
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
					case COM_WRAO:			//дģ����
					{		
							 if(rx_startnum>PARA_NUM)
                               response=0;
						     
						     else
					         {
					        	if((((FUNC[rx_startnum].attr&WR1)==0)&&((FUNC[rx_startnum].attr&WR0)==0)&&((FUNC[rx_startnum].attr&WR3)==0))||(((FUNC[rx_startnum].attr&WR1)==WR1)&&(M_ChkFlag(SL_RUN)!=0)))//��д�����������в�����
									response=0;						//�ò���ʧ�ܱ�־

								else
								{
					        		if((FUNC[rx_startnum].attr&WR3)==WR3)//����ֻ�޸�RAM���ݵ�д����
					        		{
								   		*FUNC[rx_startnum].para_add=rx_opernum;	//�޸�RAM���ֵ

					        			if(M_ChkFlag(SL_RUN)==0)			//�����в������޸Ŀ���ģʽ201005atcpc
										{
					        				if((_COMMAND1&0x0001)!=0)	M_SetFlag(SL_ENPCOPER);
											else						M_ClrFlag(SL_ENPCOPER);	
										}
					        			
					        			WRCOMMAND();	//ֻ��ʹ����PCOPER֮��������޸�������
					        		}
					        		else
					        		{
								   		*FUNC[rx_startnum].para_add=rx_opernum;	//�޸�RAM��EEROM
								
										EEPROM.mcode=rx_startnum;			//д��2�ֽ�����
										M_SetFlag(SL_EEASK_MCODE);			//��EEPROM�޸Ĺ����������־					        			
					        		}   
																	
									if((rx_startnum>=TIME_FIRST)&&(rx_startnum<=TIME_END))
										M_SetFlag(SL_SETRTIMER);			//��ʵʱʱ����Ҫ�����趨��־
								
									SCI.txb[2]=SCI.rxb[2];					//׼����������(�����ɹ�)
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
					case COM_RDRAM:		//��exRAM
						   
						 if((rx_startnum+rx_opernum)>PARA_NUM)
                              response=0;
						 
						 else
						 {						    
						    j=0; 
						    i=0;
							rx_ramwavenum = rx_startnum;									//0-49�������ݲ���,50Ϊ����ʱ������ 201005atcpc
							rx_ramsegstart = rx_opernum * RAM_SEGMENT;						//���β��ζδ��俪ʼ���ݵ�ַ 0-(n-1)
							 
							if(rx_ramwavenum<25)											//BANK_RAM:0-24
							{
								bank_pos = BANK_RAMDATA_POS + rx_ramsegstart;				//���β��ζδ��俪ʼ���ݵ�ַ+�ò��εĳ�ʼ��ַ
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
								error_pos = ERROR_RAMDATA_POS + rx_ramsegstart;				//���β��ζδ��俪ʼ���ݵ�ַ+�ò��εĳ�ʼ��ַ
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
** ��������: Sci485Ctrl
** ��������: ��sci�Ľ��շ��ͽ����ۺϿ���
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
void Sci485Ctrl(void)
{
	Uint16 i,j,response;

	if(M_ChkCounter(SCI.cnt_sciover,DELAY_SCIOVER)>=0)				//����/���ճ�ʱ?
	{
		SCI.cnt_sciover=0;
		M_ClrFlag(SL_TX);
		M_ClrFlag(SL_RX);
		Sci485_RxInit();											//485���ճ�ʼ��
	}
	else if(M_ChkFlag(SL_TX)!=0)									//����?
	{
		if(M_ChkFlag(SL_TXLONGDATA)==0)								//�����������?
		{
			if(ScibRegs.SCIFFTX.bit.TXFFST == 0)						//�������?Transmit FIFO is empty.
			{
				SCI.cnt_sciover=0;										//�������/���ճ�ʱ��ʱ��
				if(ScibRegs.SCICTL2.bit.TXEMPTY==1)						//���ͼĴ���Ϊ��?Transmitter buffer and shift registers are both empty
		 		{
					M_ClrFlag(SL_TX);
					Sci485_RxInit();									//485���ճ�ʼ��
				}
			}
		}
	}
	else															//����
	{
		if(ScibRegs.SCIFFRX.bit.RXFFST == 0)						//δ��ʼ���ջ�������?
		{
			SCI.cnt_sciover=0;										//�������/���ճ�ʱ��ʱ��
			M_ClrRxFifoOvr();
			
			if(M_ChkFlag(SL_RX)!=0)									//�������?
			{
				if(M_ChkCounter(SCI.cnt_scispace,DELAY_SCISPACE)>=0)//���յ����ͼ����ʱ?
	    		{
					if(M_ChkFlag(SL_RESUME)!=0)						//�ָ���������������?
					{
						if(M_ChkFlag(SL_EEASK_INI)==0)				//EEPROM�޸Ĺ�����������?
						{
							M_ClrFlag(SL_RESUME);					//��ָ��������������б�־
							M_SetFlag(SL_TX);						//�÷�����־λ
							M_ClrFlag(SL_RX);
							
							Sci485_TxInit();						//485���ͳ�ʼ��
							
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
						response=ScibDatpro();						//�������ݽ�������
						
						if(response==1)								//��ʾҪ�����ظ�
						{
//							M_SetFlag(SL_TX);						//�÷��������־λ
							M_ClrFlag(SL_RX);
							
							Sci485_TxInit();						//485���ͳ�ʼ��
							
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
							M_SetFlag(SL_TX);						//�÷��������־λ120625
						}
						else if(response==0)						//����Ҫ�ظ�
						{
							M_ClrFlag(SL_RX);
							Sci485_RxInit();						//485���ճ�ʼ��
						}
					}
				}
			}
		}
		else if((ScibRegs.SCIFFRX.bit.RXFFST >= 1)&&(M_ChkFlag(SL_HEADOK)==0))	//��ʼ�����һ�û���յ���ͷ?
		{
			SCI.rxb[0]=ScibRegs.SCIRXBUF.all&0x00FF;
			if(SCI.rxb[0]==_SCIB_ADDR)								//different!!! 
				M_SetFlag(SL_HEADOK);
			else
				Sci485_RxInit();									//485���ճ�ʼ��
		}
		else if(ScibRegs.SCIFFRX.bit.RXFFST >= SCI485NUM_RX-1)		//change�������?SCIFFRX.bit.RXFFST��Receive FIFO has x words
		{
			SCI.cnt_sciover=0;										//�������/���ճ�ʱ��ʱ��
			M_DisTxRxb();
			M_ClrRxFifoOvr();
			
			for(i=1;i<SCI485NUM_RX;i++)								//�������ջ���(��������ͷ) 9λ����
				SCI.rxb[i]=ScibRegs.SCIRXBUF.all&0x00FF;
			
			M_ClrFlag(SL_HEADOK);
			M_SetFlag(SL_RX);										//�ý�����ɱ�־λ
			SCI.cnt_scispace=0;										//������յ����ͼ����ʱ��
		}
	}
}

/*********************************************************************************************************
** ��������: Sci_canopenrx
** ��������: sci_CANOPEN��ʼ��
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
** ��������: Sci_canopenrx
** ��������: ��sci_CANOPEN�Ľ��շ��ͽ����ۺϿ���
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
void Sci_canopenrx(void)
{
	Uint16 i,heartbeat,xor_data,temp;
	
	if(M_ChkCounter(SCI_canopen.cnt_sciover,DELAY_SCICANOPENOVER)>=0)				//���ճ�ʱ?
	{
		SCI_canopen.cnt_sciover=0;
		Scicanopen_Init();
		M_ClrFlag(SL_CANOPENHEAD);			//20130801
		M_ClrFlag(SL_CANOPENHEADOK);		//20130801
	}
	else
	{
/*120507		if((SciaRegs.SCIFFRX.bit.RXFFST >= 1)&&(M_ChkFlag(SL_CANOPENHEADOK)==0))	//��ʼ�����һ�û���յ���ͷ?
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
		else if(SciaRegs.SCIFFRX.bit.RXFFST >= SCICANOPENRXNUM-1)			//�������?
		{
			
			//	SciaRegs.SCICTL1.bit.RXENA =0;
				for(i=1;i<SCICANOPENRXNUM;i++)								//�������ջ���(��������ͷ)
					SCI_canopen.rxb[i]=SciaRegs.SCIRXBUF.all&0x00FF;
				M_ClrFlag(SL_CANOPENHEADOK);
				
				heartbeat= SCI_canopen.rxb[2] & ONEBYTE[0];
				if(heartbeat!=SCI_canopen.heartbeat)
					SCI_canopen.cnt_heartbeat=0;
				
				SCI_canopen.heartbeat = heartbeat;

				xor_data=SCI_canopen.rxb[0];								//���У��
				for(i=1;i<(SCICANOPENRXNUM-1);i++)
					xor_data ^= SCI_canopen.rxb[i];
			
				if(xor_data == SCI_canopen.rxb[SCICANOPENRXNUM-1])							//���У�����?
				{
					if((SCI_canopen.rxb[1]|SCI_canopen.rxb[2]|SCI_canopen.rxb[3]|SCI_canopen.rxb[4]|SCI_canopen.rxb[5]|SCI_canopen.rxb[6]|SCI_canopen.rxb[7]|SCI_canopen.rxb[8])!=0) //�޳����ݶ�����0�Ļ���20090817
					{			
					//����ת��
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
/*		if((SciaRegs.SCIFFRX.bit.RXFFST >= 2)&&(M_ChkFlag(SL_CANOPENHEADOK)==0))	//��ʼ�����һ�û���յ���ͷ?
		{
			SCI_canopen.cnt_sciover=0;
	
			for(i=0;i<2;i++)								//У����հ�����ͷ��ǰ5λ��Ϣ
				SCI_canopen.rxb[i]=SciaRegs.SCIRXBUF.all&0x00FF;
			
			if(SCI_canopen.rxb[0]==CANOPENCOM_HEAD && SCI_canopen.rxb[1]==0x11)
				M_SetFlag(SL_CANOPENHEADOK);	//[0]=0x7E,[1]=������0x11,[2]=���ݳ���9���ֽ�,[3]=�ض�����0x11,[4]=��ַƫ��0x00

			else
			{	
				SciaRegs.SCIFFRX.bit.RXFIFORESET=1;		// Re-enable RX FIFO operation
				SciaRegs.SCIFFRX.bit.RXFFOVRCLR=1;
			}
		}
*/
//---------20130801----------
		if((SciaRegs.SCIFFRX.bit.RXFFST >= 1)&&(M_ChkFlag(SL_CANOPENHEADOK)==0))	//��ʼ�����һ�û���յ���ͷ?	20121215
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
					for(i=0;i<temp;i++)								//У����հ�����ͷ��ǰ5λ��Ϣ
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
					for(i=0;i<temp;i++)								//У����հ�����ͷ��ǰ5λ��Ϣ
						SCI_canopen.rxb[0]=SciaRegs.SCIRXBUF.all&0x00FF;
				}
				
			}
		}
//---------20130801----------
		else if(SciaRegs.SCIFFRX.bit.RXFFST >= SCICANOPENRXNUM-2)			//�������?
		{
//				M_NotFlag(SL_PHASEA);            							//2014-05-06��ʱ����CANopen
//				*OUT3_ADDR = _OUT3_DATA;
			
				for(i=2;i<SCICANOPENRXNUM;i++)								//�������ջ���(��������ͷ)
					SCI_canopen.rxb[i]=SciaRegs.SCIRXBUF.all&0x00FF;	//[5]-[12]��4���ֵ�ָ������,[13]��У��λ
				M_ClrFlag(SL_CANOPENHEADOK);
				M_ClrFlag(SL_CANOPENHEAD);
				
				heartbeat= SCI_canopen.rxb[6] & ONEBYTE[0];  //201105CPC ��ǣͨѶ"����λ"��Bit15
				if(heartbeat!=SCI_canopen.heartbeat)
				{
					SCI_canopen.cnt_heartbeat=0;
//    				M_NotFlag(SL_PHASEB);								//2014-05-06��ʱ����CANopen
//    				*OUT3_ADDR = _OUT3_DATA;
				}
				SCI_canopen.heartbeat = heartbeat;

				xor_data=SCI_canopen.rxb[0];								//���У��
				for(i=1;i<(SCICANOPENRXNUM-1);i++)
					xor_data ^= SCI_canopen.rxb[i];
			
				if(xor_data == SCI_canopen.rxb[SCICANOPENRXNUM-1])							//���У�����?
				{
//					if((SCI_canopen.rxb[5]|SCI_canopen.rxb[6]|SCI_canopen.rxb[7]|SCI_canopen.rxb[8]|SCI_canopen.rxb[9]|SCI_canopen.rxb[10]|SCI_canopen.rxb[11]|SCI_canopen.rxb[12])!=0) //�޳����ݶ�����0�Ļ���20090817
//					{			
					//����ת��
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
** ��������: Sci_canopentx_read
** ��������: ��G-Gate�����·���ȡ����ָ��
** �䡡��: 	 
** �䡡��:   
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:120507
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Sci_canopentx_read(void)
{
	Uint16 i,xor_data;

	SCI_canopen.txb[0]= CANOPENCOM_HEAD;						//��ͷ
	SCI_canopen.txb[1]= 0x0011;									//��������
	SCI_canopen.txb[2]= 0x0002;									//������Ϣ����=��ַƫ����+�����ֽ�����
	SCI_canopen.txb[3]= 0x0011;									//�ض����� ��Ϊ0x11
	SCI_canopen.txb[4]= 0x0000;									//��ַƫ���� ��Ϊ0
	SCI_canopen.txb[5]=	0x0008;									//������8���ֽ�

	xor_data=SCI_canopen.txb[0];								//���У��
	for(i=1;i<SCICANOPENREADNUM-1;i++)
		xor_data ^= SCI_canopen.txb[i];
				
	SCI_canopen.txb[SCICANOPENREADNUM-1] = xor_data;
		
	for(i=0;i<SCICANOPENREADNUM;i++)							//2011XGate
		SciaRegs.SCITXBUF=SCI_canopen.txb[i];
			
}  

/*********************************************************************************************************
** ��������: Sci_canopentx
** ��������: ��sci_CANOPEN�Ľ��շ��ͽ����ۺϿ���
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

		xor_data=SCI_canopen.txb[0];									//���У��
		for(i=1;i<SCICANOPENTXNUM-1;i++)
			xor_data ^= SCI_canopen.txb[i];
				
		SCI_canopen.txb[SCICANOPENTXNUM-1] = xor_data;
		
		for(i=0;i<SCICANOPENTXNUM-2;i++)
			SciaRegs.SCITXBUF=SCI_canopen.txb[i];
*/		
//CPCͨѶЭ��120507	
		SCI_canopen.txb[0]= CANOPENCOM_HEAD;						//��ͷ
		SCI_canopen.txb[1]= 0x0010;									//д������
		SCI_canopen.txb[2]= 17;										//������Ϣ����=��ַƫ����+�����ֽ�����
		SCI_canopen.txb[3]= 0x0011;									//�ض����� ��Ϊ0x11
		SCI_canopen.txb[4]= 0x0000;									//��ַƫ���� ��Ϊ0

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

		xor_data=SCI_canopen.txb[0];								//���У��
		for(i=1;i<SCICANOPENTXNUM-1;i++)
			xor_data ^= SCI_canopen.txb[i];
				
		SCI_canopen.txb[SCICANOPENTXNUM-1] = xor_data;
		
		for(i=0;i<SCICANOPENTXNUM-6;i++)							//2011XGate
			SciaRegs.SCITXBUF=SCI_canopen.txb[i];
} 
/*********************************************************************************************************
** ��������: DataFilter
** ��������: �����˲�
** �䡡��: 	Y(k-1)Ϊ�ϴ��˲�����X(k)��²���ֵ����
** �䡡��: ��Y(k)Ϊ�����˲������  
** ע  ��: 	 �˲���ʽΪ��Y(k)=cY(k-1)+(1-c)X(k),���У�c=1/(1+2*PAI*fh/fs),fhΪ��ͨ�˲����Ľ�ֹƵ�ʣ�fsΪ����Ƶ��.
			��һ�׵�ͨ�˨�У�X(k)�ΪY(k)��
			ֱ�������1��Ϊ�ȶ�ʱ�䡣
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void DataFilter( float c,float *out,float in)
{
   *out = c * *out + (1 - c) * in;
} 

/*********************************************************************************************************
** ��������: Bandstop filter 
** ��������: �ݲ���
** �䡡��:   w0 ����Ƶ��
			 wb �������Ƶ��
			 Ts=2e-4
			 
** �䡡��:   
** ע  ��: 201110bjtu
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** ����:   201110 BJTULVRT201204
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
	w2 = 4 * tan(0.0001 * w0) * tan(0.0001 * w0);                   //w0^2����Ƶ��ƽ��
	W  = 4 * (tan(0.0001 * (w0 + wb)) - tan(0.0001 * (w0 - wb)));   //W����
    	
    outtemp = ((4+w2) * var->in + (2* w2-8) * var->inp + (4+w2) * var->infp - (2* w2-8) * var->outp - (4-W+w2) * var->outfp) /(4+W+w2);     //����������ɣ�5��ʽ�á�

	if     (outtemp >  BS_var.outmax)  outtemp =  BS_var.outmax;		//����޷�
	else if(outtemp < -BS_var.outmax)  outtemp = -BS_var.outmax;		//����޷�

	var->out  = outtemp;		

	var->infp  = var->inp;      //in(k-1)-->in(k-2)
	var->inp   = var->in;       //in(k)-->in(k-1)
	var->outfp = var->outp;     //out(k-1)-->out(k-2)
	var->outp  = var->out;      //out(k)-->out(k-1)				
} 

/*********************************************************************************************************
** ��������: Bandstop filter without pre-warped
** ��������: �ݲ���-��Ԥ����
** �䡡��:   w0 ����Ƶ��
			 wb �������Ƶ��
			 Ts=2e-4
			 
** �䡡��:   
** ע  ��: 201110bjtu
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** ����:   201110 BJTULVRT201204
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void BS_Loop_S(struct BS_DATA *var,struct BS_PARA BS_var)
{

	float outtemp,w0,wb,w2,W;

	w0 = BS_var.f0 * TWOPAI;
	wb = BS_var.fb * TWOPAI;
	w2 = 4e-8 * w0 * w0;            //w0^2����Ƶ��ƽ�� ��Ԥ����
	W  = 8e-4  * wb;   				//W����
    	
    outtemp = ((4+w2) * var->in + (2* w2-8) * var->inp + (4+w2) * var->infp - (2* w2-8) * var->outp - (4-W+w2) * var->outfp) /(4+W+w2);     //����������ɣ�5��ʽ�á�

	if     (outtemp >  BS_var.outmax)  outtemp =  BS_var.outmax;		//����޷�
	else if(outtemp < -BS_var.outmax)  outtemp = -BS_var.outmax;		//����޷�

	var->out  = outtemp;		

	var->infp  = var->inp;      //in(k-1)-->in(k-2)
	var->inp   = var->in;       //in(k)-->in(k-1)
	var->outfp = var->outp;     //out(k-1)-->out(k-2)
	var->outp  = var->out;      //out(k)-->out(k-1)				
} 

/*********************************************************************************************************
** ��������: 100Hz Bandstop filter 
** ��������: �ݲ���
** �䡡��:   w0=100 ����Ƶ��
			 wb=+-50Hz �������Ƶ��
			 Ts=2e-4
			 
** �䡡��:   
** ע  ��: 201110bjtu
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** ����:   201110 BJTULVRT201204
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void BS_Loop100(struct BS_DATA *var,struct BS_PARA BS_var)
{
	float outtemp;
    	
    outtemp = 0.9409 * var->in - 1.8669 * var->inp + 0.9409 * var->infp + 1.8669 * var->outp - 0.8817 * var->outfp;     //����������ɣ�5��ʽ�á�

	if     (outtemp >  BS_var.outmax)  outtemp =  BS_var.outmax;		//����޷�
	else if(outtemp < -BS_var.outmax)  outtemp = -BS_var.outmax;		//����޷�

	var->out  = outtemp;		

	var->infp  = var->inp;      //in(k-1)-->in(k-2)
	var->inp   = var->in;       //in(k)-->in(k-1)
	var->outfp = var->outp;     //out(k-1)-->out(k-2)
	var->outp  = var->out;      //out(k)-->out(k-1)				
} 


/*********************************************************************************************************
** ��������: Ad8364Ctrl
** ��������: ��ȡ������ǰһε�ת�������ͬʱ������һ��ADת��
** 䡡��: 	 
** �䡡��:   
** ע  ��: 	 ÿ�ζ�����
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Ad8364Ctrl(void)
{
	float tempa,tempb,tempc,temp;

//------------------------------���ݶ�ȡ(��18·)---------------------------------
//����AD������100us֮ǰ�Ľ��
	AD.dat[0] = *AD_ASTART;	       // STA.Ubc ������Ӳ��ѹUbc
	AD.dat[1] = *(AD_ASTART+1);    // AMUX�������źţ�16ѡ1�������ź�
//	AD.dat[2] = *(AD_ASTART+2);    // GRD.Ubc ����ǰUbc
	AD.dat[2] = *(AD_ASTART+2);    // chopper����idc	20130805
//	AD.dat[3] = *(AD_ASTART+3);    // Ic ����SKIIP��������
//	AD.dat[4] = *(AD_ASTART+4);    // GRD.Uab ����ǰUab
	AD.dat[5] = *(AD_ASTART+5);    // MPR.ic, ���������MPR�ĵ���

	AD.dat[6]  = *AD_BSTART;       // NGS.Uab �����ѹUbc���˲�ͨ��
	AD.dat[7]  = *(AD_BSTART+1);   // MPR.ib�����������NPR
	AD.dat[8]  = *(AD_BSTART+2);   // NGS.Ubc �����ѹUbc���˲�ͨ�� 
	AD.dat[9]  = *(AD_BSTART+3);   // MPR.ia�����������NPR�ĵ��� 
	AD.dat[10] = *(AD_BSTART+4);   // Udc �м�ֱ����ѹ 
	AD.dat[11] = *(AD_BSTART+5);   // NPR.ic�����������NPR�ĵ���

//	AD.dat[12] = *AD_CSTART;       // Vdc2 
	AD.dat[13] = *(AD_CSTART+1);   // NPR.ib�����������NPR�ĵ���
	AD.dat[14] = *(AD_CSTART+2);   // NGS.Uab �����ѹUab
	AD.dat[15] = *(AD_CSTART+3);   // NPR.ia�����������NPR�ĵ���
	AD.dat[16] = *(AD_CSTART+4);   // STA.Uab ������Ӳ��ѹUab
	AD.dat[17] = *(AD_CSTART+5);   // NGS.Ubc �����ѹUbc

//----------------------------���ݶ�ȡ����------------------------------
	
	ADFINAL.ia1  = AD.dat[15];		// NPR.ia�����������NPR�ĵ���
	ADFINAL.ib1  = AD.dat[13];	    // NPR.ib�����������NPR�ĵ���
	ADFINAL.ic1  = AD.dat[11];	    // NPR.ic�����������NPR�ĵ���

	ADFINAL.ia2  = AD.dat[9];	    // MPR.ia�����������NPR�ĵ���
	ADFINAL.ib2  = AD.dat[7];		// MPR.ib�����������NPR�ĵ���	
	ADFINAL.ic2  = AD.dat[5];		// MPR.ic, ���������MPR�ĵ���	

//	ADFINAL.uab   = AD.dat[4];       // GRD.Uab ����ǰUab
//	ADFINAL.ubc   = AD.dat[2];		 // GRD.Ubc ����ǰUbc
//	ADFINAL.isa  = AD.dat[2];		// ���ӵ���A�� ���������Ϊ��
//	ADFINAL.isc  = AD.dat[4];	    // ���ӵ���C�� BJTULVRT201204
	ADFINAL.idc  = AD.dat[2];		//chopper����idc	20130805

	ADFINAL.uab1 = AD.dat[14];		// Uab �����ѹ		
	ADFINAL.ubc1 = AD.dat[17];		// Ubc �����ѹ
	
	ADFINAL.uab2 = AD.dat[16];		// Uab �����Ӳ��ѹ		
	ADFINAL.ubc2 = AD.dat[0];		// Ubc ������Ӳ��ѹ

	ADFINAL.uab3 = AD.dat[6];		// Uab �����ѹ	���˲�ͨ�� 	
	ADFINAL.ubc3 = AD.dat[8];		// Ubc �����ѹ ���˲�ͨ�� 


	ADFINAL.udc  = AD.dat[10];		//�м�ֱ����ѹ���

    if(ADFINAL.udc < 0) ADFINAL.udc=0;

	ADFINAL.AMUX = AD.dat[1];       //����AD����

//-----------------------------------------------------------	     
	switch(_OUT4_DATA)
	{
		case(8): {AMUX.NPR_tempa=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//NPR��A���¶ȣ�Skiip����,=10V*10/(32768) +20 			   				
		case(10):{AMUX.NPR_tempb=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//NPR��B���¶ȣ�Skiip����,=10V*10/(32768) +20		    
		case(12):{AMUX.NPR_tempc=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//NPR��C���¶ȣ�Skiip����,=10V*10/(32768) +20			    
		case(11):{AMUX.MPR_tempa=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//MPR��A���¶ȣ�Skiip����,=10V*10/(32768) +20			    
		case(9): {AMUX.MPR_tempb=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//MPR��B���¶ȣ�Skiip����,=10V*10/(32768) +20	     
		case(2): {AMUX.MPR_tempc=ADFINAL.AMUX   * 0.0030517 + 20;   break; }	//MPR��C���¶ȣ�Skiip����,=10V*10/(32768) +20
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
//---------------�м�ֱ��ն����-IGBT����BJTULVRT201204---------------------------
/*		case(5): {
					ADFINAL.idc = ADFINAL.AMUX ;
					AD_OUT_DC_I.b = ADFINAL.idc * 7.6294e-2 * 1.0000000;  //20ŷ���õ���,700A��Ӧ140mA(1:5000) =10V*700A/(32768*20R*140mA)
					break;
				 }
*///		case(5): {AD_OUT_STA_I.ac=ADFINAL.AMUX  * 0.0732422;		break; }	//������Ӳ�A���ߵ�����������1200A:1A,Rt=5R,=1200A*10V/(5R*1A*32768)			     
//		case(4): {AD_OUT_STA_I.ba=ADFINAL.AMUX  * 0.0732422;		break; }	//������Ӳ�B���ߵ�����������
//20130805
		case(5): {AD_OUT_STA_I.ac=ADFINAL.AMUX  * 0.0732422;		break; }	//������Ӳ�A���ߵ�����������1200A:1A,Rt=5R,=1200A*10V/(5R*1A*32768)			     
		case(4): {AD_OUT_STA_I.ba=ADFINAL.AMUX  * 0.0732422;		break; }	//������Ӳ�B���ߵ�����������
//--------
		case(7): {AD_OUT_SCR_I.a =ADFINAL.AMUX  * 0.0305176;		break; }	//ActiveCROWBAR��A��SCR����������LEM/��ѹ�ͷ���4A--400V =10V*400A/(32768*4V)			     
		case(6): {AD_OUT_SCR_I.b =ADFINAL.AMUX  * 0.0305176;		break; }	//ActiveCROWBAR��B��SCR����������LEM/��ѹ�ͷ���4A--400V =10V*400A/(32768*4V)
		default: break; 			      
	}

	if(M_ChkCounter(MAIN_LOOP.cnt_AMUX,DELAY_AMUX)>=0)
	{
	    MAIN_LOOP.cnt_AMUX=0;                         							//5ms��һ������AD	     
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
	AD_OUT_DC_I.b = ADFINAL.idc * 7.6294e-2 * 1.0000000;  //20ŷ���õ���,700A��Ӧ140mA(1:5000) =10V*700A/(32768*20R*140mA)
//----------------

//-------------------���ӵ�������λA��-------------------------------------------------------------
/*	AD_OUT_STA_I.a  =  (AD_OUT_STA_I.ac - AD_OUT_STA_I.ba) * 0.3333333;
	AD_OUT_STA_I.b  =   AD_OUT_STA_I.a  + AD_OUT_STA_I.ba;	
    AD_OUT_STA_I.c  = - AD_OUT_STA_I.a  - AD_OUT_STA_I.b; 
*/
//-------------------���ӵ�������λA��-BJTULVRT201204------------------------------------------------------------
/*	AD_OUT_STA_I.ac =   ADFINAL.isa  * 0.114441;	//LEM2000A������Ӳ�A���ߵ�����1500A-4V;==(1500/4*10)/32768  20111128
	AD_OUT_STA_I.cb =   ADFINAL.isc  * 0.114441;	//LEM2000A������Ӳ�B���ߵ�����200A-100mA;50.0ohm����,==10V*200A*D/(100mA*50R*32768*1) INA114���� 20111128
	AD_OUT_STA_I.ba = - AD_OUT_STA_I.ac - AD_OUT_STA_I.cb;
*/
	AD_OUT_STA_I.a  =  (AD_OUT_STA_I.ac - AD_OUT_STA_I.ba) * 0.3333333;	//201112fluxObs
	AD_OUT_STA_I.b  =   AD_OUT_STA_I.a  + AD_OUT_STA_I.ba;
    AD_OUT_STA_I.c  = - AD_OUT_STA_I.a  - AD_OUT_STA_I.b;
	tempa = abs(AD_OUT_STA_I.ac);
	tempb = abs(AD_OUT_STA_I.cb);
	DataFilter(0.9999,&MEAN_DATA.iac3,tempa); //���Ӳ����	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.icb3,tempb); //���Ӳ����	ƽ��ֵ�˲�10S

	tempa = abs(AD_OUT_STA_I.a);
	tempb = abs(AD_OUT_STA_I.b);
	tempc = abs(AD_OUT_STA_I.c);
	DataFilter(0.9999,&MEAN_DATA.ia3,tempa); //���Ӳ����	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.ib3,tempb); //���Ӳ����	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.ic3,tempc); //���Ӳ����	ƽ��ֵ�˲�10S

//------------------�������������------------------------------------------------------------------
    AD_OUT_NPR_I.a = - (ADFINAL.ia1 * 0.0572204);  // SKIIP��������(�����ű�Ϊ��),�װ���һ������10V=1875A,=10V*1875A/(32768*10V)
	AD_OUT_NPR_I.b = - (ADFINAL.ib1 * 0.0572204);  // SKIIP��������(�����ű�Ϊ��)�������㷨������SKIIP�
	AD_OUT_NPR_I.c = - (ADFINAL.ic1 * 0.0572204);  // SKIIP��������(�����ű�Ϊ��)
//---20121103-----------------��������˲ʱֵ����-201011LVRT--------------------------
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

	PRO.NPR_iac = temp * 100 / NPR_IACN;			//��������˲ʱ���ֵ����׼�����İٷ�ֵ

//-------------------�������������תΪʵ��ֵ-201005atcpc-----------------------------------------------------
	if(R_PHORDE==1)		//���������201005atcpc
	{
    	AD_OUT_MPR_I.b =  (ADFINAL.ia2 * 0.0572204);  // SKIIP��������(�����ű�Ϊ��),�װ���һ������10V=1875A,=10V*1875A/(32768*10V)
		AD_OUT_MPR_I.a =  (ADFINAL.ib2 * 0.0572204);  // SKIIP��������(�����ű�Ϊ��)�������㷨������SKIIPΪ�
		AD_OUT_MPR_I.c =  (ADFINAL.ic2 * 0.0572204);  // SKIIP��������(�����ű�Ϊ��)
	}
	else
	{
	    AD_OUT_MPR_I.a =  (ADFINAL.ia2 * 0.0572204);  // SKIIP��������(�����ű�Ϊ��),�װ���һ������10V=1875A,=10V*1875A/(32768*10V)
		AD_OUT_MPR_I.b =  (ADFINAL.ib2 * 0.0572204);  // SKIIP��������(�����ű�Ϊ��)�������㷨������SKIIPΪ��
		AD_OUT_MPR_I.c =  (ADFINAL.ic2 * 0.0572204);  // SKIIP��������(�����ű�Ϊ��)
	}
//----20121103----------------ת�ӵ���˲ʱֵ����-----201011LVRT----------------------
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

	PRO.MPR_iac = temp * 100 / MPR_IACN;		//ת�ӵ����ֵ����׼����İٷ�ֵ

//---------------------ֱ����ѹ----------------------------------------------------------------------
    AD_OUT_UDC      = ADFINAL.udc * 0.0448788;   // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
	DataFilter(0.44,&MEAN_DATA.udc,AD_OUT_UDC);  //Vdcֱ���˲���fh=1kHz
//	DataFilter(0.7,&MEAN_DATA.udc,AD_OUT_UDC);   //Vdcֱ���˲���fh=340Hz 201005atcpc
    AD_OUT_UDC      = MEAN_DATA.udc;
//    AD_OUT_UDC      = 1100;	//controltest
//----20121103------------�м�ֱ����ѹ����ֵ����-201011LVRT----------------------------

	PRO.udc  = AD_OUT_UDC;	                                 //��λV
						
//---------------------����ǰ������ѹ---------------------------------------------------------------------
//	AD_OUT_GRD_U.ab = ADFINAL.uab * 0.0448788;   // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
//	AD_OUT_GRD_U.bc = ADFINAL.ubc * 0.0448788;   // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA) 

//---------------------���Ϻ������ѹ---------------------------------------------------------------------
	AD_OUT_NGS_U.ab = ADFINAL.uab1 * 0.0448788;   // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
	AD_OUT_NGS_U.bc = ADFINAL.ubc1 * 0.0448788;   // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)

//-------------------������Ӳ��ߵ�ѹ---------------------------------------------
	AD_OUT_STA_U.ab  = ADFINAL.uab2 * 0.0448788;  // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
	AD_OUT_STA_U.bc  = ADFINAL.ubc2 * 0.0448788;  // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)

//-------------------���Ϻ������ѹ,���˲�ͨ��---------------------------------------------
	AD_OUT_NGF_U.ab  = ADFINAL.uab3 * 0.0448788;  // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
	AD_OUT_NGF_U.bc  = ADFINAL.ubc3 * 0.0448788;  // LEM(40mA=1500V),Rt=255R,=10V*1500V/(32768*255R*40mA)
/* 
//---------------------��Ư�˲�201005atcpc-------------------------------------------------------------------	
	DataFilter(0.999985,&MEAN_DATA.zfia1,AD_OUT_NPR_I.a); //�������������	��Ư�˲�60S
	DataFilter(0.999985,&MEAN_DATA.zfib1,AD_OUT_NPR_I.b); //�������������	��Ư�˲�60S
	DataFilter(0.999985,&MEAN_DATA.zfic1,AD_OUT_NPR_I.c); //�������������	��Ư�˲�60S

	DataFilter(0.999985,&MEAN_DATA.zfia3,AD_OUT_STA_I.a); //���Ӳ����������	��Ư�˲�60S
	DataFilter(0.999985,&MEAN_DATA.zfib3,AD_OUT_STA_I.b); //���Ӳ����������	��Ư˲�60S
	DataFilter(0.999985,&MEAN_DATA.zfic3,AD_OUT_STA_I.c); //���Ӳ����������	��Ư�˲�60S

	DataFilter(0.999985,&MEAN_DATA.zfuab,AD_OUT_GRD_U.ab); //����ǰ��ѹ��ѹ������	��Ư�˲�60S
	DataFilter(0.999985,&MEAN_DATA.zfubc,AD_OUT_GRD_U.bc); //����ǰ��ѹ��ѹ������	��Ư�˲�60S 

	DataFilter(0.999985,&MEAN_DATA.zfuab1,AD_OUT_NGS_U.ab); //��ѹ��ѹ������	��Ư�˲�60S
	DataFilter(0.999985,&MEAN_DATA.zfubc1,AD_OUT_NGS_U.bc); //��ѹ��ѹ������	��Ư�˲�60S


//------------------����Ư--������鲻����Ի������������Ư�˲���201005atcpc-------------------------------------------------------------------
	if(M_ChkFlag(SL_SENSZFSTDY)!=0)
	{
		AD_OUT_NPR_I.a = AD_OUT_NPR_I.a - MEAN_DATA.zfia1;  //�������
		AD_OUT_NPR_I.b = AD_OUT_NPR_I.b - MEAN_DATA.zfib1;
		AD_OUT_NPR_I.c = AD_OUT_NPR_I.c - MEAN_DATA.zfic1;

		AD_OUT_STA_I.a = AD_OUT_STA_I.a - MEAN_DATA.zfia3;  //���Ӳ����
		AD_OUT_STA_I.b = AD_OUT_STA_I.b - MEAN_DATA.zfib3; 
		AD_OUT_STA_I.c = AD_OUT_STA_I.c - MEAN_DATA.zfic3; 

		AD_OUT_GRD_U.ab = AD_OUT_GRD_U.ab - MEAN_DATA.zfuab; //����ǰ��ѹ��ѹ
		AD_OUT_GRD_U.bc = AD_OUT_GRD_U.bc - MEAN_DATA.zfubc; 

		AD_OUT_NGS_U.ab = AD_OUT_NGS_U.ab - MEAN_DATA.zfuab1; //��ѹ��ѹ
		AD_OUT_NGS_U.bc = AD_OUT_NGS_U.bc - MEAN_DATA.zfubc1;	
	}
*/
//-------------------����ǰ������ѹ�����ѹתΪ��ѹ------------------------------------------------------
//	AD_OUT_GRD_U.b  = (AD_OUT_GRD_U.bc - AD_OUT_GRD_U.ab) * 0.3333333;
//	AD_OUT_GRD_U.a  = AD_OUT_GRD_U.b + AD_OUT_GRD_U.ab;	
//	AD_OUT_GRD_U.c  = - AD_OUT_GRD_U.a - AD_OUT_GRD_U.b; 

//-------------------������ѹ������ѹתΪ��ѹ------------------------------------------------------
	AD_OUT_NGS_U.b  = (AD_OUT_NGS_U.bc - AD_OUT_NGS_U.ab) * 0.3333333;
	AD_OUT_NGS_U.a  = AD_OUT_NGS_U.b + AD_OUT_NGS_U.ab;	
	AD_OUT_NGS_U.c  = - AD_OUT_NGS_U.a - AD_OUT_NGS_U.b;

//-------------------�����Ӳ��ѹ������ѹ����ѹ------------------------------------------------------
	AD_OUT_STA_U.b  = (AD_OUT_STA_U.bc - AD_OUT_STA_U.ab) * 0.3333333;
	AD_OUT_STA_U.a  = AD_OUT_STA_U.b + AD_OUT_STA_U.ab;	
    AD_OUT_STA_U.c  = - AD_OUT_STA_U.a - AD_OUT_STA_U.b;

//---------------------ƽ��ֵ�˲�------------------------------------------------------------------	
	tempa = abs(AD_OUT_NPR_I.a);
	tempb = abs(AD_OUT_NPR_I.b);
	tempc = abs(AD_OUT_NPR_I.c);
	DataFilter(0.9999,&MEAN_DATA.ia1,tempa); //�������	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.ib1,tempb); //�������	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.ic1,tempc); //�������	ƽ��ֵ�˲�10S

	tempa = abs(AD_OUT_MPR_I.a);
	tempb = abs(AD_OUT_MPR_I.b);
	tempc = abs(AD_OUT_MPR_I.c);
	DataFilter(0.9999,&MEAN_DATA.ia2,tempa); //�������	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.ib2,tempb); //�������	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.ic2,tempc); //�������	ƽ��ֵ�˲�10S	

//	tempa = abs(AD_OUT_GRD_U.ab);
//	tempb = abs(AD_OUT_GRD_U.bc);
//	DataFilter(0.9999,&MEAN_DATA.uab,tempa); //����ǰ�����ѹ	ƽ��ֵ�˲�10S
//	DataFilter(0.9999,&MEAN_DATA.ubc,tempb); //����ǰ�����ѹ	ƽ��ֵ�˲�10S 

	tempa = abs(AD_OUT_NGS_U.ab);
	tempb = abs(AD_OUT_NGS_U.bc);
	DataFilter(0.9999,&MEAN_DATA.uab1,tempa); //�����ѹ	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.ubc1,tempb); //�����ѹ	ƽ��ֵ�˲�10S

	tempa = abs(AD_OUT_STA_U.ab);
	tempb = abs(AD_OUT_STA_U.bc);
	DataFilter(0.9999,&MEAN_DATA.uab2,tempa); //���Ӳ��ѹ	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.ubc2,tempb); //���Ӳ��ѹ	ƽ��ֵ�˲�10S


	tempa = abs(AD_OUT_NGS_U.a);
	tempb = abs(AD_OUT_NGS_U.b);
	tempc = abs(AD_OUT_NGS_U.c);
	DataFilter(0.9999,&MEAN_DATA.ua1,tempa); //�����ѹ	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.ub1,tempb); //�����ѹ	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.uc1,tempc); //�����ѹ	ƽ��ֵ�˲�10S

	tempa = abs(AD_OUT_NGF_U.ab);			  //MagnetCurve2013-12-13
	tempb = abs(AD_OUT_NGF_U.bc);
	DataFilter(0.9999,&MEAN_DATA.uab3,tempa); //���Ϻ������ѹ,���˲�ͨ��	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.ubc3,tempb); //���Ϻ������ѹ,���˲�ͨ��	ƽ��ֵ�˲�10S

//	tempa = abs(AD_OUT_NGF_U.ab);			  //MagnetCurve200909
//	tempb = abs(AD_OUT_NGF_U.bc);
//	DataFilter(0.9999,&MEAN_DATA.uab3,tempa); //���Ϻ������ѹ,���˲�ͨ��	ƽ��ֵ�˲�10S
//	DataFilter(0.9999,&MEAN_DATA.ubc3,tempb); //���Ϻ������ѹ,���˲�ͨ��	ƽ��ֵ�˲�10S

//--------------����ѹ�Ͷ��ӵ�ѹ��ֵƽ��ֵ------------------------------------------
    AD_OUT_STAD_U.ab = AD_OUT_NGF_U.ab - AD_OUT_STA_U.ab;
    AD_OUT_STAD_U.bc = AD_OUT_NGF_U.bc - AD_OUT_STA_U.bc;

	tempa = abs(AD_OUT_STAD_U.ab);
	tempb = abs(AD_OUT_STAD_U.bc);
	DataFilter(0.9999,&MEAN_DATA.uab_d,tempa);  //��ֵƽ��ֵ    ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.ubc_d,tempb);  //��ֵƽ��ֵ	ƽ��ֵ�˲�10S
	DataFilter(0.997,&MEAN_DATA.ubc_t,tempb);  //��ֵƽ��ֵ	ƽ��ֵ�˲�1S	//20130801
//----------------------------------------------------------------------------------
/*	//201204
	tempa = abs(AD_OUT_STA_I.ac);
	tempb = abs(AD_OUT_STA_I.ba);
	DataFilter(0.9999,&MEAN_DATA.iac3,tempa); //���Ӳ����	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.iba3,tempb); //���Ӳ����	ƽ��ֵ�˲�10S

	tempa = abs(AD_OUT_STA_I.a);
	tempb = abs(AD_OUT_STA_I.b);
	tempc = abs(AD_OUT_STA_I.c);
	DataFilter(0.9999,&MEAN_DATA.ia3,tempa); //���Ӳ����	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.ib3,tempb); //���Ӳ����	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.ic3,tempc); //���Ӳ����	ƽ��ֵ�˲�10S
*/

//--------------------------------------------------------------------------------------------------
//----------------------����ͻ������¶��˲�---------------------------------------------------------------------
	tempa = abs(AMUX.Lac_temp);
	tempb = abs(AMUX.Ldudt_temp);
	DataFilter(0.9999,&MEAN_DATA.Lac_temp,tempa); 	//�������¶�	ƽ��ֵ�˲�10S
	DataFilter(0.9999,&MEAN_DATA.Ldudt_temp,tempb); //�������¶�	ƽ��ֵ�˲�10S

//--------------------���Ų�����ʶMagnetCurve2013-12-13--ZZJ--------------------------------------------------------------
//BJTULVRT201204�������Uns_ft�Ƶ�input���ԼT0��Դ		
//	Uns_ft = 0.5 * ((MEAN_DATA.uab3/MEAN_DATA.uab1)+(MEAN_DATA.ubc3/MEAN_DATA.ubc1));//���˲���ֵ˥������
//	Uns_ft = 0.848;		//R=20k,C=0.1uF,fh=80Hz,����˥������Avh=0.848,ʵ�ʲ�����ʽ��������ֵΪ0.83���ң�2010-10-2

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
	else if(_SC_MSTDBY==99)//ȷ�����Ų�����ʶ���
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
			else if(MC_F_cnt > 5e4)		//��ʱ�ﵽ10s,��Ϊ�������߲��ʧ��,��Ҫ���²���
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
	else if(_SC_MSTDBY==100)		//�û�Ҫ�����²����������
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
	*AD_DA_CTRL     = AD8364_CONVERT;	               	  	//�����»ADת��
    CAP4.adsmptsctr = ECap4Regs.TSCTR;						//������ѹ����ʱ��
	QEPDATA.adsmposcnt=EQep2Regs.QPOSCNT;					//���������λ����Ϣ  

}

/*********************************************************************************************************
** ��������: Da5344Manage
** ����������The value of 4 da channels are chosen. 
** �䡡��: 	 
** �䡡��:   
** ע  ��: 	The da output is used to help debug or display
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:2012.4
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
//				     else   							zys[i] = 4095;	break;}  //=1LV���ද̬�������Ʊ�־λ 
			case 10:{if(M_ChkFlag(SL_LV_SYNCON)==0)		zys[i] = 2048;				
				     else   							zys[i] = 4095;	break;}  //=1LV���ද̬���ڱ�־λ
			case 11: {if(M_ChkFlag(SL_QCAPSPDIN)==0)			zys[i] = 2048;
					 else   							zys[i] = 4095;	break;}  
			case 12: {if(M_ChkFlag(SL_QCAPDISTURB)==0)		zys[i] = 2048;
				     else   							zys[i] = 4095;	break;}  //=1LV���ද̬�������Ʊ�־λ 
			case 13:{if(M_ChkFlag(SL_QEPPCO)==0)		zys[i] = 2048;				
				     else   							zys[i] = 4095;	break;}  //=1LV���ද̬���ڱ�־λ
//			case 14:{if(M_ChkFlag(SL_QEPPHE)==0)		zys[i] = 2048;				
//				     else   							zys[i] = 4095;	break;}  //=1LV���ද̬���ڱ�־λ
			case 15:{if(M_ChkFlag(SL_IN1_CBSTS)==0)		zys[i] = 2048;				
				     else   							zys[i] = 4095;	break;}  //=1LV���ද̬���ڱ�־λ
			case 16:{if(M_ChkFlag(SL_GRDFQE)==0)		zys[i] = 2048;				
				     else   							zys[i] = 4095;	break;}  //=1LV���ද̬���ڱ�־λ
			case 17:{if(M_ChkFlag(SL_HIDCOV)==0)		zys[i] = 2048;				
				     else   							zys[i] = 4095;	break;}  //=1
			case 18:{if(M_ChkFlag(SL_IDCOV)==0)			zys[i] = 2048;				
				     else   							zys[i] = 4095;	break;}  //=1
			case 19:{if(M_ChkFlag(SL_LV_SCRKEEPON)==0)	zys[i] = 2048;				
				     else   							zys[i] = 4095;	break;}  //=1


//------------------------------------------------------------------/�Ŵ��� magnify times----------------------
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

//-------------------20111208�����۲�------------------------------------------		
//-----------theta Phisdq ����idq
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


//------------��ƽ�� �ݲ���
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
** ��������: Output
** �������: 10·�ź����; 8·LED��ʾ���
** �䡡��: 	 
** �䡡��:   
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** ���:
** �ա���:
**------------------------------------------------------------------------------------------------------
***********************************************************************************************/
void Output(void)
{
//------------------------------�������-----------------------------------

   *OUT1_ADDR = _OUT1_DATA;
   *OUT2_ADDR = _OUT2_DATA;

//-----------------------------�����������⶯�����110818-------------------------

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
		M_ClrFlag(SL_SWITCHDETECT);									//���������������
		state_m=0;
		MAIN_LOOP.cnt_detectdelay = 0;
	}

	if(state_m==0)
	{
 	    if(M_ChkCounter(MAIN_LOOP.cnt_detectdelay,DELAY_DETECT)>=0)		//���������ת����������֮����ʱ_STDBY4 ms
 	    {
 	    	state_m=1;

			MAIN_LOOP.cnt_cberror=0;								//�嶨ʱ��
			MAIN_LOOP.cnt_mainkerror=0;								//�嶨ʱ��
			MAIN_LOOP.cnt_mainferror=0;								//�嶨ʱ��
			MAIN_LOOP.cnt_stacerror=0;								//�嶨ʱ��
		}

	}

	else if((action1 == action2) && (state_m==1))
	{
	    M_SetFlag(SL_SWITCHDETECT);									//�������⶯�����

	}

//---------20121107----------testCL_ZKLVRT--------
	if((_STDBY9&0x0002)!=0)		M_SetFlag(CL_ZKLVRT);	//116.1	atsp

//---------20121107----------testCL_ZKLVRT--------

//-----------------------------����ɢ�ȷ������----------------------------------
	if(M_ChkFlag(SL_FAN_WORK)!=0) 	M_SetFlag(CL_FANWORK);
	else							M_ClrFlag(CL_FANWORK); 
/*
//-----------------------------����ɢ�ȷ������--20100429systest--------------------------------
	if(M_ChkFlag(SL_FAN_WORK)!=0) 	
	{
		if(AMUX.skiiptempmax>35.0)	 	M_SetFlag(CL_FANWORK);          //��������������
		else if(AMUX.skiiptempmax<15.0)	M_ClrFlag(CL_FANWORK);          //�رչ���������
	}
	else
	{
//		if(AMUX.skiiptempmax<50.0)	 M_ClrFlag(CL_FANWORK);          	//�رչ���������
		M_ClrFlag(CL_FANWORK); 
	}							
*/
//---------------------CANOPEN�������ص�״̬����---------------------------------------------------
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



//-----------------------------��ʾ�����----------------------------------
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
** ��������: Input
** ��������: 16·�ź�����; 
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
void Input(void)
{
	Uint16 tempa,tempb,tempc,tempda,tempdb,tempdc;
	float temp_Usn,temp_Uro;
//--------------------------------��������----------------------------------
	tempa = *IN1_ADDR; 
	tempb = *IN2_ADDR;
	tempc = *IN3_ADDR;

	DELAY_US(100L);        //100us��ʱ��IO�������

	tempda = *IN1_ADDR;    
	tempdb = *IN2_ADDR;
	tempdc = *IN3_ADDR;

//--------------------------------��������----------------------------------	
	if((tempa==tempda)&&(tempb==tempdb))	
		_IN12_DATA = (tempa & 0x00FF) | ((tempb<<8) & 0xFF00);

	if(tempc==tempdc)		
		_IN34_DATA = (tempc & 0x00FF);

//---------------------CANOPENָ�����------------------------------------------------------

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
			M_SetFlag(SL_OCS_SYSRUN);	//20110707���ϻ���ͨѶ,ԭ2��ָ��϶�Ϊһ
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
			M_ClrFlag(SL_OCS_SYSRUN); 	//20110707���ϻ���ͨѶ,ԭ2��ָ����Ϊһ
		}
		MAIN_LOOP.cnt_ocsein1=0;
	}
//-----------------------------
*/

//---------------------��λ��Ҫ��λ����------------------------------------------------------
	if((SCI_canopen.rx_controlword & COM_OCSRESET)==COM_OCSRESET)	M_SetFlag(SL_OCS_RESET);									
	else 	   														M_ClrFlag(SL_OCS_RESET);													

/*	
//---------------------��λ��Ҫ�����������------------------------------------------------------
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
//---------------------��λ��Ҫ�����������-------20121206---------------------------------------
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

//---------------------��λ��Ҫ�������������������޹�����------------------------------------------------------
//	if((SCI_canopen.rx_controlword & COM_NPREIN)==COM_NPREIN)	M_SetFlag(SL_OCS_NPREIN);  //��ʱ����										
//	else 		  												M_ClrFlag(SL_OCS_NPREIN); 
}	 
//---------------------------���ղ���������PI������-------------------------------------------------
	PI_PARA_NPRU.kp           = _NPR_U_Kp/100.00;			//DOT2����
    PI_PARA_NPRU.ki           = _NPR_U_Ki/10.00;			//DOT1����
//	PI_PARA_NPRU.kd           = _NPR_U_Kd/1000.00;			//DOT3����
	PI_PARA_NPRU.outmax       = _NPR_U_outmax;
	PI_PARA_NPRU.errmax       = _NPR_U_errmax/10.00;		//DOT1���		//NPR��ѹ������
	PI_PARA_NPRU.errmin       = _NPR_U_errmin/1000.00;	    //DOT3����
	PI_PARA_NPRU.incrementmax = _NPR_U_incrementmax/10.00;  //DOT1����
/*
	PI_PARA_NPRID.kp           = _NPR_ID_Kp/100.00;			//DOT2����
    PI_PARA_NPRID.ki           = _NPR_ID_Ki/10.00;			//DOT1����
//    PI_PARA_NPRID.kd           = _NPR_ID_Kd/1000.00;			//DOT3����
	PI_PARA_NPRID.outmax       = _NPR_ID_outmax;		 					    //NPR����������
	PI_PARA_NPRID.errmax       = _NPR_ID_errmax/10.00;		//DOT1����
	PI_PARA_NPRID.errmin       = _NPR_ID_errmin/1000.00;		//DOT3����
	PI_PARA_NPRID.incrementmax = _NPR_ID_incrementmax/10.00;	//DOT1����
*/
//--------20121210--------
	lv_ctrl1           = _NPR_ID_Kp/100.00;			//DOT2����
    lv_ctrl2           = _NPR_ID_Ki/10.00;			//DOT1����
	lv_ctrl3	       = _NPR_ID_outmax;		 					    //NPR����������
	lv_ctrl4 			= _NPR_ID_errmax/10.00;		//DOT1����
	lv_ctrl5			= _NPR_ID_errmin/1000.00;		//DOT3����
	lv_ctrl6			= _NPR_ID_incrementmax/10.00;	//DOT1����

	PI_PARA_NPRID.kp           = 0.4;			//DOT2����
    PI_PARA_NPRID.ki           = 167;			//DOT1����
	PI_PARA_NPRID.outmax       = 200;		    //NPR����������
	PI_PARA_NPRID.errmax       = 50;			//DOT1����
	PI_PARA_NPRID.errmin       = 0;				//DOT3����
	PI_PARA_NPRID.incrementmax = 55;			//DOT1����
//--------20121210--------

	PI_PARA_NPRIQ.kp           = _NPR_IQ_Kp/100.00;			//DOT2����
    PI_PARA_NPRIQ.ki           = _NPR_IQ_Ki/10.00;			//DOT1����
//    PI_PARA_NPRIQ.kd           = _NPR_IQ_Kd/1000.00;			//DOT3����
	PI_PARA_NPRIQ.outmax       = _NPR_IQ_outmax;		 					    //NPR����������
	PI_PARA_NPRIQ.errmax       = _NPR_IQ_errmax/10.00;		//DOT1����
	PI_PARA_NPRIQ.errmin       = _NPR_IQ_errmin/1000.00;		//DOT3����
	PI_PARA_NPRIQ.incrementmax = _NPR_IQ_incrementmax/10.00;	//DOT1����

	PI_PARA_MPRID.kp           = _MPR_ID_Kp/100.00;			//DOT2����
    PI_PARA_MPRID.ki           = _MPR_ID_Ki/10.00;			//DOT1����
 //   PI_PARA_MPRID.kd           = _MPR_ID_Kd/1000.00;			//DOT3����
	PI_PARA_MPRID.outmax       = _MPR_ID_outmax;   							//MPR����������
	PI_PARA_MPRID.errmax       = _MPR_ID_errmax/10.00;		//DOT1����
	PI_PARA_MPRID.errmin       = _MPR_ID_errmin/1000.00;		//DOT3����
	PI_PARA_MPRID.incrementmax = _MPR_ID_incrementmax/10.00;	//DOT1����

	PI_PARA_MPRIQ.kp           = _MPR_IQ_Kp/100.00;			//DOT2����
    PI_PARA_MPRIQ.ki           = _MPR_IQ_Ki/10.00;			//DOT1����
//    PI_PARA_MPRIQ.kd           = _MPR_IQ_Kd/1000.00;			//DOT3����
	PI_PARA_MPRIQ.outmax       = _MPR_IQ_outmax;   							//MPR����������
	PI_PARA_MPRIQ.errmax       = _MPR_IQ_errmax/10.00;		//DOT1���
	PI_PARA_MPRIQ.errmin       = _MPR_IQ_errmin/1000.00;		//DOT3����
	PI_PARA_MPRIQ.incrementmax = _MPR_IQ_incrementmax/10.00;	//DOT1����
/*
	PI_PARA_MPRU.kp           = _MPR_U_Kp/100.00;			//DOT2����
    PI_PARA_MPRU.ki           = _MPR_U_Ki/10.00;			//DOT1����
    PI_PARA_MPRU.kd           = _MPR_U_Kd/1000.00;			//DOT3����
	PI_PARA_MPRU.outmax       = _MPR_U_outmax;   							//MPR��ѹ������
	PI_PARA_MPRU.errmax       = _MPR_U_errmax/10.00;		//DOT1����
	PI_PARA_MPRU.errmin       = _MPR_U_errmin/1000.00;		//DOT3����
	PI_PARA_MPRU.incrementmax = _MPR_U_incrementmax/10.00;  //DOT1����

	PI_PARA_DYNU.kp           = _DYN_U_Kp/100.00;			//DOT2����
    PI_PARA_DYNU.ki           = _DYN_U_Ki/10.00;			//DOT1����
    PI_PARA_DYNU.kd           = _DYN_U_Kd/1000.00;			//DOT3����
	PI_PARA_DYNU.outmax       = _DYN_U_outmax;   							//��̬��ѹ������
	PI_PARA_DYNU.errmax       = _DYN_U_errmax/10.00;		//DOT1����
	PI_PARA_DYNU.errmin       = _DYN_U_errmin/1000.00;		//DOT3����
	PI_PARA_DYNU.incrementmax = _DYN_U_incrementmax/10.00;  //DOT1����
*/
//----�����ѹ��������̬PI���̶�ֵBJTULVRT201204
	PI_PARA_DYNU.kp           = 0.15;						//DOT2����
    PI_PARA_DYNU.ki           = 3;							//DOT1����
    PI_PARA_DYNU.kd           = 0;							//DOT3����
	PI_PARA_DYNU.outmax       = 600;   						//��̬��ѹ������
	PI_PARA_DYNU.errmax       = 100;						//DOT1����
	PI_PARA_DYNU.errmin       = 0;							//DOT3����
	PI_PARA_DYNU.incrementmax = 100;  						//DOT1����
//-----���ද̬PI����BJTULVRT201204	_MPR_U_Kp�ڵ��Խ�����û�� ��ʱ����,��ѡPR
	PI_PARA_DYMI.kp           = _DYN_U_Kp/100.00;			//DOT2����
    PI_PARA_DYMI.ki           = _DYN_U_Ki/10.00;			//DOT1����
    PI_PARA_DYMI.kd           = _DYN_U_Kd/1000.00;			//DOT3����
	PI_PARA_DYMI.outmax       = _DYN_U_outmax;   							//MPR��ѹ������
	PI_PARA_DYMI.errmax       = _DYN_U_errmax/10.00;		//DOT1����
	PI_PARA_DYMI.errmin       = _DYN_U_errmin/1000.00;		//DOT3����
	PI_PARA_DYMI.incrementmax = _DYN_U_incrementmax/10.00;  //DOT1����
//----ͬʱ������̬PI��PR�����趨

	_eidco   = _EIDCO/1000.00;
	_encodpos= _ENCODPOS/1000.00;
	_sc_freq1= _SC_FREQ1/10.00;
	_sc_freq2= _SC_FREQ2/10.00;
	_stdby01 = _STDBY1/1000.00;								//���þ���С��㴦���ֵ
	_stdby02 = _STDBY2/100.00;								//���þ���С���㴦���ֵ
	_stdby03 = _STDBY3/10.00;								//���þ���С���㴦���ֵ
    _stdby04 = _STDBY4;			        					//����
	_stdby05 = _STDBY5;			        					//����
    _stdby06 = _STDBY6;			        					//����
	_stdby07 = _STDBY7;			        					//����
//    _stdby08 = _STDBY8;			        					//����
//���㶨�ӵ�ѹAD���˲�����2013-12-13--ZZJ
	Uns_ft = 0.5 * ((MEAN_DATA.uab3/MEAN_DATA.uab1)+(MEAN_DATA.ubc3/MEAN_DATA.ubc1));//���˲���ֵ˥������

	daindex[0] = (Uint16)_NPR_U_Kd;	//da output select BJTULVRT201204
	daindex[1] = (Uint16)_NPR_ID_Kd;
	daindex[2] = (Uint16)_NPR_IQ_Kd;
	daindex[3] = (Uint16)_MPR_ID_Kd;

//ת�ӵ��������˲����� BJTULVRT201204
	BS_PARA_MPRI.f0 = 300;
	BS_PARA_MPRI.fb = 50;
	BS_PARA_MPRI.outmax = 1800;//201205LVRTatZB
//���ӵ��������˲����� BJTULVRT201204
	BS_PARA_STAI.f0 = 50;
	BS_PARA_STAI.fb = 40;
	BS_PARA_STAI.outmax = 8000; //201205LVRTatZB

//��������趨201005atcpc

	MPR_Ls = (_SC_Lls * 1e-6)+(_SC_Lm * 1e-6);				//�����Ը�	--��λH
	MPR_Lr = (_SC_Llr * 1e-6)+(_SC_Lm * 1e-6);				//ת���Ը�	--��λH	
	MPR_Lm = _SC_Lm * 1e-6;									//��ת�ӻ���--��λH
	MPR_Rs = _SC_Rs * 1e-6;									//���ӵ���	--��λohm
	MPR_Rr = _SC_Rr * 1e-6;									//ת�ӵ���	--��λohm
	SIGMA  = 1- (MPR_Lm * MPR_Lm /(MPR_Ls * MPR_Lr));		//(1-MPR_Lm*MPR_Lm/(MPR_Ls*MPR_Lr))	
	
	temp_Usn  = (float)_SC_Usn;								//���Ӷ��ѹ(V)
	temp_Uro  = (float)_SC_Uro;								//ת�ӿ�·��ѹ(V)
	STAROTRTO = temp_Usn * SQRT3 / temp_Uro;				//��ת�ӱ�� ֻ���ö��ӽǽ�ת���ǽӵĵ��
	STAROTRTO2= STAROTRTO * STAROTRTO;						//��ת�ӱ�ȵ�ƽ��
	POLEPAIRES= (float)_SC_POLES;							//������

 	if(_SC_PLSPRVL != 2048)									//201007BJTULVRT �������ȱʡֵ2048,�����¸�ֵ
	{
		if(EQep2Regs.QPOSMAX != (4 * _SC_PLSPRVL))
		{
			EALLOW;			
 			EQep2Regs.QPOSMAX   = 4 * _SC_PLSPRVL;			// λ�ü��������ֵ
			EDIS;	
		}
	}
   
} 
/*********************************************************************************************************
** ��������: Disepwmio_NPR
** ��������: 
** �䡡��: 	 
** �䡡��:   
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** �����: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
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
** ��������: Disepwmio_MPR
** ��������: 
** ���? 	 
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
** �������ƣ� DisPwm
** ���������������ֹ
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
void DisPwm(void)
{
	Disepwmio_NPR();
	Disepwmio_MPR();
} 


/*********************************************************************************************************
** ��������: Enepwmio_NPR
** ��������: 
** �䡡��: 	 
** �䡡��:   
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա�?
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
** �������ƣ�Enepwmio_MPR
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
** ��������: ConfigPwm
** ��������: ����PWM����Ƶ�ʺ�����ʱ�������
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
void ConfigPwm(void)						
{
	Disepwmio_NPR();
	Disepwmio_MPR();
	EALLOW;
//----------NPR��PWM1-PWM6����---------------//
    EPwm1Regs.TBPRD = 3750000/_SW_FR;           // ����Ƶ��Ϊ�����,_SW_FR=200��Ӧ2kHz
    EPwm2Regs.TBPRD = 3750000/_SW_FR;           // PWMʱ��Ϊ75MHz
	EPwm3Regs.TBPRD = 3750000/_SW_FR;           // TBPRD = Time-base Period Register
    SW_NPR=3750000.0/_SW_FR;

    EPwm1Regs.DBRED = 75 * _DEADTIME;              //������_DEADTIME��λΪus 
    EPwm1Regs.DBFED = 75 * _DEADTIME;              //
    EPwm2Regs.DBRED = 75 * _DEADTIME;              //DBRED = Dead-Band Generator Rising Edge Delay Count Register
    EPwm2Regs.DBFED = 75 * _DEADTIME;              //DBFED = Dead-Band Generator Falling Edge Delay Count Register
    EPwm3Regs.DBRED = 75 * _DEADTIME;
    EPwm3Regs.DBFED = 75 * _DEADTIME;   

//----------MPR��PWM7-PWM12����---------------//
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
** ��������: EnPdpint
** ��������: 
** �䡡��: 	 
** �䡡��:   
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա�? 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
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
** ��������: DisPdpint
** ��������: 
** �䡡��: 	 
** �䡡:   
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** �����: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
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
** ��������: ClrPdpint
** ��������: 
** �䡡��: 	 
** �䡳?   
** ע  ��: 	 
**-------------------------------------------------------------------------------------------------------
** ������: 
** �ա���: 
**-------------------------------------------------------------------------------------------------------
** �޸���:
** �ա���:
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
