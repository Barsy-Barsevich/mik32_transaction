// #pragma once

// #include <stdio.h>
// #include <stdbool.h>
// #include "mik32_hal_gpio.h"
// #include "i2c.h"
// #include "mik32_memory_map.h"

// #include "dma_transaction.h"


// /* Время ожидания установки флага TXIS или RXNE в микросекундах */
// #define I2C_TIMEOUT_DEFAULT 	100000
// /* Суммарное количество тактов i2cclk, уходящих на ожидание установки лог.1 на SCL */
// #define TSYNC_SIGMA_FACTOR		10
// /* Максимальное количество байт в посылке (NBYTES) */
// #define I2C_NBYTE_MAX 	255
// /* Максимальный 7 битный адрес */
// #define I2C_ADDRESS_7BIT_MAX 	0x7F
// /* Максимальный 10 битный адрес */
// #define I2C_ADDRESS_10BIT_MAX 	0x3FF

// /* Маска для разрешенных прерываний */
// #define I2C_INTMASK 0b11111110

// /* I2C_error - номера ошибок I2C*/
// typedef enum
// {
// 	I2C_ERROR_NONE =    0,  /* Ошибок нет */
// 	I2C_ERROR_TIMEOUT = 1,  /* Превышено ожидание установки флага TXIS или RXNE */
// 	I2C_ERROR_NACK =    2,  /* Во время передачи не получено подтверждение данных (NACK) */  
// 	I2C_ERROR_BERR =    3,  /* Ошибка шины */ 
// 	I2C_ERROR_ARLO =    4,  /* Проигрыш арбитража */
// 	I2C_ERROR_OVR =     5,  /* Переполнение или недозагрузка */ 
// 	I2C_ERROR_STOP =    6,  /* Обнаружение STOP на линии */ 
// } hal_i2c_error_t;

// /* I2C_transfer_direction - Направление передачи */
// typedef enum
// {
// 	I2C_TRANSMITTING = 0,   /* Ведущий запрашивает транзакцию записи */
// 	I2C_RECEIVING = 1       /* Ведущий запрашивает транзакцию чтения */
// } hal_i2c_direction_t;

// /* I2C_OwnAddress2_mask - Маска второго собственного адреса */
// typedef enum
// {
// 	I2C_OWNADDRESS2_MASK_DISABLE = 0,    /* Нет маски */
// 	I2C_OWNADDRESS2_MASK_111111x = 1,    /* Сравниваются только OA2[7:2] */
// 	I2C_OWNADDRESS2_MASK_11111xx = 2,    /* Сравниваются только OA2[7:3]; */
// 	I2C_OWNADDRESS2_MASK_1111xxx = 3,    /* Сравниваются только OA2[7:4]; */
// 	I2C_OWNADDRESS2_MASK_111xxxx = 4,    /* Сравниваются только OA2[7:5]; */
// 	I2C_OWNADDRESS2_MASK_11xxxxx = 5,    /* Сравниваются только OA2[7:6]; */
// 	I2C_OWNADDRESS2_MASK_1xxxxxx = 6,    /* Сравниваются только OA2[7]; */
// 	I2C_OWNADDRESS2_MASK_1111111 = 7     /* OA2[7:1] маскируются, подтверждаются (ACK) все 7-битные адреса (кроме зарезервированных) */
// } hal_i2c_ownaddr2_mask_t;

// /* I2C_digital_filter - Цифровой фильтр */
// typedef enum
// {
// 	I2C_DIGITALFILTER_OFF           = 0,
// 	I2C_DIGITALFILTER_1CLOCKCYCLES  = 1,
// 	I2C_DIGITALFILTER_2CLOCKCYCLES  = 2,
// 	I2C_DIGITALFILTER_15CLOCKCYCLES = 15
// } hal_i2c_digital_filter_t;

// /* I2C_analog_filter - Цифровой фильтр */
// typedef enum
// {
// 	I2C_ANALOGFILTER_ENABLE = 1,
// 	I2C_ANALOGFILTER_DISABLE = 0
// } hal_i2c_analog_filter_t;

// /* Режим ведомый или ведущий */
// typedef enum 
// {
//     I2C_MODE_MASTER = 0,    /* Режим ведущего */
//     I2C_MODE_SLAVE = 1,     /* Режим ведомого */

// } hal_i2c_mode_t;

// typedef struct
// {
//     /* Предварительный делитель частоты I2CCLK (0..15) */
//     uint8_t PRESC;          
//     /* Длительность предустановки данных (0..15) */
//     uint8_t SCLDEL;
//     /* Длительность предустановки данных (0..15) */
//     uint8_t SDADEL;
//     /* Длительность удержания SCL в состоянии логической "1" в режиме "ведущий" (0..255) */
//     uint8_t SCLH;
//     /* Длительность удержания SCL в состоянии логического "0" в режиме "ведущий" (0..255) */
//     uint8_t SCLL;
// } hal_i2c_scl_coef_t;


// typedef struct
// {
//     I2C_TypeDef *host;
// 	hal_i2c_mode_t mode;
// 	/* Основной собственный адрес. Когда включен дополнительный адрес, длина основного адреса должна быть 7 бит */
// 	uint16_t ownaddr1;
// 	/* Дополнительный 7 битный адрес (Можно использовать, если длина основного адреса 7 бит) */
// 	uint8_t ownaddr2;
// 	/* Активация дополнительного 7 битного адреса (Можно использовать, если длина основного адреса 7 бит) */
// 	bool dualaddr_en;

// 	bool general_call_en; 
// 	/* Маска сравнения дополнительного 7 битного адреса */
// 	hal_i2c_ownaddr2_mask_t ownaddress2_mask;
// 	/* Растягивания тактового сигнала в режиме "ведомый" (Не совместим с режимом SBC) */
// 	bool nostretch_mode_en;
// 	/* Режим аппаратного контроля передачи данных в режиме "ведомый" (Не совместим с режимом NOSTRETCH) */
// 	bool sbs_en;
// 	/* Управление режимом автоматического окончания в режиме "ведущий" */
// 	bool autoend_disable;
// 	hal_i2c_digital_filter_t digital_filter_mode;
//     bool analog_filter_en;

//     hal_i2c_scl_coef_t scl_coef;
// 	uint32_t frequency;
// 	/* Скважность SCL */
// 	uint32_t duty_factor;

// } i2c_cfg_t;

// typedef enum
// {
//     HAL_I2C_STATE_READY,  /* Готов к передаче */
//     HAL_I2C_STATE_BUSY,   /* Идет передача */
//     HAL_I2C_STATE_END,    /* Передача завершена */
//     HAL_I2C_STATE_ERROR   /* Ошибка при передаче */
// } hal_i2c_state_t;


// inline void hal_i2c_Enable(I2C_TypeDef* i2c) {i2c->CR1 |= I2C_CR1_PE_M;}
// inline void hal_i2c_Disable(I2C_TypeDef* i2c) {i2c->CR1 &= ~I2C_CR1_PE_M;}

// void hal_i2c_mspinit(I2C_TypeDef* host);
// void hal_i2c_Reset(I2C_TypeDef* host);
// void hal_i2c_AnalogFilterInit(I2C_TypeDef* host, bool analog_filter_en);
// HAL_Status_t hal_i2c_DigitalFilterInit(I2C_TypeDef* host, hal_i2c_digital_filter_t df);
// void hal_i2c_NoStretchMode(I2C_TypeDef* host, bool nostretch_en);
// void hal_i2c_NoStretch_Enable(I2C_TypeDef* host);
// void hal_i2c_NoStretch_Disable(I2C_TypeDef* host);
// void hal_i2c_SlaveInit(I2C_TypeDef* host);
// void hal_i2c_MasterInit(I2C_TypeDef* host);
// HAL_Status_t hal_i2c_init(i2c_cfg_t *cfg);