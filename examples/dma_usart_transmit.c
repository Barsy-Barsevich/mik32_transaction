#include "mik32_hal.h"
#include "mik32_hal_timer32.h"
#include "dma_transaction.h"
#include "mik32_hal_usart.h"
#include "mik32_hal_scr1_timer.h"

#include "uart_lib.h"
#include "xprintf.h"

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

static char arr[] = "abcdefghijklmn0123456789\n";




dma_transaction_t uart_transaction;

int main()
{
    HAL_Init();

    SystemClock_Config();
    HAL_Time_SCR1TIM_Init();
    USART_Init();


    dma_transaction_cfg_t cfg = {
        .priority = 3,
        .read_mode = DMA_MEMORY_MODE,
        .write_mode = DMA_PERIPHERY_MODE,
        .read_increment = 1,
        .write_increment = 0,
        .read_block_size = DMA_BLOCK_8BIT,
        .write_block_size = DMA_BLOCK_8BIT,
        .read_burst_size = 0,
        .write_burst_size = 0,
        .read_request = DMA_USART_0_REQUEST,
        .write_request = DMA_USART_0_REQUEST,
        .read_ack_en = 0,
        .write_ack_en = 1,
        .irq_en = 0,

        .channel = 0,

        .src_address = (uint32_t)arr,
        .dst_address = (uint32_t)&(UART_0->TXDATA),
        .transaction_len = sizeof(arr),
    };
    dma_transaction_init(&uart_transaction, &cfg);

    while (1)
    {
        dma_transaction_start(&uart_transaction);
        if (dma_transaction_wait(&uart_transaction, 100000) != DMA_STATUS_OK)
        {
            xprintf("Timeout.\n");
        }
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


void USART_Init()
{
    husart0.Instance = UART_0;
    husart0.transmitting = Enable;
    husart0.receiving = Disable;
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
    husart0.dma_rx_request = Disable;
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