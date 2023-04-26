#include "drm_mem.h"

#define MMAP_SIZE 0x1000					/* 映射内存大小，通常为一个内存页(4096)整数 */

static int mem_open(void)
{
    int fd;
    fd = open("/dev/mem", O_RDWR | O_NDELAY);
    if(fd < 0)
    {
        printf("open /dev/men failed!\n");
        return -1;

    }
    return fd;
}

int mem_reg(uint32_t reg, uint8_t flag, uint8_t rw, int data)
{
    int fd, i;
    int *reg_base = NULL;
    int *reg1 =NULL;
    int hign_add = 0;
    int low_add = 0;
    int *mem; 
    char *mmap_base = NULL;

    hign_add = reg & 0xfffff000;
    low_add = reg &  0x00000fff;

    fd = mem_open();

    if(flag == 1){
        mmap_base = (char *)mmap(NULL, MMAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, hign_add);

        if(NULL == mmap_base)
        {
            printf("mmap failed!!\n");
            return -1;
        }

        if(rw)
        {   
            mem = (int *)(mmap_base + low_add);
            *mem = data;
            printf("Write --> reg: 0x%x, val: 0x%x\n",mem , data);
        } 
        else
        {
            for(i = 0; i< data; i++)
            {
                reg_base = (int *)(mmap_base + low_add + i);     
                printf("Read --> reg: 0x%x, val: 0x%x\n", reg, *reg_base);
            }
        }
        munmap(mmap_base, MMAP_SIZE); 
    }
    else if (flag ==2)
    {
        ;
    }
        close(fd);
    return 0;

}



