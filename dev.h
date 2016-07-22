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
typedef struct packet_info info_t;

#define MAC_ADDR_LEN 6
struct ajws_dev {
	char *name;
	u_char mac_addr[MAC_ADDR_LEN];
	struct sockaddr_in addr;
	void *priv;
};
typedef struct ajws_dev ajws_dev_t;

/*
 * Open the device for capturing packets.
 * The target device should be able to capture at least incoming packets.
 * Return true if the above conditions were met, or false otherwise.
 */
bool dev_open(ajws_dev_t *);
/*
 * Close the device.
 * After closing the device, a call to 'dev_poll' on that device
 * should fail. Closing an already closed device is a no-op.
 */
void dev_close(ajws_dev_t *);
/*
 * Interrupt a currently running packet capture process,
 * previously launched via a call to dev_poll.
 */
void dev_interrupt(ajws_dev_t *);
/*
 * Poll a packet from the device.
 * Capture a packet from the device and remove it from the
 * underlying buffer. This function should be non-blocking.
 * Returns 0 if no packets were captured, 1 if a packet was captured
 * and -1 if there was a fatal error.
 */
int dev_poll(ajws_dev_t *, u_char *, u_int, info_t *);

/*
 * Fill the given 'ajws_dev_t' structure with info about
 * the selected device. When calling the function, at least the
 * IP address field should be filled in the structure. This function
 * will fill the structure with the device name and the MAC address
 * of the device such IP address belongs to.
 * Returns false if no device was found that met the given conditions,
 * or true on success.
 */
bool dev_find_iface_by_ipaddr(ajws_dev_t *);
bool dev_find_iface_by_name(ajws_dev_t *);

#endif				/* DEV_H_ */
