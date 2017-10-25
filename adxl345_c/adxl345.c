#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define ADDRESS 0x53

#define DR           0x0B
#define DEVID        0x00
#define OFSX         0x1E
#define OFSY         0x1F
#define OFSZ         0x20
#define DUR          0x21
#define LATENT       0x22
#define WINDOW       0x23
#define INT_ENABLE   0x2E
#define BW_RATE      0x2C
#define POWER_CTL    0x2D
#define DATA_FORMAT  0x31
#define DATAXA       0x32
#define DATAXB       0x33
#define DATAYA       0x34
#define DATAYB       0x35
#define DATAZA       0x36
#define DATAZB       0x37

#define G_UNIT       0.004

static const char *devName = "/dev/i2c-1";

void write_reg(int fd, char reg, char val) {
    char ar[2];
    ar[0] = reg;
    ar[1] = val;
    write(fd, ar, 2);
}

int main(int argc, char** argv) {
    int file;
    printf("I2C: connecting\n");
    if((file = open(devName, O_RDWR)) < 0) {
        fprintf(stderr, "I2C: failed to access %s\n", devName);
        exit(1);
    }
    printf("I2C: acquiring bus to 0x%x\n", ADDRESS);
    if(ioctl(file, I2C_SLAVE, ADDRESS) < 0) {
        fprintf(stderr, "I2C: failed to acquire bus\n");
        exit(1);
    }

    write_reg(file, POWER_CTL, 0x00);
    write_reg(file, OFSX, 0x00);
    write_reg(file, OFSY, 0x00);
    write_reg(file, OFSZ, 0x00);
    write_reg(file, DUR, 0x00);
    write_reg(file, LATENT, 0x00);
    write_reg(file, WINDOW, 0x00);
    write_reg(file, BW_RATE, DR);
    write_reg(file, INT_ENABLE, 0x00);
    write_reg(file, DATA_FORMAT, 0x09);
    write_reg(file, POWER_CTL, 0x08);

    unsigned char buf[6] = { 0x00 };
    
    while(1) {
        char addr = DATAXA;
        if(write(file, &addr, 1) != 1) {
            fprintf(stderr, "I2C: error writing reg address\n");
            continue;
        }
        if(read(file, buf, 6) != 6) {
            fprintf(stderr, "I2C: error reading data\n");
            continue;
        }
        float x = (buf[1] << 8) + buf[0];
        float y = (buf[3] << 8) + buf[2];
        float z = (buf[5] << 8) + buf[4];
        if(x >= 1<<15) x -= 1<<16;
        if(y >= 1<<15) y -= 1<<16;
        if(z >= 1<<15) z -= 1<<16;
        x *= G_UNIT;
        y *= G_UNIT;
        z *= G_UNIT;
        printf("\r%+7.5f %+7.5f %+7.5f", x, y, z);
        usleep(5);
    }
    
    return 0;
}
