/*
 * log.h
 *
 *  Created on: May 16, 2015
 *      Author: ajuaristi
 */

#ifndef LOG_H
#define LOG_H

enum {
	LOG_ALWAYS,
	LOG_DEBUG,
	LOG_VERBOSE,
	LOG_FATAL
};

void logprintf(int, const char *, ...);

#define HEXLOG_MAX_LEN 16
void hexlog(const char *, int);

#endif
