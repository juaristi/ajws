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

/*
 * Open the device for capturing packets.
 * The target device should be able to capture at least incoming packets.
 * Return true if the above conditions were met, or false otherwise.
 */
bool dev_open(pajws_dev_t);
/*
 * Close the device.
 * After closing the device, a call to 'dev_poll' on that device
 * should fail. Closing an already closed device is a no-op.
 */
void dev_close(pajws_dev_t);
/*
 * Interrupt a currently running packet capture process,
 * previously launched via a call to dev_poll.
 */
void dev_interrupt(pajws_dev_t);
/*
 * Poll a packet from the device.
 * Capture a packet from the device and remove it from the
 * underlying buffer. This function should be non-blocking.
 * Returns 0 if no packets were captured, 1 if a packet was captured
 * and -1 if there was a fatal error.
 */
int dev_poll(pajws_dev_t, u_char *, u_int, pinfo_t);

/*
 * Fill the given 'ajws_dev_t' structure with info about
 * the selected device. When calling the function, at least the
 * IP address field should be filled in the structure. This function
 * will fill the structure with the device name and the MAC address
 * of the device such IP address belongs to.
 * Returns false if no device was found that met the given conditions,
 * or true on success.
 */
bool dev_find_iface(pajws_dev_t);

#endif /* DEV_H_ */
