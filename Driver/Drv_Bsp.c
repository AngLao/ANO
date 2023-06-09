#include "Drv_Bsp.h"
#include "pwm.h"
#include "Drv_RcIn.h"
#include "Drv_Spi.h"
#include "Drv_Led.h"
#include "Drv_Paramter.h"
#include "Drv_icm20602.h"
#include "drv_ak8975.h"
#include "drv_spl06.h"
#include "Drv_PwmOut.h"
#include "Drv_Adc.h"
#include "Drv_Uart.h"
#include "Ano_FcData.h"
#include "Ano_Sensor_Basic.h"
#include "Ano_RC.h"
#include "Ano_FlightCtrl.h"
#include "Drv_gps.h"
#include "ano_usb.h"
#include "Drv_UP_Flow.h"
#include "Drv_laser.h"

static uint64_t SysRunTimeMs = 0;

void SysTick_Init(void )
{
	ROM_SysTickPeriodSet(ROM_SysCtlClockGet()/1000);
	ROM_SysTickIntEnable();
	ROM_SysTickEnable();
}
void SysTick_Handler(void)
{
	SysRunTimeMs++;
}
uint32_t GetSysRunTimeMs(void)
{
	return SysRunTimeMs;
}
uint32_t GetSysRunTimeUs(void)
{
	return SysRunTimeMs*1000 + (SysTick->LOAD - SysTick->VAL) * 1000 / SysTick->LOAD;
}

void MyDelayMs(u32 time)
{
	ROM_SysCtlDelay(80000 * time /3);
}

void Drv_SenserCsPinInit(void)
{
	Drv_Icm20602CSPinInit();
	Drv_AK8975CSPinInit();
	Drv_SPL06CSPinInit();
	
	ROM_SysCtlPeripheralEnable(FLASH_CSPIN_SYSCTL);
	ROM_GPIOPinTypeGPIOOutput(FLASH_CS_PORT,FLASH_CS_PIN);
	ROM_GPIOPinWrite(FLASH_CS_PORT, FLASH_CS_PIN,FLASH_CS_PIN);
}

u8 of_init_type;
void Drv_BspInit(void)
{  
	/*设置系统主频为80M*/
	ROM_SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ |SYSCTL_OSC_MAIN);
	/*中断优先级组别设置*/
	NVIC_SetPriorityGrouping(0x03);
	/*开启浮点运算单元*/	
	ROM_FPULazyStackingEnable();
	ROM_FPUEnable();
	  
	//板载USB虚拟串口初始化
	AnoUsbCdcInit();	 
	//数据初始化
	Dvr_ParamterInit();
	//读取初始数据
	Para_Data_Init();
	//灯光初始化
	Dvr_LedInit();
	
	//遥控接收模式初始化
	Remote_Control_Init();

	//spi通信初始化
	Drv_Spi0Init();
	Drv_SenserCsPinInit();
	//初始化ICM
	sens_hd_check.acc_ok = sens_hd_check.gyro_ok = Drv_Icm20602Init();
	//初始化气压计
	sens_hd_check.baro_ok = Drv_Spl0601Init();
	//标记罗盘OK，否则罗盘不参与解算（注：此处没有做罗盘是否正常的检测程序）
	sens_hd_check.mag_ok = 0;       
	
	
	//ADC初始化
	Drv_AdcInit();
	
	//滴答时钟初始化
	SysTick_Init();	
	
	//串口初始化
	
	Drv_Uart1Init(3000000);	//接UWB
	
	Drv_Uart2Init(115200);	
	
	Drv_Uart3Init(115200);   //接数传
	 
	 
	Drv_Uart4Init(921600);	//接匿名光流
	
	
	Drv_Uart5Init(921600);	//接openMV
	
		
	//====fc
	//飞控传感器计算初始化
	Sensor_Basic_Init();	
	//飞控PID初始化
	All_PID_Init();
	//电机输出初始化
	Drv_PwmOutInit();
	
	//激光笔IO口初始化
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	ROM_GPIOPinTypeGPIOOutput(GPIOF_BASE, GPIO_PIN_0); 
	 
	ROM_GPIOPinWrite(GPIOF_BASE, GPIO_PIN_0,0); 
}




