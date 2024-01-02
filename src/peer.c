#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

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

void peer(int numtasks, int rank)
{
    pthread_t download_thread;
    pthread_t upload_thread;
    void *status;
    int r;

    struct client_t *clnt = read_file(rank);

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
