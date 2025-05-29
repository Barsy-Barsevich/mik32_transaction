#include "mik32_hal_spi.h"
#include "uart_lib.h"
#include "xprintf.h"
#include "spi_transaction.h"
#include "mik32_hal_gpio.h"
/*
 * Данный пример демонстрирует работу с SPI в режиме ведущего.
 * Ведущий передает и принимает от ведомого на выводе CS0 20 байт.
 *
 * Результат передачи выводится по UART0.
 * Данный пример можно использовать совместно с ведомым из примера HAL_SPI_Slave.
 */
SPI_HandleTypeDef hspi0;
void SystemClock_Config(void);
static void SPI0_Init(void);

spi_transaction_t spi_trans;



void cs_down(uint32_t dummy)
{
    HAL_GPIO_WritePin(GPIO_0, GPIO_PIN_7, 0);
}

void cs_up(uint32_t dummy)
{
    HAL_GPIO_WritePin(GPIO_0, GPIO_PIN_7, 1);
}



int main()
{
    SystemClock_Config();
    UART_Init(UART_0, 3333, UART_CONTROL1_TE_M | UART_CONTROL1_M_8BIT_M, 0, 0);
    SPI0_Init();
    char master_output[] = {0x01, 0x55};
    
    
    spi_transaction_cfg_t spi_trans_cfg = {
        .host = SPI_0,
        .direction = SPI_TRANSACTION_TRANSMIT,
        .dma_channel = DMA_CH_AUTO,
        .dma_priority = 3,
        .pre_cb = cs_down,
        .post_cb = cs_up
    };
    spi_transaction_init(&spi_trans, &spi_trans_cfg);

    while (1)
    {
        spi_transmit(&spi_trans, master_output, sizeof(master_output), 1000);
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


static void SPI0_Init(void)
{
    hspi0.Instance = SPI_0;
    /* Режим SPI */
    hspi0.Init.SPI_Mode = HAL_SPI_MODE_MASTER;
    /* Настройки */
    hspi0.Init.BaudRateDiv = SPI_BAUDRATE_DIV64;
    hspi0.Init.CLKPhase = SPI_PHASE_ON;
    hspi0.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi0.Init.Decoder = SPI_DECODER_NONE;
    hspi0.Init.ThresholdTX = SPI_THRESHOLD_DEFAULT;
    hspi0.Init.ManualCS = SPI_MANUALCS_ON; /* Настройки ручного режима управления сигналом CS */
    hspi0.Init.ChipSelect = SPI_CS_0;       /* Выбор ведомого устройства в автоматическом режиме управления CS */
    if (HAL_SPI_Init(&hspi0) != HAL_OK)
    {
        xprintf("SPI_Init_Error\n");
    }

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_PCC_GPIO_0_CLK_ENABLE();
    __HAL_PCC_GPIO_1_CLK_ENABLE();
    __HAL_PCC_GPIO_IRQ_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = HAL_GPIO_MODE_GPIO_OUTPUT;
    GPIO_InitStruct.Pull = HAL_GPIO_PULL_NONE;
    HAL_GPIO_Init(GPIO_0, &GPIO_InitStruct);
}