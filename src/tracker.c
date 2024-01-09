#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tracker.h"
#include "utils.h"
#include "send_recv.h"

static struct tracker_t *init(int numtasks)
{
    struct tracker_t *t = calloc(1, sizeof(*t));
    DIE(!t, "calloc() failed");

    for (int i = 1; i < numtasks; ++i)
        receive_init_files(t, i);

    for (int i = 1; i < numtasks; ++i) {
        int ack = 1;
        MPI_Send(&ack, 1, MPI_INT, i, TAG_INIT, MPI_COMM_WORLD);
    }

    return t;
}

void tracker(int numtasks, int rank)
{
    struct tracker_t *t = init(numtasks);

    int finished;
    int clients = numtasks - 1;
    while (clients) {
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch (status.MPI_TAG) {
        case TAG_SWARM:
            receive_swarm_request(t);
            break;

        case TAG_UPDATE:
            receive_update_file(t, status.MPI_SOURCE);
            break;

        case TAG_FINISH_FILE:
            MPI_Recv(&finished, 1, MPI_INT, MPI_ANY_SOURCE, TAG_FINISH_FILE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            break;

        case TAG_FINISH_CLIENT:
            MPI_Recv(&finished, 1, MPI_INT, MPI_ANY_SOURCE, TAG_FINISH_CLIENT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            --clients;
            break;
        }
    }
    for (int i = 1; i < numtasks; ++i) {
        int stop = 1;
        MPI_Send(&stop, 1, MPI_INT, i, TAG_STOP, MPI_COMM_WORLD);
    }
}
