#include "hal_i2c.h"


__attribute__((weak)) void hal_i2c_mspinit(i2c_t* hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (hi2c->host == I2C_0)
    {
        __HAL_PCC_I2C_0_CLK_ENABLE();
        
        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_InitStruct.Mode = HAL_GPIO_MODE_SERIAL;
        GPIO_InitStruct.Pull = HAL_GPIO_PULL_UP;
        HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
    }
    if (hi2c->host == I2C_1)
    {
        __HAL_PCC_I2C_1_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13;
        GPIO_InitStruct.Mode = HAL_GPIO_MODE_SERIAL;
        GPIO_InitStruct.Pull = HAL_GPIO_PULL_UP;
        HAL_GPIO_Init(GPIO_1, &GPIO_InitStruct);
    }
}



void hal_i2c_Reset(i2c_t *hi2c)
{
    hi2c->error_code = I2C_ERROR_NONE;
    hi2c->host->CR1 &= ~I2C_CR1_PE_M;
    hi2c->host->CR1 |= I2C_CR1_PE_M;
}

void hal_i2c_AnalogFilterInit(i2c_t *hi2c, bool analog_filter_en)
{
    hi2c->host->CR1 &= ~I2C_CR1_ANFOFF_M;
    hi2c->host->CR1 |= (analog_filter_en & 1) << I2C_CR1_ANFOFF_S;
}

HAL_Status_t hal_i2c_DigitalFilterInit(i2c_t *hi2c, hal_i2c_digital_filter_t df)
{
    if ((df != 0) && (df != 1) && (df != 2) && (df != 15))
        return HAL_DMA_INCORRECT_ARGUMENT;
    /* DNF можно менять только при PE = 0 */
    hi2c->host->CR1 &= ~I2C_CR1_PE_M;
    hi2c->host->CR1 &= ~I2C_CR1_DNF_M;
    hi2c->host->CR1 |= I2C_CR1_DNF(df);
    return HAL_DMA_OK;
}

void hal_i2c_NoStretchMode(i2c_t *hi2c, bool nostretch_en)
{
    /* NOSTRETCH можно менять только при PE = 0 */
    hi2c->host->CR1 &= ~I2C_CR1_PE_M;
    hi2c->host->CR1 &= ~I2C_CR1_NOSTRETCH_M;
    hi2c->host->CR1 |= (nostretch_en & 1) << I2C_CR1_NOSTRETCH_S;
}

void hal_i2c_NoStretch_Enable(i2c_t *hi2c)
{
    /* NOSTRETCH можно менять только при PE = 0 */
    hi2c->host->CR1 &= ~I2C_CR1_PE_M;
    hi2c->host->CR1 |= 1 << I2C_CR1_NOSTRETCH_S;
}

void hal_i2c_NoStretch_Disable(i2c_t *hi2c)
{
    /* NOSTRETCH можно менять только при PE = 0 */
    hi2c->host->CR1 &= ~I2C_CR1_PE_M;
    hi2c->host->CR1 &= ~I2C_CR1_NOSTRETCH_M;
}

// void HAL_I2C_OwnAddress1(i2c_t *hi2c)
// {
//     /* OA1, OA1MODE можно менять при OA1EN = 0 */
//     hi2c->host->OAR1 = 0;

//     /* Запись собственного адреса 1 */
//     if (hi2c->Init.OwnAddress1 <= I2C_ADDRESS_7BIT_MAX) /* Настройка 7 битного адреса */
//     {
//         hi2c->host->OAR1 &= ~I2C_OAR1_OA1MODE_M; /* 7 битный режим */
//         hi2c->host->OAR1 |= ((hi2c->Init.OwnAddress1 & 0x7F) << 1) << I2C_OAR1_OA1_S;
//     }
//     else /* Настройка 10 битного адреса */
//     {
//         hi2c->host->OAR1 |= I2C_OAR1_OA1MODE_M; /* 10 битный режим */
//         hi2c->host->OAR1 |= (hi2c->Init.OwnAddress1 & 0x3FF) << I2C_OAR1_OA1_S;
//     }

//     /* Запись собственного адреса 1 */
//     if (hi2c->Init.OwnAddress1 <= I2C_ADDRESS_7BIT_MAX) /* 7 битный адрес */
//     {
//         hi2c->host->OAR1 |= ((hi2c->Init.OwnAddress1 & 0x7F) << 1) << I2C_OAR1_OA1_S;
//     }
//     else /* 10 битный адрес */
//     {
//         hi2c->host->OAR1 |= (hi2c->Init.OwnAddress1 & 0x3FF) << I2C_OAR1_OA1_S;
//     }
//     hi2c->host->OAR1 |= I2C_OAR1_OA1EN_M;
    
// }

// void HAL_I2C_OwnAddress2(I2C_HandleTypeDef *hi2c)
// {
//     /* OA2, OA2MODE, OA2MSK можно менять при OA2EN = 0 */
//     hi2c->Instance->OAR2 = 0;

//     hi2c->Instance->OAR2 |= hi2c->Init.OwnAddress2Mask << I2C_OAR2_OA2MSK_S;

//     /* Запись собственного адреса 2 */
//     hi2c->Instance->OAR2 |= (hi2c->Init.OwnAddress2 & 0x7F) << I2C_OAR2_OA2_S;

//     /* Включить дополнительный собственный адрес 2 */
//     if (hi2c->Init.DualAddressMode == I2C_DUALADDRESS_ENABLE)
//     {
//         hi2c->Instance->OAR2 |= I2C_OAR2_OA2EN_M;
//     }
    
// }

// void HAL_I2C_GeneralCall(I2C_HandleTypeDef *hi2c, HAL_I2C_GeneralCallTypeDef GeneralCall)
// {
//     hi2c->Init.GeneralCall = GeneralCall;
//     hi2c->Instance->CR1 &= ~I2C_CR1_GCEN_M;
//     hi2c->Instance->CR1 |= GeneralCall << I2C_CR1_GCEN_S;
// }

// void HAL_I2C_SBCMode(I2C_HandleTypeDef *hi2c, HAL_I2C_SBCModeTypeDef SBCMode)
// {
//     hi2c->Init.SBCMode = SBCMode;
//     hi2c->Instance->CR1 &= ~I2C_CR1_SBC_M;
//     hi2c->Instance->CR1 |= SBCMode << I2C_CR1_SBC_S;
// }


void hal_i2c_set_addr(i2c_t *hi2c, uint16_t addr, bool use_10bit_addr)
{
    hi2c->host->CR2 &= ~I2C_CR2_SADD_M;

    if (!use_10bit_addr) /* 7 битный адрес */
    {
        hi2c->host->CR2 &= ~I2C_CR2_ADD10_M;
        hi2c->host->CR2 |= ((addr & 0x7F) << 1) << I2C_CR2_SADD_S;
    }
    else /* 10 битный адрес */
    {
        hi2c->host->CR2 |= I2C_CR2_ADD10_M;
        /* ведущий отправляет полную последовательность для чтения для 10 битного адреса */
        hi2c->host->CR2 &= ~I2C_CR2_HEAD10R_M;
        hi2c->host->CR2 |= (addr & 0x3FF) << I2C_CR2_SADD_S;
    }
}


void hal_i2c_SlaveInit(i2c_t *hi2c)
{
    // /* Основной и дополнительный собственный адрес */
    // HAL_I2C_OwnAddress1(hi2c);
    // HAL_I2C_OwnAddress2(hi2c);

    // /* GCEN - general call  */
    // HAL_I2C_GeneralCall(hi2c, hi2c->Init.GeneralCall);
    
    // /* Configure SBC in I2C_CR1* */
    // HAL_I2C_SBCMode(hi2c, hi2c->Init.SBCMode);

    // /* Enable interrupts and/or DMA in I2C_CR1 */
}

void hal_i2c_MasterInit(i2c_t *hi2c)
{
    /* Enable interrupts and/or DMA in I2C_CR1 */

}

HAL_Status_t hal_i2c_SetClockSpeed(i2c_t *hi2c, hal_i2c_scl_coef_t *cfg)
{
    if (cfg->PRESC > 15) return HAL_DMA_INCORRECT_ARGUMENT;
    if (cfg->SCLDEL > 15) return HAL_DMA_INCORRECT_ARGUMENT;
    if (cfg->SDADEL > 15) return HAL_DMA_INCORRECT_ARGUMENT;
    /* TIMING можно менять только при PE = 0 */
    hi2c->host->CR1 &= ~I2C_CR1_PE_M;
    hi2c->host->TIMINGR = I2C_TIMINGR_SCLDEL(cfg->SCLDEL) | I2C_TIMINGR_SDADEL(cfg->SDADEL) |
        I2C_TIMINGR_SCLL(cfg->SCLL) | I2C_TIMINGR_SCLH(cfg->SCLH) | I2C_TIMINGR_PRESC(cfg->PRESC);
    return HAL_DMA_OK;
}

void hal_i2c_calcFreqCoef(i2c_t *hi2c, hal_i2c_scl_coef_t *coef, uint32_t freq, uint32_t duty_factor)
{
    uint32_t i2cclk_freq;
    uint32_t i2c_cycles;
    uint32_t presc_del;

    i2cclk_freq = HAL_PCC_GetSysClockFreq() / (PM->DIV_APB_P+1);
    i2c_cycles = i2cclk_freq / freq - TSYNC_SIGMA_FACTOR;
    //so SCLL & SCLH are 7-bit, their summ is 8-bit, d256 max
    presc_del = i2c_cycles / 256;
    i2c_cycles = i2c_cycles / (presc_del + 1);
    coef->PRESC = presc_del;
    coef->SCLDEL = 1;
    coef->SDADEL = 1;
    coef->SCLH = i2c_cycles / duty_factor;
    coef->SCLL = i2c_cycles - coef->SCLH;
    coef->SCLH -= 1;
    coef->SCLL -= 1;
}

HAL_Status_t hal_i2c_init(i2c_t *hi2c, i2c_cfg_t *cfg)
{
    HAL_Status_t res;
    hi2c->host = cfg->host;
    hal_i2c_mspinit(hi2c);
    /* Set frequency by calculation if frequency field is not equal to 0 */
    if (cfg->frequency != 0)
        hal_i2c_calcFreqCoef(hi2c, &(cfg->scl_coef), cfg->frequency, cfg->duty_factor);
    /* Clear PE in I2C_CR1 */
    hal_i2c_Disable(hi2c);

    /* Configure ANFOFF and DNF[3:0] in I2C_CR1 */
    hal_i2c_AnalogFilterInit(hi2c, cfg->analog_filter_en);
    res = hal_i2c_DigitalFilterInit(hi2c, cfg->digital_filter_mode);
    if (res != HAL_DMA_OK) return res;

    /* Configure PRESC[3:0], SDADEL[3:0], SCLDEL[3:0], SCLH[7:0]，SCLL[7:0] in l2C_TIMINGR */
    hal_i2c_SetClockSpeed(hi2c, &(cfg->scl_coef));

    if (cfg->mode == I2C_MODE_MASTER)
    {
        hal_i2c_NoStretch_Disable(hi2c);
    }
    else
    {
        if (cfg->nostretch_mode_en)
            hal_i2c_NoStretch_Enable(hi2c);
        else
            hal_i2c_NoStretch_Disable(hi2c);
    }

    /* Set PE in I2C_CR1 */
    hal_i2c_Enable(hi2c);

    if (cfg->mode == I2C_MODE_SLAVE)
        hal_i2c_SlaveInit(hi2c);
    else if (cfg->mode == I2C_MODE_MASTER)
        hal_i2c_MasterInit(hi2c);
    else
        return HAL_DMA_INCORRECT_ARGUMENT;
    
    hi2c->state = HAL_I2C_STATE_READY;
    return HAL_DMA_OK;
}