#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tracker.h"
#include "debug.h"
#include "send-recv.h"
#include "utils.h"

static void insert_client(struct swarm_client_t *c, struct file_segments_t *s)
{
    for (int i = 0; i < MAX_CHUNKS; ++i)
        if (!strcmp(c->segments.segments[i], "") && strcmp(s->segments[i], "")) {
            strcpy(c->segments.segments[i], s->segments[i]);
            ++c->segments.nr_segments;
        }
}

static int find_client(struct swarm_t *s, int rank)
{
    for (int i = 0; i < s->size; ++i)
        if (s->clients[i].rank == rank)
            return i;

    return -1;
}

static void create_client(struct swarm_client_t *c, struct file_t *f, int rank)
{
    c->rank = rank;
    c->segments = f->segments;
}

static void insert_swarm(struct swarm_t *s, struct file_t *f, int rank)
{
    int found_client = find_client(s, rank);

    if (found_client != -1)
        insert_client(&s->clients[found_client], &f->segments);
    else {
        create_client(&s->clients[s->size++], f, rank);
    }
}

static int eq_swarm(struct swarm_t *s, char *f)
{
    return (strcmp(s->file_meta.name, f) == 0 ? 1 : 0);
}

static int find_swarm(struct tracker_t *t, char *f)
{
    for (int i = 0; i < t->size; ++i)
        if (eq_swarm(&t->swarms[i], f))
            return i;

    return -1;
}

static void create_swarm(struct swarm_t *s, struct file_t *f, int rank)
{
    s->file_meta = f->meta;
    s->clients[s->size].rank = rank;
    s->clients[s->size].segments = f->segments;
    ++s->size;
}

static void insert_tracker(struct tracker_t *t, struct file_t *f, int rank)
{
    int found_swarm = find_swarm(t, f->meta.name);

    if (found_swarm != -1)
        insert_swarm(&t->swarms[found_swarm], f, rank);
    else
        create_swarm(&t->swarms[t->size++], f, rank);
}

static void get_init_files(struct tracker_t *t, int rank)
{
    int owned_files;
    MPI_Recv(&owned_files, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (int j = 0; j < owned_files; ++j) {
        struct file_t file;
        receive_file(&file, rank);
        insert_tracker(t, &file, rank);
    }
}

static void get_send_request_files(struct tracker_t *t, int rank)
{
    int wanted_files;
    MPI_Recv(&wanted_files, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (int i = 0; i < wanted_files; ++i) {
        char w_file[MAX_FILENAME + 1];
        MPI_Recv(w_file, MAX_FILENAME + 1, MPI_CHAR, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int found_swarm = find_swarm(t, w_file);
        send_swarm(&t->swarms[found_swarm], rank);
    }
}

static struct tracker_t *init(int numtasks)
{
    struct tracker_t *t = calloc(1, sizeof(*t));
    DIE(!t, "calloc() failed");

    for (int i = 1; i < numtasks; ++i)
        get_init_files(t, i);

    for (int i = 1; i < numtasks; ++i) {
        int ack = 1;
        MPI_Send(&ack, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }

    return t;
}

void tracker(int numtasks, int rank)
{
    struct tracker_t *t = init(numtasks);

#ifdef DEBUG
    FILE *out = fopen("tracker.txt", "w");
    print_tracker(t, out);
#endif

    for (int i = 1; i < numtasks; ++i)
        get_send_request_files(t, i);
}
