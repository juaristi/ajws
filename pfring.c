/*
 * pfring.c
 *
 *  Created on: Jul 14, 2015
 *      Author: ajuaristi
 */

#include <ajws.h>
#include <pfring.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include "log.h"
#include "dev.h"

bool dev_open(pajws_dev_t dev)
{
	bool success = false;
	u_char mac_addr[6];
	pfring *ring;

	ring = pfring_open(dev->name, 128, 0);
	if (ring == NULL)
	{
		logprintf(LOG_FATAL, "Could not open device %s.\n", dev->name);
		goto end;
	}

	pfring_set_direction(ring, rx_only_direction);
	pfring_set_application_name(ring, "ajws");

	if (pfring_enable_ring(ring))
	{
		logprintf(LOG_FATAL, "The packet ring buffer could not be enabled.\n");
		goto end;
	}

	if (pfring_get_bound_device_address(ring, mac_addr) == 0)
	{
		logprintf(LOG_VERBOSE, "Using device '%s'. ", dev->name);
		logprintf(LOG_VERBOSE, "MAC address: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
			mac_addr[0],
			mac_addr[1],
			mac_addr[2],
			mac_addr[3],
			mac_addr[4],
			mac_addr[5]);
		memcpy(dev->mac_addr, mac_addr, sizeof(mac_addr));

		success = true;
	}
	else
	{
		logprintf(LOG_FATAL, "Could not get MAC address from device '%s'.\n", dev->name);
	}

	/* TODO we don't get the IP address for now */
	memset(&dev->ip_addr, 0, sizeof(struct sockaddr));

	/* Don't lose our PF_RING handle */
	dev->priv = (void *) ring;

end:
	return success;
}

void dev_close(pajws_dev_t dev)
{
	pfring *ring = (pfring *) dev->priv;
	pfring_close(ring);
}

int dev_poll(pajws_dev_t dev, u_char *buf, u_int len, pinfo_t pi)
{
	struct pfring_pkthdr hdr;
	int result = 0;
	pfring *ring = (pfring *) dev->priv;

	result = pfring_recv(ring, &buf, len, &hdr, 0);
	if (pi && result == 1)
	{
		memcpy(&pi->ts, &hdr.ts, sizeof(struct timeval));
		pi->caplen = hdr.caplen;
		pi->len = hdr.len;
	}

	return result;
}
