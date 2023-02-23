#include "drm_set.h"

int show_rail(struct drmtool_device *device)
{
	int i , num = 0, numflag = 0;
	struct drm_device *dev = device->drm_dev;
	struct drm_buffer *buff = &dev->buffers[0];
	FILE *fp = NULL;

	for(i = 0; i <  buff->width * buff->height; i++)
	{
		num++;
		if(num >= 86400)
		{
			num = 0;
			numflag ++;
		}
		buff->fb_base[i] = argb8888_to_rgb565(0xff0000 >> numflag);
	}

	for(i = 0 ;i < 20; i++)
	{
		printf("buff->fb_base[%d] : 0x%x \n", i, buff->fb_base[i]);
	}

	if(NULL==(fp = fopen("./te.bin","wb+")))
	{
		printf("open te.txt error\n");
	}

	if((buff->width * buff->height) < fwrite(buff->fb_base, 2 , buff->width * buff->height, fp)){
		printf ("write error \n");
	} else printf("write succfor!\n");

	fclose(fp);

	
	return 0;
}

int show_column(struct drmtool_device *device)
{
	int i, num = 0, numflag = 0;
	struct drm_device *dev = device->drm_dev;
	struct drm_buffer *buff = &dev->buffers[0];

	for(i = 0; i< buff->width * buff->height; i++)
	{
		num++;
		if(num >= 20520)
		{
			num = 0;
			numflag += 655;
		}
		buff->fb_base[i] = numflag;
	}
}


