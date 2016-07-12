// Neil Kimber
// Test of using Sun OS RPC
// Read a binary file (an image file)
// Send to RPC call (on localhost)
// Receive new image file as a return

#include <errno.h>
#include <getopt.h>
#include <stdio.h>

#include <strings.h>
#include <stdlib.h>     // atoi() etc..
#include "shm_channel.h"

/* OPTIONS DESCRIPTOR ====================================================== */
static struct option gLongOptions[] = {
        {"port",          required_argument,      NULL,           'p'},
        {"thread-count",  required_argument,      NULL,           't'},
        {"server",        required_argument,      NULL,           's'},
        {"help",          no_argument,            NULL,           'h'},
        {NULL,            0,                      NULL,             0}
};

#define USAGE                                                                 \
"usage:\n"                                                                    \
"  clientmain [options]\n"                                                     \
"options:\n"                                                                  \
"  -n number of segments to use in communication with cache.           "\
"  -z the size (in bytes) of the segments.              "\
"  -p [listen_port]    Listen port (Default: 8888)\n"                         \
"  -t [thread_count]   Num worker threads (Default: 1, Range: 1-1000)\n"      \
"  -s [server]         The server to connect to (Default: Udacity S3 instance)"\
"  -h                  Show this help message\n"                              \
"special options:\n"                                                          \
"  -d [drop_factor]    Drop connects if f*t pending requests (Default: 5).\n"


//int debugmode=1;

/* Main ========================================================= */
int main(int argc, char **argv) {
    int  option_char = 0;

    //int port, nworkerthreads, sizeSegments, numSegments;
   // char * server;

    // Use getopts

    // Parse and set command line arguments
    while ((option_char = getopt_long(argc, argv, "p:t:s:h:z:n:v", gLongOptions, NULL)) != -1) {
        switch (option_char) {
            case 'p': // listen-port
                //port = atoi(optarg);
                break;
            case 't': // thread-count
                //nworkerthreads = atoi(optarg);
                break;
            case 's': // file-path
                //server = optarg;
                break;
            case 'h': // help
                fprintf(stdout, "%s", USAGE);
                exit(0);
                break;
            case 'v':
                debugmode=1;    // turn on debugging
                debugprint("Debugging output turned on.\n");
                break;
            case 'z':
                //sizeSegments= atoi(optarg);
                break;
            case 'n':
                //numSegments= atoi(optarg);
                break;
            default:
                fprintf(stderr, "%s", USAGE);
                exit(1);
        }
    }

    printf("Starting program...\n");

}