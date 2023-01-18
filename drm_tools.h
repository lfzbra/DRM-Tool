#ifndef __DRM_TOOLS_H__
#define __DRM_TOOLS_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <poll.h>
#include <pthread.h>
#include <fcntl.h>
#include <time.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <drm/drm.h>
#include <drm/drm_mode.h>
#include </usr/include/xf86drm.h>
#include </usr/include/xf86drmMode.h>
#include <linux/videodev2.h>


#define INFO_LEVEL				4
#define DBG_LEVEL				5
#define ERR_LEVEL				6

static uint32_t log_level;

#define drm_printf(LEVEL, fmt, args...)  \
do {                                      \
	if (LEVEL <= log_level)               \
		printf("(%s:%d): "fmt, __func__, __LINE__, ##args);   \
} while(0)

#define drm_info(fmt, args...)  \
		drm_printf(INFO_LEVEL,"\x1B[36m"fmt"\e[0m", ##args)
#define drm_dbg(fmt, args...)   \
		drm_printf(DBG_LEVEL, "\x1B[33m"fmt"\e[0m", ##args)
#define drm_err(fmt, args...)   \
	    drm_printf(ERR_LEVEL, "\x1B[31m"fmt"\e[0m", ##args)
#define drm_warn(fmt, args...)   \
	    drm_printf(ERR_LEVEL, "\x1B[32m"fmt"\e[0m", ##args)


#endif // __DRM_TOOLS_H__