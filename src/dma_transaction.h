#pragma once

#include <stdbool.h>
#include "dma_config.h"
#include "mik32_memory_map.h"
#include "mik32_hal.h"


#define DMA_CH_AUTO     5

typedef enum
{
    HAL_DMA_OK = 0,
    HAL_DMA_ERROR = 1,
    HAL_DMA_TIMEOUT = 2,
    HAL_DMA_INCORRECT_ARGUMENT = 3,
} HAL_Status_t;

typedef enum
{
    DMA_MIN_PRIORITY = 0,
    DMA_MAX_PRIORITY = 3,
    DMA_PRIORITY_0 = 0,
    DMA_PRIORITY_1 = 1,
    DMA_PRIORITY_2 = 2,
    DMA_PRIORITY_3 = 3,
} HAL_DMA_Prior_t;


typedef enum
{
    DMA_PERIPHERY_MODE = 0,
    DMA_MEMORY_MODE = 1
} HAL_DMA_AddressMode_t;


typedef enum
{
    DMA_BLOCK_BYTE = 0,
    DMA_BLOCK_HALFWORD = 1,
    DMA_BLOCK_WORD = 2,
    DMA_BLOCK_8BIT = 0,
    DMA_BLOCK_16BIT = 1,
    DMA_BLOCK_32BIT = 2
} HAL_DMA_BlockSize_t;


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
} HAL_DMA_Request_t;


//TODO: replace uint8_t
typedef struct
{
    HAL_DMA_Prior_t priority;
    HAL_DMA_AddressMode_t read_mode;
    HAL_DMA_AddressMode_t write_mode;
    uint8_t read_increment;
    uint8_t write_increment;
    HAL_DMA_BlockSize_t read_block_size;
    HAL_DMA_BlockSize_t write_block_size;
    uint8_t read_burst_size;
    uint8_t write_burst_size;
    HAL_DMA_Request_t read_request;
    HAL_DMA_Request_t write_request;
    uint8_t read_ack_en;
    uint8_t write_ack_en;
    uint8_t irq_en;

    uint8_t channel;

    uint32_t src_address;
    uint32_t dst_address;
    uint32_t transaction_len;
} HAL_DMA_Config_t;


typedef struct
{
    DMA_CHANNEL_TypeDef config;
    uint8_t channel;
} HAL_DMA_Transaction_t;


/*!
 * @brief Инициализация DMA-транзакции
 * @p transaction указатель на структуру-дескриптор транзакции
 * @p cfg указатель на структуру инициализации транзакции
 * @return HAL_OK или HAL_INCORRECT_ARGUMENT
 */
HAL_Status_t dma_transaction_init(HAL_DMA_Transaction_t *transaction, HAL_DMA_Config_t *cfg);
/*!
 * @brief Запуск DMA-транзакции
 * @p transaction указатель на структуру-дескриптор транзакции
 * @return none
 */
void dma_transaction_start(HAL_DMA_Transaction_t *transaction);
/*!
 * @brief 
 */
bool dma_transaction_ready(HAL_DMA_Transaction_t *transaction);
/*!
 * @brief Ожидание завершения DMA-транзакции
 * @p transaction указатель на структуру-дескриптор транзакции
 * @p timeout_us максимальное время ожидания в микросекундах
 * @return HAL_OK или HAL_TIMEOUT
 * */
HAL_Status_t dma_transaction_wait(HAL_DMA_Transaction_t *transaction, uint32_t timeout_us);