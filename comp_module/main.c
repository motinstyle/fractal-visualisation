#include "utils.h"
#include "event_queue.h"
#include "compute.h"
#include "main.h"
#include "messages.h"
#include "keyboard.h"

static void send_message(message msg, uint8_t* msg_buf, int msg_len, int pipe_out);

static void ok_msg(int pipe_out);

// input: pipe out
void *main_thread(void *d)
{
    my_assert(d != NULL, __func__, __LINE__, __FILE__);
    int pipe_out = *(int *)d;
    message msg;
    uint8_t msg_buf[sizeof(message)];
    int msg_len = 0;

    // send startup message 
    message msg_start;
    uint8_t msg_start_buf[sizeof(message)];
    int msg_start_len = 0;
    msg_start.type = MSG_STARTUP;
    msg_start.data.startup.message[0] = 'k';
    msg_start.data.startup.message[1] = 'u';
    msg_start.data.startup.message[2] = 's';
    msg_start.data.startup.message[3] = 'h';
    msg_start.data.startup.message[4] = 'n';
    msg_start.data.startup.message[5] = 'm';
    msg_start.data.startup.message[6] = 'a';
    msg_start.data.startup.message[7] = 't';
    msg_start.data.startup.message[8] = '\0';
    send_message(msg_start, msg_start_buf, msg_start_len, pipe_out);
 
    struct current_computation cur_comp = {.abort = false};

    // process events 
    while(!is_quit())
    {
        event ev = queue_pop();
        msg.type = MSG_NBR;
        if (ev.type == EV_QUIT){
            free(ev.data.msg);
            msg.type = MSG_DONE;
            break;
        }
        switch (ev.data.msg->type){

            case MSG_GET_VERSION:
                msg.type = MSG_VERSION;
                msg.data.version.major = 1;
                msg.data.version.minor = 0;
                msg.data.version.patch = 1;
                break;
            case MSG_ABORT:
                ok_msg(pipe_out);
                set_abort(cur_comp);
                info("computation aborted");
                break;
            case MSG_SET_COMPUTE:
                ok_msg(pipe_out);
                cur_comp.c_re = ev.data.msg->data.set_compute.c_re;
                cur_comp.c_im = ev.data.msg->data.set_compute.c_im;
                cur_comp.d_re = ev.data.msg->data.set_compute.d_re > 0 ? ev.data.msg->data.set_compute.d_re : -1.0 * ev.data.msg->data.set_compute.d_re;
                cur_comp.d_im = ev.data.msg->data.set_compute.d_im > 0 ? ev.data.msg->data.set_compute.d_im : -1.0 * ev.data.msg->data.set_compute.d_im;
                cur_comp.n = ev.data.msg->data.set_compute.n;
                break;
            case MSG_COMPUTE:

                ok_msg(pipe_out);
                
                cur_comp.cid = ev.data.msg->data.compute.cid;
                cur_comp.re = ev.data.msg->data.compute.re;
                cur_comp.im = ev.data.msg->data.compute.im;
                cur_comp.n_re = ev.data.msg->data.compute.n_re;
                cur_comp.n_im = ev.data.msg->data.compute.n_im;

                double max_re = cur_comp.re + cur_comp.d_re * cur_comp.n_re;
                double max_im = cur_comp.im + cur_comp.d_im * cur_comp.n_im;
                
                uint8_t pixel_re = 0;
                uint8_t pixel_im = 0;

                for (double cur_y = max_im; cur_y > cur_comp.im ; cur_y-=cur_comp.d_im, pixel_im++)
                {
                    pixel_re = 0;
                    for (double cur_x = cur_comp.re; cur_x < max_re; cur_x+=cur_comp.d_re, pixel_re++)
                    {
                        message msg_out;
                        msg_out.type = MSG_COMPUTE_DATA;
                        uint8_t msg_out_buf[sizeof(message)];
                        int msg_out_len = 0;
                        msg_out.data.compute_data.cid = cur_comp.cid;
                        msg_out.data.compute_data.i_re = pixel_re;
                        msg_out.data.compute_data.i_im = pixel_im;
                        msg_out.data.compute_data.iter = max_iter(cur_comp.n, cur_x, cur_y, cur_comp.c_re, cur_comp.c_im);
                        
                        send_message(msg_out, msg_out_buf, msg_out_len, pipe_out);
                    }
                }
                msg.type = MSG_DONE;
                break;
            default:
                msg.type = MSG_ERROR;
                error("error message occuried");
                break; 
        }
        free(ev.data.msg);

        if (msg.type != MSG_NBR)
        {
            send_message(msg, msg_buf, msg_len, pipe_out);
        }
    }

    debug("finish main_thread");
    set_quit();
    queue_cleanup();

    return NULL;
}

void send_message(message msg, uint8_t* msg_buf, int msg_len, int pipe_out)
{
    my_assert(fill_message_buf(&msg, msg_buf, sizeof(message), &msg_len), __func__, __LINE__, __FILE__);

    // --- if you want to get notification every time data is sent ---

    // if (write(pipe_out, msg_buf, msg_len) == msg_len)
    // {
    //     debug("send data to pipe_out");
    // }
    // else
    // {
    //     error("send message failed!");
    // }
    if (write(pipe_out, msg_buf, msg_len) != msg_len){
        error("send message failed!");
    }
}

void ok_msg(int pipe_out){
    message msg_ok = { .type = MSG_OK};
    uint8_t msg_ok_buf[sizeof(message)];
    int msg_ok_len = 0;
    send_message(msg_ok, msg_ok_buf, msg_ok_len, pipe_out);
}

