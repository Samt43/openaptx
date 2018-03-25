/*
 * [open]aptx - bt-aptx-stub.c
 * Copyright (c) 2017 Arkadiusz Bokowy
 *
 * This file is a part of [open]aptx.
 *
 * This project is licensed under the terms of the MIT license.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "openaptx.h"
#include <libavcodec/avcodec.h>
 
/* Auto-generated buffer with apt-X encoded test sound. */
extern unsigned char sonar_aptx[];
extern unsigned int sonar_aptx_len;

static struct aptxbtenc_encoder {
	bool swap;
	unsigned int counter;
    const AVCodec * avCodec;
    AVCodecContext *c;
    AVFrame *frame;
    AVPacket *pkt;
} encoder;


static void encode(AVCodecContext *ctx, AVFrame *frame, AVPacket *pkt,
                   uint16_t code[2])
{
    int ret;
    /* send the frame for encoding */
    ret = avcodec_send_frame(ctx, frame);
    if (ret < 0) {
        fprintf(stderr, "error sending the frame to the encoder\n");
        return ret;
    }
    /* read all the available output packets (in general there may be any
     * number of them */
    while (ret >= 0) {
        ret = avcodec_receive_packet(ctx, pkt);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            fprintf(stderr, "error encoding audio frame\n");
            return ret;
        }

        memcpy(code, pkt->data, pkt->size);
        av_packet_unref(pkt);
    }
}

int aptxbtenc_init(APTXENC enc, bool swap) {
    int ret = 0;
	struct aptxbtenc_encoder *e = (struct aptxbtenc_encoder *)enc;

	fprintf(stderr, "\n"
			"WARNING! Initializing apt-X encoder stub library. This library does NOT\n"
			"perform any encoding at all. The sole reason for this library to exist,\n"
			"is to simplify integration with the proprietary apt-X encoder library\n"
			"for open-source projects.\n\n");

	e->swap = swap;
	e->counter = 0;

        /* register all the codecs */
    avcodec_register_all();
    e->avCodec = avcodec_find_encoder(AV_CODEC_ID_APTX);
    if (!e->avCodec) {
        fprintf(stderr, "codec not found\n");
        return -1;
    }
    
    e->c = avcodec_alloc_context3(e->avCodec);
    /* put sample parameters */
    e->c->bit_rate = 48000;
    /* check that the encoder supports s16 pcm input */
    e->c->sample_fmt = AV_SAMPLE_FMT_S32P;
    e->c->sample_rate = 48000;
    
    /* select other audio parameters supported by the encoder */
    e->c->frame_size    = 4;
    e->c->time_base     = (AVRational){ 1, 48000 };
    e->c->channel_layout = AV_CH_LAYOUT_STEREO;
    e->c->channels       = av_get_channel_layout_nb_channels(e->c->channel_layout);
    /* open it */
    if (avcodec_open2(e->c, e->avCodec, NULL) < 0) {
        fprintf(stderr, "could not open codec\n");
        return ret;
    }
    /* packet for holding encoded output */
    e->pkt = av_packet_alloc();
    if (!e->pkt) {
        fprintf(stderr, "could not allocate the packet\n");
        return ret;
    }
    /* frame containing input raw audio */
    e->frame = av_frame_alloc();
    if (!e->frame) {
        fprintf(stderr, "could not allocate audio frame\n");
        return ret;
    }
    e->frame->nb_samples     = e->c->frame_size;
    e->frame->format         = e->c->sample_fmt;
    e->frame->channel_layout = e->c->channel_layout;
    /* allocate the data buffers */
    ret = av_frame_get_buffer(e->frame, 0);
    if (ret < 0) {
        fprintf(stderr, "could not allocate audio data buffers\n");
        return ret;
    }

    return ret;
}

int aptxbtenc_encodestereo(
		APTXENC enc,
		const int32_t pcmL[4],
		const int32_t pcmR[4],
		uint16_t code[2]) {
	(void)pcmL;
	(void)pcmR;
    int ret;

	struct aptxbtenc_encoder *e = (struct aptxbtenc_encoder *)enc;
    
    /* make sure the frame is writable -- makes a copy if the encoder
        * kept a reference internally */
    ret = av_frame_make_writable(e->frame);
    if (ret < 0)
        return ret;
    uint16_t* samples = (uint16_t*)e->frame->data[0];

    for (int j = 0; j < e->c->frame_size; j++) {
        samples[2*j] = pcmL[j];
        for (int k = 1; k < e->c->channels; k++)
            samples[2*j + k] = pcmR[j];
    }
    encode(e->c, e->frame, e->pkt, code);

	return ret;
}

const char *aptxbtenc_build(void) {
	return "stub-1.0";
}

const char *aptxbtenc_version(void) {
	return "1.0.0";
}

size_t SizeofAptxbtenc(void) {
	return sizeof(encoder);
}

APTXENC NewAptxEnc(bool swap) {
	aptxbtenc_init(&encoder, swap);
	return &encoder;
}

APTXENC aptxbtenc_init2(bool swap) {
	struct aptxbtenc_encoder *e;
	if ((e = malloc(SizeofAptxbtenc())) != NULL)
		aptxbtenc_init(e, swap);
	return e;
}

void aptxbtenc_free(APTXENC enc) {

	struct aptxbtenc_encoder *e = (struct aptxbtenc_encoder *)enc;
    av_frame_free(&e->frame);
    av_packet_free(&e->pkt);
    avcodec_free_context(&e->c);
    
	free(enc);
}
