/**
  ******************************************************************************
  * @file    Template/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    20-September-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "main.h"
#include "queue.h"

/** @addtogroup Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define STR_MAX_LENGTH	100
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

volatile uint8_t b_receive_done;

volatile int flag_esc;
volatile int flag_multi_input;
volatile int time_countdown;
volatile int time_countup;
volatile int flag_time_update;


queue_t queue_sender;
queue_t queue_receiver;

uint8_t msgSend;
uint8_t msgReceive;
uint8_t b_success;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

void USART3_IRQHandler(void)
{
  if(USART_GetITStatus(USART3, USART_IT_TXE) == SET)
  {
    if(0 == queue_is_empty(&queue_sender))
    {
      b_success = 0;
      msgSend = queue_pop(&queue_sender, &b_success);
      USART_SendData(USART3, (uint16_t)msgSend);
    }
    else
    {
      USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
    }
  }
  else if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
  {
    msgReceive = (uint8_t)USART_ReceiveData(USART3);
		if(msgReceive == 27)
			flag_esc = 1;
		else
			queue_push(&queue_receiver, msgReceive);
		if(flag_multi_input == 0)
		{
			USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
			b_receive_done = 1;
		}
		else if(msgReceive == 10)
		{
			USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
			b_receive_done = 1;
		}
		
	}
}
void TIM3_IRQHandler(void)
{
	static uint32_t time1 =0;
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		if(++time1>1000)
		{
			time1 = 0;
			time_countdown --;
			flag_time_update = 1;
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 

	}
}
void TIM4_IRQHandler(void)
{
	static uint32_t time2 =0;
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		time2 ++;
		if(time2>1000)
		{
			time2 = 0;
			time_countup ++;
			STM_EVAL_LEDOn(LED3);
		}
		if(time2 == 500)
			STM_EVAL_LEDOff(LED3);
		
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update); 

	}
}
/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
