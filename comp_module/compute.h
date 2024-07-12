#ifndef __COMPUTE_H__
#define __COMPUTE_H__

#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "messages.h"

struct current_computation {
    double re;    // start of the x-coords (real)
    double im;    // start of the y-coords (imaginary)
    double c_re;  // re (x) part of the c constant in recursive equation
    double c_im;  // im (y) part of the c constant in recursive equation
    double d_re;  // increment in the x-coords
    double d_im;  // increment in the y-coords

    uint8_t cid;  // chunk id
    uint8_t n_re; // number of cells in x-coords
    uint8_t n_im; // number of cells in y-coords
    uint8_t i_re; // x-coords in block
    uint8_t i_im; // y-coords in block
    uint8_t iter; // number of iterations // aka result
    uint8_t n;    // number of iterations per each pixel

    bool abort;

};

void set_cur_comp(message* msg, struct current_computation cur_comp);
uint8_t max_iter(uint8_t n, double z_re, double z_im, double c_re, double c_im);

void set_abort(struct current_computation cur_comp);
bool is_abort(struct current_computation cur_comp);

#endif