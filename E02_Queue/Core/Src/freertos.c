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
#include <string.h>
#include <stdlib.h>
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
/* USER CODE BEGIN PTD */
typedef struct {
  GPIO_TypeDef* Port;
  uint16_t Pin;
  GPIO_PinState State;
  _Bool Blink;
  char Id[8];
} SERIAL_API_IO_TypeDef;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define HLEDS_LEN 3
#define LED_CMD_LEN 4
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
uint8_t com_rx[32];
uint16_t com_rx_len = 1;
SERIAL_API_IO_TypeDef hleds[HLEDS_LEN] = {
  { .Id = "LD1", .Port = LD1_GPIO_Port, .Pin = LD1_Pin, .State = GPIO_PIN_RESET, .Blink = 0 },
  { .Id = "LD2", .Port = LD2_GPIO_Port, .Pin = LD2_Pin, .State = GPIO_PIN_RESET, .Blink = 0 },
  { .Id = "LD3", .Port = LD3_GPIO_Port, .Pin = LD3_Pin, .State = GPIO_PIN_RESET, .Blink = 0 },
};
/* USER CODE END Variables */
/* Definitions for LdCtrlTask */
osThreadId_t LdCtrlTaskHandle;
uint32_t LdCtrlTaskBuffer[ 128 ];
osStaticThreadDef_t LdCtrlTaskControlBlock;
const osThreadAttr_t LdCtrlTask_attributes = {
  .name = "LdCtrlTask",
  .cb_mem = &LdCtrlTaskControlBlock,
  .cb_size = sizeof(LdCtrlTaskControlBlock),
  .stack_mem = &LdCtrlTaskBuffer[0],
  .stack_size = sizeof(LdCtrlTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MsgParseTask */
osThreadId_t MsgParseTaskHandle;
uint32_t MsgParseTaskBuffer[ 128 ];
osStaticThreadDef_t MsgParseTaskControlBlock;
const osThreadAttr_t MsgParseTask_attributes = {
  .name = "MsgParseTask",
  .cb_mem = &MsgParseTaskControlBlock,
  .cb_size = sizeof(MsgParseTaskControlBlock),
  .stack_mem = &MsgParseTaskBuffer[0],
  .stack_size = sizeof(MsgParseTaskBuffer),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for msgQueue */
osMessageQueueId_t msgQueueHandle;
uint8_t msgQueueBuffer[ 16 * sizeof( uint8_t ) ];
osStaticMessageQDef_t msgQueueControlBlock;
const osMessageQueueAttr_t msgQueue_attributes = {
  .name = "msgQueue",
  .cb_mem = &msgQueueControlBlock,
  .cb_size = sizeof(msgQueueControlBlock),
  .mq_mem = &msgQueueBuffer,
  .mq_size = sizeof(msgQueueBuffer)
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
/**
 * @brief Serial API LED control message reading
 * @param[in]     msg      : Input message
 * @param[in/out] leds     : Serial API IO control structure array
 * @param[in]     leds_len : Serial API IO control structure array's length
 * @retval Parsing status: 0 if successful, <0 otherwise
 */
int SERIAL_API_LED_ReadMsg(const char* msg, SERIAL_API_IO_TypeDef* leds, int leds_len);
/* USER CODE END FunctionPrototypes */

void StartLdCtrlTaskTask(void *argument);
void StartMsgParseTask(void *argument);

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

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of msgQueue */
  msgQueueHandle = osMessageQueueNew (16, sizeof(uint8_t), &msgQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of LdCtrlTask */
  LdCtrlTaskHandle = osThreadNew(StartLdCtrlTaskTask, NULL, &LdCtrlTask_attributes);

  /* creation of MsgParseTask */
  MsgParseTaskHandle = osThreadNew(StartMsgParseTask, NULL, &MsgParseTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartLdCtrlTaskTask */
/**
  * @brief  Function implementing the LdCtrlTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartLdCtrlTaskTask */
void StartLdCtrlTaskTask(void *argument)
{
  /* USER CODE BEGIN StartLdCtrlTaskTask */
  /* Infinite loop */
  for(;;)
  {
    // Built-in LEDs handling tasks
    // reading LEDs state from 'hleds' array and writing to GPIOs
    for(uint8_t i = 0; i < HLEDS_LEN; i++)
    {
      if(hleds[i].Blink)
        hleds[i].State ^= GPIO_PIN_SET;
      HAL_GPIO_WritePin(hleds[i].Port, hleds[i].Pin, hleds[i].State);
    }
    osDelay(100);
  }
  /* USER CODE END StartLdCtrlTaskTask */
}

/* USER CODE BEGIN Header_StartMsgParseTask */
/**
* @brief Function implementing the MsgParseTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartMsgParseTask */
void StartMsgParseTask(void *argument)
{
  /* USER CODE BEGIN StartMsgParseTask */

  // Listen for message and call UART Rx callback when received
  HAL_UART_Receive_IT(&huart3, com_rx, com_rx_len);

  // Received message buffer
  uint32_t msgToParseLen = LED_CMD_LEN;
  char msgToParse[msgToParseLen+1];
  msgToParse[msgToParseLen] = '\0';

  /* Infinite loop */
  for(;;)
  {
    // If number of elements in queue is EQUAL to length
    // of the message to parse
    if(osMessageQueueGetCount(msgQueueHandle) == msgToParseLen)
    {
      // Read i-th element from queue to array
      for(uint8_t i = 0; i < msgToParseLen; i++)
        while(osMessageQueueGet(msgQueueHandle, &msgToParse[i], 0, 10) != osOK);

      // Send back queue content
      char com_tx[64];
      uint16_t com_tx_len = sprintf(com_tx, "Queue content: %s\r\n", msgToParse);
      HAL_UART_Transmit(&huart3, (uint8_t*)com_tx, com_tx_len, 100);

      // Read message with LED control command
      // and update LED handlers states
      SERIAL_API_LED_ReadMsg(msgToParse, hleds, HLEDS_LEN);
    }
    osDelay(100);
  }
  /* USER CODE END StartMsgParseTask */
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
  // If adding received message to queue is successful
  if(osMessageQueuePut(msgQueueHandle, (void*)com_rx, 0, 0) == osOK)
  {
#ifdef SERIAL_PORT_ECHO
    // Send back queue count
    char com_tx[64];
    uint16_t com_tx_len = sprintf(com_tx, "Queue count: %d\r\n", (int)osMessageQueueGetCount(msgQueueHandle));
    HAL_UART_Transmit(&huart3, (uint8_t*)com_tx, com_tx_len, 100);
#endif
    // Listen for message and call UART Rx callback when received
    HAL_UART_Receive_IT(&huart3, com_rx, com_rx_len);
  }
  else
  {
    // Turn on red LED blinking
    hleds[2].Blink = 1;
  }
}

/**
 * @brief Serial API LED control message reading
 * @param[in]     msg      : Input message
 * @param[in/out] leds     : Serial API LED control structure array
 * @param[in]     leds_len : Serial API LED control structure array's length
 * @retval Parsing status: 0 if successful, <0 otherwise
 */
int SERIAL_API_LED_ReadMsg(const char* msg, SERIAL_API_IO_TypeDef* leds, int leds_len)
{
  for(int i = 0; i < leds_len; i++)
  {
    /* Message structure: 3 character ID + state as 0/1 e.g. LD11 to turn LD1 on */
    if(strncmp(leds[i].Id, msg, 3) == 0)
      leds[i].State = strtol(&msg[3], NULL, 10);
  }
  return 0;
}

/* USER CODE END Application */

