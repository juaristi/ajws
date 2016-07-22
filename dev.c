/*
 * dev.c
 *
 *  Created on: Jul 30, 2015
 *      Author: ajuaristi
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include "dev.h"
#include "alloc.h"

typedef bool(*func) (struct ifaddrs *, ajws_dev_t *);

static u_char read_hex_number(FILE * f)
{
	u_char num = fgetc(f);
	if (num >= '0' && num <= '9') {
		num = (num - '0');
	} else {
		if (num >= 'A' && num <= 'F')
			num = tolower(num);
		if (num >= 'a' && num <= 'f')
			num = (num - 'a') + 10;
	}
	return num;
}

static bool dev_get_mac_addr(ajws_dev_t * dev)
{
	int i;
	char prefix[] = "/sys/class/net/", suffix[] = "/address";
#define DEV_NAME_LEN (sizeof(prefix) + strlen(dev->name) + sizeof(suffix))
	char *path = (char *)ec_malloc(DEV_NAME_LEN);

	strcpy(path, prefix);
	strcat(path, dev->name);
	strcat(path, suffix);

	FILE *f = fopen(path, "r");
	if (!f)
		return false;

	for (i = 0; i < MAC_ADDR_LEN; i++) {
		dev->mac_addr[i] =
		    (read_hex_number(f) << 4) + read_hex_number(f);

		/* Skip the semicolon */
		read_hex_number(f);
	}

	fclose(f);
	return true;
}

static bool __dev_cmp_ipaddr(struct ifaddrs *iface, ajws_dev_t * dev)
{
	in_addr_t ipaddrs[] = {
		((struct sockaddr_in *)iface->ifa_addr)->sin_addr.s_addr,
		dev->addr.sin_addr.s_addr
	};

	if (ipaddrs[0] == ipaddrs[1]) {
		dev->name = (char *)ec_malloc(strlen(iface->ifa_name) + 1);
		strcpy(dev->name, iface->ifa_name);
		return true;
	}

	return false;
}

static bool __dev_cmp_name(struct ifaddrs *iface, ajws_dev_t * dev)
{
	if (strcmp(iface->ifa_name, dev->name) == 0) {
		memcpy(&dev->addr, iface->ifa_addr, sizeof(dev->addr));
		return true;
	}

	return false;
}

static bool dev_find_iface(ajws_dev_t * dev, func f)
{
	struct ifaddrs *ifaces, *iface;
	bool result = false;

	if (getifaddrs(&ifaces) == -1)
		return false;

	for (iface = ifaces; iface && !result; iface = iface->ifa_next) {
		/*
		 * We assume IP address is an IPv4 (AF_INET).
		 * We don't support IPv6 yet. Patience!
		 */
		if (iface->ifa_addr->sa_family == AF_INET) {
			result = f(iface, dev);
			if (result)
				dev->addr.sin_family = AF_INET;
		}
	}

	if (result) {
		/* We got the interface name. Now get the MAC and we're done. */
		result = dev_get_mac_addr(dev);
	}

	freeifaddrs(ifaces);
	return result;
}

struct ajws_iface *iface_find_by_ipaddr_dotted(const char *ipaddr_dotted)
{
	struct in_addr ipaddr;
	struct ajws_iface *iface;

	if (!ipaddr_dotted || inet_aton(ipaddr_dotted, &ipaddr) == 0)
		goto end;

	iface = iface_find_by_ipaddr(&ipaddr);

end:
	return iface;
}

struct ajws_iface *iface_find_by_ipaddr(struct in_addr *ipaddr)
{
	if (!ipaddr)
		return NULL;

	return dev_find_iface(dev, __dev_cmp_ipaddr);
}

struct ajws_iface *iface_find_by_name(const char *name)
{
	if (!name)
		return NULL;

	return dev_find_iface(dev, __dev_cmp_name);
}
