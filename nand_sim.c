#include "nand_sim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int nand_sim_init(NandSimCtx *ctx, const char *img_path)
{
    memset(ctx, 0, sizeof(NandSimCtx));
    ctx->img_path = img_path;

    size_t data_total = NAND_BLOCK_CNT * PAGE_PER_BLOCK * NAND_PAGE_SIZE;
    size_t oob_total  = NAND_BLOCK_CNT * PAGE_PER_BLOCK * NAND_OOB_SIZE;

    ctx->data_buf = malloc(data_total);
    ctx->oob_buf  = malloc(oob_total);
    if(!ctx->data_buf || !ctx->oob_buf){
        perror("malloc nand buf");
        free(ctx->data_buf);
        free(ctx->oob_buf);
        return -1;
    }
    memset(ctx->data_buf, 0xFF, data_total);
    memset(ctx->oob_buf,  0xFF, oob_total);

    for(int i = 0; i < NAND_BLOCK_CNT; i++){
        ctx->blk_info[i].erase_cnt = 0;
        ctx->blk_info[i].is_bad = false;
        memset(ctx->blk_info[i].page_valid, 0, sizeof(ctx->blk_info[i].page_valid));
    }

    nand_load_img(ctx);
    return 0;
}

void nand_sim_deinit(NandSimCtx *ctx)
{
    if(!ctx) return;
    nand_save_img(ctx);
    free(ctx->data_buf);
    free(ctx->oob_buf);
    memset(ctx, 0, sizeof(NandSimCtx));
}

int nand_erase_block(NandSimCtx *ctx, uint32_t blk_no)
{
    if(blk_no >= NAND_BLOCK_CNT)
        return -1;
    if(ctx->blk_info[blk_no].is_bad)
        return -1;

    uint32_t data_offset = blk_no * PAGE_PER_BLOCK * NAND_PAGE_SIZE;
    uint32_t oob_offset  = blk_no * PAGE_PER_BLOCK * NAND_OOB_SIZE;
    memset(ctx->data_buf + data_offset, 0xFF, PAGE_PER_BLOCK * NAND_PAGE_SIZE);
    memset(ctx->oob_buf  + oob_offset,  0xFF, PAGE_PER_BLOCK * NAND_OOB_SIZE);

    ctx->blk_info[blk_no].erase_cnt ++;
    memset(ctx->blk_info[blk_no].page_valid, 0, sizeof(ctx->blk_info[blk_no].page_valid));
    return 0;
}

int nand_write_page(NandSimCtx *ctx, uint32_t blk_no, uint32_t page_in_blk,
                    const uint8_t *data, const uint8_t *oob)
{
    if(blk_no >= NAND_BLOCK_CNT || page_in_blk >= PAGE_PER_BLOCK)
        return -1;
    if(ctx->blk_info[blk_no].is_bad)
        return -1;
    if(ctx->blk_info[blk_no].page_valid[page_in_blk])
        return -1;

    uint32_t data_offset = blk_no * PAGE_PER_BLOCK * NAND_PAGE_SIZE + page_in_blk * NAND_PAGE_SIZE;
    uint32_t oob_offset  = blk_no * PAGE_PER_BLOCK * NAND_OOB_SIZE + page_in_blk * NAND_OOB_SIZE;

    memcpy(ctx->data_buf + data_offset, data, NAND_PAGE_SIZE);
    memcpy(ctx->oob_buf  + oob_offset,  oob,  NAND_OOB_SIZE);
    ctx->blk_info[blk_no].page_valid[page_in_blk] = 1;
    return 0;
}

int nand_read_page(NandSimCtx *ctx, uint32_t blk_no, uint32_t page_in_blk,
                   uint8_t *data, uint8_t *oob)
{
    if(blk_no >= NAND_BLOCK_CNT || page_in_blk >= PAGE_PER_BLOCK)
        return -1;
    if(ctx->blk_info[blk_no].is_bad)
        return -1;

    uint32_t data_offset = blk_no * PAGE_PER_BLOCK * NAND_PAGE_SIZE + page_in_blk * NAND_PAGE_SIZE;
    uint32_t oob_offset  = blk_no * PAGE_PER_BLOCK * NAND_OOB_SIZE + page_in_blk * NAND_OOB_SIZE;

    memcpy(data, ctx->data_buf + data_offset, NAND_PAGE_SIZE);
    memcpy(oob,  ctx->oob_buf  + oob_offset,  NAND_OOB_SIZE);
    return 0;
}

int nand_mark_bad(NandSimCtx *ctx, uint32_t blk_no)
{
    if(blk_no >= NAND_BLOCK_CNT)
        return -1;
    ctx->blk_info[blk_no].is_bad = true;
    return 0;
}

bool nand_is_bad(NandSimCtx *ctx, uint32_t blk_no)
{
    if(blk_no >= NAND_BLOCK_CNT)
        return true;
    return ctx->blk_info[blk_no].is_bad;
}

int nand_save_img(NandSimCtx *ctx)
{
    if(!ctx || !ctx->img_path) return -1;
    int fd = open(ctx->img_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd < 0){
        perror("save img open");
        return -1;
    }
    (void)write(fd, ctx->data_buf, NAND_BLOCK_CNT * PAGE_PER_BLOCK * NAND_PAGE_SIZE);
    (void)write(fd, ctx->oob_buf,  NAND_BLOCK_CNT * PAGE_PER_BLOCK * NAND_OOB_SIZE);
    (void)write(fd, ctx->blk_info, sizeof(ctx->blk_info));
    close(fd);
    return 0;
}

int nand_load_img(NandSimCtx *ctx)
{
    if(!ctx || !ctx->img_path) return -1;
    int fd = open(ctx->img_path, O_RDONLY);
    if(fd < 0){
        return 0;
    }
    (void)read(fd, ctx->data_buf, NAND_BLOCK_CNT * PAGE_PER_BLOCK * NAND_PAGE_SIZE);
    (void)read(fd, ctx->oob_buf,  NAND_BLOCK_CNT * PAGE_PER_BLOCK * NAND_OOB_SIZE);
    (void)read(fd, ctx->blk_info, sizeof(ctx->blk_info));
    close(fd);
    return 0;
}
