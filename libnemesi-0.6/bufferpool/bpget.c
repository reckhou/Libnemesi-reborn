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

#include "bufferpool.h"
#include "comm.h"
#include <syslog.h>

FILE* logfp;

/*!
* \brief Restituisce uno slot di memoria libero dal Buffer Pool.
*
* Prende dalla testa della Free List l'indice di uno slot libero
* dal quale si puo' ricavare il puntatore all'area di memoria
* da utilizzare.
*
* \param bp Il puntatore al Buffer Pool corrente.
* \return L'indice dello slot libero nel vettore del Buffer Pool.
* \see bprmv
* \see bufferpool.h
* */
static char gbploc[32] = {0};

void bp_setloc(char* location)
{
	if (location != NULL)
		strcpy(gbploc, location);
	else 
		sprintf(gbploc, "NOT SET");
}

int bpget(buffer_pool * bp)
{
        int offset;
#ifdef BPSTAT
		static int bpgetcnt = 0;
		char bplog[64] = {0};
		char bplogcmd[64] = "date >> ";
		sprintf(bplog, "/var/ftp/Bpstat%s.log", gbploc);
		strcat(bplogcmd, bplog);
#endif
		pthread_mutex_lock(&(bp->fl_mutex));
        while (bp->flhead == -1) {
                if (bpenlarge(bp)) {
                        nms_printf(NMSML_WARN, "Bufferpool reached maximum size!\n");
//						syslog(LOG_ERR, "%s Bufferpool reached maximum size, reboot!\n");
						if ( (logfp = fopen("/var/ftp/reboot.log", "a")) != NULL )
						{
							fprintf(logfp, "esdump: Bufferpool reached maximum size, location %s, reboot!\n", gbploc);
							fclose(logfp);
							system("date >> /var/ftp/reboot.log");
	 						//system("echo 'nvd close reboot' >> /var/ftp/reboot.log");
						}
						system("/root/nvd_app/mcuhb.out off");
						system("reboot");
                        pthread_cond_wait(&(bp->cond_full), &(bp->fl_mutex));
                } else {
                        nms_printf(NMSML_DBG1, "Bufferpool enlarged\n");
                }
        }

        offset = bp->flhead;
        bp->flhead = bp->freelist[bp->flhead];
        bp->flcount++;
#ifdef BPSTAT
		if(bpgetcnt == 500){
						if ( (logfp = fopen(bplog, "a")) != NULL )
						{
							fprintf(logfp, "Bufferpool[%s]: Freeslotcnt:%d, Used size:%dKB, Alloacted bpsize:%dKB\n", gbploc, bp->flcount, bp->flcount*BP_SLOT_SIZE/1024, bp->size*BP_SLOT_SIZE/1024);
							fclose(logfp);
							system(bplogcmd);
	 						//system("echo 'nvd close reboot' >> /var/ftp/reboot.log");
						}
						bpgetcnt = 0;
		}						
						bpgetcnt++;
#endif
        pthread_mutex_unlock(&(bp->fl_mutex));
		
        return offset;
}


