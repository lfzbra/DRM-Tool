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
#define CAM_MASK				0x0001

static uint32_t g_cap_fmt;
static uint32_t log_level;

#define v4l2_printf(LEVEL, fmt, args...)  \
do {                                      \
	if (LEVEL <= log_level)               \
		printf("(%s:%d): "fmt, __func__, __LINE__, ##args);   \
} while(0)

#define v4l2_info(fmt, args...)  \
		v4l2_printf(INFO_LEVEL,"\x1B[36m"fmt"\e[0m", ##args)
#define v4l2_dbg(fmt, args...)   \
		v4l2_printf(DBG_LEVEL, "\x1B[33m"fmt"\e[0m", ##args)
#define v4l2_err(fmt, args...)   \
	    v4l2_printf(ERR_LEVEL, "\x1B[31m"fmt"\e[0m", ##args)
#define v4l2_warn(fmt, args...)   \
	    v4l2_printf(ERR_LEVEL, "\x1B[32m"fmt"\e[0m", ##args)
struct drm_buffer {
	void *fb_base;

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

static void dump_drm_clients(const int dev_num)
{
	char cmd[50];

	sprintf(cmd, "cat /sys/kernel/debug/dri/%d/clients", dev_num);

	printf("========================================================\n");
	system(cmd);
	printf("========================================================\n");
	printf("Please ensure there is no other master client\n");
	printf("========================================================\n");
}

static void printf_help(void)
{
    printf("list helper functions\n");
}

static int input_command(int argc, char *argv[])
{
    int i;

    if(argc < 2)
    {
        printf_help();
        return -1;
    }
    printf("command:");
    for(i = 1; i < argc; i++){
        printf(" %s", *(argv+i));
    }
    printf("\n");
    for(i = 1; i < argc; i++){
        if(strcmp(argv[i], "-help") == 0)
        {
            printf_help();
            return -1;
        } else if(strcmp(argv[i], "-L") == 0)
        {
            printf("List all available\n");
            return -1;
        } else if(strcmp(argv[i], "-M") == 0)
        {
            printf("set mode\n");
        } else
        {
            //printf_help();
            return -1;
        }
        
    }

    return 0;

}


static int adjust(__u32 fourcc)
{
	int bpp;

	switch(fourcc) {
		case V4L2_PIX_FMT_XRGB32:
		case V4L2_PIX_FMT_XBGR32:
		case V4L2_PIX_FMT_ARGB32:
		case V4L2_PIX_FMT_ABGR32:
			bpp = 32;
			break;
		case V4L2_PIX_FMT_RGB565:
			bpp = 16;
			break;
		default:
			bpp = 32;
	}
	return bpp;
}

static void drm_destroy_fb(int fd, int index, struct drm_buffer *buf)
{
	struct drm_mode_destroy_dumb dreq;

	munmap(buf->fb_base, buf->size);
	drmModeRmFB(fd, buf->buf_id);

	memset(&dreq, 0, sizeof(dreq));
	dreq.handle = buf->handle;
	drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
}


static int drm_create_fb(int fd, int index, struct drm_buffer *buf)
{
	struct drm_mode_create_dumb creq;
	struct drm_mode_destroy_dumb dreq;
	struct drm_mode_map_dumb mreq;
	int ret;

	memset(&creq, 0, sizeof(creq));
	creq.width = buf->width;
	creq.height = buf->height;
	creq.bpp = adjust(g_cap_fmt);

	ret = drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
	if (ret < 0) {
		v4l2_err("cannot create dumb buffer[%d]\n", index);
		return ret;
	}

	buf->stride = creq.pitch;
	buf->size = creq.size;
	buf->handle = creq.handle;

	ret = drmModeAddFB(fd, buf->width, buf->height, creq.bpp, creq.bpp,
				buf->stride, buf->handle, &buf->buf_id);

	if (ret < 0) {
		v4l2_err("Add framebuffer (%d) fail\n", index);
		goto destroy_fb;
	}

	memset(&mreq, 0, sizeof(mreq));
	mreq.handle = buf->handle;
	ret = drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
	if (ret) {
		v4l2_err("Map buffer[%d] dump ioctl fail\n", index);
		goto remove_fb;
	}

	buf->fb_base = mmap(0, buf->size, PROT_READ | PROT_WRITE, MAP_SHARED,
							fd, mreq.offset);
	if (buf->fb_base == MAP_FAILED) {
		v4l2_err("Cannot mmap dumb buffer[%d]\n", index);
		goto remove_fb;
	}
	memset(buf->fb_base, 0x55, buf->size);

	return 0;

remove_fb:
	drmModeRmFB(fd, buf->buf_id);
destroy_fb:
	memset(&dreq, 0, sizeof(dreq));
	dreq.handle = buf->handle;
	drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
	v4l2_err("Create DRM buffer[%d] fail\n", index);
	return ret;
}

static int modeset_find_crtc(struct drm_device *drm,
				drmModeRes *res, drmModeConnector *conn)
{
	drmModeEncoder *encoder;
	int drm_fd = drm->drm_fd;
	int crtc_id, j, i;

	for (i = 0; i < conn->count_encoders; i++) {
		encoder = drmModeGetEncoder(drm_fd, conn->encoders[i]);
		if (!encoder) {
			v4l2_err("can't retrieve encoders[%d]\n", i);
			continue;
		}

		for (j = 0; j < res->count_crtcs; j++) {
			if (encoder->possible_crtcs & (1 << j)) {
				crtc_id = res->crtcs[j];
				if (crtc_id > 0) {
					drm->crtc_id = crtc_id;
					drmModeFreeEncoder(encoder);
					return 0;
				}
			}
			crtc_id = -1;
		}

		if (j == res->count_crtcs && crtc_id == -1) {
			v4l2_err("cannot find crtc\n");
			drmModeFreeEncoder(encoder);
			continue;
		}
		drmModeFreeEncoder(encoder);
	}
	v4l2_err("cannot find suitable CRTC for connector[%d]\n", conn->connector_id);
	return -ENOENT;
}

static int modeset_setup_dev(struct drm_device *drm,
				drmModeRes *res, drmModeConnector *conn)
{
	struct drm_buffer *buf = drm->buffers;
	int i, ret;

	ret = modeset_find_crtc(drm, res, conn);
	if (ret < 0)
		return ret;

	memcpy(&drm->mode, &conn->modes[0], sizeof(drm->mode));
	/* Double buffering */
	for (i = 0; i < 2; i++) {
		buf[i].width  = conn->modes[0].hdisplay;
		buf[i].height = conn->modes[0].vdisplay;
		ret = drm_create_fb(drm->drm_fd, i, &buf[i]);
		if (ret < 0) {
			while(i)
				drm_destroy_fb(drm->drm_fd, i - 1, &buf[i-1]);
			return ret;
		}
		v4l2_dbg("DRM bufffer[%d] addr=0x%p size=%d w/h=(%d,%d) buf_id=%d\n",
				 i, buf[i].fb_base, buf[i].size,
				 buf[i].width, buf[i].height, buf[i].buf_id);
	}
	drm->bits_per_pixel = adjust(g_cap_fmt);
	drm->bytes_per_pixel = drm->bits_per_pixel >> 3;
	return 0;
}


static int drm_device_prepare(struct drm_device *drm)
{
	drmModeRes *res;
	drmModeConnector *conn;
	int drm_fd = drm->drm_fd;
	int ret, i;

	ret = drmSetMaster(drm_fd);
	if (ret < 0) {
		dump_drm_clients(drm->card_id);
		return ret;
	}

	res = drmModeGetResources(drm_fd);
	if (res == NULL) {
		v4l2_err("Cannot retrieve DRM resources\n");
		drmDropMaster(drm_fd);
		return -errno;
	}

	/* iterate all connectors */
	for (i = 0; i < res->count_connectors; i++) {
		/* get information for each connector */
		conn = drmModeGetConnector(drm_fd, res->connectors[i]);
		if (!conn) {
			v4l2_err("Cannot retrieve DRM connector %u:%u (%d)\n",
				i, res->connectors[i], errno);
			continue;
		}

		/* valid connector? */
		if (conn->connection != DRM_MODE_CONNECTED ||
					conn->count_modes == 0) {
			drmModeFreeConnector(conn);
			continue;
		}

		/* find a valid connector */
		drm->conn_id = conn->connector_id;
		ret = modeset_setup_dev(drm, res, conn);
		if (ret < 0) {
			v4l2_err("mode setup device environment fail\n");
			drmDropMaster(drm_fd);
			drmModeFreeConnector(conn);
			drmModeFreeResources(res);
			return ret;
		}
		drmModeFreeConnector(conn);
	}
	drmModeFreeResources(res);
	return 0;
}

static int drm_init(struct drm_device *drm)
{
    
    int fd, i, ret;
    uint64_t has_dumb;
    drmModeRes *res;
	drmModeConnector *conn;
    struct drm_buffer *buf;

    drm = malloc(sizeof(*drm));
		if (drm == NULL) {
			printf("alloc DRM device fail\n");
			return -ENOMEM;
		}
	memset(drm, 0, sizeof(*drm));

    drm->drm_fd = open("/dev/dri/card1", O_RDWR | O_CLOEXEC | O_NONBLOCK);
	if (drm->drm_fd < 0) {
		printf("Open /dev/dri/card1 fail\n");
		return -1;
	}

	if (drmGetCap(drm->drm_fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 ||
	    !has_dumb) {
		printf("drm device /dev/dri/card1 does not support dumb buffers\n");
		close(drm->drm_fd);
		return -1;
	}
    printf("Open /dev/dri/card1 success\n");

    ret = drm_device_prepare(drm);
		if (ret < 0) {
			drmDropMaster(drm->drm_fd);
			return ret;
		}

    buf = &drm->buffers[drm->front_buf];
		ret = drmModeSetCrtc(drm->drm_fd, drm->crtc_id, buf->buf_id,
							 0, 0, &drm->conn_id, 1, &drm->mode);
		if (ret < 0) {
			v4l2_err("buffer[%d] set CRTC fail\n", buf->buf_id);
			return ret;
		}

}

int main(int argc, char *argv[])
{   
	struct drm_device *drm;
    input_command(argc, argv);

	if(!drm_init(drm))
	{
		close(drm->drm_fd);
	}else
		close(drm->drm_fd);

    printf("drm_tools is end!\n");
    return 0;
}




