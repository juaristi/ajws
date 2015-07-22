/*
 * main.c
 *
 *  Created on: May 14, 2015
 *      Author: ajuaristi
 */

#include <ajws.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "log.h"
#include "dev.h"

#define APP_NAME "ajws"
bool interrupted;

void sighandler(int signal)
{
	logprintf(LOG_ALWAYS, "Interrupted. ");
	interrupted = true;
}

#ifndef TESTING
int main(int argc, char **argv)
{
	const char *device = "lo";

	pajws_dev_t dev = NULL;
	info_t pi;
	struct sockaddr_in *ipaddr;
#define BUFLEN 512
	u_char buf[BUFLEN];
	u_int bufsiz = BUFLEN;

	char answer[3];

	opt.debug = true;
	opt.verbose = true;

	/* Register signal handlers */
	interrupted = false;
	signal(SIGINT, sighandler);

	/* Allocate buffers */
	dev = (pajws_dev_t) malloc(sizeof(ajws_dev_t));
	if (!dev)
	{
		logprintf(LOG_FATAL, "Could not allocate device buffer for device '%s'.\n", device);
		exit(EXIT_FAILURE);
	}
	memset(dev, 0, sizeof(ajws_dev_t));

	dev->name = (char *) malloc(strlen(device) + 1);
	if (!dev->name)
	{
		logprintf(LOG_FATAL, "Could not allocate device buffer for device '%s'.\n", device);
		exit(EXIT_FAILURE);
	}
	strcpy(dev->name, device);

	/* Prepare the target device for capturing packets */
	if (!dev_open(dev, APP_NAME))
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

	if (dev->ip_addr.sa_family)
	{
		ipaddr = (struct sockaddr_in *) &dev->ip_addr;
		logprintf(LOG_VERBOSE, "\tIP address: %s\n", inet_ntoa(ipaddr->sin_addr));
	}
	else
		logprintf(LOG_VERBOSE, "\tIP address: <not found>\n");

	while (!interrupted)
	{
		switch (dev_poll(dev, buf, bufsiz, &pi))
		{
		case 0:		/* No packets received. Continue. */
			break;
		case 1:		/* We received a packet! */
			logprintf(LOG_ALWAYS, "Packet received (%d.%ud)\n", pi.ts.tv_sec, pi.ts.tv_usec);
			hexlog(buf, pi.caplen);
			break;
		case -1:	/* Error */
		default:	/* Fall through. Consider every other value an error. */
			logprintf(LOG_FATAL, "Could not fetch packet. Exiting.\n");
			interrupted = true;
			break;
		}
	}

	dev_close(dev);

	/* Make some checks before exiting */
	do
	{
		logprintf(LOG_ALWAYS, "Keep the pcap file (y/n)? ");
		memset(answer, 0, sizeof(answer));
		fgets(answer, sizeof(answer), stdin);
		if (answer[1] == 0x0a)
			answer[1] = 0;
		if (!strcmp(answer, "y"))
			logprintf(LOG_ALWAYS, "Saving.\n");
		else if (!strcmp(answer, "n"))
			logprintf(LOG_ALWAYS, "Removing.\n");
		else
			logprintf(LOG_ALWAYS, "Please answer 'y' or 'n'.\n");
	} while (answer[0] != 'y' && answer[0] != 'n');
	logprintf(LOG_ALWAYS, "\n");

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
