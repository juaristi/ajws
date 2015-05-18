/*
 * main.c
 *
 *  Created on: May 14, 2015
 *      Author: ajuaristi
 */
#include <stdio.h>
#include <pfring.h>
#include "main.h"
#include "log.h"

int main(int argc, char **argv)
{
	const char *device = "eth0";
	u_char mac_addr[6];

	opt.debug = TRUE;
	opt.verbose = TRUE;

	pfring *ring = pfring_open(device, 128, 0);
	if (ring == NULL)
	{
		printf("ERROR: Could not open device %s.\n", device);
		exit(EXIT_FAILURE);
	}

	if (pfring_get_bound_device_address(ring, mac_addr) == 0)
	{
		printf("Using device %s. ", device);
		printf("MAC address: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
				mac_addr[0],
				mac_addr[1],
				mac_addr[2],
				mac_addr[3],
				mac_addr[4],
				mac_addr[5]);
	}
	else
	{
		printf("Could not get MAC from device %s.\n", device);
	}

	pfring_close(ring);

	return 0;
}
