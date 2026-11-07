#ifndef SCSI_CDB_H
#define SCSI_CDB_H

#include <stdint.h>
#include "nand_sim.h"

#define SCSI_STATUS_GOOD	0x00
#define SCSI_STATUS_CHECK_COND	0x02

#define SENSE_KEY_NO_SENSE	0x00
#define SENSE_KEY_MEDIUM_ERROR	0x03

#define VENDOR_ID	"LONGSYS"
#define PRODUCT_ID	"SIM_UFS01"
#define REV_ID		"1.00"

#define OP_INQUIRY		0x12
#define OP_TEST_UNIT_READY	0x00
#define OP_READ10		0x28
#define OP_WRITE10		0x2A


int scsi_cdb_process(uint8_t *cdb_buf, int cdb_len,
			uint8_t *rsp_buf, int *rsp_len,
			NandSimCtx *nand);

#endif
