/*
 * main.c
 *
 *  Created on: May 14, 2015
 *      Author: ajuaristi
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include "ajws.h"
#include "options.h"
#include "log.h"
#include "dev.h"
#include "alloc.h"

bool interrupted;
ajws_dev_t *dev;

static void
print_usage(const char *name)
{
	printf("./%s [-dv] <ifname|ipaddr> <port>\n", name);
}

void
sighandler(int signal)
{
	logprintf(LOG_ALWAYS, "Interrupted. ");
	if (dev)
		dev_interrupt(dev);
	interrupted = true;
}

static void
ajws_run(ajws_dev_t *dev)
{
	char *ipaddr = NULL;
#define BUFLEN 512
	u_char buf[BUFLEN];
	u_int bufsiz = BUFLEN;
	info_t pi;

	/* Prepare the target device for capturing packets */
	if (!dev_open(dev))
	{
		logprintf(LOG_FATAL, "Could not open device '%s'.\n", dev->name);
		exit(EXIT_FAILURE);
	}

	/* We're up and running. Print some info about our chosen device. */
	logprintf(LOG_VERBOSE, "Using device '%s'.\n", dev->name);
	if (dev->mac_addr[0] != 0)
		logprintf(LOG_VERBOSE, "\tMAC address: %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n",
				dev->mac_addr[0],
				dev->mac_addr[1],
				dev->mac_addr[2],
				dev->mac_addr[3],
				dev->mac_addr[4],
				dev->mac_addr[5]);
	else
		logprintf(LOG_VERBOSE, "\tMAC address: <not found>\n");

	ipaddr = inet_ntoa(dev->addr.sin_addr);
	logprintf(LOG_VERBOSE, "\tIP address: %s\n", (ipaddr ? ipaddr : "<not found>"));

	while (!interrupted)
	{
		switch (dev_poll(dev, buf, bufsiz, &pi))
		{
		case 0:		/* No packets received. Continue. */
			break;
		case 1:		/* We received a packet! */
			logprintf(LOG_DEBUG, "Packet received (%d.%ud)\n", pi.ts.tv_sec, pi.ts.tv_usec);
			hexlog((const char *) buf, pi.caplen);
			break;
		case -1:	/* Error */
		default:	/* Fall through. Consider every other value an error. */
			logprintf(LOG_FATAL, "Could not fetch packet. Exiting.\n");
			interrupted = true;
			break;
		}
	}

	dev_close(dev);
}

#ifndef TESTING
int
main(int argc, char **argv)
{
#define MAX_PORT 65536
	int pos;
	struct in_addr addr;
	bool (* find_iface_func) (ajws_dev_t *) = NULL;
	struct sigaction sigact = {
		.sa_flags = SA_NODEFER,
		.sa_handler = sighandler
	};

	interrupted = false;
	dev = NULL;

	if (argc < 3)
		goto bail;

	init_options();
	pos = parse_options(argc, argv);

	if (opt.help || (pos + 2 != argc))
		goto bail;

	/* Register signal handlers */
	sigaction(SIGINT, &sigact, NULL);

	/* Allocate buffers */
	dev = (ajws_dev_t *) ec_malloc(sizeof(ajws_dev_t));
	memset(dev, 0, sizeof(ajws_dev_t));

	/*
	 * Parse the source address.
	 * Here we could have either a dotted IPv4 address, or an interface name.
	 */
	if (inet_aton(argv[pos], &addr))
	{
		/* This is a valid IP address */
		memcpy(&dev->addr.sin_addr, &addr, sizeof(dev->addr.sin_addr));
		find_iface_func = dev_find_iface_by_ipaddr;
	}
	else
	{
		/* User specified an interface name */
		dev->name = (char *) ec_malloc(strlen(argv[pos]) + 1);
		strcpy(dev->name, argv[pos]);
		find_iface_func = dev_find_iface_by_name;
	}

	if (!find_iface_func(dev))
		goto bail;

	/* Parse the source port we'll listen on */
	dev->addr.sin_port = (in_port_t) strtol(argv[pos + 1], NULL, 10);
	if (errno == ERANGE || dev->addr.sin_port > MAX_PORT)
	{
		/* User probably introduced an invalid port */
		goto bail;
	}

	ajws_run(dev);

	ec_free_all();
	return 0;

bail:
	print_usage(argv[0]);
	exit(0);
}
#else
int
main(int argc, char **argv)
{
	char logstr[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,'A','B','C'};

	opt.debug = TRUE;
	opt.verbose = TRUE;

	hexlog(logstr, sizeof(logstr));

	return 0;
}
#endif
