/*
 * options.c
 *
 *  Created on: Jul 31, 2015
 *      Author: ajuaristi
 */

#include <stdlib.h>
#include "ajws.h"

struct option
{
	char *name;
	void *loc;
	char *defval;
	void (*setter)(struct option *, const char *);
};
struct option_values opt;

static void set_number(struct option *, const char *);
static void set_boolean(struct option *, const char *);

struct option opts[] = {
		{"verbose", &opt.verbose, "0", set_boolean},
		{"debug", &opt.debug, "0", set_boolean}
};

static void
set_number(struct option *opt, const char *val)
{
	if (val)
		*(int *)(opt->loc) = atoi(val);
	else
		set_number(opt, opt->defval);
}

static void
set_boolean(struct option *opt, const char *val)
{
	bool v;

	if (val)
	{
		if (strcasecmp(val, "true") == 0 ||
				strcasecmp(val, "1") == 0)
			v = true;
		else if (strcasecmp(val, "false") == 0 ||
					strcasecmp(val, "0") == 0)
			v = false;

		*(bool *)(opt->loc) = v;
	}
	else
		set_boolean(opt, opt->defval);
}

void
set_opt(const char *name, const char *val)
{
	int i;
	for (i = 0; i < countof(opts); i++)
	{
		if (strcmp(opts[i].name, name) == 0)
			opts[i].setter(&opts[i], val);
	}
}

void
unset_opt(const char *name)
{
	set_opt(name, NULL);
}
