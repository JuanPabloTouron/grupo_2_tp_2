
/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os.h"
#include "board.h"
#include "logger.h"
#include "dwt.h"

#include <ao_led.h>

/********************** macros and definitions *******************************/


/********************** internal data declaration ****************************/


/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/
extern QueueHandle_t hqueue;
/********************** external data definition *****************************/

/********************** internal functions definition ************************/
/**
 * @brief Función principal del LED.
 *
 * Recibe un handler a un objeto activo de LED, lee la cola propia
 * del OA. Si hay un mensaje, ejecuta la acción correspondiente
 * con el LED.
 */
static void task_(void *argument)
{

  while (true)
  {
    ao_event_t* pevent;

    if (pdPASS == xQueueReceive(hqueue, &pevent, portMAX_DELAY))
    {
      ao_led_handle_t* hao = pevent->hao;
      ao_led_message_t* pmsg = &pevent->msg;
      switch (pmsg->action) {
        case AO_LED_MESSAGE_ON:
          LOGGER_INFO("Led %d ON - LED", hao->color);
          HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,SET);
          break;

        case AO_LED_MESSAGE_OFF:
          LOGGER_INFO("Led %d OFF - LED", hao->color);
          HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,RESET);
          break;

        case AO_LED_MESSAGE_FLASH:
          LOGGER_INFO("Led %d FLASH - LED", hao->color);
          HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,SET);
          vTaskDelay(pdMS_TO_TICKS(1000));
          HAL_GPIO_WritePin(LD2_GPIO_Port,LD2_Pin,RESET);
          break;
        default:
          break;
      }
     pmsg->callback_completed(pmsg);
    }
    vTaskDelete(NULL);
  }
}

/********************** external functions definition ************************/
/**
 * @brief Función para enviar mensajes a la cola del LED.
 *
 * Recibe un handler a un objeto activo de LED, y un mensaje.
 * Lo envía a la cola del OA. Devuelve true si pudo
 * hacerlo y false si no.
 *
 * @param hao Handler al objeto activo.
 * @param msg Mensaje a enviar.
 *
 * @return Booleano que indica éxito.
 */
bool ao_led_send(ao_led_handle_t* hao, ao_led_message_t* pmsg)
{
  ao_event_t* event = (ao_event_t*)pmsg;
  event->hao = hao;
  return (pdPASS == xQueueSend(hqueue, &event, 0));
}

/**
 * @brief Función de inicialización del OA de LED.
 *
 * Recibe un handler a un objeto activo de LED y un color.
 * Le crea una cola y le asigna un handler a su tarea
 *
 * @param hao Handler al objeto activo.
 * @param color Color del LED.
 */
void ao_led_init(ao_led_handle_t* hao, ao_led_color color)
{
  hao->color = color;
  hao->htask = NULL;


}

void new_led_ao(ao_led_handle_t* hao)
{
	BaseType_t status;
	status = xTaskCreate(task_, "task_ao", 128, hao, tskIDLE_PRIORITY, &hao->htask);
	while (pdPASS != status)
	{
	  LOGGER_INFO("Error al crear la tarea - LED");
	}
}

/********************** end of file ******************************************/
