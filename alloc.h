/*
 * alloc.h - Definitions for alloc.c.
 *
 *  Created on: Aug 1, 2015
 *      Author: ajuaristi
 */

#ifndef ALLOC_H_
#define ALLOC_H_

#include <sys/types.h>

void *ec_malloc(size_t);
void *ec_realloc(void *, size_t);
void ec_free(void *);
void ec_free_all();

#endif /* ALLOC_H_ */
