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

	// for(i = 0 ;i < 20; i++)
	// {
	// 	printf("buff->fb_base[%d] : 0x%x \n", i, buff->fb_base[i]);
	// }

	// if(NULL==(fp = fopen("./te.bin","wb+")))
	// {
	// 	printf("open te.txt error\n");
	// }

	// if((buff->width * buff->height) < fwrite(buff->fb_base, 2 , buff->width * buff->height, fp)){
	// 	printf ("write error \n");
	// } else printf("write succfor!\n");

	// fclose(fp);

	
	return 0;
}

int show_column(struct drmtool_device *device)
{
	int i, num = 0, numflag = 0,ret;
	struct drm_device *dev = device->drm_dev;
	struct drm_buffer *buff = &dev->buffers[0];
	char test_buff[50]={0};
	printf("show_column \n");
	while(1)
	{
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
	// printf("start read------------\n");
	// ret = read(dev->drm_fd,test_buff,sizeof(test_buff));
	// if(ret < 0)
	// 	printf("ret is 0x%x ,read failed\n",ret);

	// for(i=0; i< sizeof(test_buff); i++)
	// {
	// 	printf("test_buf[%d] is 0x%x\n",i , test_buff[i]);
	// }
}


