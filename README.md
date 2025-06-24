# MIK32 DMA libraries

## Abstract
Created to simplify DMA usage on the MIK32 platform. Provide easier initialization and faster transaction run.

Before you transmit or receive some data using these libraries, you should to create __transaction__ and initialize it. Transaction initializing takes all the long-time setting operations linking user-friendly interface and optimized internal design. When you run the transaction, it does not spend time for setup, it is produced quickly

## Headers
### `dma_transaction.h`
Common DMA-library, provides DMA initialization, DMA-transaction startup and progress control

### `usart_transaction.h`
USART DMA library. Allows to run long USART transactions while the processor process the other tasks

### `i2c_transaction.h`
I2C DMA library. As the others these libraries, optimized for speed, so produce I2C transactions faster than traditional pending method

### `spi_transaction.h`
SPI DMA library.

### `dac_transaction.h`
DAC DMA library. Sends data from array to DAC output via DMA while processor works on other tasks.
