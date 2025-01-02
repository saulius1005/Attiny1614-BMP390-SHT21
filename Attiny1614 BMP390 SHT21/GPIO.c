/*
 * GPIO.c
 *
 * Created: 2024-03-08 19:47:06
 *  Author: Saulius
 */ 
#include "Settings.h"

void GPIO_init(){
	CPU_CCP = CCP_IOREG_gc;
	CLKCTRL.MCLKCTRLB = 0 << CLKCTRL_PEN_bp;
	while (CLKCTRL.MCLKSTATUS & CLKCTRL_SOSC_bm);

	PORTB.DIRSET = PIN0_bm | PIN1_bm; //I2C SCL, I2C SDA
	PORTB.PIN0CTRL = PORT_PULLUPEN_bm;	//pullup
	PORTB.PIN1CTRL = PORT_PULLUPEN_bm; //pullup
}