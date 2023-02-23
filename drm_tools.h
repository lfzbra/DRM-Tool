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



#include <inttypes.h>
#include <drm_fourcc.h>


#define INFO_LEVEL				4
#define DBG_LEVEL				5
#define ERR_LEVEL				6

#define argb8888_to_rgb565(color) ({ \
 unsigned int temp = (color); \
 ((temp & 0xF80000UL) >> 8) | \
 ((temp & 0xFC00UL) >> 5) | \
 ((temp & 0xF8UL) >> 3); \
 })

static uint32_t g_cap_fmt;

char *filename;
// char *setcon_str;
// char *setplans_str;
uint32_t con_id;
uint32_t cr_id;
uint32_t pl_id; 


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


struct drm_set_mode {
	__u32 c_mode_width;
	__u32 c_mode_heigth;
	__u32 p_mode_width;
	__u32 p_mode_heigth;
} drm_set_mode;

struct drm_buffer {
	__u16 *fb_base; // void 改为 __u32 ,报段错误，改为 u16 显示正常

	__u32 width;
	__u32 height;
	__u32 stride;
	__u32 size;

	__u32 handle;
	__u32 buf_id;
};

struct drm_device {
	int drm_fd;

	__s32 crtc_id;
	__s32 card_id;
	uint32_t conn_id;

	__u32 bits_per_pixel;
	__u32 bytes_per_pixel;

	drmModeModeInfo mode;
	drmModeCrtc *saved_crtc;

	/* double buffering */
	struct drm_buffer buffers[2];
	__u32 nr_buffer;
	__u32 front_buf;
};

struct crtc {
	drmModeCrtc *crtc;
	drmModeObjectProperties *props;
	drmModePropertyRes **props_info;
	drmModeModeInfo *mode;
};

struct encoder {
	drmModeEncoder *encoder;
};

struct connector {
	drmModeConnector *connector;
	drmModeObjectProperties *props;
	drmModePropertyRes **props_info;
	char *name;
};

struct fb {
	drmModeFB *fb;
};

struct plane {
	drmModePlane *plane;
	drmModeObjectProperties *props;
	drmModePropertyRes **props_info;
};

struct resources {
	drmModeRes *res;
	drmModePlaneRes *plane_res;

	struct crtc *crtcs;
	struct encoder *encoders;
	struct connector *connectors;
	struct fb *fbs;
	struct plane *planes;
};



struct device {
	int fd;

	struct resources *resources;

	struct {
		unsigned int width;
		unsigned int height;

		unsigned int fb_id;
		// struct bo *bo;
		// struct bo *cursor_bo;
	} mode;
};

struct drmtool_device
{
	struct drm_device *drm_dev;
	struct drm_buffer *drm_buff;
	struct device drm_qdev;
};



#endif // __DRM_TOOLS_H__