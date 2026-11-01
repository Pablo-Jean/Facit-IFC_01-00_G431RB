/*
 * app.c
 *
 *  Created on: 10 de out. de 2026
 *      Author: pablo
 */

/* Includes */
#include "app.h"

#include "gpio.h"

/* Macros */


/* Enums */


/* Typedefs */


/* Privates */
app_t *App = NULL;

/* Tasks */

static void app_task(void *pvParams){
	comm_t *comm = &App->comm;
	app_comm_queue_item_t comm_item;

	// Inicializar outras partes da aplicação
	comm->u32_retries = 5;
	comm->u32_timeout_ms = 100;
	comm->u32_ka_cycle_ms = 500;
	comm->u32_ka_timeout_ms = 3000;
	comm_init(comm);

	App->x_comm_queue = xQueueCreate(10, sizeof(app_comm_queue_item_t));

	comm_register_keep_alive(comm, 0x010);
	comm_register_keep_alive(comm, 0x020);

	while(1){
		if (xQueueReceive(App->x_comm_queue, &comm_item, pdMS_TO_TICKS(100)) == pdPASS){
			// Process received data

		}

	}
}

/* Publics */

void app_run(app_t *app){
	App = app;
	xTaskCreate(app_task, "Application", 256, NULL, 1, NULL);
	vTaskStartScheduler();
}

/* Callbacks */

void comm_event_callback(comm_callback_evt_e event, void *data){
	switch (event){
		case COMM_EVT_DATA_RECEIVED:
			// Process received data
			// inserir em uma fila para ser processado na aplicacao
			break;

		case COMM_EVT_KEEP_ALIVE:
			HAL_GPIO_TogglePin(SWING_GPIO_Port, SWING_Pin);
			break;

		case COMM_EVT_SEND_FAILED:
			// Handle send failure
			// indicar na aplicacao que houve falha no envio
			break;

		case COMM_EVT_TIMEOUT:
			// Handle timeout
			// indicar na aplicacao que houve timeout
			break;

		default:
			break;
	}
}
