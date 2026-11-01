/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, AUTO_MCU_Pin|SWING_Pin|TIMER_Pin|LIMPEZA_MCU_Pin
                          |EXAUSTAO_Pin|SECAGEM_Pin|VENTILADOR_MCU_Pin|RE_MCU_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GELANDO_MCU_Pin|SIMBOLO_TEMP_MCU_Pin|PONTO_MCU_Pin|SEG_A_MCU_Pin
                          |DIG_01_PEQ_Pin|DIG_02_PEQ_Pin|DIG_01_GRA_Pin|DIG_02_GRA_Pin
                          |DIG_03_GRA_Pin|SEG_G_MCU_Pin|SEG_F_MCU_Pin|SEG_E_MCU_Pin
                          |SEG_D_MCU_Pin|SEG_C_MCU_Pin|SEG_B_MCU_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : DESLIGA_MCU_Pin MAIS_MCU_Pin MENOS_MCU_Pin MENU_MCU_Pin
                           SOBE_MCU_Pin DESCE_MCU_Pin */
  GPIO_InitStruct.Pin = DESLIGA_MCU_Pin|MAIS_MCU_Pin|MENOS_MCU_Pin|MENU_MCU_Pin
                          |SOBE_MCU_Pin|DESCE_MCU_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : AUTO_MCU_Pin SWING_Pin TIMER_Pin LIMPEZA_MCU_Pin
                           EXAUSTAO_Pin SECAGEM_Pin VENTILADOR_MCU_Pin RE_MCU_Pin */
  GPIO_InitStruct.Pin = AUTO_MCU_Pin|SWING_Pin|TIMER_Pin|LIMPEZA_MCU_Pin
                          |EXAUSTAO_Pin|SECAGEM_Pin|VENTILADOR_MCU_Pin|RE_MCU_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : GELANDO_MCU_Pin SIMBOLO_TEMP_MCU_Pin PONTO_MCU_Pin SEG_A_MCU_Pin
                           DIG_01_PEQ_Pin DIG_02_PEQ_Pin DIG_01_GRA_Pin DIG_02_GRA_Pin
                           DIG_03_GRA_Pin SEG_G_MCU_Pin SEG_F_MCU_Pin SEG_E_MCU_Pin
                           SEG_D_MCU_Pin SEG_C_MCU_Pin SEG_B_MCU_Pin */
  GPIO_InitStruct.Pin = GELANDO_MCU_Pin|SIMBOLO_TEMP_MCU_Pin|PONTO_MCU_Pin|SEG_A_MCU_Pin
                          |DIG_01_PEQ_Pin|DIG_02_PEQ_Pin|DIG_01_GRA_Pin|DIG_02_GRA_Pin
                          |DIG_03_GRA_Pin|SEG_G_MCU_Pin|SEG_F_MCU_Pin|SEG_E_MCU_Pin
                          |SEG_D_MCU_Pin|SEG_C_MCU_Pin|SEG_B_MCU_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
