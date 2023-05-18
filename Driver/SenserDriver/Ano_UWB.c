//==����
#include "Ano_UWB.h"
#include "Drv_Uart.h"
#include "ring_buffer.h"
#include "nlink_utils.h"
#include "nlink_linktrack_tagframe0.h"

#include "Ano_Imu.h"
#include "Ano_FcData.h"

#define fc_sta flag

//==��������
_uwb_data_st uwb_data;
  

/**********************************************************************************************************
*�� �� ��: UWB_Get_Data_Task                         
*����˵��: UWB���ݻ�ȡ����
*��    ��: ���ڣ����룩
*�� �� ֵ: ��
**********************************************************************************************************/

//�Ƴ�����UWB����,������TPB UWB����
static u16 uwb_check_time;
void UWB_Get_Data_Task(u8 dT_ms)
{ 
	//�����λ����� 
	static  int RingBufferDataLen = 0;
	static unsigned char pData[1280]; 
	RingBufferDataLen = RingBuffer_GetCount(&U1rxring) ;
	//����uwb����
	if(RingBufferDataLen){
		RingBuffer_PopMult(&U1rxring, pData,RingBufferDataLen);
		if (g_nlt_tagframe0.UnpackData(pData, RingBufferDataLen))
		{
//			printf("x:%d  y:%d\r\n",(int)g_nlt_tagframe0.result.pos_3d[0]
//														 ,(int)g_nlt_tagframe0.result.pos_3d[1] );

//			uwb_data.raw_data_loc[1] = -(float)(s16)((*(UWB_RxBuffer+6)<<8)|*(UWB_RxBuffer+7)) / 100;
//			uwb_data.raw_data_loc[0] =  (float)(s16)((*(UWB_RxBuffer+8)<<8)|*(UWB_RxBuffer+9)) / 100;
//			uwb_data.raw_data_loc[2] =  (float)(s16)((*(UWB_RxBuffer+10)<<8)|*(UWB_RxBuffer+11)) / 100;
//			uwb_data.raw_data_vel[1] = -(float)(s16)((*(UWB_RxBuffer+12)<<8)|*(UWB_RxBuffer+13)) / 100;
//			uwb_data.raw_data_vel[0] =  (float)(s16)((*(UWB_RxBuffer+14)<<8)|*(UWB_RxBuffer+15)) / 100;
//			uwb_data.raw_data_vel[2] =  (float)(s16)((*(UWB_RxBuffer+16)<<8)|*(UWB_RxBuffer+17)) / 100;
			//UWB����
//			uwb_check_time = 0;	
//		  uwb_data.online = 1;		
			return;
		}  
	}
	
//	//UWB���߼�ʱ,����ʱ��δ�յ�UWB�����Ŷ�Ϊ����
//	if(uwb_check_time <1000){
//		uwb_check_time += dT_ms;
//	}else{
//		uwb_data.online = 0;
//	}
	
}

/**********************************************************************************************************
*�� �� ��: Ano_UWB_Data_Calcu_Task
*����˵��: UWB���ݼ�������
*��    ��: ���ڣ����룩
*�� �� ֵ: ��
**********************************************************************************************************/
void Ano_UWB_Data_Calcu_Task(u8 dT_ms)
{
	//����ǰ����¼��ǰ����X����������ˮƽ��ͶӰΪUWB����X��������
	//Ҫ�����ǰ���ɻ�����������X���������׼UWB������X��������
	//��¼�ο�����
	if(!fc_sta.unlock_sta)
	{
		//
		uwb_data.init_ok = 0;
		//
		uwb_data.ref_dir[X] = imu_data.hx_vec[X];
		uwb_data.ref_dir[Y] = imu_data.hx_vec[Y];

	}
	//
	else
	{
		//
		uwb_data.init_ok = 1;
	}
	//�ο�����ת�������꣨�����ͬ�ڵ������꣩
	h2w_2d_trans(uwb_data.raw_data_loc,uwb_data.ref_dir,uwb_data.w_dis_cm);
	//�����ٶ�
	h2w_2d_trans(uwb_data.raw_data_vel,uwb_data.ref_dir,uwb_data.w_vel_cmps);
	
}
