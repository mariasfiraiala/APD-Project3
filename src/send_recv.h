#ifndef SEND_RECV_H
#define SEND_RECV_H

/**
 * Receives `file_meta_t` structure.
 * @param meta
 *	Structure to receive info into
 * @param rank
 *  Rank to receive from
 * @param tag
 *  Message tag
 * @return
 *	No return
 */
void receive_meta(struct file_meta_t *meta, int rank, int tag);

/**
 * Receives `file_segments_t` structure.
 * @param segments
 *	Structure to receive info into
 * @param rank
 *  Rank to receive from
 * @param tag
 *  Message tag
 * @return
 *	No return
 */
void receive_segments(struct file_segments_t *segments, int rank, int tag);

/**
 * Receives `full_file_t` structure.
 * @param file
 *	Structure to receive info into
 * @param rank
 *  Rank to receive from
 * @param tag
 *  Message tag
 * @return
 *	No return
 */
void receive_full_file(struct full_file_t *file, int rank, int tag);

/**
 * Receives `swarm_client_t` structure.
 * @param client
 *	Structure to receive info into
 * @param rank
 *  Rank to receive from
 * @param tag
 *  Message tag
 * @return
 *	No return
 */
void receive_client(struct swarm_client_t *client, int rank, int tag);

/**
 * Receives `swarm_t` structure.
 * @param swarm
 *	Structure to receive info into
 * @param rank
 *  Rank to receive from
 * @param tag
 *  Message tag
 * @return
 *	No return
 */
void receive_swarm(struct swarm_t *swarm, int rank, int tag);

/**
 * Receives `sparse_file_t` structure.
 * @param file
 *	Structure to receive info into
 * @param rank
 *  Rank to receive from
 * @param tag
 *  Message tag
 * @return
 *	No return
 */
void receive_sparse_file(struct sparse_file_t *file, int rank, int tag);

/**
 * Sends `swarm_client_t` structure.
 * @param client
 *	Structure to send info from
 * @param rank
 *  Rank to send to
 * @param tag
 *  Message tag
 * @return
 *	No return
 */
void send_client(struct swarm_client_t *client, int rank, int tag);

/**
 * Sends `swarm_t` structure.
 * @param swarm
 *	Structure to send info from
 * @param rank
 *  Rank to send to
 * @param tag
 *  Message tag
 * @return
 *	No return
 */
void send_swarm(struct swarm_t *swarm, int rank, int tag);

/**
 * Sends `file_meta_t` structure.
 * @param meta
 *	Structure to send info from
 * @param rank
 *  Rank to send to
 * @param tag
 *  Message tag
 * @return
 *	No return
 */
void send_meta(struct file_meta_t *meta, int rank, int tag);

/**
 * Sends `file_segments_t` structure.
 * @param segments
 *	Structure to send info from
 * @param rank
 *  Rank to send to
 * @param tag
 *  Message tag
 * @return
 *	No return
 */
void send_segments(struct file_segments_t *segments, int rank, int tag);

/**
 * Sends `full_file_t` structure.
 * @param file
 *	Structure to send info from
 * @param rank
 *  Rank to send to
 * @param tag
 *  Message tag
 * @return
 *	No return
 */
void send_full_file(struct full_file_t *file, int rank, int tag);

/**
 * Sends `sparse_file_t` structure.
 * @param file
 *	Structure to send info from
 * @param rank
 *  Rank to send to
 * @param tag
 *  Message tag
 * @return
 *	No return
 */
void send_sparse_file(struct sparse_file_t *file, int rank, int tag);

#endif /* SEND_RECV_H */
