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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
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
uint8_t com_rx[32];
uint16_t com_rx_len = 1;
/* USER CODE END Variables */
/* Definitions for Ld2ToggleTask */
osThreadId_t Ld2ToggleTaskHandle;
uint32_t Ld2ToggleTaskBuffer[ 128 ];
osStaticThreadDef_t Ld2ToggleTaskControlBlock;
const osThreadAttr_t Ld2ToggleTask_attributes = {
  .name = "Ld2ToggleTask",
  .cb_mem = &Ld2ToggleTaskControlBlock,
  .cb_size = sizeof(Ld2ToggleTaskControlBlock),
  .stack_mem = &Ld2ToggleTaskBuffer[0],
  .stack_size = sizeof(Ld2ToggleTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for semaphoreLd2 */
osSemaphoreId_t semaphoreLd2Handle;
osStaticSemaphoreDef_t semaphoreLd2ControlBlock;
const osSemaphoreAttr_t semaphoreLd2_attributes = {
  .name = "semaphoreLd2",
  .cb_mem = &semaphoreLd2ControlBlock,
  .cb_size = sizeof(semaphoreLd2ControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartLd2ToggleTask(void *argument);

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

  /* Create the semaphores(s) */
  /* creation of semaphoreLd2 */
  semaphoreLd2Handle = osSemaphoreNew(1, 0, &semaphoreLd2_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Ld2ToggleTask */
  Ld2ToggleTaskHandle = osThreadNew(StartLd2ToggleTask, NULL, &Ld2ToggleTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartLd2ToggleTask */
/**
  * @brief  Function implementing the Ld2ToggleTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartLd2ToggleTask */
void StartLd2ToggleTask(void *argument)
{
  /* USER CODE BEGIN StartLd2ToggleTask */

  // Listen for message and call UART Rx callback when received
  HAL_UART_Receive_IT(&huart3, com_rx, com_rx_len);

  uint32_t semaphoreLd2Timeout = 2000; // milliseconds

  /* Infinite loop */
  for(;;)
  {
    // If binary semaphore is successfully acquired
    //      (that is if is released by UART Rx IT)
    if(osSemaphoreAcquire(semaphoreLd2Handle, semaphoreLd2Timeout) == osOK)
    {
      // Toggle blue LED
      HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    }
    // Toggle green LED after each binary semaphore
    // acquisition attempt (success or timeout)
    HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
  }
  /* USER CODE END StartLd2ToggleTask */
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
  // Binary semaphore is released
  osSemaphoreRelease(semaphoreLd2Handle);

  // Listen for message and call UART Rx callback when received
  HAL_UART_Receive_IT(&huart3, com_rx, com_rx_len);
}
/* USER CODE END Application */

