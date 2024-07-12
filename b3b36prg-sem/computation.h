#ifndef __COMPUTATION__
#define __COMPUTATION__

#include <stdbool.h>
#include <stdio.h>

#include"messages.h"

void computation_init(void);
void computation_cleanup(void);

void get_grid_size(int *w, int *h);
bool is_computing(void);
bool is_done(void);
bool is_abort(void);

void enable_comp(void);
void abort_comp(void);

bool set_compute(message *msg);
bool compute(message *msg);

void update_image(int w, int h, unsigned char *img);
void update_data(const msg_compute_data *compute_data );

void params_print();
void process_params();

#endif