/*
 * menu.h
 *
 *  Created on: 1 de nov. de 2026
 *      Author: pablo
 */

#ifndef MENU_H_
#define MENU_H_

#include <stdint.h>
#include <stdbool.h>

#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

#include "gpio.h"
#include "tim.h"

#include "disp7.h"
#include "disp7_sweep.h"

/* Macros */

#define MENU_LONG_PRESS_MS			1500
#define MENU_SHORT_PRESS_MS			(MENU_LONG_PRESS_MS - 75)
#define MENU_BTN_TIMER_MS			25

#define MENU_BTN_NOT_PRESSED		0
#define MENU_BTN_SHORT_PRESSED		1
#define MENU_BTN_LONG_PRESSED		2


/* Enumerates */
typedef enum{
	MENU_BTN_MENU,
	MENU_BTN_SOBE,
	MENU_BTN_DESCE,
	MENU_BTN_DESLIGA,
	MENU_BTN_MAIS,
	MENU_BTN_MENOS,

	MENU_BTN_COUNT
}menu_btn_e;

typedef enum{
	MENU_BTN_STATE_NOT_PRESSED,
	MENU_BTN_STATE_SHORT_PRESSED,
	MENU_BTN_STATE_LONG_PRESSED
}menu_btn_state_e;


/* Typedefs */

typedef struct{
	disp7_t disp7;
	disp7_alone_t disp7_digits[5];
	disp7_sweep_t disp7_sweep;

	TimerHandle_t x_timer_btn_menu;

	struct{
		int32_t i32_btn_array[6];
	}btns;
}menu_t;

/* publics */

void menu_init(menu_t *menu);

/* Callbacks */

void menu_btn_callback(menu_btn_e btn, menu_btn_state_e state);


#endif /* MENU_H_ */
