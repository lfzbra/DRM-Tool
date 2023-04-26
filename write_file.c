#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define argb8888_to_rgb565(color) ({ \
 unsigned int temp = (color); \
 ((temp & 0xF80000UL) >> 8) | \
 ((temp & 0xFC00UL) >> 5) | \
 ((temp & 0xF8UL) >> 3); \
 })

int main(void)
{
    int con =0x9898;
    int n = con == 0;
    FILE *fp = NULL;
    char buff[]="andrew liu fu";
    int buff1[]={98,0xfff,10};
    // buff1[1] = buff1[1] + 0x30;
    //string str = (string) buff1[0];

    if(NULL==(fp = fopen("2.txt","w")))
    {
        printf("open error! \n");
    }

    if(sizeof(buff)> fwrite(buff, 1, sizeof(buff), fp)){
        printf("fwrite error\n");
        fclose(fp);
    }
     if(sizeof(buff1)> fwrite(buff1, 1, sizeof(buff1), fp)){
        printf("fwrite error\n");
        fclose(fp);
    }

    
    printf("con: %c , n: %d \n",con, n);
    con = argb8888_to_rgb565(0xff0000);
    printf("con = 0x%x \n",con);
    return 0;
}
