/******************** (C) COPYRIGHT 2017 ANO Tech ********************************
 * ����    �������ƴ�
 * ����    ��www.anotc.com
 * �Ա�    ��anotc.taobao.com
 * ����QȺ ��190169595
 * ����    ������ģ���ȡ
**********************************************************************************/
#include "Drv_laser.h"
#include "Ano_FcData.h"
#include "Drv_Uart.h"


#define  Min_err_hei	 (-5.0f)
#define  Max_err_hei   (20.0f)

u8 LASER_LINKOK = 0;	//0:��Ч��1����Ч��2��������

u16 Laser_height_cm;
u8 Drv_Laser_Init(void)
{
	LASER_LINKOK = 2;
	for(u16 i=0; i<1000; i++)
	{
		if(LASER_LINKOK == 1)
			break;
		else
			Delay_ms(1);
	}
	if(LASER_LINKOK == 2)
		LASER_LINKOK = 0;
	sens_hd_check.tof_ok = LASER_LINKOK;
	return LASER_LINKOK;
}

void Drv_Laser_GetOneByte(u8 data)
{
	static u8 tmp[9];
	static u8 sta = 0;
	static u8 cnt = 0;
	
	if(sta == 0 && data == 0x59)
	{
		tmp[0] = 0x59;
		sta = 1;
	}
	else if(sta == 1)
	{
		if(data == 0x59)
		{
			tmp[1] = 0x59;
			sta = 2;
			cnt = 2;
		}
		else
			sta = 0;
	}
	else if(sta == 2)
	{
		tmp[cnt++] = data;
		if(cnt >= 9)
		{
			u8 sum = 0;
			for(u8 i=0; i<8; i++)
				sum += tmp[i];
			if(sum == tmp[8])	//У��ͨ��
			{
				if(LASER_LINKOK == 2)
					LASER_LINKOK = 1;
				Laser_height_cm = (tmp[2] + ((u16)tmp[3]<<8));
			}
			sta = 0;
			cnt = 0;
		}
	}
}


////////////////////////////////����ΪUS100 KS103�������ײ�///////////////////////////////////////////////////////
s8 ultra_start_f;
u8 ultra_time;
u8 ultra_ok = 0;

u16 last_hei;
float dlt_hei;

_height_st ultra;

/*���������պ������ڴ��ڽ����ж��ڵ���*/
//ultra.measure_ok    0  ��Ч   1  ��Ч
void Ultra_Get(u8 com_data)
{
	static u8 ultra_tmp;
	
	if( ultra_start_f == 1 )
	{
		ultra_tmp = com_data;
		ultra_start_f = 2;
	}
	else if( ultra_start_f == 2 )
	{
		Laser_height_cm =  ((ultra_tmp<<8) + com_data)/10;//��λΪcm
		
		if(Laser_height_cm < 500) // 5�׷�Χ����Ϊ��Ч������ֵԼ10��.
		{
			ultra.measure_ok = 1;
			dlt_hei = Laser_height_cm - last_hei;
			
			if(dlt_hei < Min_err_hei)
			{
				Laser_height_cm = last_hei + Min_err_hei;
			}
			
			else if(dlt_hei > Max_err_hei)
			{
			 Laser_height_cm = last_hei + Max_err_hei;
			}
			
			last_hei =  Laser_height_cm;
		}
		
		else
		{
			ultra.measure_ok = 0; 
			
			Laser_height_cm = last_hei ;
							
		}
		ultra_start_f = 0;
		
	}
	ultra.measure_ot_cnt = 0; //�����ʱ������ι����
}


/*�������������*/

void Ultra_Duty()
{
	u8 temp[3];
	
	ultra.h_dt = 0.05f; //50msһ��
	
	ultra_time++;
	ultra_time = ultra_time%2;//50msһ�Σ���100ms
	
	if( ultra_time == 0 )  //100ms//���÷����жϣ���ʡʱ�䡣
	{
  /*
		//////////////////////////////////////////////
		UART5->DR = 0xe8;   //ks103��ַ�������ã�
		while( (UART5->SR & USART_FLAG_TXE) == 0 );
		
		UART5->DR = 0x02;   //++++
		while( (UART5->SR & USART_FLAG_TXE) == 0 );

		UART5->DR = 0xbc;  //70ms,���¶Ȳ���
		while( (UART5->SR & USART_FLAG_TXE) == 0 );
		//////////////////////////////////////////////
	*/	
		
	#if defined(USE_KS103)
		temp[0] = 0xe8;
		temp[1] = 0x02;
		temp[2] = 0xbc;
		Drv_Uart5SendBuf(temp ,3);
	#elif defined(USE_US100)
		temp[0] = 0x55;
		Drv_Uart5SendBuf(temp ,1);
	#endif
///////////////////////////////////////////////
	if(ultra.measure_ot_cnt<200) //200ms
	{
		ultra.measure_ot_cnt += ultra.h_dt *1000;
	}
	else
	{
		ultra.measure_ok = 0;//��ʱ����λ
	}
		ultra_start_f = 1;
	}
}

