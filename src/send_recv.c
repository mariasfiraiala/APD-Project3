#include <mpi.h>
#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "send_recv.h"

void receive_meta(struct file_meta_t *meta, int rank, int tag)
{
    MPI_Recv(meta->name, MAX_FILENAME + 1, MPI_CHAR, rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&meta->size, 1, MPI_INT, rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void receive_segments(struct file_segments_t *segments, int rank, int tag)
{
    MPI_Recv(&segments->nr_segments, 1, MPI_INT, rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(segments->segments, MAX_CHUNKS, MPI_INT, rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void receive_full_file(struct full_file_t *file, int rank, int tag)
{
    receive_meta(&file->meta, rank, tag);
    MPI_Recv(&file->segments, MAX_CHUNKS * (HASH_SIZE + 1), MPI_CHAR, rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}

void receive_client(struct swarm_client_t *client, int rank, int tag)
{
    MPI_Recv(&client->rank, 1, MPI_INT, rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    receive_segments(&client->chunks, rank, tag);
}

void receive_swarm(struct swarm_t *swarm, int rank, int tag)
{
    receive_full_file(&swarm->file, rank, tag);
    MPI_Recv(&swarm->size, 1, MPI_INT, rank, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    for (int i = 0; i < swarm->size; ++i)
        receive_client(&swarm->clients[i], rank, tag);
}

void receive_sparse_file(struct sparse_file_t *file, int rank, int tag)
{
    receive_meta(&file->meta, rank, tag);
    receive_segments(&file->chunks, rank, tag);
}

void send_client(struct swarm_client_t *client, int rank, int tag)
{
    MPI_Send(&client->rank, 1, MPI_INT, rank, tag, MPI_COMM_WORLD);
    send_segments(&client->chunks, rank, tag);
}

void send_swarm(struct swarm_t *swarm, int rank, int tag)
{
    send_full_file(&swarm->file, rank, tag);
    MPI_Send(&swarm->size, 1, MPI_INT, rank, tag, MPI_COMM_WORLD);
    for (int i = 0; i < swarm->size; ++i)
        send_client(swarm->clients, rank, tag);
}

void send_meta(struct file_meta_t *meta, int rank, int tag)
{
    MPI_Send(meta->name, MAX_FILENAME + 1, MPI_CHAR, rank, tag, MPI_COMM_WORLD);
    MPI_Send(&meta->size, 1, MPI_INT, rank, tag, MPI_COMM_WORLD);
}

void send_segments(struct file_segments_t *segments, int rank, int tag)
{
    MPI_Send(&segments->nr_segments, 1, MPI_INT, rank, tag, MPI_COMM_WORLD);
    MPI_Send(segments->segments, MAX_CHUNKS, MPI_INT, rank, tag, MPI_COMM_WORLD);
}

void send_full_file(struct full_file_t *file, int rank, int tag)
{
    send_meta(&file->meta, rank, tag);
    MPI_Send(file->segments, MAX_CHUNKS * (HASH_SIZE + 1), MPI_CHAR, rank, tag, MPI_COMM_WORLD);
}

void send_sparse_file(struct sparse_file_t *file, int rank, int tag)
{
    send_meta(&file->meta, rank, tag);
    send_segments(&file->chunks, rank, tag);
}
