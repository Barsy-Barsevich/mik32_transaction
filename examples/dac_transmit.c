#include "mik32_hal.h"
#include "mik32_hal_timer32.h"
#include "dma_transaction.h"
#include "mik32_hal_usart.h"
#include "mik32_hal_scr1_timer.h"
#include "usart_transaction.h"

#include "uart_lib.h"
#include "xprintf.h"

#include "mik32_stdout.h"
#include "dac_transaction.h"

/*
 * Пример демонстрирует работу с DMA
 *
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

USART_HandleTypeDef husart0;

void SystemClock_Config(void);
void USART_Init(void);


usart_transaction_t trans;




dac_transaction_t dac_trans;

uint32_t word_src[] = {
    1706, 1813, 1920, 2026, 2131, 2234, 2334,
    2433, 2528, 2621, 2709, 2794, 2874, 2950,
    3021, 3087, 3147, 3201, 3250, 3293, 3329,
    3359, 3382, 3399, 3409, 3413, 3409, 3399,
    3382, 3359, 3329, 3293, 3250, 3201, 3147,
    3087, 3021, 2950, 2874, 2794, 2709, 2621,
    2528, 2433, 2334, 2234, 2131, 2026, 1920,
    1813, 1706, 1599, 1492, 1387, 1282, 1179,
    1078, 980, 884, 792, 703, 619, 538,
    462, 392, 326, 266, 211, 162, 120,
    84, 54, 30, 13, 3, 0, 3,
    13, 30, 54, 84, 120, 162, 211,
    266, 326, 392, 462, 538, 619, 703,
    792, 884, 980, 1078, 1179, 1282, 1387,
    1492, 1599
};

int main()
{
    HAL_Init();

    SystemClock_Config();
    HAL_Time_SCR1TIM_Init();
    USART_Init();
    mik32_stdout_init(UART_0, 9600);

    dma_timer_sync_cfg_t timer_cfg = {
        .host = TIMER32_0,
        .rate = 10000
    };

    dac_transaction_cfg_t cfg = {
        .host = DAC_1,
        .dma_channel = 3,
        .dma_priority = 1,
        .src = word_src,
        .len = sizeof(word_src),
        .dac_rate = 1000000,
        .sync = DMA_SYNC_TIMER,
        .timer_sync_cfg = &timer_cfg,
        .token = 0,
        .pre_cb = NULL,
        .post_cb = NULL,
    };
    dma_status_decoder(dac_transaction_init(&dac_trans, &cfg));
    uint32_t clk_freq = HAL_PCC_GetSysClockFreq() / ((PM->DIV_AHB+1) * (PM->DIV_APB_P+1));
    uint32_t divider = (dac_trans.host->CFG & DAC_CFG_DIV_M) >> DAC_CFG_DIV_S;
    uint32_t rate = clk_freq / (divider+1);

    while (1)
    {
        dac_transaction_start(&dac_trans);
        dac_transaction_wait(&dac_trans, 100000);
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


void USART_Init()
{
    husart0.Instance = UART_0;
    husart0.transmitting = Enable;
    husart0.receiving = Enable;
    husart0.frame = Frame_8bit;
    husart0.parity_bit = Disable;
    husart0.parity_bit_inversion = Disable;
    husart0.bit_direction = LSB_First;
    husart0.data_inversion = Disable;
    husart0.tx_inversion = Disable;
    husart0.rx_inversion = Disable;
    husart0.swap = Disable;
    husart0.lbm = Disable;
    husart0.stop_bit = StopBit_1;
    husart0.mode = Asynchronous_Mode;
    husart0.xck_mode = XCK_Mode0;
    husart0.last_byte_clock = Disable;
    husart0.overwrite = Disable;
    husart0.rts_mode = AlwaysEnable_mode;
    /* Разрешить активацию DMA по запросу USART0 */
    husart0.dma_tx_request = Enable;
    husart0.dma_rx_request = Enable;
    husart0.channel_mode = Duplex_Mode;
    husart0.tx_break_mode = Disable;
    husart0.Interrupt.ctsie = Disable;
    husart0.Interrupt.eie = Disable;
    husart0.Interrupt.idleie = Disable;
    husart0.Interrupt.lbdie = Disable;
    husart0.Interrupt.peie = Disable;
    husart0.Interrupt.rxneie = Disable;
    husart0.Interrupt.tcie = Disable;
    husart0.Interrupt.txeie = Disable;
    husart0.Modem.rts = Disable; //out
    husart0.Modem.cts = Disable; //in
    husart0.Modem.dtr = Disable; //out
    husart0.Modem.dcd = Disable; //in
    husart0.Modem.dsr = Disable; //in
    husart0.Modem.ri = Disable;  //in
    husart0.Modem.ddis = Disable;//out
    husart0.baudrate = 9600;
    HAL_USART_Init(&husart0);
}