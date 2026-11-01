/*
 * menu.c
 *
 *  Created on: 1 de nov. de 2026
 *      Author: pablo
 */


#include "menu.h"


/* Macros */


/* Struct privates */
typedef struct{
	GPIO_TypeDef *GPIO_Port;
	uint16_t GPIO_Pin;
}menu_btn_gpio_t;


/* Globals */

disp7_sweep_t *Sweep = NULL;
menu_btn_gpio_t menu_btns_gpio[MENU_BTN_COUNT] = {
	{MENU_MCU_GPIO_Port, MENU_MCU_Pin},
	{SOBE_MCU_GPIO_Port, SOBE_MCU_Pin},
	{DESCE_MCU_GPIO_Port, DESCE_MCU_Pin},
	{DESLIGA_MCU_GPIO_Port, DESLIGA_MCU_Pin},
	{MAIS_MCU_GPIO_Port, MAIS_MCU_Pin},
	{MENOS_MCU_GPIO_Port, MENOS_MCU_Pin}
};

/* Private prototypes */
void Disp7_Write_Pin(disp7_pins_e DisplayPin, uint8_t ValueLevel);

void Disp7_Control_0(uint8_t TurnOn);
void Disp7_Control_1(uint8_t TurnOn);
void Disp7_Control_2(uint8_t TurnOn);
void Disp7_Control_3(uint8_t TurnOn);
void Disp7_Control_4(uint8_t TurnOn);


void menu_display_init(menu_t *menu);
void menu_btn_init(menu_t *menu);

menu_btn_state_e menu_btn_read(menu_t *menu, menu_btn_e idx);

void menu_self_test();

void menu_set_led_auto(uint8_t TurnOn);
void menu_set_led_limpeza(uint8_t TurnOn);
void menu_set_led_vent(uint8_t TurnOn);
void menu_set_led_timer(uint8_t TurnOn);
void menu_set_led_secagem(uint8_t TurnOn);
void menu_set_led_gelando(uint8_t TurnOn);
void menu_set_led_temp(uint8_t TurnOn);

/* Hardware Interrupts */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance == TIM7){
		if (Sweep != NULL){
			disp7Sweep_timer_interrupt(Sweep);
		}
	}
}

/* Timers */
void menu_timer_btn_process(TimerHandle_t pxTimer){
	menu_t *menu = (menu_t*)pvTimerGetTimerID(pxTimer);
	menu_btn_e btn;

	for (btn=0 ; btn<MENU_BTN_COUNT ; btn++){
		if (menu_btn_read(menu, (menu_btn_e)btn) == MENU_BTN_STATE_SHORT_PRESSED){
			menu_btn_callback(btn, MENU_BTN_STATE_SHORT_PRESSED);
		}
		else if (menu_btn_read(menu, (menu_btn_e)btn) == MENU_BTN_STATE_LONG_PRESSED){
			menu_btn_callback(btn, MENU_BTN_STATE_LONG_PRESSED);
		}
	}

}

/* Tasks */

void menu_task(void *pvParams){
	menu_t *menu = (menu_t*)pvParams;
	disp7_sweep_t *disp7_sweep = &menu->disp7_sweep;

	menu_btn_init(menu);
	menu_display_init(menu);

	while(1){
		// Update the menu display
		vTaskDelay(pdMS_TO_TICKS(10));
	}
}

/* Publics */

void menu_init(menu_t *menu){
	xTaskCreate(menu_task, "Menu", 256, (void*)menu, 2, NULL);
}


/* Privates */

void menu_set_led_auto(uint8_t TurnOn){
	HAL_GPIO_WritePin(AUTO_MCU_GPIO_Port, AUTO_MCU_Pin, TurnOn);
}
void menu_set_led_limpeza(uint8_t TurnOn){
	HAL_GPIO_WritePin(LIMPEZA_MCU_GPIO_Port, LIMPEZA_MCU_Pin, TurnOn);
}
void menu_set_led_vent(uint8_t TurnOn){
	HAL_GPIO_WritePin(VENTILADOR_MCU_GPIO_Port, VENTILADOR_MCU_Pin, TurnOn);
}
void menu_set_led_timer(uint8_t TurnOn){
	HAL_GPIO_WritePin(TIMER_GPIO_Port, TIMER_Pin, TurnOn);
}
void menu_set_led_secagem(uint8_t TurnOn){
	HAL_GPIO_WritePin(SECAGEM_GPIO_Port, SECAGEM_Pin, TurnOn);
}
void menu_set_led_gelando(uint8_t TurnOn){
	HAL_GPIO_WritePin(GELANDO_MCU_GPIO_Port, GELANDO_MCU_Pin, TurnOn);
}
void menu_set_led_temp(uint8_t TurnOn){
	HAL_GPIO_WritePin(SIMBOLO_TEMP_MCU_GPIO_Port, SIMBOLO_TEMP_MCU_Pin, TurnOn);
}

void menu_self_test(){
	uint32_t i, value;
	disp7_sweep_t *disp7_sweep = Sweep;

	menu_set_led_auto(1);
	menu_set_led_limpeza(1);
	menu_set_led_vent(1);
	menu_set_led_timer(1);
	menu_set_led_secagem(1);
	menu_set_led_gelando(1);
	menu_set_led_temp(1);

	value = 100000;
	for (i=0 ; i<10 ; i++){
		disp7Sweep_print_number(disp7_sweep, value);
		vTaskDelay(pdMS_TO_TICKS(250));
		value += 11111;
	}
	disp7Sweep_print_number(disp7_sweep, 100000);
	vTaskDelay(pdMS_TO_TICKS(250));

	menu_set_led_auto(0);
	menu_set_led_limpeza(0);
	menu_set_led_vent(0);
	menu_set_led_timer(0);
	menu_set_led_secagem(0);
	menu_set_led_gelando(0);
	menu_set_led_temp(0);
}

void menu_display_init(menu_t *menu){
	disp7_params_t disp7_params = {0};
	disp7_t *disp7 = &menu->disp7;
	disp7_alone_t *disp7_digits = menu->disp7_digits;
	disp7_sweep_t *disp7_sweep = &menu->disp7_sweep;

	disp7_params.eType = DISP7_COMMON_CATHODE;
	disp7_params.WriteToPinFxn = Disp7_Write_Pin;
	disp7_init(disp7, &disp7_params);

	disp7Sweep_init(disp7_sweep);

	disp7_digits[0].fxnControlDisplay = Disp7_Control_0;
	disp7_digits[0].psDisplay = disp7;
	disp7_digits[1].fxnControlDisplay = Disp7_Control_1;
	disp7_digits[1].psDisplay = disp7;
	disp7_digits[2].fxnControlDisplay = Disp7_Control_2;
	disp7_digits[2].psDisplay = disp7;
	disp7_digits[3].fxnControlDisplay = Disp7_Control_3;
	disp7_digits[3].psDisplay = disp7;
	disp7_digits[4].fxnControlDisplay = Disp7_Control_4;
	disp7_digits[4].psDisplay = disp7;

	disp7Sweep_add_display(disp7_sweep, &disp7_digits[0]);
	disp7Sweep_add_display(disp7_sweep, &disp7_digits[1]);
	disp7Sweep_add_display(disp7_sweep, &disp7_digits[2]);
	disp7Sweep_add_display(disp7_sweep, &disp7_digits[3]);
	disp7Sweep_add_display(disp7_sweep, &disp7_digits[4]);

	Sweep = disp7_sweep;

	HAL_TIM_Base_Start_IT(&htim7);

	menu_self_test();
}

void menu_btn_init(menu_t *menu){
	uint32_t i;

	for (i=0 ; i<MENU_BTN_COUNT ; i++){
		menu->btns.i32_btn_array[i] = MENU_LONG_PRESS_MS;
	}

	menu->x_timer_btn_menu = xTimerCreate("KeyRead",
					pdMS_TO_TICKS(MENU_BTN_TIMER_MS),
					pdTRUE,
					(void*)(menu),
					menu_timer_btn_process);
	xTimerStart(menu->x_timer_btn_menu, 0);
}

/* Estamos usando uma tecnica de leitura periodica para facilitar
 * debouncing e para permitir a leitura de long press e short press.
 * A cada leitura, o valor do botão é decrementado. Se o botão for solto
 * antes de atingir 0, é considerado short press.
 * Se atingir 0, é considerado long press.
 */
menu_btn_state_e menu_btn_read(menu_t *menu, menu_btn_e idx){
	GPIO_TypeDef *GPIO_Port = menu_btns_gpio[idx].GPIO_Port;
	uint16_t GPIO_Pin = menu_btns_gpio[idx].GPIO_Pin;
	menu_btn_state_e ret = MENU_BTN_STATE_NOT_PRESSED;

	if (idx >= MENU_BTN_COUNT){
		return ret;
	}

	if (HAL_GPIO_ReadPin(GPIO_Port, GPIO_Pin) == GPIO_PIN_RESET){
		if (menu->btns.i32_btn_array[idx] > 0){
			menu->btns.i32_btn_array[idx] -= MENU_BTN_TIMER_MS;
			if (menu->btns.i32_btn_array[idx] <= 0){
				ret = MENU_BTN_STATE_LONG_PRESSED;
			}
		}
	}
	else{
		if (menu->btns.i32_btn_array[idx] <= MENU_SHORT_PRESS_MS &&
				menu->btns.i32_btn_array[idx] > 0){
			ret = MENU_BTN_STATE_SHORT_PRESSED;
		}
		menu->btns.i32_btn_array[idx] = MENU_LONG_PRESS_MS;
	}

	return ret;
}

void Disp7_Write_Pin(disp7_pins_e DisplayPin, uint8_t ValueLevel){
	switch (DisplayPin){
	case DISP7_PIN_A:
		HAL_GPIO_WritePin(SEG_A_MCU_GPIO_Port, SEG_A_MCU_Pin, ValueLevel);
		break;
	case DISP7_PIN_B:
		HAL_GPIO_WritePin(SEG_B_MCU_GPIO_Port, SEG_B_MCU_Pin, ValueLevel);
		break;
	case DISP7_PIN_C:
		HAL_GPIO_WritePin(SEG_C_MCU_GPIO_Port, SEG_C_MCU_Pin, ValueLevel);
		break;
	case DISP7_PIN_D:
		HAL_GPIO_WritePin(SEG_D_MCU_GPIO_Port, SEG_D_MCU_Pin, ValueLevel);
		break;
	case DISP7_PIN_E:
		HAL_GPIO_WritePin(SEG_E_MCU_GPIO_Port, SEG_E_MCU_Pin, ValueLevel);
		break;
	case DISP7_PIN_F:
		HAL_GPIO_WritePin(SEG_F_MCU_GPIO_Port, SEG_F_MCU_Pin, ValueLevel);
		break;
	case DISP7_PIN_G:
		HAL_GPIO_WritePin(SEG_G_MCU_GPIO_Port, SEG_G_MCU_Pin, ValueLevel);
		break;
	case DISP7_PIN_dp:
		// not implemented
		break;
	default:
		break;
	}
}

void Disp7_Control_0(uint8_t TurnOn){
	if (TurnOn)
		HAL_GPIO_WritePin(DIG_01_PEQ_GPIO_Port, DIG_01_PEQ_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(DIG_01_PEQ_GPIO_Port, DIG_01_PEQ_Pin, GPIO_PIN_RESET);
}

void Disp7_Control_1(uint8_t TurnOn){
	if (TurnOn)
		HAL_GPIO_WritePin(DIG_02_PEQ_GPIO_Port, DIG_02_PEQ_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(DIG_02_PEQ_GPIO_Port, DIG_02_PEQ_Pin, GPIO_PIN_RESET);
}

void Disp7_Control_2(uint8_t TurnOn){
	if (TurnOn)
		HAL_GPIO_WritePin(DIG_01_GRA_GPIO_Port, DIG_01_GRA_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(DIG_01_GRA_GPIO_Port, DIG_01_GRA_Pin, GPIO_PIN_RESET);
}

void Disp7_Control_3(uint8_t TurnOn){
	if (TurnOn)
		HAL_GPIO_WritePin(DIG_02_GRA_GPIO_Port, DIG_02_GRA_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(DIG_02_GRA_GPIO_Port, DIG_02_GRA_Pin, GPIO_PIN_RESET);
}

void Disp7_Control_4(uint8_t TurnOn){
	if (TurnOn)
		HAL_GPIO_WritePin(DIG_03_GRA_GPIO_Port, DIG_03_GRA_Pin, GPIO_PIN_SET);
	else
		HAL_GPIO_WritePin(DIG_03_GRA_GPIO_Port, DIG_03_GRA_Pin, GPIO_PIN_RESET);
}
