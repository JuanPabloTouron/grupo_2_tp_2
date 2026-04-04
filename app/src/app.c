/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/
#include "main.h"
#include "cmsis_os.h"
#include "logger.h"
#include "dwt.h"
#include "board.h"

//Inclusión de los archivos desarrollados
#include "task_button.h"
#include "ao_led.h"
#include "ao_ui.h"
#include "memory_pool.h"

/********************** macros and definitions *******************************/

#define QUEUE_LENGTH_            (1)
#define QUEUE_ITEM_SIZE_         (sizeof(ao_event_t*))

#define MEMORY_POOL_NBLOCKS       (10)
#define MEMORY_POOL_BLOCK_SIZE    (sizeof(ao_event_t))

/********************** internal data declaration ****************************/
//Instanciación del objeto activo de la interfaz de usuario
ao_ui_handle_t ui;

//Instanciación de los objetos activos de los LEDs
ao_led_handle_t led_red;
ao_led_handle_t led_green;
ao_led_handle_t led_blue;

QueueHandle_t hqueue;

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
static memory_pool_t memory_pool_;
static uint8_t memory_pool_memory_[MEMORY_POOL_SIZE(MEMORY_POOL_NBLOCKS, MEMORY_POOL_BLOCK_SIZE)];

/********************** external data declaration *****************************/
memory_pool_t* const hmp = &memory_pool_;
/********************** external functions definition ************************/
void app_init(void)
{
  hqueue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
  while (NULL == hqueue)
  {
	  LOGGER_INFO("Error al crear la cola - APP");
  }

  memory_pool_init(hmp, memory_pool_memory_, MEMORY_POOL_NBLOCKS, MEMORY_POOL_BLOCK_SIZE);

  //Inicialización del objeto activo de la interfaz de usuario
  ao_ui_init(&ui);

  //Inicialización de los objetos activos de los LEDs
  ao_led_init(&led_red,AO_LED_COLOR_RED);
  ao_led_init(&led_green,AO_LED_COLOR_GREEN);
  ao_led_init(&led_blue,AO_LED_COLOR_BLUE);

  //Creación de la tarea del botón
  BaseType_t status;
  status = xTaskCreate(task_button, "task_button", 128, NULL, tskIDLE_PRIORITY, NULL);
  while (pdPASS != status)
  {
	  LOGGER_INFO("Error al crear la tarea del botón - APP");
  }


  LOGGER_INFO("Aplicación inicializada - APP");

  cycle_counter_init();
}

/********************** end of file ******************************************/
