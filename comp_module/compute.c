#include "utils.h"
#include "comp_queue.h"
#include "compute.h"

double abs_val(double re, double im){
    double ret_val = 0.0;
    ret_val = sqrt(re * re + im * im);
    return ret_val;
}

uint8_t max_iter(uint8_t n, double z_re, double z_im, double c_re, double c_im){
    uint8_t k = 0;
    double new_z_re = z_re;
    double new_z_im = z_im;
    while(abs_val(new_z_re, new_z_im) < 2.0 && k < n){
        k+=1;
        z_re = new_z_re;
        z_im = new_z_im;
        new_z_re = (z_re * z_re) - (z_im * z_im) + c_re;
        new_z_im = (2.0 * z_re * z_im) + c_im;
    }
    return k;
}

void set_cur_comp(message* msg, struct current_computation cur_comp){
    my_assert(msg != NULL && msg->type == MSG_SET_COMPUTE, __func__, __LINE__, __FILE__);
    cur_comp.c_re = msg->data.set_compute.c_re;
    cur_comp.c_im = msg->data.set_compute.c_im;
    cur_comp.d_re = msg->data.set_compute.d_re > 0 ? msg->data.set_compute.d_re : -1.0 * msg->data.set_compute.d_re;
    cur_comp.d_im = msg->data.set_compute.d_im > 0 ? msg->data.set_compute.d_im : -1.0 * msg->data.set_compute.d_im;
    cur_comp.n = msg->data.set_compute.n;
}

void set_abort(struct current_computation cur_comp) { cur_comp.abort = true; }

bool is_abort(struct current_computation cur_comp) { return cur_comp.abort; }

