/*
 * SHT21.c
 *
 * Created: 2024-03-15 12:08:00
 *  Author: Saulius
 */ 
#include "Settings.h"

void SHT21_Settings_Read(){
 uint8_t reader = ReadReg(SHT21_ADD, R_USER_REG);

 SHT21.Battery = (reader & (ON << BATTERY_ADD))>>BATTERY_ADD; //extracting settings
 SHT21.OTP_DISABLE = (reader & (ON << OTP_ADD))>>OTP_ADD;
 SHT21.Resolution = reader & RESOLUTION_ADD;
 SHT21.Heater = (reader & (ON << HEATER_ADD))>>HEATER_ADD;
}

void SHT21_Settings_Write(){
 WriteToReg(SHT21_ADD, W_USER_REG, SHT21.Resolution + (SHT21.Heater << HEATER_ADD) + (SHT21.OTP_DISABLE << OTP_ADD) + (SHT21.Battery << BATTERY_ADD));
}

uint32_t SHT21_Read(uint8_t mode){
 if((mode == HOLD_MASTER_T_MES) || (mode == HOLD_MASTER_RH_MES)){ //faster
  return (uint32_t)ReadMulti(SHT21_ADD, mode, 3);
 }
 else if((mode == NO_HOLD_MASTER_T_MES) || (mode == NO_HOLD_MASTER_RH_MES)){ //slower
  TranssmitAdd(SHT21_ADD, W);
  TranssmitByte(mode, NO_STOP);
  _delay_us(20);
  switch(SHT21.Resolution){
   case RH_11b_T_11b:
    if(mode == NO_HOLD_MASTER_T_MES)
     _delay_ms(11); 
	else
	 _delay_ms(15);
   break;
   case RH_10b_T_13b:
    if(mode == NO_HOLD_MASTER_T_MES)
     _delay_ms(43);
	else
	 _delay_ms(9);
   break;
   case RH_8b_T_12b:
    if(mode == NO_HOLD_MASTER_T_MES)
     _delay_ms(22);
	else
	 _delay_ms(4);
   break;
   default:
    if(mode == NO_HOLD_MASTER_T_MES)
     _delay_ms(85);
	else
	 _delay_ms(29);
   break;
  }
  TranssmitAdd(SHT21_ADD, R);
 uint32_t data = 0;
  for(uint8_t i = 0; i<3; i++){
   if(i == 2){
	data += ReadByteNACK();
	Stop();
	return data;
   }
   else{
	data += ReadByteACK();
	data <<= 8;
   }
  }  
 }
 return 0;
}

uint8_t Separator(uint32_t data, uint8_t ignoreCRC){ //ignoring of crc is faster but not so much reliable
 float result = 0.00;
 uint8_t msb = 0,
	    lsb = 0,
	    crc8M = 0;
 uint8_t	reversed_values[256] = {
	 0xff, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
	 0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
	 0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
	 0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
	 0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
	 0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
	 0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
	 0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
	 0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
	 0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
	 0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
	 0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
	 0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
	 0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
	 0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
	 0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
	 0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
	 0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
	 0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
	 0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
	 0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
	 0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
	 0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
	 0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
	 0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
	 0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
	 0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
	 0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
	 0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
	 0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
	 0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
	 0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0x00
  }; 
 if(ignoreCRC != 1){
  msb = (data >>16); //separated
  lsb = (data & 0xff00) >> 8; //separated
  crc8M = (data & 0xff); //separated
  msb = reversed_values[msb]; //change bits order
  lsb = reversed_values[lsb]; //change bits order
  crc8M = reversed_values[crc8M]; //change bits order
  if(CRC(((uint32_t)msb<<16)+ ((uint16_t)lsb<<8)+ crc8M ) & 1){ // if CRC functions bring back 1, that means CRC is incorrect
   SHT21.Fault = 1;
   return 1;
  }  
 }
 data >>=8;////if ignoring crc value or crc is correct just removing it
 if(data & 2){ //after all crc bullshit calculate RH %
  result = (float)(data-2)/65536;
  SHT21.RH = (result*125)-6 ;
 }
 else{ // and T Centigrade
  result = (float)data/65536;
  SHT21.T = (result*175.72)-46.85 ;
 }
 return 0;
}

void SHT21_Reset(uint8_t save){
 TranssmitAdd(SHT21_ADD, W);
 TranssmitByte(SOFT_RESET, STOP);
 SHT21_Settings_Read(); //Reading restored settings
 if(save & 1)
  SHT21_Settings_Write();//writing default settings
}

