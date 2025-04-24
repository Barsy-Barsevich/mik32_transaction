#include "mik32_hal_i2c.h"

#include "uart_lib.h"
#include "xprintf.h"
#include "i2c_transaction.h"

#include "mik32_hal_scr1_timer.h"

/*
* В данном примере демонстрируется работа I2C в режиме ведущего.
* Ведущий записывает по адресу 0x36 10 байт, а затем считывает.
* Используется режим автоматического окончания.
*
* Данный пример может быть использован совместно с ведомым из примера Hal_I2C_Slave.
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
i2c_transaction_t trans;


int main()
{    

    SystemClock_Config();
    HAL_Time_SCR1TIM_Init();

    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    I2C0_Init();


    i2c_transaction_cfg_t cfg = {
        .host = I2C_0,
        .dma_channel = 0,
        .dma_priority = 3,
        .direction = I2C_TRANSACTION_TRANSMIT,
        .address = 0x68,
        .use_10bit_address = false,
    };
    i2c_transaction_init(&trans, &cfg);


    char arr[] = {0x22, 0xDD, 0x77};
    
    while (1)
    {    
        if (i2c_transmit_start(&trans, arr, sizeof(arr)) != HAL_DMA_OK)
            xprintf("Start error\n");
        if (i2c_transaction_end(&trans, 100000) != HAL_DMA_OK)
            xprintf("Wait error\n");
        
        i2c_transaction_err_decode(&trans);
        // HAL_StatusTypeDef res = HAL_I2C_Master_Transmit(&hi2c0, 0x68, (uint8_t*)arr, 3, 1000000);
        // xprintf("ans: %d\n", res);
        // HAL_DelayMs(1000);
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
    hi2c0.Init.frequency = 400000;
    hi2c0.Init.duty_factor = 2;


    if (HAL_I2C_Init(&hi2c0) != HAL_OK)
    {
        xprintf("I2C_Init error\n");
    }

}