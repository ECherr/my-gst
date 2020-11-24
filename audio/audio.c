#include <gst/gst.h>

#include "../include/audio.h"
#include "../include/utils.h"

extern struct opts_struct opts;
struct AUDIOSENDER audioSender;
struct AUDIORECEIVER audioReceiver;

/* Initialize the part of audio
 * 1. create pipeline
 * 2. create elements
 * 3. chek up creation
 * 4. set properties and caps
 * 5. add elements to pipeline
 */
int audio_init()
{
    /* create pipelines */
    audioSender.pipeline = gst_pipeline_new("audioSender");
    audioReceiver.pipeline = gst_pipeline_new("audioReceiver");

    /* create elements */
    /* audioSend */
    audioSender.alsasrc = gst_element_factory_make(\
            "alsasrc", "audioSender_src");
    audioSender.audiorate = gst_element_factory_make(\
            "audiorate", "audioSender_audiorate");
    audioSender.audioconvert = gst_element_factory_make(\
            "audioconvert", "audioSender_audioconvert");
    audioSender.audioresample = gst_element_factory_make(\
            "audioresample", "audioSender_audioresample");
    audioSender.opusenc = gst_element_factory_make(\
            "opusenc", "audioSender_opusenc");
    audioSender.rtpopuspay = gst_element_factory_make(\
            "rtpopuspay", "audioSender_rtpopuspay");
    audioSender.udpsink = gst_element_factory_make(\
            "udpsink", "audioSender_udpsink");

    /* audioReceive */
    audioReceiver.udpsrc = gst_element_factory_make(\
            "udpsrc", "audioReceiver_udpsrc");
    audioReceiver.rtpjitterbuffer = gst_element_factory_make(\
            "rtpjitterbuffer", "audioReceiver_rtpjitterbuffer");
    audioReceiver.rtpopusdepay = gst_element_factory_make(\
            "rtpopusdepay", "audioReceiver_rtpopusdepay");
    audioReceiver.opusdec = gst_element_factory_make(\
            "opusdec", "audioReceiver_opusdec");
    audioReceiver.audioconvert = gst_element_factory_make(\
            "audioconvert", "audioReceiver_audioconvert");
    audioReceiver.audioresample = gst_element_factory_make(\
            "audioresample", "audioReceiver_audioresample");
    audioReceiver.autoaudiosink = gst_element_factory_make(\
            "autoaudiosink", "audioReceiver_autoaudiosink");

    /* create success? */
    /* audioSend */
    if (!audioSender.pipeline \
            || !audioSender.alsasrc \
            || !audioSender.audiorate \
            || !audioSender.audioconvert \
            || !audioSender.audioresample \
            || !audioSender.opusenc \
            || !audioSender.rtpopuspay \
            || !audioSender.udpsink){
        g_printerr("Not all elements(audioSend) could be created.\n");
        return -1;        
    }
    /* audioReceive */
    if (!audioReceiver.pipeline \
            || !audioReceiver.udpsrc \
            || !audioReceiver.rtpjitterbuffer \
            || !audioReceiver.rtpopusdepay \
            || !audioReceiver.opusdec \
            || !audioReceiver.audioconvert \
            || !audioReceiver.audioresample \
            || !audioReceiver.autoaudiosink){
        g_printerr("Not all elements(audioReceiver) could be created.\n");
        return -1;        
    }
    /* set properties and caps for elements */
    /* audioSend */
    g_object_set(audioSender.alsasrc, \
            "device", opts.audioDevice, NULL);
    g_object_set(audioSender.opusenc, \
            "frame-size", 5, NULL);
    g_object_set(audioSender.opusenc, \
            "bitrate-type", 1, NULL);
    g_object_set(audioSender.udpsink, \
            "host", opts.audioDesHost, NULL);
    g_object_set(audioSender.udpsink, \
            "port", opts.audioDesPort, NULL);
    /* audioReceive */
    g_object_set(audioReceiver.udpsrc, \
            "port", opts.audioSrcPort, NULL);
    audioReceiver.udpsrc_caps = gst_caps_from_string(\
            AUDIO_RECEIVER_UDPSRC_CAPS);
    g_object_set(audioReceiver.udpsrc, \
            "caps", audioReceiver.udpsrc_caps, NULL);
    g_object_set(audioReceiver.opusdec, \
            "plc", AUDIO_RECEIVER_OPUSDEC_PLC, NULL);

    /* add elements to pipeline */
    /* audioSend */
    gst_bin_add_many(GST_BIN(audioSender.pipeline), \
            audioSender.alsasrc, \
            audioSender.audiorate, \
            audioSender.audioconvert, \
            audioSender.audioresample, \
            audioSender.opusenc, \
            audioSender.rtpopuspay, \
            audioSender.udpsink, \
            NULL);
    if (gst_element_link_many(\
                audioSender.alsasrc, \
                audioSender.audiorate, \
                audioSender.audioconvert, \
                audioSender.audioresample, \
                audioSender.opusenc, \
                audioSender.rtpopuspay, \
                audioSender.udpsink, NULL) != TRUE){
        g_printerr("Elements(audioSender) could not be linked.\n");
        return -1;
    }
    /* audioReceive */
    gst_bin_add_many(GST_BIN(audioReceiver.pipeline), \
            audioReceiver.udpsrc, \
            audioReceiver.rtpjitterbuffer, \
            audioReceiver.rtpopusdepay, \
            audioReceiver.opusdec, \
            audioReceiver.audioconvert, \
            audioReceiver.audioresample, \
            audioReceiver.autoaudiosink, NULL);
    if (gst_element_link_many(\
                audioReceiver.udpsrc, \
                audioReceiver.rtpjitterbuffer, \
                audioReceiver.rtpopusdepay, \
                audioReceiver.opusdec, \
                audioReceiver.audioconvert, \
                audioReceiver.audioresample, \
                audioReceiver.autoaudiosink, NULL) != TRUE){
        g_printerr("Elements(audioReceiver) could not be linked.\n");
        return -1;
    }

    return 0;
}

int change_audio_pipeline_state()
{ 
    /* audioSend */
    audioSender.ret = gst_element_set_state(\
            audioSender.pipeline, GST_STATE_PLAYING);
    if (audioSender.ret == GST_STATE_CHANGE_FAILURE){
        g_printerr("Unable to set the audioSender_pipeline"
                " to the playing state.\n");
        return -1;
    }
    /* audioReceive */
    audioReceiver.ret = gst_element_set_state(\
            audioReceiver.pipeline, GST_STATE_PLAYING);
    if (audioReceiver.ret == GST_STATE_CHANGE_FAILURE){
        g_printerr("Unable to set the audioReceiver_pipeline" 
                " to the playing state.\n");
        return -1;
    }

    return 0;
}

int handle_audio_error()
{
    /* error - bus - message */
    /* audioSend */
	printf("lala0\n");
    audioSender.bus = gst_element_get_bus(\
            audioSender.pipeline);
	printf("lala1\n");
	audioSender.message = gst_bus_timed_pop_filtered(audioSender.bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
    
	printf("lala2\n");
    if(audioSender.message != NULL){
        GError *err;
        gchar *debug_info;
        switch (GST_MESSAGE_TYPE(audioSender.message))
        {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(\
                        audioSender.message, \
                        &err, \
                        &debug_info);
                g_printerr("(audio send)Error received from element %s: %s\n", \
                    GST_OBJECT_NAME(audioSender.message->src), \
                    err->message);
                g_printerr("(audio send)Debugging information: %s\n", \
                    debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("(audio send)End-Of-Stream reached.\n");
                break;
            default:
                /* We should not reach here because we only aked for ERRORs and EOS */
                g_printerr("(audio send)Unexpected message received.\n");
                break;
        }
        gst_message_unref(audioSender.message);
    }
    
    // audioReceiver
    audioReceiver.bus = gst_element_get_bus(\
            audioReceiver.pipeline);
    audioReceiver.message = gst_bus_timed_pop_filtered(\
            audioReceiver.bus, \
            GST_CLOCK_TIME_NONE, \
            GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
    
    if(audioReceiver.message != NULL){
        GError *err;
        gchar *debug_info;
        switch (GST_MESSAGE_TYPE(audioReceiver.message))
        {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(audioReceiver.message, \
                        &err, \
                        &debug_info);
                g_printerr("(audio receive)Error received from element %s: %s\n",\
                    GST_OBJECT_NAME(audioReceiver.message->src), \
                    err->message);
                g_printerr("(audio receive)Debugging information: %s\n",
                    debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("(audio receiver)End-Of-Stream reached.\n");
                break;
            default:
                /* We should not reach here because we only aked for ERRORs and EOS */
                g_printerr("(audio receiver)Unexpected message received.\n");
                break;
        }
        gst_message_unref(audioReceiver.message);
    }  

	printf("lala3\n");
    return 0;
}

void audio_release()
{
	gst_object_unref(audioSender.bus);
    gst_element_set_state(audioSender.pipeline, GST_STATE_NULL);
    gst_object_unref(audioSender.pipeline);

    gst_object_unref(audioReceiver.bus);
    gst_element_set_state(audioReceiver.pipeline, GST_STATE_NULL);
    gst_object_unref(audioReceiver.pipeline);
}

