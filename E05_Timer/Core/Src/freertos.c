/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticTimer_t osStaticTimerDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint32_t timer_period = 100; /* milliseconds */;
uint32_t timer_period_new;
uint8_t com_rx[32];
uint16_t com_rx_len = 4;
/* USER CODE END Variables */
/* Definitions for timerTask */
osThreadId_t timerTaskHandle;
uint32_t timerTaskBuffer[ 128 ];
osStaticThreadDef_t timerTaskControlBlock;
const osThreadAttr_t timerTask_attributes = {
  .name = "timerTask",
  .cb_mem = &timerTaskControlBlock,
  .cb_size = sizeof(timerTaskControlBlock),
  .stack_mem = &timerTaskBuffer[0],
  .stack_size = sizeof(timerTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Ld2Timer */
osTimerId_t Ld2TimerHandle;
osStaticTimerDef_t Ld2TimerControlBlock;
const osTimerAttr_t Ld2Timer_attributes = {
  .name = "Ld2Timer",
  .cb_mem = &Ld2TimerControlBlock,
  .cb_size = sizeof(Ld2TimerControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTimerTask(void *argument);
void Ld2TimerCallback(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* Create the timer(s) */
  /* creation of Ld2Timer */
  Ld2TimerHandle = osTimerNew(Ld2TimerCallback, osTimerPeriodic, NULL, &Ld2Timer_attributes);

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of timerTask */
  timerTaskHandle = osThreadNew(StartTimerTask, NULL, &timerTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTimerTask */
/**
  * @brief  Function implementing the timerTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTimerTask */
void StartTimerTask(void *argument)
{
  /* USER CODE BEGIN StartTimerTask */

  timer_period_new = timer_period;

  // Listen for message and call UART Rx callback when received
  HAL_UART_Receive_IT(&huart3, com_rx, com_rx_len);

  /* Infinite loop */
  for(;;)
  {
    // If timer is running
    if(osTimerIsRunning(Ld2TimerHandle))
    {
      // Turn on green LED
      HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);

      // If timer period changed - stop timer
      if(timer_period_new != timer_period)
      {
        // If timer isn't successfully stopped
        // turn on red LED
        if(osTimerStop(Ld2TimerHandle) != osOK)
          HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);

        // Set new timer period
        timer_period = timer_period_new;
      }
    }
    // Otherwise turn off green LED and start timer
    else
    {
      HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);

      // If timer isn't successfully started
      // turn on red LED
      if(osTimerStart(Ld2TimerHandle, timer_period) != osOK)
        HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
    }

    // Task idle time
    osDelay(1);
  }
  /* USER CODE END StartTimerTask */
}

/* Ld2TimerCallback function */
void Ld2TimerCallback(void *argument)
{
  /* USER CODE BEGIN Ld2TimerCallback */
  // Toggle blue LED every timer_period milliseconds
  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
  /* USER CODE END Ld2TimerCallback */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/**
  * @brief  Rx Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  // Parse message
  timer_period_new = strtol((char*)com_rx, NULL, 10);

  // Saturate timer period <100, 1000> millisecond
  if(timer_period_new > 1000)
    timer_period_new = 1000;
  else if(timer_period_new < 100)
    timer_period_new = 100;

  // Listen for message and call UART Rx callback when received
  HAL_UART_Receive_IT(&huart3, com_rx, com_rx_len);

}
/* USER CODE END Application */

