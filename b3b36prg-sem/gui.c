#include "gui.h"
#include "xwin_sdl.h"
#include "utils.h"
#include "computation.h"
#include "event_queue.h"

#ifndef SDL_EVENT_POLL_WAIT_MS
#define SDL_EVENT_POLL_WAIT_MS 10
#endif

static struct
{
    int w;
    int h;
    unsigned char *img;
} gui = {.img = NULL};

//create image
void gui_init(void)
{
    get_grid_size(&gui.w, &gui.h);
    gui.img = my_alloc(gui.w * gui.h * 3);
    my_assert(xwin_init(gui.w, gui.h) == 0, __func__, __LINE__, __FILE__);
}

// destroy image
void gui_cleanup(void)
{
    if (gui.img)
    {
        free(gui.img);
        gui.img = NULL;
    }
    xwin_close();
}

// update image
void gui_refresh(void)
{
    if (gui.img)
    {
        update_image(gui.w, gui.h, gui.img);
        xwin_redraw(gui.w, gui.h, gui.img);
    }
}

// control activity via window withiut terminal
// great part of code is collected from sem-video, some remarks were provided
void *gui_win_thread(void *d)
{
    info("gui_win_thread - start");
    bool quit = false;
    SDL_Event event;
    struct event ev;
    while (!quit)
    {
        ev.type = EV_TYPE_NUM;
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_KEYDOWN)
            {
                switch (event.key.keysym.sym)
                {
                case SDLK_q:
                    debug("q was pressed in GUI");
                    ev.type = EV_QUIT;
                    set_quit();
                    break;
                case SDLK_s:
                    ev.type = EV_SET_COMPUTE;
                    break;
                case SDLK_a:
                    ev.type = EV_ABORT;
                    break;
                case SDLK_c:
                    if (!is_computing() || (is_abort() && is_computing()))
                    {
                        ev.type = EV_COMPUTE;
                    }
                    break;
                case SDLK_g:
                    ev.type = EV_GET_VERSION;
                    break;
                }
            }
        } // end SDL_PollEvent
        if (ev.type != EV_TYPE_NUM)
        {
            queue_push(ev);
        }
        SDL_Delay(SDL_EVENT_POLL_WAIT_MS);
        quit = is_quit();
    }
    //set_quit();
    info("gui_win_thread - end");
    return NULL;
}