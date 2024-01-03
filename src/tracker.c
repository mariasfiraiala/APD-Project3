#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tracker.h"
#include "debug.h"
#include "utils.h"

static void receive_meta(struct file_meta_t *meta, int rank)
{
    MPI_Recv(meta->name, MAX_FILENAME + 1, MPI_CHAR, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&meta->size, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

static void receive_segments(struct file_segments_t *segments, int rank)
{
    MPI_Recv(&segments->nr_segments, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(segments->segments, MAX_CHUNKS * (HASH_SIZE + 1), MPI_CHAR, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

static void receive_file(struct file_t *file, int rank)
{
    receive_meta(&file->meta, rank);
    receive_segments(&file->segments, rank);
}

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

static int eq_swarm(struct swarm_t *s, struct file_meta_t *f)
{
    return (strcmp(s->file_meta.name, f->name) == 0 ? 1 : 0);
}

static int find_swarm(struct tracker_t *t, struct file_meta_t *f)
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
    int found_swarm = find_swarm(t, &f->meta);

    if (found_swarm != -1)
        insert_swarm(&t->swarms[found_swarm], f, rank);
    else
        create_swarm(&t->swarms[t->size++], f, rank);
}

static void get_files(struct tracker_t *t, int rank)
{
    int nr_files;
    MPI_Recv(&nr_files, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (int j = 0; j < nr_files; ++j) {
        struct file_t file;
        receive_file(&file, rank);
        insert_tracker(t, &file, rank);
    }
}

static struct tracker_t *init(int numtasks)
{
    struct tracker_t *s = calloc(1, sizeof(*s));
    DIE(!s, "calloc() failed");

    for (int i = 1; i < numtasks; ++i)
        get_files(s, i);

    for (int i = 1; i < numtasks; ++i) {
        int ack = 1;
        MPI_Send(&ack, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
    }

    return s;
}

void tracker(int numtasks, int rank)
{
    struct tracker_t *s = init(numtasks);
#ifdef DEBUG
    FILE *out = fopen("tracker.txt", "w");
    print_tracker(s, out);
#endif
}
