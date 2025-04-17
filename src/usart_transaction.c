#include "usart_transaction.h"



HAL_Status_t usart_transaction_init(usart_transaction_t *trans, usart_transaction_cfg_t *cfg)
{
    trans->host = cfg->host;
    HAL_DMA_Request_t req = DMA_USART_0_REQUEST;
    if (cfg->host == UART_1) req = DMA_USART_1_REQUEST;
    HAL_DMA_Config_t dma_cfg = {
        .priority = cfg->dma_priority,
        .read_mode = DMA_MEMORY_MODE,
        .write_mode = DMA_PERIPHERY_MODE,
        .read_increment = 1,
        .write_increment = 0,
        .read_block_size = DMA_BLOCK_8BIT,
        .write_block_size = DMA_BLOCK_8BIT,
        .read_burst_size = 0,
        .write_burst_size = 0,
        .read_request = req,
        .write_request = req,
        .read_ack_en = 0,
        .write_ack_en = 1,
        .irq_en = 0,
        .channel = cfg->dma_channel,
        .src_address = 0,
        .dst_address = (uint32_t)&(trans->host->TXDATA),
        .transaction_len = 0,
    };
    return dma_transaction_init(&(trans->dma_transaction), &dma_cfg);
}

void RAM_ATTR usart_transaction_start(usart_transaction_t *trans, char *src, uint32_t len)
{
    trans->dma_transaction.config.SRC = (uint32_t)src;
    trans->dma_transaction.config.LEN = len;
    dma_transaction_start(&(trans->dma_transaction));
}