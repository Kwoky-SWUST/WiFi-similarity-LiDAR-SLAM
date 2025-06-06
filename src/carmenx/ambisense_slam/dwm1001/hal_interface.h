/*! ------------------------------------------------------------------------------------------------------------------
 * @file    hal_interface.h
 * @brief   definition of the interface being used.
 *
 *          in makefile, define interface configuration using:
 *          INTERFACE_NUMBER = 
 *          0: INTERFACE_USE_UART (default)
 *          1: INTERFACE_USE_SPI    
 *          2: INTERFACE_USE_SPI_DRDY
 *          note: if INTERFACE_NUMBER is not defined in makefile, the default
 *                value 0 will be used (in dwm1001.mak file)
 *
 * @attention
 *
 * Copyright 2017 (c) Decawave Ltd, Dublin, Ireland.
 *
 * All rights reserved.
 *
 */
#ifndef _HAL_IF_H_
#define _HAL_IF_H_

#include "hal_uart.h"
#define HAL_IF_Tx          HAL_UART_Tx
#define HAL_IF_Rx          HAL_UART_Rx
#define HAL_IF_STR         "HAL_UART"

#endif //_HAL_IF_H_

