/*
 * comm.c
 *
 *  Created on: 10 de out. de 2026
 *      Author: pablo
 */


/* Includes */

#include "comm.h"

#include "gpio.h"
#include "usart.h"

/* Macros */

#define COMM_KEEP_ALIVE_SEND		0x44
#define COMM_KEEP_ALIVE_RESPONSE	0x55

/* Typedefs */


/* Internal Global */

comm_t *Comm = NULL;

/* Private prototypes */
void comm_update_keep_alive_list(comm_t *comm, uint16_t destination);
void comm_keep_alive_list_process(comm_t *comm);
void comm_keep_alive_list_decrement(comm_t *comm, uint16_t ms);


/* Interrupts */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	BaseType_t xTaskWoken = pdFALSE;

	if (huart->Instance == USART2){
		dorime_rx_event(&Comm->_intern.x_dorime);
		xEventGroupSetBitsFromISR(Comm->_intern.x_comm_event, COMM_EVT_GROUP_RXTX, &xTaskWoken);
	}

	portYIELD_FROM_ISR( xTaskWoken );
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart){
	BaseType_t xTaskWoken = pdFALSE;

	if (huart->Instance == USART2){
		dorime_tx_event(&Comm->_intern.x_dorime);
		xEventGroupSetBitsFromISR(Comm->_intern.x_comm_event, COMM_EVT_GROUP_RXTX, &xTaskWoken);
	}

	portYIELD_FROM_ISR( xTaskWoken );
}

/* Dorime Functions */

static uint8_t  dorime_tx_fxn(uint8_t *data, uint16_t len){
	HAL_UART_Transmit_IT(&huart2, data, len);
	return 0;
}

static uint8_t  dorime_rx_fxn(uint8_t *data, uint16_t len){
	HAL_UART_Receive_IT(&huart2, data, len);

	return 0;
}

static uint8_t  dorime_dir_fxn(dorime_interface_dir_e dir){
	if (dir == DORIME_INTERFACE_DIRECTION_TX){
		HAL_GPIO_WritePin(RE_MCU_GPIO_Port, RE_MCU_Pin, GPIO_PIN_SET);
	}
	else{
		HAL_GPIO_WritePin(RE_MCU_GPIO_Port, RE_MCU_Pin, GPIO_PIN_RESET);
	}

	return 0;
}

static uint32_t dorime_getcnt_fxn(void){
	return Comm->_intern.u32_tick_count;
}

static void     dorime_abort_fxn(void){
	HAL_UART_Abort(&huart2);
}

static void     dorime_lock_fxn(void){
	xSemaphoreTake(Comm->_intern.x_dorime_mutex, portMAX_DELAY);
}

static void     dorime_unlock_fxn(void){
	xSemaphoreGive(Comm->_intern.x_dorime_mutex);
}

static void*    dorime_alloc_fxn(size_t size){
	return pvPortMalloc(size);
}

static void     dorime_free_fxn(void *p){
	vPortFree(p);
}

/* Timers */

static void timer_dorime(TimerHandle_t pxTimer){
	Comm->_intern.u32_tick_count++;
}

static void timer_keep_alive(TimerHandle_t pxTimer){
	xEventGroupSetBits(Comm->_intern.x_comm_event, COMM_EVT_GROUP_KEEP_ALIVE);
}

static void timer_keep_alive_decrement(TimerHandle_t pxTimer){
	comm_keep_alive_list_decrement(Comm, 500);
}

/* Dorime Callback */
void dorime_cb_event(dorime_t *control, dorime_event_e event, void* params){
	dorime_evparams_data_t *data;
	static uint8_t u8_keep_alive_byte = COMM_KEEP_ALIVE_RESPONSE;

	switch (event){
	case 	DORIME_EVT_ADDR_MATCH:		// params: dorime_evparams_addr_match_t*
		// Nothing to do
		break;

	case	DORIME_EVT_DATA_REC:		// params: dorime_evparams_data_t*
		// Take the data and throw to the application
		data = (dorime_evparams_data_t*)params;
		// verifica se é evento de keep alive
		if (data->len > 1){
			comm_event_callback(COMM_EVT_DATA_RECEIVED, (void*)data);
		}
		else{
			if (data->data[0] == COMM_KEEP_ALIVE_SEND){
				// envia resposta de keep alive
				dorime_send_data(&Comm->_intern.x_dorime,
								 data->addr,
								 &u8_keep_alive_byte,
								 1);
				comm_event_callback(COMM_EVT_KEEP_ALIVE, NULL);
			}
			else if (data->data[0] == COMM_KEEP_ALIVE_RESPONSE){
				comm_event_callback(COMM_EVT_KEEP_ALIVE, NULL);
			}
		}
		break;

	case	DORIME_EVT_RESP_REC:		// reserved
		// Nothing to do
		break;

	case	DORIME_EVT_REC_FAILED:		// a reception was discarded (bad CRC, tail...)
		// do a retry?
		break;

	case	DORIME_EVT_ACK_OK:			// dorime_send_data() finished successfully
		// succefully sended data
		Comm->_intern.b_is_sending = 0;
		vPortFree(Comm->_intern.pu8_data_to_send);
		Comm->_intern.pu8_data_to_send = NULL;
		// Indicate the keep alive bit
		comm_update_keep_alive_list(Comm, (uint16_t)(params));

		break;

	case	DORIME_EVT_SEND_FAILED:		// dorime_send_data() failed (NOT_ACK, tx error)
		// Error on sending, do a retry
		/* break no needed */
	case	DORIME_EVT_TIMEOUT:			// a transaction timed out (see u32Timeout)
		// Timeout, do a retry
		if (Comm->_intern.b_is_sending){
			Comm->_intern.u32_send_retries--;
			if (Comm->_intern.b_is_sending > 0){
				if (dorime_send_data(&Comm->_intern.x_dorime,
									 Comm->_intern.u16_data_destination,
									 Comm->_intern.pu8_data_to_send,
									 Comm->_intern.u32_data_len) != DORIME_OK){
					comm_event_callback(COMM_EVT_SEND_FAILED, NULL);
				}
			}
			else{
				Comm->_intern.b_is_sending = false;
				vPortFree(Comm->_intern.pu8_data_to_send);
				Comm->_intern.pu8_data_to_send = NULL;
				comm_event_callback(COMM_EVT_SEND_FAILED, NULL);
			}
		}
		break;

	default:

		break;
	}
}

/* Tasks */

static void comm_task(void *pvParams){
	dorime_t *dorime = &Comm->_intern.x_dorime;
	TimerHandle_t *x_keep_alive_timer = &Comm->_intern.x_keep_alive_timer;
	TimerHandle_t *x_dorime_tick_timer = &Comm->_intern.x_dorime_tick_timer;
	TimerHandle_t *x_keep_alive_decrement_timer = &Comm->_intern.x_keep_alive_decrement_timer;
	SemaphoreHandle_t *x_dorime_mutex = &Comm->_intern.x_dorime_mutex;
	EventGroupHandle_t *x_comm_event = &Comm->_intern.x_comm_event;
	EventBits_t x_event_bits;

	Comm->_intern.u32_tick_count = 0;
	Comm->_intern.pu8_data_to_send = NULL;
	Comm->_intern.b_is_sending = false;
	Comm->_intern.u32_send_retries = 0;
	Comm->_intern.u16_data_destination = 0;
	Comm->_intern.u32_data_len = 0;
	Comm->_intern.list_keep_alive = NULL;
	Comm->_intern.list_keep_alive_actual = NULL;

	// Configurar o Dorime Serial Protocol
	dorime->u16Address = DORIME_ADDR_CONTROLLER;
	dorime->u32MsPerCount = 10;
	dorime->u32Timeout = Comm->u32_timeout_ms;
	dorime->u32ByteTimeout = 0;
	dorime->u32CooldownTimeout = 50;
	dorime->u32BroadcastGuard = 500;
	dorime->u16MaxDataLen = 256;

	DORIME_FXN_SET_TX(dorime, dorime_tx_fxn);
	DORIME_FXN_SET_RX(dorime, dorime_rx_fxn);
	DORIME_FXN_SET_ABORT(dorime, dorime_abort_fxn);
	DORIME_FXN_SET_DIR(dorime, dorime_dir_fxn);
	DORIME_FXN_SET_LOCK(dorime, dorime_lock_fxn);
	DORIME_FXN_SET_UNLOCK(dorime, dorime_unlock_fxn);
	DORIME_FXN_SET_TCKCNT(dorime, dorime_getcnt_fxn);
	DORIME_FXN_SET_MALLOC(dorime, dorime_alloc_fxn);
	DORIME_FXN_SET_FREE(dorime, dorime_free_fxn);

	dorime_init(dorime);

	// Inicializa timer de 10ms para o Dorime
	*x_dorime_tick_timer = xTimerCreate("Dorime_Tick",
										pdMS_TO_TICKS(10),
										pdTRUE,
										0,
										timer_dorime);
	xTimerStart(*x_dorime_tick_timer, portMAX_DELAY);
	// Inicializa timer de keep alive
	*x_keep_alive_timer = xTimerCreate("KA",
									   pdMS_TO_TICKS(Comm->u32_ka_cycle_ms),
									   pdTRUE,
									   0,
									   timer_keep_alive);
	xTimerStart(*x_keep_alive_timer, portMAX_DELAY);
	*x_keep_alive_decrement_timer = xTimerCreate("KADecrement",
									   pdMS_TO_TICKS(500),
									   pdTRUE,
									   0,
									   timer_keep_alive_decrement);
	// Cria Mutex do Dorime
	*x_dorime_mutex = xSemaphoreCreateMutex();
	// Cria evento de comunicação
	*x_comm_event = xEventGroupCreate();

	for (;;){
		// Espera por evento de comunicação
		x_event_bits = xEventGroupWaitBits(*x_comm_event, 0xFFFF, pdTRUE, pdFALSE, pdMS_TO_TICKS(10));

		switch (x_event_bits){
		case COMM_EVT_GROUP_KEEP_ALIVE:
			comm_keep_alive_list_process(Comm);
			break;

		case COMM_EVT_GROUP_RXTX:
			// just process the dorime events
			break;
		default:

			break;
		}

		dorime_handler(dorime);
	}
}

/* Publics */

void comm_init(comm_t *comm){
	Comm = comm;
	xTaskCreate(comm_task, "Comm", 256, NULL, 2, &Comm->_intern.x_task_handle);
}

void comm_update_params(comm_t *comm, uint32_t u32_timeout_ms, uint32_t u32_retries){
	comm->u32_retries = u32_retries;
	comm->u32_timeout_ms = u32_timeout_ms;

	comm->_intern.x_dorime.u32Timeout = u32_timeout_ms;
}

int8_t comm_data_send(comm_t *comm, uint16_t destination, uint8_t *data, uint32_t len){
	if (dorime_send_data(&comm->_intern.x_dorime, destination, data, len) == DORIME_OK){
		comm->_intern.b_is_sending = true;
		if (Comm->_intern.pu8_data_to_send != NULL){
			vPortFree(Comm->_intern.pu8_data_to_send);
		}
		Comm->_intern.u16_data_destination = destination;
		Comm->_intern.pu8_data_to_send = pvPortMalloc(len);
		Comm->_intern.u32_data_len = len;
		Comm->_intern.u32_send_retries = Comm->u32_retries;

		return 0;
	}

	return -1;
}

int8_t comm_register_keep_alive(comm_t *comm, uint16_t destination){
	comm_keep_alive_list_t *head, *actual;

	head = comm->_intern.list_keep_alive;
	if (head == NULL){
		head = pvPortMalloc(sizeof(comm_keep_alive_list_t));
		if (head == NULL){
			return -1;
		}
		head->u16_destination = destination;
		head->u16_alive_refresh = comm->u32_ka_timeout_ms;
		head->pv_next = NULL;
		comm->_intern.list_keep_alive = head;
	}
	else{
		// check if destination already exists
		while (head != NULL){
			if (head->u16_destination == destination){
				return -1; // already registered
			}
			if (head->pv_next == NULL){
				break;
			}
			head = head->pv_next;
		}
		actual = pvPortMalloc(sizeof(comm_keep_alive_list_t));
		if (actual == NULL){
			return -1;
		}
		actual->u16_destination = destination;
		actual->u16_alive_refresh = comm->u32_ka_timeout_ms;
		actual->pv_next = NULL;
		head->pv_next = actual;
	}

	return 0;
}

int8_t comm_unregister_keep_alive(comm_t *comm, uint16_t destination){
	comm_keep_alive_list_t *head, *actual;

	head = comm->_intern.list_keep_alive;
	// search for the destination in the list
	while (head != NULL){
		if (head->u16_destination == destination){
			// found, remove it from the list
			if (head == comm->_intern.list_keep_alive){
				comm->_intern.list_keep_alive = head->pv_next;
			}
			else{
				actual->pv_next = head->pv_next;
			}
			vPortFree(head);
			return 0;
		}
		actual = head;
		head = head->pv_next;
	}

	// if destination not found, consider it unregistered and return success
	return 0;
}

int8_t comm_get_keep_alive(comm_t *comm, uint16_t destination){
	comm_keep_alive_list_t *head = comm->_intern.list_keep_alive;

	while (head != NULL){
		if (head->u16_destination == destination){
			return (head->u16_alive_refresh > 0);
		}
		head = head->pv_next;
	}

	// if destination not found, return -1
	return -1;
}


/* Privates */
void comm_update_keep_alive_list(comm_t *comm, uint16_t destination){
	comm_keep_alive_list_t *head = comm->_intern.list_keep_alive;

	while (head != NULL){
		if (head->u16_destination == destination){
			head->u16_alive_refresh = comm->u32_ka_timeout_ms;
			return;
		}
		head = head->pv_next;
	}
}

void comm_keep_alive_list_process(comm_t *comm){
	comm_keep_alive_list_t *head = comm->_intern.list_keep_alive;
	comm_keep_alive_list_t *actual = comm->_intern.list_keep_alive_actual;
	static uint8_t u8_keep_alive_byte = COMM_KEEP_ALIVE_SEND;

	if (head == NULL){
		return;
	}

	if (actual == NULL){
		actual = head;
	}
	else{
		comm->_intern.list_keep_alive_actual = actual->pv_next;
	}

	dorime_send_data(&comm->_intern.x_dorime,
				     actual->u16_destination,
					 &u8_keep_alive_byte,
					 1);
}

void comm_keep_alive_list_decrement(comm_t *comm, uint16_t ms){
	comm_keep_alive_list_t *head = comm->_intern.list_keep_alive;

	while (head != NULL){
		if (head->u16_alive_refresh > ms){
			head->u16_alive_refresh -= ms;
		}
		else{
			head->u16_alive_refresh = 0;
		}
		head = head->pv_next;
	}
}
