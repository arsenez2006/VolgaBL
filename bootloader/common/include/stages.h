#ifndef COMMON_STAGES_H
#define COMMON_STAGES_H

#include "stages/mbr.h"
#include "stages/ssl.h"
#include "stages/tsl.h"

#define EXTRA_LOAD_ADDRESS SSL_ADDRESS
#define EXTRA_LOAD_SIZE (SSL_SIZE + TSL_SIZE)

#endif // !COMMON_STAGES_H
