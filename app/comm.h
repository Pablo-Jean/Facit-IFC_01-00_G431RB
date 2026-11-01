/*
 * comm.h
 *
 *  Created on: 10 de out. de 2026
 *      Author: pablo
 */

#ifndef COMM_H_
#define COMM_H_

/* Includes */

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>
#include <timers.h>
#include <event_groups.h>

#include <dorime-sp.h>

/* enumerates */

typedef enum {
	COMM_EVT_DATA_RECEIVED,
	COMM_EVT_KEEP_ALIVE,
	COMM_EVT_SEND_FAILED,
	COMM_EVT_TIMEOUT,
}comm_callback_evt_e;

typedef enum{
	COMM_EVT_GROUP_KEEP_ALIVE 	= 0x0001,
	COMM_EVT_GROUP_RXTX			= 0x0002
}comm_event_e;

/* Typedefs */

typedef struct{
	uint16_t u16_destination;
	uint16_t u16_alive_refresh;
	void *pv_next;
}comm_keep_alive_list_t;

typedef struct {
	uint32_t u32_timeout_ms;
	uint32_t u32_retries;
	uint32_t u32_ka_cycle_ms;
	uint32_t u32_ka_timeout_ms;

	struct{
		TaskHandle_t x_task_handle;
		SemaphoreHandle_t x_dorime_mutex;
		EventGroupHandle_t x_comm_event;
		TimerHandle_t x_dorime_tick_timer;
		TimerHandle_t x_keep_alive_timer;
		TimerHandle_t x_keep_alive_decrement_timer;
		dorime_t x_dorime;
		uint32_t u32_tick_count;

		bool b_is_sending;
		uint32_t u32_send_retries;
		uint8_t *pu8_data_to_send;
		uint16_t u16_data_destination;
		uint32_t u32_data_len;

		comm_keep_alive_list_t *list_keep_alive;
		comm_keep_alive_list_t *list_keep_alive_actual;
	}_intern;
}comm_t;

/* Publics */

void comm_init(comm_t *comm);

void comm_update_params(comm_t *comm, uint32_t u32_timeout_ms, uint32_t u32_retries);

int8_t comm_data_send(comm_t *comm, uint16_t destination, uint8_t *data, uint32_t len);

int8_t comm_register_keep_alive(comm_t *comm, uint16_t destination);

int8_t comm_unregister_keep_alive(comm_t *comm, uint16_t destination);

int8_t comm_get_keep_alive(comm_t *comm, uint16_t destination);

/* callback */

void comm_event_callback(comm_callback_evt_e event, void *data);

#endif /* COMM_H_ */
