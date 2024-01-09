#ifndef SEND_RECV_H
#define SEND_RECV_H

void receive_meta(struct file_meta_t *meta, int rank, int tag);

void receive_segments(struct file_segments_t *segments, int rank, int tag);

void receive_full_file(struct full_file_t *file, int rank, int tag);

void receive_client(struct swarm_client_t *client, int rank, int tag);

void receive_swarm(struct swarm_t *swarm, int rank, int tag);

void receive_sparse_file(struct sparse_file_t *file, int rank, int tag);

void send_client(struct swarm_client_t *client, int rank, int tag);

void send_swarm(struct swarm_t *swarm, int rank, int tag);

void send_meta(struct file_meta_t *meta, int rank, int tag);

void send_segments(struct file_segments_t *segments, int rank, int tag);

void send_full_file(struct full_file_t *file, int rank, int tag);

void send_sparse_file(struct sparse_file_t *file, int rank, int tag);

#endif
