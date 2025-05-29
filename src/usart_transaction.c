#include "usart_transaction.h"


dma_status_t usart_transaction_init(usart_transaction_t *trans, usart_transaction_cfg_t *cfg)
{
    trans->host = cfg->host;
    trans->direction = cfg->direction;

    dma_transaction_cfg_t dma_cfg = {0};
    dma_cfg.priority = cfg->dma_priority;
    dma_cfg.read_block_size = DMA_BLOCK_8BIT;
    dma_cfg.write_block_size = DMA_BLOCK_8BIT;
    dma_cfg.read_burst_size = 0;
    dma_cfg.write_burst_size = 0;

    if (cfg->host == UART_0)
    {
        dma_cfg.read_request = DMA_USART_0_REQUEST;
        dma_cfg.write_request = DMA_USART_0_REQUEST;
    }
    else if (cfg->host == UART_1)
    {
        dma_cfg.read_request = DMA_USART_1_REQUEST;
        dma_cfg.write_request = DMA_USART_1_REQUEST;
    }
    else return HAL_DMA_INCORRECT_ARGUMENT;

    //TODO: post_cb as in esp-idf
    dma_cfg.irq_en = 0;

    if (cfg->direction == USART_TRANSACTION_TRANSMIT)
    {
        trans->host->CONTROL3 |= UART_CONTROL3_DMAT_M;
        trans->host->CONTROL1 |= UART_CONTROL1_TE_M;
        while((trans->host->FLAGS & UART_FLAGS_TEACK_M) == 0);
        dma_cfg.read_mode = DMA_MEMORY_MODE;
        dma_cfg.write_mode = DMA_PERIPHERY_MODE;
        dma_cfg.read_increment = 1;
        dma_cfg.write_increment = 0;
        dma_cfg.read_ack_en = 0;
        dma_cfg.write_ack_en = 1;
        dma_cfg.src_address = 0;
        dma_cfg.dst_address = (uint32_t)&(trans->host->TXDATA);
    }
    else if (cfg->direction == USART_TRANSACTION_RECEIVE)
    {
        trans->host->CONTROL3 |= UART_CONTROL3_DMAR_M;
        trans->host->CONTROL1 |= UART_CONTROL1_RE_M;
        while((trans->host->FLAGS & UART_FLAGS_REACK_M) == 0);
        dma_cfg.read_mode = DMA_PERIPHERY_MODE;
        dma_cfg.write_mode = DMA_MEMORY_MODE;
        dma_cfg.read_increment = 0;
        dma_cfg.write_increment = 1;
        dma_cfg.read_ack_en = 1;
        dma_cfg.write_ack_en = 0;
        dma_cfg.src_address = (uint32_t)&(trans->host->RXDATA);
        dma_cfg.dst_address = 0;
    }
    else return HAL_DMA_INCORRECT_ARGUMENT;
    dma_cfg.channel = cfg->dma_channel,
    dma_cfg.transaction_len = 0;
    
    return dma_transaction_init(&(trans->dma_transaction), &dma_cfg);
}

void RAM_ATTR usart_transaction_start(usart_transaction_t *trans, char *arr, uint32_t len)
{
    if (trans->direction == USART_TRANSACTION_TRANSMIT)
        trans->dma_transaction.config.SRC = (uint32_t)arr;
    else if (trans->direction == USART_TRANSACTION_RECEIVE)
        trans->dma_transaction.config.DST = (uint32_t)arr;
    else return;
    trans->dma_transaction.config.LEN = len;
    dma_transaction_start(&(trans->dma_transaction));
}

dma_status_t RAM_ATTR usart_transmit_start(usart_transaction_t *trans, const char *src, uint32_t len)
{
    if (trans->direction != USART_TRANSACTION_TRANSMIT)
        return HAL_DMA_INCORRECT_ARGUMENT;
    trans->dma_transaction.config.SRC = src;
    trans->dma_transaction.config.LEN = len;
    dma_transaction_start(&(trans->dma_transaction));
    return HAL_DMA_OK;
}

dma_status_t RAM_ATTR usart_transmit(usart_transaction_t *trans, const char *src, uint32_t len, uint32_t timeout_us)
{
    dma_status_t res;
    res = usart_transmit_start(trans, src, len);
    if (res == HAL_DMA_OK)
        res = usart_transaction_wait(trans, timeout_us);
    return res;
}

dma_status_t RAM_ATTR usart_receive_start(usart_transaction_t *trans, char *dst, uint32_t len)
{
    if (trans->direction != USART_TRANSACTION_RECEIVE)
        return HAL_DMA_INCORRECT_ARGUMENT;
    trans->dma_transaction.config.DST = dst;
    trans->dma_transaction.config.LEN = len;
    dma_transaction_start(&(trans->dma_transaction));
    return HAL_DMA_OK;
}

dma_status_t RAM_ATTR usart_receive(usart_transaction_t *trans, char *dst, uint32_t len, uint32_t timeout_us)
{
    dma_status_t res;
    res = usart_receive_start(trans, dst, len);
    if (res == HAL_DMA_OK)
        res = usart_transaction_wait(trans, timeout_us);
    return res;
}