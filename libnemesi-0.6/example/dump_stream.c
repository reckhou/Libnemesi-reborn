/*
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "rtsp.h"
#include "rtp.h"
#include "sdp.h"


int main(int argc, char **argv)
{
	static int flag = 0;
        int opt, i = 0;
        char *url, *out = malloc(12), *base = "dump_nms";
        int outfd[128];
        rtsp_ctrl *ctl;
        rtp_thread *rtp_th;
        rtsp_session *sess;
        rtp_ssrc *ssrc;
        rtp_buff conf;
        rtp_frame fr;
        nms_rtsp_hints rtsp_hints = { -1 };

        if (argc < 2) {
                fprintf(stderr, "\tPlease specify at least an url.\n");
                fprintf(stderr, "\tUsage: %s [-f basename ][-p port][-t][-s][-b prebuf] url\n",
                        argv[0]);
                exit(1);
        }

        while ((opt = getopt(argc, argv, "f:p:b:ts")) != -1) {
                switch (opt) {
				case 'f':  /*  Set output file  */
                        base = strdup(optarg);
                        out = realloc(out, strlen(base) + 4);
                        break;
                case 'p': /* Set rtp port */
                        rtsp_hints.first_rtp_port = atoi(optarg);
                        break;
                case 't': /* Force TCP interleaved */
                        rtsp_hints.pref_rtsp_proto = TCP;
                        rtsp_hints.pref_rtp_proto = TCP;
                        break;
                case 's': /* Force SCTP */
                        rtsp_hints.pref_rtsp_proto = SCTP;
                        rtsp_hints.pref_rtp_proto = SCTP;
                        break;
				case 'b': /* Prebuffer size */
                        rtsp_hints.prebuffer_size = atoi(optarg);
                        break;
                case '?': /* Unknown option */
                        fprintf(stderr, "\n  Unknown option `-%c'.\n", optopt);
                        fprintf(stderr,
                                "\tUsage: %s [-f outputfile ][-p port] url\n",
                                argv[0]);

                        return 1;
                }
        }

        memset(outfd, 0, sizeof(outfd));

        url = argv[argc - 1];

        fprintf(stderr, "URL %s.\n", url);

        if ((ctl = rtsp_init(&rtsp_hints)) == NULL) {
                fprintf(stderr, "Cannot init rtsp.\n");
                return 1;
        }

        if (rtsp_open(ctl, url)) {
                fprintf(stderr, "rtsp_open failed.\n");
                // die
                return 1;
        }
        rtsp_wait(ctl);

        //Get the session information
        sess = ctl->rtsp_queue;
        if (!sess) {
                fprintf(stderr, "No session available.\n");
                return 1;
        }

        rtsp_play(ctl, 0.0, 0.0);
        rtsp_wait(ctl);

        fprintf(stderr, "\nDumping...");

        rtp_th = rtsp_get_rtp_th(ctl);
        while (!rtp_fill_buffers(rtp_th)) {  // Till there is something to parse
        	// Foreach ssrc active
                for (ssrc=rtp_active_ssrc_queue(rtsp_get_rtp_queue(ctl)); ssrc; ssrc=rtp_next_active_ssrc(ssrc)) {
                        if (!rtp_fill_buffer(ssrc, &fr, &conf)) {    // Parse the stream
                                if (outfd[fr.pt] ||    // Write it to a file
                                                sprintf(out, "%s.%d", base, fr.pt)
                                                && (outfd[fr.pt] = creat(out, 00644)) > 0) {
					/* Write the pps and sps */
					if (i++ % 100 == 0) {
						unsigned char sps_start[] = {0x00,0x00,0x00,0x01,0x67,0x42};

						if (memcmp(conf.data, sps_start, sizeof(sps_start)) == 0) {
							nms_printf(NMSML_WARN, "set profile to main, level to 40\n");
							conf.data[5] = 77;
							conf.data[7] = 40;
						}

						if(write(outfd[fr.pt], conf.data, conf.len) < conf.len) {
							return 1;
						}
					}
                                        if (write(outfd[fr.pt], fr.data, fr.len) < fr.len)
                                                return 1;
                                } else {
                                        return 1;
                                }
                        }
                }
        }

        for (i = 0; i < 128; i++)
                if (outfd[i])
                        close(outfd[i]);

        fprintf(stderr, " Complete\n");

        free(out);

        rtsp_close(ctl);
        rtsp_wait(ctl);

        rtsp_uninit(ctl);

        return 0;
}
