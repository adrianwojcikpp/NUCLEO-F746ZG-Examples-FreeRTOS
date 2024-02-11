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
#include <stdio.h>
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
/* Definitions for Ld2BlinkTask */
osThreadId_t Ld2BlinkTaskHandle;
uint32_t Ld2BlinkTaskBuffer[ 128 ];
osStaticThreadDef_t Ld2BlinkTaskControlBlock;
const osThreadAttr_t Ld2BlinkTask_attributes = {
  .name = "Ld2BlinkTask",
  .cb_mem = &Ld2BlinkTaskControlBlock,
  .cb_size = sizeof(Ld2BlinkTaskControlBlock),
  .stack_mem = &Ld2BlinkTaskBuffer[0],
  .stack_size = sizeof(Ld2BlinkTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for semaphoreCountLd2 */
osSemaphoreId_t semaphoreCountLd2Handle;
osStaticSemaphoreDef_t semaphoreCountLd2ControlBlock;
const osSemaphoreAttr_t semaphoreCountLd2_attributes = {
  .name = "semaphoreCountLd2",
  .cb_mem = &semaphoreCountLd2ControlBlock,
  .cb_size = sizeof(semaphoreCountLd2ControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartLd2BlinkTask(void *argument);

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
  /* creation of semaphoreCountLd2 */
  semaphoreCountLd2Handle = osSemaphoreNew(16, 16, &semaphoreCountLd2_attributes);

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
  /* creation of Ld2BlinkTask */
  Ld2BlinkTaskHandle = osThreadNew(StartLd2BlinkTask, NULL, &Ld2BlinkTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartLd2BlinkTask */
/**
  * @brief  Function implementing the Ld2BlinkTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartLd2BlinkTask */
void StartLd2BlinkTask(void *argument)
{
  /* USER CODE BEGIN StartLd2BlinkTask */

  // Listen for message and call UART Rx callback when received
  HAL_UART_Receive_IT(&huart3, com_rx, com_rx_len);

  /* Infinite loop */
  for(;;)
  {
    // If semaphore counter is greater then 0
    if(osSemaphoreGetCount(semaphoreCountLd2Handle) > 0)
    {
      // Wait some time before reporting to user semaphore count
      osDelay(100);

      // Send back semaphore count
      char com_tx[64];
      uint16_t com_tx_len = sprintf(com_tx, "Semaphore count: %d\r\n", (int)osSemaphoreGetCount(semaphoreCountLd2Handle));
      HAL_UART_Transmit(&huart3, (uint8_t*)com_tx, com_tx_len, 100);

      do
      {
        // If counting semaphore is successfully acquired
        // (counter is decremented)
        if(osSemaphoreAcquire(semaphoreCountLd2Handle, osWaitForever) == osOK)
        {
          // Toggle blue LED
          HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

          // Blinking period: 100 millisecond
          osDelay(100);
        }
      } while(osSemaphoreGetCount(semaphoreCountLd2Handle) > 0);
    }
  }
  /* USER CODE END StartLd2BlinkTask */
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

  // If counting semaphore is successfully released
  // (counter is incremented)
  if(osSemaphoreRelease(semaphoreCountLd2Handle) == osOK)
  {
    // Listen for message and call UART Rx callback when received
    HAL_UART_Receive_IT(&huart3, com_rx, com_rx_len);
  }
  else
  {
    // Turn on red LED
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
  }

}
/* USER CODE END Application */

