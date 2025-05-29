#include "mik32_hal_i2c.h"

#include "uart_lib.h"
#include "xprintf.h"
#include "i2c_transaction.h"

#include "mik32_hal_scr1_timer.h"

/**
 * This example shows how to receive data via I2C using i2c_transaction library.
 * 
 * This example reads WHO_AM_I register from MPU6050.
 * 
 * A new option - DMA_CH_AUTO - is available now.
 * If the dma channel chosen as DMA_CH_AUTO, the library checks DMA channels'
 * ready status each time when transaction started. I takes a bit extra processor's
 * time but provides more abstract access to DMA resources.
 */

uint32_t HAL_Micros()
{
    return HAL_Time_SCR1TIM_Micros();
}
uint32_t HAL_Millis()
{
    return HAL_Time_SCR1TIM_Millis();
}
void HAL_DelayUs(uint32_t time_us)
{
    HAL_Time_SCR1TIM_DelayUs(time_us);
}
void HAL_DelayMs(uint32_t time_ms)
{
    HAL_Time_SCR1TIM_DelayMs(time_ms);
}

void SystemClock_Config(void);
static void I2C0_Init(void);


I2C_HandleTypeDef hi2c0;
i2c_transaction_t tx_trans;
i2c_transaction_t rx_trans;

int main()
{    

    SystemClock_Config();
    HAL_Time_SCR1TIM_Init();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    I2C0_Init();

    i2c_transaction_cfg_t tx_cfg = {
        .host = I2C_0,
        .dma_channel = DMA_CH_AUTO,
        .dma_priority = 3,
        .direction = I2C_TRANSACTION_TRANSMIT,
        .address = 0x68,
        .use_10bit_address = false,
    };
    i2c_transaction_init(&tx_trans, &tx_cfg);

    i2c_transaction_cfg_t rx_cfg = {
        .host = I2C_0,
        .dma_channel = DMA_CH_AUTO,
        .dma_priority = 3,
        .direction = I2C_TRANSACTION_RECEIVE,
        .address = 0x68,
        .use_10bit_address = false,
    };
    i2c_transaction_init(&rx_trans, &rx_cfg);


    char reg_addr[] = {0x75};
    char buf[10];
    
    while (1)
    {    
        if (i2c_transmit(&tx_trans, reg_addr, sizeof(reg_addr), 1000) != DMA_STATUS_OK)
        {
            i2c_transaction_err_decode(&tx_trans);
        }
        // HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(&hi2c0, 0x68, (uint8_t*)reg_addr, 1, 1000000);
        // xprintf("ans: %d\n", res);

        if (i2c_receive(&rx_trans, buf, 1, 1000) != DMA_STATUS_OK)
        {
            i2c_transaction_err_decode(&rx_trans);
        }
        
        xprintf("ID: 0x%02X\n", buf[0]);
        HAL_DelayMs(1000);
    }
}

void SystemClock_Config(void)
{
    PCC_InitTypeDef PCC_OscInit = {0};

    PCC_OscInit.OscillatorEnable = PCC_OSCILLATORTYPE_ALL;
    PCC_OscInit.FreqMon.OscillatorSystem = PCC_OSCILLATORTYPE_OSC32M;
    PCC_OscInit.FreqMon.ForceOscSys = PCC_FORCE_OSC_SYS_UNFIXED;
    PCC_OscInit.FreqMon.Force32KClk = PCC_FREQ_MONITOR_SOURCE_OSC32K;
    PCC_OscInit.AHBDivider = 0;
    PCC_OscInit.APBMDivider = 0;
    PCC_OscInit.APBPDivider = 0;
    PCC_OscInit.HSI32MCalibrationValue = 128;
    PCC_OscInit.LSI32KCalibrationValue = 8;
    PCC_OscInit.RTCClockSelection = PCC_RTC_CLOCK_SOURCE_AUTO;
    PCC_OscInit.RTCClockCPUSelection = PCC_CPU_RTC_CLOCK_SOURCE_OSC32K;
    HAL_PCC_Config(&PCC_OscInit);
}

static void I2C0_Init(void)
{

    /* Общие настройки */
    hi2c0.Instance = I2C_0;

    hi2c0.Init.Mode = HAL_I2C_MODE_MASTER;

    hi2c0.Init.DigitalFilter = I2C_DIGITALFILTER_OFF;
    hi2c0.Init.AnalogFilter = I2C_ANALOGFILTER_DISABLE;
    hi2c0.Init.AutoEnd = I2C_AUTOEND_DISABLE;

    /* Настройка частоты */
    hi2c0.Init.frequency = 100000;
    hi2c0.Init.duty_factor = 2;


    if (HAL_I2C_Init(&hi2c0) != HAL_OK)
    {
        xprintf("I2C_Init error\n");
    }

}