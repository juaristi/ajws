/*
 * dev.c
 *
 *  Created on: Jul 30, 2015
 *      Author: ajuaristi
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include "dev.h"

static u_char
read_hex_number(FILE *f)
{
	u_char num = fgetc(f);
	if (num >= '0' && num <= '9')
		num = (num - '0');
	else
	{
		if (num >= 'A' && num <= 'F')
			num = tolower(num);
		if (num >= 'a' && num <= 'f')
			num = (num - 'a') + 10;
	}
	return num;
}

static bool
dev_get_mac_addr(pajws_dev_t dev)
{
	int i, j;
	char ch;
	char prefix[] = "/sys/class/net/", suffix[] = "/address";
	char *path = (char *) malloc (sizeof(prefix) + strlen(dev->name) + sizeof(suffix));

	if (!path)
		return false;

	strcpy(path, prefix);
	strcat(path, dev->name);
	strcat(path, suffix);

	FILE *f = fopen(path, "r");
	if (!f)
		return 0;

	for (i = 0; i < 6; i++)
	{
		dev->mac_addr[i] = (read_hex_number(f) << 4) + read_hex_number(f);

		/* Skip the semicolon */
		read_hex_number(f);
	}

	fclose(f);
	return true;
}

bool
dev_find_iface(pajws_dev_t dev)
{
	struct ifaddrs *ifaces, *iface;
	struct sockaddr_in *ip;
	bool result = false;

	if (getifaddrs(&ifaces) == -1)
		return false;

	for (iface = ifaces; iface && !result; iface = iface->ifa_next)
	{
		/*
		 * We assume IP address is an IPv4 (AF_INET).
		 * We don't support IPv6 yet. Patience!
		 */
		if (iface->ifa_addr->sa_family == AF_INET)
		{
			ip = (struct sockaddr_in *) iface->ifa_addr;
			if (strcmp(inet_ntoa(ip->sin_addr), dev->ip_addr) == 0)
			{
				/* We found a matching interface. Copy its name and exit. */
				dev->name = (char *) malloc(strlen(iface->ifa_name));
				if (dev->name)
				{
					strcpy(dev->name, iface->ifa_name);
					result = true;
				}
				else
					return false;
			}
		}
	}

	/* We got the interface name. Now get the MAC and we're done. */
	return dev_get_mac_addr(dev);
}
