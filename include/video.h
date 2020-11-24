#ifndef __VIDEO_H__
#define __VIDEO_H__
#include <gst/gst.h>

#define VIDEO_RECEIVER_UDPSRC_CAPS          "application/x-rtp, encoding-name=JPEG,payload=26"

struct VIDEOSENDER{
    GstElement *pipeline;

    GstElement *v4l2src;;
    GstElement *rtpjpegpay;
    GstElement *multiudpsink;
    
    GstCaps *v4l2src_caps;

    GstBus *bus;
    GstMessage *message;
    GstStateChangeReturn ret;

};

struct VIDEORECEIVER{
    GstElement *pipeline;

    GstElement *udpsrc;
    GstElement *rtpjpegdepay;
    GstElement *jpegparse; 
    GstElement *jpegdec;
    GstElement *autovideosink;

    GstCaps *udpsrc_caps;
   
    GstBus *bus;
    GstMessage *message;
    GstStateChangeReturn ret;

};

static gboolean link_elements_with_filter(GstElement *element1, GstElement *element2);

int  video_init(const int flag);
int  change_video_pipeline_state(const int flag);
int  handle_video_error(const int flag);
void video_release(const int flag);


#endif

