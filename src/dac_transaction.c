#include "dac_transaction.h"

dma_status_t dac_transaction_init(dac_transaction_t *trans, dac_transaction_cfg_t *cfg)
{
    trans->host = cfg->host;
    trans->token = cfg->token;
    trans->pre_cb = cfg->pre_cb;
    trans->post_cb = cfg->post_cb;

    /* DMA init */
    dma_transaction_cfg_t dma_cfg = {0};
    dma_cfg.priority = cfg->dma_priority;
    dma_cfg.read_block_size = DMA_BLOCK_32BIT;
    dma_cfg.write_block_size = DMA_BLOCK_32BIT;
    dma_cfg.read_burst_size = 2;
    dma_cfg.write_burst_size = 2;
    if (cfg->host == DAC_0)
    {
        dma_cfg.read_request = DMA_DAC_0_REQUEST;
        dma_cfg.write_request = DMA_DAC_0_REQUEST;
    }
    else if (cfg->host == DAC_1)
    {
        dma_cfg.read_request = DMA_DAC_1_REQUEST;
        dma_cfg.write_request = DMA_DAC_1_REQUEST;
    }
    else return DMA_STATUS_INCORRECT_ARGUMENT;
    dma_cfg.irq_en = 0;
    dma_cfg.read_mode = DMA_MEMORY_MODE;
    dma_cfg.write_mode = DMA_PERIPHERY_MODE;
    dma_cfg.read_increment = 1;
    dma_cfg.write_increment = 0;
    dma_cfg.read_ack_en = 0;
    dma_cfg.write_ack_en = 0;
    dma_cfg.src_address = cfg->src;
    dma_cfg.dst_address = (uint32_t)&(trans->host->VALUE);
    dma_cfg.channel = cfg->dma_channel,
    dma_cfg.transaction_len = cfg->len;
    dma_status_t ret = dma_transaction_init(&(trans->dma_transaction), &dma_cfg);

    /* DAC pins init */
    __HAL_PCC_ANALOG_REGS_CLK_ENABLE();
    GPIO_InitTypeDef dac_pins_cfg = {
        .Mode = HAL_GPIO_MODE_ANALOG,
        .Pull = HAL_GPIO_PULL_NONE,
    };
    
    if (trans->host == DAC_0)
    {
        dac_pins_cfg.Pin = GPIO_PIN_12;
    }
    else if (trans->host == DAC_1)
    {
        dac_pins_cfg.Pin = GPIO_PIN_13;
    }
    else return DMA_STATUS_INCORRECT_ARGUMENT;
    HAL_GPIO_Init(GPIO_1, &dac_pins_cfg);

    /* DAC init */
    /* Reset settings */
    trans->host->CFG = 0;
    /* Reset DAC */
    trans->host->CFG |= DAC_CFG_EN_M;
    trans->host->CFG |= DAC_CFG_RN_M;
    /* Set divider */
    uint32_t clk_freq = HAL_PCC_GetSysClockFreq() / ((PM->DIV_AHB+1) * (PM->DIV_APB_P+1));
    if (cfg->rate > 1000000) cfg->rate = 1000000;
    uint32_t divider = clk_freq / cfg->rate;
    if (divider > 255) divider = 255;
    else if (divider != 0) divider -= 1;
    uint32_t dac_config = trans->host->CFG;
    dac_config &= ~DAC_CFG_DIV_M;
    dac_config |= divider << DAC_CFG_DIV_S;
    trans->host->CFG = dac_config;
    /* Reference voltage source setting */
    dac_config |=
        (0 << DAC_CFG_EXTEN_S) |
        (0 << DAC_EXTPAD_S);
    trans->host->CFG = dac_config;
}


dma_status_t RAM_ATTR dac_transaction_start(dac_transaction_t *trans)
{
    if (trans->pre_cb != NULL)
    {
        trans->pre_cb(trans->token);
    }
    return dma_transaction_start(&(trans->dma_transaction));
}

bool RAM_ATTR dac_transaction_ready(dac_transaction_t *trans)
{
    return dma_transaction_ready(&(trans->dma_transaction));
}

dma_status_t RAM_ATTR dac_transaction_wait(dac_transaction_t *trans, uint32_t timeout_us)
{
    if (timeout_us == DMA_TIMEOUT_AUTO)
    {
        uint32_t clk_freq = HAL_PCC_GetSysClockFreq() / ((PM->DIV_AHB+1) * (PM->DIV_APB_P+1));
        uint32_t divider = (trans->host->CFG & DAC_CFG_DIV_M) >> DAC_CFG_DIV_S;
        uint32_t rate = clk_freq / (divider+1);
        /* DAC maximum rate is 1MHz */
        uint32_t period = 1000000 / rate;
        timeout_us = period * (trans->dma_transaction.config.LEN+1);
    }
    dma_status_t res = dma_transaction_wait(&(trans->dma_transaction), timeout_us);
    if (trans->post_cb != NULL)
    {
        trans->post_cb(trans->token);
    }
    return res;
}