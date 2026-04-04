
/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include "ao_ui.h"
#include "ao_led.h"
#include "memory_pool.h"

/********************** macros and definitions *******************************/

#define QUEUE_LENGTH_            (1)
#define QUEUE_ITEM_SIZE_         (sizeof(msg_event_t))

/********************** internal data declaration ****************************/



/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
uint8_t msg_wip = 0;


/********************** external data definition *****************************/
//Declaración de objetos activos definidos en el app.c
extern ao_led_handle_t led_red;
extern ao_led_handle_t led_green;
extern ao_led_handle_t led_blue;

extern memory_pool_t* const hmp;

/********************** internal functions definition ************************/
/*	Función de callback */
static void callback_completed_(ao_led_message_t* pmsg)
{
	  memory_pool_block_put(hmp, (void*)pmsg);
	  LOGGER_INFO("Memoria liberada desde la UI");
	  msg_wip--;
	  LOGGER_INFO("Mensajes en proceso: %d", msg_wip);
}


/**
 * @brief Función principal de la interfaz de usuario.
 *
 * Recibe un handler a un objeto activo de interfaz de usuario,
 * apaga los LEDs, lee la cola propia del OA. Si hay un mensaje,
 * enciende el LED que corresponde por 1 segundo.
 */
static void task_(void *argument)
{

  ao_ui_handle_t* hao = (ao_ui_handle_t*)argument;


  //pled_msg->action = AO_LED_MESSAGE_OFF;
  //pled_msg->callback_completed = callback_completed_;

  //ao_led_send(&led_red, led_msg);
  //ao_led_send(&led_green, led_msg);
  //ao_led_send(&led_blue, led_msg);

  while (true)
  {

    msg_event_t event_msg;
    if (pdPASS == xQueueReceive(hao->hqueue, &event_msg, portMAX_DELAY))
    {
      ao_led_message_t* pled_msg = (ao_led_message_t*) memory_pool_block_get(hmp);

      if (pled_msg != NULL)
      {
          pled_msg->action = AO_LED_MESSAGE_FLASH;
          pled_msg->callback_completed = callback_completed_;
          bool msg_success = true;
          switch (event_msg)
          {
            case MSG_EVENT_BUTTON_PULSE:
              LOGGER_INFO("Encender LED rojo - UI");
              msg_success = ao_led_send(&led_red, pled_msg);
              break;
            case MSG_EVENT_BUTTON_SHORT:
              LOGGER_INFO("Encender LED verde - UI");
              msg_success = ao_led_send(&led_green, pled_msg);
              break;
            case MSG_EVENT_BUTTON_LONG:
              LOGGER_INFO("Encender LED azul - UI");
              msg_success = ao_led_send(&led_blue, pled_msg);
              break;
            default:
              break;
          }
          if (msg_success == true)
          {
        	  msg_wip++;
        	  LOGGER_INFO("Mensaje enviado con éxito. Mensajes en proceso: %d",msg_wip);

          }
          else
          {
        	  memory_pool_block_put(hmp,(void*)pled_msg);
        	  LOGGER_INFO("Memoria liberada desde UI, no pudo enviarse el mensaje.");
          }
      }
      else
      {
    	  LOGGER_INFO("Memoria insuficiente.");
      }
    }
  }
}

/********************** external functions PULSEdefinition ************************/
/**
 * @brief Función para enviar mensajes a la cola de la UI.
 *
 * Recibe un handler a un objeto activo de interfaz de usuario,
 * y un mensaje. Lo envía a la cola del OA. Devuelve true si pudo
 * hacerlo y false si no.
 *
 * @param hao Handler al objeto activo.
 * @param msg Mensaje a enviar.
 *
 * @return Booleano que indica éxito.
 */
bool ao_ui_send_event(ao_ui_handle_t* hao, msg_event_t msg)
{
  return (pdPASS == xQueueSend(hao->hqueue, (void*)&msg, 0));
}

/**
 * @brief Función de inicialización del OA de UI.
 *
 * Recibe un handler a un objeto activo de interfaz de usuario.
 * Le crea una cola y le asigna un handler a su tarea
 *
 * @param hao Handler al objeto activo.
 */
void ao_ui_init(ao_ui_handle_t* hao)
{
  hao->hqueue = xQueueCreate(QUEUE_LENGTH_, QUEUE_ITEM_SIZE_);
  while(NULL == hao->hqueue)
  {
	  LOGGER_INFO("Error al crear la cola - UI");
  }

  BaseType_t status;
  status = xTaskCreate(task_, "task_ao_ui", 128, hao, tskIDLE_PRIORITY, &hao->htask);
  while (pdPASS != status)
  {
	  LOGGER_INFO("Error al crear la tarea - UI");
  }
}

/********************** end of file ******************************************/
