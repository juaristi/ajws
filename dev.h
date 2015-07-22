/*
 * dev.h
 *
 *  Created on: Jul 14, 2015
 *      Author: ajuaristi
 */

#ifndef DEV_H_
#define DEV_H_

#include <sys/socket.h>
#include <sys/types.h>
#include "ajws.h"

struct packet_info {
	u_int32_t caplen;
	u_int32_t len;
	struct timeval ts;
};
typedef struct packet_info info_t, *pinfo_t;

struct ajws_dev {
	char *name;
	u_char mac_addr[6];
	struct sockaddr ip_addr;
	void *priv;
};
typedef struct ajws_dev ajws_dev_t, *pajws_dev_t;

bool dev_open(pajws_dev_t, const char *);
void dev_close(pajws_dev_t);
int dev_poll(pajws_dev_t, u_char *, u_int, pinfo_t);

#endif /* DEV_H_ */
