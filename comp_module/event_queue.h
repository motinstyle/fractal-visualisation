// author: Jan Faigl

#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__

#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

#include "messages.h"

typedef enum
{
   EV_COMPUTE,      
   EV_ABORT,
   EV_GET_VERSION,
   EV_QUIT,
   EV_SET_COMPUTE,
   EV_PIPE_IN_MESSAGE,
   EV_TYPE_NUM
} event_type;

typedef struct event
{
   event_type type;
   union
   {
      int param;
      message *msg;
   } data;
} event;

void queue_init(void);
void queue_cleanup(void);

event queue_pop(void);

void queue_push(event ev);

bool is_quit();

void set_quit();

#endif

/* end of event_queue.h */
