/*
 * main.h
 *
 *  Created on: May 16, 2015
 *      Author: ajuaristi
 */
#ifndef MAIN_H
#define MAIN_H

typedef int BOOL;
#define TRUE  1
#define FALSE 0

struct options {
	BOOL debug;
	BOOL verbose;
};

/* command line options */
struct options opt;

#endif
