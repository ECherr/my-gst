#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "audio.h"
#include "video.h"

#define MAC_SIZE		    18
#define IP_SIZE			    16

/* FLAG
 * 0    -   without videostreamer
 * 1    -   with videostreamer(send&&receive)
 * 2    -   with videostreamer(send&&receive(others and own)
 */
#define FLAG                0

struct opts_struct{
    // audio options
    int audioSrcPort;
    char *audioDevice;
    int audioDesPort;
    char *audioDesHost;

    // video options
	int ownVideoPort;
    int videoSrcPort;
    char *videoDevice;
    int videoDesPort;
    char *videoDesHost;
}; 

void usage();
void getopts(int argc, char **argv);
int init(int flag, int argc, char **argv);
int get_local_ip(const char *eth_inf, char *ip);
void print_info(int flag);

#endif

