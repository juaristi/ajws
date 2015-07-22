/*
 * log.c
 *
 *  Created on: May 16, 2015
 *      Author: ajuaristi
 */

#include <ajws.h>
#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include "log.h"

void logprintf(int level, const char *fmt, ...)
{
	switch (level)
	{
	case LOG_ALWAYS:
		break;
	case LOG_DEBUG:
		if (!opt.debug)
			return;
		break;
	case LOG_VERBOSE:
		if (!opt.verbose)
			return;
		break;
	case LOG_FATAL:
		printf("ERROR - ");
		break;
	default:
		return;
	}

	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}

void hexlog(const char *hexstr, int len)
{
	const char *start, *end, *curpos;
	int written, written_ttl;
	int round, padding;

	/* Hexdumps are only supposed to be used for debugging. */
	if (!opt.debug)
		return;

	start = hexstr;
	written = 0;
	written_ttl = 0;

	do
	{
		end = (len - written_ttl > HEXLOG_MAX_LEN) ? start + HEXLOG_MAX_LEN : start + (len - written_ttl);

		for (round = 0; round < 2; round++)
		{
			for (curpos = start; curpos != end; curpos++)
			{
				if (round == 0)
				{
					printf("%.2hhx ", (char) *curpos);
					written++;
					written_ttl++;
				}
				else if (round == 1)
				{
					if (*curpos >= 32 && *curpos <= 126)
						printf("%c", *curpos);
					else
						printf(".");
				}
			}

			if (round == 0)
			{
				if (written < HEXLOG_MAX_LEN)
				{
					for (padding = 0; padding < ((HEXLOG_MAX_LEN - written) * 3); padding++)
						printf(" ");
				}
				printf("| ");
			}
		}

		written = 0;
		printf("\n");

		if (written_ttl < len)
			start = end;
	} while (written_ttl < len);
}
