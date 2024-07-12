#include "utils.h"
#include "event_queue.h"
#include "main.h"


void* keyboard_thread(void *d){
    call_termios(0);

    while (1){
        if(getchar() == 'q'){
            set_quit();
            event ev = {.type = EV_QUIT};
            queue_push(ev);
            break;
        }
    }

    call_termios(1);
    return NULL;
}
