#include "mik32_hal.h"
#include "mik32_hal_usart.h"
#include "mik32_hal_scr1_timer.h"
#include "usart_transaction.h"
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


usart_transaction_t tx_trans;
usart_transaction_t rx_trans;

int main()
{
    HAL_Init();

    SystemClock_Config();
    HAL_Time_SCR1TIM_Init();
    USART_Init();
    
    usart_transaction_cfg_t tx_cfg = {
        .host = UART_0,
        .dma_channel = 0,
        .dma_priority = 3,
        .direction = USART_TRANSACTION_TRANSMIT
    };
    usart_transaction_init(&tx_trans, &tx_cfg);

    usart_transaction_cfg_t rx_cfg = {
        .host = UART_0,
        .dma_channel = 0,
        .dma_priority = 3,
        .direction = USART_TRANSACTION_RECEIVE
    };
    usart_transaction_init(&rx_trans, &rx_cfg);

    char arr[3];
    xprintf("USART DMA mirror example");
    xprintf("Enter 3 random symbols. The program will read and return it dia DMA.\n");

    while (1)
    {
        bool rx_timeout = false;
        usart_transaction_start(&rx_trans, arr, sizeof(arr));
        if (usart_transaction_wait(&rx_trans, 10000000))
        {
            xprintf("Receiving timeout\n");
            rx_timeout = true;
        }

        if (!rx_timeout)
        {
            usart_transaction_start(&tx_trans, arr, sizeof(arr));
            if (usart_transaction_wait(&tx_trans, 10000))
            {
                xprintf("Writing timeout\n");
            }
        }
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
    husart0.transmitting = 1;
    husart0.receiving = 1;
    husart0.frame = Frame_8bit;
    husart0.parity_bit = 0;
    husart0.parity_bit_inversion = 0;
    husart0.bit_direction = LSB_First;
    husart0.data_inversion = 0;
    husart0.tx_inversion = 0;
    husart0.rx_inversion = 0;
    husart0.swap = 0;
    husart0.lbm = 0;
    husart0.stop_bit = StopBit_1;
    husart0.mode = Asynchronous_Mode;
    husart0.xck_mode = XCK_Mode0;
    husart0.last_byte_clock = 0;
    husart0.overwrite = 0;
    husart0.rts_mode = AlwaysEnable_mode;
    /* Разрешить активацию DMA по запросу USART0 */
    husart0.dma_tx_request = 1;
    husart0.dma_rx_request = 1;
    husart0.channel_mode = Duplex_Mode;
    husart0.tx_break_mode = 0;
    husart0.Interrupt.ctsie = 0;
    husart0.Interrupt.eie = 0;
    husart0.Interrupt.idleie = 0;
    husart0.Interrupt.lbdie = 0;
    husart0.Interrupt.peie = 0;
    husart0.Interrupt.rxneie = 0;
    husart0.Interrupt.tcie = 0;
    husart0.Interrupt.txeie = 0;
    husart0.Modem.rts = 0; //out
    husart0.Modem.cts = 0; //in
    husart0.Modem.dtr = 0; //out
    husart0.Modem.dcd = 0; //in
    husart0.Modem.dsr = 0; //in
    husart0.Modem.ri = 0;  //in
    husart0.Modem.ddis = 0;//out
    husart0.baudrate = 9600;
    HAL_USART_Init(&husart0);
}