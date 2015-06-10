/*
 * main.c
 *
 *  Created on: May 14, 2015
 *      Author: ajuaristi
 */

#include <stdio.h>
#include <signal.h>
#include <pfring.h>
#include "main.h"
#include "log.h"

#define APP_NAME "ajws"
bool interrupted;

void sighandler(int signal)
{
	logprintf(LOG_ALWAYS, "Interrupted.\n");
	interrupted = TRUE;
}

#ifndef TESTING
int main(int argc, char **argv)
{
	const char *device = "eth0";
	u_char mac_addr[6];
#define BUFLEN 512
	u_char *buf = NULL;
	u_int bufsiz = 0;
	struct pfring_pkthdr hdr;

	opt.debug = TRUE;
	opt.verbose = TRUE;

	/* Register signal handlers */
	interrupted = FALSE;
	signal(SIGINT, sighandler);

	/* Allocate buffers */
	buf = (u_char *) malloc(BUFLEN * sizeof(char));
	if (!buf)
	{
		logprintf(LOG_FATAL, "Could not allocate packet buffer.\n");
		exit(EXIT_FAILURE);
	}
	bufsiz = BUFLEN;

	/* Start PF_RING */
	pfring *ring = pfring_open(device, 128, 0);
	if (ring == NULL)
	{
		logprintf(LOG_FATAL, "Could not open device %s.\n", device);
		exit(EXIT_FAILURE);
	}

	pfring_set_application_name(ring, APP_NAME);
	pfring_set_direction(ring, rx_and_tx_direction);
	if (pfring_enable_ring(ring))
	{
		logprintf(LOG_FATAL, "The packet ring buffer could not be enabled.\n");
		exit(EXIT_FAILURE);
	}

	if (pfring_get_bound_device_address(ring, mac_addr) == 0)
	{
		logprintf(LOG_VERBOSE, "Using device %s. ", device);
		logprintf(LOG_VERBOSE, "MAC address: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
				mac_addr[0],
				mac_addr[1],
				mac_addr[2],
				mac_addr[3],
				mac_addr[4],
				mac_addr[5]);
	}
	else
	{
		logprintf(LOG_FATAL, "Could not get MAC from device %s.\n", device);
	}

	while (!interrupted)
	{
		switch (pfring_recv(ring, &buf, bufsiz, &hdr, 0))
		{
		case 0:		/* No packets received. Continue. */
			break;
		case 1:		/* We received a packet! */
			logprintf(LOG_ALWAYS, "Packet received (%d.%ud)\n", hdr.ts.tv_sec, hdr.ts.tv_usec);
			hexlog(buf, hdr.caplen);
			break;
		case -1:	/* Error */
		default:	/* Fall through. Consider every other value an error. */
			logprintf(LOG_FATAL, "Could not fetch packet. Exiting.\n");
			interrupted = TRUE;
			break;
		}
	}

	pfring_close(ring);
	free(buf);

	return 0;
}
#else
int main(int argc, char **argv)
{
	char logstr[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,'A','B','C'};

	opt.debug = TRUE;
	opt.verbose = TRUE;

	hexlog(logstr, sizeof(logstr));

	return 0;
}
#endif
