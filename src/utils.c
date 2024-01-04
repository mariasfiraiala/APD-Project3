#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "send_recv.h"

void insert_client(struct swarm_client_t *c, struct file_segments_t *s)
{
    for (int i = 0; i < MAX_CHUNKS; ++i)
        if (!strcmp(c->segments.segments[i], "") && strcmp(s->segments[i], "")) {
            strcpy(c->segments.segments[i], s->segments[i]);
            ++c->segments.nr_segments;
        }
}

int find_client(struct swarm_t *s, int rank)
{
    for (int i = 0; i < s->size; ++i)
        if (s->clients[i].rank == rank)
            return i;

    return -1;
}

void create_client(struct swarm_client_t *c, struct file_t *f, int rank)
{
    c->rank = rank;
    c->segments = f->segments;
}

void insert_swarm(struct swarm_t *s, struct file_t *f, int rank)
{
    int found_client = find_client(s, rank);

    if (found_client != -1)
        insert_client(&s->clients[found_client], &f->segments);
    else {
        create_client(&s->clients[s->size++], f, rank);
    }
}

int eq_swarm(struct swarm_t *s, char *f)
{
    return (strcmp(s->file_meta.name, f) == 0 ? 1 : 0);
}

int find_swarm(struct tracker_t *t, char *f)
{
    for (int i = 0; i < t->size; ++i)
        if (eq_swarm(&t->swarms[i], f))
            return i;

    return -1;
}

void create_swarm(struct swarm_t *s, struct file_t *f, int rank)
{
    s->file_meta = f->meta;
    s->clients[s->size].rank = rank;
    s->clients[s->size].segments = f->segments;
    ++s->size;
}

void insert_tracker(struct tracker_t *t, struct file_t *f, int rank)
{
    int found_swarm = find_swarm(t, f->meta.name);

    if (found_swarm != -1)
        insert_swarm(&t->swarms[found_swarm], f, rank);
    else
        create_swarm(&t->swarms[t->size++], f, rank);
}

void get_init_files(struct tracker_t *t, int rank)
{
    int owned_files;
    MPI_Recv(&owned_files, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (int j = 0; j < owned_files; ++j) {
        struct file_t file;
        receive_file(&file, rank);
        insert_tracker(t, &file, rank);
    }
}

void get_send_request_files(struct tracker_t *t, int rank)
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

int empty_string(char *s)
{
    return (strcmp(s, "") == 0 ? 1 : 0);
}

int get_missing_chunks(struct client_t *c, struct file_meta_t *f, int *chunks)
{
    int ch = 0;
    for (int i = 0; i < c->owned_files; ++i) {
        if (!strcmp(c->o_files[i].meta.name, f->name)) {
            for (int j = 0; j < f->size; ++j)
                if (empty_string(c->o_files[i].segments.segments[j]))
                    chunks[ch++] = j;

            return ch;
        }
    }

    for (ch = 0; ch < f->size; ++ch)
        chunks[ch] = ch;

    return ch;
}

int get_chunks_client(struct swarm_t *s, int index, int *clients)
{
    int cl = 0;
    for (int i = 0; i < s->size; ++i)
        if (!empty_string(s->clients[i].segments.segments[index]))
            clients[cl++] = s->clients[i].rank;

    return cl;
}

int get_best_peer(int *load, int *clients, int cl)
{
    int min_client = clients[0];
    int min = load[min_client];

    for (int i = 1; i < cl; ++i)
        if (load[clients[i]] < min) {
            min_client = clients[i];
            min = load[min_client];
        }

    return min_client;
}

char *get_hash(struct swarm_t *s, int rank, int index)
{
    int cl = find_client(s, rank);

    char *hash = malloc(HASH_SIZE + 1);
    memcpy(hash, s->clients[cl].segments.segments[index], HASH_SIZE + 1);

    return hash;
}

void request_missing_chunks(struct client_t *c, struct swarm_t *s)
{
    int *chunks = malloc(MAX_CHUNKS * sizeof(*chunks));
    DIE(!chunks, "malloc() failed");
    int *clients = malloc(MAX_CLIENTS * sizeof(*clients));
    DIE(!clients, "malloc() failed");

    int ch = get_missing_chunks(c, &s->file_meta, chunks);
    for (int i = 0; i < ch; ++i) {
        int cl = get_chunks_client(s, chunks[i], clients);
        int peer = get_best_peer(c->load, clients, cl);
        char *hash = get_hash(s, peer, chunks[i]);

        MPI_Send(hash, HASH_SIZE + 1, MPI_CHAR, peer, 1, MPI_COMM_WORLD);
        ++c->load[peer];

        int ack;
        MPI_Recv(&ack, 1, MPI_INT, peer, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}
