#ifndef PEER_H
#define PEER_H

/**
 * Keeps client alive and well functioning
 *
 * @param numtasks
 *	Number of process in communicator
 * @param rank
 *  Rank of current process
 * @return
 *	No return value
 */
void peer(int numtasks, int rank);

#endif /* PEER_H */
