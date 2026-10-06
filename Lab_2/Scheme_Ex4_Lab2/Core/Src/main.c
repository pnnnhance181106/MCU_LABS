/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body - Lab 2 Exercise 3
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
const int MAX_LED = 4;
int led_buffer[4] = {1, 2, 3, 0};

int led_counter = 10;  // 10 * 10ms = 100ms
int dot_counter = 100; // 100 * 10ms = 1000ms = 1s
int led_index = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void display7SEG(int num);
void update7SEG(int index);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static const uint8_t seg_code[10] = {
    0b1000000, // 0: a,b,c,d,e,f
    0b1111001, // 1: b,c
    0b0100100, // 2: a,b,d,e,g
    0b0110000, // 3: a,b,c,d,g
    0b0011001, // 4: b,c,f,g
    0b0010010, // 5: a,c,d,f,g
    0b0000010, // 6: a,c,d,e,f,g
    0b1111000, // 7: a,b,c
    0b0000000, // 8: a,b,c,d,e,f,g
    0b0010000  // 9: a,b,c,d,f,g
};

void display7SEG(int num)
{
    if (num < 0 || num > 9) return;
    uint8_t code = seg_code[num];
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, (code & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, (code & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, (code & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, (code & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, (code & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, (code & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, (code & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void update7SEG(int index)
{
    if (index < 0 || index > 3) return;
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_SET); // Tắt hết 4 LED chống bóng mờ

    switch(index)
    {
    case 0:
        display7SEG(led_buffer[0]);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
        break;
    case 1:
        display7SEG(led_buffer[1]);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
        break;
    case 2:
        display7SEG(led_buffer[2]);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
        break;
    case 3:
        display7SEG(led_buffer[3]);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
        break;
    default:
        break;
    }
}
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_TIM2_Init();

  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);
  /* USER CODE END 2 */

  while (1)
  {
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        // 1. Chớp tắt đèn DOT mỗi 1 giây (100 * 10ms = 1000ms)
        dot_counter--;
        if (dot_counter <= 0)
        {
            dot_counter = 100;
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);
        }

        // 2. Quét chuyển LED mỗi 500ms (50 * 10ms = 500ms)
        led_counter--;
        if (led_counter <= 0)
        {
            led_counter = 10;

            update7SEG(led_index);

            led_index++;
            if (led_index >= MAX_LED)
            {
                led_index = 0;
            }

            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        }
    }
}
/* USER CODE END 4 */
