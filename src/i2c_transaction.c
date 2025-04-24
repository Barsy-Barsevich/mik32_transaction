#include "i2c_transaction.h"


HAL_Status_t i2c_transaction_init(i2c_transaction_t *trans, i2c_transaction_cfg_t *cfg)
{
    HAL_Status_t res;
    trans->host = cfg->host;
    trans->direction = cfg->direction;
    trans->address = cfg->address;
    trans->use_10bit_address = cfg->use_10bit_address;
    trans->_done = false;

    /* autoend enable */
    trans->host->CR2 |= I2C_CR2_AUTOEND_M;
    /* Разрешение поддержки DMA при передаче */
    trans->host->CR1 |= (I2C_CR1_TXDMAEN_M | I2C_CR1_RXDMAEN_M);
    /* Writing address */
    if (!trans->use_10bit_address) /* 7 битный адрес */
        trans->host->CR2 &= ~I2C_CR2_ADD10_M;
    else /* 10 битный адрес */
    {
        trans->host->CR2 |= I2C_CR2_ADD10_M;
        /* ведущий отправляет полную последовательность для чтения для 10 битного адреса */
        trans->host->CR2 &= ~I2C_CR2_HEAD10R_M;
    }

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
        dma_cfg.dst_address = 0;
    }
    else return HAL_DMA_INCORRECT_ARGUMENT;
    dma_cfg.transaction_len = 0;
    dma_transaction_init(&(trans->dma_transaction), &dma_cfg);

    /* Enable DMA channel */
    trans->dma_transaction.config.CFG |= DMA_CH_CFG_ENABLE_M;

    return HAL_DMA_OK;
}


HAL_Status_t RAM_ATTR i2c_transmit_start(i2c_transaction_t *trans, const char *src, uint8_t len)
{
    if (trans->direction != I2C_TRANSACTION_TRANSMIT)
    return HAL_DMA_INCORRECT_ARGUMENT;

    trans->_done = true;

    /* write num of bytes */
    trans->host->CR2 &= ~I2C_CR2_NBYTES_M;
    trans->host->CR2 |= I2C_CR2_NBYTES(len);
    // /* autoend enable */
    // trans->host->CR2 &= ~I2C_CR2_AUTOEND_M;
    // trans->host->CR2 |= (1 << I2C_CR2_AUTOEND_S);

    /* Writing address */
    trans->host->CR2 &= ~I2C_CR2_SADD_M;
    uint8_t addr = trans->address;
    if (!trans->use_10bit_address) addr <<= 1;
    trans->host->CR2 |= (addr << I2C_CR2_SADD_S);

    /* Задать направление передачи - запись */
    trans->host->CR2 &= ~I2C_CR2_RD_WRN_M;
    /* Настройка и включение канала DMA */
    trans->dma_transaction.config.SRC = (uint32_t)src;
    trans->dma_transaction.config.LEN = len;
    dma_transaction_start(&(trans->dma_transaction));

    // HAL_GPIO_WritePin(GPIO_2, GPIO_PIN_7, 1);
    /* Старт */
    trans->host->CR2 |= I2C_CR2_START_M;

    return HAL_DMA_OK;
}


HAL_Status_t RAM_ATTR i2c_transaction_end(i2c_transaction_t *trans, uint32_t timeout_us)
{
    HAL_Status_t res = dma_transaction_wait(&(trans->dma_transaction), timeout_us);
    if (res != HAL_DMA_OK) return res;
    // HAL_GPIO_WritePin(GPIO_2, GPIO_PIN_7, 0);
    
    /* Writing error statuses */
    if (DMA_CONFIG->CONFIG_STATUS & (1 << (trans->dma_transaction.channel + DMA_STATUS_CHANNEL_BUS_ERROR_S)))
        trans->status_dma_error = true;
    else trans->status_dma_error = false;
    uint32_t status_isr = trans->host->ISR;
    uint8_t err = I2C_TRANSACTION_ERR_NONE;
    if (status_isr & I2C_ISR_BERR_M) err |= I2C_TRANSACTION_ERR_BERR;
    if (status_isr & I2C_ISR_ARLO_M) err |= I2C_TRANSACTION_ERR_ARLO;
    if (status_isr & I2C_ISR_OVR_M)  err |= I2C_TRANSACTION_ERR_OVR;
    trans->status_i2c_error = err;
    return res;
}


HAL_Status_t RAM_ATTR i2c_transmit(i2c_transaction_t *trans, const char *src, uint8_t len, uint32_t timeout_us)
{
    HAL_Status_t res;
    res = i2c_transmit_start(trans, src, len);
    if (res == HAL_DMA_OK)
    {
        res = i2c_transaction_end(trans, timeout_us);
    }
    return res;
}


HAL_Status_t RAM_ATTR i2c_receive_start(i2c_transaction_t *trans, char *dst, uint8_t len)
{
    if (trans->direction != I2C_TRANSACTION_RECEIVE)
    return HAL_DMA_INCORRECT_ARGUMENT;

    trans->_done = true;

    /* write num of bytes */
    trans->host->CR2 &= ~I2C_CR2_NBYTES_M;
    trans->host->CR2 |= I2C_CR2_NBYTES(len);
    // /* autoend enable */
    // trans->host->CR2 &= ~I2C_CR2_AUTOEND_M;
    // trans->host->CR2 |= (1 << I2C_CR2_AUTOEND_S);

    /* Writing address */
    trans->host->CR2 &= ~I2C_CR2_SADD_M;
    uint8_t addr = trans->address;
    if (!trans->use_10bit_address) addr <<= 1;
    trans->host->CR2 |= (addr << I2C_CR2_SADD_S);

    /* Задать направление передачи - чтение */
    trans->host->CR2 |= I2C_CR2_RD_WRN_M;

    /* Настройка и включение канала DMA */
    trans->dma_transaction.config.DST = (uint32_t)dst;
    trans->dma_transaction.config.LEN = len;
    dma_transaction_start(&(trans->dma_transaction));

    // HAL_GPIO_WritePin(GPIO_2, GPIO_PIN_7, 1);
    /* Старт */
    trans->host->CR2 |= I2C_CR2_START_M;
    return HAL_DMA_OK;
}


HAL_Status_t RAM_ATTR i2c_receive(i2c_transaction_t *trans, char *dst, uint8_t len, uint32_t timeout_us)
{
    HAL_Status_t res;
    res = i2c_receive_start(trans, dst, len);
    if (res == HAL_DMA_OK)
    {
        res = i2c_transaction_end(trans, timeout_us);
    }
    return res;
}


HAL_Status_t RAM_ATTR i2c_repeat_transaction_start(i2c_transaction_t *trans)
{
    if (!trans->_done) return HAL_DMA_ERROR;
    DMA_CONFIG->CHANNELS[trans->dma_transaction.channel].CFG = trans->dma_transaction.config.CFG;
    trans->host->CR2 |= I2C_CR2_START_M;
    return HAL_DMA_OK;
}


HAL_Status_t RAM_ATTR i2c_repeat_transaction(i2c_transaction_t *trans, uint32_t timeout_us)
{
    HAL_Status_t res;
    res = i2c_repeat_transaction_start(trans);
    if (res == HAL_DMA_OK)
    {
        res = i2c_transaction_end(trans, timeout_us);
    }
    return res;
}


__attribute__((weak)) void i2c_transaction_err_decode(i2c_transaction_t *trans)
{
    xprintf("%lu", HAL_Millis());
    char tag[20];
    xsprintf(tag, "%lu\tI2C transaction: ");

    if (trans->status_dma_error) xprintf("%sI2C transaction: DMA err\n", tag);
    i2c_transaction_err_t err = trans->status_i2c_error;
    if (err == I2C_TRANSACTION_ERR_NONE) xprintf("%sI2C err: [NONE] (there aren't any error)\n", tag);
    if (err & I2C_TRANSACTION_ERR_BERR) xprintf("%sI2C err: [BERR] (bus error)\n", tag);
    if (err & I2C_TRANSACTION_ERR_ARLO) xprintf("%sI2C err: [ARLO] (the arbitrary fault)\n", tag);
    if (err & I2C_TRANSACTION_ERR_OVR) xprintf("%sI2C err: [OVR] (data over/underloading error)\n", tag);
    if (err & ~(I2C_TRANSACTION_ERR_BERR | I2C_TRANSACTION_ERR_ARLO | I2C_TRANSACTION_ERR_OVR))
        xprintf("%sI2C err: [Unexpected] (unexpected error, code 0x%02X)\n", tag, err);
}