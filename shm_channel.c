//In case you want to implement the shared memory IPC as a library...
#include <stdio.h>
#include <stdarg.h>   // Needed for my DebugPrint()

#include <string.h>    // Needed for strlen
#include <stdlib.h>    // malloc etc...

#include "shm_channel.h"

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define MAX_RETRIES 10

int debugmode=1;   // this will be set from cmdline and 1 will mean we are turned on

#ifndef MSGQIDPATHNAME_DEFINED
#define MSGQIDPATHNAME_DEFINED

// This is a pathname that points to a file that is used to seed the msgqid
char * msgqidpathname = "/bin/ls";
char * shmkeypathname = "/bin/mv";    // This is a pathname used to generate a key for SHM
char * semreadkeypathname = "/bin/ping";    // This is a pathname used to generate a key for read Semaphores
char * semwritekeypathname = "/bin/cp";     // pathname used to create key for write semaphores
char * semmsgqueuepathname="/bin/pwd";    // pathname to create key for shared request queue
#endif


int msqid;    // This is a unique id for accessing our message queue.
shmnode_t ** listshmnodes;    // array of SHM node info
int NumSegments;    // Number of segments
shmnode_t * LinkedListAvailableShmNodes;

pthread_mutex_t LockAvailableShmNodes;   // mutex lock to prevent race conditions on queue of available shm segments

// struct for handling semaphores
union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};

void debugprint(char * message, ...)
{
    if (debugmode==1) {
        va_list ap;
        va_start(ap, message);
        //vprintf(message, ap);    // prints to stdout
        vfprintf(stderr, message, ap);   // prints to stderr
        va_end(ap);
    }
}


// Extract the filename from a filepath string. Basically, go through string backwards until you hit the first /
char * getfilename (char * filepath)
{
    int len = strlen(filepath);
    int lastslash=0;
    for (int iLoop=len; iLoop>0; iLoop--)
    {
        if (filepath[iLoop]=='/')
        {
            lastslash=iLoop;
            break;
        }
    }

    // Now create a new string and grab the end part
    int filenamelength=len-lastslash;
    char * result = (char *) malloc(filenamelength + 5);
    strcpy(result, filepath + lastslash+1);
    result[filenamelength+1]='\0';

    return result;
}


// This is Beej's routine for allowing 2 different processes to attempt to create a semaphore or
// grab a semaphore if it already exists.
/*
** initsem() -- more-than-inspired by W. Richard Stevens' UNIX Network
** Programming 2nd edition, volume 2, lockvsem.c, page 295.
*/
int initsem(key_t key, int nsems)  /* key from ftok() */
{
    int i;
    union semun arg;
    struct semid_ds buf;
    struct sembuf sb;
    int semid;

    semid = semget(key, nsems, IPC_CREAT | IPC_EXCL | 0666);

    if (semid >= 0) { /* we got it first */
        sb.sem_op = 1; sb.sem_flg = 0;
        arg.val = 1;

        for(sb.sem_num = 0; sb.sem_num < nsems; sb.sem_num++) {
            /* do a semop() to "free" the semaphores. */
            /* this sets the sem_otime field, as needed below. */
            if (semop(semid, &sb, 1) == -1) {
                int e = errno;
                semctl(semid, 0, IPC_RMID); /* clean up */
                errno = e;
                return -1; /* error, check errno */
            }
        }

    } else if (errno == EEXIST) { /* someone else got it first */
        int ready = 0;

        semid = semget(key, nsems, 0); /* get the id */
        if (semid < 0) return semid; /* error, check errno */

        /* wait for other process to initialize the semaphore: */
        arg.buf = &buf;
        for(i = 0; i < MAX_RETRIES && !ready; i++) {
            semctl(semid, nsems-1, IPC_STAT, arg);
            if (arg.buf->sem_otime != 0) {
                ready = 1;
            } else {
                sleep(1);
            }
        }
        if (!ready) {
            errno = ETIME;
            return -1;
        }
    } else {
        return semid; /* error, check errno */
    }

    return semid;
}