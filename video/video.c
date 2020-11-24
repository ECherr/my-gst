#include "../include/video.h"
#include "../include/utils.h"
 
extern struct opts_struct opts;
struct VIDEOSENDER videoSender;
struct VIDEORECEIVER videoReceiver;
struct VIDEORECEIVER videoReceiverForOwn;

/* Initialize the part of audio
 * 1. create pipeline
 * 2. create elements
 * 3. chek up creation
 * 4. set properties and caps
 * 5. add elements to pipeline
 */
int video_init(const int flag)
{
    /* create pipelines */
    /* videoSend */
    videoSender.pipeline = gst_pipeline_new("videoSender");

    /* videoReceive 
     * flag 1: receive from others
     * flag 2: recevie with feedback
     */
    if (flag >= 2){
        videoReceiver.pipeline = gst_pipeline_new("videoReceiver");
    }
    if (flag >= 3){
		videoReceiverForOwn.pipeline = gst_pipeline_new("videoReceiveOwn");
    }

    /* create elements */
    /* videoSend */
    videoSender.v4l2src = gst_element_factory_make(\
            "v4l2src", "videoSender_v4l2src");
    videoSender.rtpjpegpay = gst_element_factory_make(\
            "rtpjpegpay", "videoSender_rtpjpegpay");
    videoSender.multiudpsink = gst_element_factory_make(\
            "multiudpsink", "videoSender_multiudpsink");

	/* videoReceive */
	if (flag >= 2){
		videoReceiver.udpsrc = gst_element_factory_make(\
				"udpsrc", "videoReceiver_udpsrc");
		videoReceiver.rtpjpegdepay = gst_element_factory_make(\
				"rtpjpegdepay", "videoReceiver_rtpjpegdepay");
        videoReceiver.jpegparse = gst_element_factory_make(\
				"jpegparse", "videoReceiver_jpegparse");
        videoReceiver.jpegdec = gst_element_factory_make(\
				"jpegdec", "videoReceiver_jpegdec");
        videoReceiver.autovideosink = gst_element_factory_make(\
				"autovideosink", "videoReceiver_autovideosink");
	}
	if (flag >= 3){
		videoReceiverForOwn.udpsrc = gst_element_factory_make(\
				"udpsrc", "videoReceiverForOwn_udpsrc");
		videoReceiverForOwn.rtpjpegdepay = gst_element_factory_make(\
				"rtpjpegdepay", "videoReceiverForOwn_rtpjpegdepay");
        videoReceiverForOwn.jpegparse = gst_element_factory_make(\
				"jpegparse", "videoReceiverForOwn_jpegparse");
        videoReceiverForOwn.jpegdec = gst_element_factory_make(\
				"jpegdec", "videoReceiverForOwn_jpegdec");
        videoReceiverForOwn.autovideosink = gst_element_factory_make(\
				"autovideosink", "videoReceiverForOwn_autovideosink");
	}


    /* create success? */
    if(!videoSender.pipeline \
            || !videoSender.v4l2src \
            || !videoSender.rtpjpegpay \
            || !videoSender.multiudpsink){
        g_printerr("Not all elements(videoSender) could be created.\n");
        return -1;
    }
	if (flag >= 2){
    	if(!videoReceiver.pipeline \
				|| !videoReceiver.udpsrc \
				|| !videoReceiver.rtpjpegdepay \
				|| !videoReceiver.jpegparse \
				|| !videoReceiver.jpegdec \
				|| !videoReceiver.autovideosink){
        	g_printerr("Not all elements(video receiver) could be created.\n");
        	return -1;
       }
	}
	if (flag >= 3){
    	if(!videoReceiverForOwn.pipeline \
				|| !videoReceiverForOwn.udpsrc \
				|| !videoReceiverForOwn.rtpjpegdepay \
				|| !videoReceiverForOwn.jpegparse \
				|| !videoReceiverForOwn.jpegdec \
				|| !videoReceiverForOwn.autovideosink){
        	g_printerr("Not all elements(video receiver) could be created.\n");
        	return -1;
       }
	}

    /* set properties and caps for elements */
    /* videoSender */
    g_object_set(videoSender.v4l2src, \
            "device", opts.videoDevice, NULL);
	if (flag >= 3) {
		char multiclients[64];
		sprintf(multiclients, "127.0.0.1:%d,%s:%d", \
				opts.ownVideoPort, \
				opts.videoDesHost, \
				opts.videoDesPort);
    	printf("multiclients are %s", \
            	multiclients);
    	g_object_set(videoSender.multiudpsink, \
				"clients", \
				multiclients, NULL);  
	}else {
		char multiclients[32];
		sprintf(multiclients, "%s:%d", \
				opts.videoDesHost, \
				opts.videoDesPort);
    	g_object_set(videoSender.multiudpsink, \
				"clients", \
				multiclients, NULL);  
	}

	if (flag >= 2){
		g_object_set(videoReceiver.udpsrc, \
				"port", opts.videoSrcPort, NULL);
        videoReceiver.udpsrc_caps = gst_caps_from_string(\
				VIDEO_RECEIVER_UDPSRC_CAPS);
        g_object_set(videoReceiver.udpsrc, \
				"caps", videoReceiver.udpsrc_caps, NULL);
        gst_caps_unref(videoReceiver.udpsrc_caps);
	}
	if (flag >= 3){
		g_object_set(videoReceiverForOwn.udpsrc, \
				"port", opts.ownVideoPort, NULL);
        videoReceiverForOwn.udpsrc_caps = gst_caps_from_string(\
				VIDEO_RECEIVER_UDPSRC_CAPS);
        g_object_set(videoReceiverForOwn.udpsrc, \
				"caps", videoReceiverForOwn.udpsrc_caps, NULL);
        gst_caps_unref(videoReceiverForOwn.udpsrc_caps);
	}

    /* add elements to pipeline */
    // videoSender
    gst_bin_add_many(GST_BIN(videoSender.pipeline), \
            videoSender.v4l2src, \
            videoSender.rtpjpegpay, \
            videoSender.multiudpsink, \
            NULL);
    link_elements_with_filter(\
            videoSender.v4l2src, \
            videoSender.rtpjpegpay);
    gst_element_link(\
            videoSender.rtpjpegpay, \
            videoSender.multiudpsink);

	if (flag >= 2){
        gst_bin_add_many(GST_BIN(videoReceiver.pipeline), \
				videoReceiver.udpsrc, \
				videoReceiver.rtpjpegdepay, \
				videoReceiver.jpegparse, \
				videoReceiver.jpegdec, \
				videoReceiver.autovideosink, \
				NULL);
        if(gst_element_link_many(\
					videoReceiver.udpsrc, \
					videoReceiver.rtpjpegdepay, \
					videoReceiver.jpegparse, \
					videoReceiver.jpegdec, \
					videoReceiver.autovideosink, NULL) != TRUE){
            g_printerr("Elements(video receiver) could not be linked.\n");
            return -1;
        }
	}
	if (flag >= 3){
        gst_bin_add_many(GST_BIN(videoReceiverForOwn.pipeline), \
				videoReceiverForOwn.udpsrc, \
				videoReceiverForOwn.rtpjpegdepay, \
				videoReceiverForOwn.jpegparse, \
				videoReceiverForOwn.jpegdec, \
				videoReceiverForOwn.autovideosink, \
				NULL);
        if(gst_element_link_many(\
					videoReceiverForOwn.udpsrc, \
					videoReceiverForOwn.rtpjpegdepay, \
					videoReceiverForOwn.jpegparse, \
					videoReceiverForOwn.jpegdec, \
					videoReceiverForOwn.autovideosink, NULL) != TRUE){
            g_printerr("Elements(video receiver own) could not be linked.\n");
            return -1;
        }
	}

    return 0;
}

int change_video_pipeline_state(const int flag)
{
    videoSender.ret = gst_element_set_state(\
            videoSender.pipeline, GST_STATE_PLAYING);
    if(videoSender.ret == GST_STATE_CHANGE_FAILURE){
        g_printerr("Unable to set the videoSender_pipeline"
                "to the playing state.\n");
        return -1;
    }

	if (flag >= 2){
    	videoReceiver.ret = gst_element_set_state(\
				videoReceiver.pipeline, GST_STATE_PLAYING);    
    	if(videoReceiver.ret == GST_STATE_CHANGE_FAILURE){    
        	g_printerr("Unable to set the videoReceiver_pipeline"
					" to the playing state.\n");    
        	return -1;    
      	}
	}
	if (flag >= 3){
    	videoReceiverForOwn.ret = gst_element_set_state(\
				videoReceiverForOwn.pipeline, GST_STATE_PLAYING);    
    	if(videoReceiverForOwn.ret == GST_STATE_CHANGE_FAILURE){    
        	g_printerr("Unable to set the videoReceiverForOwn_pipeline"
					" to the playing state.\n");    
        	return -1;    
      	}
	}

   return 0;     
}

int handle_video_error(const int flag)
{
    // videoSender
    videoSender.bus = gst_element_get_bus(\
            videoSender.pipeline);
    videoSender.message = gst_bus_timed_pop_filtered(\
            videoSender.bus, \
            GST_CLOCK_TIME_NONE, \
            GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    if(videoSender.message != NULL){
        GError *err;
        gchar *debug_info;
        switch (GST_MESSAGE_TYPE(videoSender.message))
        {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(videoSender.message, \
                        &err, \
                        &debug_info);
                g_printerr("(video sender)Error received from element %s: %s\n", \
                        GST_OBJECT_NAME(videoSender.message->src), \
                        err->message);
                g_printerr("(video sender)Debugging information: %s\n",
                    debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("(video sender)End-Of-Stream reached.\n");
                break;
            default:
                /* We should not reach here because we only aked for ERRORs and EOS */
                g_printerr("(video sender)Unexpected message received.\n");
                break;
        }
        gst_message_unref(videoSender.message);
    }

    if (flag >= 2){
		videoReceiver.bus = gst_element_get_bus(\
				videoReceiver.pipeline);
    	videoReceiver.message = gst_bus_timed_pop_filtered(\
				videoReceiver.bus, \
				GST_CLOCK_TIME_NONE, \
				GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
	    if(videoReceiver.message != NULL){
        	GError *err;
        	gchar *debug_info;
        	switch (GST_MESSAGE_TYPE(videoReceiver.message)){
            case GST_MESSAGE_ERROR:
            	gst_message_parse_error(\
						videoReceiver.message, \
						&err, \
						&debug_info);
                g_printerr("(video receiver)Error received from element " \
						"%s: %s\n", \
						GST_OBJECT_NAME(videoReceiver.message->src), \
						err->message);
                g_printerr("(video receiver)Debugging information: %s\n", \
						debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("(video receiver)End-Of-Stream reached.\n");
                break;
            default:
                /* We should not reach here because we only aked for ERRORs and EOS */
                g_printerr("(video receiver)Unexpected message received.\n");
                break;
        	}
    		gst_message_unref(videoReceiver.message);
    	}
	}
    if (flag >= 3){
		videoReceiverForOwn.bus = gst_element_get_bus(\
				videoReceiverForOwn.pipeline);
    	videoReceiverForOwn.message = gst_bus_timed_pop_filtered(\
				videoReceiverForOwn.bus, \
				GST_CLOCK_TIME_NONE, \
				GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
	    if(videoReceiverForOwn.message != NULL){
        	GError *err;
        	gchar *debug_info;
        	switch (GST_MESSAGE_TYPE(videoReceiverForOwn.message)){
            case GST_MESSAGE_ERROR:
            	gst_message_parse_error(\
						videoReceiverForOwn.message, \
						&err, \
						&debug_info);
                g_printerr("(video receiver)Error received from element " \
						"%s: %s\n", \
						GST_OBJECT_NAME(videoReceiverForOwn.message->src), \
						err->message);
                g_printerr("(video receiver)Debugging information: %s\n", \
						debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("(video receiver)End-Of-Stream reached.\n");
                break;
            default:
                /* We should not reach here because we only aked for ERRORs and EOS */
                g_printerr("(video receiver)Unexpected message received.\n");
                break;
        	}
    		gst_message_unref(videoReceiverForOwn.message);
    	}
    }

    return 0;
}

void video_release(const int flag)
{
    gst_object_unref(videoSender.bus);
    gst_element_set_state(videoSender.pipeline, GST_STATE_NULL);
    gst_object_unref(videoSender.pipeline);  

    if (flag >= 2){
		gst_object_unref(videoReceiver.bus);
    	gst_element_set_state(videoReceiver.pipeline, GST_STATE_NULL);
    	gst_object_unref(videoReceiver.pipeline);
    }
    if (flag >= 3){
		gst_object_unref(videoReceiverForOwn.bus);
    	gst_element_set_state(videoReceiverForOwn.pipeline, GST_STATE_NULL);
    	gst_object_unref(videoReceiverForOwn.pipeline);
    }
}

static gboolean 
link_elements_with_filter(GstElement *element1, GstElement *element2){
    gboolean link_ok;
    GstCaps *caps;

    caps = gst_caps_new_simple ("image/jpeg",
            "width", G_TYPE_INT, 640,
            "height", G_TYPE_INT, 480,
            "framerate", GST_TYPE_FRACTION, 30, 1,
            NULL);

    link_ok = gst_element_link_filtered (element1, element2, caps);
    gst_caps_unref (caps);

    if (!link_ok) {
        g_warning ("Failed to link element1 and element2!");
    }

    return link_ok;
}

