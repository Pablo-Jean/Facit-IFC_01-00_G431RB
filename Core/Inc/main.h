/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DESLIGA_MCU_Pin GPIO_PIN_0
#define DESLIGA_MCU_GPIO_Port GPIOC
#define MAIS_MCU_Pin GPIO_PIN_1
#define MAIS_MCU_GPIO_Port GPIOC
#define MENOS_MCU_Pin GPIO_PIN_2
#define MENOS_MCU_GPIO_Port GPIOC
#define MENU_MCU_Pin GPIO_PIN_3
#define MENU_MCU_GPIO_Port GPIOC
#define AUTO_MCU_Pin GPIO_PIN_5
#define AUTO_MCU_GPIO_Port GPIOA
#define SWING_Pin GPIO_PIN_6
#define SWING_GPIO_Port GPIOA
#define TIMER_Pin GPIO_PIN_7
#define TIMER_GPIO_Port GPIOA
#define SOBE_MCU_Pin GPIO_PIN_4
#define SOBE_MCU_GPIO_Port GPIOC
#define DESCE_MCU_Pin GPIO_PIN_5
#define DESCE_MCU_GPIO_Port GPIOC
#define GELANDO_MCU_Pin GPIO_PIN_0
#define GELANDO_MCU_GPIO_Port GPIOB
#define SIMBOLO_TEMP_MCU_Pin GPIO_PIN_1
#define SIMBOLO_TEMP_MCU_GPIO_Port GPIOB
#define PONTO_MCU_Pin GPIO_PIN_2
#define PONTO_MCU_GPIO_Port GPIOB
#define SEG_A_MCU_Pin GPIO_PIN_10
#define SEG_A_MCU_GPIO_Port GPIOB
#define DIG_01_PEQ_Pin GPIO_PIN_11
#define DIG_01_PEQ_GPIO_Port GPIOB
#define DIG_02_PEQ_Pin GPIO_PIN_12
#define DIG_02_PEQ_GPIO_Port GPIOB
#define DIG_01_GRA_Pin GPIO_PIN_13
#define DIG_01_GRA_GPIO_Port GPIOB
#define DIG_02_GRA_Pin GPIO_PIN_14
#define DIG_02_GRA_GPIO_Port GPIOB
#define DIG_03_GRA_Pin GPIO_PIN_15
#define DIG_03_GRA_GPIO_Port GPIOB
#define LIMPEZA_MCU_Pin GPIO_PIN_8
#define LIMPEZA_MCU_GPIO_Port GPIOA
#define EXAUSTAO_Pin GPIO_PIN_9
#define EXAUSTAO_GPIO_Port GPIOA
#define SECAGEM_Pin GPIO_PIN_10
#define SECAGEM_GPIO_Port GPIOA
#define VENTILADOR_MCU_Pin GPIO_PIN_11
#define VENTILADOR_MCU_GPIO_Port GPIOA
#define RE_MCU_Pin GPIO_PIN_15
#define RE_MCU_GPIO_Port GPIOA
#define SEG_G_MCU_Pin GPIO_PIN_4
#define SEG_G_MCU_GPIO_Port GPIOB
#define SEG_F_MCU_Pin GPIO_PIN_5
#define SEG_F_MCU_GPIO_Port GPIOB
#define SEG_E_MCU_Pin GPIO_PIN_6
#define SEG_E_MCU_GPIO_Port GPIOB
#define SEG_D_MCU_Pin GPIO_PIN_7
#define SEG_D_MCU_GPIO_Port GPIOB
#define SEG_C_MCU_Pin GPIO_PIN_8
#define SEG_C_MCU_GPIO_Port GPIOB
#define SEG_B_MCU_Pin GPIO_PIN_9
#define SEG_B_MCU_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
