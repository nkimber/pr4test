#include <pthread.h>  // give me some Mutex magic


//In case you want to implement the shared memory IPC as a library...

// All of the request info must go in here.
// We send this as a package to handle_with_cache and then transmit the whole thing on the queue to simplecached
typedef struct cacherequest_info {
    char buffer[4096];
    int segmentsize;
    char server[1024];
    int ThreadNum;
    int shmid;        // this is the system id identifying the Shared Memory segment to be used in this request
    int semreadid;        // this is the system id identfiying the Semaphore set being used so yu can read the shm
    int semwriteid;        // this is the system id identfiying the Semaphore set being used for notifying you can write to the shm
    int semnumber;    // this is the number of the semaphore in the set that is being used.
    char filename[1024];   // useful for debug purposes
} info_t;


// This is included in both webproxy and simplecached so it's the ideal place to share implementation
typedef struct cache_msgbuf {
    long mtype;  /* must be positive and must be first item in struct*/
    info_t info; /* this is the main message */
} cache_msgbuf_t;

// This is a node in a linked list of Available ShmSegments
typedef struct shmnode {
    int nodenum;    // the index into the global list of Shm Segments - useful to use as an index into the semaphore set, this is dedicated to the shm segment below - if you want to access the shm segment then you need to use this semaphore
    int shmid;     // shm segment
    int semreadid;      // semaphore system id - this is a set of semaphores, see nodenum above to work out which semaphore needs to be used to read this shm segment
    int semwriteid;      // semaphore system id - this is a set of semaphores, see nodenum above to work out which semaphore needs to be used to write to this shm segment
    int semqueueid;    // sem system id - allows for lock on the shared msg queue.
    struct shmnode * nextnode;
} shmnode_t;

typedef struct filechunkinfo {
    int chunknum;    // starts with 1 and carries on with each chunk
    int bytesremaining;    // are there any further bytes remaining, and if so, how many?
    int buffersize;    // states how much data is in the shm following this struct
    int ErrorCode;      // 200- success, 400- File Not Found
} filechunkinfo_t;


extern char * shmkeypathname;    // This is a pathname used to generate a key for SHM
extern char * semreadkeypathname;     // pathname used to create key for read semaphores
extern char * semwritekeypathname;     // pathname used to create key for write semaphores
extern char * semmsgqueuepathname;     // pathname used to create key for write semaphores
extern char * msgqidpathname;

extern shmnode_t ** listshmnodes;    // array of SHM node info
extern int msqid;    // This is a unique id for accessing our message queue.
extern int NumSegments;    // Number of segments
extern shmnode_t * LinkedListAvailableShmNodes;    // Global list of shmnodes. When one is in use it will be removed from this list. When it is freed up it will be placed back on the list.
extern pthread_mutex_t LockAvailableShmNodes;   // mutex lock to prevent race conditions on queue of available shm segments

extern int debugmode;

char * getfilename (char * filepath);
int initsem(key_t key, int nsems);
void debugprint(char * message, ...);