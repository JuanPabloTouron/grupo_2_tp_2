
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

/********************** macros and definitions *******************************/

#define QUEUE_LENGTH_            (1)
#define QUEUE_ITEM_SIZE_         (sizeof(msg_event_t))

/********************** internal data declaration ****************************/



/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/



/********************** external data definition *****************************/
//Declaración de objetos activos definidos en el app.c
extern ao_led_handle_t led_red;
extern ao_led_handle_t led_green;
extern ao_led_handle_t led_blue;

/********************** internal functions definition ************************/
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

  ao_led_message_t led_msg;
  led_msg.action = AO_LED_MESSAGE_OFF;
  ao_led_send(&led_red, led_msg);
  ao_led_send(&led_green, led_msg);
  ao_led_send(&led_blue, led_msg);


  while (true)
  {

    msg_event_t event_msg;


    if (pdPASS == xQueueReceive(hao->hqueue, &event_msg, portMAX_DELAY))
    {
      led_msg.action = AO_LED_MESSAGE_FLASH;
      switch (event_msg)
      {
        case MSG_EVENT_BUTTON_PULSE:
          LOGGER_INFO("Encender LED rojo - UI");
          ao_led_send(&led_red, led_msg);
          break;
        case MSG_EVENT_BUTTON_SHORT:
          LOGGER_INFO("Encender LED verde - UI");
          ao_led_send(&led_green, led_msg);
          break;
        case MSG_EVENT_BUTTON_LONG:
          LOGGER_INFO("Encender LED azul - UI");
          ao_led_send(&led_blue, led_msg);
          break;
        default:
          break;
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
