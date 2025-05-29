#include "dma_transaction.h"


/*!
 * @brief Инициализация DMA-транзакции
 * @p transaction указатель на структуру-дескриптор транзакции
 * @p cfg указатель на структуру инициализации транзакции
 * @return HAL_OK или HAL_INCORRECT_ARGUMENT
 */
dma_status_t dma_transaction_init(dma_transaction_t *transaction, dma_transaction_cfg_t *cfg)
{
    __HAL_PCC_DMA_CLK_ENABLE();
    DMA_CONFIG->CONFIG_STATUS = 0x3F;

    if (cfg->channel > DMA_CHANNEL_COUNT)
        return DMA_STATUS_INCORRECT_ARGUMENT;
    transaction->channel = cfg->channel;
    
    uint32_t cfg_word = 0;
    cfg_word |= DMA_CH_CFG_ENABLE_M
    | (cfg->priority & 0b11) << DMA_CH_CFG_PRIOR_S
    | (cfg->read_mode & 0b1) << DMA_CH_CFG_READ_MODE_S
    | (cfg->write_mode & 0b1) << DMA_CH_CFG_WRITE_MODE_S
    | (cfg->read_increment & 0b1) << DMA_CH_CFG_READ_INCREMENT_S
    | (cfg->write_increment & 0b1) << DMA_CH_CFG_WRITE_INCREMENT_S
    | (cfg->read_block_size & 0b11) << DMA_CH_CFG_READ_SIZE_S
    | (cfg->write_block_size & 0b11) << DMA_CH_CFG_WRITE_SIZE_S
    | (cfg->read_burst_size & 0b111) << DMA_CH_CFG_READ_BURST_SIZE_S
    | (cfg->write_burst_size & 0b111) << DMA_CH_CFG_WRITE_BURST_SIZE_S
    | (cfg->read_request & 0b1111) << DMA_CH_CFG_READ_REQUEST_S
    | (cfg->write_request & 0b1111) << DMA_CH_CFG_WRITE_REQUEST_S
    | (cfg->read_ack_en & 0b1) << DMA_CH_CFG_READ_ACK_EN_S
    | (cfg->write_ack_en & 0b1) << DMA_CH_CFG_WRITE_ACK_EN_S
    | (cfg->irq_en & 1) << DMA_CH_CFG_IRQ_EN_S;
    
    transaction->config.SRC = cfg->src_address;
    transaction->config.DST = cfg->dst_address;
    transaction->config.LEN = cfg->transaction_len;
    transaction->config.CFG = cfg_word;

    // xprintf("%04X\n", cfg_word);
    return DMA_STATUS_OK;
}


/*!
 * @brief Запуск DMA-транзакции
 * @p transaction указатель на структуру-дескриптор транзакции
 * @return none
 */
dma_status_t RAM_ATTR dma_transaction_start(dma_transaction_t *transaction)
{
    //xprintf("cfg: %lX src: %lX dst: %lX len: %lX\n", transaction->config.CFG, transaction->config.SRC, transaction->config.DST, transaction->config.LEN);
    // DMA_CONFIG->CHANNELS[transaction->channel].SRC = transaction->config.SRC;
    // DMA_CONFIG->CHANNELS[transaction->channel].DST = transaction->config.DST;
    // DMA_CONFIG->CHANNELS[transaction->channel].LEN = transaction->config.LEN-1;
    // DMA_CONFIG->CHANNELS[transaction->channel].CFG = transaction->config.CFG;
    uint8_t ch = transaction->channel;
    if (ch > DMA_CHANNEL_COUNT) return DMA_STATUS_INCORRECT_ARGUMENT;
    if (ch == DMA_CHANNEL_COUNT) //DMA_CH_AUTO
    {
        uint32_t config_status = DMA_CONFIG->CONFIG_STATUS;
        bool status = false;
        for (uint8_t i=0; i<DMA_CHANNEL_COUNT; i++)
        {
            if ((config_status & (1 << (i+DMA_STATUS_READY_S))) != 0)
            {
                ch = i;
                status = true;
                break;
            }
        }
        if (!status) return DMA_STATUS_ERROR;
    }
    transaction->temp_channel = ch;
    DMA_CONFIG->CHANNELS[ch].SRC = transaction->config.SRC;
    DMA_CONFIG->CHANNELS[ch].DST = transaction->config.DST;
    DMA_CONFIG->CHANNELS[ch].LEN = transaction->config.LEN-1;
    DMA_CONFIG->CHANNELS[ch].CFG = transaction->config.CFG;
    return DMA_STATUS_OK;
}


/*!
 * @brief 
 */
bool RAM_ATTR dma_transaction_ready(dma_transaction_t *transaction)
{
    uint32_t mask = (1 << transaction->channel) << DMA_STATUS_READY_S;
    bool ret = false;
    if ((DMA_CONFIG->CONFIG_STATUS & mask) != 0) ret = true;
    return ret;
}


/*!
 * @brief Ожидание завершения DMA-транзакции
 * @p transaction указатель на структуру-дескриптор транзакции
 * @p timeout_us максимальное время ожидания в микросекундах
 * @return HAL_OK или HAL_TIMEOUT
 * */
dma_status_t RAM_ATTR dma_transaction_wait(dma_transaction_t *transaction, uint32_t timeout_us)
{
    uint32_t mask = (1 << transaction->temp_channel) << DMA_STATUS_READY_S;
    uint32_t timestamp = HAL_Micros();
    dma_status_t ret = DMA_STATUS_TIMEOUT;
    while (HAL_Micros() - timestamp < timeout_us)
    {
        if ((DMA_CONFIG->CONFIG_STATUS & mask) != 0)
        {
            ret = DMA_STATUS_OK;
            break;
        }
    }
    // disable channel if the timeout
    if (ret != DMA_STATUS_OK)
    {
        DMA_CONFIG->CHANNELS[transaction->temp_channel].CFG &= ~DMA_CH_CFG_ENABLE_M;
    }
    return ret;
}

void dma_status_decoder(dma_status_t errcode)
{
    xprintf("DMA status: ");
    switch (errcode)
    {
        case DMA_STATUS_OK:
            xprintf("no error\n");
            break;
        case DMA_STATUS_ERROR:
            xprintf("error\n");
            break;
        case DMA_STATUS_INCORRECT_ARGUMENT:
            xprintf("incorrect argument\n");
            break;
        case DMA_STATUS_TIMEOUT:
            xprintf("timeout\n");
            break;
        default:
            xprintf("unexpected error (%u)\n", errcode);
    }
}