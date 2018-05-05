
#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include <stdlib.h>

int log_init(char *);

FILE *log_get(void);

void log_done(void);

#endif
