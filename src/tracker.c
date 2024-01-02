#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "tracker.h"
#include "debug.h"
#include "utils.h"

static void receive_file(struct file_t *file, int rank)
{
    MPI_Recv(&file->owner, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(file->name, MAX_FILENAME + 1, MPI_CHAR, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&file->nr_segments, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(file->segments, MAX_CHUNKS * (HASH_SIZE + 1), MPI_CHAR, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

static void get_files(struct swarm_t *s, int rank)
{
    int nr_files;
    MPI_Recv(&nr_files, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    s->capacity += nr_files;

    struct file_t *tmp = realloc(s->all_files, s->capacity * sizeof(*s->all_files));
    DIE(!tmp, "realloc() failed");
    s->all_files = tmp;

    for (int j = 0; j < nr_files; ++j)
        receive_file(&s->all_files[s->size++], rank);
}

static struct swarm_t *init(int numtasks)
{
    struct swarm_t *s = calloc(1, sizeof(*s));
    DIE(!s, "calloc() failed");

    for (int i = 1; i < numtasks; ++i)
        get_files(s, i);

    return s;
}

void tracker(int numtasks, int rank)
{
    struct swarm_t *s = init(numtasks); 
#ifdef DEBUG
    FILE *out = fopen("swarm.txt", "w");
    print_swarm(s, out);
#endif
}
