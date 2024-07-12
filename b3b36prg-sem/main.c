#include "utils.h"
#include "messages.h"
#include "event_queue.h"
#include "computation.h"
#include "gui.h"
#include "main.h"

static void process_pipe_message(event *const ev);

void *main_thread(void *d)
{
    my_assert(d != NULL, __func__, __LINE__, __FILE__);
    int pipe_out = *(int *)d;
    message msg;
    uint8_t msg_buf[sizeof(message)];
    int msg_len;
    bool quit = false;

    computation_init();
    gui_init();

    // process events 
    do
    {
        event ev = queue_pop();
        msg.type = MSG_NBR;
        switch (ev.type)
        {
            case EV_QUIT:
                set_quit();
                debug("Quit received");
                break;
            case EV_GET_VERSION:
                msg.type = MSG_GET_VERSION;
                break;
            case EV_SET_COMPUTE:
                info(set_compute(&msg) ? "set compute" : "fail set compute");
                break;
            case EV_COMPUTE:
                enable_comp();
                info(compute(&msg) ? "compute" : "fail compute");
                break;
            case EV_ABORT:
                abort_comp();
                msg.type = MSG_ABORT;
                break;
            case EV_PIPE_IN_MESSAGE:
                process_pipe_message(&ev);
                break;
            default:
                break;
        } // end switch
        // send msg to pipe
        if (msg.type != MSG_NBR)
        {
            my_assert(fill_message_buf(&msg, msg_buf, sizeof(message), &msg_len), __func__, __LINE__, __FILE__);
            if (write(pipe_out, msg_buf, msg_len) == msg_len)
            {
                debug("send date to pipe_out");
            }
            else
            {
                error("send message failed!");
            }
        }
        quit = is_quit();
    } while (!quit);
    
    gui_cleanup();
    computation_cleanup();
    debug("main_thread - finished");
    return NULL;
}

// if got message from comp_module
void process_pipe_message(event *const ev)
{
    my_assert(ev != NULL && ev->type == EV_PIPE_IN_MESSAGE && ev->data.msg, __func__, __LINE__, __FILE__);
    ev->type = EV_TYPE_NUM;
    const message *msg = ev->data.msg;
    switch (msg->type)
    {
    case MSG_OK:
        info("OK");
        break;
    case MSG_VERSION:
        fprintf(stderr, "INFO: Module version %d.%d-p%d\n", msg->data.version.major, msg->data.version.minor, msg->data.version.patch);
        break;
    case MSG_COMPUTE_DATA:
        if (!is_abort() && !is_quit())
        {
            update_data(&(msg->data.compute_data)); // updates chunk of grid
        }
        break;
    case MSG_DONE:
        gui_refresh();
        if (is_done() && !is_abort())
        {
            info("Computation done");
        }
        else if (!is_abort())
        {
            event ev = {.type = EV_COMPUTE};
            queue_push(ev);   
        }
        info("msg_done");
        break;
    case MSG_ABORT:
        info("Computation aborted");
        abort_comp();
        break;
    default:
        fprintf(stderr, "Unhandled pipe message type %d\n", msg->type);
        break;
    }
    free(ev->data.msg);
    ev->data.msg = NULL;
}