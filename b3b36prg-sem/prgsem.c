#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

#include "utils.h"
#include "keyboard.h"
#include "main.h"
#include "prg_io_nonblock.h"
#include "event_queue.h"
#include "computation.h"
#include "gui.h"

#ifndef IO_READ_TIMEOUT_MS
#define IO_READ_TIMEOUT_MS 100
#endif

void *read_pipe_thread(void *);

int main(int argc, char *argv[])
{
    int ret = EXIT_SUCCESS;
    
    // initialize pipe
    const char *fname_pipe_input = argc > 1 ? argv[1] : "/tmp/computational_module.out";
    const char *fname_pipe_output = argc > 2 ? argv[2] : "/tmp/computational_module.in";

    // initialize parameters
    params_print();
    process_params();

    // "open pipe"
    int pipe_input = io_open_read(fname_pipe_input);
    int pipe_output = io_open_write(fname_pipe_output);

    // initialize threads
    my_assert(pipe_input != -1 && pipe_output != -1, __func__, __LINE__, __FILE__);
    enum
    {
        READ_PIPE_THRD,
        MAIN_THRD,
        KEYBOARD_THRD,
        WIN_THRD,
        NUM_THREADS
    };
    const char *thrd_names[] = {"ReadPipe", "Main", "Keyboard", "GuiWin"};
    void *(*thrd_functions[])(void *) = {read_pipe_thread, main_thread, keyboard_thread, gui_win_thread };
    pthread_t threads[NUM_THREADS];
    void *thrd_data[NUM_THREADS] = {};
    thrd_data[READ_PIPE_THRD] = &pipe_input;
    thrd_data[MAIN_THRD] = &pipe_output;

    // create threads
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        int r = pthread_create(&threads[i], NULL, thrd_functions[i], thrd_data[i]);
        printf("Create thread '%s' %s\r\n", thrd_names[i], (r == 0 ? "OK" : "FAIL"));
    }

    // finish threads
    int *ex;
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        printf("Call join to the thread %s\r\n", thrd_names[i]);
        int r = pthread_join(threads[i], (void *)&ex);
        printf("Joining the thread %s has been %s\r\n", thrd_names[i], (r == 0 ? "OK" : "FAIL"));
    }

    // close pipe
    io_close(pipe_input);
    io_close(pipe_output);
    return ret;
}

// thread that recieves data from pipe
void *read_pipe_thread(void *d)
{
    my_assert(d != NULL, __func__, __LINE__, __FILE__);
    int pipe_input = *(int *)d;

    fprintf(stderr, "read_pipe_thread - start\n");
    bool end = false;
    uint8_t msg_buf[sizeof(message)]; // aka 1 msg
    int i = 0;
    int len = 0;

    unsigned char c;
    while (io_getc_timeout(pipe_input, IO_READ_TIMEOUT_MS, &c) > 0)
    {
    }; // discard garbage

    while (!end)
    {
        int r = io_getc_timeout(pipe_input, IO_READ_TIMEOUT_MS, &c);
        if (r > 0)
        { // char has been read
            // fill message and push to the queue
            if (i == 0)
            {
                len = 0;
                if (get_message_size(c, &len))
                {
                    msg_buf[i++] = c;
                }
                else
                {
                    fprintf(stderr, "ERROR: unknown message type0x%x\n", c);
                }
            }
            else
            { // read remaining bytes of the message
                msg_buf[i++] = c;
            }
            if (i == len && len > 0)
            {
                message *msg = my_alloc(sizeof(message));
                if (parse_message_buf(msg_buf, len, msg))
                {
                    event ev = {.type = EV_PIPE_IN_MESSAGE};
                    ev.data.msg = msg;
                    queue_push(ev);
                }
                else
                {
                    fprintf(stderr, "ERROR: cannot parse massage type %d\n", msg_buf[0]);
                    free(msg);
                }
                i = len = 0;
            }
        }
        else if (r == 0)
        { // timeout
        }
        else
        { // error
            fprintf(stderr, "ERROR: reading from pipe\n");
            set_quit();
            event ev = {.type = EV_QUIT};
            queue_push(ev);
        }
        end = is_quit();
    } // end while

    fprintf(stderr, "read_pipe_thread - finished\n");
    return NULL;
}