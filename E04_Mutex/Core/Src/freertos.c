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
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticSemaphore_t osStaticMutexDef_t;
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
/* Definitions for userBtnTask */
osThreadId_t userBtnTaskHandle;
uint32_t userBtnTaskBuffer[ 128 ];
osStaticThreadDef_t userBtnTaskControlBlock;
const osThreadAttr_t userBtnTask_attributes = {
  .name = "userBtnTask",
  .cb_mem = &userBtnTaskControlBlock,
  .cb_size = sizeof(userBtnTaskControlBlock),
  .stack_mem = &userBtnTaskBuffer[0],
  .stack_size = sizeof(userBtnTaskBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for serialPortMutex */
osMutexId_t serialPortMutexHandle;
osStaticMutexDef_t serialPortMutexControlBlock;
const osMutexAttr_t serialPortMutex_attributes = {
  .name = "serialPortMutex",
  .cb_mem = &serialPortMutexControlBlock,
  .cb_size = sizeof(serialPortMutexControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/**
  * @brief  Task-safe C-string transmission via serial port
  * @param[in] str : C-string (null-terminated) to transmit
  * @retval  None
  */
void UART_TransmitStr_s(const char* str);
/* USER CODE END FunctionPrototypes */

void StartLd2BlinkTask(void *argument);
void StartUserBtnTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of serialPortMutex */
  serialPortMutexHandle = osMutexNew(&serialPortMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

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

  /* creation of userBtnTask */
  userBtnTaskHandle = osThreadNew(StartUserBtnTask, NULL, &userBtnTask_attributes);

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
  GPIO_PinState Ld2State;
  char com_tx[64];

  /* Infinite loop */
  for(;;)
  {
    // Toggle blue LED
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    Ld2State = HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin);

    // Send message with blue LED state via serial port
    snprintf(com_tx, sizeof(com_tx)-1, "LD2 state: %s\n\r", Ld2State ? "SET" : "RESET");
    UART_TransmitStr_s(com_tx);

    // Blinking period: 500 millisecond
    osDelay(500);
  }
  /* USER CODE END StartLd2BlinkTask */
}

/* USER CODE BEGIN Header_StartUserBtnTask */
/**
* @brief Function implementing the userBtnTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUserBtnTask */
void StartUserBtnTask(void *argument)
{
  /* USER CODE BEGIN StartUserBtnTask */
  GPIO_PinState usrBtnState, usrBtnStatePrev = GPIO_PIN_RESET;
  GPIO_PinState Ld1State;
  char com_tx[64];

  /* Infinite loop */
  for(;;)
  {
    usrBtnState = HAL_GPIO_ReadPin(USER_Btn_GPIO_Port, USER_Btn_Pin);
    // If button is pushed
    if(usrBtnState  == GPIO_PIN_SET && usrBtnStatePrev == GPIO_PIN_RESET)
    {
      // Toggle green LED
      HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
      Ld1State = HAL_GPIO_ReadPin(LD1_GPIO_Port, LD1_Pin);

      // Send message with green LED state via serial port
      snprintf(com_tx, sizeof(com_tx)-1, "LD1 state: %s\n\r", Ld1State ? "SET" : "RESET");
      UART_TransmitStr_s(com_tx);
    }
    usrBtnStatePrev = usrBtnState;

    // Button push polling period: 10 millisecond
    osDelay(10);
  }
  /* USER CODE END StartUserBtnTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/**
  * @brief  Task-safe C-string transmission via serial port
  * @param[in] str : C-string (null-terminated) to transmit
  * @retval  None
  */
void UART_TransmitStr_s(const char* str)
{
  // If counting semaphore is successfully acquired
  // (resource is available)
  if(osMutexAcquire(serialPortMutexHandle, 1000) == osOK)
  {
    // Transmit string via serial port
    HAL_UART_Transmit(&huart3, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);

    // Release acquired resource
    osMutexRelease(serialPortMutexHandle);
  }
  else
  {
    // Turn on red LED
    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
  }
}

/* USER CODE END Application */

