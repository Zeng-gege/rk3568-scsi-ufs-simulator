#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<termios.h>
#include<string.h>


int set_uart(int fd, int speed)
{
struct termios opt;
tcgetattr(fd, &opt);

speed_t sp;
switch(speed){
case 9600: sp = B9600; break;
case 115200: sp = B115200; break;
default: sp = B115200;
}
cfsetispeed(&opt, sp);
cfsetospeed(&opt, sp);


opt.c_cflag &= ~PARENB;
opt.c_cflag &= ~CSTOPB;
opt.c_cflag &= ~CSIZE;
opt.c_cflag |= ~CS8;
opt.c_cflag |= CREAD | CLOCAL;
opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
opt.c_iflag &= ~(IXON | IXOFF | IXANY);
opt.c_oflag &= ~OPOST;

tcsetattr(fd, TCSANOW, &opt);
return 0;
}

int main(void)
{
int fd = open("/dev/ttyS3", O_RDWR | O_NOCTTY);
if(fd < 0){
perror("open ttyS3 fail");
return -1;
}
set_uart(fd, 115200);

char tx_buf[] = "Hello UART from RK3568!\r\n";
char rx_buf[128];

printf("UART ttyS3 ready, send data...\n");
write(fd, tx_buf, strlen(tx_buf));

printf("Waiting recv...\n");

int n = read(fd,rx_buf, sizeof(rx_buf)-1);
if(n > 0){
rx_buf[n] = '\0';
printf("Recv: %s\n", rx_buf);
}
close(fd);
return 0;
}
