/* *
 * This file is part of libnemesi
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 *
 * libnemesi is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libnemesi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libnemesi; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * */

#include "rtpparser.h"
#include "rtp_utils.h"
#include <math.h>

/**
 * @file rtp_aac.c
 * MPEG 4 Part 3 depacketizer RFC 3640
 */

/**
 * List element, contains aac frames recovered from an aggregated rtp packet.
 */
typedef struct aac_elem_s {
    uint8_t *data;
    long len;
    struct aac_elem_s *next;
} aac_elem;

/**
 * Local structure, contains data necessary to compose a aac frame out
 * of rtp fragments.
 */
typedef struct {
    uint8_t *data;		//!< constructed frame, fragments will be copied there
    long len;			//!< buf length, it's the sum of the fragments length
    long data_size;		//!< allocated bytes for data
    unsigned long timestamp;	//!< timestamp of progressive frame
    uint8_t *conf;		//!< extradata
    long conf_len;		//!< extradata length
    int size_len;		//!< Number of bits in the AU header for fragment size
    int index_len;		//!< Number of bits in the AU header for the index
    int delta_len;		//!< Number of bits in the AU header for the delta index
    aac_elem *head;		//!< Head of aac frame list
} rtp_aac;



static rtpparser_info aac_served = {
    -1,
    {"MPEG4-GENERIC", NULL}
};

static int aac_init_parser(rtp_session * rtp_sess, unsigned pt)
{
    rtp_aac *priv = calloc(1, sizeof(rtp_aac));
    rtp_pt_attrs *attrs = &rtp_sess->ptdefs[pt]->attrs;
    char value[1024];
    uint8_t buffer[1024];
    int i, v_len, len, err = RTP_ERRALLOC;

    if (!priv)
	return RTP_ERRALLOC;


    nms_printf(NMSML_WARN, "aac_init_parser >>>>\n");

    for (i = 0; i < attrs->size; i++) {
	if ((v_len = nms_get_attr_value(attrs->data[i], "config", value,
					sizeof(value)))) {
	    nms_printf(NMSML_WARN, "config: %s\n", value);
	    if (!(v_len % 2) && v_len > 0) {
		if ((len =
		     nms_hex_decode(buffer, value, sizeof(buffer))) < 0)
		    goto err_alloc;
		if (!
		    (priv->conf =
		     realloc(priv->conf, priv->conf_len + len)))
		    goto err_alloc;
		memcpy(priv->conf + priv->conf_len, buffer, len);
		priv->conf_len += len;
	    }
	}
	if ((v_len = nms_get_attr_value(attrs->data[i], "mode", value,
					sizeof(value)))) {
	    nms_printf(NMSML_WARN, "mode: %s\n", value);
	    if (strcmp(value, "AAC-hbr")) {
		nms_printf(NMSML_ERR, "Mode %s not supported\n", value);
		err = RTP_PARSE_ERROR;
		goto err_alloc;
	    }
	}
	if ((v_len = nms_get_attr_value(attrs->data[i], "sizeLength",
					value, sizeof(value)))) {
	    nms_printf(NMSML_WARN, "sizeLength: %s\n", value);
	    if ((priv->size_len = strtol(value, NULL, 0)) != 13) {
		nms_printf(NMSML_ERR, "Size Length %d not supported\n",
			   priv->size_len);
		err = RTP_PARSE_ERROR;
		goto err_alloc;
	    }
	}
	if ((v_len = nms_get_attr_value(attrs->data[i], "indexLength",
					value, sizeof(value)))) {
	    nms_printf(NMSML_WARN, "indexLength: %s\n", value);
	    if ((priv->index_len = strtol(value, NULL, 0)) != 3) {
		nms_printf(NMSML_ERR, "Index Length %d not supported\n",
			   priv->index_len);
		err = RTP_PARSE_ERROR;
		goto err_alloc;
	    }
	}
	if ((v_len = nms_get_attr_value(attrs->data[i], "indexDeltaLength",
					value, sizeof(value)))) {
	    nms_printf(NMSML_WARN, "indexDeltaLength: %s\n", value);
	    if ((priv->delta_len = strtol(value, NULL, 0)) != 3) {
		nms_printf(NMSML_ERR,
			   "Index Delta Length %d not supported\n",
			   priv->index_len);
		err = RTP_PARSE_ERROR;
		goto err_alloc;
	    }
	}
	// chenlei, 
	if ((v_len = nms_get_attr_value(attrs->data[i], "constantDuration",
					value, sizeof(value)))) {
	    nms_printf(NMSML_WARN, "interleaving is not support\n");
	    err = RTP_PARSE_ERROR;
	    goto err_alloc;
	}
    }

    rtp_sess->ptdefs[pt]->priv = priv;

    nms_printf(NMSML_WARN, "aac_init_parser <<<<\n");

    return 0;

  err_alloc:
    if (priv->data)
	free(priv->data);
    free(priv);
    return err;
}

static int aac_uninit_parser(rtp_ssrc * ssrc, unsigned pt)
{
    aac_elem *tmp;
    rtp_aac *priv = ssrc->rtp_sess->ptdefs[pt]->priv;

    if (priv) {
	if (priv->data)
	    free(priv->data);
	if (priv->conf)
	    free(priv->conf);
	while (priv->head) {
	    tmp = priv->head;
	    priv->head = priv->head->next;
	    if (tmp->data)
		free(tmp->data);
	    free(tmp);
	}
	free(priv);
    }

    ssrc->rtp_sess->ptdefs[pt]->priv = NULL;

    return 0;
}

/**
 * it should return an aac frame by fetching one or more than
 * a single rtp packet
 */

static int aac_parse(rtp_ssrc * ssrc, rtp_frame * fr, rtp_buff * config)
{
    rtp_pkt *pkt;
    uint8_t *buf, *payload;
    aac_elem *cur = NULL;
    rtp_aac *priv = ssrc->rtp_sess->ptdefs[fr->pt]->priv;
    size_t len;
    int i, buf_index;
    int header_len, header_len_bytes, headers_num, frame_len, frame_index;

    int err = RTP_FILL_OK;
#if 0
    {
        static int xxxaaa = 1;
        if (xxxaaa) {
            xxxaaa = 0;
            nms_printf(NMSML_ERR, "%s: lchen: Not do real parse\n",
                    __FUNCTION__);
        }

        if (rtp_get_pkt(ssrc, &len) == NULL)
            return RTP_BUFF_EMPTY;
        rtp_rm_pkt(ssrc);
        return -1;
    }
#endif
    if (!(pkt = rtp_get_pkt(ssrc, &len)))
        return RTP_BUFF_EMPTY;

    payload = buf = RTP_PKT_DATA(pkt);
    len = RTP_PAYLOAD_SIZE(pkt, len);

    if (priv->len && (RTP_PKT_TS(pkt) != priv->timestamp)) {
        nms_printf(NMSML_WARN,
                "incomplete packet without final fragment\n");
        //incomplete packet without final fragment -> flush
        fr->data = priv->data;
        fr->len = priv->len;
        priv->len = 0;
        return RTP_FILL_OK;
    }

    if (!priv->head) {
        header_len = (buf[0] << 8) | buf[1];	// the value in bits
        header_len_bytes = (header_len + 7) / 8;
        headers_num = header_len / 16;	//ripped from gstreamer

//        printf("------- PTR payload -------\n");
//        printf("%15s %d (bits)\n", "headers len", header_len);
//        printf("%15s %d (bytes)\n", "headers len", header_len_bytes);
//        printf("%15s %d\n", "headers number", headers_num);
//        printf("%15s %d\n", "RTP Mark", RTP_PKT_MARK(pkt));
//        printf("\n");

        payload += 2;
        buf_index = 2 + header_len_bytes;

        //chenlei
        if (headers_num == 1 && RTP_PKT_MARK(pkt) == 0) {
            nms_printf(NMSML_WARN, "fragment (not last)!!!\n");
        } else if (headers_num > 1 && RTP_PKT_MARK(pkt) == 0) {
            nms_printf(NMSML_WARN, "bad pkt!!!\n");
            rtp_rm_pkt(ssrc);
            return RTP_PKT_UNKNOWN;
        }

        for (i = 0; i < headers_num; i++) {
            frame_len = ((payload[0] << 8) | payload[1]) >> 3;
            frame_len &= 0x1FFF;	//chenlei, for safe
            frame_index = payload[1] & 0x7;
            payload += 2;

//            printf("AU[%d]\n", i);
//            printf("  %15s %d\n", "frame len", frame_len);
//            printf("  %15s %d\n", "frame index", frame_index);

            if (!cur) {
                cur = priv->head = calloc(1, sizeof(aac_elem));
            } else {
                cur->next = calloc(1, sizeof(aac_elem));
                cur = cur->next;
            }

            if (!cur)
                return RTP_ERRALLOC;

            cur->data = malloc(frame_len);

            if (!cur->data)
                return RTP_ERRALLOC;

            memcpy(cur->data, buf + buf_index, frame_len);

            cur->len = frame_len;
            buf_index += frame_len;
        }
    }

    if (priv->data_size < priv->len + priv->head->len) {
        if (!(priv->data = realloc(priv->data, len + priv->len))) {
            return RTP_ERRALLOC;
        }
        priv->data_size = priv->len + priv->head->len;
    }

    if (priv->head) {
        memcpy(priv->data + priv->len, priv->head->data, priv->head->len);
        priv->len += priv->head->len;
        cur = priv->head;
        priv->head = priv->head->next;
        if (cur->data)
            free(cur->data);
        free(cur);
    }

    if (!RTP_PKT_MARK(pkt)) {
        priv->timestamp = RTP_PKT_TS(pkt);
        err = EAGAIN;
    } else {
        fr->data = priv->data;
        fr->len = priv->len;
        priv->len = 0;
//        printf("frame data: %02x %02x %02x %02x\n", 
//                fr->data[0], fr->data[1], fr->data[2], fr->data[3]);
//        printf("frame  len: %d\n", fr->len);
    }

    if (priv->conf_len) {
        config->data = priv->conf;
        config->len = priv->conf_len;
        //printf("config: %02x %02x, len %d\n", config->data[0], config->data[1], config->len);
    }

    if (!priv->head) {
        rtp_rm_pkt(ssrc);
    }

    return err;
}

RTP_PARSER_FULL(aac);
