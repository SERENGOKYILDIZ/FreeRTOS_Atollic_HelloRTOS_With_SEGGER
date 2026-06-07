#include "stm32f4xx.h"
#include "stm32f4_discovery.h"

#include "stdio.h" // for printf
#include "stdbool.h" // for bool

#include "FreeRTOS.h"
#include "task.h"

#include "SEGGER_SYSVIEW.h" // for SEGGER

// For printf
/*
int _write(int32_t file, uint8_t *ptr, int32_t len)
{
	for(int i=0;i<len;i++)
	{
		// That does to writing a char at terminal
		ITM_SendChar((*ptr++));
	}
	return len;
}
*/

TaskHandle_t myTask1Handle = NULL;
TaskHandle_t myTask2Handle = NULL;

// My Functions
void Task1_Handler(void *params);
void Task2_Handler(void *params);

void Setup(void);
void GPIO_Config(void);
void USART_Config(void);
void printMessage(char* msg);

bool UART_ACCESS_KEY = 1;
char myMessage[300];

int main(void)
{
	RCC_DeInit(); // SYSCLK = 16MHz; HSI = ON, HSE = OFF, So PLL = OFF
	// Note: We should change 'configCPU_CLOCK_HZ', It'll be 16MHz in 'FreeRTOSConfig.h'

	SystemCoreClockUpdate(); //-> It can do for changing the variable of 'SystemCoreClock' (So it'll be 16Mhz)

	// --- Launching SEGGER SystemView  ---//
	// 1. Enable DWT (Time Stamp Counter) (so that it can run without the debugger)
	DWT->CTRL |= (1 << 0); // Control Register is set

	// 2. Launch SystemView
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();
	// ------------------------------------------

	Setup();

	sprintf(myMessage, "Hello World with Me\n");
	printMessage(myMessage);

	// printf("Starting RTOS project.\n");
	// You should to open "Serial Wire Viewer" in Debugger
	// So you should to select port 0 in "Configure trace" in SWV Console

	////// Creating a new task
	xTaskCreate(Task1_Handler, "Görev 1", configMINIMAL_STACK_SIZE, NULL, 4, &myTask1Handle);

	xTaskCreate(Task2_Handler, "Görev 2", configMINIMAL_STACK_SIZE, NULL, 4, &myTask2Handle);

	/*
	 xTaskCreate(
				 	TaskFunction_t pxTaskCode,
					const char * const pcName,
					const configSTACK_DEPTH_TYPE uxStackDepth,
					void * const pvParameters,
					UBaseType_t uxPriority,
					TaskHandle_t * const pxCreatedTask
				)
	 */

	vTaskStartScheduler();

	while (1)
	{

	}
}

/// That's the task1's of function
void Task1_Handler(void *params)
{
	for(;;) // While or For, Either is ok
	{
		if(UART_ACCESS_KEY==1)
		{
			UART_ACCESS_KEY=0;
			printMessage("Hello World - TASK 1\n");
			UART_ACCESS_KEY=1;
			taskYIELD(); // Move the other tasks
		}
		//vTaskDelay(1);
	}
}

void Task2_Handler(void *params)
{
	while(1) // While or For, Either is ok
	{
		if(UART_ACCESS_KEY==1)
		{
			UART_ACCESS_KEY=0;
			printMessage("Hello World - TASK 2\n");
			UART_ACCESS_KEY=1;
			taskYIELD(); // Move the other tasks
		}
		//vTaskDelay(1);
	}
}


void Setup(void)
{
	GPIO_Config();
	USART_Config();
}

void GPIO_Config(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); // USART2_TX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); // USART2_RX

	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void USART_Config(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	USART_InitTypeDef USART_InitStruct = {0};

	memset(&USART_InitStruct, 0, sizeof(USART_InitStruct));

	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART2, &USART_InitStruct);

	USART_Cmd(USART2, ENABLE);
}

void printMessage(char* msg)
{
	for(int i=0;i<strlen(msg);i++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET); // Wait
		USART_SendData(USART2, msg[i]);
	}
}



void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
  return;
}
uint16_t EVAL_AUDIO_GetSampleCallBack(void){
  return -1;
}
