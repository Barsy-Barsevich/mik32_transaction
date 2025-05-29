#include "spi_transaction.h"


dma_status_t spi_transaction_init(spi_transaction_t *trans, spi_transaction_cfg_t *cfg)
{
    dma_status_t res;
    trans->host = cfg->host;
    trans->direction = cfg->direction;
    trans->token = cfg->token;
    trans->pre_cb = cfg->pre_cb;
    trans->post_cb = cfg->post_cb;

    dma_transaction_cfg_t dma_cfg = {0};
    dma_cfg.priority = cfg->dma_priority;
    dma_cfg.channel = cfg->dma_channel;
    dma_cfg.irq_en = 0;
    dma_cfg.read_block_size = DMA_BLOCK_8BIT;
    dma_cfg.write_block_size = DMA_BLOCK_8BIT;
    dma_cfg.read_burst_size = 0;
    dma_cfg.write_burst_size = 0;
    if (cfg->host == SPI_0)
    {
        dma_cfg.read_request = DMA_SPI_0_REQUEST;
        dma_cfg.write_request = DMA_SPI_0_REQUEST;
    }
    else if (cfg->host == SPI_1)
    {
        dma_cfg.read_request = DMA_SPI_1_REQUEST;
        dma_cfg.write_request = DMA_SPI_1_REQUEST;
    }
    else return HAL_DMA_INCORRECT_ARGUMENT;
    if (trans->direction == SPI_TRANSACTION_TRANSMIT)
    {
        dma_cfg.read_mode = DMA_MEMORY_MODE;
        dma_cfg.write_mode = DMA_PERIPHERY_MODE;
        dma_cfg.read_increment = 1;
        dma_cfg.write_increment = 0;
        dma_cfg.read_ack_en = 0;
        dma_cfg.write_ack_en = 0;
        dma_cfg.src_address = 0;
        dma_cfg.dst_address = (uint32_t)&(trans->host->TXDATA);
    }
    else if (trans->direction == SPI_TRANSACTION_RECEIVE)
    {
        dma_cfg.read_mode = DMA_PERIPHERY_MODE;
        dma_cfg.write_mode = DMA_MEMORY_MODE;
        dma_cfg.read_increment = 0;
        dma_cfg.write_increment = 1;
        dma_cfg.read_ack_en = 0;
        dma_cfg.write_ack_en = 0;
        dma_cfg.src_address = (uint32_t)&(trans->host->RXDATA);
        dma_cfg.dst_address = 0;
    }
    else return HAL_DMA_INCORRECT_ARGUMENT;
    dma_cfg.transaction_len = 0;
    dma_transaction_init(&(trans->dma_transaction), &dma_cfg);

    /* Enable DMA channel */
    trans->dma_transaction.config.CFG |= DMA_CH_CFG_ENABLE_M;

    return HAL_DMA_OK;
}



dma_status_t RAM_ATTR spi_transmit_start(spi_transaction_t *trans, const char *src, uint32_t len_bytes)
{
    if (trans->direction != SPI_TRANSACTION_TRANSMIT)
        return HAL_DMA_INCORRECT_ARGUMENT;
    if (trans->pre_cb != NULL)
    {
        trans->pre_cb(trans->token);
    }
    trans->host->ENABLE = SPI_ENABLE_M;
    trans->dma_transaction.config.SRC = (uint32_t)src;
    trans->dma_transaction.config.LEN = len_bytes;
    return dma_transaction_start(&(trans->dma_transaction));
}

bool RAM_ATTR spi_transaction_ready(spi_transaction_t *trans)
{
    return dma_transaction_ready(&(trans->dma_transaction));
}

dma_status_t RAM_ATTR spi_transaction_end(spi_transaction_t *trans, uint32_t timeout_us)
{
    dma_status_t res;
    res =  dma_transaction_wait(&(trans->dma_transaction), timeout_us);
    trans->host->ENABLE = 0;
    if (trans->post_cb != NULL)
    {
        trans->post_cb(trans->token);
    }
    return res;
}

dma_status_t RAM_ATTR spi_transmit(spi_transaction_t *trans, const char *src, uint32_t len_bytes, uint32_t timeout_us)
{
    dma_status_t res;
    res = spi_transmit_start(trans, src, len_bytes);
    if (res == HAL_DMA_OK)
    {
        res = spi_transaction_end(trans, timeout_us);
    }
    return res;
}

dma_status_t spi_receive_start(spi_transaction_t *trans, char *dst, uint32_t len_bytes)
{
    if (trans->direction != SPI_TRANSACTION_RECEIVE)
        return HAL_DMA_INCORRECT_ARGUMENT;
    if (trans->pre_cb != NULL)
    {
        trans->pre_cb(trans->token);
    }
    trans->host->ENABLE = SPI_ENABLE_M;
    trans->dma_transaction.config.DST = (uint32_t)dst;
    trans->dma_transaction.config.LEN = len_bytes;
    dma_transaction_start(&(trans->dma_transaction));
    return HAL_DMA_OK;
}

dma_status_t spi_receive(spi_transaction_t *trans, char *dst, uint32_t len_bytes, uint32_t timeout_us)
{
    dma_status_t res;
    res = spi_receive_start(trans, dst, len_bytes);
    if (res == HAL_DMA_OK)
    {
        res = spi_transaction_end(trans, timeout_us);
    }
    return res;
}