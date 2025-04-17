#include "i2c_transaction.h"


void i2c_transaction_set_len(i2c_transaction_t *trans, uint16_t len)
{
    trans->host->CR2 &= ~I2C_CR2_NBYTES_M;
    trans->host->CR2 |= I2C_CR2_NBYTES(len);
}

HAL_Status_t i2c_transaction_init(i2c_transaction_t *trans, i2c_transaction_cfg_t *cfg)
{
    HAL_Status_t res;
    trans->host = cfg->host;
    trans->direction = cfg->direction;
    
    // HAL_I2C_Master_SlaveAddress(hi2c, SlaveAddress);
    /* Задать направление передачи - запись */
    trans->host->CR2 &= ~I2C_CR2_RD_WRN_M;

    /* Разрешение поддержки DMA при передаче */
    trans->host->CR1 |= I2C_CR1_TXDMAEN_M;

    HAL_DMA_Config_t dma_cfg = {0};
    dma_cfg.priority = cfg->dma_priority;
    dma_cfg.channel = cfg->dma_channel;
    dma_cfg.irq_en = 0;
    dma_cfg.read_block_size = DMA_BLOCK_8BIT;
    dma_cfg.write_block_size = DMA_BLOCK_8BIT;
    dma_cfg.read_burst_size = 0;
    dma_cfg.write_burst_size = 0;
    if (cfg->host == I2C_0)
    {
        dma_cfg.read_request = DMA_I2C_0_REQUEST;
        dma_cfg.write_request = DMA_I2C_0_REQUEST;
    }
    else if (cfg->host == I2C_1)
    {
        dma_cfg.read_request = DMA_I2C_1_REQUEST;
        dma_cfg.write_request = DMA_I2C_1_REQUEST;
    }
    else return HAL_DMA_INCORRECT_ARGUMENT;
    if (trans->direction == I2C_TRANSACTION_TRANSMIT)
    {
        dma_cfg.read_mode = DMA_MEMORY_MODE;
        dma_cfg.write_mode = DMA_PERIPHERY_MODE;
        dma_cfg.read_increment = 1;
        dma_cfg.write_increment = 0;
        dma_cfg.read_ack_en = 0;
        dma_cfg.write_ack_en = 1;
        dma_cfg.src_address = 0;
        dma_cfg.dst_address = (uint32_t)&(trans->host->TXDR);
    }
    else if (trans->direction == I2C_TRANSACTION_RECEIVE)
    {
        dma_cfg.read_mode = DMA_PERIPHERY_MODE;
        dma_cfg.write_mode = DMA_MEMORY_MODE;
        dma_cfg.read_increment = 0;
        dma_cfg.write_increment = 1;
        dma_cfg.read_ack_en = 1;
        dma_cfg.write_ack_en = 0;
        dma_cfg.src_address = (uint32_t)&(trans->host->RXDR);
        dma_cfg.dst_address = (uint32_t)&(trans->host->TXDR);
    }
    else return HAL_DMA_INCORRECT_ARGUMENT;
    dma_cfg.transaction_len = 0;
    dma_transaction_init(&(trans->dma_transaction), &dma_cfg);

    /* Старт */
    trans->host->CR2 |= I2C_CR2_START_M;

    return HAL_DMA_OK;
}