/*
 * pcap.c
 *
 *  Created on: Jul 15, 2015
 *      Author: ajuaristi
 */

#include "ajws.h"
#include <pcap/pcap.h>
#include <string.h>
#include <malloc.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include "log.h"
#include "dev.h"

static struct sockaddr *get_ip_addr(pcap_if_t * dev)
{
	struct sockaddr *addr = NULL;
	struct sockaddr_in *addr_in;
	struct pcap_addr *cur_addr = NULL;

	for (cur_addr = dev->addresses; cur_addr && (!addr);
	     cur_addr = cur_addr->next) {
		if (cur_addr->addr->sa_family == AF_INET) {
			addr_in = (struct sockaddr_in *)cur_addr->addr;
			logprintf(LOG_VERBOSE, "Got IP: %s\n",
				  inet_ntoa(addr_in->sin_addr));
			addr =
			    (struct sockaddr *)malloc(sizeof(struct sockaddr));
			if (addr)
				memcpy(addr, cur_addr->addr,
				       sizeof(struct sockaddr));
		}
	}

	return addr;
}

bool dev_open(ajws_dev_t * dev)
{
	bool success = false;

	pcap_t *pcap;
	pcap_if_t *devs;
	pcap_if_t *cur_dev;

	struct sockaddr *ipaddr;
	char err[PCAP_ERRBUF_SIZE];

	pcap = pcap_create(dev->name, err);
	if (pcap == NULL) {
		logprintf(LOG_FATAL, "Could not open device '%s': %s\n",
			  dev->name, err);
		goto end;
	}

	pcap_setdirection(pcap, PCAP_D_IN);
	pcap_set_snaplen(pcap, 128);

	if (pcap_activate(pcap)) {
		/* TODO we could do a fine-grained error checking here */
		logprintf(LOG_FATAL,
			  "Could not enable packet capture engine.\n");
		goto end;
	}

	/* Don't lose our pcap handle */
	dev->priv = (void *)pcap;

	success = true;

 end:
	return success;
}

void dev_close(ajws_dev_t * dev)
{
	pcap_t *pcap = (pcap_t *) dev->priv;
	pcap_close(pcap);
}

void dev_interrupt(ajws_dev_t * dev)
{
	pcap_t *pcap = (pcap_t *) dev->priv;
	pcap_breakloop(pcap);
}

int dev_poll(ajws_dev_t * dev, u_char * buf, u_int len, info_t * pi)
{
	struct pcap_pkthdr *hdr;
	const u_char *pkt_data;
	int result = 0;
	pcap_t *pcap = (pcap_t *) dev->priv;

	result = pcap_next_ex(pcap, &hdr, &pkt_data);
	if (result == 1) {
		memcpy(buf, pkt_data, (hdr->caplen < len ? hdr->caplen : len));
		if (pi && hdr) {
			memcpy(&pi->ts, &hdr->ts, sizeof(struct timeval));
			pi->caplen = hdr->caplen;
			pi->len = hdr->len;
		}
	}

	return result;
}
