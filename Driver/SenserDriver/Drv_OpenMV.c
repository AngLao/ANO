//Ĭ�����ã�
#include "Drv_OpenMV.h"

//�趨
#define OPMV_OFFLINE_TIME_MS  1000  //����

//ȫ�ֱ���
u16 offline_check_time;
u16 offline_check_time2g;
u8 openmv_buf[20];
u8 openmv2gnd_buf[20];
_openmv_data_st opmv;
_openmv_data_st opmv2g;
/**********************************************************************************************************
*�� �� ��: OpenMV_Byte_Get
*����˵��: OpenMV�ֽ����ݻ�ȡ
*��    ��: �ֽ�����
*�� �� ֵ: ��
**********************************************************************************************************/
void OpenMV_Byte_Get(u8 bytedata)
{	
//	static u8 len = 0,rec_sta;
//	u8 check_val=0;
//	
//	//
//	openmv_buf[rec_sta] = bytedata;
//	//
//	if(rec_sta==0)
//	{
//		if(bytedata==0xaa)
//		{
//			rec_sta++;
//		}
//		else
//		{
//			rec_sta=0;
//		}
//	}
//	else if(rec_sta==1)
//	{
//		if(1)//(bytedata==0x29)δȷ��
//		{
//			rec_sta++;
//		}	
//		else
//		{
//			rec_sta=0;
//		}		
//	}
//	else if(rec_sta==2)
//	{
//		if(bytedata==0x05)
//		{
//			rec_sta++;
//		}
//		else
//		{
//			rec_sta=0;
//		}		
//	}
//	else if(rec_sta==3)
//	{
//		if(bytedata==0x41 || bytedata==0x42)
//		{
//			rec_sta++;
//		}
//		else
//		{
//			rec_sta=0;
//		}		
//	}
//	else if(rec_sta==4)
//	{
//		//
//		len = bytedata;
//		if(len<20)
//		{
//			rec_sta++;
//		}		
//		else
//		{
//			rec_sta=0;
//		}
//	}
//	else if(rec_sta==(len+5))
//	{
//		//
//		for(u8 i=0;i<len+5;i++)
//		{
//			check_val += openmv_buf[i];
//		}
//		//
//		if(check_val == bytedata)
//		{
//			//�����ɹ�
//			OpenMV_Data_Analysis(openmv_buf,len+6);
//			//
//			rec_sta=0;
//		}
//		else
//		{
//			rec_sta=0;
//		}		
//	}
//	else
//	{
//		//	
//		rec_sta++;
//	}

/////////////////////////////////////////////////////////
    /* ���ݳ��� *//* ���������±� */
    static u8  _data_cnt = 0;
    /* ����״̬ */
    static u8 state = 0;
		
	 /* ֡ͷ1 */
    if(state==0&&bytedata==0xAA)
    {
        state=1;
    }

    /* ֡ͷ2 */
    else if(state==1&&bytedata==0xAE)
    {
        state=2;
        _data_cnt = 0;
    }
		
    /* ���������� */
    else if(state==2)
    {
        openmv_buf[_data_cnt++]=bytedata;
        if(_data_cnt>=16)
        {
            state = 0;
            OpenMV_Data_Analysis(openmv_buf,_data_cnt);
        }
    }

    /* ���д������µȴ�����֡ͷ */
    else
        state = 0;	
}

/**********************************************************************************************************
*�� �� ��: OpenMV_Data_Analysis
*����˵��: OpenMV���ݽ���
*��    ��: �������ݣ��βΣ�������
*�� �� ֵ: ��
**********************************************************************************************************/
#include "Ano_DT.h"
u8 is_t;
static void OpenMV_Data_Analysis(u8 *buf_data,u8 len)
{
//	if(*(buf_data+3)==0x41)
//	{
//		opmv.cb.color_flag = *(buf_data+5);
//		opmv.cb.sta = *(buf_data+6);
//		opmv.cb.pos_x = (s16)((*(buf_data+7)<<8)|*(buf_data+8));
//		opmv.cb.pos_y = (s16)((*(buf_data+9)<<8)|*(buf_data+10));
//		//
//		opmv.mode_sta = 1;
//	}
//	else if(*(buf_data+3)==0x42)
//	{
//		opmv.lt.sta = *(buf_data+5);
//		opmv.lt.angle = (s16)((*(buf_data+6)<<8)|*(buf_data+7));
//		opmv.lt.deviation = (s16)((*(buf_data+8)<<8)|*(buf_data+9));
//		opmv.lt.p_flag = *(buf_data+10);
//		opmv.lt.pos_x = (s16)((*(buf_data+11)<<8)|*(buf_data+12));
//		opmv.lt.pos_y = (s16)((*(buf_data+13)<<8)|*(buf_data+14));
//		
//		//
//		opmv.mode_sta = 2;
//	}
//	//
//	OpenMV_Check_Reset();
////////////////////////////////////////////////////////////////////

	int cb_sta_temp,pos_x_temp,pos_y_temp;
	int lt_sta_temp,deviation_temp,angle_temp,t_temp;
	
	if(*(buf_data+0)==0x21)//Ѱ��
	{
		cb_sta_temp   = (int)(*(buf_data+1)<<0) | (int)(*(buf_data+2)<<8)  | (int)(*(buf_data+3)<<16)  | (int)(*(buf_data+4)<<24);
		pos_x_temp = (int)(*(buf_data+5)<<0) | (int)(*(buf_data+6)<<8)  | (int)(*(buf_data+7)<<16)  | (int)(*(buf_data+8)<<24);
		pos_y_temp = (int)(*(buf_data+9)<<0) | (int)(*(buf_data+10)<<8) | (int)(*(buf_data+11)<<16) | (int)(*(buf_data+12)<<24);
		
		opmv.cb.sta   = (u8)(cb_sta_temp);
		opmv.cb.pos_x = (s16)pos_x_temp;
		opmv.cb.pos_y = (s16)pos_y_temp;
		//
		opmv.mode_sta = 1;
		
	}
	
	else if(*(buf_data+0)==0x41)//Ѱ��
	{
		
		t_temp         = 	(int)(*(buf_data+1)<<0)  | (int)(*(buf_data+2)<<8)  | (int)(*(buf_data+3)<<16)  | (int)(*(buf_data+4)<<24);
		lt_sta_temp    =  (int)(*(buf_data+5)<<0)  | (int)(*(buf_data+6)<<8)  | (int)(*(buf_data+7)<<16)  | (int)(*(buf_data+8)<<24);
		angle_temp     =  (int)(*(buf_data+9)<<0)  | (int)(*(buf_data+10)<<8) | (int)(*(buf_data+11)<<16) | (int)(*(buf_data+12)<<24);
		deviation_temp =  (int)(*(buf_data+13)<<0) | (int)(*(buf_data+14)<<8) | (int)(*(buf_data+15)<<16) | (int)(*(buf_data+16)<<24);
			
		is_t = (u8)t_temp;
		opmv.lt.sta = (u8)lt_sta_temp;
		opmv.lt.angle  = (s16)angle_temp;
		opmv.lt.deviation = (s16)deviation_temp;
		
		//
		opmv.mode_sta = 2;
	}
	//
	OpenMV_Check_Reset();
}

/**********************************************************************************************************
*�� �� ��: OpenMV_Offline_Check
*����˵��: OpenMV���߼�⣬�������Ӳ���Ƿ�����
*��    ��: ʱ�䣨���룩
*�� �� ֵ: ��
**********************************************************************************************************/
void OpenMV_Offline_Check(u8 dT_ms)
{
	if(offline_check_time<OPMV_OFFLINE_TIME_MS)
	{
		offline_check_time += dT_ms;
	}
	else
	{
		opmv.offline = 1;
		opmv.mode_sta = 0;
	}
	
}

void OpenMV2G_Offline_Check(u8 dT_ms)
{
	if(offline_check_time2g<OPMV_OFFLINE_TIME_MS)
	{
		offline_check_time2g += dT_ms;
	}
	else
	{
		opmv2g.offline = 1;
		opmv2g.mode_sta = 0;
	}
}

/**********************************************************************************************************
*�� �� ��: OpenMV_Check_Reset
*����˵��: OpenMV���߼�⸴λ��֤��û�е���
*��    ��: ��
*�� �� ֵ: ��
**********************************************************************************************************/
static void OpenMV_Check_Reset()
{
	offline_check_time = 0;
	opmv.offline = 0;
}

static void OpenMV2G_Check_Reset()
{
	offline_check_time2g = 0;
	opmv2g.offline = 0;
}

void OpenMV2GND_Byte_Get(u8 bytedata)
{
	  /* ���ݳ��� *//* ���������±� */
    static u8  _data_cnt = 0;
    /* ����״̬ */
    static u8 state = 0;
		
	 /* ֡ͷ1 */
    if(state==0&&bytedata==0xAA)
    {
        state=1;
    }

    /* ֡ͷ2 */
    else if(state==1&&bytedata==0xAE)
    {
        state=2;
        _data_cnt = 0;
    }
		
    /* ���������� */
    else if(state==2)
    {
        openmv2gnd_buf[_data_cnt++]= bytedata;
        if(_data_cnt>=12)
        {
            state = 0;
            OpenMV2GND_Data_Analysis(openmv2gnd_buf,_data_cnt);
        }
    }

    /* ���д������µȴ�����֡ͷ */
    else
        state = 0;	
	
}


void OpenMV2GND_Data_Analysis(u8 *buf_data,u8 len)
{	
	int cb_sta_temp_2g,pos_x_temp_2g,pos_y_temp_2g;
	int lt_sta_temp_2g,deviation_temp_2g,angle_temp_2g,t_temp_2g;
	
	if(*(buf_data+0)==0x21)//Ѱ��
	{
		cb_sta_temp_2g   = (int)(*(buf_data+1)<<0) | (int)(*(buf_data+2)<<8)  | (int)(*(buf_data+3)<<16)  | (int)(*(buf_data+4)<<24);
		pos_x_temp_2g = (int)(*(buf_data+5)<<0) | (int)(*(buf_data+6)<<8)  | (int)(*(buf_data+7)<<16)  | (int)(*(buf_data+8)<<24);
		pos_y_temp_2g = (int)(*(buf_data+9)<<0) | (int)(*(buf_data+10)<<8) | (int)(*(buf_data+11)<<16) | (int)(*(buf_data+12)<<24);
		//��
		opmv2g.cb.sta   = (u8)(cb_sta_temp_2g);
		opmv2g.cb.pos_x = (s16)pos_x_temp_2g;
		opmv2g.cb.pos_y = (s16)pos_y_temp_2g;
		//
		opmv2g.mode_sta = 1;		
	}
	
	else if(*(buf_data+0)==0x41)//Ѱ��
	{
		
		t_temp_2g         =  (int)(*(buf_data+1)<<0)  | (int)(*(buf_data+2)<<8)  | (int)(*(buf_data+3)<<16)  | (int)(*(buf_data+4)<<24);
		lt_sta_temp_2g    =  (int)(*(buf_data+5)<<0)  | (int)(*(buf_data+6)<<8)  | (int)(*(buf_data+7)<<16)  | (int)(*(buf_data+8)<<24);
		angle_temp_2g     =  (int)(*(buf_data+9)<<0)  | (int)(*(buf_data+10)<<8) | (int)(*(buf_data+11)<<16) | (int)(*(buf_data+12)<<24);
		deviation_temp_2g =  (int)(*(buf_data+13)<<0) | (int)(*(buf_data+14)<<8) | (int)(*(buf_data+15)<<16) | (int)(*(buf_data+16)<<24);
			
		//is_t = (u8)t_temp_2g;
  	opmv2g.lt.sta = (u8)lt_sta_temp_2g;
		opmv2g.lt.angle  = (s16)angle_temp_2g;
		opmv2g.lt.deviation = (s16)deviation_temp_2g;
		
		//
		opmv2g.mode_sta = 2;
	}
	//
	OpenMV2G_Check_Reset();
		
}
