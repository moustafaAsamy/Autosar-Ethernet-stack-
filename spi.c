/*
 * spi.c
 *
 *  Created on: Oct 22, 2019
 *      Author: lenovo
 */


#include <inc/hw_ints.h>
#include <stdint.h>
#include "spi.h"
#include "driverlib/rom_map.h"
#include <driverlib/interrupt.h>
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"


/* Pins */
//#define ENC_CS_PORT         GPIO_PORTD_BASE
//#define ENC_INT_PORT        GPIO_PORTD_BASE
////#define ENC_RESET_PORT        GPIO_PORTA_BASE
//#define ENC_CS          GPIO_PIN_1
//#define ENC_INT         GPIO_PIN_7
////#define ENC_RESET     GPIO_PIN_2



void spi_init(void) {
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
  MAP_GPIOPinConfigure(GPIO_PD0_SSI1CLK);
  MAP_GPIOPinConfigure(GPIO_PD2_SSI1RX);
  MAP_GPIOPinConfigure(GPIO_PD3_SSI1TX);
  MAP_GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_3 | GPIO_PIN_2);
  MAP_SSIConfigSetExpClk(SSI1_BASE, MAP_SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,SSI_MODE_MASTER, 1000000, 8);
  MAP_SSIEnable(SSI1_BASE);
  unsigned long b;
  while(MAP_SSIDataGetNonBlocking(SSI1_BASE, &b)) {}
}

void gpio_comm_init(void) {
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
  MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
  MAP_GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, 4|2|8);
  MAP_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, ENC_CS);
  MAP_GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, ENC_INT);
  MAP_GPIOPinWrite(ENC_CS_PORT, ENC_CS, ENC_CS);
  MAP_GPIOPinWrite(GPIO_PORTF_BASE, 4, 0);
  MAP_IntEnable(INT_GPIOD);
  MAP_IntMasterEnable();
  MAP_GPIOIntTypeSet(GPIO_PORTD_BASE, ENC_INT, GPIO_FALLING_EDGE);
  GPIOIntClear(GPIO_PORTD_BASE, ENC_INT);
  GPIOIntEnable(GPIO_PORTD_BASE, ENC_INT);
}

uint8_t spi_send(uint8_t c) {
  unsigned long val;
  MAP_SSIDataPut(SSI1_BASE, c);
  MAP_SSIDataGet(SSI1_BASE, &val);
  return (uint8_t)val;
}
