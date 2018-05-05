//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
//
//
//
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "log.h"

void memory_init(void);

void memory_done(void);

void memory_malloc_secure(void **new_ptr, uint32_t size, char *info);

void *memory_malloc(uint32_t size, char *info);

void memory_free(void *ptr);

void memory_memoryUsage(FILE *fp);


#endif
