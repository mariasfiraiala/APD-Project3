#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "utils.h"

void print_file_meta(struct file_meta_t *m, FILE *out)
{
    fprintf(out, "File name:%s file size:%d\n", m->name, m->size);
}

void print_file_segments(struct file_segments_t *s, FILE *out)
{
    fprintf(out, "Number of owned segments %d\n", s->nr_segments);
    for (int i = 0; i < MAX_CHUNKS; ++i)
        if (strcmp(s->segments[i], ""))
            fprintf(out, "%s\n", s->segments[i]);
}

void print_swarm_client(struct swarm_client_t *client, FILE *out)
{
    fprintf(out, "Rank: %d\n", client->rank);
    print_file_segments(&client->segments, out);
}

void print_swarm(struct swarm_t *swarm, FILE *out)
{
    print_file_meta(&swarm->file_meta, out);
    fprintf(out, "Number of clients: %d\n", swarm->size);
    for (int i = 0; i < swarm->size; ++i)
        print_swarm_client(&swarm->clients[i], out);
}

void print_tracker(struct tracker_t *t, FILE *out)
{
    fprintf(out, "Number of swarms: %d\n", t->size);
    for (int i = 0; i < t->size; ++i)
        print_swarm(&t->swarms[i], out);
}
