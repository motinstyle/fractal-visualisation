#include "computation.h"
#include "utils.h"

// extra srtuct to hold user parameters
static struct
{
    unsigned int start_n;
    unsigned int start_size_w;
    unsigned int start_size_h;
    unsigned int start_cell_w;
    unsigned int start_cell_h;
    double start_c_re;
    double start_c_im;
    double start_re_min;
    double start_im_min;
    double start_re_max;
    double start_im_max;
} params = {
    .start_n = 60,
    .start_c_re = -0.4,
    .start_c_im = 0.6,
    .start_size_w = 640,
    .start_size_h = 480,
    .start_cell_w = 64,
    .start_cell_h = 48,
    .start_re_min = -1.6,
    .start_re_max = 1.6,
    .start_im_min = -1.1,
    .start_im_max = 1.1
};

void params_print()
{
    fprintf(stderr, "USE PARAMS IF NEEDED:\n\n");
    fprintf(stderr, "n - change number of iterrations\n");
    fprintf(stderr, "a - change 'min' number\n");
    fprintf(stderr, "b - change 'max' number\n");
    fprintf(stderr, "c - change c parametr\n");
    fprintf(stderr, "x - change size of cell\n");
    fprintf(stderr, "s - change size of the window\n\n");
    fprintf(stderr, "ENTER to include, 'f' to finish\n\n");
    fprintf(stderr, "example: \n");
    fprintf(stderr, "n   *click ENTER* 50        *click ENTER*\n");
    fprintf(stderr, "a   *click ENTER* 1 2       *click ENTER*\n");
    fprintf(stderr, "b   *click ENTER* 1 2       *click ENTER*\n");
    fprintf(stderr, "c   *click ENTER* 1 2       *click ENTER*\n");
    fprintf(stderr, "x   *click ENTER* 64 48     *click ENTER*\n");
    fprintf(stderr, "s   *click ENTER* 640 480   *click ENTER*\n");
}

// terminal interface to get correct parametrs 
void process_params()
{
    char command = '\0';
    while (command != 'f')
    {
        if (scanf(" %c", &command) == 0)
        {
            error("wrong input!");
            continue;
        }
        switch (command)
        {
        case 'n':
            unsigned int prev_n = params.start_n;
            if (!scanf("%u", &params.start_n))
            {
                error("wrong input!");
                params.start_n = prev_n;
            }
            break;
        case 's':
            unsigned int prev_w = params.start_size_w;
            unsigned int prev_h = params.start_size_h;
            if (!scanf("%u %u", &params.start_size_w, &params.start_size_h))
            {
                error("wrong input!");
                params.start_size_w = prev_w;
                params.start_size_h = prev_h;
            }
            break;
        case 'a':
            unsigned int prev_a_re = params.start_re_min;
            unsigned int prev_a_im = params.start_im_min;
            if (!scanf("%lf %lf", &params.start_re_min, &params.start_im_min))
            {
                error("wrong input!");
                params.start_re_min = prev_a_re;
                params.start_im_min = prev_a_im;
            }
            if (params.start_re_min > params.start_re_max || params.start_im_min > params.start_im_max)
            {
                error("error: min value must be smaller then max");
                params.start_re_min = prev_a_re;
                params.start_im_min = prev_a_im;
            }
            break;
        case 'b':
            unsigned int prev_b_re = params.start_re_max;
            unsigned int prev_b_im = params.start_im_max;
            if (!scanf("%lf %lf", &params.start_re_max, &params.start_im_max))
            {
                error("wrong input!");
                params.start_re_max = prev_b_re;
                params.start_im_max = prev_b_im;
            }
            if (params.start_re_max < params.start_re_min || params.start_im_max < params.start_im_min)
            {
                error("error: max value must be greater then min");
                params.start_re_max = prev_b_re;
                params.start_im_max = prev_b_im;
            }
            break;
        case 'c':
            unsigned int prev_c_re = params.start_c_re;
            unsigned int prev_c_im = params.start_c_im;
            if (!scanf("%lf %lf", &params.start_c_re, &params.start_c_im))
            {
                error("wrong input!");
                params.start_c_re = prev_c_re;
                params.start_c_im = prev_c_im;
            }
            break;
        case 'x':
            unsigned int prev_cell_w = params.start_cell_w;
            unsigned int prev_cell_h = params.start_cell_h;
            if (!scanf("%d %d", &params.start_cell_w, &params.start_cell_h))
            {
                error("wrong input!");
                params.start_cell_w = prev_cell_w;
                params.start_cell_h = prev_cell_h;
            }
            if ((params.start_size_h * params.start_size_w) % (params.start_cell_w * params.start_cell_h) != 0)
            {
                error("grid wont be displayed correct");
                error("cell size must divide grid size");
                error("you can change grid size or cell size");
            }
            break;
        case 'f':
            info("finishing entering params");
            return;
        default:
            error("wrong command, try from the list above");
            break;
        }
    }
    return;
}

// main struct to hold computational parameters
static struct
{
    double c_re;
    double c_im;
    int n;

    double range_re_min;
    double range_re_max;
    double range_im_min;
    double range_im_max;

    int grid_w;
    int grid_h;

    int cur_x;
    int cur_y;

    double d_re;
    double d_im;

    int nbr_chunks;
    int cid;
    int reset_cid;
    double chunk_re;
    double chunk_im;

    uint8_t chunk_n_re;
    uint8_t chunk_n_im;

    uint8_t *grid;
    bool computing;
    bool abort;
    bool was_aborted;
    bool done;

} comp = {

    .grid = NULL,
    .reset_cid = 0,

    .computing = false,
    .abort = false,
    .was_aborted = false,
    .done = false

};

// initialize parameters
void computation_init(void)
{

    comp.n = params.start_n;
    comp.c_re = params.start_c_re;
    comp.c_im = params.start_c_im;
    comp.grid_w = params.start_size_w;
    comp.grid_h = params.start_size_h;
    comp.chunk_n_re = params.start_cell_w;
    comp.chunk_n_im = params.start_cell_h;
    comp.range_re_min = params.start_re_min;
    comp.range_re_max = params.start_re_max;
    comp.range_im_min = params.start_im_min;
    comp.range_im_max = params.start_im_max;

    comp.grid = my_alloc(comp.grid_w * comp.grid_h);
    comp.d_re = (comp.range_re_max - comp.range_re_min) / (1. * comp.grid_w);
    comp.d_im = -(comp.range_im_max - comp.range_im_min) / (1. * comp.grid_h);
    comp.nbr_chunks = (comp.grid_w * comp.grid_h) / (comp.chunk_n_re * comp.chunk_n_im);
}

// destroy computational grid
void computation_cleanup(void)
{
    if (comp.grid)
    {
        free(comp.grid);
    }
    comp.grid = NULL;
}

// status of compoletness of different stages
bool is_computing(void) { return comp.computing; }
bool is_done(void) { return comp.done; }
bool is_abort(void) { return comp.abort; }

void get_grid_size(int *w, int *h)
{
    *w = comp.grid_w;
    *h = comp.grid_h;
}

void abort_comp(void)
{
    comp.abort = true;
    comp.was_aborted = true;
}

void enable_comp(void) { comp.abort = false; }

// set computational parameters
bool set_compute(message *msg)
{
    my_assert(msg != NULL, __func__, __LINE__, __FILE__);
    bool ret = !is_computing();
    if (ret)
    {
        msg->type = MSG_SET_COMPUTE;
        msg->data.set_compute.c_re = comp.c_re;
        msg->data.set_compute.c_im = comp.c_im;
        msg->data.set_compute.d_re = comp.d_re;
        msg->data.set_compute.d_im = comp.d_im;
        msg->data.set_compute.n = comp.n;
        comp.done = false;
    }
    return ret;
}

// set actual chunk and a cell of computation 
bool compute(message *msg)
{
    my_assert(msg != NULL, __func__, __LINE__, __FILE__);
    if (!is_computing())
    { // fisrt chunk
        comp.cid = 0;
        comp.computing = true;
        comp.cur_x = comp.cur_y = 0;       // start computation of the whole image
        comp.chunk_re = comp.range_re_min; // upper left corner
        comp.chunk_im = comp.range_im_max; // upper left corner
        msg->type = MSG_COMPUTE;
    }
    else
    { // next chunk
        if (!comp.was_aborted)
        { // if wasnt aborted
            comp.cid += 1;
            if (comp.cid == 256)
            {
                comp.reset_cid++;
                comp.cid = 0;
            }
            if (comp.cid + 256 * comp.reset_cid < comp.nbr_chunks)
            {
                comp.cur_x += comp.chunk_n_re;
                comp.chunk_re += comp.chunk_n_re * comp.d_re;
                if (comp.cur_x >= comp.grid_w)
                {
                    comp.chunk_re = comp.range_re_min;
                    comp.chunk_im += comp.chunk_n_im * comp.d_im;
                    comp.cur_x = 0;
                    comp.cur_y += comp.chunk_n_im;
                }
                msg->type = MSG_COMPUTE;
            }
            else
            { // all has been compute
            }
        }
        else
        { // if was aborted
            comp.was_aborted = !comp.was_aborted;
            msg->type = MSG_COMPUTE;
        }
    }

    if (comp.computing && msg->type == MSG_COMPUTE)
    {
        msg->data.compute.cid = comp.cid;
        msg->data.compute.re = comp.chunk_re;
        msg->data.compute.im = comp.chunk_im;
        msg->data.compute.n_re = comp.chunk_n_re;
        msg->data.compute.n_im = comp.chunk_n_im;
    }

    return is_computing();
}

// turn grid into img
void update_image(int w, int h, unsigned char *img)
{
    my_assert(img && comp.grid && w == comp.grid_w && h == comp.grid_h, __func__, __LINE__, __FILE__);
    for (int i = 0; i < w * h; ++i)
    {
        const double t = 1. * comp.grid[i] / (comp.n + 1.0);
        *(img++) = 9 * (1 - t) * t * t * t * 255;
        *(img++) = 15 * (1 - t) * (1 - t) * t * t * 255;
        *(img++) = 8.5 * (1 - t) * (1 - t) * (1 - t) * t * 255;
    }
}

// actualising of grid
void update_data(const msg_compute_data *compute_data)
{
    my_assert(compute_data != NULL, __func__, __LINE__, __FILE__);
    if (compute_data->cid == comp.cid)
    {
        const int idx = comp.cur_x + compute_data->i_re + (comp.cur_y + compute_data->i_im) * comp.grid_w;
        if (idx >= 0 && idx < (comp.grid_w * comp.grid_h))
        {
            comp.grid[idx] = compute_data->iter;
        }
        if ((comp.cid + 256 * comp.reset_cid + 1) >= comp.nbr_chunks 
            && (compute_data->i_re + 1) == comp.chunk_n_re 
            && (compute_data->i_im + 1) == comp.chunk_n_im)
        {
            comp.done = true;
            comp.computing = false;
        }
    }
    else
    {
        warn("Received chunk with unexpected chunk id (cid)");
    }
}
