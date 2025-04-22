#pragma once

#include "stdio.h"
#include "stdbool.h"
#include "mik32_hal_i2c.h"
#include "dma_transaction.h"
#include "xprintf.h"


typedef enum
{
    I2C_TRANSACTION_TRANSMIT = 0,
    I2C_TRANSACTION_RECEIVE = 1
} i2c_transaction_direction_t;

typedef enum
{
    I2C_TRANSACTION_ERR_NONE = 0,
    I2C_TRANSACTION_ERR_BERR = (1<<0),
    I2C_TRANSACTION_ERR_ARLO = (1<<1),
    I2C_TRANSACTION_ERR_OVR  = (1<<2),
} i2c_transaction_err_t;

typedef struct
{
    I2C_TypeDef *host;
    uint8_t dma_channel;
    uint8_t dma_priority;
    i2c_transaction_direction_t direction;
    uint16_t address;
    bool use_10bit_address;
} i2c_transaction_cfg_t;

typedef struct
{
    I2C_TypeDef *host;
    HAL_DMA_Transaction_t dma_transaction;
    i2c_transaction_direction_t direction;
    uint16_t address;
    bool use_10bit_address;
    /* Error status can be read after i2c_transaction_end call */
    bool status_dma_error;
    i2c_transaction_err_t status_i2c_error;
} i2c_transaction_t;

HAL_Status_t i2c_transaction_end(i2c_transaction_t *trans, uint32_t timeout_us);

HAL_Status_t i2c_transaction_init(i2c_transaction_t *trans, i2c_transaction_cfg_t *cfg);
HAL_Status_t i2c_transmit_start(i2c_transaction_t *trans, const char *src, uint8_t len);

void i2c_transaction_err_decode(i2c_transaction_t *trans);