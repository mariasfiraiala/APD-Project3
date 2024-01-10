#ifndef TRACKER_H
#define TRACKER_H

/**
 * Keeps tracker alive and well functioning
 *
 * @param numtasks
 *	Number of process in communicator
 * @param rank
 *  Rank of current process
 * @return
 *	No return value
 */
void tracker(int numtasks, int rank);

#endif /* TRACKER_H */
