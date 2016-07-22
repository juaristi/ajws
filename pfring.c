/*
 * pfring.c
 *
 *  Created on: Jul 14, 2015
 *      Author: ajuaristi
 */

#include "ajws.h"
#include <pfring.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include "log.h"
#include "dev.h"

bool dev_open(ajws_dev_t * dev)
{
	bool success = false;
	pfring *ring;

	ring = pfring_open(dev->name, 128, 0);
	if (ring == NULL) {
		logprintf(LOG_FATAL, "Could not open device %s.\n", dev->name);
		goto end;
	}

	pfring_set_direction(ring, rx_only_direction);
	pfring_set_application_name(ring, "ajws");

	if (pfring_enable_ring(ring)) {
		logprintf(LOG_FATAL,
			  "The packet ring buffer could not be enabled.\n");
		goto end;
	}

	/* Don't lose our PF_RING handle */
	dev->priv = (void *)ring;

 end:
	return success;
}

void dev_close(ajws_dev_t * dev)
{
	pfring *ring = (pfring *) dev->priv;
	pfring_close(ring);
}

void dev_interrupt(ajws_dev_t * dev)
{
	pfring *ring = (pfring *) dev->priv;
	pfring_breakloop(ring);
}

int dev_poll(ajws_dev_t * dev, u_char * buf, u_int len, info_t * pi)
{
	struct pfring_pkthdr hdr;
	int result = 0;
	pfring *ring = (pfring *) dev->priv;

	result = pfring_recv(ring, &buf, len, &hdr, 0);
	if (pi && result == 1) {
		memcpy(&pi->ts, &hdr.ts, sizeof(struct timeval));
		pi->caplen = hdr.caplen;
		pi->len = hdr.len;
	}

	return result;
}
