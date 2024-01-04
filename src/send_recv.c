#include <mpi.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "send_recv.h"

void receive_meta(struct file_meta_t *meta, int rank)
{
    MPI_Recv(meta->name, MAX_FILENAME + 1, MPI_CHAR, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&meta->size, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void receive_segments(struct file_segments_t *segments, int rank)
{
    MPI_Recv(&segments->nr_segments, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(segments->segments, MAX_CHUNKS * (HASH_SIZE + 1), MPI_CHAR, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void receive_file(struct file_t *file, int rank)
{
    receive_meta(&file->meta, rank);
    receive_segments(&file->segments, rank);
}

void receive_client(struct swarm_client_t *client, int rank)
{
    MPI_Recv(&client->rank, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    receive_segments(&client->segments, rank);
}

void receive_swarm(struct swarm_t *swarm, int rank)
{
    receive_meta(&swarm->file_meta, rank);
    MPI_Recv(&swarm->size, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for (int i = 0; i < swarm->size; ++i)
        receive_client(&swarm->clients[i], rank);
}

void send_client(struct swarm_client_t *client, int rank)
{
    MPI_Send(&client->rank, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
    send_segments(&client->segments, rank);
}

void send_swarm(struct swarm_t *swarm, int rank)
{
    send_meta(&swarm->file_meta, rank);
    MPI_Send(&swarm->size, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
    for (int i = 0; i < swarm->size; ++i)
        send_client(swarm->clients, rank);
}

void send_meta(struct file_meta_t *meta, int rank)
{
    MPI_Send(meta->name, MAX_FILENAME + 1, MPI_CHAR, rank, 0, MPI_COMM_WORLD);
    MPI_Send(&meta->size, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
}

void send_segments(struct file_segments_t *segments, int rank)
{
    MPI_Send(&segments->nr_segments, 1, MPI_INT, rank, 0, MPI_COMM_WORLD);
    MPI_Send(segments->segments, MAX_CHUNKS * (HASH_SIZE + 1), MPI_CHAR, rank, 0, MPI_COMM_WORLD);
}

void send_file(struct file_t *file, int rank)
{
    send_meta(&file->meta, rank);
    send_segments(&file->segments, rank);
}
