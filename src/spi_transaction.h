#pragma once

#include <stdio.h>
#include "dma_transaction.h"
#include "mik32_hal_spi.h"

typedef enum
{
    SPI_TRANSACTION_TRANSMIT = 0,
    SPI_TRANSACTION_RECEIVE = 1
} spi_transaction_direction_t;


typedef struct
{
    SPI_TypeDef *host;
    uint8_t dma_channel;
    uint8_t dma_priority;
    spi_transaction_direction_t direction;
    uint32_t token;
    void (*pre_cb)(uint32_t);
    void (*post_cb)(uint32_t);
} spi_transaction_cfg_t;

typedef struct
{
    SPI_TypeDef *host;
    dma_transaction_t dma_transaction;
    spi_transaction_direction_t direction;
    uint32_t token;
    void (*pre_cb)(uint32_t);
    void (*post_cb)(uint32_t);
} spi_transaction_t;

dma_status_t spi_transaction_init(spi_transaction_t *trans, spi_transaction_cfg_t *cfg);
dma_status_t spi_transmit_start(spi_transaction_t *trans, const char *src, uint32_t len_bytes);
bool spi_transaction_ready(spi_transaction_t *trans);
dma_status_t spi_transaction_end(spi_transaction_t *trans, uint32_t timeout_us);
dma_status_t spi_transmit(spi_transaction_t *trans, const char *src, uint32_t len_bytes, uint32_t timeout_us);
dma_status_t spi_receive_start(spi_transaction_t *trans, char *dst, uint32_t len_bytes);
dma_status_t spi_receive(spi_transaction_t *trans, char *dst, uint32_t len_bytes, uint32_t timeout_us);