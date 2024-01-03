#ifndef SEND_RECV_H
#define SEND_RECV_H

void receive_meta(struct file_meta_t *meta, int rank);

void receive_segments(struct file_segments_t *segments, int rank);

void receive_file(struct file_t *file, int rank);

void receive_client(struct swarm_client_t *client, int rank);

void receive_swarm(struct swarm_t *swarm, int rank);

void send_client(struct swarm_client_t *client, int rank);

void send_swarm(struct swarm_t *swarm, int rank);

void send_meta(struct file_meta_t *meta, int rank);

void send_segments(struct file_segments_t *segments, int rank);

void send_file(struct file_t *file, int rank);

#endif
