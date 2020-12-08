/*
 * led.h
 *
 *  Created on: Oct 23, 2019
 *      Author: lenovo
 */

#include "stdint.h"
#include "driverlib/rom_map.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#ifndef LED_H_
#define LED_H_


#define red       0x2
#define blue      0x4
#define green     0x8
#define yellow    8|2
#define white     4|8|2
#define indego    2|4
#define all     4|8|2
void led_off(void);
void led_on (uint8_t led);

#endif /* LED_H_ */
