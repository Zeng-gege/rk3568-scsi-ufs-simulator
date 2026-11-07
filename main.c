#include "uart.h"
#include "scsi_cdb.h"
#include "nand_sim.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define UART_DEV "/dev/ttyS4"
#define BAUDRATE 115200
#define RSP_BUF_SIZE 128

int main(void)
{
	int uart_fd = uart_open(UART_DEV, BAUDRATE);
	if(uart_fd < 0){
		printf("open %s faild\n", UART_DEV);
	fflush(stdout);
	return -1;
			}
	printf("uart open success: %s\n", UART_DEV);
	fflush(stdout);

	NandSimCtx nand_ctx;
	int ret = nand_sim_init(&nand_ctx, NULL);
	if(ret != 0)
	{
		printf("nand sim init faild\n");
		fflush(stdout);
		uart_close(uart_fd);
		return -1;
	}
	printf("nand simulator ready\n");
	fflush(stdout);

	uint8_t rx_buf[128];
	uint8_t rsp_buf[RSP_BUF_SIZE];
	int rsp_len;

	while(1)
	{
		int rlen = uart_recv(uart_fd, rx_buf, sizeof(rx_buf), 500);
		if(rlen <= 0)
		{
			continue;
		}
		uint8_t opcode = rx_buf[0];
		int expect_cdb_len = 0;
		switch(opcode)
		{
			case 0x00: expect_cdb_len = 6; break;
			case 0x12: expect_cdb_len = 10; break;
			case 0x28: expect_cdb_len = 10; break;
			case 0x2a: expect_cdb_len =10; break;
			default:
				expect_cdb_len = 6;
				break;
		}


		if(rlen < expect_cdb_len)
		{
			continue;
		}
		memset(rsp_buf, 0,RSP_BUF_SIZE);
		rsp_len = 0;
		scsi_cdb_process(rx_buf, rlen, rsp_buf, &rsp_len, &nand_ctx);

		if(rsp_len > 0)
		{
			uart_send(uart_fd, rsp_buf, rsp_len);
		}

		}

	nand_sim_deinit(&nand_ctx);
	uart_close(uart_fd);
	return 0;
}
