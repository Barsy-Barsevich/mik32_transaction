#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "mik32_memory_map.h"
#include "uart.h"

#include "dma_transaction.h"


typedef enum
{
    USART_TRANSACTION_TRANSMIT = 0,
    USART_TRANSACTION_RECEIVE = 1
} usart_transaction_direction_t;

typedef struct
{
    UART_TypeDef *host;
    uint8_t dma_channel;
    uint8_t dma_priority;
    usart_transaction_direction_t direction;
    uint32_t src;
    uint32_t dst;
} usart_transaction_cfg_t;

typedef struct
{
    UART_TypeDef *host;
    HAL_DMA_Transaction_t dma_transaction;
    usart_transaction_direction_t direction;
} usart_transaction_t;

inline bool usart_transaction_ready(usart_transaction_t *trans)
{
    return dma_transaction_ready(&(trans->dma_transaction));
}

inline HAL_Status_t usart_transaction_wait(usart_transaction_t *trans, uint32_t timeout_us)
{
    return dma_transaction_wait(&(trans->dma_transaction), timeout_us);
}

HAL_Status_t usart_transaction_init(usart_transaction_t *trans, usart_transaction_cfg_t *cfg);
void usart_transaction_start(usart_transaction_t *trans, char *src, uint32_t len);
HAL_Status_t usart_transmit_start(usart_transaction_t *trans, const char *src, uint32_t len);
HAL_Status_t usart_transmit(usart_transaction_t *trans, const char *src, uint32_t len, uint32_t timeout_us);
HAL_Status_t usart_receive_start(usart_transaction_t *trans, char *dst, uint32_t len);
HAL_Status_t usart_receive(usart_transaction_t *trans, char *dst, uint32_t len, uint32_t timeout_us);