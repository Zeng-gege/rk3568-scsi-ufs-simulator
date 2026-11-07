#ifndef NAND_SIM_H
#define NAND_SIM_H

#include <stdint.h>
#include <stdbool.h>

#define NAND_BLOCK_SIZE         (64 * 1024)
#define NAND_PAGE_SIZE          (2048)
#define NAND_OOB_SIZE           (64)
#define NAND_BLOCK_CNT          (128)

#define PAGE_PER_BLOCK          (NAND_BLOCK_SIZE / NAND_PAGE_SIZE)

typedef struct {
    uint32_t erase_cnt;
    bool is_bad;
    uint8_t page_valid[PAGE_PER_BLOCK];
} NandBlockInfo;

typedef struct {
    uint8_t *data_buf;
    uint8_t *oob_buf;
    NandBlockInfo blk_info[NAND_BLOCK_CNT];
    const char *img_path;
} NandSimCtx;

int nand_sim_init(NandSimCtx *ctx, const char *img_path);
void nand_sim_deinit(NandSimCtx *ctx);

int nand_erase_block(NandSimCtx *ctx, uint32_t blk_no);
int nand_write_page(NandSimCtx *ctx, uint32_t blk_no, uint32_t page_in_blk,
                    const uint8_t *data, const uint8_t *oob);
int nand_read_page(NandSimCtx *ctx, uint32_t blk_no, uint32_t page_in_blk,
                   uint8_t *data, uint8_t *oob);

int nand_mark_bad(NandSimCtx *ctx, uint32_t blk_no);
bool nand_is_bad(NandSimCtx *ctx, uint32_t blk_no);

int nand_save_img(NandSimCtx *ctx);
int nand_load_img(NandSimCtx *ctx);

#endif
