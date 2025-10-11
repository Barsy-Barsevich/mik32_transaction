#pragma once

#if defined(__cplusplus)
extern "C" {
#endif

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
    dma_transaction_t dma_transaction;
    usart_transaction_direction_t direction;
} usart_transaction_t;


dma_status_t usart_transaction_init(usart_transaction_t *trans, usart_transaction_cfg_t *cfg);
void usart_transaction_start(usart_transaction_t *trans, char *src, uint32_t len_bytes);
bool usart_transaction_ready(usart_transaction_t *trans);
dma_status_t usart_transaction_wait(usart_transaction_t *trans, uint32_t timeout_us);
dma_status_t usart_transmit_start(usart_transaction_t *trans, const char *src, uint32_t len_bytes);
dma_status_t usart_transmit(usart_transaction_t *trans, const char *src, uint32_t len_bytes, uint32_t timeout_us);
dma_status_t usart_receive_start(usart_transaction_t *trans, char *dst, uint32_t len_bytes);
dma_status_t usart_receive(usart_transaction_t *trans, char *dst, uint32_t len_bytes, uint32_t timeout_us);
uint32_t usart_transaction_left_bytes(usart_transaction_t *trans);
uint32_t usart_transaction_done_bytes(usart_transaction_t *trans);

#if defined(__cplusplus)
}
#endif
