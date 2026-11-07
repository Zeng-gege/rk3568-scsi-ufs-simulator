#include "scsi_cdb.h"
#include <string.h>
#include <stdio.h>

int scsi_cdb_process(uint8_t *cdb_buf, int cdb_len,
			uint8_t *rsp_buf,int *rsp_len,
			NandSimCtx *nand)
{
	uint8_t op = cdb_buf[0];
	printf("scsi_cdb_process enter,op =0x%02x\n", op);
	fflush(stdout);

	rsp_buf[0] = SCSI_STATUS_GOOD;
	*rsp_len = 1;
	return 0;
}
