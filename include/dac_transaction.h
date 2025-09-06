#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "mik32_memory_map.h"
#include "analog_reg.h"
#include "dma_transaction.h"
#include "mik32_hal_gpio.h"

#define DAC_0       (DAC_TypeDef*)&(ANALOG_REG->DAC0)
#define DAC_1       (DAC_TypeDef*)&(ANALOG_REG->DAC1)


typedef struct
{
    DAC_TypeDef *host;
    uint8_t dma_channel;
    uint8_t dma_priority;
    void *src;
    uint32_t token;
    void (*pre_cb)(uint32_t);
    void (*post_cb)(uint32_t);
    uint32_t len;
    uint32_t dac_rate;
    dma_sync_t sync;
    dma_timer_sync_cfg_t *timer_sync_cfg;

} dac_transaction_cfg_t;

typedef struct
{
    DAC_TypeDef *host;
    dma_transaction_t dma_transaction;
    uint32_t token;
    void (*pre_cb)(uint32_t);
    void (*post_cb)(uint32_t);
} dac_transaction_t;


dma_status_t dac_transaction_init(dac_transaction_t *trans, dac_transaction_cfg_t *cfg);
dma_status_t dac_transaction_start(dac_transaction_t *trans);
bool dac_transaction_ready(dac_transaction_t *trans);
dma_status_t dac_transaction_wait(dac_transaction_t *trans, uint32_t timeout_us);