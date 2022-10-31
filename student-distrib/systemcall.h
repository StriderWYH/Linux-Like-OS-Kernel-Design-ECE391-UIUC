#ifndef _SYSTEMCALL_H
#define _SYSTEMCALL_H

#include "types.h"

#define ARG_NUM 100
#define FNAME_SIZE 32
#define BUFSIZE 4

int32_t execute(const uint8_t* command);

#endif

