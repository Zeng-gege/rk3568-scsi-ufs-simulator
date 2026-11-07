#ifndef UART_H
#define UART_H

#include <stdint.h>

int uart_open(const char *dev, int baudrate);
void uart_close(int fd);
int uart_send(int fd, const uint8_t *buf, int len);
int uart_recv(int fd, uint8_t *buf,int max_len, int timeout_ms);

#endif



