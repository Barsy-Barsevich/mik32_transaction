#include "spi_transaction.h"


static int spi_init(SPI_TypeDef *host)
{
    if (host == SPI_0)
    {
        __HAL_PCC_SPI_0_CLK_ENABLE();
        __HAL_PCC_GPIO_0_CLK_ENABLE();
        uint8_t miso_pin = 0;
        PAD_CONFIG->PORT_0_CFG &= ~(0b11 << (2 * miso_pin));
        PAD_CONFIG->PORT_0_CFG |= (0b01 << (2 * miso_pin));
        uint8_t mosi_pin = 1;
        PAD_CONFIG->PORT_0_CFG &= ~(0b11 << (2 * mosi_pin));
        PAD_CONFIG->PORT_0_CFG |= (0b01 << (2 * mosi_pin));
        uint8_t sck_pin = 2;
        PAD_CONFIG->PORT_0_CFG &= ~(0b11 << (2 * sck_pin));
        PAD_CONFIG->PORT_0_CFG |= (0b01 << (2 * sck_pin));
        uint8_t ssin_pin = 3;
        PAD_CONFIG->PORT_0_CFG &= ~(0b11 << (2 * ssin_pin));
        PAD_CONFIG->PORT_0_CFG |= (0b01 << (2 * ssin_pin));
        PAD_CONFIG->PORT_0_PUPD &= ~(0b11 << (2 * ssin_pin));
        PAD_CONFIG->PORT_0_PUPD |= (0b01 << (2 * ssin_pin));
    }
    else if (host == SPI_1)
    {
        __HAL_PCC_SPI_1_CLK_ENABLE();
        __HAL_PCC_GPIO_1_CLK_ENABLE();
        uint8_t miso_pin = 0;
        PAD_CONFIG->PORT_1_CFG &= ~(0b11 << (2 * miso_pin));
        PAD_CONFIG->PORT_1_CFG |= (0b01 << (2 * miso_pin));
        uint8_t mosi_pin = 1;
        PAD_CONFIG->PORT_1_CFG &= ~(0b11 << (2 * mosi_pin));
        PAD_CONFIG->PORT_1_CFG |= (0b01 << (2 * mosi_pin));
        uint8_t sck_pin = 2;
        PAD_CONFIG->PORT_1_CFG &= ~(0b11 << (2 * sck_pin));
        PAD_CONFIG->PORT_1_CFG |= (0b01 << (2 * sck_pin));
        uint8_t ssin_pin = 3;
        PAD_CONFIG->PORT_1_CFG &= ~(0b11 << (2 * ssin_pin));
        PAD_CONFIG->PORT_1_CFG |= (0b01 << (2 * ssin_pin));
        PAD_CONFIG->PORT_1_PUPD &= ~(0b11 << (2 * ssin_pin));
        PAD_CONFIG->PORT_1_PUPD |= (0b01 << (2 * ssin_pin));
    }
    else return 1;

    host->ENABLE &= ~SPI_ENABLE_M;
    host->ENABLE |= SPI_ENABLE_CLEAR_RX_FIFO_M;
    host->ENABLE |= SPI_ENABLE_CLEAR_TX_FIFO_M;
    volatile uint32_t unused = host->INT_STATUS;
    (void)unused;
    uint32_t cfg = SPI_CONFIG_MASTER_M | host->CONFIG;
    // cfg |= 0b111<<SPI_CONFIG_BAUD_RATE_DIV_S;
    cfg |= SPI_CONFIG_MANUAL_CS_M;
    cfg |= 1<<SPI_CONFIG_CLK_PH_S;
    cfg |= 1<<SPI_CONFIG_CLK_POL_S;
    cfg |= 0b1111<<SPI_CONFIG_CS_S;
    host->CONFIG = cfg;
    uint32_t delay = 0;
    delay |= 0x01 << SPI_DELAY_BTWN_S;
    delay |= 0x00 << SPI_DELAY_AFTER_S;
    delay |= 0x00 << SPI_DELAY_INIT_S;
    host->DELAY = delay;
    host->TX_THR = 1;
    return 0;
}

dma_status_t spi_transaction_init(spi_transaction_t *trans, spi_transaction_cfg_t *cfg)
{
    dma_status_t res;
    trans->host = cfg->host;
    trans->direction = cfg->direction;
    trans->token = cfg->token;
    trans->pre_cb = cfg->pre_cb;
    trans->post_cb = cfg->post_cb;

    int spi_init_res = spi_init(cfg->host);
    if (spi_init_res != 0)
    {
        return DMA_STATUS_INCORRECT_ARGUMENT;
    }

    dma_transaction_cfg_t dma_cfg = {0};
    dma_cfg.priority = cfg->dma_priority;
    dma_cfg.channel = cfg->dma_channel;
    dma_cfg.irq_en = 0;
    dma_cfg.read_block_size = DMA_BLOCK_8BIT;
    dma_cfg.write_block_size = DMA_BLOCK_8BIT;
    dma_cfg.read_burst_size = 0;
    dma_cfg.write_burst_size = 0;
    if (cfg->host == SPI_0)
    {
        dma_cfg.read_request = DMA_SPI_0_REQUEST;
        dma_cfg.write_request = DMA_SPI_0_REQUEST;
    }
    else if (cfg->host == SPI_1)
    {
        dma_cfg.read_request = DMA_SPI_1_REQUEST;
        dma_cfg.write_request = DMA_SPI_1_REQUEST;
    }
    else return DMA_STATUS_INCORRECT_ARGUMENT;
    if (trans->direction == SPI_TRANSACTION_TRANSMIT)
    {
        dma_cfg.read_mode = DMA_MEMORY_MODE;
        dma_cfg.write_mode = DMA_PERIPHERY_MODE;
        dma_cfg.read_increment = 1;
        dma_cfg.write_increment = 0;
        dma_cfg.read_ack_en = 0;
        dma_cfg.write_ack_en = 0;
        dma_cfg.src_address = 0;
        dma_cfg.dst_address = (uint32_t)&(trans->host->TXDATA);
    }
    else if (trans->direction == SPI_TRANSACTION_RECEIVE)
    {
        dma_cfg.read_mode = DMA_PERIPHERY_MODE;
        dma_cfg.write_mode = DMA_MEMORY_MODE;
        dma_cfg.read_increment = 0;
        dma_cfg.write_increment = 1;
        dma_cfg.read_ack_en = 0;
        dma_cfg.write_ack_en = 0;
        dma_cfg.src_address = (uint32_t)&(trans->host->RXDATA);
        dma_cfg.dst_address = 0;
    }
    else return DMA_STATUS_INCORRECT_ARGUMENT;
    dma_cfg.transaction_len = 0;
    dma_transaction_init(&(trans->dma_transaction), &dma_cfg);

    /* Enable DMA channel */
    trans->dma_transaction.config.CFG |= DMA_CH_CFG_ENABLE_M;

    return DMA_STATUS_OK;
}



dma_status_t RAM_ATTR spi_transmit_start(spi_transaction_t *trans, const char *src, uint32_t len_bytes)
{
    printf("len: %u\n", (unsigned)len_bytes);
    if (trans->direction != SPI_TRANSACTION_TRANSMIT)
        return DMA_STATUS_INCORRECT_ARGUMENT;
    if (trans->pre_cb != NULL)
    {
        trans->pre_cb(trans->token);
    }
    trans->host->ENABLE = SPI_ENABLE_M;
    trans->dma_transaction.config.SRC = (uint32_t)src;
    trans->dma_transaction.config.LEN = len_bytes;
    return dma_transaction_start(&(trans->dma_transaction));
}

bool RAM_ATTR spi_transaction_ready(spi_transaction_t *trans)
{
    return dma_transaction_ready(&(trans->dma_transaction));
}

dma_status_t RAM_ATTR spi_transaction_end(spi_transaction_t *trans, uint32_t timeout_us)
{
    dma_status_t res;
    res =  dma_transaction_wait(&(trans->dma_transaction), timeout_us);
    trans->host->ENABLE = 0;
    if (trans->post_cb != NULL)
    {
        trans->post_cb(trans->token);
    }
    return res;
}

dma_status_t RAM_ATTR spi_transmit(spi_transaction_t *trans, const char *src, uint32_t len_bytes, uint32_t timeout_us)
{
    dma_status_t res;
    res = spi_transmit_start(trans, src, len_bytes);
    if (res == DMA_STATUS_OK)
    {
        res = spi_transaction_end(trans, timeout_us);
    }
    return res;
}

dma_status_t spi_receive_start(spi_transaction_t *trans, char *dst, uint32_t len_bytes)
{
    if (trans->direction != SPI_TRANSACTION_RECEIVE)
        return DMA_STATUS_INCORRECT_ARGUMENT;
    if (trans->pre_cb != NULL)
    {
        trans->pre_cb(trans->token);
    }
    trans->host->ENABLE = SPI_ENABLE_M;
    trans->dma_transaction.config.DST = (uint32_t)dst;
    trans->dma_transaction.config.LEN = len_bytes;
    dma_status_t res;
    res = dma_transaction_start(&(trans->dma_transaction));
    return res;
}

dma_status_t spi_receive(spi_transaction_t *trans, char *dst, uint32_t len_bytes, uint32_t timeout_us)
{
    dma_status_t res;
    res = spi_receive_start(trans, dst, len_bytes);
    if (res == DMA_STATUS_OK)
    {
        res = spi_transaction_end(trans, timeout_us);
    }
    return res;
}