#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tracker.h"
#include "debug.h"
#include "send_recv.h"
#include "utils.h"

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
