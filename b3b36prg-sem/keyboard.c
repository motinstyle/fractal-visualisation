#include "keyboard.h"
#include "utils.h"
#include "computation.h"
#include "event_queue.h"

// process keys from keyboard
// push an event tot queue
void *keyboard_thread(void *d)
{
    fprintf(stderr, "keyboard_thread - start\n");
    call_termios(0);
    int c;
    event ev;
    while ((c = getchar()) != 'q' && !is_quit())
    {
        ev.type = EV_TYPE_NUM;
        switch (c)
        {
        case 'g':
            ev.type = EV_GET_VERSION;
            debug("g pressed");
            break;
        case 'a':
            ev.type = EV_ABORT;
            break;
        case 's':
            ev.type = EV_SET_COMPUTE;
            break;
        case 'c':
            if (!is_computing() || (is_abort() && is_computing()))
            {
                ev.type = EV_COMPUTE;
            }
            break;
        default:
            break;

        } // end switch
        if (ev.type != EV_TYPE_NUM)
        {
            queue_push(ev);
        }
    } // end while

    ev.type = EV_QUIT;
    queue_push(ev);
    set_quit();
    call_termios(1); // restore terminal settings
    fprintf(stderr, "keyboard_thread - finished\n");
    return NULL;
}