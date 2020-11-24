#include "../include/utils.h"

struct opts_struct opts = {
    4006, (char *)"hw:1,0", 4007, (char *)"192.168.21.22", \
    5006, \
	5007, (char *)"/dev/video0", 5007, (char *)"127.0.0.1"
};

void usage()
{
	printf("GStreamer audio&video publish&subscribe\n");
	printf("Usage: app <flag> <options>\n");
    printf("flag is:\n");
#if AUDIO
    printf("0  send and receive other's audio streame.\n");
#endif
#if VIDEO
    printf("1  just send video streame\n");
    printf("2  send and receive other's video streame.\n");
    printf("3  send and receive both other's and own video streame.\n");
#endif
    printf("options are:\n");
#if AUDIO
	printf("  --audioSrcPort\t\t<port>\t\t(default is 4000)\n");
	printf("  --audioDevice\t\t\t<device>\t(default is hw:0,1)\n");
	printf("  --audioDesPort\t\t<port>\t\t(default is 4000)\n");
	printf("  --audioDesHost\t\t<hostname>\t(default is 192.168.21.22)\n");
    printf("\n");
#endif
#if VIDEO
	printf("  --ownVideoPort\t\t<port>\t\t(default is 5006)\n");
	printf("  --videoSrcPort\t\t<port>\t\t(default is 5006)\n");
	printf("  --videoDevice\t\t\t<device>\t(default is /dev/video0)\n");
	printf("  --videoDesPort\t\t<port>\t\t(default is 5006)\n");
	printf("  --videoDesHost\t\t<hostname>\t(default is localhost)\n");
#endif
	exit(-1);
}

void getopts(int argc, char *argv[])
{
    int count = 2;
    while(count < argc){
        if(strcmp(argv[count], "--audioSrcPort") == 0){
            if(++count < argc){
                // printf("audioSrcPort: %d.\n", atoi(argv[count]));
                opts.audioSrcPort = atoi(argv[count]);
            }else
                usage();
        }
        else if(strcmp(argv[count], "--audioDevice") == 0){
            if(++count < argc){
                // printf("audioDevice: %s.\n", argv[count]);
                //memset(opts.audioDevice, 0, sizeof(opts.audioDevice));
                opts.audioDevice = argv[count];
            }else
                usage();            
        }
        else if(strcmp(argv[count], "--audioDesPort") == 0){
            if(++count < argc){
                // printf("audioDesPort: %d.\n", opts.audioDesPort);
                opts.audioDesPort = atoi(argv[count]);
            }else
                usage();
        }
        else if(strcmp(argv[count], "--audioDesHost") == 0){
            if(++count < argc){
                // memset(opts.audioDesHost, 0, sizeof(opts.audioDesHost));
                opts.audioDesHost = argv[count];
            }else
                usage();                        
        }
        else if(strcmp(argv[count], "--ownVideoPort") == 0){
            if(++count < argc){
                opts.ownVideoPort = atoi(argv[count]);
            }else
                usage();
        }
        else if(strcmp(argv[count], "--videoSrcPort") == 0){
            if(++count < argc){
                opts.videoSrcPort = atoi(argv[count]);
            }else
                usage();
        }
        else if(strcmp(argv[count], "--videoDevice") == 0){
            if(++count < argc){
                // memset(opts.videoDevice, 0, sizeof(opts.videoDevice));
                opts.videoDevice = argv[count];
            }else
                usage();           
        }
        else if(strcmp(argv[count], "--videoDesPort") == 0){
            if(++count < argc){
                opts.videoDesPort = atoi(argv[count]);
            }else
                usage();
        }
        else if(strcmp(argv[count], "--videoDesHost") == 0){
            if(++count < argc){
                // memset(opts.videoDesHost, 0, sizeof(opts.videoDesHost));
                opts.videoDesHost = argv[count];
            }else
                usage();            
        }
        else if(strcmp(argv[count], "--help") == 0){
            usage();
        }
        count++;        
    }
}

int init(int flag, int argc, char **argv)
{
    if (flag != 0 && flag != 1 && flag != 2 && flag !=3){
        usage();
        exit(-1);
    }

    int ret = 0;

    print_info(flag);
    /* Initialize GStreamer */
    gst_init(&argc, &argv);

#if AUDIO
	/* audio function */
    audio_init();
    change_audio_pipeline_state();
	/* handle_audio_error(); */
	/* audio_release(); */
#endif

#if VIDEO
	/* video function */
	video_init(flag);
	change_video_pipeline_state(flag);
	handle_video_error(flag);
	video_release(flag);
#endif
#if AUDIO
	handle_audio_error();
	audio_release();
#endif

    return 0;
}

int get_local_ip(const char *eth_inf, char *ip)
{
    int sd;
    struct sockaddr_in sin;
    struct ifreq ifr;

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sd)
    {
        printf("socket error: %s\n", strerror(errno));
        return -1;
    }

    strncpy(ifr.ifr_name, eth_inf, IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    // if error: No such device
    if (ioctl(sd, SIOCGIFADDR, &ifr) < 0)
    {
        printf("ioctl error: %s\n", strerror(errno));
        close(sd);
        return -1;
    }

    memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
    snprintf(ip, IP_SIZE, "%s", inet_ntoa(sin.sin_addr));

    close(sd);
    return 0;
}

void print_info(int flag)
{
#if AUDIO
    if (flag >= 0){
        printf("flag is: %d\n", flag);
        printf("informations:\n");
        printf("now use %s to send audioStream to %s:%d.\n", \
                        opts.audioDevice, \
                        opts.audioDesHost, \
                        opts.audioDesPort);
        printf("listening on %d to receive audioStream.\n", \
                        opts.audioSrcPort);
    }
#endif
#if VIDEO
    if (flag >= 1){
        printf("now use %s to send video streame to %s:%d.\n", \
                        opts.videoDevice, \
                        opts.videoDesHost, \
                        opts.videoDesPort);
    }
    if (flag >= 2){
        printf("listening on %d to receive other's video streame.\n", \
                        opts.videoSrcPort);
    }
	if (flag >= 3){
		printf("listening on %d to receive own video streame.\n", \
				opts.ownVideoPort);
	}
#endif
}

