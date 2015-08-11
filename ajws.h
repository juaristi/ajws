/*
 * main.h
 *
 *  Created on: May 16, 2015
 *      Author: ajuaristi
 */

#ifndef AJWS_H
#define AJWS_H

typedef _Bool bool;
#define true  1
#define false 0

struct option_values
{
	bool verbose;
	bool debug;
};

/* Command line options */
extern struct option_values opt;

void init_options();
void parse_options(int argc, char **argv);

#define OPT(val) (opt.val)
#define OPT_INT(val) ((int) OPT(val))
#define OPT_BOOL(val) ((bool) OPT(val))

#define countof(o) (sizeof(o)/sizeof(o[0]))

#define likely(x)	__builtin_expect((x),1)
#define unlikely(x)	__builtin_expect((x),0)

#endif
