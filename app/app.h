/*
 * app.h
 *
 *  Created on: 10 de out. de 2026
 *      Author: pablo
 */

#ifndef APP_H_
#define APP_H_

/* Includes */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>
#include <event_groups.h>
#include <timers.h>

#include "comm.h"
#include "menu.h"

/* Enumerates */
typedef enum{
	APP_COMM_CMD_SET_PARAM,
	APP_COMM_CMD_GET_PARAM,
	APP_COMM_CMD_GET_INFO
}app_comm_cmd_e;

typedef enum{
	APP_COMM_PARAM_NONE,
	APP_COMM_PARAM_HUMID,
	APP_COMM_PARAM_TEMP,
	APP_COMM_PARAM_VENT
}app_comm_param_e;

/* Typedefs */
typedef struct{
	app_comm_cmd_e cmd;
	app_comm_param_e param;
	uint8_t data[16];
}app_comm_packet_t;

typedef struct{
	uint8_t data[sizeof(app_comm_packet_t)];
	uint16_t sender;
}app_comm_queue_item_t;

typedef struct{
	comm_t comm;
	menu_t menu;
	QueueHandle_t x_comm_queue;
}app_t;

/* Publics */

void app_run(app_t *app);

#endif /* APP_H_ */
