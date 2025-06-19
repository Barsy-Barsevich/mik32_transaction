#pragma once

#include <stdbool.h>
#include "dma_config.h"
#include "mik32_memory_map.h"
#include "mik32_hal.h"
#include "xprintf.h"

#define DMA_CH_AUTO     DMA_CHANNEL_COUNT

#define DMA_TIMEOUT_AUTO    0x00000000
#define DMA_NO_TIMEOUT      0xFFFFFFFF

typedef enum
{
    DMA_STATUS_OK = 0,
    DMA_STATUS_ERROR = 1,
    DMA_STATUS_TIMEOUT = 2,
    DMA_STATUS_INCORRECT_ARGUMENT = 3,
} dma_status_t;

typedef enum
{
    DMA_MIN_PRIORITY = 0,
    DMA_MAX_PRIORITY = 3,
    DMA_PRIORITY_0 = 0,
    DMA_PRIORITY_1 = 1,
    DMA_PRIORITY_2 = 2,
    DMA_PRIORITY_3 = 3,
} dma_priority_t;


typedef enum
{
    DMA_PERIPHERY_MODE = 0,
    DMA_MEMORY_MODE = 1
} dma_address_mode_t;


typedef enum
{
    DMA_BLOCK_BYTE = 0,
    DMA_BLOCK_HALFWORD = 1,
    DMA_BLOCK_WORD = 2,
    DMA_BLOCK_8BIT = 0,
    DMA_BLOCK_16BIT = 1,
    DMA_BLOCK_32BIT = 2
} dma_block_size_t;


typedef enum
{
    DMA_USART_0_REQUEST = 0,
    DMA_USART_1_REQUEST = 1,
    DMA_CRYPTO_REQUEST = 2,
    DMA_SPI_0_REQUEST = 3,
    DMA_SPI_1_REQUEST = 4,
    DMA_I2C_0_REQUEST = 5,
    DMA_I2C_1_REQUEST = 6,
    DMA_SPIFI_REQUEST = 7,
    DMA_TIMER32_1_REQUEST = 8,
    DMA_TIMER32_2_REQUEST = 9,
    DMA_DAC_0_REQUEST = 10,
    DMA_DAC_1_REQUEST = 11,
    DMA_TIMER32_0_REQUEST = 12
} dma_request_t;


//TODO: replace uint8_t
typedef struct
{
    dma_priority_t priority;
    dma_address_mode_t read_mode;
    dma_address_mode_t write_mode;
    uint8_t read_increment;
    uint8_t write_increment;
    dma_block_size_t read_block_size;
    dma_block_size_t write_block_size;
    uint8_t read_burst_size;
    uint8_t write_burst_size;
    dma_request_t read_request;
    dma_request_t write_request;
    uint8_t read_ack_en;
    uint8_t write_ack_en;
    uint8_t irq_en;

    uint8_t channel;

    uint32_t src_address;
    uint32_t dst_address;
    uint32_t transaction_len;
} dma_transaction_cfg_t;


typedef struct
{
    DMA_CHANNEL_TypeDef config;
    uint8_t channel;
    uint8_t temp_channel;
} dma_transaction_t;


/*!
 * @brief Инициализация DMA-транзакции
 * @p transaction указатель на структуру-дескриптор транзакции
 * @p cfg указатель на структуру инициализации транзакции
 * @return HAL_OK или HAL_INCORRECT_ARGUMENT
 */
dma_status_t dma_transaction_init(dma_transaction_t *transaction, dma_transaction_cfg_t *cfg);
/*!
 * @brief Запуск DMA-транзакции
 * @p transaction указатель на структуру-дескриптор транзакции
 * @return none
 */
dma_status_t dma_transaction_start(dma_transaction_t *transaction);
/*!
 * @brief 
 */
bool dma_transaction_ready(dma_transaction_t *transaction);
/*!
 * @brief Ожидание завершения DMA-транзакции
 * @p transaction указатель на структуру-дескриптор транзакции
 * @p timeout_us максимальное время ожидания в микросекундах
 * @return HAL_OK или HAL_TIMEOUT
 * */
dma_status_t dma_transaction_wait(dma_transaction_t *transaction, uint32_t timeout_us);
/*!
 *
 */
void dma_status_decoder(dma_status_t errcode);