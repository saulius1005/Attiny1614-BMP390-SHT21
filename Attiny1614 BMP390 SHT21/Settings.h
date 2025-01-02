/*
 * Settings.h
 *
 * Created: 2024-03-08 19:46:22
 *  Author: Saulius
 */ 


#ifndef SETTINGS_H_
#define SETTINGS_H_

#define F_CPU 20000000
#define TWI0_BAUD(F_SCL, T_RISE) ((((((float)F_CPU / (float)F_SCL)) - 10 - ((float)F_CPU * T_RISE / 1000000))) / 2) //I2C baud calculation
#define W 0 //I2C write
#define R 1 //I2C read
#define NO_STOP 0 //for TranssmitByte() function
#define STOP 1 //for TranssmitByte() function
#define USE_CRC 0 //for Separator function
#define IGNORE_CRC 1 //for Separator function

#define SHT21_ADD 0b1000000 //binary address for SHT sensor in hex it will be 0x40 (not including W or R)

#define HOLD_MASTER_T_MES 0b11100011 //register address
#define HOLD_MASTER_RH_MES 0b11100101
#define NO_HOLD_MASTER_T_MES 0b11110011
#define NO_HOLD_MASTER_RH_MES 0b11110101
#define W_USER_REG 0b11100110
#define R_USER_REG 0b11100111
#define SOFT_RESET 0b11111110

#define OTP_ADD 1 //1bit
#define HEATER_ADD 2 //2bit //3..5 bits are reserved
#define BATTERY_ADD 6 //6bit
#define RESOLUTION_ADD 129 //(0 and 7 bits value)

#define RH_12b_T_14b 0 //T and RH resolution values
#define RH_8b_T_12b 1
#define RH_10b_T_13b 128
#define RH_11b_T_11b 129

#define ON 1 //for SHT21_Settings_Read and SHT21_Reset functions
#define OFF 0 //for SHT21_Settings_Read and SHT21_Reset functions

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct SettingsSHT21{
 uint8_t OTP_DISABLE; //Restore factory settings only
 uint8_t Heater; // turn on or off integrated heater
 uint8_t Resolution; //T and RH measuring resolution
 uint8_t Battery; //2.25V detection on or off
 float T; //calculated temperature in C degrees
 float RH; //calculated Humidity %
 uint8_t Fault; //0- CRC correct 1- Incorrect CRC (if enabled)
}SHT21;

struct Error{
	uint8_t Fault; //1-NACK ADD, 2-NACK Reading, 3-NACK Sending
}I2C;

void GPIO_init(); //Attiny1614 CPU speed and Pins settings

uint8_t CRC(uint32_t command); //CRC decoder for crc8 Dallas/MAXIM

void I2C_init(); //I2C settings
uint8_t TranssmitAdd(uint8_t Add, uint8_t Read);
uint8_t TranssmitByte(uint8_t Data, uint8_t stop);
uint8_t ReadByteACK();
uint8_t ReadByteNACK();
void WriteToReg(uint8_t Add, uint8_t Reg, uint8_t Data);
void WriteMulti(uint8_t Add, uint64_t data, uint8_t bytes);
uint8_t ReadReg(uint8_t Add, uint8_t Reg);
uint64_t ReadMulti(uint8_t Add, uint8_t FReg, uint8_t bytes);// address, first register, bytes count 1-4 (1-8bites, 2-16bites ...)
uint8_t Stop();

void SHT21_Settings_Read(); //for settings reading for SHT21
void SHT21_Settings_Write(); //Setings writing function
void SHT21_Reset(uint8_t save); //Reset and restore SHT21 to factory default settings if save 0- only load default settings 1-save those settings for further useage
uint32_t SHT21_Read(uint8_t mode); //return T or RH raw data including crc
uint8_t Separator(uint32_t data, uint8_t ignoreCRC); //return 0 if crc correct and 1 if incorrect (if CRC is enabled)

#endif /* SETTINGS_H_ */