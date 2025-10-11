#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "mik32_memory_map.h"
#include "timer32.h"
#include "power_manager.h"
#include "mik32_hal_pcc.h"

typedef struct
{
    TIMER32_TypeDef *host;
    uint32_t rate;
} dma_timer_sync_cfg_t;

bool dma_sync_timer_init(dma_timer_sync_cfg_t *cfg, bool external_clock);

#if defined(__cplusplus)
}
#endif
