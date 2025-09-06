#include "dma_timer.h"

bool dma_sync_timer_init(dma_timer_sync_cfg_t *cfg, bool external_clock)
{
    if (cfg->host != TIMER32_0 &&
        cfg->host != TIMER32_1 &&
        cfg->host != TIMER32_2
    ) return false;

    uint32_t hclk = HAL_PCC_GetSysClockFreq();
    switch ((uint32_t)cfg->host)
    {
        case (uint32_t)TIMER32_0:
            __HAL_PCC_TIMER32_0_CLK_ENABLE();
            hclk /= ((PM->DIV_AHB+1) * (PM->DIV_APB_M+1));
            break;
        case (uint32_t)TIMER32_1:
            __HAL_PCC_TIMER32_1_CLK_ENABLE();
            hclk /= ((PM->DIV_AHB+1) * (PM->DIV_APB_P+1));
            break;
        case (uint32_t)TIMER32_2:
            __HAL_PCC_TIMER32_2_CLK_ENABLE();
            hclk /= ((PM->DIV_AHB+1) * (PM->DIV_APB_P+1));
            break;
    }
    cfg->host->INT_MASK = 0;
    cfg->host->INT_CLEAR = 0xFFFFFFFF;
    cfg->host->ENABLE = 1;
    /* Top & prescaler setting */
    uint32_t top = hclk / cfg->rate;
    uint32_t prescaler = 0;//(top / 0x10000) + (top&0xFFFF) == 0 ? -1 : 0;
    top /= (prescaler+1);
    printf("top: %u; presc: %u\n", top, prescaler);
    cfg->host->TOP = top;
    cfg->host->PRESCALER = prescaler;
    /* Mode: incrementing */
    cfg->host->CONTROL = 0;
    cfg->host->CONTROL |= (0b00 << TIMER32_CONTROL_MODE_S);
    /* Clock setting */
    if (external_clock)
    {
        cfg->host->TOP = 1;
        cfg->host->CONTROL |= TIMER32_CONTROL_CLOCK_TX_PIN_M;
    }
    else
    {
        /* AHB/(PRESCALER-1) source */
        cfg->host->CONTROL |= TIMER32_CONTROL_CLOCK_PRESCALER_M;
    }
    cfg->host->ENABLE = 0b11;
    return true;
}