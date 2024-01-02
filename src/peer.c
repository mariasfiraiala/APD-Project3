#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "utils.h"
#include "peer.h"

static void *download_thread_func(void *arg)
{
    int rank = *(int*) arg;

    return NULL;
}

static void *upload_thread_func(void *arg)
{
    int rank = *(int*) arg;

    return NULL;
}

static void send_file(struct file_t *file)
{
    MPI_Send(&file->owner, 1, MPI_INT, TRACKER_RANK, 0, MPI_COMM_WORLD);
    MPI_Send(file->name, MAX_FILENAME + 1, MPI_CHAR, TRACKER_RANK, 0, MPI_COMM_WORLD);
    MPI_Send(&file->nr_segments, 1, MPI_INT, TRACKER_RANK, 0, MPI_COMM_WORLD);
    MPI_Send(file->segments, MAX_CHUNKS * (HASH_SIZE + 1), MPI_CHAR, TRACKER_RANK, 0, MPI_COMM_WORLD);
}

struct client_t *init(int rank)
{
    struct client_t *c = read_file(rank);

    MPI_Send(&c->owned_files, 1, MPI_INT, TRACKER_RANK, 0, MPI_COMM_WORLD);
    for (int i = 0; i < c->owned_files; ++i) {
        send_file(&c->o_files[i]);
    }

    return c;
}

void peer(int numtasks, int rank)
{
    pthread_t download_thread;
    pthread_t upload_thread;
    void *status;
    int r;

    struct client_t *c = init(rank);

    r = pthread_create(&download_thread, NULL, download_thread_func, (void *) &rank);
    if (r) {
        printf("Eroare la crearea thread-ului de download\n");
        exit(-1);
    }

    r = pthread_create(&upload_thread, NULL, upload_thread_func, (void *) &rank);
    if (r) {
        printf("Eroare la crearea thread-ului de upload\n");
        exit(-1);
    }

    r = pthread_join(download_thread, &status);
    if (r) {
        printf("Eroare la asteptarea thread-ului de download\n");
        exit(-1);
    }

    r = pthread_join(upload_thread, &status);
    if (r) {
        printf("Eroare la asteptarea thread-ului de upload\n");
        exit(-1);
    }
}
