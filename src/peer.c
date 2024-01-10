#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "utils.h"
#include "peer.h"
#include "send_recv.h"

/**
 * Download thread, interacts with both the tracker and other clients.
 *
 * @param arg
 *  Pointer to client structure
 * @return
 *  NULL, for correctly exiting the thread
 */
static void *download_thread_func(void *arg)
{
    struct client_t *c = (struct client_t *) arg;

    for (int i = 0; i < c->wanted_files; ++i) {
        MPI_Send(c->w_files[i].meta.name, MAX_FILENAME + 1, MPI_CHAR, TRACKER_RANK, TAG_SWARM, MPI_COMM_WORLD);

        struct swarm_t s;
        receive_swarm(&s, TRACKER_RANK, TAG_SWARM);

        request_missing_chunks(c, &s);
    }

    int finished = 0;
    MPI_Send(&finished, 1, MPI_INT, TRACKER_RANK, TAG_FINISH_CLIENT, MPI_COMM_WORLD);
    return NULL;
}

/**
 * Upload thread, interacts with other clients, and with the tracker
 * in order to receive the exit signal.
 *
 * @param arg
 *  Pointer to client structure
 * @return
 *  NULL, for correctly exiting the thread
 */
static void *upload_thread_func(void *arg)
{
    struct client_t *c = (struct client_t *) arg;

    char hash[HASH_SIZE + 1];
    int ack, stop;
    int running = 1;
    while (running) {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch (status.MPI_TAG) {
        case TAG_DOWNLOAD:
            MPI_Recv(hash, HASH_SIZE + 1, MPI_CHAR, status.MPI_SOURCE, TAG_DOWNLOAD, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Send(&ack, 1, MPI_INT, status.MPI_SOURCE, TAG_DOWNLOAD_ACK, MPI_COMM_WORLD);
            break;
        case TAG_STOP:
            MPI_Recv(&stop, 1, MPI_INT, TRACKER_RANK, TAG_STOP, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            running = 0;
        }
    }

    return NULL;
}

/**
 * Creates client with info from its input file and sends owned files
 * to tracker. 
 *
 * @param rank
 *  Process rank
 * @return
 *  Pointer to client structure, fully initialized and ready for exchange
 */
static struct client_t *init(int rank)
{
    struct client_t *c = read_file(rank);
    c->rank = rank;

    MPI_Send(&c->owned_files, 1, MPI_INT, TRACKER_RANK, TAG_INIT, MPI_COMM_WORLD);
    for (int i = 0; i < c->owned_files; ++i)
        send_full_file(&c->o_files[i], TRACKER_RANK, TAG_INIT);

    int ack;
    MPI_Recv(&ack, 1, MPI_INT, TRACKER_RANK, TAG_INIT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    return c;
}

void peer(int numtasks, int rank)
{
    pthread_t download_thread;
    pthread_t upload_thread;
    void *status;
    int r;

    struct client_t *c = init(rank);

    r = pthread_create(&download_thread, NULL, download_thread_func, (void *) c);
    DIE(r, "pthread_create() failed");

    r = pthread_create(&upload_thread, NULL, upload_thread_func, (void *) c);
    DIE(r, "pthread_create() failed");

    r = pthread_join(download_thread, &status);
    DIE(r, "pthread_join() failed");

    r = pthread_join(upload_thread, &status);
    DIE(r, "pthread_join() failed");
}
