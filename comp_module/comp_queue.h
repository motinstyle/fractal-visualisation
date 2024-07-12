#ifndef __COMP_QUEUE_H__
#define __COMP_QUEUE_H__

#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>

#include "messages.h"


void queue_msg_init(void);
void queue_cleanup_msg(void);

message* queue_pop_msg(void);

void queue_push_msg(message* msg);

bool is_quit_msg();

void set_quit_msg();

#endif