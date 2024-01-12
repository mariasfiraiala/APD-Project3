#include <mpi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "send_recv.h"

/**
 * Finds swarm based on file name.
 *
 * @param t
 *	Tracker to search swarm in
 * @param name
 *	File name for which we retrieve swarm
 * @return
 *	- (>=0): Index of wanted swarm
 *	- (-1): Swarm not found
 */
static int s_find_name(struct tracker_t *t, char *name)
{
	for (int i = 0; i < t->nr_swarms; ++i)
		if (!strcmp(t->swarms[i].file.meta.name, name))
			return i;

	return -1;
}

/**
 * Finds client in swarm based on rank.
 *
 * @param t
 *	Tracker to search client in
 * @param rank
 *	Rank for which we retrieve client
 * @return
 *	- (>=0): Index of wanted client
 *	- (-1): Client not found
 */
static int c_find_rank(struct swarm_t *s, int rank)
{
	for (int i = 0; i < s->size; ++i)
		if (s->clients[i].rank == rank)
			return i;

	return -1;
}

/**
 * Inserts full file for client.
 *
 * @param s
 *	Swarm to populate with full file
 * @param f
 *	File to populate swarm with
 * @param rank
 *	Client to mark as owning the full file
 * @return
 *	No return
 */
static void s_insert_full_file(struct swarm_t *s, struct full_file_t *f, int rank)
{
	s->clients[s->size].rank = rank;
	s->clients[s->size].chunks.nr_segments = f->meta.size;
	for (int i = 0; i < f->meta.size; ++i)
		s->clients[s->size].chunks.segments[i] = 1;

	++s->size;
}

/**
 * Inserts sparse file for client.
 *
 * @param s
 *	Swarm to populate with sparse file
 * @param f
 *	File to populate swarm with
 * @param rank
 *	Client to mark as owning the sparse file
 * @return
 *	No return
 */
static void s_insert_sparse_file(struct swarm_t *s, struct sparse_file_t *f, int rank)
{
	int found_client = c_find_rank(s, rank);

	if (found_client != -1)
		s->clients[found_client].chunks = f->chunks;
	else {
		s->clients[s->size].rank = rank;
		s->clients[s->size].chunks = f->chunks;
		++s->size;
	}
}

/**
 * Creates full file for client.
 *
 * @param s
 *	Swarm to populate with full file
 * @param f
 *	File to populate swarm with
 * @param rank
 *	Client to mark as owning the sparse file
 * @return
 *	No return
 */
static void s_create_full_file(struct swarm_t *s, struct full_file_t *f, int rank)
{
	s->file = *f;
	
	s_insert_full_file(s, f, rank);
}

/**
 * Inserts full file in tracker.
 *
 * @param t
 *	Tracker to insert full file in
 * @param f
 *	File to be inserted
 * @param rank
 *	Client to mark as owning the full file
 * @return
 *	No return
 */
static void t_insert_full_file(struct tracker_t *t, struct full_file_t *f, int rank)
{
	int found_swarm = s_find_name(t, f->meta.name);

	if (found_swarm != -1)
		s_insert_full_file(&t->swarms[found_swarm], f, rank);
	else
		s_create_full_file(&t->swarms[t->nr_swarms++], f, rank);
}

/**
 * Inserts sparse file in tracker.
 *
 * @param t
 *	Tracker to insert sparse file in
 * @param f
 *	File to be inserted
 * @param rank
 *	Client to mark as owning the sparse file
 * @return
 *	Swarm in which the file was inserted
 */
static int t_insert_sparse_file(struct tracker_t *t, struct sparse_file_t *f, int rank)
{
	int found_swarm = s_find_name(t, f->meta.name);

	s_insert_sparse_file(&t->swarms[found_swarm], f, rank);

	return found_swarm;
}

/**
 * Finds the indeces of the missing chunks based on file.
 *
 * @param c
 *	Client to search chunks in
 * @param f
 *	File to be searched after
 * @param chunks
 *	Array in which the indeces are saved
 * @return
 *	Number of missing chunks
 */
static int get_missing_chunks(struct client_t *c, struct full_file_t *f, int *chunks)
{
	int ch = 0;
	for (int i = 0; i < c->wanted_files; ++i)
		if (!strcmp(c->w_files[i].meta.name, f->meta.name)) {
			for (int j = 0; j < f->meta.size; ++j)
				if (!c->w_files[i].chunks.segments[j])
					chunks[ch++] = j;
			return ch;
		}

	for (ch = 0; ch < f->meta.size; ++ch)
		chunks[ch] = ch;

	return ch;
}

/**
 * Finds the clients owning a certain chunk from a swarm.
 *
 * @param s
 *	Swarm to search chunks in
 * @param chunk
 *	Chunk to search for
 * @param chunks
 *	Array in which the clients owning the chunk are saved
 * @return
 *	Number of owning clients
 */
static int get_chunks_client(struct swarm_t *s, int chunk, int *clients)
{
	int cl = 0;
	for (int i = 0; i < s->size; ++i)
		if (s->clients[i].chunks.segments[chunk])
			clients[cl++] = s->clients[i].rank;

	return cl;
}

/**
 * Finds the most unused peer to download file from.
 *
 * @param load
 *	Array of clients loads
 * @param f
 *	Array of clients
 * @param cl
 *	Number of clients
 * @return
 *	Rank of best peer to download from
 */
static int get_best_peer(int *load, int *clients, int cl)
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

/**
 * Marks chunk as owned.
 *
 * @param c
 *	Client to mark chunk in
 * @param f
 *	File to which the chunk belongs to
 * @param chunk
 *	Index of now owned chunk
 * @return
 *  No return
 */
static void update_missing_chunk(struct client_t *c, struct full_file_t *f, int chunk)
{
	for (int i = 0; i < c->wanted_files; ++i)
		if (!strcmp(c->w_files[i].meta.name, f->meta.name)) {
			++c->w_files[i].chunks.nr_segments;
			c->w_files[i].chunks.segments[chunk] = 1;
			return;
		}
}

/**
 * Sends newly updated information regarding owned segments.
 *
 * @param c
 *	Client to send chunks from
 * @param f
 *	File to be searched after
 * @return
 *  No return
 */
static void send_update_file(struct client_t *c, struct file_meta_t *f)
{
	for (int i = 0; i < c->wanted_files; ++i)
		if (!strcmp(c->w_files[i].meta.name, f->name)) {
			send_sparse_file(&c->w_files[i], TRACKER_RANK, TAG_UPDATE);
			return;
		}
}

/**
 * Marks the file as fully downloaded and prints its segments.
 *
 * @param c
 *	Client for which the file is fully downloaded
 * @param f
 *	File that is now completed
 * @param chunks
 *	Array in which the indeces are saved
 * @return
 *	No return
 */
static void finished_file(struct client_t *c, struct full_file_t *f)
{
	char file[2 * MAX_FILENAME + 1];
	snprintf(file, 2 * MAX_FILENAME + 1, "client%d_%s", c->rank, f->meta.name);
	FILE *out = fopen(file, "w");

	for (int i = 0; i < f->meta.size; ++i)
		fprintf(out, "%s\n", f->segments[i]);

	fclose(out);

	int finished = 0;
	MPI_Send(&finished, 1, MPI_INT, TRACKER_RANK, TAG_FINISH_FILE, MPI_COMM_WORLD);
}

void receive_init_files(struct tracker_t *t, int rank)
{
	int nr_files;
	MPI_Recv(&nr_files, 1, MPI_INT, rank, TAG_INIT, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	for (int j = 0; j < nr_files; ++j) {
		struct full_file_t f;
		receive_full_file(&f, rank, TAG_INIT);
		t_insert_full_file(t, &f, rank);
	}
}

void receive_swarm_request(struct tracker_t *t, int rank)
{
	char file[MAX_FILENAME + 1];
	MPI_Recv(file, MAX_FILENAME + 1, MPI_CHAR, rank, TAG_SWARM, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	int found_swarm = s_find_name(t, file);
	send_swarm(&t->swarms[found_swarm], rank, TAG_SWARM);
}

void receive_update_file(struct tracker_t *t, int rank)
{
	struct sparse_file_t file;
	receive_sparse_file(&file, rank, TAG_UPDATE);

	int found_swarm = t_insert_sparse_file(t, &file, rank);
	send_swarm(&t->swarms[found_swarm], rank, TAG_UPDATE);
}

void request_missing_chunks(struct client_t *c, struct swarm_t *s)
{
	int chunks[MAX_CHUNKS];
	int clients[MAX_CLIENTS];

	int ch = get_missing_chunks(c, &s->file, chunks);
	for (int i = 0; i < ch; ++i) {
		int cl = get_chunks_client(s, chunks[i], clients);
		int peer = get_best_peer(c->load, clients, cl);

		char *hash = s->file.segments[chunks[i]];
		MPI_Send(hash, HASH_SIZE + 1, MPI_CHAR, peer, TAG_DOWNLOAD, MPI_COMM_WORLD);
		++c->load[peer];

		int ack;
		MPI_Recv(&ack, 1, MPI_INT, peer, TAG_DOWNLOAD_ACK, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		update_missing_chunk(c, &s->file, chunks[i]);
		
		c->received_chunks = (c->received_chunks + 1) % MOD;
		if (!c->received_chunks || i == ch - 1) {
			send_update_file(c, &s->file.meta);
			receive_swarm(s, TRACKER_RANK, TAG_UPDATE);
		}
	}
	finished_file(c, &s->file);
}
