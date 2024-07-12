#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

void* keyboard_thread(void *d);
bool is_quit_key();

#endif