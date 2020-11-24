#ifndef __AUDIO_H__
#define __AUDIO_H__
#include <gst/gst.h>

/* define the audio and video content */
#define AUDIO_RECEIVER_UDPSRC_CAPS          "application/x-rtp,channels=2"
#define AUDIO_RECEIVER_OPUSDEC_PLC          TRUE

struct AUDIOSENDER{
    GstElement *pipeline;

    GstElement *alsasrc;   
	GstElement *audiorate;
	GstElement *audioconvert;
	GstElement *audioresample; 
	GstElement *opusenc;
	GstElement *rtpopuspay;
	GstElement *udpsink;

	GstCaps *opusenc_caps;

    GstBus *bus;
    GstMessage *message;
    GstStateChangeReturn ret;
};

struct AUDIORECEIVER{
    GstElement *pipeline;

    GstElement *udpsrc;
    GstElement *rtpjitterbuffer;
    GstElement *rtpopusdepay;
    GstElement *opusdec;
    GstElement *audioconvert;
    GstElement *audioresample;
    GstElement *autoaudiosink;

    GstCaps *udpsrc_caps;

    GstBus *bus;
    GstMessage *message;
    GstStateChangeReturn ret;
};

int audio_init();
int change_audio_pipeline_state();
int handle_audio_error();
void audio_release();

#endif
