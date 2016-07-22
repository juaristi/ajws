/*
 * options.c - Command-line options.
 *
 *  Add new command line options to the opts[]
 *  structure array.
 *
 *  Created on: Jul 31, 2015
 *      Author: ajuaristi
 */

#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "ajws.h"
#include "alloc.h"

struct ajws_option {
	const char *name;
	char short_opt;
	void *loc;
	const char *defval;
	void (*setter) (struct ajws_option *, const char *);
};

struct option_values opt;

static void set_boolean(struct ajws_option *, const char *);

struct ajws_option opts[] = {
	{"verbose", 'v', &opt.verbose, "0", set_boolean},
	{"debug", 'd', &opt.debug, "0", set_boolean},
	{"help", 'h', &opt.help, "0", set_boolean}
};

static void set_boolean(struct ajws_option *opt, const char *val)
{
	bool v;

	if (val) {
		if (strcasecmp(val, "true") == 0 || strcmp(val, "1") == 0)
			v = true;
		else if (strcasecmp(val, "false") == 0 || strcmp(val, "0") == 0)
			v = false;

		*(bool *) (opt->loc) = v;
	} else {
		if (opt->defval)
			set_boolean(opt, opt->defval);
	}
}

static void set_opt(const char *name, const char *val)
{
	int i;
	for (i = 0; i < countof(opts); i++) {
		if (strcmp(opts[i].name, name) == 0) {
			opts[i].setter(&opts[i], val);
			break;
		}
	}
}

static void set_short_opt(char short_opt, const char *val)
{
	int i;
	for (i = 0; i < countof(opts); i++) {
		if (opts[i].short_opt == short_opt) {
			opts[i].setter(&opts[i], val);
			break;
		}
	}
}

static char *generate_shortopts_string()
{
	char *shortopts = NULL;
	int num_shortopts = 0, i;

	for (i = 0; i < countof(opts); i++) {
		if (opts[i].short_opt) {
			num_shortopts++;
			shortopts =
			    (char *)ec_realloc(shortopts, num_shortopts + 1);
			shortopts[num_shortopts - 1] = opts[i].short_opt;
		}
	}

	shortopts[num_shortopts] = '\0';
	return shortopts;
}

void init_options()
{
	int i;
	for (i = 0; i < countof(opts); i++)
		opts[i].setter(&opts[i], NULL);
}

int parse_options(int argc, char **argv)
{
	struct option long_opts[countof(opts)], *long_opt;
	int i, cur_opt, opt_index = 0;
	const char *shortopts;

	for (i = 0; i < countof(opts); i++) {
		long_opt = &long_opts[i];
		long_opt->name = opts[i].name;
		if (opts[i].setter == set_boolean) {
			long_opt->has_arg = no_argument;
			long_opt->flag = NULL;
			long_opt->val = opts[i].short_opt;
		}
	}
	shortopts = generate_shortopts_string();

	while ((cur_opt =
		getopt_long(argc, argv, shortopts, long_opts,
			    &opt_index)) != -1) {
		if (cur_opt == 0)
			set_opt(long_opts[opt_index].name, "true");
		else if ((cur_opt >= 'a' && cur_opt <= 'z')
			 || (cur_opt >= 'A' && cur_opt <= 'Z'))
			set_short_opt(cur_opt, "true");
		/*switch (cur_opt)
		   {
		   case 0:
		   set_opt(long_opts[opt_index].name, "true");
		   break;
		   case 'v':
		   set_opt("verbose", "true");
		   break;
		   case 'd':
		   set_opt("debug", "true");
		   break;
		   default:
		   break;
		   } */
	}

	ec_free(shortopts);
	return optind;
}
