#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <linux/spi/spidev.h>

#define SPI_DEV_PATH "/dev/spidev0.0"

/*SPI 接收 、发送 缓冲区*/
unsigned char tx_buffer[200] = "hello the world ! andrew test 123456789 abxdefghigklmmaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbccccccdddddd";
unsigned char rx_buffer[200];


int fd;                  					// SPI 控制引脚的设备文件描述符
static unsigned  mode = SPI_MODE_2;         //用于保存 SPI 工作模式
static uint8_t bits = 8;        			// 接收、发送数据位数
static uint32_t speed = 10000000; 			// 发送速度
static uint16_t delay;          			//保存延时时间

void transfer(int fd, uint8_t const *tx, uint8_t const *rx, size_t len)
{
    int ret;

    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = len,
        .delay_usecs = delay,
        .speed_hz = speed,
        .bits_per_word = bits,
        .tx_nbits = 1,
        .rx_nbits = 1
    };

    ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
    
    if (ret < 1)
        printf("can't send spi message\n");
}

void spi_init(void)
{
    int ret = 0;
    //打开 SPI 设备
    fd = open(SPI_DEV_PATH, O_RDWR);
    if (fd < 0)
        printf("can't open %s\n",SPI_DEV_PATH);

    //spi mode 设置SPI 工作模式
    ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);
    if (ret == -1)
        printf("can't set spi mode\n");

    //bits per word  设置一个字节的位数
    ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    if (ret == -1)
        printf("can't set bits per word\n");

    //max speed hz  设置SPI 最高工作频率
    ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    if (ret == -1)
        printf("can't set max speed hz\n");

    //打印
    printf("spi mode: 0x%x\n", mode);
    printf("bits per word: %d\n", bits);
    printf("max speed: %d Hz (%d KHz)\n", speed, speed / 1000);
}

int main(int argc, char *argv[])
{
    
    /*初始化SPI */
    spi_init();

    while(1){
        /*执行发送*/
        transfer(fd, tx_buffer, rx_buffer, sizeof(tx_buffer));

        /*打印 tx_buffer 和 rx_buffer*/
        printf("tx_buffer: \n %s\n ", tx_buffer);
        printf("rx_buffer: \n %s\n ", rx_buffer);
    }
    close(fd);
    return 0;
}
