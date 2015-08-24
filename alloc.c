/*
 * alloc.c - Memory management.
 *
 *  This is a thin wrapper around calloc/realloc/free.
 *  We should try to keep it as thin as possible.
 *
 *  Created on: Aug 1, 2015
 *      Author: ajuaristi
 */

#include <stdlib.h>
#include <malloc.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "ajws.h"
#include "alloc.h"
#include "log.h"

struct chunk
{
	struct chunk *prev;
	struct chunk *next;
};
struct chunk *head = NULL, *tail = NULL;

static void
append_chunk(struct chunk *chunk)
{
	if (!head)
	{
		head = chunk;
		tail = chunk;
		chunk->prev = NULL;
		chunk->next = NULL;
	}
	else
	{
		tail->next = chunk;
		chunk->prev = tail;
		tail = chunk;
	}
}

void *
ec_malloc(size_t length)
{
	struct chunk *chk = NULL;
	void *data = NULL;
	void *mem = calloc(1, sizeof(struct chunk) + length);
	if (!unlikely(mem))
	{
		/* This is fatal. We want to exit as soon as possible. */
		logprintf(LOG_FATAL, "Out of memory.\n");
		ec_free_all();
		kill(getpid(), SIGINT);
		exit(EXIT_FAILURE);return NULL;
	}

	chk = (struct chunk *) mem;
	data = mem + sizeof(struct chunk);

	append_chunk(chk);

	return data;
}

void *
ec_realloc(void *ptr, size_t length)
{
	struct chunk *chk = NULL;
	void *mem;

	if (!unlikely(ptr))
		return ec_malloc(length);

	chk = ptr - sizeof(struct chunk);

	mem = realloc(chk, length);
	if (!unlikely(mem))
	{
		logprintf(LOG_FATAL, "Out of memory.\n");
		ec_free_all();
		kill(getpid(), SIGINT);
		exit(EXIT_FAILURE);
	}

	append_chunk((struct chunk *) mem);

	return mem + sizeof(struct chunk);
}

void
ec_free(void *ptr)
{
	struct chunk *chk, *prev, *next;

	/*
	 * We assume the pointer we originally returned from ec_malloc
	 * hasn't changed. This could be dangerous, but we want this function
	 * to run at constant time.
	 * But trying to free() an invalid pointer will horribly segfault anyway,
	 * so this is a safe assumption.
	 */
	chk = ptr - sizeof(struct chunk);

	prev = chk->prev;
	next = chk->next;

	free(chk);

	prev->next = next;
}

void
ec_free_all()
{
	struct chunk *chk = NULL, *next = NULL;
	for (chk = head; chk; chk = next)
	{
		 next = chk->next;
		 free(chk);
	}
}
