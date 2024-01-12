Copyright 2024 Maria Sfiraiala (maria.sfiraiala@stud.acs.upb.ro)

# BitTorrent - Project3

## Description

The project aims to implement both the tracker and client from the BitTorrent protocol using MPI standards.
As opposed to the project documentation, the tracker doesn't store the client state whatsoever, as the only thing of interest when it comes to downloading chunks of the file is, actually, the clients owning them.

### Tracker

The tracker has an intialization step during which it's expecting the clients to send their owned files.
With every received file, it, immediately, either creates a swarm for it, or inserts the id of the client in the already existing swarm, together with the owned segments from the file.

```C
struct swarm_t {
    struct full_file_t file;
    int size;
    struct swarm_client_t clients[MAX_CLIENTS];
};
```

A swarm is made out of a full file (name, size and hashes) and a list of swarm clients.
Each swarm client is identified by its rank and the indeces of the owned segments.

After the tracker has gained full knowledge of the complete files in the network, it starts listening for swarm requests, update files or finish messages.
It handles every request based on the tag of the message, which is got before actually receiving the communication buffer by performing a `MPI_Probe()` call.

### Client

Similarly to the tracker, the client also goes through an initialization stage in which it reads the configuration file, populates its structure and sends the owned files to the rank 0 process.

```C
struct client_t {
	int rank;
	int owned_files;
	int wanted_files;
	struct full_file_t o_files[MAX_FILES];
	struct sparse_file_t w_files[MAX_FILES];
	int load[MAX_CLIENTS];
	int received_chunks;
};
```

After the intialization stage is completed, the client starts its download and upload threads.

The **download thread** iteratively goes through the wanted files, and piece by piece, starting with the least index, requests the chunk from the most unused client in the network.

The most unused client is determined by getting the minimum value from the `load` frequency array.
Of course, the client must first find the peers that own the missing chunk, so the `load` array is also filtered by this criterion.

When a file is fully downloaded or when the client acquaired a multiple of 10 chunks, it informs the tracker of the current file segment ownership.

The **upload thread** loops, awaiting for a segment request or a stop signal sent my the tracker to inform all the upload threads in the network to terminate, as there are no more peers to ask for files.
Acting on a certain message is implemented similarly to the tracker, in the sense that the seed probes for the tag, before actually receiving it.

> **Note**: The upload thread simply acks the request for segment, it doesn't perform any search whatsoever to confirm it actually owns that piece.

## Observations Regarding the Project

The project was nice, I just whish we had some sort of debugging an MPI program knowledge before starting it (a tutorial or lab).
I hate printing stuff out, and getting `gdb` to work with MPI was painful.
What's more, my `mpirun` (version 4.1.6) decided that printing errors when exiting is not entirely mandatory, so I had to guess what went wrong (or ask a friend to run the program with their version of `mpirun`).
