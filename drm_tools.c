#include "drm_tools.h"
#include "drm_set.h"
#include "drm_query.h"
#include "drm_mem.h"

#define CAM_MASK				0x0001
#define DEFAULT_LOG            6

uint32_t mem_address = 0;
int mem_data = 0;
struct drm_flags
{
	uint8_t query;
	uint8_t query_e;
	uint8_t query_f;
	uint8_t query_c;
	uint8_t query_p;
	uint8_t query_C;
	uint8_t _help_;
	uint8_t display;
	uint8_t setcon;
	uint8_t setplans;
	uint8_t setion_f;
	uint8_t display_num;
	uint8_t mem_flag;
	uint8_t than_argc;
	uint8_t mode_flag;
} drm_flags = {0};

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
    printf("list helper functions: \n");
	printf("\n");
	printf("========================================================\n");
	printf("  -Q\tQuery options:\n");
	printf("\n");
	printf("\t\t-cc\tlist CRTCs\n");
	printf("\t\t-c\tlist connectors\n");
	printf("\t\t-e\tlist encoders\n");
	printf("\t\t-f\tlist framebuffers\n");
	printf("\t\t-p\tlist planes\n");
	printf("\n");

	printf("--------------------------------------------------------\n");
	printf("  -S\tSettings options:\n");
	printf("\n");
	printf("\t\t-s\tSet connector and CRTC\n");
	printf("\t\t  \t<connector_id> <CRTC_id> <mode_width> <mode_height>\n");
	printf("\t\t-P\tSet planes\n");
	printf("\t\t  \t<planes_id> <plane_width> <plane_height>\n");
	printf("\n");

	printf("--------------------------------------------------------\n");
	printf("  -D\tDisplay options:\n");
	printf("\n");
	printf("\t\t<device>\tuse the given device\n");
	printf("\n");
	printf("--------------------------------------------------------\n");
	printf("  -mm\tDump registers:\n");
	printf("\n");
	printf("\t\t-r\tRead register address,for example,56190000 1\n");
	printf("\t\t  \t<address> <count>\n");
	printf("\t\t-w\tWrite Register address,for example,56190000 1f1f\n");
	printf("\t\t  \t<address> <data>");
	printf("\n");
	printf("========================================================\n");
	printf("\n");

}

static void printf_help_mode(void)
{
	printf("========================================================\n");
    printf("Please input mode:\n");
	printf("\n");
	printf("  -Q\tQuery options:\n");
	printf("\n");
	printf("  -S\tSettings options:\n");
	printf("\n");
	printf("  -D\tDisplay options:\n");
	printf("\n");
	printf("  -mm\tDump registers:\n");
	printf("\n");
	printf("========================================================\n");
}

static void printf_help_query(void)
{
	printf("========================================================\n");
    printf("list Query options:\n");
	printf("\n");
	printf("\t-cc\tlist CRTCs\n");
	printf("\t-c\tlist connectors\n");
	printf("\t-e\tlist encoders\n");
	printf("\t-f\tlist framebuffers\n");
	printf("\t-p\tlist planes\n");
	printf("\n");
	printf("========================================================\n");
}

static void printf_help_s(void)
{
	printf("========================================================\n");
	printf("Please confirm the input parameters!\n");
	printf("\t-s\tSet connector and CRTC\n");
	printf("\t  \t<connector_id> <CRTC_id> <mode_width> <mode_height>\n");
	printf("\n");
	printf("========================================================\n");
}

static void printf_help_P(void)
{
	printf("========================================================\n");
	printf("Please confirm the input parameters!\n");
	printf("\t-P\tSet planes\n");
	printf("\t  \t<planes_id> <plane_width> <plane_height>\n");
	printf("\n");
	printf("========================================================\n");
}

static void printf_help_D(void)
{
	printf("========================================================\n");
	printf("Please confirm the input parameters!\n");
	printf("  -D\tDisplay options:\n");
	printf("\t\t<device>\tuse the given device\n");
	printf("\n");
	printf("========================================================\n");
}

static void printf_help_dump(void)
{
	printf("========================================================\n");
	printf("Dump registers:\n");
	printf("\n");
	printf("\t-r\tRead register address,for example,56190000\n");
	printf("\t  \t<address> <count>\n");
	printf("\t-w\tWrite Register address,for example,56190000 1f1f\n");
	printf("\t  \t<address> <data>");
	printf("\n");
	printf("========================================================\n");
}

static void printf_help_r(void)
{
	printf("========================================================\n");
	printf("Please confirm the input parameters!\n");
	printf("\t-r\tRead register address,for example,56190000 1\n");
	printf("\t  \t<address> <count>\n");
	printf("\n");
	printf("========================================================\n");
}

static void printf_help_w(void)
{
	printf("========================================================\n");
	printf("Please confirm the input parameters!\n");
	printf("\t-w\tWrite Register address,for example,56190000 1f1f\n");
	printf("\t  \t<address> <data>");
	printf("\n");
	printf("========================================================\n");
}

static int input_command(int argc, char *argv[])
{
    int i;

    if(argc < 2)
    {
        printf_help();
        return -1;
    }
	// filename = argv[1];
    printf("command:");
    for(i = 1; i < argc; i++){
        printf(" %s", *(argv+i));
    }
    printf("\n");
    for(i = 1; i < argc; i++){
        if(strcmp(argv[i], "-help") == 0)
        {
            printf_help();
			drm_flags._help_=1;
            return -1;
        } else if(strcmp(argv[i], "-Q") == 0)
        {
            drm_flags.query = 1;
			drm_flags.mode_flag = 1;
            
        } else if(strcmp(argv[i], "-c") == 0)
        {
            drm_flags.query_c = 1;
        } else if(strcmp(argv[i], "-p") == 0)
        {
            drm_flags.query_p = 1;
        } else if(strcmp(argv[i], "-e") == 0)
        {
            drm_flags.query_e = 1;
        } else if(strcmp(argv[i], "-f") == 0)
        {
            drm_flags.query_f = 1;
        } else if(strcmp(argv[i], "-cc") == 0)
        {
            drm_flags.query_C = 1;
        } else if(strcmp(argv[i], "-D") == 0)
        {
			drm_flags.mode_flag = 2;
			if(i>=(argc-1))
			{
				printf_help_D();
				return -1;
			}
            drm_flags.display = 1;
			filename = argv[++i];
			// return 0;
        } else if(strcmp(argv[i], "-s") == 0)
        {
			if(i>=(argc-4))
			{
				printf_help_s();
				return -1;
			}
            drm_flags.setcon = 1;
			//setcon_str = argv[++i];
			con_id = atoi(argv[++i]);
			cr_id = atoi(argv[++i]);
			drm_set_mode.c_mode_width = atoi(argv[++i]);
			drm_set_mode.c_mode_heigth = atoi(argv[++i]);
			//return 0;
        } else if(strcmp(argv[i], "-P") == 0)
        {
			if(i>=(argc-3))
			{
				printf_help_P();
				return -1;
			}
            drm_flags.setplans = 1;
			pl_id = atoi(argv[++i]);
			drm_set_mode.p_mode_width = atoi(argv[++i]);
			drm_set_mode.p_mode_heigth = atoi(argv[++i]);
			// setplans_str = argv[++i];
			
			//return 0;
        } 
		else if(strcmp(argv[i], "-S") == 0)
        {
			drm_flags.mode_flag = 3;
            drm_flags.setion_f = 1;
			//return 0;
        } 
		else if(strcmp(argv[i], "-ra") == 0)
        {
            drm_flags.display_num = 1;
			//return 0;
        } 
		else if(strcmp(argv[i], "-co") == 0)
        {
            drm_flags.display_num = 2;
			//return 0;
        } 
		else if(strcmp(argv[i], "-mm") == 0)
        {
			drm_flags.mode_flag = 4;
			if(i>=(argc-1))
			{
				printf_help_dump();
				return -1;
			}
			if(strcmp(argv[++i], "-r") == 0)
			{
				if(i>=(argc-2))
				{
					printf_help_r();
					return -1;
				}
				sscanf(argv[++i], "%x", &mem_address);
				sscanf(argv[++i], "%x", &mem_data);
				mem_reg(mem_address, 1, 0, mem_data);
			}
			else if (strcmp(argv[i], "-w") == 0)
			{
				if(i>=(argc-2))
				{
					printf_help_w();
					return -1;
				}
				sscanf(argv[++i], "%x", &mem_address);
				sscanf(argv[++i], "%x", &mem_data);
				mem_reg(mem_address, 1, 1,mem_data);
			}
			else 
				printf_help_dump();
			drm_flags.mem_flag = 1;
			return 0;
        } 
		else if(strcmp(argv[i], "-M") == 0)
        {
		   
           mem_reg(mem_address, 2, 1, 1);
		   drm_flags.mem_flag = 1;
			return 0;
        } 
		else
        {
            printf_help();
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
			bpp = 16;
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
	printf("drm_create_fb\n");
	memset(&creq, 0, sizeof(creq));
	creq.width = buf->width;
	creq.height = buf->height;
	creq.bpp = adjust(g_cap_fmt);

	/* handle, pitch, size will be returned */
	ret = drmIoctl(fd, DRM_IOCTL_MODE_CREATE_DUMB, &creq);
	if (ret < 0) {
		drm_err("cannot create dumb buffer[%d]\n", index);
		return ret;
	}

	buf->stride = creq.pitch;
	buf->size = creq.size;
	buf->handle = creq.handle;

	/* bind the dumb-buffer to an FB object */
	ret = drmModeAddFB(fd, buf->width, buf->height, creq.bpp, creq.bpp,
				buf->stride, buf->handle, &buf->buf_id);

	if (ret < 0) {
		drm_err("Add framebuffer (%d) fail\n", index);
		goto destroy_fb;
	}

	/* map the dumb-buffer to userspace */
	memset(&mreq, 0, sizeof(mreq));
	mreq.handle = buf->handle;
	ret = drmIoctl(fd, DRM_IOCTL_MODE_MAP_DUMB, &mreq);
	if (ret) {
		drm_err("Map buffer[%d] dump ioctl fail\n", index);
		goto remove_fb;
	}

	buf->fb_base = mmap(0, buf->size, PROT_READ | PROT_WRITE, MAP_SHARED,
							fd, mreq.offset);
	if (buf->fb_base == MAP_FAILED) {
		drm_err("Cannot mmap dumb buffer[%d]\n", index);
		goto remove_fb;
	}
	memset(buf->fb_base, 0, buf->size);
	return 0;

remove_fb:
	drmModeRmFB(fd, buf->buf_id);
destroy_fb:
	memset(&dreq, 0, sizeof(dreq));
	dreq.handle = buf->handle;
	drmIoctl(fd, DRM_IOCTL_MODE_DESTROY_DUMB, &dreq);
	drm_err("Create DRM buffer[%d] fail\n", index);
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
			drm_err("can't retrieve encoders[%d]\n", i);
			continue;
		}
		drm_dbg("encoder->encoder_id[%d]\n", encoder->encoder_id);

		for (j = 0; j < res->count_crtcs; j++) {
			if (encoder->possible_crtcs & (1 << j)) {
				crtc_id = res->crtcs[j];
				drm_dbg("crtc_id[%d]\n", crtc_id);
				if (crtc_id > 0) {
					drm->crtc_id = crtc_id;
					drmModeFreeEncoder(encoder);
					return 0;
				}
			}
			crtc_id = -1;
		}

		if (j == res->count_crtcs && crtc_id == -1) {
			drm_err("cannot find crtc\n");
			drmModeFreeEncoder(encoder);
			continue;
		}
		drmModeFreeEncoder(encoder);
	}
	drm_err("cannot find suitable CRTC for connector[%d]\n", conn->connector_id);
	return -ENOENT;
}

static int modeset_setup_dev(struct drm_device *drm,
				drmModeRes *res, drmModeConnector *conn)
{
	struct drm_buffer *buf = drm->buffers;
	int i, ret;
	printf("modeset_setup_dev\n");
	ret = modeset_find_crtc(drm, res, conn);// 找到 CRTC id 
	if (ret < 0)
		return ret;

	memcpy(&drm->mode, &conn->modes[0], sizeof(drm->mode));
	/* Double buffering */
	for (i = 0; i < 2; i++) {
		buf[i].width  = conn->modes[0].hdisplay;
		buf[i].height = conn->modes[0].vdisplay;
		ret = drm_create_fb(drm->drm_fd, i, &buf[i]);
		printf("drm_create_fb returned %d\n", ret);
		if (ret < 0) {
			
			while(i)
				drm_destroy_fb(drm->drm_fd, i - 1, &buf[i-1]);
			return ret;
		}
		drm_dbg("DRM bufffer[%d] addr=0x%p size=%d w/h=(%d,%d) buf_id=%d\n",
				 i, buf[i].fb_base, buf[i].size,
				 buf[i].width, buf[i].height, buf[i].buf_id);
	}
	drm->bits_per_pixel = adjust(g_cap_fmt);
	drm->bytes_per_pixel = drm->bits_per_pixel >> 3;
	return 0;
}

static int modeset_set_setup_dev(struct drm_device *drm,
				drmModeRes *res, drmModeConnector *conn)
{
	struct drm_buffer *buf = drm->buffers;
	int i, ret;
	printf("modeset_setup_dev\n");
	ret = modeset_find_crtc(drm, res, conn);// 找到 CRTC id 
	if (ret < 0)
		return ret;

	memcpy(&drm->mode, &conn->modes[0], sizeof(drm->mode));
	/* Double buffering */
	for (i = 0; i < 1; i++) {
		buf[i].width  = drm_set_mode.c_mode_width;
		buf[i].height = drm_set_mode.c_mode_heigth;
		ret = drm_create_fb(drm->drm_fd, i, &buf[i]);
		printf("drm_create_fb returned %d\n", ret);
		if (ret < 0) {
			
			while(i)
				drm_destroy_fb(drm->drm_fd, i - 1, &buf[i-1]);
			return ret;
		}
		drm_dbg("DRM bufffer[%d] addr=0x%p size=%d w/h=(%d,%d) buf_id=%d\n",
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
		drm_err("Cannot retrieve DRM resources\n");
		drmDropMaster(drm_fd);
		return -errno;
	}

	/* iterate all connectors */
	for (i = 0; i < res->count_connectors; i++) {
		/* get information for each connector */
		conn = drmModeGetConnector(drm_fd, res->connectors[i]);
		if (!conn) {
			drm_err("Cannot retrieve DRM connector %u:%u (%d)\n",
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
		printf("modeset_setup_dev returned %d\n", ret);
		if (ret < 0) {
			drm_err("mode setup device environment fail\n");
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

static int drm_set_device_prepare(struct drm_device *drm)
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
		drm_err("Cannot retrieve DRM resources\n");
		drmDropMaster(drm_fd);
		return -errno;
	}

	/* iterate all connectors */
	for (i = 0; i < res->count_connectors; i++) {
		/* get information for each connector */
		conn = drmModeGetConnector(drm_fd, res->connectors[i]);
		if (!conn) {
			drm_err("Cannot retrieve DRM connector %u:%u (%d)\n",
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
		ret = modeset_set_setup_dev(drm, res, conn);
		printf("modeset_setup_dev returned %d\n", ret);
		if (ret < 0) {
			drm_err("mode setup device environment fail\n");
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

static int drm_init(struct drmtool_device *dev)
{
    
    int fd, i, ret;
    uint64_t has_dumb;
    drmModeRes *res;
	drmModeConnector *conn;
	struct drm_device *drm;
	struct drm_buffer *buf;

    drm = malloc(sizeof(*drm));
		if (drm == NULL) {
			printf("alloc DRM device fail\n");
			return -ENOMEM;
		}
	memset(drm, 0, sizeof(*drm));

	dev->drm_dev = drm;

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
			drm_err("buffer[%d] set CRTC fail\n", buf->buf_id);
			return ret;
		}
	printf("show 111>>> \n");
		for(i=0; i< 2; i++) {
			memset(buf->fb_base, 0, buf->size);
			sleep(1);
			memset(buf->fb_base, 0xff, buf->size);
			sleep(1);
			memset(buf->fb_base, 0, buf->size);
			sleep(1);
			memset(buf->fb_base, 0xff, buf->size);
			sleep(1);
			memset(buf->fb_base, 0, buf->size);
			sleep(1);
		}
		drmDropMaster(drm->drm_fd);
		drm_destroy_fb(drm->drm_fd, 0, &drm->buffers[0]);
		drm_destroy_fb(drm->drm_fd, 0, &drm->buffers[1]);
		close(drm->drm_fd);
		free(drm);
	
	return 0;

}

static int drm_malloc(struct drmtool_device *dev)
{
	struct drm_device *drm;

	drm = malloc(sizeof(*drm));
		if (drm == NULL) {
			printf("alloc DRM device fail\n");
			return -ENOMEM;
		}
	memset(drm, 0, sizeof(*drm));

	dev->drm_dev = drm;

	return 0;
}

static int drm_open(struct drmtool_device *dev)
{
	struct drm_device *drm = dev->drm_dev;
	uint64_t has_dumb;

	drm->drm_fd = open(filename, O_RDWR | O_CLOEXEC | O_NONBLOCK);
	// drm->drm_fd = drmOpen("imx-drm", NULL);
	if (drm->drm_fd < 0) {
		printf("Open %s fail\n", filename);
		return -1;
	}

	if (drmGetCap(drm->drm_fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 ||
	    !has_dumb) {
		printf("drm device %s does not support dumb buffers\n",filename);
		close(drm->drm_fd);
		return -1;
	}
    printf("Open %s success\n",filename);

	return 0;

}

static int drm_set_open(struct drmtool_device *dev)
{
	struct drm_device *drm = dev->drm_dev;
	uint64_t has_dumb;
	printf("drm_set_open\n");
	//drm->drm_fd = open(filename, O_RDWR | O_CLOEXEC | O_NONBLOCK);
	 drm->drm_fd = drmOpen("imx-drm", NULL);
	if (drm->drm_fd < 0) {
		printf("Open imx-drm fail\n");
		return -1;
	}

	if (drmGetCap(drm->drm_fd, DRM_CAP_DUMB_BUFFER, &has_dumb) < 0 ||
	    !has_dumb) {
		printf("drm device imx-drm does not support dumb buffers\n");
		close(drm->drm_fd);
		return -1;
	}
    printf("Open imx-drm success\n");

	return 0;

}

static int drm_prepare(struct drmtool_device *dev)
{
	int ret;
	struct drm_device *drm = dev->drm_dev;

	ret = drm_device_prepare(drm);
	if (ret < 0) {
		drmDropMaster(drm->drm_fd);
		return ret;
	}

	return 0;

}

static int drm_set_prepare(struct drmtool_device *dev)
{
	int ret;
	struct drm_device *drm = dev->drm_dev;

	ret = drm_set_device_prepare(drm);
	if (ret < 0) {
		drmDropMaster(drm->drm_fd);
		return ret;
	}

	return 0;

}

static int drm_start(struct drmtool_device *dev)
{
	int ret,i;
	struct drm_device *drm = dev->drm_dev;
	struct drm_buffer *buf;
		
	buf = &drm->buffers[drm->front_buf];
	ret = drmModeSetCrtc(drm->drm_fd, drm->crtc_id, buf->buf_id,
							 0, 0, &drm->conn_id, 1, &drm->mode);
	if (ret < 0) {
		drm_err("buffer[%d] set CRTC fail\n", buf->buf_id);
		return ret;
	}
	printf("show 222>>> \n");
		for(i=0; i< 1; i++) {
			memset(buf->fb_base, 0, buf->size);
			sleep(1);
			memset(buf->fb_base, 0x99, buf->size);
			sleep(1);
			memset(buf->fb_base, 0, buf->size);
		}

	return 0;
}

static int drm_set_start(struct drmtool_device *dev)
{
	int ret,i;
	struct drm_device *drm = dev->drm_dev;
	struct drm_buffer *buf;
	unsigned int w = drm_set_mode.p_mode_width;
	unsigned int h = drm_set_mode.p_mode_heigth;
	printf("con_id = %d , cr_id = %d ,w = %d, h = %d,\n",con_id, cr_id, w, h);
	buf = &drm->buffers[drm->front_buf];
	ret = drmModeSetCrtc(drm->drm_fd, cr_id, buf->buf_id,
							 0, 0, &con_id, 1, &drm->mode);
	if (ret < 0) {
		drm_err("buffer[%d] set CRTC fail\n", buf->buf_id);
		return ret;
	}
	if(drm_flags.setplans){
		ret = drmModeSetPlane(drm->drm_fd, pl_id, cr_id, buf->buf_id, 0,
				(buf->width - w)/2, (buf->height - h)/2, w, h,
				0 << 16, 0 << 16, w << 16, h << 16);
		if(ret < 0)
			printf("set plane is failed!\n");
		printf("(buf->width - w)/2 = %d ,(buf->height - w)/2 =%d \n",(buf->width - w)/2,(buf->height - h)/2);
	}
	printf("show 222>>> \n");
		for(i=0; i< 1; i++) {
			memset(buf->fb_base, 0, buf->size);
			sleep(1);
			memset(buf->fb_base, 0x99, buf->size);
			sleep(1);
			memset(buf->fb_base, 0, buf->size);
		}

	return 0;
}

static int drm_show_blink(struct drmtool_device *dev)
{
	int i;
	
	struct drm_device *drm = dev->drm_dev;
	struct drm_buffer *buff = &drm->buffers[drm->front_buf];
	for (i = 0; i < 1; i++) {
		memset(buff->fb_base, 0, buff->size);
		sleep(1);
		memset(buff->fb_base, 0x55, buff->size);
		sleep(1);
		memset(buff->fb_base, 0, buff->size);
		sleep(1);
		memset(buff->fb_base, 0x55, buff->size);
	}
	drm_dbg(" twe memset \n");
	printf("buff->size = %d, buff->width * buff->height / 2= %d buff->h: %d\n",buff->size, buff->width * buff->height / 2, buff->height);
	for(i = 0; i < buff->width * buff->height; i++)
	{
		// buff->fb_base[i] = argb8888_to_rgb565(0xff0000);
		if(i < 691200)
		{
			buff->fb_base[i] = argb8888_to_rgb565(0xff0000);
		}
		if(691200 <= i && i< 1382400)
		{
			buff->fb_base[i] = argb8888_to_rgb565(0xff00);
		}
		if(i>= 1382400)
			buff->fb_base[i] = argb8888_to_rgb565(0xff);
	}
	return 0;
}

void drm_show_all(struct drmtool_device *dev, uint8_t num)
{
	switch(num){
		case 0: 
			drm_show_blink(dev);
			break;
		case 1:
			show_rail(dev);
			break;
		case 2:
			show_column(dev);
			break;
		default:
			break;
	}
}

int main(int argc, char *argv[])
{   
	int i,ret;
	int encoders, connectors, crtcs, planes, framebuffers;
	struct drmtool_device dev;
	char *device = NULL ;
	char *module = NULL ;
	struct pipe_arg *pipe_args = NULL;
	struct plane_arg *plane_args = NULL;
	
	log_level = DEFAULT_LOG;
	// mem_reg(1);
	memset(&dev,0,sizeof(dev));
	if(input_command(argc, argv))
		return 0;
	if(drm_flags.mode_flag == 1)	//操作 DRM 框架节点, DUMP 框架资源
	{
		printf("drm_flags.display is 0\n");
		dev.drm_qdev.fd= mod_open(device, module); //打开 DRM 节点
		dev.drm_qdev.resources = get_resources(&dev.drm_qdev); //获取节点资源
		if(!dev.drm_qdev.resources){
			drmClose(dev.drm_qdev.fd);
			return 1;
		}
		if(drm_flags.query)
		{	
			if(drm_flags.query_e)
				dump_encoders(&dev.drm_qdev); //显示 encoders 信息
			if(drm_flags.query_c)
				dump_connectors(&dev.drm_qdev); //显示 connectors 信息
			if(drm_flags.query_C)
				dump_crtcs(&dev.drm_qdev);	//显示 CRTCS 信息
			if(drm_flags.query_p)
				dump_planes(&dev.drm_qdev);	// 显示 planes 信息
			if(drm_flags.query_f)
				dump_framebuffers(&dev.drm_qdev);	//显示 framebuffers 信息
			if(!drm_flags.query_e && !drm_flags.query_c && !drm_flags.query_C && !drm_flags.query_p && !drm_flags.query_f)
				printf_help_query();
		}

		free_resources(dev.drm_qdev.resources);
	} 
	else if (drm_flags.mode_flag == 2) // 操作设备节点，显示图像
	{	
		printf("drm_flags.mode_flag is 3\n");
		
		ret = drm_malloc(&dev);
		if (ret < 0) {
			printf("No enough memory\n");
			return -ENOMEM;
		}

		ret = drm_open(&dev);
		if (ret < 0)
			goto free;
		ret = drm_prepare(&dev);
		if (ret < 0)
			goto close;

		ret = drm_start(&dev);
		if (ret < 0)
			goto cleanup;

		printf("display_num = %d \n",drm_flags.display_num);
		drm_show_all(&dev, drm_flags.display_num);
		getchar();
		printf("drm tool app successfully! \n");		

	cleanup:
		drmDropMaster(dev.drm_dev->drm_fd);
		drm_destroy_fb(dev.drm_dev->drm_fd, 0, &dev.drm_dev->buffers[0]);
		drm_destroy_fb(dev.drm_dev->drm_fd, 0, &dev.drm_dev->buffers[1]);

	close:
		close(dev.drm_dev->drm_fd);

	free:
		free(dev.drm_dev);
	} 
	else if(drm_flags.mode_flag == 3)  // 通过配置 connector、CRTCS 和 planes 显示图像
	{
		printf("drm_flags.mode_flag is 3\n");
		ret = drm_malloc(&dev);
		if (ret < 0) {
			printf("No enough memory\n");
			return -ENOMEM;
		}

		ret = drm_set_open(&dev);
		if (ret < 0)
			goto free1;
		ret = drm_set_prepare(&dev);
		if (ret < 0)
			goto close1;

		ret = drm_set_start(&dev);
		if (ret < 0)
			goto cleanup1;

		drm_show_all(&dev, drm_flags.display_num);

		getchar();
		printf("drm tool app successfully! \n");		

	cleanup1:
		drmDropMaster(dev.drm_dev->drm_fd);
		drm_destroy_fb(dev.drm_dev->drm_fd, 0, &dev.drm_dev->buffers[0]);
		drm_destroy_fb(dev.drm_dev->drm_fd, 0, &dev.drm_dev->buffers[1]);

	close1:
		close(dev.drm_dev->drm_fd);

	free1:
		free(dev.drm_dev);
	}
	else if (drm_flags.mode_flag == 4)
	{
		;
	}
	else
	{
		printf_help_mode();
	}
    printf("drm_tools is end!\n");
    return 0;
}

