
#ifndef TASK_UI_H_
#define TASK_UI_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "ao_led.h"

/********************** macros ***********************************************/

/********************** typedef **********************************************/
typedef enum
{
  MSG_EVENT_BUTTON_PULSE,
  MSG_EVENT_BUTTON_SHORT,
  MSG_EVENT_BUTTON_LONG,
} msg_event_t;

typedef struct
{
    QueueHandle_t hqueue;
    TaskHandle_t htask;
} ao_ui_handle_t;

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/
static void callback_completed(ao_led_message_t* pmsg);

bool ao_ui_send_event(ao_ui_handle_t* hao, msg_event_t msg);

void ao_ui_init();

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/

void task_ui(void* argument);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_UI_H_ */
/********************** end of file ******************************************/
