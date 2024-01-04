#ifndef DEBUG_H
#define DEBUG_H

#include "utils.h"

void print_file_meta(struct file_meta_t *m, FILE *out);

void print_file_segments(struct file_segments_t *s, FILE *out);

void print_swarm_client(struct swarm_client_t *c, FILE *out);

void print_swarm(struct swarm_t *s, FILE *out);

void print_tracker(struct tracker_t *s, FILE *out);

#endif