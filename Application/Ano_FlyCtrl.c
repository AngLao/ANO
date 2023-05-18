#include "Ano_FlyCtrl.h"
#include "Ano_DT.h"
#include "Ano_LocCtrl.h"
#include "Drv_OpenMV.h"
#include "Ano_OPMV_LineTracking_Ctrl.h"
#include "Ano_ProgramCtrl_User.h"

#include "nlink_linktrack_tagframe0.h"
#include "nlink_utils.h"

#include "game_map.h"
#include "Drv_Uart.h"

#define  T_CONFIRM_TIMES    10
#define  STEP_CONFIRM_TIMES 40

_fly_ct_st program_ctrl; 

#include "Ano_FcData.h"
#include "Ano_FlightCtrl.h"
#include "ANO_IMU.h"
#include "Drv_Uart.h"

//自添加代码
_onekey_ct_st onekey;
 
unsigned char broadcasting_Task(unsigned char dT_ms){
				static int  time =0;
				static unsigned char tick =0;
				
				if( time == 0 ){
//					printf("flash light\r\n"  );  
					ROM_GPIOPinWrite(GPIOF_BASE, GPIO_PIN_0, 1); 
				}
				
				time += dT_ms;
				//播洒持续时间
				#define continue_time 200
				if( time == continue_time ){ 
//					printf("close light\r\n"  );  
					ROM_GPIOPinWrite(GPIOF_BASE, GPIO_PIN_0, 0); 
					time = -continue_time; 
					//播洒次数
					
					if(++tick == 1){
					  time = 0; 
						tick = 0;
						return 0; //播洒结束
					}
				}  
				return 1; //任务正在运行
}

unsigned char UWBTest_Task(unsigned char dT_ms){
	
				//执行播洒任务状态
				static unsigned char is_broadcasting = 	0;
				//当前目标
				static unsigned char targe_index = 0; 
				//执行任务点顺序
				static unsigned char targe_buf[] = {
																						21 , 28 , 27 , 20 , 19  , 26 ,
																						25 , 18 , 14 , 10 ,  8  ,  4 ,
																						 3 ,  7 ,  9 , 13 , 17  , 24 ,  
																						23 , 16 , 12 ,  6 ,  2  ,
																						22 , 15 , 11 ,  5 ,  1  ,
//																					4,8,10,14,18,25,	24,23,22,15,11,5,1,2,3,4
																						0
				}; 
				//执行任务点个数
				static int targe_len =	sizeof(targe_buf)/sizeof(targe_buf[0]);
	
				//起飞状态为初始状态，且遥控有信号,且光流或者GPS有效
				if(flag.auto_take_off_land == AUTO_TAKE_OFF_NULL)
				{	
					static char take_off_flag=0;
					//飞控状态正常 且为第一次起飞
					if(flag.unlock_err == 0 && take_off_flag == 0 )
					{ 
						take_off_flag = 1;
						//一键起飞
						printf("Take off start\r\n"  );  
						printf("targe_index:%d\r\n",targe_index );  
						printf("x %d y %d\r\n",plant_map.dot[targe_buf[targe_index]].y,
																			plant_map.dot[targe_buf[targe_index]].x ); 
						one_key_take_off();
					}

				}
				static unsigned int take_off_delay = 0;
				//起飞后延时8S
				if(take_off_delay!=8000){
					take_off_delay += dT_ms;
				}else{
					//起飞完成执行到达指定位置任务 单位毫米 
					if(flag.auto_take_off_land == AUTO_TAKE_OFF_FINISH && is_broadcasting == 0)
					{
						//目标值
						int this_targe = targe_buf[targe_index];
						int targe_pos_x = plant_map.dot[this_targe].y;
						int targe_pos_y = plant_map.dot[this_targe].x;
						
						//测量值
						int mesure_pos_x = (int)g_nlt_tagframe0.result.pos_3d[0];
						int mesure_pos_y = (int)g_nlt_tagframe0.result.pos_3d[1];
						
						//计算误差值 
						int error_pos_x = mesure_pos_x - targe_pos_x;
						int error_pos_y = mesure_pos_y - targe_pos_y;
						
						
						static unsigned int over_time = 0;
						
						static unsigned char allow_error = 50;  //x,y所允许的误差单位为 (mm)
						//在误差运行范围内() 到达了指定地点 
						if(abs(error_pos_x) < allow_error && abs(error_pos_y) < allow_error){
							
							//依靠光流悬停
							Program_Ctrl_User_Set_HXYcmps(0,0);
							//在指定位置悬停了多少时间 单位(mm)
							if( over_time == 200 ){
								
								//所有位置都已走完结束飞行
								if(targe_index == targe_len-1){
									/*一键降落*/
									one_key_land(); 
									printf("886\r\n"); 
									//任务运行结束	+++++++++++++++++++++++++++++++++++++++++
									return 0; 
								}
								
								//根据MV结果判断是否播洒 如果res=0则是在白色区域，res=1是起飞点，res=2是A点，res=3是绿色块
								printf("openMV_res:%d\r\n",openMV_res );
								openMV_res=2;								
								if(openMV_res > 1){//需要执行任务 
									//开始执行播洒任务
									is_broadcasting = 1 ;
								}else{
									targe_index += 1; //索引加一 将目标点设置为下一个点位 
									printf("targe_dot:%d\r\n",targe_buf[targe_index] );  
									printf("x %d y %d\r\n",plant_map.dot[targe_buf[targe_index]].y,
																						plant_map.dot[targe_buf[targe_index]].x ); 
								}
								over_time = 0;
								//保持执行悬停任务状态 返回结果不进行时间累加
								return 1; 
							}
							
							//20ms 的执行频率
							over_time+=dT_ms;
						}else{
							//误差过大
							over_time=0;
							 
							//根据误差进行比例缩放为速度控制值
							//左前移动速度为正
							Program_Ctrl_User_Set_HXYcmps(-error_pos_y*0.05  , error_pos_x*0.05);
						}
					} 
					//执行播洒任务
					if(is_broadcasting == 1){ 
							is_broadcasting =broadcasting_Task(dT_ms);
							
							//播洒任务执行成功
							if(is_broadcasting == 0){ 
								targe_index += 1; //索引加一 将目标点设置为下一个点位 
								printf("targe_index:%d\r\n",targe_index );  
								printf("x %d y %d\r\n",plant_map.dot[targe_buf[targe_index]].y,
																					plant_map.dot[targe_buf[targe_index]].x ); 
														   
							}
							 
					}
				}
				
				return 1; //任务正在运行
	
}
 
