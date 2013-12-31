
/*
 *	ais.c
 *
 *	(c) Ruben Undheim 2008
 *	(c) Heikki Hannikainen 2008
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#include "ais.h"
#include "receiver.h"
#include "protodec.h"
#include "hmalloc.h"
#include "hlog.h"
#include "cfg.h"
#include "out_mysql.h"
#include "out_json.h"
#include "cache.h"
#include "ipc.h"
#ifdef HAVE_PULSEAUDIO
#include "pulseaudio.h"
#endif

#include <string.h>
#include "sound.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

int done;

void closedown(int sig)
{
	done = 1;
}
void brokenconnection(int sig)
{
	hlog(LOG_DEBUG,"There is a broken connection\n");
}

int main(int argc, char *argv[])
{
	int err;
	done = 0;
	FILE *sound_in_fd = NULL;
	FILE *sound_out_fd = NULL;
	int channels;
	short *buffer = NULL;
	int buffer_l;
	int buffer_read;
	struct serial_state_t *serial = NULL;
	struct ipc_state_t *ipc = NULL;
	struct receiver *rx_a = NULL;
	struct receiver *rx_b = NULL;
	
	/* open syslog, write an initial log message and read configuration */
	open_log(logname, 0);
	hlog(LOG_NOTICE, "Starting up...");
	if (read_config()) {
		hlog(LOG_CRIT, "Initial configuration failed.");
		exit(1);
	}
	
	/* initialize position cache for timed JSON AIS transmission */
	if (uplink_config) {
		hlog(LOG_DEBUG, "Initializing cache...");
		if (cache_init())
			exit(1);
		hlog(LOG_DEBUG, "Initializing jsonout...");
		if (jsonout_init())
			exit(1);
	}
	
	/* initialize the AIS decoders */
	if (sound_channels != SOUND_CHANNELS_MONO) {
		hlog(LOG_DEBUG, "Initializing demodulator A");
		rx_a = init_receiver('A', 2, 0,ipc);
		hlog(LOG_DEBUG, "Initializing demodulator B");
		rx_b = init_receiver('B', 2, 1,ipc);
		channels = 2;
	} else {
		hlog(LOG_DEBUG, "Initializing demodulator A");
		rx_a = init_receiver('A', 1, 0,ipc);
		channels = 1;
	}


	printf("HEJ!");

		hlog(LOG_NOTICE, "Reading audio from blob.");
		buffer_l = 1024;
		int extra = buffer_l % 5;
		buffer_l -= extra;
		buffer = (short *) hmalloc(buffer_l * sizeof(short) * channels);
	
	hlog(LOG_NOTICE, "Started");
	
	int dataoffset = 0;

#define AAA 0

#if AAA
	sound_in_fd = fopen("xaa", "rb");
#endif

	while (!done) {
		printf(".");
#if AAA
		buffer_read = fread(buffer, channels * sizeof(short), buffer_l, sound_in_fd);
			if (buffer_read <= 0)
				done = 1;
#else
		memcpy(buffer, staticSoundData + dataoffset, buffer_l * channels * sizeof(short));
		dataoffset += buffer_l * channels;
		if (dataoffset >= sizeof(staticSoundData) / sizeof(short)) {
			done = 1;
		}
		buffer_read = buffer_l;

#endif

# if 0
		printf("buffer_read: %d\n", buffer_read);
		int xx;
		for (xx = 0; xx < buffer_l * channels; xx++)
			printf("%x ", buffer[xx]);
		
		done = 1;
#endif

		if (sound_channels == SOUND_CHANNELS_MONO) {
			receiver_run(rx_a, buffer, buffer_read);
		}
		if (sound_channels == SOUND_CHANNELS_BOTH
		    || sound_channels == SOUND_CHANNELS_RIGHT) {
			/* ch a/0/right */
			receiver_run(rx_a, buffer, buffer_read);
		}
		if (sound_channels == SOUND_CHANNELS_BOTH
		    || sound_channels == SOUND_CHANNELS_LEFT) {	
			/* ch b/1/left */
			receiver_run(rx_b, buffer, buffer_read);
		}
	}
	
	hlog(LOG_NOTICE, "Closing down...");
	if (sound_in_fd) {
		fclose(sound_in_fd);
	}
	
	hfree(buffer);

	if (uplink_config)
		jsonout_deinit();
	
	if (cache_positions)
		cache_deinit();
	
	if (rx_a) {
		struct demod_state_t *d = rx_a->decoder;
		hlog(LOG_INFO,
			"A: Received correctly: %d packets, wrong CRC: %d packets, wrong size: %d packets",
			d->receivedframes, d->lostframes,
			d->lostframes2);
	}
	
	if (rx_b) {
		struct demod_state_t *d = rx_b->decoder;
		hlog(LOG_INFO,
			"B: Received correctly: %d packets, wrong CRC: %d packets, wrong size: %d packets",
			d->receivedframes, d->lostframes,
			d->lostframes2);
	}
	
	free_receiver(rx_a);
	free_receiver(rx_b);
	
	free_config();
	close_log(0);
	
	return 0;
}

