/*
 * main.h
 *
 *  Created on: May 16, 2015
 *      Author: ajuaristi
 */

#ifndef MAIN_H
#define MAIN_H

typedef _Bool bool;
#define true  1
#define false 0

struct options {
	bool debug;
	bool verbose;
};

/* command line options */
struct options opt;

#endif