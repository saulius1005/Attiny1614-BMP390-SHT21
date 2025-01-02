/*
 * BMP390.c
 *
 * Created: 2024-03-10 13:34:01
 *  Author: Saulius
 */ 
#include "Settings.h"

void BMP390_SPI_init(){
 PORTA.OUTCLR = PIN4_bm; //Pull SS low to switch from I2C to SPI
}