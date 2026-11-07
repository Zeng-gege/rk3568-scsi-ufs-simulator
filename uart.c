#include "uart.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <sys/select.h>

static int set_uart_attr(int fd, int speed)
{
    struct termios opt;
    if (tcgetattr(fd, &opt) < 0) {
        perror("tcgetattr");
        fflush(stdout);
        return -1;
    }

    speed_t sp;
    switch(speed){
        case 9600:    sp = B9600; break;
        case 115200:  sp = B115200; break;
        default: sp = B115200;
    }
    cfsetispeed(&opt, sp);
    cfsetospeed(&opt, sp);

    opt.c_cflag &= ~PARENB;
    opt.c_cflag &= ~CSTOPB;
    opt.c_cflag &= ~CSIZE;
    opt.c_cflag |= CS8;
    opt.c_cflag |= CREAD | CLOCAL;

    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    opt.c_iflag &= ~(IXON | IXOFF | IXANY);
    opt.c_oflag &= ~OPOST;

    // 清空收发缓冲区
    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &opt) < 0) {
        perror("tcsetattr");
        fflush(stdout);
        return -1;
    }
    return 0;
}

int uart_open(const char *dev, int baudrate)
{
    int fd = open(dev, O_RDWR | O_NOCTTY);
    if(fd < 0){
        perror("uart open");
        fflush(stdout);
        return -1;
    }
    if (set_uart_attr(fd, baudrate) < 0) {
        close(fd);
        return -1;
    }
    return fd;
}

void uart_close(int fd)
{
    if(fd > 0) close(fd);
}

int uart_send(int fd, const uint8_t *buf, int len)
{
    return write(fd, buf, len);
}

int uart_recv(int fd, uint8_t *buf, int max_len, int timeout_ms)
{
    fd_set rfds;
    struct timeval tv;
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    tv.tv_sec  = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int ret = select(fd + 1, &rfds, NULL, NULL, &tv);
    if(ret <= 0)
        return 0;

    return read(fd, buf, max_len);
}

