#pragma once

#include "stdio.h"
#include "stdbool.h"
#include "hal_i2c.h"
#include "dma_transaction.h"


typedef enum
{
    I2C_TRANSACTION_TRANSMIT = 0,
    I2C_TRANSACTION_RECEIVE = 1
} i2c_transaction_direction_t;

typedef struct
{
    I2C_TypeDef *host;
    uint8_t dma_channel;
    uint8_t dma_priority;
    i2c_transaction_direction_t direction;
    uint32_t src;
    uint32_t dst;
    uint16_t address;
} i2c_transaction_cfg_t;

typedef struct
{
    I2C_TypeDef *host;
    HAL_DMA_Transaction_t dma_transaction;
    i2c_transaction_direction_t direction;
} i2c_transaction_t;